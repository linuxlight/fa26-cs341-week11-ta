#!/bin/bash
# One container per group; each group gets five host ports.
#   usage: ./run_groups.sh <num_groups> [base_port]
# Group g -> host ports base+g*10 + 1..5 for good/leak/crash/shortread/singlethread.
set -eu
N="${1:?usage: run_groups.sh <num_groups> [base_port]}"
BASE="${2:-14000}"
IMG=chatroom-lab
for g in $(seq 1 "$N"); do
    gp=$((BASE + g*10))
    docker run -d --rm --name "chatlab-g$g" \
        -p "$((gp+1)):14001" -p "$((gp+2)):14002" -p "$((gp+3)):14003" \
        -p "$((gp+4)):14004" "$IMG" >/dev/null
    printf "group %2d  good=%d leak=%d shortread=%d singlethread=%d\n" \
        "$g" "$((gp+1))" "$((gp+2))" "$((gp+3))" "$((gp+4))" 
done
echo "containers up. students connect to this host at the ports above."
