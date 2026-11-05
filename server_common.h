#pragma once
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "netproto.h"
#define MAX_CLIENTS 16
static int clients[MAX_CLIENTS];
static pthread_mutex_t clients_mtx = PTHREAD_MUTEX_INITIALIZER;
static ssize_t get_message_size(int fd) {
    uint32_t net;
    ssize_t r = read_all_from_socket(fd, (char *)&net, 4);
    if (r <= 0) return r;
    return (ssize_t)ntohl(net);
}
static ssize_t write_message_size(size_t size, int fd) {
    uint32_t net = htonl((uint32_t)size);
    return write_all_to_socket(fd, (char *)&net, 4);
}
static void broadcast(const char *buf, size_t size) {
    pthread_mutex_lock(&clients_mtx);
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i] != -1) {
            if (write_message_size(size, clients[i]) > 0)
                write_all_to_socket(clients[i], buf, size);
        }
    pthread_mutex_unlock(&clients_mtx);
}
static void add_client(int fd) {
    pthread_mutex_lock(&clients_mtx);
    for (int i = 0; i < MAX_CLIENTS; i++) if (clients[i] == -1) { clients[i] = fd; break; }
    pthread_mutex_unlock(&clients_mtx);
}
static void remove_client(int fd) {
    pthread_mutex_lock(&clients_mtx);
    for (int i = 0; i < MAX_CLIENTS; i++) if (clients[i] == fd) clients[i] = -1;
    pthread_mutex_unlock(&clients_mtx);
}
static int make_listener(const char *port) {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) { perror("socket"); exit(1); }
    int one = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; hints.ai_socktype = SOCK_STREAM; hints.ai_flags = AI_PASSIVE;
    int rc = getaddrinfo(NULL, port, &hints, &res);
    if (rc) { fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc)); exit(1); }
    if (bind(s, res->ai_addr, res->ai_addrlen) < 0) { perror("bind"); exit(1); }
    if (listen(s, MAX_CLIENTS) < 0) { perror("listen"); exit(1); }
    freeaddrinfo(res);
    return s;
}
static void run_accept_loop(const char *port, void *(*handler)(void *)) {
    for (int i = 0; i < MAX_CLIENTS; i++) clients[i] = -1;
    int s = make_listener(port);
    printf("listening on port %s\n", port);
    while (1) {
        int fd = accept(s, NULL, NULL);
        if (fd < 0) continue;
        add_client(fd);
        pthread_t t; pthread_create(&t, NULL, handler, (void *)(intptr_t)fd); pthread_detach(t);
    }
}
