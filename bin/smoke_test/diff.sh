#!/bin/bash
GOLD_DIR=smoke_test/gold
OUTPUT_DIR=smoke_test/output

# option "-w" to ignore white space different between Linux and Win32.
diff -qw ${GOLD_DIR} ${OUTPUT_DIR}

# check multi-thread results
MULTI_DIR=smoke_test/multithread
GOLD_FILE=${GOLD_DIR}/result_jma_run_stream.utf8
count=0
for file in `find ${MULTI_DIR}/*`
do
    diff -qw ${GOLD_FILE} ${file}
    let count++
done

THREAD_NUM=100
if [ $count != $THREAD_NUM ]
then
    echo "error: $THREAD_NUM threads output only $count results."
fi
