#ifndef LEXY_FMT
#define LEXY_FMT


/* maximum tolerance value for what constitutes a "whole number" */
#define INTEGER_FLOAT_EPSILON 0.000001

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

/* ----- */
#define COLOURED_TXT(must_apply, colour, text, ...) \
    if (must_apply) { \
        printf(colour text ANSI_RESET, __VA_ARGS__); \
    } else { \
        printf(text, __VA_ARGS__); \
    }

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

/* ----- */
unsigned short int strequ(const char* ref, const char* txt);
unsigned short int isvint(double f);
void psout(const char* txt);

#endif
