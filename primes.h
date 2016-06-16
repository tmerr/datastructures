#ifndef H_PRIMES
#define H_PRIMES

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// this implementation is taken from
// http://stackoverflow.com/a/5694432/1546343

bool is_prime(size_t x);
size_t next_prime(size_t x);

#endif
