#!/bin/bash

BASE_DIR="../../"
MCNC_PD91_DIR="mcnc/pd91/bench/"

BENCH_DIR="mixed"
BENCH_NAME="a3"

mkdir -p $BENCH_DIR

# print to file
./yal2json \
    "$BASE_DIR/$MCNC_PD91_DIR/$BENCH_DIR/$BENCH_NAME.yal" \
    "$BENCH_DIR/$BENCH_NAME.json" \
    2> /dev/null
python test.py $BENCH_DIR/$BENCH_NAME.json > /dev/null

# print to stdout
diff $BENCH_DIR/$BENCH_NAME.json <( \
    ./yal2json \
    "$BASE_DIR/$MCNC_PD91_DIR/$BENCH_DIR/$BENCH_NAME.yal" \
    2> /dev/null)

# mix stderr in
diff $BENCH_DIR/$BENCH_NAME.json \
 <(./yal2json "$BASE_DIR/$MCNC_PD91_DIR/$BENCH_DIR/$BENCH_NAME.yal" 2>&1)