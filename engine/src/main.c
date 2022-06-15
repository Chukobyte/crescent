#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "core/core.h"
#include "core/networking/rbe_network.h"
#include "core/utils/logger.h"

#define RBE_NETWORKING_PORT 8888

// Server
void server_callback(const char* message) {
    printf("Received from client: '%s'\n", message);
    rbe_udp_server_send_message(message);
}

void server_start() {
    rbe_udp_server_initialize(RBE_NETWORKING_PORT, server_callback);
    while (true) {}
    rbe_udp_server_finalize();
}

// Client
void client_callback(const char* message) {
    printf("Received from server: '%s'\n", message);
}

void client_start() {
    rbe_udp_client_initialize("127.0.0.1", RBE_NETWORKING_PORT, client_callback);
    char sendMessageBuffer[512];
    while (true) {
        memset(sendMessageBuffer,'\0', 512);
        printf("Enter message:\n");
        gets(sendMessageBuffer);
        rbe_udp_client_send_message(sendMessageBuffer);
    }
    rbe_udp_client_finalize();
}

// Main
int main(int argv, char** args) {
//    if (!rbe_initialize(argv, args)) {
//        return EXIT_FAILURE;
//    }
//
//    while (rbe_is_running()) {
//        rbe_update();
//    }
//
//    rbe_shutdown();
    rbe_logger_set_level(LogLevel_DEBUG);

    server_start();
//    client_start();

    return EXIT_SUCCESS;
}
