#ifndef TMUL_TYPE
#define TMUL_TYPE

#include <stdlib.h>


/* ... */
#define true 1
#define false 0
typedef unsigned short int boolean;

/* TL function error */
#define TLERR_BAD_NUM     "invalid number"
#define TLERR_DIV_ZERO    "division by zero"
#define TLERR_UNBOUND_SYM "unbound symbol '%s'"
#define TLERR_UNBOUND_VARIADIC \
    "function format invalid. Symbol '&' not followed by single symbol"


struct tlval_S;
struct tlenv_S;
typedef struct tlval_S tlval_T;
typedef struct tlenv_S tlenv_T;
typedef struct tlbtin_meta_S tlbtin_meta_T;

/* Function pointer definition */
typedef tlval_T* (*tlbtin)(tlenv_T*, tlval_T*);

/* Enumeration of TL value types */
typedef enum tltype
{
    TLVAL_FUN,
    TLVAL_NUM,
    TLVAL_STR,
    TLVAL_ERR,
    TLVAL_SYM,
    TLVAL_SEXPR,
    TLVAL_QEXPR
}
tltype_E;

/* ... */
typedef enum tlcond
{
    TLVAL_UNSET,
    TLVAL_CONSTANT,
    TLVAL_DYNAMIC
}
tlcond_E;

struct tlbtin_meta_S
{
    char* name;
    char* description;
};

/* Representation of a value (number, sexpr, qexpr...) */
struct tlval_S
{
    int type;

    char* error;
    char* symbol;
    char* string;
    float number;
    tlcond_E condition;

    tlval_T* body;
    tlval_T* formals;
    tlenv_T* environ;
    tlbtin builtin;
    tlbtin_meta_T* btin_meta;

    size_t counter;
    tlval_T** cell;
};

/* Representation of an environment */
struct tlenv_S
{
    size_t counter;

    char** symbols;
    tlval_T** values;
    tlenv_T* parent;
};

#endif
