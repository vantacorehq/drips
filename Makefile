CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -O2

.PHONY: test clean

test: drips.c drips_test.c drips.h
	$(CC) $(CFLAGS) -o drips_test drips.c drips_test.c
	./drips_test

clean:
	rm -f drips_test
