#include <stdbool.h>
#include <math.h>
#include "mpc.h"

enum
{
    TLVAL_NUM,
    TLVAL_ERR
};

enum
{
    LERR_DIV_ZERO,
    LERR_BAD_OP,
    LERR_BAD_NUM
};

typedef struct
{
    int type;
    int error;
    float number;
}
tlval;

bool equals(char* ref, char* txt);
tlval eval_op(char* op, tlval x, tlval y);
tlval eval(mpc_ast_t* t);
