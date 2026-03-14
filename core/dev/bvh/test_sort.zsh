#!/usr/bin/env zsh

cmake .. -DSAN=ON -DDEV_FILE=bvh/test_bvh_sort.c -DUSE_DEBUG_RAYLIB=ON && make
if [[ $? -ne 0 ]]; then return $?; fi
for ((i = 0; i < 10; i++)); do
    x=$(( 10 * $i ))
    y=$(( 50 * $i ))
    z=$(( 100 * $i ))
    ./at $x $y $z
    for LETTER in {x,y,z}; do
        sort -n "$LETTER.txt" > "${LETTER}_sorted.txt"
        diff "$LETTER.txt" "${LETTER}_sorted.txt";
    done

    return 0
done
