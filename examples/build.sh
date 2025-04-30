#!/bin/bash

SUFFIX="${1:-01}"

SRC="example${SUFFIX}.c"
EXEC="example${SUFFIX}"

if [ "$#" -ne 1 ]; then
    echo -e "Usage: ./build.sh <num>"
    echo "<num> have to be prefixed with 0"
    exit 1
fi

if [ ! -f "$SRC" ]; then
    echo "Error: File $SRC not found!"
    exit 1
fi

if [ "$(uname)" = "Linux" ]; then
    EXEC="${EXEC}.out"
    gcc "$SRC" -lm -O2 -o "$EXEC" && ./"$EXEC"
elif [ "$(uname | grep -i 'mingw\|msys\|cygwin')" ]; then
    EXEC="${EXEC}.exe"
    gcc "$SRC" -lm -O2 -o "$EXEC" && "$EXEC"
fi
rm $EXEC
