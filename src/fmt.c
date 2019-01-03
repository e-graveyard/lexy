#include <math.h>
#include <string.h>

#include "fmt.h"


/**
 * Compares the equality of two string values.
 */
int strequ(char* ref, char* txt)
{
    int len = (unsigned)strlen(txt);
    return strncmp(ref, txt, len) == 0;
}


/**
 * Checks if a given float number is a round, integer number.
 */
int isfint(float f)
{
    return (fabsf(roundf(f) - f) <= INTEGER_FLOAT_EPSILON);
}
