// CS 341 Week 11 lab - chatroom client (STUDENT skeleton).
// Fill in the 5 TODOs so the client connects and chats. Same binary protocol
// and function names as the real assignment.  usage: ./client <host> <port> <username>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "netproto.h"

static int serverSocket;

int connect_to_server(const char *host, const char *port) {
    // TODO 1: create a TCP socket (AF_INET, SOCK_STREAM, 0). Replace the -1.
    int sockfd = -1;
    if (sockfd < 0) { fprintf(stderr, "connect_to_server: socket not created (finish TODO 1)\n"); exit(1); }

    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    // TODO 2: call getaddrinfo(host, port, &hints, &result); store the return in err.
    int err = -1;
    if (err != 0) { fprintf(stderr, "getaddrinfo (finish TODO 2): %s\n", gai_strerror(err)); exit(1); }

    // TODO 3: connect sockfd to result->ai_addr (length result->ai_addrlen);
    //         exit(1) with perror("connect") on failure.

    freeaddrinfo(result);
    return sockfd;
}

ssize_t write_message_size(size_t size, int fd) {
    // TODO 4: convert size to uint32_t in NETWORK byte order, send exactly 4 bytes.
    (void)size; (void)fd;
    return -1;
}

ssize_t get_message_size(int fd) {
    uint32_t net;
    ssize_t r = read_all_from_socket(fd, (char *)&net, 4);
    if (r <= 0) return r;
    // TODO 5: convert the 4 bytes in `net` from network to host order and return them.
    return -1;
}

void *read_from_server(void *arg) {
    (void)arg;
    ssize_t size;
    while ((size = get_message_size(serverSocket)) > 0) {
        char *buf = calloc(1, size + 1);
        if (read_all_from_socket(serverSocket, buf, size) <= 0) { free(buf); break; }
        printf("\r%s\n> ", buf); fflush(stdout); free(buf);
    }
    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 4) { fprintf(stderr, "usage: %s <host> <port> <username>\n", argv[0]); return 1; }
    const char *name = argv[3];
    serverSocket = connect_to_server(argv[1], argv[2]);
    printf("connected. type messages, Ctrl-D to quit.\n> "); fflush(stdout);
    pthread_t reader; pthread_create(&reader, NULL, read_from_server, NULL);
    char line[512];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char msg[600]; snprintf(msg, sizeof(msg), "%s: %s", name, line);
        size_t body_len = strlen(msg);
        size_t declared = body_len;   // (Part 2) try making this bigger than body_len
        if (write_message_size(declared, serverSocket) <= 0) break;
        if (write_all_to_socket(serverSocket, msg, body_len) <= 0) break;
        printf("> "); fflush(stdout);
    }
    close(serverSocket);
    return 0;
}
