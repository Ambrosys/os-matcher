#!/usr/bin/env bash

set -euo pipefail

rm -rf build/

docker build -f Dockerfile.docs -t ambrosys/os-matcher:docs .

echo ==========================
echo Open http://localhost:8080
echo ==========================

docker run --rm -it -p 8080:80 ambrosys/os-matcher:docs
