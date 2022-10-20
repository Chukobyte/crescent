#include "rbe_network_socket.h"

#include <string.h>

#include "../thread/rbe_pthread.h"
#include "../utils/logger.h"
#include "../utils/rbe_assert.h"

//--- RBE Socket ---//
int rbe_socket_get_last_error() {
#ifdef _WIN32
    return WSAGetLastError();
#else
    return -1;
#endif
}

bool rbe_socket_system_initialize() {
#ifdef _WIN32
    // Initialise winsock
    WSADATA wsa;
    rbe_logger_debug("Initialising client Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        rbe_logger_error("Client: Failed. Error Code : %d", rbe_socket_get_last_error());
        return false;
    }
    rbe_logger_debug("Client: Initialized Winsock.");
#endif
    return true;
}

void rbe_socket_system_finalize() {
#ifdef _WIN32
    WSACleanup();
#endif
}

bool rbe_socket_send_message(RBESocket* sock, const char* message) {
#define RBE_SOCKET_SEND_BUFFER_SIZE 512
    static char socket_output_buffer[RBE_SOCKET_SEND_BUFFER_SIZE];
    strcpy(socket_output_buffer, message);
    if (sendto(sock->sock, socket_output_buffer, (int) strlen(socket_output_buffer), 0, (struct sockaddr*) &sock->si_other, sock->size) == SOCKET_ERROR) {
        rbe_logger_error("sendto() failed with error code : %d", rbe_socket_get_last_error());
        return false;
    }
    return true;
}

bool rbe_socket_receive_data(RBESocket* sock, char* buffer, int buffer_size) {
    if ((recvfrom(sock->sock, buffer, buffer_size, 0, (struct sockaddr*) &sock->si_other, &sock->size)) == SOCKET_ERROR) {
        rbe_logger_error("recvfrom() failed with error code : %d", rbe_socket_get_last_error());
        return false;
    }
    return true;
}

void rbe_socket_close(RBESocket* sock) {}

//--- Server Socker ---//
static pthread_t server_thread;

RBESocket rbe_socket_create_server(int port, on_rbe_socket_receive_data callback_func) {
    RBE_ASSERT_FMT(callback_func, "Callback func is NULL");
    RBESocket sock;
    sock.size = sizeof(sock.si_other);

    // Create a socket
    if ((sock.sock = socket(AF_INET, SOCK_DGRAM, 0 )) == INVALID_SOCKET) {
        RBE_ASSERT_FMT(false, "Server: could not create socket : %d", rbe_socket_get_last_error());
    }
    rbe_logger_debug("Server: socket created!");

    // Prepare the sockaddr_in structure
    sock.si.sin_family = AF_INET;
    sock.si.sin_addr.s_addr = INADDR_ANY;
    sock.si.sin_port = htons(port);

    // Bind
    if (bind(sock.sock, (struct sockaddr *) &sock.si, sizeof(sock.si)) == SOCKET_ERROR) {
        RBE_ASSERT_FMT(false, "Server: Bind failed with error code : %d", rbe_socket_get_last_error());
    }
    rbe_logger_debug("Server: Bind done!");

    // Start Networking Thread
    if (pthread_create(&server_thread, NULL, callback_func, NULL) != 0) {
        RBE_ASSERT_FMT(false, "Failed to create server thread!");
    }
    rbe_logger_debug("Server: Initialized thread.");
    return sock;
}

//--- Client Socket ---//
static pthread_t client_thread;

RBESocket rbe_socket_create_client(const char* serverAddr, int serverPort, on_rbe_socket_receive_data callback_func) {
    //create socket
    RBESocket sock;
    sock.size = sizeof(sock.si_other);
    rbe_logger_debug("Client: Creating socket....");
    if ((int) (sock.sock = (int) socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
        RBE_ASSERT_FMT(false, "Client: socket() failed with error code : %d", rbe_socket_get_last_error());
    }
    rbe_logger_debug("Client: socket created.");

    // setup address structure
    memset((char*) &sock.si_other, 0, sizeof(sock.si_other));
    sock.si_other.sin_family = AF_INET;
    sock.si_other.sin_port = htons(serverPort);
#ifdef _WIN32
    sock.si_other.sin_addr.S_un.S_addr = inet_addr(serverAddr);
#else
    if (inet_aton(serverAddr, &sock.si_other.sin_addr) == 0) {
        RBE_ASSERT_FMT(false, "inet_aton() failed!");
    }
#endif

    // Start Networking Thread
    if (pthread_create(&client_thread, NULL, callback_func, NULL) != 0) {
        RBE_ASSERT_FMT(false, "Failed to create server thread!");
    }

    rbe_logger_debug("Client: Initialized thread.");
    return sock;
}
