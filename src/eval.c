#include "eval.h"

bool equals(char* ref, char* txt)
{
    int len = (unsigned)strlen(txt);
    return strncmp(ref, txt, len) == 0;
}

float eval_op(char* op, float x, float y)
{
    if(equals(op, "+") || equals(op, "add"))
        return x + y;

    if(equals(op, "-") || equals(op, "sub"))
        return x - y;

    if(equals(op, "*") || equals(op, "mul"))
        return x * y;

    if(equals(op, "/") || equals(op, "div"))
        return x / y;

    if(equals(op, "^") || equals(op, "pow"))
        return pow(x, y);

    if(equals(op, "min"))
        return (x > y) ? y : x;

    if(equals(op, "max"))
        return (x > y) ? x : y;

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
