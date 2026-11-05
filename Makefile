CC=gcc
CFLAGS=-Wall -g -pthread
SERVERS=server_good server_leak server_shortread server_singlethread

all: $(SERVERS) client client_solution client_attack

server_%: server_%.c server_common.h netproto.h
	$(CC) $(CFLAGS) -o $@ $<
client: client_skeleton.c netproto.h
	$(CC) $(CFLAGS) -o $@ $<
client_solution: client_solution.c netproto.h
	$(CC) $(CFLAGS) -o $@ $<
client_attack: client_attack.c netproto.h
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(SERVERS) client client_solution client_attack
