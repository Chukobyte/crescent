#pragma once

#include <stdbool.h>

//--- UDP SERVER ---//
typedef void (*on_network_server_callback) (const char*);

bool rbe_udp_server_initialize(int port, on_network_server_callback user_callback);
void* rbe_udp_server_poll(void* arg);
bool rbe_udp_server_send_message(const char* message);
void rbe_udp_server_finalize();

//--- UDP CLIENT ---//
typedef void (*on_network_client_callback) (const char*);

bool rbe_udp_client_initialize(const char* serverAddr, int serverPort, on_network_client_callback);
void* rbe_udp_client_poll();
bool rbe_udp_client_send_message(const char* message);
void rbe_udp_client_finalize();
