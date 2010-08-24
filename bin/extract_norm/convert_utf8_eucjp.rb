#!/usr/bin/ruby1.9.1 -w

# input
IPA_UTF8_FILE = 'ipa_new_ipa_format.csv'

# output
IPA_EUCJP_FILE = 'ipa_new_eucjp.csv'
IPA_ONLY_UTF8_FILE = 'ipa_new_ipa_format_only_utf8_not_eucjp.csv'

# convert from UTF8 to EUC-JP
File.open(IPA_EUCJP_FILE, "w:euc-jp") do |eucjpFile|
    File.open(IPA_ONLY_UTF8_FILE, "w:utf-8") do |bkFile|
        File.open(IPA_UTF8_FILE, "r:utf-8") do |utf8File|
            while line = utf8File.gets
                begin
                    eucjp = line.encode("euc-jp")
                    eucjpFile.puts eucjp
                rescue
                    bkFile.puts line
                end
            end
        end
    end
end
