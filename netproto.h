#pragma once
#include <arpa/inet.h>
#include <errno.h>
#include <stddef.h>
#include <sys/socket.h>
#include <unistd.h>
static inline ssize_t read_all_from_socket(int fd, char *buf, size_t count) {
    size_t total = 0;
    while (total < count) {
        ssize_t r = read(fd, buf + total, count - total);
        if (r == 0) return 0;
        if (r < 0) { if (errno == EINTR || errno == EAGAIN) continue; return -1; }
        total += r;
    }
    return (ssize_t)total;
}
static inline ssize_t write_all_to_socket(int fd, const char *buf, size_t count) {
    size_t total = 0;
    while (total < count) {
        ssize_t w = write(fd, buf + total, count - total);
        if (w == 0) return 0;
        if (w < 0) { if (errno == EINTR || errno == EAGAIN) continue; return -1; }
        total += w;
    }
    return (ssize_t)total;
}
