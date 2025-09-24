# Spell Checker

This is a simple spell checker program written in C. It reads a text file, checks each word against a dictionary of correctly spelled words, and outputs any misspelled words it finds.

To compile and run the program, use the following command:

```bash
gcc spell_checker.c utils.c hash_map.c linked_list.c -o sc.out && time ./sc.out data/text_small.txt data/dictionary_small.txt linear
```

`dictionary_small.txt` and `dictionary_big.txt` are downloaded from [here](https://github.com/dwyl/english-words).

## Todo

- [x] ~~Implement a more efficient data structure for the dictionary for faster lookups.~~
- [ ] Implement trie.
- [ ] Add support for suggestions for misspelled words.
- [ ] Make GUI.
