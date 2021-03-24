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

#ifndef LEXY_FMT
#define LEXY_FMT


/* maximum tolerance value for what constitutes a "whole number" */
#define INTEGER_FLOAT_EPSILON 0.000001

#ifdef LEXY_NO_COLORS
/* --- */
#define COLOURED_TXT(must_apply, colour, text, ...) \
    printf(text, __VA_ARGS__);

#define ANSI_RESET          ""
#define ANSI_COLOR_RED      ""
#define ANSI_COLOR_GREEN    ""
#define ANSI_COLOR_YELLOW   ""
#define ANSI_COLOR_BLUE     ""
#define ANSI_COLOR_MAGENTA  ""
#define ANSI_COLOR_CYAN     ""
#define ANSI_COLOR_GREY     ""
#define ANSI_STYLE_BOLD     ""
#define ANSI_STYLE_DIM      ""

#else
/* --- */
#define COLOURED_TXT(must_apply, colour, text, ...) \
    if (must_apply) { \
        printf(colour text ANSI_RESET, __VA_ARGS__); \
    } else { \
        printf(text, __VA_ARGS__); \
    }

#define ANSI_RESET          "\x1b[0m"
#define ANSI_COLOR_RED      "\x1b[31m"
#define ANSI_COLOR_GREEN    "\x1b[32m"
#define ANSI_COLOR_YELLOW   "\x1b[33m"
#define ANSI_COLOR_BLUE     "\x1b[34m"
#define ANSI_COLOR_MAGENTA  "\x1b[35m"
#define ANSI_COLOR_CYAN     "\x1b[36m"
#define ANSI_COLOR_GREY     "\x1b[90m"
#define ANSI_STYLE_BOLD     "\x1b[1m"
#define ANSI_STYLE_DIM      "\x1b[2m"
#endif


/* ... */

#define RED_TXT(must_apply, text, ...) \
    COLOURED_TXT(must_apply, ANSI_COLOR_RED, text, __VA_ARGS__);

#define GREEN_TXT(must_apply, text, ...) \
    COLOURED_TXT(must_apply, ANSI_COLOR_GREEN, text, __VA_ARGS__);

#define BLUE_TXT(must_apply, text, ...) \
    COLOURED_TXT(must_apply, ANSI_COLOR_BLUE, text, __VA_ARGS__);

#define YELLOW_TXT(must_apply, text, ...) \
    COLOURED_TXT(must_apply, ANSI_COLOR_YELLOW, text, __VA_ARGS__);

#define CYAN_TXT(must_apply, text, ...) \
    COLOURED_TXT(must_apply, ANSI_COLOR_CYAN, text, __VA_ARGS__);

#define MAGENTA_TXT(must_apply, text, ...) \
    COLOURED_TXT(must_apply, ANSI_COLOR_MAGENTA, text, __VA_ARGS__);

#define GREY_TXT(must_apply, text, ...) \
    COLOURED_TXT(must_apply, ANSI_COLOR_GREY, text, __VA_ARGS__);

#define BOLD_TXT(must_apply, text, ...) \
    COLOURED_TXT(must_apply, ANSI_STYLE_BOLD, text, __VA_ARGS__);

/* ... */
unsigned short int strequ(const char* ref, const char* txt);
unsigned short int isvint(double f);
void psout(const char* txt);

#endif
