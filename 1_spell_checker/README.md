# Spell Checker

This is a simple spell checker program written in C. It reads a text file, checks each word against a dictionary of correctly spelled words, and outputs any misspelled words it finds.

To compile and run the program, use the following command:

```bash
gcc spell_checker.c utils.c -o sc.out && ./sc.out data/text_small.txt data/dictionary.txt
```

## Todo

- [ ] Implement a more efficient data structure for the dictionary for faster lookups.
- [ ] Add support for suggestions for misspelled words.
- [ ] Make GUI.
