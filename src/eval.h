#include "mpc.h"

#define ERROR_MESSAGE_BYTE_LENGTH 512

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
    int counter;

    char* error;
    char* symbol;
    float number;

    tlbtin func;
    tlval_T** cell;
};


// Representation of an environment
struct tlenv_S
{
    int counter;

    char** symbols;
    tlval_T** values;
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


// ...
void     tlenv_init (tlenv_T* env);
tlenv_T* tlenv_new  (void);
void     tlval_del  (tlval_T* v);
tlval_T* tlval_read (mpc_ast_t* t);
tlval_T* tlval_eval (tlenv_T* env, tlval_T* value);
