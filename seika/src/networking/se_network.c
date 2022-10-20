#include "se_network.h"

#include <stdio.h>
#include <string.h>

#include "se_network_socket.h"
#include "../utils/logger.h"
#include "../utils/se_assert.h"

#ifdef _MSC_VER
#pragma warning(disable : 4996) // for strcpy
#endif

//--- NETWORK ---//
#define RBE_NETWORK_HANDSHAKE_MESSAGE "init"

static bool se_is_server = false;

bool se_network_is_server() {
    return se_is_server;
}

//--- UDP SERVER ---//
#define SERVER_BUFFER_SIZE 512

void* se_udp_server_poll(void* arg);

static on_network_server_callback server_user_callback = NULL;
static on_network_server_client_connected_callback server_client_connected_callback = NULL;

static SESocket server_socket;

bool se_udp_server_initialize(int port, on_network_server_callback user_callback) {
    server_user_callback = user_callback;
    se_socket_system_initialize();
    server_socket = se_socket_create_server(port, se_udp_server_poll);
    se_is_server = true;

    return true;
}

void se_udp_server_register_client_connected_callback(on_network_server_client_connected_callback client_connected_callback) {
    server_client_connected_callback = client_connected_callback;
}

void* se_udp_server_poll(void* arg) {
    static char server_input_buffer[SERVER_BUFFER_SIZE];
    //keep listening for data
    while (true) {
        fflush(stdout);

        //clear the buffer by filling null, it might have previously received data
        memset(server_input_buffer, '\0', SERVER_BUFFER_SIZE);
        //try to receive some data, this is a blocking call
        if (!se_socket_receive_data(&server_socket, server_input_buffer, SERVER_BUFFER_SIZE)) {
            se_logger_error("Failed to receive socket data with server!");
//            return NULL;
            continue;
        }

        // Process data from client
        if (strcmp(server_input_buffer, RBE_NETWORK_HANDSHAKE_MESSAGE) == 0) {
            if (server_client_connected_callback != NULL) {
                server_client_connected_callback();
            }
            se_udp_server_send_message(RBE_NETWORK_HANDSHAKE_MESSAGE);
        } else {
            // Call user callback
            server_user_callback(server_input_buffer);
        }
    }
    return NULL;
}

bool se_udp_server_send_message(const char* message) {
    if (!se_socket_send_message(&server_socket, message)) {
        se_logger_error("Failed to send message '%s'", message);
        return false;
    }
    return true;
}

void se_udp_server_finalize() {
    se_socket_close(&server_socket);
    se_socket_system_finalize();
}

//--- UDP CLIENT ---//
#define CLIENT_BUFFER_SIZE 512

void* se_udp_client_poll(void*);

static SESocket client_socket;
static on_network_client_callback client_user_callback = NULL;

bool se_udp_client_initialize(const char* serverAddr, int serverPort, on_network_client_callback user_callback) {
    client_user_callback = user_callback;
    se_socket_system_initialize();
    client_socket = se_socket_create_client(serverAddr, serverPort, se_udp_client_poll);
    se_logger_debug("Client: Initialized thread.");

    return true;
}

void* se_udp_client_poll(void *unused) {
    static char client_input_buffer[CLIENT_BUFFER_SIZE];
    // TODO: Do handshake
    // TODO: Figure out why there is failure if no message is sent at first
    se_socket_send_message(&client_socket, RBE_NETWORK_HANDSHAKE_MESSAGE);

    while (true) {
        fflush(stdout);
        //clear the buffer by filling null, it might have previously received data
        memset(client_input_buffer, '\0', CLIENT_BUFFER_SIZE);
        //try to receive some data, this is a blocking call
        if (!se_socket_receive_data(&client_socket, client_input_buffer, CLIENT_BUFFER_SIZE)) {
            se_logger_error("Failed to receive socket data from client!");
//            RBE_ASSERT(false);
            continue;
        }

        if (strcmp(client_input_buffer, RBE_NETWORK_HANDSHAKE_MESSAGE) == 0) {
            if (server_client_connected_callback != NULL) {
                server_client_connected_callback();
            }
        } else {
            // Call user callback
            client_user_callback(client_input_buffer);
        }
    }

    return NULL;
}

bool se_udp_client_send_message(const char* message) {
    if (!se_socket_send_message(&client_socket, message)) {
        se_logger_error("Failed to send message '%s'", message);
        return false;
    }

    return true;
}

void se_udp_client_finalize() {
    se_socket_close(&client_socket);
    se_socket_system_finalize();
}
