#include "rbe_network.h"

#include<stdio.h>
#include<winsock2.h>

#include "../utils/logger.h"

#define SERVER_BUFFER_SIZE 512

static SOCKET server_socket;
static struct sockaddr_in server;
static struct sockaddr_in si_other;
static int slen = 0;
static int recv_len = 0;
static char buffer[SERVER_BUFFER_SIZE];

bool rbe_udp_server_initialize(int port) {
    slen = sizeof(si_other);
    WSADATA wsa;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        rbe_logger_error("Failed to initialize winsock! Error Code : %d", WSAGetLastError());
        return false;
    }
    rbe_logger_debug("Winsock initialized!");

    // Create a socket
    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0 )) == INVALID_SOCKET) {
        rbe_logger_debug("Could not create socket : %d", WSAGetLastError());
        return false;
    }
    rbe_logger_debug("Socket created!");

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    // Bind
    if (bind(server_socket, (struct sockaddr *) &server, sizeof(server)) == SOCKET_ERROR) {
        rbe_logger_error("Bind failed with error code : %d", WSAGetLastError());
        return false;
    }
    rbe_logger_debug("Bind done!");

    return true;
}

bool rbe_udp_server_poll() {
    //keep listening for data
    while (true) {
        rbe_logger_debug("Waiting for data...");
        fflush(stdout);

        //clear the buffer by filling null, it might have previously received data
        memset(buffer,'\0', SERVER_BUFFER_SIZE);

        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(server_socket, buffer, SERVER_BUFFER_SIZE, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR) {
            rbe_logger_error("recvfrom() failed with error code : %d", WSAGetLastError());
            exit(EXIT_FAILURE);
            return false;
        }

        //print details of the client/peer and the data received
        rbe_logger_debug("Received packet from %s:%d", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        rbe_logger_debug("Data: %s", buffer);

        //now reply the client with the same data
        if (sendto(server_socket, buffer, recv_len, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR) {
            rbe_logger_error("sendto() failed with error code : %d", WSAGetLastError());
            return false;
        }
    }
    return true;
}

void rbe_udp_server_finalize() {
    closesocket(server_socket);
    WSACleanup();
}
