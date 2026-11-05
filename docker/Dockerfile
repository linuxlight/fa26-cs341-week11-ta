FROM gcc:13 AS build
WORKDIR /src
COPY netproto.h server_common.h server_good.c server_leak.c server_crash.c server_shortread.c server_singlethread.c ./
RUN for s in good leak crash shortread singlethread; do \
      gcc -Wall -O2 -pthread -o server_$s server_$s.c; done

FROM debian:stable-slim
WORKDIR /app
COPY --from=build /src/server_good /src/server_leak /src/server_crash /src/server_shortread /src/server_singlethread /app/
COPY docker/supervise.sh /app/supervise.sh
EXPOSE 14001 14002 14003 14004 14005
ENTRYPOINT ["/bin/bash", "/app/supervise.sh"]
