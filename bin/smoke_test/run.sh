#!/bin/bash
OUTPUT_DIR=smoke_test/output

INPUT=smoke_test/input/run.utf8
DICT=../db/ipadic/bin_utf8
echo "jma_run --stream utf8..."
./jma_run --stream ${INPUT} ${OUTPUT_DIR}/result_jma_run_stream.utf8 --dict ${DICT}
echo

INPUT=smoke_test/input/run.eucjp
DICT=../db/ipadic/bin_eucjp
echo "jma_run --stream eucjp..."
./jma_run --stream ${INPUT} ${OUTPUT_DIR}/result_jma_run_stream.eucjp --dict ${DICT}
echo

INPUT=smoke_test/input/run.sjis
DICT=../db/ipadic/bin_sjis
echo "jma_run --stream sjis..."
./jma_run --stream ${INPUT} ${OUTPUT_DIR}/result_jma_run_stream.sjis --dict ${DICT}
echo

INPUT=smoke_test/input/run.utf8
DICT=../db/ipadic/bin_utf8
echo "jma_run --sentence..."
./jma_run --sentence 5 --dict ${DICT} <${INPUT} >${OUTPUT_DIR}/result_jma_run_sentence_5.utf8
echo

INPUT=smoke_test/input/user_noun.utf8
USER_DIC=../db/userdic/user_noun.utf8
echo "test_jma_userdic utf8..."
./test_jma_userdic --user ${USER_DIC} <${INPUT} >${OUTPUT_DIR}/result_jma_userdic_utf8.utf8
echo

USER_DIC=../db/userdic/user_noun.eucjp
echo "test_jma_userdic eucjp..."
./test_jma_userdic --user ${USER_DIC} <${INPUT} >${OUTPUT_DIR}/result_jma_userdic_eucjp.utf8
echo

USER_DIC=../db/userdic/user_noun.sjis
echo "test_jma_userdic sjis..."
./test_jma_userdic --user ${USER_DIC} <${INPUT} >${OUTPUT_DIR}/result_jma_userdic_sjis.utf8
echo

USER_DIC=../db/userdic/user_noun.utf8
echo "test_jma_userdic utf8 decompose..."
./test_jma_userdic --user ${USER_DIC} --decomp 1 <${INPUT} >${OUTPUT_DIR}/result_jma_userdic_utf8_decomp.utf8
echo

INPUT=smoke_test/input/run.utf8
DICT=../db/ipadic/bin_utf8
echo "test_jma_keyword_extraction..."
./test_jma_keyword_extraction --pos NP-G,NP-H,NP-S,NP-GN,NP-O,NP-P,NP-C --dict ${DICT} <${INPUT} >${OUTPUT_DIR}/result_jma_keyword_extraction.utf8
echo

COMPOUND_INPUT_FILE=smoke_test/input/compound.utf8
DICT=../db/ipadic/bin_utf8
echo "test_jma_compound --comp 0..."
./test_jma_compound --comp 0 --dict ${DICT} <${COMPOUND_INPUT_FILE} >${OUTPUT_DIR}/result_jma_compound_0.utf8
echo

DICT=../db/ipadic/bin_utf8
echo "test_jma_compound --comp 1..."
./test_jma_compound --comp 1 --dict ${DICT} <${COMPOUND_INPUT_FILE} >${OUTPUT_DIR}/result_jma_compound_1.utf8
echo

CONVERT_INPUT_FILE=smoke_test/input/convert.utf8
DICT=../db/ipadic/bin_utf8
echo "test_jma_convert_kana --to hira,half,lower..."
./test_jma_convert_kana --to hira,half,lower --dict ${DICT} <${CONVERT_INPUT_FILE} >${OUTPUT_DIR}/result_jma_convert_to_left.utf8
echo

DICT=../db/ipadic/bin_utf8
echo "test_jma_convert_kana --to kata,full,upper..."
./test_jma_convert_kana --to kata,full,upper --dict ${DICT} <${CONVERT_INPUT_FILE} >${OUTPUT_DIR}/result_jma_convert_to_right.utf8
echo

NORM_INPUT_FILE=smoke_test/input/norm.utf8
DICT=../db/ipadic/bin_utf8
echo "test_jma_norm..."
./test_jma_norm --dict ${DICT} <${NORM_INPUT_FILE} >${OUTPUT_DIR}/result_jma_norm.utf8
echo

SENTENCE_INPUT_FILE=smoke_test/input/sentence.utf8
DICT=../db/ipadic/bin_utf8
echo "jma_split_sentence..."
./jma_split_sentence ${SENTENCE_INPUT_FILE} ${OUTPUT_DIR}/result_jma_sentence.utf8 --dict ${DICT}
echo

INPUT=smoke_test/input/run.utf8
STOP_DIC=../db/stopworddic/stop.utf8
DICT=../db/ipadic/bin_utf8
echo "test_jma_stopword utf8..."
./test_jma_stopword --stop ${STOP_DIC} --dict ${DICT} <${INPUT} >${OUTPUT_DIR}/result_jma_stopword.utf8
echo

INPUT=smoke_test/input/run.eucjp
STOP_DIC=../db/stopworddic/stop.eucjp
DICT=../db/ipadic/bin_eucjp
echo "test_jma_stopword eucjp..."
./test_jma_stopword --stop ${STOP_DIC} --dict ${DICT} <${INPUT} >${OUTPUT_DIR}/result_jma_stopword.eucjp
echo

INPUT=smoke_test/input/run.sjis
STOP_DIC=../db/stopworddic/stop.sjis
DICT=../db/ipadic/bin_sjis
echo "test_jma_stopword sjis..."
./test_jma_stopword --stop ${STOP_DIC} --dict ${DICT} <${INPUT} >${OUTPUT_DIR}/result_jma_stopword.sjis
echo

INPUT=smoke_test/input/run.utf8
MULTI_DIR=smoke_test/multithread
DICT=../db/ipadic/bin_utf8
mkdir -p ${MULTI_DIR}
rm -f ${MULTI_DIR}/*
echo "jma_multithread --stream utf8..."
./jma_multithread ${INPUT} ${MULTI_DIR} --dict ${DICT} --num 100
echo

