'''
XO offset extraction tool
Takes output from the winc_programmer tool, finds the valid efuse bank, extracts the crystal offset and sends it to stdout

Support for; WINC1500, WINC3400 efuse banks
Parses the efuse ouput from the winc_programmer command line, for example:
 > winc_programmer_i2c.exe -d winc1500  -r efuse -pfw programmer_firmware\release3A0\programmer_release_text.bin | python3 extract_xo_offset.py

'''
import sys
import re

efuse_banks = []


def process_efuse_line(line):
    efuse_str_words = re.findall(r'0x([0-9a-fA-F]+)(?:0x)?\s', line)
    efuse_hex_words = []

    for str_word in efuse_str_words:
        efuse_hex_words.append(int(str_word,16))

    return efuse_hex_words


def print_banks(banks):
    b = 0
    for bank in banks:
        print "BANK {}: ".format(b),
        b += 1
        for word in bank:
            print "0x{0:08x} ".format(word),
        print("")


def find_active_bank(banks):
    for bank in banks:
        if bank[0] & 0x80000000:
            if bank[0] & 0x40000000 == 0:
                break

    #print "\nACTIVE BANK IS 0x{0:08x} 0x{1:08x} 0x{2:08x} 0x{3:08x}".format(bank[0],bank[1],bank[2],bank[3])
    return bank


def get_xo_offset(bank):
    efuse_word = bank[2]
    xo_offs = (efuse_word & 0x7fff0000) >> 16
    xo_offs_valid = efuse_word & 0x80000000 
    if xo_offs_valid != 0:
        return xo_offs
    return 0


for line in sys.stdin:
    match = re.search("efuse bank", line)
    if match:
        efuse_banks.append(process_efuse_line(match.string))

#print_banks(efuse_banks)

if (len(efuse_banks) > 0):
    active_bank = find_active_bank(efuse_banks)
    xo_offs = get_xo_offset(active_bank)
    print "0x{0:04x}".format(xo_offs)
else:
    print "0x0"