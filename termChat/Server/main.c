#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define SER_IP "127.0.0.1"
#define SER_PORT 4444
#define BACKLOG 10  // Added the missing definition

volatile sig_atomic_t keep_running = 1;

typedef struct {
    int socket;
    struct sockaddr_in address;
    int uid;
    char name[32];
} client_t;

client_t *clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
static int uid_counter = 10;
int online_count = 0;

void handle_sigint(int sig) { keep_running = 0; }

void add_client(client_t *_client) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (!clients[i]) {
            clients[i] = _client;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void remove_client(int uid) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] && clients[i]->uid == uid) {
            clients[i] = NULL;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

// Sends a message to everyone (Echo logic)
void send_message(char *s) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i]) {
            send(clients[i]->socket, s, strlen(s), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

// Sends the hidden protocol tag to update client headers
void broadcast_count() {
    char count_msg[32];
    snprintf(count_msg, sizeof(count_msg), "|COUNT|%d", online_count);
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i]) {
            send(clients[i]->socket, count_msg, strlen(count_msg), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void* handle_client(void* arg) {
    client_t *cli = (client_t*)arg;
    char buffer[1024];
    char name_buffer[32];
    int has_joined = 0;

    // 1. Handshake
    send(cli->socket, "Enter Username: ", 16, 0);
    int name_len = recv(cli->socket, name_buffer, sizeof(name_buffer) - 1, 0);

    if (name_len <= 0) goto cleanup;

    name_buffer[name_len] = '\0';
    strtok(name_buffer, "\r\n");
    strncpy(cli->name, name_buffer, 31);

    pthread_mutex_lock(&clients_mutex);
    online_count++;
    has_joined = 1;
    pthread_mutex_unlock(&clients_mutex);

    // Update headers for everyone
    broadcast_count();

    // Broadcast Join Message
    char join_msg[128];
    snprintf(join_msg, sizeof(join_msg), " *** %s joined the chat group ***\n", cli->name);
    send_message(join_msg);

    // 2. Chat Loop
    while (1) {
        int read = recv(cli->socket, buffer, sizeof(buffer) - 1, 0);
        if (read > 0) {
            buffer[read] = '\0';
            strtok(buffer, "\r\n");
            if (strlen(buffer) == 0) continue;

            char formatted[1100];
            snprintf(formatted, sizeof(formatted), "%s: %s\n", cli->name, buffer);
            send_message(formatted);
        } else break;
    }

cleanup:
    if (has_joined) {
        pthread_mutex_lock(&clients_mutex);
        online_count--;
        pthread_mutex_unlock(&clients_mutex);
        broadcast_count(); // Update headers on leave

        char leave_msg[128];
        snprintf(leave_msg, sizeof(leave_msg), " *** %s left the chat ***\n", cli->name);
        send_message(leave_msg);
    }
    close(cli->socket);
    remove_client(cli->uid);
    free(cli);
    return NULL;
}

int main() {
    signal(SIGINT, handle_sigint);
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("Socket failed"); exit(1); }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SER_PORT);
    inet_pton(AF_INET, SER_IP, &addr.sin_addr);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Bind failed"); exit(1);
    }

    if (listen(server_fd, BACKLOG) < 0) {
        perror("Listen failed"); exit(1);
    }

    printf("Server Active on %s:%d\n", SER_IP, SER_PORT);

    while (keep_running) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int cli_fd = accept(server_fd, (struct sockaddr*)&client_addr, &len);

        if (cli_fd < 0) {
            if (keep_running) perror("Accept failed");
            break;
        }

        client_t *cli = malloc(sizeof(client_t));
        cli->socket = cli_fd;
        cli->address = client_addr; // Store the address correctly
        cli->uid = uid_counter++;
        cli->name[0] = '\0';

        add_client(cli);

        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, cli) != 0) {
            perror("Thread creation failed");
            free(cli);
            close(cli_fd);
        } else {
            pthread_detach(tid);
        }
    }

    close(server_fd);
    printf("Server shut down.\n");
    return 0;
}