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

#ifndef LEXY_TYPE
#define LEXY_TYPE

#include <stdlib.h>


/* ... */
#define bool unsigned short int
#define TRUE 1
#define FALSE 0

/* ... */
#define TLERR_BAD_NUM          "invalid number\n"
#define TLERR_DIV_ZERO         "division by zero\n"
#define TLERR_UNBOUND_SYM      "unbound symbol '%s'\n"
#define TLERR_UNBOUND_VARIADIC "function format invalid. Symbol '&' not followed by single symbol\n"


struct lval_S;
struct lenv_S;
typedef struct lval_S lval_T;
typedef struct lenv_S lenv_T;
typedef struct lbtin_meta_S lbtin_meta_T;


/* function pointer definition */
typedef lval_T* (*lbtin)(lenv_T*, lval_T*);


/* enumeration of value types */
typedef enum ltype
{
    LTYPE_FUN,
    LTYPE_NUM,
    LTYPE_STR,
    LTYPE_ERR,
    LTYPE_SYM,
    LTYPE_SEXPR,
    LTYPE_QEXPR
}
ltype_E;


/* ... */
typedef enum lcond
{
    LCOND_UNSET,
    LCOND_CONSTANT,
    LCOND_DYNAMIC
}
lcond_E;


/* ... */
typedef enum lexec
{
    LEXEC_UNDEF,
    LEXEC_REPL,
    LEXEC_FILE,
    LEXEC_CLI
}
lexec_E;


/* ... */
struct lbtin_meta_S
{
    char* name;
    char* description;
};


/* representation of a value (number, sexpr, qexpr...) */
struct lval_S
{
    size_t  counter;
    lenv_T* environ;

    ltype_E type;
    lcond_E condition;

    lval_T*  body;
    lval_T*  formals;
    lval_T** cell;

    char*  error;
    char*  symbol;
    char*  string;
    double number;

    lbtin builtin;
    lbtin_meta_T* btin_meta;
};


/* representation of an environment */
struct lenv_S
{
    size_t  counter;
    lexec_E exec_type;
    lenv_T* parent;

    char**   symbols;
    lval_T** values;
};

#endif
