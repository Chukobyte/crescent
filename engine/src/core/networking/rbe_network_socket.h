#pragma once

#include <stdbool.h>

#ifdef _WIN32
#include <WinSock2.h>

typedef size_t socklen_t;
typedef SOCKET rbe_socket;
#else
#include<arpa/inet.h>
#include<sys/socket.h>

typedef rbe_socket int;
typedef int rbe_socket;
#endif

bool rbe_socket_system_initialize();
void rbe_socket_system_finalize();

rbe_socket rbe_socket_create();
void rbe_socket_bind();
void rbe_socket_close();
