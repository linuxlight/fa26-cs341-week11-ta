#include "server_common.h"
#define MAX_MSG 4096
static void *handle(void *p) {
    int fd = (int)(intptr_t)p; ssize_t size;
    while ((size = get_message_size(fd)) > 0) {
        if (size > MAX_MSG) break;
        char *buf = calloc(1, size + 1);
        // BUG: one read(), no loop. A single read can return fewer bytes than
        // asked (the grader's my_read injects exactly this), so part of the
        // message is lost. Here it reliably gets back only the first part.
        ssize_t n = read(fd, buf, size / 2 + 1);
        if (n <= 0) { free(buf); break; }
        broadcast(buf, n);                 // broadcasts only the piece it read
        free(buf);
    }
    close(fd); remove_client(fd); return NULL;
}
int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "%s <port>\n", argv[0]); return 1; }
    signal(SIGPIPE, SIG_IGN); run_accept_loop(argv[1], handle);
}
