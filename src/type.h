#ifndef TMUL_TYPE
#define TMUL_TYPE

#include <stdlib.h>


// TL function error
#define TLERR_NOT_A_FUNC  "first element is not a function"
#define TLERR_BAD_NUM     "invalid number"
#define TLERR_DIV_ZERO    "division by zero"
#define TLERR_UNBOUND_SYM "unbound symbol '%s'"

// ...
struct tlval_S;
struct tlenv_S;
typedef struct tlval_S tlval_T;
typedef struct tlenv_S tlenv_T;


// Function pointer definition
typedef tlval_T* (*tlbtin)(tlenv_T*, tlval_T*);


// Representation of a value (number, sexpr, qexpr...)
struct tlval_S
{
    int type;

    char* error;
    char* symbol;
    float number;

    tlval_T* body;
    tlval_T* formals;
    tlenv_T* environ;
    tlbtin builtin;

    size_t counter;
    tlval_T** cell;
};


// Representation of an environment
struct tlenv_S
{
    size_t counter;

    char** symbols;
    tlval_T** values;
    tlenv_T* parent;
};


// Enumeration of TL value types
typedef enum tltype
{
    TLVAL_FUN,
    TLVAL_NUM,
    TLVAL_ERR,
    TLVAL_SYM,
    TLVAL_SEXPR,
    TLVAL_QEXPR
}
tltype_E;

#endif
