#ifndef TMUL_FMT
#define TMUL_FMT


/* maximum tolerance value for what constitutes a "whole number" */
#define INTEGER_FLOAT_EPSILON 0.000001

/* prototypes */
unsigned short int strequ(const char* ref, const char* txt);
unsigned short int isvint(double f);
void psout(const char* txt);

#endif
