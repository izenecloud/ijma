#!/bin/bash
# define the variables
JMA_HOME=jma
JMA_DICS=(ipadic jumandic unidic)
OTHER_JMAS_HOME=(basis chasen)
SCORE_HOME=scores
STAT_FILE=stat.txt

echo "Start the Comparison"
# init
if [ ! -d $SCORE_HOME ]
then
	mkdir -p $SCORE_HOME
fi

echo -e "Name\tRecall\tPrecision\tF-Measure" > $STAT_FILE

# do the compare
startPath=${OTHER_JMAS_HOME[0]}
startLen=${#startPath}

DICS_LEN=${#JMA_DICS[*]}
OTHERS_LEN=${#OTHER_JMAS_HOME[*]}

for file in `find ${startPath}/`
do
	if test -f $file
	then
		fileName=${file:startLen}
		echo Compare file ${fileName:1}
		i=0 # dics
		while [ $i -lt $DICS_LEN ]; do
			dicName=${JMA_DICS[$i]}
			scoreDir="${SCORE_HOME}/${dicName}"
			if [ ! -d $scoreDir ]
			then
				mkdir -p $scoreDir			
			fi
			
			j=0 # other jmas
			while [ $j -lt $OTHERS_LEN ]; do
				otherName=${OTHER_JMAS_HOME[$j]}
				compareName="${otherName}-${dicName}${fileName//\//-}"
				destFile="${scoreDir}/${compareName}"
				#echo ${JMA_HOME}/${dicName}${fileName} is jma-path
				#echo ${otherName}${fileName} is other path
				./score.pl dictionary.txt  ${otherName}${fileName} ${JMA_HOME}/${dicName}${fileName} > $destFile
				echo -n -e "$compareName\t" >> $STAT_FILE
				tail -n 7 $destFile | head -n 3 | tr -d 'A-Z= :\t' | tr '\n' '\t' >> $STAT_FILE
				echo -n -e "\n" >> $STAT_FILE
				let j++
			done

			let i++
		done
	fi
done

echo "Comparison Done. The statistical file see ${STAT_FILE} and comparison results are under directory ${SCORE_HOME}"
