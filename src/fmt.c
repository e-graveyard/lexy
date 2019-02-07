/*

   __           _
  / _|         | |
 | |_ _ __ ___ | |_   ___
 |  _| '_ ` _ \| __| / __|
 | | | | | | | | |_ | (__
 |_| |_| |_| |_|\__(_)___|

 fmt.c: Format checking

 ---------------------------------------------------------------------

 Copyright 2018-2019 Caian R. Ertl <hi@caian.org>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "fmt.h"


/**
 * strequ - String equality
 *
 * Compares the equality of two string values.
 */
unsigned short int
strequ(const char* ref, const char* txt)
{
    return (strncmp(txt, ref, strlen(txt)) == 0);
}


/**
 * isfint - Is float integer
 *
 * Checks if a given float number is a round, integer number.
 */
unsigned short int
isfint(const float f)
{
    return (fabsf(roundf(f) - f) <= INTEGER_FLOAT_EPSILON);
}


/**
 * psout - Print to stdout
 *
 * Prints using "fputs", passing the stdout as the default file.
 */
void
psout(const char* txt)
{
    fputs(txt, stdout);
}
