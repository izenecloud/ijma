#!/usr/bin/ruby -w

require 'ftools'

if ARGV.size < 2
    puts "usage: add_user_noun_matrix.rb matrix.def new_matrix.def"
    exit 1
end

inputName = ARGV[0]
outputName = ARGV[1]
File.copy(inputName, outputName)

NORM_NOUN_INDEX = 1285 # 名詞,一般,*,*,*,*,* in left-id.def
USER_NOUN_INDEX = 1316 # the last index in left-id.def

File.open(outputName, "a") do |outputFile|
    File.open(inputName, "r") do |inputFile|
        normLeft = Array.new(USER_NOUN_INDEX)
        inputFile.gets # ignore the 1st size line

        while line = inputFile.gets
            line.chomp!

            if line =~ /^(\S+)\s(\S+)\s(\S+)$/
                left = $~[1].to_i
                right = $~[2].to_i
                value = $~[3].to_i

                if left == NORM_NOUN_INDEX
                    normLeft[right] = value
                end

                if right == NORM_NOUN_INDEX
                    outputFile.puts "#{left} #{USER_NOUN_INDEX} #{value}"
                end
            else
                puts "invalid line: #{line}"
            end
        end

        normLeft.each_with_index do |value, index|
            unless value
                puts "unassigned value for matrix[#{NORM_NOUN_INDEX}, #{index}]"
                next
            end
            outputFile.puts "#{USER_NOUN_INDEX} #{index} #{value}"
        end
        outputFile.puts "#{USER_NOUN_INDEX} #{USER_NOUN_INDEX} #{normLeft[NORM_NOUN_INDEX]}"
    end
end
