#include "server_common.h"
#define SECRET_AT 64
#define ARENA 1024
static void *handle(void *p) {
    int fd = (int)(intptr_t)p; ssize_t size;
    while ((size = get_message_size(fd)) > 0) {
        if (size > ARENA) size = ARENA;
        static __thread char arena[ARENA];
        memset(arena, '.', ARENA);                               // printable filler
        memcpy(arena + SECRET_AT, "FLAG{trust_no_length}", 21);  // planted secret
        read(fd, arena, SECRET_AT - 1);          // BUG: read only what arrived
        broadcast(arena, size);                  // ... broadcast the declared count
    }
    close(fd); remove_client(fd); return NULL;
}
int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "%s <port>\n", argv[0]); return 1; }
    signal(SIGPIPE, SIG_IGN); run_accept_loop(argv[1], handle);
}
