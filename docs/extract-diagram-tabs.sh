#!/usr/bin/env bash

set -euo pipefail

# Note: Use this script to create drawio files for each tab in each xml file in the current directory.
#       (Not needed in normal workflow.)

source_files=*.xml

for source_file in $source_files; do
    ids=$(cat "$source_file" | xmlstarlet sel -t -m '//mxfile/diagram/@id' -v . -n)
    for id in $ids; do
        tab_content=$(cat "$source_file" | xmlstarlet ed -d "//mxfile/diagram[@id!='$id']")
        tab_name=$(cat "$source_file" | xmlstarlet sel -t -m "//mxfile/diagram[@id='$id']/@name" -v . -n)
        tab_name_pascal_case=$(python -c "print( '$tab_name'.title().replace(' ', '') )")
        target_file="${source_file%.*}-${tab_name_pascal_case}.drawio"
        echo $tab_content > "$target_file"

        # uncompress and format
        drawio -x -f xml --uncompressed -o "$target_file" "$target_file"
        xmlstarlet fo "$target_file" | sponge "$target_file"
    done
    rm "$source_file"
done
