#include "lib/common.h"
#include "lib/tools.h"

    // Creates TCP socket
    // An HTTP server is basically a TCP socket that listens on port 80
    // and responds in HTTP format

int main() {

    int socket_fd, client_fd;
    struct sockaddr_in address;
    socklen_t addr_len = sizeof(address);
    char buffer[4096] = {0};

    // Create socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    // Bind socket
    bind_socket(socket_fd, 8080);

    // Listen for connections
    listen(socket_fd, 10);

    printf("Listening on port 8080...\n");

    while (1) {
        // Accept a connection
        client_fd = accept(socket_fd, (struct sockaddr*)&address, &addr_len);

        // Read request
        read(client_fd, buffer, sizeof(buffer));
        printf("Received request:\n%s\n", buffer);

        // Send a response
        const char *http_response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 20\r\n"
            "\r\n"
            "<h1>Hello world!</h1>";
        
        send(client_fd, http_response, strlen(http_response), 0);

        // Close the connection
        close(client_fd);
    }

    return 0;
}