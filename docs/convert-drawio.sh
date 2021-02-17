#!/usr/bin/env bash

set -euo pipefail

# Note: Used in CMakeLists.txt

drawio -x -s 2.0 -f png -o "$2" "$1"
convert "$2" -fuzz 2% -trim -bordercolor white -border 20 "$2"
