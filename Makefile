CC=clang
CFLAGS=-Werror=vla -Wextra -Wall -Wshadow -Wswitch-default -std=c11 -g
CFLAG_SAN=$(CFLAGS) -fsanitize=address -fsanitize=leak -g
DEPS=
OBJ=hashmap.o

hashmap.o: hashmap.c hashmap.h $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
clean:
	rm *.o
