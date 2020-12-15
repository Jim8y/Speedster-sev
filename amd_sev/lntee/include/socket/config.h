#include <arpa/inet.h>
#include <cerrno>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <eEVM/common.h>
#include <string>

#ifndef SOCKET_CONFIG_H
#define SOCKET_CONFIG_H

const int MAX_LINE = 2048;
const int PORT_BASE = 6000;
const int PORT_0 = PORT_BASE;
const int PORT_1 = PORT_BASE + 1;
const int PORT_2 = PORT_BASE + 2;
const int PORT_SEV = 20000;

#define SEV_SOCK_TOKEN "0x000000000000000000000000000000000000000000000000"

#define _CONTRACT_RPC_

//#define _TEST_ECDSA_



#ifdef _CONTRACT_ERC20_
#define _CONTRACT_ "ERC20"
#endif

#ifdef _CONTRACT_GOMOKU_
#define _CONTRACT_ "gomoku"
#endif

#ifdef _CONTRACT_MONOPOLY_
#define _CONTRACT_ "monopoly"
#endif

#ifdef _CONTRACT_RPC_
#define _CONTRACT_ "rpc"
#endif


const int BACKLOG = 10;
const int LISTENQ = 6666;
const int MAX_CONNECT = 20;

#define IP_ADDR "127.0.0.1"
#define IP_SEV_0 "172.30.18.242"
#define IP_SEV_1 "172.16.175.134"
#define IP_SEV_2 "172.16.175.135"

#ifdef __cplusplus

typedef void (*callback_ptr)(const unsigned char *, size_t, void *);

typedef void (*cmd_call_back_ptr)(const unsigned char *, void *);

//enum COMMAND {
//    LOAD_CONTRACT = 1,
//    RECV_TRANSACTION,
//    SEND_TRANSACTION,
//    CONTRACT_MSG,
//    HOST_REQUEST_SEV_ACCOUNT
//};

typedef struct _thread_args {
    int connfd;
    char *ip;
    int port;
    void *ptr_class; // call back class
    callback_ptr ptr;
} thread_args;

extern "C"
{
void *recv_message(void *fd);
}
#endif

#endif
