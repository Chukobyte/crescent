#include "rbe_network.h"

#include<stdio.h>
#include<winsock2.h>

#include "../utils/logger.h"

//--- UDP SERVER ---//
#define SERVER_BUFFER_SIZE 512

static SOCKET server_socket;
static struct sockaddr_in server;
static struct sockaddr_in server_si_other;
static int server_socket_size = 0;
static int server_recv_len = 0;
static char server_buffer[SERVER_BUFFER_SIZE];

bool rbe_udp_server_initialize(int port) {
    rbe_logger_set_level(LogLevel_DEBUG);
    server_socket_size = sizeof(server_si_other);
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
        memset(server_buffer, '\0', SERVER_BUFFER_SIZE);

        //try to receive some data, this is a blocking call
        if ((server_recv_len = recvfrom(server_socket, server_buffer, SERVER_BUFFER_SIZE, 0, (struct sockaddr *) &server_si_other, &server_socket_size)) == SOCKET_ERROR) {
            rbe_logger_error("recvfrom() failed with error code : %d", WSAGetLastError());
            return false;
        }

        //print details of the client/peer and the data received
        rbe_logger_debug("Received packet from %s:%d", inet_ntoa(server_si_other.sin_addr), ntohs(server_si_other.sin_port));
        rbe_logger_debug("Data: %s", server_buffer);

        //now reply the client with the same data
        if (sendto(server_socket, server_buffer, server_recv_len, 0, (struct sockaddr*) &server_si_other, server_socket_size) == SOCKET_ERROR) {
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

//--- UDP CLIENT ---//
#define CLIENT_BUFFER_SIZE 512

static int client_socket;
static struct sockaddr_in client_si_other;
static int client_socket_size = 0;
static char client_buffer[CLIENT_BUFFER_SIZE];
static char client_message_buffer[CLIENT_BUFFER_SIZE];

bool rbe_udp_client_initialize(const char* serverAddr, int serverPort) {
    rbe_logger_set_level(LogLevel_DEBUG);
    client_socket_size = sizeof(client_si_other);
    WSADATA wsa;

    //Initialise winsock
    rbe_logger_debug("Initialising client Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        rbe_logger_error("Failed. Error Code : %d", WSAGetLastError());
        return false;
    }
    rbe_logger_debug("Initialized Winsock.");

    //create socket
    rbe_logger_debug("Creating client socket....");
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
        rbe_logger_error("socket() failed with error code : %d", WSAGetLastError());
        return false;
    }
    rbe_logger_debug("Client socket created.");

    //setup address structure
    memset((char*) &client_si_other, 0, sizeof(client_si_other));
    client_si_other.sin_family = AF_INET;
    client_si_other.sin_port = htons(serverPort);
    client_si_other.sin_addr.S_un.S_addr = inet_addr(serverAddr);

    return true;
}

bool rbe_udp_client_poll() {
    while (true) {
        printf("Enter message : ");
        fflush(stdout);
        gets(client_message_buffer);

        //send the message
        if (sendto(client_socket, client_message_buffer, strlen(client_message_buffer), 0, (struct sockaddr*) &client_si_other, client_socket_size) == SOCKET_ERROR) {
            rbe_logger_error("sendto() failed with error code : %d", WSAGetLastError());
            return false;
        }

        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(client_buffer,'\0', CLIENT_BUFFER_SIZE);
        //try to receive some data, this is a blocking call
        if (recvfrom(client_socket, client_buffer, CLIENT_BUFFER_SIZE, 0, (struct sockaddr *) &client_si_other, &client_socket_size) == SOCKET_ERROR) {
            rbe_logger_error("recvfrom() failed with error code : %d", WSAGetLastError());
            return false;
        }

        rbe_logger_debug("Server response = %s", client_buffer);
    }

    return true;
}

void rbe_udp_client_finalize() {
    closesocket(client_socket);
    WSACleanup();
}
