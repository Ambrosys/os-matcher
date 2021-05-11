#!/usr/bin/env bash

set -euo pipefail

rm -rf build/

sudo docker build -f Dockerfile.docs -t ambrosys/os-matcher:docs .

echo "========================================================"
echo "HTML documentation: http://localhost:8080"
echo "PDF documentation:  http://localhost:8080/os-matcher.pdf"
echo "========================================================"

sudo docker run --rm -it -p 8080:80 ambrosys/os-matcher:docs
