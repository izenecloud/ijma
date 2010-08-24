#!/usr/bin/ruby1.9.1 -w

require 'set'

# input
UNI_NORM_FILE = 'uni_norm.txt'
UNI_NOUN_DIR = 'uni_noun'

# output
IPA_NEW_NORM_FILE = 'ipa_new_norm.csv'

# load UniDic words
uniWords = Set.new
File.open(UNI_NORM_FILE, "r") do |inputFile|
    while line = inputFile.gets
        line.chomp!
        if line.empty?
            puts "unknown format line in #{UNI_NORM_FILE}:", line
        else
            uniWords << line
        end
    end
end

ipaNewNormFile = File.open(IPA_NEW_NORM_FILE, "w")

# UniDic file pattern
filePattern = File.join(UNI_NOUN_DIR, '*')
# iterate each UniDic file
Dir.glob(filePattern) do |fileName|
    File.open(fileName, "r") do |inputFile|
        while line = inputFile.gets
            if line =~ /^(\S+?),/
                if uniWords.include?($~[1])
                    ipaNewNormFile.puts line
                end
            else
                puts "unknown format line in #{fileName}:", line
            end
        end
    end
end

ipaNewNormFile.close
