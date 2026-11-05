#!/bin/bash
docker ps -a --filter "name=chatlab-g" --format '{{.Names}}' | xargs -r docker rm -f
echo "all chatlab containers stopped."
