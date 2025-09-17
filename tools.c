#include "lib/common.h"
#include "lib/tools.h"

// Bind a socket to a specified port
int bind_socket(int socket_fd, int port) {
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // any interface
    address.sin_port = htons(port);
    return bind(socket_fd, (struct sockaddr*)&address, sizeof(address));
}