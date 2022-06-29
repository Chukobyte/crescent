#pragma once

#include <stdbool.h>

#ifdef _WIN32
#include <WinSock2.h>

typedef int rbe_socket_size;
typedef SOCKET rbe_socket;
#else
#include<arpa/inet.h>
#include<sys/socket.h>

typedef socklen_t rbe_socket_size;
typedef rbe_socket int;
typedef int rbe_socket;
#endif

//--- RBE Socket---//
typedef void* (*on_rbe_socket_receive_data) (void*);

typedef struct RBESocket {
    rbe_socket sock;
    rbe_socket_size size;
    struct sockaddr_in si;
    struct sockaddr_in si_other;
} RBESocket;

bool rbe_socket_system_initialize();
void rbe_socket_system_finalize();
bool rbe_socket_send_message(RBESocket* sock, const char* message);
bool rbe_socket_receive_data(RBESocket* sock, char* buffer, int buffer_size);
void rbe_socket_close(RBESocket* sock);

//--- Server Socket ---//
RBESocket rbe_socket_create_server(int port, on_rbe_socket_receive_data callback_func);

//--- Client Socket ---//
RBESocket rbe_socket_create_client(const char* serverAddr, int serverPort, on_rbe_socket_receive_data callback_func);
