#!/usr/bin/ruby1.9.1 -w

# input
UNI_INPUT_FILE = 'ipa_new_total.csv'
POS_MAP_FILE = 'map-pos-uni-ipa.def'

# output
IPA_OUTPUT_FILE = 'ipa_new_ipa_format.csv'

# load pos map
posMap = Hash.new
File.open(POS_MAP_FILE, "r") do |posFile|
    while line = posFile.gets
        line.chomp!
        next if line.empty? || line[0,1] == '#'
        posArray = line.split
        if posArray.size > 1
            posMap[posArray[0]] = posArray[1..-1]
        else
            puts "invalid line in #{POS_MAP_FILE}:", line
        end
    end
end

posMap.each do |x, y|
    puts "#{x} => #{y}"
end

# convert dict format
File.open(IPA_OUTPUT_FILE, "w") do |outputFile|
    File.open(UNI_INPUT_FILE, "r") do |inputFile|
        while line = inputFile.gets
            if line =~ /^(\S+?),(\d+?,){2}(\d+?,)((\S+?,){4})(\S+?,\S+?,)\S+?,(\S+?),\S+?,(\S+?,)(\S+?,)\S+?,(\S+?,)/
                word = $~[1]
                weight = $~[3]
                pos = $~[4]
                ctype = $~[6]
                norm = $~[7]
                pron = $~[8]
                read = $~[9]
                base = $~[10]

                #if read != pron
                    #puts "line: #{line}"
                    #puts "word: #{word}", "weight: #{weight}", "pos: #{pos}", "norm: #{norm}", "pron: #{pron}", "read: #{read}", "base: #{base}"
                    #break
                #end
                #next

                if ctype != "*,*,"
                    puts "unknown ctype in #{UNI_INPUT_FILE}:", ctype
                    break;
                end

                posArray = posMap[pos]
                if posArray
                    posArray.each do |x|
                        outputFile.puts "#{word},-1,-1,#{weight}#{x}#{ctype}#{base}#{read}#{pron}#{norm}"
                    end
                else
                    puts "unknown POS in #{UNI_INPUT_FILE}:", pos
                    break;
                end
            else
                puts "unknown format line in #{UNI_INPUT_FILE}:", line
                break;
            end
        end
    end
end
