#include "mpc.h"

// Enumeration of TL value types
typedef enum tltype
{
    TLVAL_NUM,
    TLVAL_ERR,
    TLVAL_SYM,
    TLVAL_QEXPR,
    TLVAL_SEXPR
}
tltype_E;


// Struct representation of a TL value
typedef struct tlval_S
{
    int type;
    int counter;
    char* error;
    char* symbol;
    float number;
    struct tlval_S** cell;
}
tlval_T;


// Prototypes

// --- constructors
tlval_T* tlval_num   (float n);
tlval_T* tlval_err   (char* e);
tlval_T* tlval_sym   (char* s);
tlval_T* tlval_sexpr (void);
tlval_T* tlval_qexpr (void);

// --- value operations
tlval_T* tlval_add        (tlval_T* v, tlval_T* x);
void tlval_del            (tlval_T* v);
tlval_T* tlval_pop        (tlval_T* t, int i);
tlval_T* tlval_take       (tlval_T* t, int i);
tlval_T* tlval_join       (tlval_T* x, tlval_T* y);
tlval_T* tlval_read       (mpc_ast_t* t);
tlval_T* tlval_read_num   (mpc_ast_t* t);
tlval_T* tlval_eval       (tlval_T* t);
tlval_T* tlval_eval_sexpr (tlval_T* t);

// --- built in functions
tlval_T* builtin      (tlval_T* expr, char* func);
tlval_T* builtin_op   (tlval_T* t, char* op);
tlval_T* builtin_head (tlval_T* args);
tlval_T* builtin_tail (tlval_T* args);
tlval_T* builtin_list (tlval_T* args);
tlval_T* builtin_eval (tlval_T* args);


// TL value assertion
#define TLASSERT(args, cond, err) \
    if(!cond) { tlval_del(args); return tlval_err(err); }


// TL function error
#define TLFE(fname)                "function '" fname "' has "

#define TLERR_BAD_TYPE(fname)      TLFE(fname) "an incorrect type"
#define TLERR_EMPTY_QEXPR(fname)   TLFE(fname) "taken an empty expression"
#define TLERR_TOO_MANY_ARGS(fname) TLFE(fname) "taken too many arguments"
#define TLERR_BAD_FUNC_NAME        "function has not been declared"


// Textual description of evaluation errors
#define TLERR_BAD_OP        "invalid operator"
#define TLERR_BAD_NUM       "invalid number"
#define TLERR_DIV_ZERO      "division by zero"
#define TLERR_MISSING_SYM   "S-Expression does not start with symbol"
#define TLERR_NON_NUM       "cannot operate on non-number"
