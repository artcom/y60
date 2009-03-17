#!/bin/bash

for  n in {0..10}; do
    (( n = 10 * n ))
    echo $n
    make clean
    echo "#define NUM_FUNCTIONS $n" > src/benchmark_config.h
    time  make >/dev/null
done
