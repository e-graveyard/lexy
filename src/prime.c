/*

   Copyright (c) 2018-2021 Caian R. Ertl <hi@caian.org>

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation
   files (the "Software"), to deal in the Software without
   restriction, including without limitation the rights to use,
   copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following
   conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
   OTHER DEALINGS IN THE SOFTWARE.

 */

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
