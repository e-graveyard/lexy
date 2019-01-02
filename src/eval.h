#include "mpc.h"

#define TLERR_BAD_OP      "Invalid operator"
#define TLERR_BAD_NUM     "Invalid number"
#define TLERR_DIV_ZERO    "Division by zero"
#define TLERR_MISSING_SYM "S-expression does not start with symbol"
#define TLERR_NON_NUM     "Cannot operate on non-number"

typedef enum tltype
{
    TLVAL_NUM,
    TLVAL_ERR,
    TLVAL_SYM,
    TLVAL_SEXPR
}
tltype_E;

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

void tlval_del(tlval_T* v);
tlval_T* tlval_add(tlval_T* v, tlval_T* x);
tlval_T* tlval_err(char* e);
tlval_T* tlval_eval(tlval_T* t);
tlval_T* tlval_eval_sexpr(tlval_T* t);
tlval_T* tlval_num(float n);
tlval_T* tlval_pop(tlval_T* t, int i);
tlval_T* tlval_read(mpc_ast_t* t);
tlval_T* tlval_read_num(mpc_ast_t* t);
tlval_T* tlval_sexpr(void);
tlval_T* tlval_sym(char* s);
tlval_T* tlval_take(tlval_T* t, int i);
tlval_T* builtin_op(tlval_T* t, char* op);
