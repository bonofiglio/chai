chai: *.c *.h
	$(CC) -g *.c -o chai -Wall -Wextra -pedantic -std=c99
