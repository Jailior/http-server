#include "lib/common.h"
#include "lib/tools.h"
#include <pthread.h>

void *handle_client(void *arg) {
    int client_fd = *(int *) arg;
    free(arg);

    char buffer[4096] = {0};

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
    return NULL;
}

// Creates TCP socket
// An HTTP server is basically a TCP socket that listens on port 80
// and responds in HTTP format

int main() {

    int socket_fd;
    struct sockaddr_in address;
    socklen_t addr_len = sizeof(address);
    

    // Create socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    // Bind socket
    bind_socket(socket_fd, 8080);

    // Listen for connections
    listen(socket_fd, 10);

    printf("Listening on port 8080...\n");

    while (1) {
        // Accept a connection
        int *client_fd = malloc(sizeof(int));
        
        *client_fd = accept(socket_fd, (struct sockaddr*)&address, &addr_len);

        pthread_t thread_id;

        if (pthread_create(&thread_id, NULL, handle_client, client_fd) != 0) {
            perror("Failed to create thread");
            close(*client_fd);
            free(client_fd);
        } else {
            pthread_detach(thread_id);
        }
    }

    close(socket_fd);
    return 0;
}