#!/bin/bash
# define the variables
JMA_PROJECT_HOME="`dirname $0`/.."
BUILD_EXE="${JMA_PROJECT_HOME}/bin/jma_encode_sysdict"
JMA_DICS=(ipadic jumandic unidic)
ENCODE_TYPES=(eucjp sjis utf8)

# usage warning
echo "!! This script is used to build dictionaries of IPA, JUMAN and UniDic."
echo "!! The destination directory of the binary files would be such like $JMA_PROJECT_HOME/db/${JMA_DICS[0]}/bin_${ENCODE_TYPES[0]}."
echo "!! Warning: the original binary files would be overwritten."

# confirm with user
proceed=empty
until [ "$proceed" = "y" -o "$proceed" = "n" ]
do
    echo -n "!! Do you want to proceed? [y|n] "
    read proceed
done

echo
if [ "$proceed" = "n" ]
then
    exit 1
fi

echo "!! Dictionary build start."
echo

# build dictionary
DICS_NUM=${#JMA_DICS[*]}
ENCODE_NUM=${#ENCODE_TYPES[*]}
dic=0

while [ $dic -lt $DICS_NUM ]; do
	dicPath="$JMA_PROJECT_HOME/db/${JMA_DICS[$dic]}"
    src="$dicPath/src"
    encode=0

    while [ $encode -lt $ENCODE_NUM ]; do
        bin="$dicPath/bin_${ENCODE_TYPES[$encode]}"

        if [ ! -d $dicPath ]
        then
            echo "!!$dicPaht not exists, create it."
            mkdir $dicPath
        fi

        echo "!! building from $src to $bin"
        $BUILD_EXE --encode ${ENCODE_TYPES[$encode]} $src $bin 2>/dev/null

        let encode++
        echo
    done

	let dic++
    echo
done

echo "!! Dictionary build done."
