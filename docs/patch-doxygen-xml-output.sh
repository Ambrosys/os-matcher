#!/usr/bin/env bash

set -euo pipefail

# Note: Used in CMakeLists.txt

XML_DIR=$1

REPLACE_IN_FILENAME_ARRAY=(
   # to prevent os errors due to too long filenames (encryptfs allows max 143 characters)
   's/anonymous__namespace/anon_ns/g'
)

REPLACE_IN_CONTENT_ARRAY=(
   ${REPLACE_IN_FILENAME_ARRAY[0]}

   # to prevent sphinx/breathe errors due to `{` and `}` characters
   # when handling exception `breathe.directives.UnableToResolveFunctionError`
   's/anonymous_namespace\{(.*)\}/anonymous_namespace_\1/g'
)

for file in $XML_DIR/*; do
   for sed_string in "${REPLACE_IN_CONTENT_ARRAY[@]}"; do
      sed -i -E $sed_string $file
   done

   filename=${file#$XML_DIR/}
   new_filename=$filename
   for sed_string in "${REPLACE_IN_FILENAME_ARRAY[@]}"; do
      new_filename="$(echo $new_filename | sed -E $sed_string)"
   done

   if [[ $filename != $new_filename ]]; then
      mv "$XML_DIR/$filename" "$XML_DIR/$new_filename"
   fi
done
