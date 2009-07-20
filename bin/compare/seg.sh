#!/bin/bash
# define the variables
JMA_PROJECT_HOME="`dirname $0`/../.."
JMA_EXE="${JMA_PROJECT_HOME}/bin/jma_run"
BASIS_EXE="${JMA_PROJECT_HOME}/bin/basis_test/go-seg.sh"
CHASEN_EXE=chasen

# raw meterail home
RAW_HOME=raw

JMA_HOME=jma
JMA_DICS=(ipadic jumandic unidic)
BASIS_HOME=basis
CHASEN_HOME=chasen

DICS_LEN=${#JMA_DICS[*]}
i=0
while [ $i -lt $DICS_LEN ]; do
	dicPath="$JMA_HOME/${JMA_DICS[$i]}"
	if [ ! -d $dicPath ]
	then
		mkdir -p $dicPath
	fi
	let i++
done

if [ ! -d $BASIS_HOME ]
then
	mkdir -p $BASIS_HOME
fi

if [ ! -d $CHASEN_HOME ]
then
	mkdir -p $CHASEN_HOME
fi

echo "!!Start Segmentation"

# do the compare
startPath=${RAW_HOME}
startLen=${#startPath}


for file in `find ${startPath}/`
do
	if test -f $file
	then
		fileName=${file:startLen}
		echo !!Segment file ${fileName:1}
		
		# chasen segment
		destFile="${CHASEN_HOME}${fileName}"
		echo !!Use Chasen:
		time `${CHASEN_EXE} -F "%m  " -o ${destFile} ${file}`

		# basis segment
		tmpUtf8In="${BASIS_HOME}${fileName}.in"
		destFile="${BASIS_HOME}${fileName}"
		tmpUtf8Out="${BASIS_HOME}${fileName}.out"

		iconv -t utf8 -f euc-jp -o ${tmpUtf8In} ${file}
		echo !!Use Basis:
		${BASIS_EXE} ${tmpUtf8In} ${tmpUtf8Out}
		iconv -t euc-jp -f utf8 -o ${destFile} ${tmpUtf8Out}
		
		rm ${tmpUtf8In} ${tmpUtf8Out}

		# jma segment
		i=0
		while [ $i -lt $DICS_LEN ]; do
			dicName=${JMA_DICS[$i]}
			destFile="${JMA_HOME}/${dicName}${fileName}"
			dicDbPath="${JMA_PROJECT_HOME}/db/${dicName}/bin_eucjp"
			echo !!Use JMA with dictionary $dicName:
			${JMA_EXE} --stream ${file} ${destFile} --dict ${dicDbPath}
			let ++i
		done
	fi
done

echo "!!Segmentation Done."
