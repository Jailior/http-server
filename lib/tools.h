const char *get_mime_type(const char *path);
void send_response(int client_fd, int status_code, const char *status_text, const char *content_type, const char *body, size_t body_len);
void serve_file(int client_fd, const char *path);
int bind_socket(int socket_fd, int port);