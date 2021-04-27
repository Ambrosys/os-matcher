#!/usr/bin/env bash

set -euo pipefail

stop_container() {
    echo "Stopping conan server..."
    docker stop $container >/dev/null
}

container=$(
  docker run \
    --rm -d \
    -p 9300:9300 \
    -v $PWD/server.conf:/root/.conan_server/server.conf:ro \
    -v $PWD/data:/root/.conan_server/data \
    docker.io/conanio/conan_server:latest
)
trap stop_container EXIT

echo Conan server logs: docker logs -f $container
echo Conan remote: http://localhost:9300/
read -p "Press any key to stop conan server "
