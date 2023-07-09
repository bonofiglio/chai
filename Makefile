chai: *.c *.h
	$(CC) *.c -g -o chai -Wall -Wextra -pedantic -std=c99 -Wno-gnu-statement-expression
