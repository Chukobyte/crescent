#include "rbe_network_socket.h"

#include "../thread/rbe_pthread.h"
#include "../utils/logger.h"
#include "../utils/rbe_assert.h"

bool rbe_socket_system_initialize() {
#ifdef _WIN32
    // Initialise winsock
    WSADATA wsa;
    rbe_logger_debug("Initialising client Winsock...");
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        rbe_logger_error("Client: Failed. Error Code : %d", WSAGetLastError());
        return false;
    }
    rbe_logger_debug("Client: Initialized Winsock.");
#endif
    return true;
}

rbe_socket rbe_socket_create_server(int port) {
    rbe_socket sock;
    // Create a socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0 )) == INVALID_SOCKET) {
        rbe_logger_debug("Server: could not create socket : %d", WSAGetLastError());
        return false;
    }
    rbe_logger_debug("Server: socket created!");

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    // Bind
    if (bind(sock, (struct sockaddr *) &server, sizeof(server)) == SOCKET_ERROR) {
        RBE_ASSERT_FMT(false, "Server: Bind failed with error code : %d", WSAGetLastError());
    }
    rbe_logger_debug("Server: Bind done!");

    // Start Networking Thread
    pthread_t thread;
    if (pthread_create(&thread, NULL, rbe_udp_server_poll, NULL) != 0) {
        RBE_ASSERT_FMT(false, "Failed to create server thread!");
    }
    rbe_logger_debug("Server: Initialized thread.");
}

rbe_socket rbe_socket_create_client(const char* serverAddr, int serverPort) {
    //create socket
    rbe_socket sock;
    rbe_logger_debug("Client: Creating socket....");
    if ((sock = (int) socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
        RBE_ASSERT_FMT(false, "Client: socket() failed with error code : %d", WSAGetLastError());
    }
    rbe_logger_debug("Client: socket created.");

    // setup address structure
    memset((char*) &client_si_other, 0, sizeof(client_si_other));
    client_si_other.sin_family = AF_INET;
    client_si_other.sin_port = htons(serverPort);
#ifdef _WIN32
    client_si_other.sin_addr.S_un.S_addr = inet_addr(serverAddr);
#else
    if (inet_aton(serverAddr, &client_si_other.sin_addr) == 0) {
        rbe_logger_error("inet_aton() failed!");
        return false;
    }
#endif

    // Start Networking Thread
    pthread_t thread;
    if (pthread_create(&thread, NULL, rbe_udp_client_poll, NULL) != 0) {
        rbe_logger_debug("Failed to create server thread!");
        return false;
    }

    rbe_logger_debug("Client: Initialized thread.");
}

rbe_socket rbe_socket_create() {
}
