#!/usr/bin/env bash

set -euo pipefail

sudo docker build \
  --network=host \
  --build-arg USER_ID=$(id -u) \
  --build-arg GROUP_ID=$(id -g) \
  -f Dockerfile.apps \
  -t ambrosys/os-matcher:latest \
  .
