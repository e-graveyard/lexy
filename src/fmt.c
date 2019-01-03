#include <math.h>
#include <stdio.h>
#include <string.h>

#include "fmt.h"


/**
 * strequ - String equality
 *
 * Compares the equality of two string values.
 */
int strequ(char* ref, char* txt)
{
    int len = (unsigned)strlen(txt);
    return strncmp(ref, txt, len) == 0;
}


/**
 * isfint - Is float integer
 *
 * Checks if a given float number is a round, integer number.
 */
int isfint(float f)
{
    return (fabsf(roundf(f) - f) <= INTEGER_FLOAT_EPSILON);
}


/**
 * psout - Print to stdout
 *
 * Prints using "fputs", passing the stdout as the default file.
 */
void psout(char* txt)
{
    fputs(txt, stdout);
}
