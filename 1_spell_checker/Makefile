CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = sc.out
SOURCES = spell_checker.c utils.c hash_map.c linked_list.c trie.c

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)

test: $(TARGET)
	time ./$(TARGET) data/text_big.txt data/dictionary_big.txt hashmap

.PHONY: clean test