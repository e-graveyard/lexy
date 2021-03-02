#ifndef LEXY_TYPE
#define LEXY_TYPE

#include <stdlib.h>


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
    LVAL_FUN,
    LVAL_NUM,
    LVAL_STR,
    LVAL_ERR,
    LVAL_SYM,
    LVAL_SEXPR,
    LVAL_QEXPR
}
ltype_E;


/* ... */
typedef enum lcond
{
    LVAL_UNSET,
    LVAL_CONSTANT,
    LVAL_DYNAMIC
}
lcond_E;


/* ... */
struct lbtin_meta_S
{
    char* name;
    char* description;
};


/* representation of a value (number, sexpr, qexpr...) */
struct lval_S
{
    int type;

    char* error;
    char* symbol;
    char* string;
    double number;
    lcond_E condition;

    lval_T* body;
    lval_T* formals;
    lenv_T* environ;
    lbtin builtin;
    lbtin_meta_T* btin_meta;

    size_t counter;
    lval_T** cell;
};


/* representation of an environment */
struct lenv_S
{
    size_t counter;

    char** symbols;
    lval_T** values;
    lenv_T* parent;
};

#endif
