#!/bin/bash

BASE_DIR="../../"
MCNC_PD91_DIR="mcnc/pd91/bench/"

BENCH_DIR="mixed"
BENCH_NAME="a3"

mkdir -p $BENCH_DIR

./yal2json \
    "$BASE_DIR/$MCNC_PD91_DIR/$BENCH_DIR/$BENCH_NAME.yal" \
    "$BENCH_DIR/$BENCH_NAME.json"

python test.py $BENCH_DIR/$BENCH_NAME.json