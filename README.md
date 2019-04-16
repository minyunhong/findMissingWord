extract existing and not existing word list with comparing xxx.bnf and xxx.txt file.

user guide

Copy the xxx.bnf and xxx.txt in the same directory.
# cmake CMakeLists.txt
# make

# use case 1 ( xxx.bnf pattern.txt )
# ./findMissingWord.exe xxx.bnf pattern.txt
# xxx.bnf file is for checking pronunciation
# pattern.txt have all of the sentence
# check notExistWordList.txt and existWordList.txt