#include "lib/common.h"
#include "lib/tools.h"
#include <pthread.h>

#define PORT 8080
#define MAX_QUEUE 100
#define THREAD_POOL_SIZE 8

/* Job Queue */
int job_queue[MAX_QUEUE];
int front, rear, count = 0;

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;

void enqueue(int client_fd) {
    pthread_mutex_lock(&queue_mutex);
    while (count == MAX_QUEUE) {
        // queue is full, wait
        pthread_cond_wait(&queue_cond, &queue_mutex);
    }
    job_queue[rear] = client_fd;
    rear = (rear + 1) % MAX_QUEUE;
    count++;
    pthread_cond_signal(&queue_cond);
    pthread_mutex_unlock(&queue_mutex);
}

int dequeue() {
    pthread_mutex_lock(&queue_mutex);
    while (count == 0) {
        // queue is empty, wait
        pthread_cond_wait(&queue_cond, &queue_mutex);
    }
    int client_fd = job_queue[front];
    front = (front + 1) % MAX_QUEUE;
    count--;
    pthread_cond_signal(&queue_cond);
    pthread_mutex_unlock(&queue_mutex);
    return client_fd;
}

/* Worker Thread Function */
void *worker_thread(void *arg) {
    while (1) {
        int client_fd = dequeue();

        char buffer[4096] = {0};
        read(client_fd, buffer, sizeof(buffer) - 1);
        
        char method[8], path[256];
        sscanf(buffer, "%s %s", method, path);

        printf("Request: %s %s\n", method, path);

        if (strcmp(method, "GET") == 0) {
            serve_file(client_fd, path);
        } else {
            const char *not_allowed = "<h1>405 Method Not Allowed</h1>";
            send_response(client_fd, 405, "Not Allowed", "text/html", not_allowed, strlen(not_allowed));
        }

        close(client_fd);
    }
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

    int opt = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // Bind socket
    bind_socket(socket_fd, 8080);

    // Listen for connections
    listen(socket_fd, 10);

    printf("Server listening on port %d with %d worker threads...\n", PORT, THREAD_POOL_SIZE);

    pthread_t threads[THREAD_POOL_SIZE];
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        pthread_create(&threads[i], NULL, worker_thread, NULL);
        pthread_detach(threads[i]);
    }

    while (1) {
        int client_fd = accept(socket_fd, (struct sockaddr*)&address, &addr_len);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }
        enqueue(client_fd);
    }


    close(socket_fd);
    return 0;
}