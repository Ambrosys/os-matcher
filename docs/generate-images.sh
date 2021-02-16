#!/usr/bin/env bash

set -euo pipefail

# Note: Call this script in img directories via symlink.
#
# This script will run the conversion for untracked or modified drawio files
# in the current directory.
# To regenerate them regardless of git status, remove the directory `generated`.
#
# If you give one or more drawio files as arguments,
# the conversion will be done for those instead.
#
# The conversion will do the following:
# - uncompress and reformat input XML file
# - generate output PNG image in `./generated/`

if [ $# -ge 1 ]; then
    INPUT_FILES="$@"
    IGNORE_GIT_STATUS=true
else
    INPUT_FILES=*.drawio
fi

if [ ! -d generated ]; then
    mkdir generated
    IGNORE_GIT_STATUS=true
fi

for input in $INPUT_FILES; do
    if [ "${IGNORE_GIT_STATUS:-false}" = false ] ; then
        if ! git ls-files --error-unmatch "$input" 2>/dev/null && ! git diff --exit-code "$input" >/dev/null; then
            continue
        fi
    fi

    # uncompress and reformat input XML file

    drawio -x -f xml --uncompressed -o "$input" "$input"
    xmlstarlet fo "$input" | sponge "$input"

    # generate output PNG image in `./generated/`

    output="generated/$input.png"
    # Exportieren ohne definierten Rahmen, da drawio den Inhalt nicht richtig rendert,
    # oft sind die Ränder links oder oben zu groß, auch wenn keine unsichtbaren Elemente dort liegen.
    drawio -x -s 2.0 -f png -o "$output" "$input"
    # Nachbearbeiten: Rand entfernen mit kleiner Toleranz (mind. 5 von 256 Werten ~ 2%, da um den Text teilweise Artefakte liegen)
    # und danach 20 Pixel gleichmäßig hinzufügen.
    convert "$output" -fuzz 2% -trim -bordercolor white -border 20 "$output"
done
