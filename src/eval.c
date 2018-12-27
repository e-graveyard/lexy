#include "eval.h"

long eval_op(char* op, long x, long y)
{
    if(strcmp(op, "+") == 0)
        return x + y;

    if(strcmp(op, "-") == 0)
        return x - y;

    if(strcmp(op, "*") == 0)
        return x * y;

    if(strcmp(op, "/") == 0)
        return x / y;

    return 0;
}

long eval(mpc_ast_t* t)
{
    if(strstr(t->tag, "numb"))
    {
        return atol(t->contents);
    }

    char* op = t->children[1]->contents;

    long x = eval(t->children[2]);

    int i = 3;
    while(strstr(t->children[i]->tag, "expr"))
    {
        x = eval_op(op, x, eval(t->children[i]));
        i++;
    }

    return x;
}
