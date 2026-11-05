#!/bin/bash
# Keeps all lab servers alive in one container. Restarts any that die (the crash
# server will when a group breaks it). Handles SIGTERM/SIGINT so `docker stop`
# and Ctrl-C shut the container down cleanly and free the ports.
set -u
BIN_DIR="${BIN_DIR:-/app}"
declare -A PORT=( [good]=14001 [leak]=14002 [crash]=14003 [shortread]=14004 [singlethread]=14005 )
ORDER=(good leak crash shortread singlethread)
declare -A PID

start() { "$BIN_DIR/server_$1" "${PORT[$1]}" & PID[$1]=$!; echo "started server_$1 on port ${PORT[$1]} (pid ${PID[$1]})"; }
shutdown() { echo "shutting down"; for s in "${ORDER[@]}"; do kill "${PID[$s]}" 2>/dev/null; done; exit 0; }
trap shutdown TERM INT

for s in "${ORDER[@]}"; do start "$s"; done
while true; do
    wait -n
    for s in "${ORDER[@]}"; do
        if ! kill -0 "${PID[$s]}" 2>/dev/null; then echo "server_$s died; restarting"; start "$s"; fi
    done
done
