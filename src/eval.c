/*
                  _
                 | |
   _____   ____ _| |  ___
  / _ \ \ / / _` | | / __|
 |  __/\ V / (_| | || (__
  \___| \_/ \__,_|_(_)___|

 eval.c: Expression evaluation

 */

#include <math.h>
#include "eval.h"
#include "fmt.h"

// Prototypes
tlval_T* tlval_eval_sexpr(tlval_T* t);
tlval_T* builtin(tlval_T* expr, char* func);


/*
 * ---------- CONSTRUCTORS ----------
 */


/**
 * tlval_num - TL number representation
 *
 * Constructs a pointer to a new TL number representation.
 */
tlval_T* tlval_num(float n)
{
    tlval_T* v = malloc(sizeof(struct tlval_S));
    v->type = TLVAL_NUM;
    v->number = n;

    return v;
}


/**
 * tlval_err - TL error representation
 *
 * Constructs a pointer to a new TL error representation.
 */
tlval_T* tlval_err(char* e)
{
    tlval_T* v = malloc(sizeof(struct tlval_S));
    v->type = TLVAL_ERR;
    v->error = malloc(strlen(e) + 1);
    strcpy(v->error, e);

    return v;
}


/**
 * tlval_sym - TL symbol representation
 *
 * Constructs a pointer to a new TL symbol representation.
 */
tlval_T* tlval_sym(char* s)
{
    tlval_T* v = malloc(sizeof(struct tlval_S));
    v->type = TLVAL_SYM;
    v->symbol = malloc(strlen(s) + 1);
    strcpy(v->symbol, s);

    return v;
}


/**
 * tlval_sexpr - TL S-Expression representation
 *
 * Constructs a pointer to a new TL symbolic expression representation.
 */
tlval_T* tlval_sexpr(void)
{
    tlval_T* v = malloc(sizeof(struct tlval_S));
    v->type = TLVAL_SEXPR;
    v->counter = 0;
    v->cell = NULL;

    return v;
}


/**
 * tlval_qexpr - TL Q-Expression representation
 *
 * Constructs a pointer to a new TL quoted expression representation.
 */
tlval_T* tlval_qexpr(void)
{
    tlval_T* v = malloc(sizeof(struct tlval_S));
    v->type = TLVAL_QEXPR;
    v->counter = 0;
    v->cell = NULL;

    return v;
}


/*
 * ---------- VALUE REPRESENTATION ----------
 */


/**
 * tlval_read - TL value reading
 */
tlval_T* tlval_read(mpc_ast_t* t)
{
    if(strstr(t->tag, "numb"))
        return tlval_read_num(t);

    if(strstr(t->tag, "symb"))
        return tlval_sym(t->contents);

    tlval_T* x = NULL;
    if(strequ(t->tag, ">") || strstr(t->tag, "sexp"))
        x = tlval_sexpr();

    if(strstr(t->tag, "qexp"))
        x = tlval_qexpr();

    for(int i = 0; i < t->children_num; i++)
    {
        if(strequ(t->children[i]->contents, "(") ||
           strequ(t->children[i]->contents, ")") ||
           strequ(t->children[i]->contents, "'(") ||
           strequ(t->children[i]->tag, "regex")) continue;

        x = tlval_add(x, tlval_read(t->children[i]));
    }

    return x;
}


/**
 * tlval_read_num - TL numeric value reading
 */
tlval_T* tlval_read_num(mpc_ast_t* t)
{
    errno = 0;
    float f = strtof(t->contents, NULL);

    return (errno != ERANGE)
        ? tlval_num(f)
        : tlval_err(TLERR_BAD_NUM);
}


/*
 * ---------- EXPRESSION OPERATIONS ----------
 */


/**
 * tlval_add - TL value addition
 *
 * Adds a TL value to a S-Expression construct.
 */
tlval_T* tlval_add(tlval_T* v, tlval_T* x)
{
    v->counter++;
    v->cell = realloc(v->cell, sizeof(struct tlval_S) * v->counter);
    v->cell[v->counter - 1] = x;

    return v;
}


/**
 * tlval_del - TL value deletion
 *
 * Recursively deconstructs a TL value.
 */
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
        case TLVAL_QEXPR:
            for(int i = 0; i < v->counter; i++)
                tlval_del(v->cell[i]);

            free(v->cell);
            break;
    }

    free(v);
}


/**
 * tlval_pop - TL pop value operation
 *
 * Takes a S-Expression, extracts the element at index "i" from it and returns it.
 * The "pop" operation does not delete the original input list.
 */
tlval_T* tlval_pop(tlval_T* t, int i)
{
    tlval_T* v = t->cell[i];

    memmove(&t->cell[i], &t->cell[i + 1], (sizeof(tlval_T*) * t->counter));

    t->counter--;
    t->cell = realloc(t->cell, (sizeof(tlval_T*) * t->counter));

    return v;
}


/**
 * tlval_take - TL value take operation
 *
 * Takes a S-Expression, extracts the element at index "i" from it and return it.
 * The "take" operation deletes the original input list;
 */
tlval_T* tlval_take(tlval_T* t, int i)
{
    tlval_T* v = tlval_pop(t, i);
    tlval_del(t);

    return v;
}


/**
 * tlval_join - TL value join
 *
 * Takes two expressions and joins together all of its arguments.
 */
tlval_T* tlval_join(tlval_T* x, tlval_T* y)
{
    while(y->counter)
        x = tlval_add(x, tlval_pop(y, 0));

    tlval_del(y);
    return x;
}


/*
 * ---------- EVALUATION ----------
 */


/**
 * tlval_eval - TL value evaluation
 */
tlval_T* tlval_eval(tlval_T* t)
{
    if(t->type == TLVAL_SEXPR)
        return tlval_eval_sexpr(t);

    return t;
}


/**
 * tlval_eval_sexpr - TL S-Expression evaluation
 */
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

    tlval_T* res = builtin(t, h->symbol);
    tlval_del(h);

    return res;
}


/*
 * ---------- BUILT IN FUNCTIONS ----------
 */


/**
 * builtin_op - Built in operation
 */
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

    if(strequ(op, "-") && t->counter == 0)
        x->number = -x->number;

    while(t->counter > 0)
    {
        tlval_T* y = tlval_pop(t, 0);

        if(strequ(op, "+") || strequ(op, "add"))
            x->number += y->number;

        if(strequ(op, "-") || strequ(op, "sub"))
            x->number -= y->number;

        if(strequ(op, "*") || strequ(op, "mul"))
            x->number *= y->number;

        if(strequ(op, "/") || strequ(op, "div"))
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

        if(strequ(op, "%") || strequ(op, "mod"))
            x->number = fmodf(x->number, y->number);

        if(strequ(op, "^") || strequ(op, "pow"))
            x->number = powf(x->number, y->number);

        if(strequ(op, "min"))
        {
            x->number = (x->number > y->number)
                ? y->number
                : x->number;
        }

        if(strequ(op, "max"))
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


/**
 * builtin_head - Built in "head" function
 *
 * Takes a Q-Expression and returns the first element of it.
 */
tlval_T* builtin_head(tlval_T* qexpr)
{
    TLASSERT(qexpr, (qexpr->counter == 1), TLERR_TOO_MANY_ARGS("head"));
    TLASSERT(qexpr, (qexpr->cell[0]->type == TLVAL_QEXPR), TLERR_BAD_TYPE("head"));
    TLASSERT(qexpr, (qexpr->cell[0]->counter != 0), TLERR_EMPTY_QEXPR("head"));

    tlval_T* val = tlval_take(qexpr, 0);
    while(val->counter > 1)
        tlval_del(tlval_pop(val, 1));

    return val;
}


/**
 * builtin_tail - Built in "tail" function
 *
 * Takes a Q-Expression and return it minus the first element.
 */
tlval_T* builtin_tail(tlval_T* qexpr)
{
    TLASSERT(qexpr, (qexpr->counter == 1), TLERR_TOO_MANY_ARGS("tail"));
    TLASSERT(qexpr, (qexpr->cell[0]->type == TLVAL_QEXPR), TLERR_BAD_TYPE("tail"));
    TLASSERT(qexpr, (qexpr->cell[0]->counter != 0), TLERR_EMPTY_QEXPR("tail"));

    tlval_T* val = tlval_take(qexpr, 0);
    tlval_del(tlval_pop(val, 0));

    return val;
}


/**
 * builtin_list - Built in "list" function
 *
 * Takes a S-Expression and converts it to a Q-Expression.
 */
tlval_T* builtin_list(tlval_T* sexpr)
{
    sexpr->type = TLVAL_QEXPR;
    return sexpr;
}


/**
 * builtin_join - Built in "join" function
 *
 * Takes a Q-Expression and joins all of its arguments.
 */
tlval_T* builtin_join(tlval_T* qexprv)
{
    for(int i = 0; i < qexprv->counter; i++)
        TLASSERT(qexprv, (qexprv->cell[i]->type == TLVAL_QEXPR), TLERR_BAD_TYPE("join"));

    tlval_T* nexpr = tlval_pop(qexprv, 0);
    while(qexprv->counter)
        nexpr = tlval_join(nexpr, tlval_pop(qexprv, 0));

    tlval_del(qexprv);
    return nexpr;
}


/**
 * builtin_eval - Built in "eval" function
 *
 * Takes a Q-Expression and evaluates it as a S-Expression.
 */
tlval_T* builtin_eval(tlval_T* qexpr)
{
    TLASSERT(qexpr, (qexpr->counter == 1), TLERR_TOO_MANY_ARGS("eval"));
    TLASSERT(qexpr, (qexpr->cell[0]->type == TLVAL_QEXPR), TLERR_BAD_TYPE("eval"));

    tlval_T* nexpr = tlval_take(qexpr, 0);
    nexpr->type = TLVAL_SEXPR;

    return tlval_eval(nexpr);
}


tlval_T* builtin(tlval_T* expr, char* func)
{
    if(strequ(func, "head"))
        return builtin_head(expr);

    if(strequ(func, "tail"))
        return builtin_tail(expr);

    if(strequ(func, "list"))
        return builtin_list(expr);

    if(strequ(func, "join"))
        return builtin_join(expr);

    if(strequ(func, "eval"))
        return builtin_eval(expr);

    // ----

    if(strequ(func, "add") || strequ(func, "+") ||
       strequ(func, "sub") || strequ(func, "-") ||
       strequ(func, "mul") || strequ(func, "*") ||
       strequ(func, "div") || strequ(func, "/") ||
       strequ(func, "mod") || strequ(func, "%") ||
       strequ(func, "pow") || strequ(func, "^") ||
       strequ(func, "min") || strequ(func, "max"))
        return builtin_op(expr, func);

    tlval_del(expr);
    return tlval_err(TLERR_BAD_FUNC_NAME);
}
