#!/bin/bash

# build path
JMA_PROJECT_HOME="`dirname $0`/.."
BUILD_PATH="${JMA_PROJECT_HOME}/build/temp"

# build type
BUILD_TYPE=release

# build system under win32 platform
WIN32_BUILD_SYSTEM="Visual Studio 9 2008"

if [ "$1" = "clean" ]
then 
    if test -d $BUILD_PATH
    then
        cd $BUILD_PATH
        make clean
        cd -
        rm -r $BUILD_PATH
    fi

elif [ "$1" = "" ] || [ "$1" = "debug" ] || [ "$1" = "release" ] || [ "$1" = "profile" ]
then
    if [ "$1" != "" ]
    then
        BUILD_TYPE=$1
    fi

    echo "build type:" $BUILD_TYPE

    if [ -d $BUILD_PATH ]
    then
        echo $BUILD_PATH directory exists
    else
        echo make directory $BUILD_PATH
        mkdir $BUILD_PATH
    fi

    cd $BUILD_PATH

    if [ "$2" = "" -o "$2" = "linux" ]
    then
        echo "generating Makefiles for GCC"
        cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_JMA_DEBUG_PRINT=0 ../../source
        make all
    elif [ "$2" = "win32" ]
    then
        echo "generating MSVC project (\"$WIN32_BUILD_SYSTEM\" is required)"
        cmake -G "$WIN32_BUILD_SYSTEM" -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_JMA_DEBUG_PRINT=0 ../../source
        if [ $? = 0 ]
        then
            echo "MSVC project file \"temp\JMA.sln\" is generated,"
            echo "please open and build it inside MSVC IDE."
        fi
    elif [ "$2" = "aix" ]
    then
        echo "generating Makefiles for AIX (\"xlC_r\" is required)"
        CC=xlC_r
        CXX=xlC_r
        OBJECT_MODE=64
        export CC CXX OBJECT_MODE
        echo "CC: $CC, CXX: $CXX, OBJECT_MODE: $OBJECT_MODE"
        cmake -G "Unix Makefiles" -DCMAKE_COMPILER_IS_XLC=1 -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_JMA_DEBUG_PRINT=0 ../../source
        make all
    else
        echo "unkown platform $2, please use \"linux\", \"win32\" or \"aix\"."
        exit 1
    fi

else
    echo "usage: $0 [debug|release|profile|clean] [linux|win32|aix]"
    exit 1
fi
