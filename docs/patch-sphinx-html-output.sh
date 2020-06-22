#!/usr/bin/env bash

HTML_DIR=$1

shopt -s globstar
for file in $HTML_DIR/_api/*.html $HTML_DIR/content/**/*.html $HTML_DIR/index.html; do
    cat "$file" \
        | hxremove 'a[href*="#full-api"] + ul > li.toctree-l3 > ul' \
        | sed -r 's#(<script [^>]+)/>#\1></script>#g' \
        | sed -r 's#(<i [^>]+)/>#\1></i>#g' \
        | sponge "$file"
done
