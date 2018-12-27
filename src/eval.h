#include <stdbool.h>
#include <math.h>
#include "mpc.h"

bool equals(char* ref, char* txt);
float eval_op(char* op, float x, float y);
float eval(mpc_ast_t* t);
