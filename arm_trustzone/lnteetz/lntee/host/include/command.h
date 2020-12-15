#include "lntee_u.h"
#include <fstream> // Read the contract from the file
#include <iostream>
#include <openenclave/host.h>
#include <sys/time.h>
#include "Global.h"
#include "payload.pb.h"
#include <nlohmann/json.hpp>
#include "libf2f/router.h"
#include <eEVM/common.h>
#include "libf2f/message.h"
#include "lntee_messages.h"

#ifndef LNTEE_COMMAND_H
#define LNTEE_COMMAND_H

// #include "peer_manager.h"

namespace lntee {
    class Command {
    private:
        libf2f::Router *router;

        std::string random_string(size_t length);

    public:
        static long int ms;

        /**
         * Constructor
         */
        Command(libf2f::Router *r) : router(r) {
            Command::ms = 0;
        }

        libf2f::Router * getRouter() { return router; }
        /**
         * Connect to a new node
         */
        void connect(std::string peer_info);

        /**
         * Load the enclave, in the mean time an account will be created.
         *
         * @param the root path of the contract file
         * @param enc: path to the enclave file
         * @return int: enclave load result
         */
        int load_enclave(const char* con_path, const char *enc);

        /**
         * Terminate the enclave in case of error
         * @return
         */
        void terminate_enclave();

        /**
         * Load contract from the file
         * @param the root path of the contract file
         * @param contract_name path to the contract file
         */
        void load_contract(std::string con_path, std::string contract_name);

        /**
         * Generate a share key to process message exchanging
         * This shared key should be attached with the instance id
         * 1. Generate an instance id
         * 2. Generate an share key
         */
        void generate_share_key();

        /**
        * Decrypt the ecdh share message from the counter part
        * @param pubkey ecdh pubkey of counterpart
        * @param msg secret msg from the peer
        * @param len length of the secret message
        * @param ecdh_secret get the local ecdh secret key
        */
        void tee_add_peer(const char *bin_pubkey, char *bin_msg, char *bin_ecdh_secret);

        /**
         * Get initial state from the blockchain
         * The local initial state can be updated only if the current state has been
         * syncronized
         *
         * @param tx_list:  the list of txid that deposit into the TEE account on
         * the blockchain
         */
        void load_initial_state();

        /**
         * 1. Get current initial state, balance, and the contract address
         * 2. Sign (State || balance || contract address)
         * 3. Send it to the counterpart
         */
        void verify_initial_state(oe_enclave_t *enclave, const char *msg);

        /**
         * Message that comes from the host
         * send the msg to the enclave to create a transaction
         */
        void send_msg_to_enclave(std::string msg);

        /**
         * This one should be called when a transaction is created in enclave
         */
        void broad_cast_tx(const char *tx);

        /**
         * Send contract transaction
         * @param tx transactions in hex format
         */
        void send_contract_tx(std::string target,const char *tx, int len);

        /**
          * Send contract transaction
          * @param tx transactions in hex format
          */
        void recv_contract_tx(const char *tx);


        /**
         * This function handles transactions receive from counterpart
         *
         * @param tx the content of the transaction, should be in hex form
         */
        void process_tx(const char *tx);

        /**
         * Directly send credit to target
         * @param target  target public key in hex format
         * @param amt amount to send
         */
        void direct_send(std::string target, int amt);

        /**
         * Receive direct transaction
         * @param tx direct transaction script
         */
        void direct_recv(const char *tx_hex);

        /***
         * Benchmark the time consumption
         * @param tag tag of the log
         */
        static void time_log(const char *tag);

        static void time_curr(const char *tag);


        /**
         * Call back function of the command class to handle
         * messages come from the peer
         * @param tx hex version of tx string
         */
        std::string peer_in_come(nlohmann::json j);

    };
};

#endif