#include "server_common.h"
#define MAX_MSG 4096
int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "%s <port>\n", argv[0]); return 1; }
    signal(SIGPIPE, SIG_IGN);
    for (int i = 0; i < MAX_CLIENTS; i++) clients[i] = -1;
    int s = make_listener(argv[1]);
    printf("listening on port %s\n", argv[1]);
    while (1) {
        int fd = accept(s, NULL, NULL);     // next client waits here...
        if (fd < 0) continue;
        add_client(fd);
        ssize_t size;
        // BUG: no thread. This loop serves ONE client fully; nobody else is
        // accepted or heard until this client disconnects.
        while ((size = get_message_size(fd)) > 0) {
            if (size > MAX_MSG) break;
            char *buf = calloc(1, size + 1);
            if (read_all_from_socket(fd, buf, size) <= 0) { free(buf); break; }
            broadcast(buf, size);
            free(buf);
        }
        close(fd); remove_client(fd);
    }
}
