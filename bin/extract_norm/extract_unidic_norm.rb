#!/usr/bin/ruby1.9.1 -w

require 'set'

$katakana = Set.new
$hiragana = Set.new
$alphabet = Set.new

# load one character in each line of source file into set
def loadChar(fileName, set)
    # input file
    File.open(fileName, "r") do |inputFile|
        while line = inputFile.gets
            line.chomp!
            next if line.empty? || line[0,1] == '#'
            set << line
        end
    end
end

# check whether each character is Katakana
def isKatakana?(token)
    token.each_char do |x|
        return false unless $katakana.include?(x)
    end
    true
end

# check whether each character is Kanji
def isKanji?(token)
    token.each_char do |x|
        return false if $katakana.include?(x) || $hiragana.include?(x) || $alphabet.include?(x)
    end
    true
end

# extract the UniDic words whose lemma differs from its word
def extract(inputName, outputName)
    # output file
    outputFile = File.open(outputName, "w")

    # input file
    File.open(inputName, "r") do |inputFile|
        wordPattern = /^(\S+?),/
        lemmaPattern = /^(\S+?,){#{LEMMA_INDEX+4}}(\S+?),/
        while line = inputFile.gets
            if line =~ wordPattern
                word = $~[1]
            end

            if line =~ lemmaPattern
                lemma = $~[2]
            end

            if word != lemma
                if (isKatakana?(word) && isKatakana?(lemma)) || (isKanji?(word) && isKanji?(lemma) && word.length == lemma.length)
                    outputFile.puts "#{lemma} #{line}"
                end
            end
        end
    end

    outputFile.close
end

# the index of read form start from zero
LEMMA_INDEX = 7

if ARGV.size < 5
    puts "usage: extract_unidic_norm hiraFile kataFile alphaFile inputFile outputFile"
    exit 1
end

loadChar(ARGV[0], $hiragana)
loadChar(ARGV[1], $katakana)
loadChar(ARGV[2], $alphabet)

extract(ARGV[3], ARGV[4])
