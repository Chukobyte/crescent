#include <stdlib.h>
//#include "core/core.h"

#include "core/networking/rbe_network.h"
#include "core/utils/rbe_assert.h"

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

    RBE_ASSERT(rbe_udp_server_initialize(8888));
    rbe_udp_server_poll();
    rbe_udp_server_finalize();

    return EXIT_SUCCESS;
}
