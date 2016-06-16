all: test.c primes.c ihashtable.c
	gcc -std=c99 test.c primes.c ihashtable.c -o test

clean:
	rm test
