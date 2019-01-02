#include <math.h>
#include <string.h>

#include "fmt.h"

int strequ(char* ref, char* txt)
{
    int len = (unsigned)strlen(txt);
    return strncmp(ref, txt, len) == 0;
}

int isfint(float f)
{
    return (fabsf(roundf(f) - f) <= 0.00001f);
}
