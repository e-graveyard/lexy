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
    LEXEC_STDIN
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
