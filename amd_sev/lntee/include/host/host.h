#include <eEVM/simple/simpleglobalstate.h>

#include <socket/config.h>
#include <fstream>
#include <stdio.h>
#include <thread>
#include <eEVM/common.h>
#include <unistd.h>
#include <thread> // std::thread
#include <iostream>
#include "mbedtls/sha256.h"

using namespace std;

#ifndef TEST_H
#define TEST_H

//static int _node_id = 0;
void load_contract();

void start_host_server(int node_id);

void start_host_client(int node_id);

/**
 * Connect to the SEV enclave
 */
void start_sev_enclave(const char *enclave_ip);

void test_send_tx(int test_scale);

void resv_call_back(const unsigned char *tx, size_t len);

void sev_call_back(const unsigned char *tx, size_t len);

//void process_sev_message(const unsigned char *msg);

void contract_menu();

/** Broad cast message to the group */
void broad_cast(const unsigned char *msg);

/** Send message to specific target */
void send_to();

#endif