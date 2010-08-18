#!/usr/bin/ruby -w

require 'ftools'

# hash from Kata to Hira
$kataHira = Hash.new

# convert Kata in input to Hira in output
def convertFile(inputName, outputName)
    # output file
    outputFile = File.open(outputName, "w")

    # input file
    File.open(inputName, "r") do |inputFile|
        pattern = /^(\S+?,){#{READ_FORM_INDEX}}/
        while line = inputFile.gets
            startIndex = line[pattern].size
            endIndex = line.index(',', startIndex)

            x = startIndex
            while x < endIndex do
                kata = line[x,2]
                if $kataHira[kata]
                    line[x,2] = $kataHira[kata]
                else
                    puts "line: #{inputFile.lineno}, unkown katakana: #{kata}"
                end
                x += 2
            end

            outputFile.puts line
        end
    end

    outputFile.close
end

# the index of read form start from zero
READ_FORM_INDEX = 11

if ARGV.size < 3
    puts "usage: convert_read_hiragana map-kana.def inputDir outputDir"
    exit 1
end

# read map-kana.def
File.open(ARGV[0], "r") do |kanaFile|
    while line = kanaFile.gets
        line.chomp!
        next if line.empty? || line[0,1] == '#'
        kanas = line.split
        if kanas.size == 2
            $kataHira[kanas[1]] = kanas[0]
        else
            puts "invalid line: ", line
        end
    end
end

# input file pattern
filePattern = File.join(ARGV[1], '*')

# iterate each input file
Dir.glob(filePattern) do |inputfile|
    outputfile = File.join(ARGV[2], File.basename(inputfile))
    if File.extname(inputfile) == '.csv'
        puts "#{inputfile} *=> #{outputfile}"
        convertFile(inputfile, outputfile)
    else
        puts "#{inputfile} => #{outputfile}"
        File.copy(inputfile, outputfile)
    end
end
