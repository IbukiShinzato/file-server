#!/bin/sh

for i in $(seq 1 16); do
    LC_ALL=C tr -dc 'A-Za-z0-9' < /dev/urandom | head -c 32 > "$HOME/test${i}.txt"
    echo >> "$HOME/test${i}.txt"
done

echo "created test1.txt ~ test16.txt in $HOME"
