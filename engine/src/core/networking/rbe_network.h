#pragma once

#include <stdbool.h>

//--- UDP SERVER ---//
bool rbe_udp_server_initialize(int port);
bool rbe_udp_server_poll();
void rbe_udp_server_finalize();
