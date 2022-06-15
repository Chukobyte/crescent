#include "rbe_network.h"

#include <stdio.h>
#include <winsock2.h>

#include "../thread/rbe_pthread.h"
#include "../utils/logger.h"
#include "../utils/rbe_assert.h"

//--- UDP SERVER ---//
#define SERVER_BUFFER_SIZE 512

static SOCKET server_socket;
static struct sockaddr_in server;
static struct sockaddr_in server_si_other;
static int server_socket_size = 0;
static int server_recv_len = 0;
static on_network_server_callback server_user_callback = NULL;

bool rbe_udp_server_initialize(int port, on_network_server_callback user_callback) {
    server_socket_size = sizeof(server_si_other);
    server_user_callback = user_callback;
    WSADATA wsa;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        rbe_logger_error("Server: Failed to initialize winsock! Error Code : %d", WSAGetLastError());
        return false;
    }
    rbe_logger_debug("Server: winsock initialized!");

    // Create a socket
    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0 )) == INVALID_SOCKET) {
        rbe_logger_debug("Server: could not create socket : %d", WSAGetLastError());
        return false;
    }
    rbe_logger_debug("Server: socket created!");

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    // Bind
    if (bind(server_socket, (struct sockaddr *) &server, sizeof(server)) == SOCKET_ERROR) {
        rbe_logger_error("Server: Bind failed with error code : %d", WSAGetLastError());
        return false;
    }
    rbe_logger_debug("Server: Bind done!");

    // Start Networking Thread
    pthread_t thread;
    pthread_create(&thread, NULL, rbe_udp_server_poll, NULL);
    if (pthread_create(&thread, NULL, rbe_udp_server_poll, NULL) != 0) {
        rbe_logger_debug("Failed to create server thread!");
        return false;
    }
    rbe_logger_debug("Server: Initialized thread.");

    return true;
}

void* rbe_udp_server_poll(void* arg) {
    static char server_input_buffer[SERVER_BUFFER_SIZE];
    //keep listening for data
    while (true) {
        fflush(stdout);

        //clear the buffer by filling null, it might have previously received data
        memset(server_input_buffer, '\0', SERVER_BUFFER_SIZE);
        //try to receive some data, this is a blocking call
        if ((server_recv_len = recvfrom(server_socket, server_input_buffer, SERVER_BUFFER_SIZE, 0, (struct sockaddr *) &server_si_other, &server_socket_size)) == SOCKET_ERROR) {
            rbe_logger_error("Server: recvfrom() failed with error code : %d", WSAGetLastError());
            return NULL;
        }
        // Call user callback
        server_user_callback(server_input_buffer);

        //print details of the client/peer and the data received
//        rbe_logger_debug("Received packet from %s:%d", inet_ntoa(server_si_other.sin_addr), ntohs(server_si_other.sin_port));
//        rbe_logger_debug("Data: %s", server_input_buffer);

    }
    return NULL;
}

bool rbe_udp_server_send_message(const char* message) {
    static char server_output_buffer[SERVER_BUFFER_SIZE];
    strcpy(server_output_buffer, message);
    if (sendto(server_socket, server_output_buffer, server_recv_len, 0, (struct sockaddr*) &server_si_other, server_socket_size) == SOCKET_ERROR) {
        rbe_logger_error("Server: sendto() failed with error code : %d", WSAGetLastError());
        return false;
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
static on_network_client_callback client_user_callback = NULL;

bool rbe_udp_client_initialize(const char* serverAddr, int serverPort, on_network_client_callback userCallback) {
    client_socket_size = sizeof(client_si_other);
    client_user_callback = userCallback;
    WSADATA wsa;

    // Initialise winsock
    rbe_logger_debug("Initialising client Winsock...");
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        rbe_logger_error("Client: Failed. Error Code : %d", WSAGetLastError());
        return false;
    }
    rbe_logger_debug("Client: Initialized Winsock.");

    //create socket
    rbe_logger_debug("Client: Creating socket....");
    if ((client_socket = (int) socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
        rbe_logger_error("Client: socket() failed with error code : %d", WSAGetLastError());
        return false;
    }
    rbe_logger_debug("Client: socket created.");

    // setup address structure
    memset((char*) &client_si_other, 0, sizeof(client_si_other));
    client_si_other.sin_family = AF_INET;
    client_si_other.sin_port = htons(serverPort);
    client_si_other.sin_addr.S_un.S_addr = inet_addr(serverAddr);

    // Start Networking Thread
    pthread_t thread;
    if (pthread_create(&thread, NULL, rbe_udp_client_poll, NULL) != 0) {
        rbe_logger_debug("Failed to create server thread!");
        return false;
    }

    rbe_logger_debug("Client: Initialized thread.");

    return true;
}

void* rbe_udp_client_poll() {
    static char client_input_buffer[CLIENT_BUFFER_SIZE];
    // TODO: Do handshake
    rbe_udp_client_send_message("Init message from client!");

    while (true) {
        fflush(stdout);
        //clear the buffer by filling null, it might have previously received data
        memset(client_input_buffer, '\0', CLIENT_BUFFER_SIZE);
        //try to receive some data, this is a blocking call

        if (recvfrom(client_socket, client_input_buffer, CLIENT_BUFFER_SIZE, 0, (struct sockaddr *) &client_si_other, &client_socket_size) == SOCKET_ERROR) {
            rbe_logger_error("Client: recvfrom() failed with error code : %d", WSAGetLastError());
            RBE_ASSERT(false);
        }
        // Call user callback
        client_user_callback(client_input_buffer);
    }

    return NULL;
}

bool rbe_udp_client_send_message(const char* message) {
    static char client_output_buffer[SERVER_BUFFER_SIZE];
    strcpy(client_output_buffer, message);
    if (sendto(client_socket, client_output_buffer, (int) strlen(client_output_buffer), 0, (struct sockaddr*) &client_si_other, client_socket_size) == SOCKET_ERROR) {
        rbe_logger_error("sendto() failed with error code : %d", WSAGetLastError());
        return false;
    }
    return true;
}

void rbe_udp_client_finalize() {
    closesocket(client_socket);
    WSACleanup();
}

// Networking Testing Stuff

//#define RBE_NETWORKING_PORT 8888
//
//// Server
//void server_callback(const char* message) {
//    printf("Received from client: '%s'\n", message);
//    rbe_udp_server_send_message(message);
//}
//
//void server_start() {
//    rbe_udp_server_initialize(RBE_NETWORKING_PORT, server_callback);
//    while (true) {}
//    rbe_udp_server_finalize();
//}
//
//// Client
//void client_callback(const char* message) {
//    printf("Received from server: '%s'\n", message);
//}
//
//void client_start() {
//    rbe_udp_client_initialize("127.0.0.1", RBE_NETWORKING_PORT, client_callback);
//    char sendMessageBuffer[512];
//    while (true) {
//        memset(sendMessageBuffer,'\0', 512);
//        printf("Enter message:\n");
//        gets(sendMessageBuffer);
//        rbe_udp_client_send_message(sendMessageBuffer);
//    }
//    rbe_udp_client_finalize();
//}