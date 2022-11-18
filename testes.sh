#!/bin/bash

echo "Running test file corpus16MB"
for ((i = 9; i <= 16; i++))
do
    echo "Run ${i}... ${CURRENTDATE}"
    ./build/iti_lzw ~/Downloads/corpus16MB.txt compress ${i} output/corpus16_${i}.lzw
done

echo "Running test file disco.mp4"
for ((i = 9; i <= 16; i++))
do
    echo "Run ${i}... ${CURRENTDATE}"
    ./build/iti_lzw ~/Downloads/disco.mp4 compress ${i} output/disco_${i}.lzw
done
