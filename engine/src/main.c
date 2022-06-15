#include <stdlib.h>
#include <stdio.h>
//#include "core/core.h"

#include "core/networking/rbe_network.h"
#include "core/utils/rbe_assert.h"

#define RBE_TEST_PORT 8888

void server_callback(const char* message) {
    printf("server_callback - message: '%s'\n", message);
}

void run_server() {
    RBE_ASSERT(rbe_udp_server_initialize(RBE_TEST_PORT, server_callback));
    rbe_udp_server_poll();
    rbe_udp_server_finalize();
}

void run_client() {
    RBE_ASSERT(rbe_udp_client_initialize("127.0.0.1", RBE_TEST_PORT));
    rbe_udp_client_poll();
    rbe_udp_client_finalize();
}

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

//    rbe_logger_set_level(LogLevel_DEBUG);
    run_server();
//    run_client();

    return EXIT_SUCCESS;
}
