#!/bin/bash

# build path
BUILD_PATH=./temp
CURRENT_PATH=`pwd`

# build type
BUILD_TYPE=debug

if [ "$1" = "clean" ]
then 
    if test -d $BUILD_PATH
    then rm -r $BUILD_PATH
    fi

    # remove all lib files
    rm -fr $CURRENT_PATH/../lib/lib*

    # remove all test files
    rm -fr $CURRENT_PATH/../bin/test_* $CURRENT_PATH/../bin/mecab* $CURRENT_PATH/../bin/demo_*
elif [ "$1" = "" ] || [ "$1" = "debug" ] || [ "$1" = "release" ] || [ "$1" = "profile" ]
then
    if [ "$1" != "" ]
    then
        BUILD_TYPE=$1
    fi

    echo "build type:" $BUILD_TYPE

    if [ -d $BUILD_PATH ]; then
        echo $BUILD_PATH directory exists
    else
        echo make directory $BUILD_PATH
        mkdir $BUILD_PATH
    fi

    cd $BUILD_PATH
    cmake -G "Unix Makefiles" -DCMAKE_COMPILER_IS_GNUCXX=1 -DCMAKE_BUILD_TYPE=$BUILD_TYPE $CURRENT_PATH/../source
    make all
else
    echo "usage: $0 [debug|release|profile|clean]"
fi
