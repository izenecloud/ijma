#!/bin/sh
#
# File:    go-cpp-samples.sh
# 
# Purpose: This wrapper script runs the C++ RLP samples found in 
#          rlp\samples\cplusplus.
#          This illustrates the required files, options, and environment
#          variables needed to run these samples. 
# 

if [ "$1" = "" ] && [ "$2" = "" ]
then
    echo Usage: $0 inputFile outputFile
    exit 1
fi

EXE_NAME=rlp_pos
BT_ROOT=/usr/local/BasisTech/BT_RLP ; export BT_ROOT
LD_LIBRARY_PATH=$BT_ROOT/rlp/lib/ia32-glibc23-gcc32:$BT_ROOT/rlp/bin/ia32-glibc23-gcc32 ; export LD_LIBRARY_PATH


echo 
echo ---------------------------------------
echo Running rlp_sample C++ sample - output in rlp_sample.out
echo ---------------------------------------
echo
#./rlp_sample $BT_ROOT ja "$BT_ROOT/rlp/etc/rlp-global.xml" "$BT_ROOT/rlp/samples/data/ja-text.txt" rlp_sample.out
`dirname $0`/$EXE_NAME $BT_ROOT ja "$BT_ROOT/rlp/etc/rlp-global.xml" $1 $2

if [ "$?" -ne "0" ]; then
  echo "Trouble running $EXE_NAME.  Please check your installation and try again."
  exit 1
fi

#echo
#echo ---------------------------------------
#echo Running rlbl_sample C++ sample - output in rlbl_sample.out
#echo ---------------------------------------
#echo
#"$BT_ROOT/rlp/bin/ia32-glibc23-gcc32/rlbl_sample"  $BT_ROOT  "$BT_ROOT/rlp/etc/rlp-global.xml" "$BT_ROOT/rlp/samples/data/Spanish-French-German.txt" rlbl_sample.out

#if [ "$?" -ne "0" ]; then
  #echo "Trouble running $BT_ROOT/rlp/bin/ia32-glibc23-gcc32/rlbl_sample.  Please check your installation and try again."
  #exit 1
#fi
