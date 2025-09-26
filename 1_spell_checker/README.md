# Spell Checker

This is a simple spell checker program written in C. It reads a text file, checks each word against a dictionary of correctly spelled words, and outputs any misspelled words it finds.

To compile and run the program, use the following command:

```bash
./build_and_run.sh [text_file] [dictionary_file] [search_strategy]
```

You can test different search strategies by changing the last argument to:

- `linear`: Linear search
- `hashmap`: Hash map search
- `trie`: Trie search

`dictionary_small.txt` and `dictionary_big.txt` are downloaded from [here](https://github.com/dwyl/english-words).

## Todo

- [x] ~~Implement a more efficient data structure for the dictionary for faster lookups.~~
- [x] ~~Implement trie.~~
- [ ] Add support for suggestions for misspelled words.
- [ ] Make GUI.
