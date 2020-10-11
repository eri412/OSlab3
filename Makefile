CC=gcc
CFLAGS=-pedantic -Wall -Wextra -Werror -pthread

all: main random

main: main.c
	$(CC) $(CFLAGS) $< -o $@
	
random: random.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f main random
