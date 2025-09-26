#!/bin/bash
set -e

TEXT_FILE=${1:-data/text_small.txt}
DICT_FILE=${2:-data/dictionary_small.txt}
STRATEGY=${3:-linear}

gcc -Wall -g spell_checker.c utils.c hash_map.c linked_list.c trie.c -o sc.out

time ./sc.out "$TEXT_FILE" "$DICT_FILE" "$STRATEGY"
