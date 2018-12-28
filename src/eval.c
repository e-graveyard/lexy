#include "eval.h"

tlval tlval_num(float val)
{
    tlval v;
    v.type = TLVAL_NUM;
    v.number = val;

    return v;
}

tlval tlval_err(int e)
{
    tlval v;
    v.type = TLVAL_ERR;
    v.error = e;

    return v;
}

bool equals(char* ref, char* txt)
{
    int len = (unsigned)strlen(txt);
    return strncmp(ref, txt, len) == 0;
}

tlval eval_op(char* op, tlval x, tlval y)
{
    if(x.type == TLVAL_ERR) { return x; }
    if(y.type == TLVAL_ERR) { return y; }

    if(equals(op, "+") || equals(op, "add"))
    {
        return tlval_num(x.number + y.number);
    }

    if(equals(op, "-") || equals(op, "sub"))
    {
        return tlval_num(x.number - y.number);
    }

    if(equals(op, "*") || equals(op, "mul"))
    {
        return tlval_num(x.number * y.number);
    }

    if(equals(op, "/") || equals(op, "div"))
    {
        return (y.number == 0)
            ? tlval_err(LERR_DIV_ZERO)
            : tlval_num(x.number / y.number);
    }

    if(equals(op, "%") || equals(op, "mod"))
    {
        return tlval_num(fmodf(x.number, y.number));
    }

    if(equals(op, "^") || equals(op, "pow"))
    {
        return tlval_num(pow(x.number, y.number));
    }

    if(equals(op, "min"))
    {
        return (x.number > y.number)
            ? tlval_num(y.number)
            : tlval_num(x.number);
    }

    if(equals(op, "max"))
    {
        return (x.number > y.number)
            ? tlval_num(y.number)
            : tlval_num(x.number);
    }

    return tlval_err(LERR_BAD_OP);
}

tlval eval(mpc_ast_t* t)
{
    if(strstr(t->tag, "numb"))
    {
        errno = 0;
        float f = strtof(t->contents, NULL);

        return (errno != ERANGE)
            ? tlval_num(f)
            : tlval_err(LERR_BAD_NUM);
    }

    char* op = t->children[1]->contents;
    tlval x = eval(t->children[2]);

    int i = 3;
    while(strstr(t->children[i]->tag, "expr"))
    {
        x = eval_op(op, x, eval(t->children[i]));
        i++;
    }

    return x;
}
