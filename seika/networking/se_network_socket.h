#pragma once

#include <stdbool.h>

#ifdef _WIN32
#include <WinSock2.h>

typedef int se_socket_size;
typedef SOCKET se_socket;
#else
#include<arpa/inet.h>
#include<sys/socket.h>

#define SOCKET_ERROR (-1)
#define INVALID_SOCKET (-1)

typedef socklen_t se_socket_size;
typedef int se_socket;
#endif

//--- RBE Socket---//
typedef void* (*on_se_socket_receive_data) (void*);

typedef struct SESocket {
    se_socket sock;
    se_socket_size size;
    struct sockaddr_in si;
    struct sockaddr_in si_other;
} SESocket;

bool se_socket_system_initialize();
void se_socket_system_finalize();
bool se_socket_send_message(SESocket* sock, const char* message);
bool se_socket_receive_data(SESocket* sock, char* buffer, int buffer_size);
void se_socket_close(SESocket* sock);

//--- Server Socket ---//
SESocket se_socket_create_server(int port, on_se_socket_receive_data callback_func);

//--- Client Socket ---//
SESocket se_socket_create_client(const char* serverAddr, int serverPort, on_se_socket_receive_data callback_func);
