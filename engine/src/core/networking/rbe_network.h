#pragma once

#include <stdbool.h>

//--- UDP SERVER ---//
bool rbe_udp_server_initialize(int port);
bool rbe_udp_server_poll();
void rbe_udp_server_finalize();

//--- UDP CLIENT ---//
bool rbe_udp_client_initialize(const char* serverAddr, int serverPort);
bool rbe_udp_client_poll();
void rbe_udp_client_finalize();
