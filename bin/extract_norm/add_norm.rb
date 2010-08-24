#!/usr/bin/ruby1.9.1 -w

# input
IPA_OLD_FILE = 'ipa_old_revise.txt'
IPA_NOUN_DIR = 'ipa_noun_utf8_old'

# output
IPA_NOUN_NEW_DIR = 'ipa_noun_utf8_new'

# load IPA (word norm)
wordNorm = Hash.new
File.open(IPA_OLD_FILE, "r") do |inputFile|
    while line = inputFile.gets
        if line =~ /^(\S+)\s(\S+)$/
            word = $~[1]
            norm = $~[2]

            wordNorm[word] = norm
        else
            puts "unknown format line in #{IPA_OLD_FILE}:", line
            break
        end
    end
end

# IPA file pattern
filePattern = File.join(IPA_NOUN_DIR, '*')
# iterate each IPA file
Dir.glob(filePattern) do |inputName|
    outputName = File.join(IPA_NOUN_NEW_DIR, File.basename(inputName))
    File.open(outputName, "w") do |outputFile|
        File.open(inputName, "r") do |inputFile|
            while line = inputFile.gets
                line.chomp!
                if line =~ /^(\S+?),/
                    norm = wordNorm[$~[1]]
                    if norm
                        outputFile.puts "#{line},#{norm}"
                    else
                        outputFile.puts line
                    end
                else
                    puts "unknown format line in #{inputName}:", line
                    break;
                end
            end
        end
    end
end
