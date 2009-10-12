#!/bin/bash

ID=$1
RAW_NAME=$2

CUR_PATH=`dirname $0`
JMA_PATH=$CUR_PATH/../..
DB_PATH=$CUR_PATH/db
TEST_PATH=$DB_PATH/$ID

TOTAL_LOG=$DB_PATH/log.txt
RAW_TEMP=$DB_PATH/$ID.txt
LOG=$TEST_PATH/log.txt
RAW_INPUT=$TEST_PATH/$RAW_NAME
RAW_SENTENCE=$TEST_PATH/raw_sentence_utf8.txt
BASIS_OUTPUT=$TEST_PATH/basis_utf8.txt
JMA_OUTPUT=$TEST_PATH/jma_utf8.txt

echo ">>>>> start id: $ID, file: $RAW_NAME at `date`" >> $TOTAL_LOG

mkdir $TEST_PATH

echo `date` >> $LOG
echo ">>>>> mv $RAW_TEMP $RAW_INPUT" >> $LOG
mv $RAW_TEMP $RAW_INPUT 2>>$LOG
RET=$?
if [ "$RET" != "0" ]
then
    echo "<<<<< mv end with error $RET" >> $LOG
    echo >> $LOG
    echo "<<<<< error (mv) id: $ID, file: $RAW_NAME at `date`" >> $TOTAL_LOG
    echo >> $TOTAL_LOG
    exit $RET
fi

echo ">>>>> Sentence split start" >> $LOG
$JMA_PATH/bin/jma_split_sentence $RAW_INPUT $RAW_SENTENCE --encode utf8 --config $JMA_PATH/db/config/sen-utf8.config >>$LOG 2>&1
RET=$?
if [ "$RET" != "0" ]
then
    echo "<<<<< Sentence split end with error $RET" >> $LOG
    echo >> $LOG
    echo "<<<<< error (split) id: $ID, file: $RAW_NAME at `date`" >> $TOTAL_LOG
    echo >> $TOTAL_LOG
    exit $RET
fi

echo ">>>>> Basis start" >> $LOG
$JMA_PATH/bin/basis_test/go-pos.sh $RAW_SENTENCE $BASIS_OUTPUT >>$LOG 2>&1
RET=$?
if [ "$RET" != "0" ]
then
    echo "<<<<< Basis end with error $RET" >> $LOG
    echo >> $LOG
    echo "<<<<< error (basis) id: $ID, file: $RAW_NAME at `date`" >> $TOTAL_LOG
    echo >> $TOTAL_LOG
    exit $RET
fi

echo ">>>>> iJMA start" >> $LOG
$JMA_PATH/bin/jma_pos_nbest --dict $JMA_PATH/db/jumandic/bin_utf8 --nbest 4 $RAW_SENTENCE $JMA_OUTPUT >>$LOG 2>&1
RET=$?
if [ "$RET" != "0" ]
then
    echo "<<<<< iJMA end with error $RET" >> $LOG
    echo >> $LOG
    echo "<<<<< error (ijma) id: $ID, file: $RAW_NAME at `date`" >> $TOTAL_LOG
    echo >> $TOTAL_LOG
    exit $RET
fi

echo >> $LOG
echo "<<<<< end id: $ID, file: $RAW_NAME at `date`" >> $TOTAL_LOG
echo >> $TOTAL_LOG
