#include "eval.h"

float eval_op(char* op, float x, float y)
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

float eval(mpc_ast_t* t)
{
    if(strstr(t->tag, "numb"))
    {
        return atof(t->contents);
    }

    char* op = t->children[1]->contents;

    float x = eval(t->children[2]);

    int i = 3;
    while(strstr(t->children[i]->tag, "expr"))
    {
        x = eval_op(op, x, eval(t->children[i]));
        i++;
    }

    return x;
}
