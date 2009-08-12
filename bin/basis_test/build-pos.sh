#!/bin/sh

if [ "$AT_BASIS" = "1" ]; then
  compiler="/usr/bin/g++ -m32"
  linker="/usr/bin/g++ -m32"
else
  #Default to whichever compiler/linker is in $PATH
  compiler="g++ -m32"
  linker="g++ -m32"
fi

BT_ROOT=/usr/local/BasisTech/BT_RLP ; export BT_ROOT
NAME=rlp_pos
rm -rf $NAME $NAME.o


args="-c  -DNDEBUG -D_GNU_SOURCE -D_STL=std -I$BT_ROOT/rlp/rlp/include -I$BT_ROOT/rlp/utilities/include  -pthread -pipe -ffor-scope -fno-gnu-keywords -fPIC -Wall -W -Werror   -O -ffloat-store $NAME.cpp -o $NAME.o"
$compiler $args
if [ "$?" -ne "0" ]; then
  echo Trouble running the following command:
  echo $compiler $args
  echo Please check your installation and try again.
  exit 1
fi


args="-o $NAME $NAME.o -L$BT_ROOT/rlp/lib/ia32-glibc23-gcc32 -lbtrlpcore -lbtutils -lpthread -ldl"
$compiler $args
if [ "$?" -ne "0" ]; then
  echo Trouble running the following command:
  echo $compiler $args
  echo Please check your installation and try again.
  exit 1
fi
