#!/bin/bash

# current path, which should be "bin/"
CURRENT_PATH=`pwd`
# path of the source dictionary
ORIGIN_DIC=$CURRENT_PATH/../db/ipadic
# path of temporary dictionary for training
TEMP_DIC=$CURRENT_PATH/ipadic_temp
# source path of new dictionary to generate
NEW_DIC=$CURRENT_PATH/ipadic_new_src

# raw corpus file
TRAIN_RAW=$CURRENT_PATH/../db/test/asahi_train_raw_eucjp_0.1.txt
# corpus file
TRAIN_CORPUS=$TEMP_DIC/asahi.train
# model to generate
MODEL=$TEMP_DIC/asahi.model
# POS definition file to copy
POS_DEF=pos-id.def

# clear and create directories
rm -fr $TEMP_DIC $NEW_DIC
cp -r $ORIGIN_DIC/src $TEMP_DIC
mkdir $NEW_DIC

# generate binary files
$CURRENT_PATH/mecab-dict-index -d $TEMP_DIC -o $TEMP_DIC

# generate corpus file
$CURRENT_PATH/mecab-exe -d $ORIGIN_DIC/bin_eucjp $TRAIN_RAW -o $TRAIN_CORPUS

# train CRF model (feature cut off: 2, thread number: 2)
$CURRENT_PATH/mecab-cost-train -f2 -p2 -d $TEMP_DIC $TRAIN_CORPUS $MODEL

# generate new dictionary source file
$CURRENT_PATH/mecab-dict-gen -d $TEMP_DIC -m $MODEL -o $NEW_DIC

# copy POS definition file
cp $TEMP_DIC/$POS_DEF $NEW_DIC/$POS_DEF
