#include <math.h>
#include <stdlib.h>

#include "prime.h"


enum
{
    IMPOSSIBLE_TO_DEFINE,
    NOT_PRIME,
    PRIME_NUMBER
};


/*
 * Return whether a given value is a prime number or not.
 *
 * Args:
 *     n : A long integer value to be checked.
 *
 * Returns:
 *    -1 : Undefined.
 *     0 : Not a prime.
 *     1 : Prime.
 *
 */
short int is_prime(const long n)
{
    if (n < 2)
        return IMPOSSIBLE_TO_DEFINE;

    if (n < 4)
        return PRIME_NUMBER;

    if ((n % 2) == 0)
        return NOT_PRIME;

    for (size_t i = 3; i <= floor(sqrt((double) n)); i += 2)
    {
        if ((n % i) == 0)
            return NOT_PRIME;
    }

    return PRIME_NUMBER;
}


/*
 * Return the next prime after n, or n if n is prime.
 *
 * Args:
 *     n : A long integer value.
 *
 * Returns:
 *     n : The next prime.
 */
long next_prime(long n)
{
    while (is_prime(n) != PRIME_NUMBER)
        n++;

    return n;
}
