#include <math.h>
#include "eval.h"

tlval_T* tlval_eval_sexpr(tlval_T* t);

bool equals(char* ref, char* txt)
{
    int len = (unsigned)strlen(txt);
    return strncmp(ref, txt, len) == 0;
}

tlval_T* tlval_num(float n)
{
    tlval_T* v = malloc(sizeof(struct tlval_S));
    v->type = TLVAL_NUM;
    v->number = n;

    return v;
}

tlval_T* tlval_err(char* e)
{
    tlval_T* v = malloc(sizeof(struct tlval_S));
    v->type = TLVAL_ERR;
    v->error = malloc(strlen(e) + 1);
    strcpy(v->error, e);

    return v;
}

tlval_T* tlval_sym(char* s)
{
    tlval_T* v = malloc(sizeof(struct tlval_S));
    v->type = TLVAL_SYM;
    v->symbol = malloc(strlen(s) + 1);
    strcpy(v->symbol, s);

    return v;
}

tlval_T* tlval_sexpr(void)
{
    tlval_T* v = malloc(sizeof(struct tlval_S));
    v->type = TLVAL_SEXPR;
    v->counter = 0;
    v->cell = NULL;

    return v;
}

void tlval_del(tlval_T* v)
{
    switch(v->type)
    {
        case TLVAL_NUM: break;

        case TLVAL_ERR:
            free(v->error);
            break;

        case TLVAL_SYM:
            free(v->symbol);
            break;

        case TLVAL_SEXPR:
            for(int i = 0; i < v->counter; i++)
                tlval_del(v->cell[i]);

            free(v->cell);
            break;
    }

    free(v);
}

tlval_T* tlval_read(mpc_ast_t* t)
{
    if(strstr(t->tag, "numb"))
        return tlval_read_num(t);

    if(strstr(t->tag, "symb"))
        return tlval_sym(t->contents);

    tlval_T* x = NULL;
    if(equals(t->tag, ">") || strstr(t->tag, "sexp"))
        x = tlval_sexpr();

    for(int i = 0; i < t->children_num; i++)
    {
        if(equals(t->children[i]->contents, "(") ||
           equals(t->children[i]->contents, ")") ||
           equals(t->children[i]->tag, "regex")) continue;

        x = tlval_add(x, tlval_read(t->children[i]));
    }

    return x;
}

tlval_T* tlval_read_num(mpc_ast_t* t)
{
    errno = 0;
    float f = strtof(t->contents, NULL);

    return (errno != ERANGE)
        ? tlval_num(f)
        : tlval_err(TLERR_BAD_NUM);
}

tlval_T* tlval_add(tlval_T* v, tlval_T* x)
{
    v->counter++;
    v->cell = realloc(v->cell, sizeof(struct tlval_S) * v->counter);
    v->cell[v->counter - 1] = x;

    return v;
}

tlval_T* tlval_pop(tlval_T* t, int i)
{
    tlval_T* v = t->cell[i];

    memmove(&t->cell[i], &t->cell[i + 1], (sizeof(tlval_T*) * t->counter));

    t->counter--;
    t->cell = realloc(t->cell, (sizeof(tlval_T*) * t->counter));

    return v;
}

tlval_T* tlval_take(tlval_T* t, int i)
{
    tlval_T* v = tlval_pop(t, i);
    tlval_del(t);

    return v;
}

tlval_T* builtin_op(tlval_T* t, char* op)
{
    for(int i = 0; i < t->counter; i++)
    {
        if(t->cell[i]->type != TLVAL_NUM)
        {
            tlval_del(t);
            return tlval_err(TLERR_NON_NUM);
        }
    }

    tlval_T* x = tlval_pop(t, 0);

    if(equals(op, "-") && t->counter == 0)
        x->number = -x->number;

    while(t->counter > 0)
    {
        tlval_T* y = tlval_pop(t, 0);

        if(equals(op, "+") || equals(op, "add"))
            x->number += y->number;

        if(equals(op, "-") || equals(op, "sub"))
            x->number -= y->number;

        if(equals(op, "*") || equals(op, "mul"))
            x->number *= y->number;

        if(equals(op, "/") || equals(op, "div"))
        {
            if(y->number == 0)
            {
                tlval_del(x);
                tlval_del(y);

                x = tlval_err(TLERR_DIV_ZERO);
                break;
            }

            x->number /= y->number;
        }

        if(equals(op, "%") || equals(op, "mod"))
            x->number = fmodf(x->number, y->number);

        if(equals(op, "^") || equals(op, "pow"))
            x->number = pow(x->number, y->number);

        if(equals(op, "min"))
        {
            x->number = (x->number > y->number)
                ? y->number
                : x->number;
        }

        if(equals(op, "max"))
        {
            x->number = (x->number > y->number)
                ? x->number
                : y->number;
        }

        tlval_del(y);
    }

    tlval_del(t);
    return x;
}

tlval_T* tlval_eval(tlval_T* t)
{
    if(t->type == TLVAL_SEXPR)
        return tlval_eval_sexpr(t);

    return t;
}

tlval_T* tlval_eval_sexpr(tlval_T* t)
{
    for(int i = 0; i < t->counter; i++)
        t->cell[i] = tlval_eval(t->cell[i]);

    for(int i = 0; i < t->counter; i++)
    {
        if(t->cell[i]->type == TLVAL_ERR)
            return tlval_take(t, i);
    }

    if(t->counter == 0)
        return t;

    if(t->counter == 1)
        return tlval_take(t, 0);

    tlval_T* h = tlval_pop(t, 0);
    if(h->type != TLVAL_SYM)
    {
        tlval_del(t);
        tlval_del(h);

        return tlval_err(TLERR_MISSING_SYM);
    }

    tlval_T* res = builtin_op(t, h->symbol);
    tlval_del(h);

    return res;
}
