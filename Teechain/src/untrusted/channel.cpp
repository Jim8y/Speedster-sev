// #include "sgx_uae_service.h"
// #include "sgx_uae_launch.h"
// #include "sgx_uae_launch.h"
// #include  "sgx_uae_epid.h"
//  #include  "sgx_uae_quote_ex.h"
#include "teechain_u.h"

#include "channel.h"
#include "network.h"
#include "service_provider.h"
#include "teechain.h"
#include "utils.h"

extern sgx_enclave_id_t global_eid;

std::map<std::string, ChannelState*> channelStates; // map channel ID to state

void init_channel_connection(ChannelConnection* connection) {
	memcpy((char *) connection->id, "", 1);
	memcpy((char *) connection->remote_host, "", 1);
	connection->remote_host_len = 0;
	connection->remote_port = -1;
	connection->remote_sockfd = -1;
}

void init_channel_state(ChannelState* state) {
	init_channel_connection(&(state->connection));
	state->context = -1;
	state->enclave_lost_retry_time = 1;
	state->busy_retry_time = 4;
	state->status = SGX_SUCCESS;
	state->is_initiator = false;
	state->is_backup_channel = false;
}

ChannelState* create_channel_state() {
	//log_debug("create_channel_state() (untrusted)");
	ChannelState *state = (ChannelState*) malloc(sizeof(ChannelState));
	init_channel_state(state);
	return state;
}

void initialise_channel_state(ChannelState* state, bool initiator) {
	state->is_initiator = initiator;
}

void associate_channel_state(std::string channel_id, ChannelState* state_to_associate) {
	memcpy((char*) state_to_associate->connection.id, channel_id.c_str(), CHANNEL_ID_LEN);
	channelStates.insert(std::pair<std::string, ChannelState*>(channel_id, state_to_associate));
}

void remove_association(std::string channel_id) {
        channelStates.erase(channel_id);
}

ChannelState* get_channel_state(std::string channel_id) {
    std::map<std::string, ChannelState*>::iterator it = channelStates.find(channel_id);
    if (it == channelStates.end()) {
        printf("Untrusted get_channel_state() could not find channel state for given channel_id");
        printf("%s", channel_id.c_str());
        printf("Printing contents of channel states!");
        for (std::map<std::string, ChannelState*>::const_iterator it = channelStates.begin(); it != channelStates.end(); it++) {
            print(it->first.c_str());
        }
        error("Failed to get state!! Terminating...");
    }
    return it->second;
}


void destroy_channel_state(std::string channel_id) {
	ChannelState *found;

	std::map<std::string, ChannelState*>::iterator it = channelStates.find(channel_id);
	if (it != channelStates.end()) {
		found = it->second;
	}

	if (found != NULL) {
		channelStates.erase(channel_id);
		free(found);
	}
}

bool exists_state(std::string channel_id) {
	return channelStates.find(channel_id) != channelStates.end();
}

std::vector<std::string> get_all_channel_ids() {
	std::vector<std::string> channel_ids;
	std::map<std::string, ChannelState*>::iterator it;

	for (it = channelStates.begin(); it != channelStates.end(); it++) {
		channel_ids.push_back(it->first);
	}

	return channel_ids;
}

// clean up enclave channel_state
void cleanup() {
    std::vector<std::string> channel_ids = get_all_channel_ids();
    std::vector<std::string>::iterator it;
    for (it = channel_ids.begin(); it != channel_ids.end(); it++) {
        ChannelState *channel_state = get_channel_state(*it);
        if (channel_state->context != -1) {
            int ret;
            // ecall_enclave_ra_close(global_eid, (sgx_status_t*) &ret, channel_state->context); // ignore return value
        }
    }

    // destroy SGX enclave channel_state
    sgx_destroy_enclave(global_eid);
}