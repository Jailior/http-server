#include "lib/common.h"
#include "lib/tools.h"


const char *get_mime_type(const char *path) {
    if (strstr(path, ".html")) return "text/html";
    if (strstr(path, ".css"))  return "text/css";
    if (strstr(path, ".js"))   return "application/javascript";
    if (strstr(path, ".png"))  return "image/png";
    if (strstr(path, ".jpg"))  return "image/jpeg";
    if (strstr(path, ".gif"))  return "image/gif";
    return "text/plain";
}

void send_response(int client_fd, int status_code, const char *status_text, const char *content_type, const char *body, size_t body_len) {
    char header[512];
    int header_len = snprintf(header, sizeof(header), 
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "\r\n", 
        status_code, status_text, content_type, body_len);

    send(client_fd, header, header_len, 0);
    if (body_len > 0) {
        send(client_fd, body, body_len, 0);
    }
}

// Bind a socket to a specified port
int bind_socket(int socket_fd, int port) {
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // any interface
    address.sin_port = htons(port);
    return bind(socket_fd, (struct sockaddr*)&address, sizeof(address));
}

/* File Handling */

void serve_file(int client_fd, const char *path) {
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s%s", WEB_ROOT, path);

    // Default to index.html
    if (strcmp(path, "/") == 0) {
        snprintf(full_path, sizeof(full_path), "%s/index.html", WEB_ROOT);
    }

    int fd = open(full_path, O_RDONLY);
    if (fd < 0) {
        const char *not_found_body = "<h1>404 Not Found</h1>";
        send_response(client_fd, 404, "Not Found", "text/html", not_found_body, strlen(not_found_body));
        return;
    }

    struct stat st;
    fstat(fd, &st);
    size_t file_size = st.st_size;

    char *file_buffer = malloc(file_size);
    if (!file_buffer) {
        close(fd);
        return;
    }

    read(fd, file_buffer, file_size);
    close(fd);

    const char *mime_type = get_mime_type(full_path);
    send_response(client_fd, 200, "OK", mime_type, file_buffer, file_size);

    free(file_buffer);
}
