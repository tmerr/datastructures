#include "primes.h"

// see documentation in header

bool is_prime(size_t x)
{
    size_t o = 4;
    for (size_t i = 5; true; i += o)
    {
        size_t q = x / i;
        if (q < i)
            return true;
        if (x == q * i)
            return false;
        o ^= 6;
    }
    return true;
}

size_t next_prime(size_t x)
{
    switch (x)
    {
    case 0:
    case 1:
    case 2:
        return 2;
    case 3:
        return 3;
    case 4:
    case 5:
        return 5;
    }
    size_t k = x / 6;
    size_t i = x - 6 * k;
    size_t o = i < 2 ? 1 : 5;
    x = 6 * k + o;
    for (i = (3 + o) / 2; !is_prime(x); x += i)
        i ^= 6;
    return x;
}