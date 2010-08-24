#!/usr/bin/ruby1.9.1 -w

require 'set'

# input
UNI_INPUT_FILE = 'unidic_revise_Noun.all.csv'
IPA_NOUN_DIR = 'ipa_noun_utf8_old'

# output
IPA_OLD_FILE = 'ipa_old.txt'
IPA_NEW_FILE = 'ipa_new.csv'
UNI_NORM_FILE = 'uni_norm.txt'

# load IPA words
ipaWords = Set.new
# IPA file pattern
filePattern = File.join(IPA_NOUN_DIR, '*')
# iterate each IPA file
Dir.glob(filePattern) do |fileName|
    File.open(fileName, "r") do |inputFile|
        wordPattern = /^(\S+?),/
        while line = inputFile.gets
            if line =~ wordPattern
                ipaWords << $~[1]
            else
                puts "unknown format line in #{fileName}:", line
            end
        end
    end
end

# split UniDic words from UNI_INPUT_FILE
ipaOldFile = File.open(IPA_OLD_FILE, "w")
ipaNewFile = File.open(IPA_NEW_FILE, "w")
uniNormFile = File.open(UNI_NORM_FILE, "w")

File.open(UNI_INPUT_FILE, "r") do |inputFile|
    while line = inputFile.gets
        if line =~ /^(\S+)\s(.+)$/
            norm = $~[1]
            wordLine = $~[2]

            if wordLine =~ /^(\S+?),/
                word = $~[1]
            else
                puts "unknown format word line in #{UNI_INPUT_FILE}:", wordLine
                break
            end

            if ipaWords.include?(word)
                ipaOldFile.puts "#{word} #{norm}"
            else
                ipaNewFile.puts wordLine
            end

            unless ipaWords.include?(norm)
                uniNormFile.puts norm
            end
        else
            puts "unknown format line in #{UNI_INPUT_FILE}:", line
            break
        end
    end
end

ipaOldFile.close
ipaNewFile.close
uniNormFile.close
