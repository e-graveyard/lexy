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


/*
 * ---------- VALUE ASSERTION MACROS ----------
 */


#define TLASSERT(args, cond, fmt, ...) \
    if(!cond) { \
        tlval_T* err = tlval_err(fmt, ##__VA_ARGS__); \
        tlval_del(args); \
        return err; \
    }


#define TLASSERT_TYPE(fname, args, index, expect) \
    TLASSERT(args, (args->cell[index]->type == expect), \
            "function '%s' has taken an incorrect type at argument %i. " \
            "Got '%s', expected '%s'", \
            fname, (index + 1), tltype_nrepr(args->cell[index]->type), tltype_nrepr(expect))


#define TLASSERT_NUM(fname, args, num) \
    TLASSERT(args, (args->counter == num), \
            "function '%s' has taken an incorrect number of arguments. " \
            "Got %i, expected %i", \
            fname, args->counter, num);


#define TLASSERT_NOT_EMPTY(fname, args, index) \
    TLASSERT(args, (args->cell[index]->counter != 0), \
            "function '%s' has taken nil value for argument %i", \
            fname, index);


// TL function error
#define TLERR_NOT_A_FUNC  "first element is not a function"
#define TLERR_BAD_NUM     "invalid number"
#define TLERR_DIV_ZERO    "division by zero"
#define TLERR_UNBOUND_SYM "unbound symbol '%s'"


/*
 * ---------- PROTOTYPES ----------
 */


// built-in functions
tlval_T* btinfn_add    (tlenv_T* env, tlval_T* args);
tlval_T* btinfn_div    (tlenv_T* env, tlval_T* args);
tlval_T* btinfn_eval   (tlenv_T* env, tlval_T* qexpr);
tlval_T* btinfn_head   (tlenv_T* env, tlval_T* qexpr);
tlval_T* btinfn_join   (tlenv_T* env, tlval_T* qexprv);
tlval_T* btinfn_let    (tlenv_T* env, tlval_T* qexpr);
tlval_T* btinfn_list   (tlenv_T* env, tlval_T* sexpr);
tlval_T* btinfn_max    (tlenv_T* env, tlval_T* args);
tlval_T* btinfn_min    (tlenv_T* env, tlval_T* args);
tlval_T* btinfn_mod    (tlenv_T* env, tlval_T* args);
tlval_T* btinfn_mul    (tlenv_T* env, tlval_T* args);
tlval_T* btinfn_pow    (tlenv_T* env, tlval_T* args);
tlval_T* btinfn_sqrt   (tlenv_T* env, tlval_T* args);
tlval_T* btinfn_sub    (tlenv_T* env, tlval_T* args);
tlval_T* btinfn_tail   (tlenv_T* env, tlval_T* qexpr);
tlval_T* builtin_numop (tlenv_T* env, tlval_T* args, char* op);
void     tlenv_incb    (tlenv_T* env, char* name, tlbtin func);
void     tlenv_del     (tlenv_T* e);
tlval_T* tlenv_get     (tlenv_T* env, tlval_T* val);
void     tlenv_init    (tlenv_T* env);
tlenv_T* tlenv_new     (void);
void     tlenv_put     (tlenv_T* env, tlval_T* var, tlval_T* value);
char*    tltype_nrepr  (int type);
tlval_T* tlval_add     (tlval_T* v, tlval_T* x);
tlval_T* tlval_copy    (tlval_T* val);
void     tlval_del     (tlval_T* v);
tlval_T* tlval_err     (char* fmt, ...);
tlval_T* tlval_eval    (tlenv_T* env, tlval_T* value);
tlval_T* tlval_evsexp  (tlenv_T* env, tlval_T* val);
tlval_T* tlval_fun     (tlbtin func);
tlval_T* tlval_join    (tlval_T* x, tlval_T* y);
tlval_T* tlval_num     (float n);
tlval_T* tlval_pop     (tlval_T* t, int i);
tlval_T* tlval_qexpr   (void);
tlval_T* tlval_read    (mpc_ast_t* t);
tlval_T* tlval_rnum    (mpc_ast_t* t);
tlval_T* tlval_sexpr   (void);
tlval_T* tlval_sym     (char* s);
tlval_T* tlval_take    (tlval_T* t, int i);


/*
 * ---------- TYPE REPRESENTATION ----------
 */


/**
 * tltype_nrepr - TL type name representation
 */
char* tltype_nrepr(int type)
{
    switch(type)
    {
        case TLVAL_FUN:
            return "Function";
            break;

        case TLVAL_NUM:
            return "Number";
            break;

        case TLVAL_ERR:
            return "Error";
            break;

        case TLVAL_SYM:
            return "Symbol";
            break;

        case TLVAL_SEXPR:
            return "S-Expression";
            break;

        case TLVAL_QEXPR:
            return "Q-Expression";
            break;

        default:
            return "Undefined";
            break;
    }
}


/*
 * ---------- VALUE CONSTRUCTORS ----------
 */


/**
 * tlval_fun - TL function representation
 *
 * Constructs a pointer to a new TL function representation.
 */
tlval_T* tlval_fun(tlbtin func)
{
    tlval_T* v = malloc(sizeof(struct tlval_S));
    v->type = TLVAL_FUN;
    v->builtin = func;

    return v;
}


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
tlval_T* tlval_err(char* fmt, ...)
{
    tlval_T* v = malloc(sizeof(struct tlval_S));
    v->type = TLVAL_ERR;

    va_list va;
    va_start(va, fmt);

    v->error = malloc(ERROR_MESSAGE_BYTE_LENGTH);
    vsnprintf(v->error, (ERROR_MESSAGE_BYTE_LENGTH - 1), fmt, va);

    v->error = realloc(v->error, strlen(v->error) + 1);
    va_end(va);

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
 * ---------- ENVIRONMENT CONSTRUCTORS ----------
 */


/**
 * tlenv_new - TL environment creation
 *
 * Constructs a pointer to a new TL environment representation.
 */
tlenv_T* tlenv_new(void)
{
    tlenv_T* e = malloc(sizeof(struct tlenv_S));
    e->counter = 0;
    e->symbols = NULL;
    e->values = NULL;

    return e;
}


/*
 * ---------- ENVIRONMENT OPERATIONS ----------
 */


void tlenv_init(tlenv_T* env)
{
    tlenv_incb(env, "+",   btinfn_add);
    tlenv_incb(env, "add", btinfn_add);
    tlenv_incb(env, "-",   btinfn_sub);
    tlenv_incb(env, "sub", btinfn_sub);
    tlenv_incb(env, "*",   btinfn_mul);
    tlenv_incb(env, "mul", btinfn_mul);
    tlenv_incb(env, "/",   btinfn_div);
    tlenv_incb(env, "div", btinfn_div);
    tlenv_incb(env, "%",   btinfn_mod);
    tlenv_incb(env, "mod", btinfn_mod);
    tlenv_incb(env, "^",   btinfn_pow);
    tlenv_incb(env, "pow", btinfn_pow);
    tlenv_incb(env, "max", btinfn_max);
    tlenv_incb(env, "min", btinfn_min);
    tlenv_incb(env, "sqrt", btinfn_sqrt);

    tlenv_incb(env, "let", btinfn_let);
    tlenv_incb(env, "head", btinfn_head);
    tlenv_incb(env, "tail", btinfn_tail);
    tlenv_incb(env, "list", btinfn_list);
    tlenv_incb(env, "join", btinfn_join);
    tlenv_incb(env, "eval", btinfn_eval);
}


/**
 * tlenv_incbin - TL environment include built-in
 */
void tlenv_incb(tlenv_T* env, char* fname, tlbtin fref)
{
    tlval_T* symb = tlval_sym(fname);
    tlval_T* func = tlval_fun(fref);
    tlenv_put(env, symb, func);

    tlval_del(symb);
    tlval_del(func);
}


/**
 * tlenv_del - Environment creation
 */
void tlenv_del(tlenv_T* e)
{
    for(int i = 0; i < e->counter; i++)
    {
        free(e->symbols[i]);
        tlval_del(e->values[i]);
    }

    free(e->symbols);
    free(e->values);
    free(e);
}


/**
 * tlenv_put - Put variable to environment
 */
void tlenv_put(tlenv_T* env, tlval_T* var, tlval_T* value)
{
    for(int i = 0; i < env->counter; i++)
    {
        if(strequ(env->symbols[i], var->symbol))
        {
            tlval_del(env->values[i]);
            env->values[i] = tlval_copy(value);

            return;
        }
    }

    env->counter++;
    env->values = realloc(env->values, (sizeof(tlval_T*) * env->counter));
    env->symbols = realloc(env->symbols, (sizeof(char*) * env->counter));

    env->values[env->counter - 1] = tlval_copy(value);
    env->symbols[env->counter - 1] = malloc(strlen(var->symbol) + 1);

    strcpy(env->symbols[env->counter - 1], var->symbol);
}


/**
 * tlenv_get - Get from environment
 *
 * Takes an environment and returns a given expression if it exists.
 */
tlval_T* tlenv_get(tlenv_T* env, tlval_T* val)
{
    for(int i = 0; i < env->counter; i++)
    {
        if(strequ(val->symbol, env->symbols[i]))
            return tlval_copy(env->values[i]);
    }

    return tlval_err(TLERR_UNBOUND_SYM, val->symbol);
}


/*
 * ---------- PARSED REPRESENTATION ----------
 */


/**
 * tlval_read - TL value reading
 */
tlval_T* tlval_read(mpc_ast_t* t)
{
    if(strstr(t->tag, "number"))
        return tlval_rnum(t);

    if(strstr(t->tag, "symbol"))
        return tlval_sym(t->contents);

    tlval_T* x = NULL;
    if(strequ(t->tag, ">") || strstr(t->tag, "sexpr"))
        x = tlval_sexpr();

    if(strstr(t->tag, "qexpr"))
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
 * tlval_rnum - TL numeric value reading
 */
tlval_T* tlval_rnum(mpc_ast_t* t)
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
        case TLVAL_FUN: break;
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
 * tlval_copy - TL value copying
 *
 * Takes an expression, copy it's content to a new memory location and returns it.
 */
tlval_T* tlval_copy(tlval_T* val)
{
    tlval_T* nval = malloc(sizeof(struct tlval_S));
    nval->type = val->type;

    switch(val->type)
    {
        case TLVAL_FUN:
            nval->builtin = val->builtin;
            break;

        case TLVAL_NUM:
            nval->number = val->number;
            break;

        case TLVAL_ERR:
            nval->error = malloc(strlen(val->error) + 1);
            strcpy(nval->error, val->error);
            break;

        case TLVAL_SYM:
            nval->symbol = malloc(strlen(val->symbol) + 1);
            strcpy(nval->symbol, val->symbol);
            break;

        case TLVAL_SEXPR:
        case TLVAL_QEXPR:
            nval->counter = val->counter;
            nval->cell = malloc(sizeof(struct tlval_S) * nval->counter);

            for(int i = 0; i < nval->counter; i++)
                nval->cell[i] = tlval_copy(val->cell[i]);
            break;
    }

    return nval;
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
 * ---------- EXPRESSION EVALUATION ----------
 */


/**
 * tlval_eval - TL value evaluation
 */
tlval_T* tlval_eval(tlenv_T* env, tlval_T* value)
{
    if(value->type == TLVAL_SYM)
    {
        tlval_T* nval = tlenv_get(env, value);
        tlval_del(value);

        return nval;
    }

    if(value->type == TLVAL_SEXPR)
        return tlval_evsexp(env, value);

    return value;
}


/**
 * tlval_evsexp - TL S-Expression evaluation
 */
tlval_T* tlval_evsexp(tlenv_T* env, tlval_T* val)
{
    for(int i = 0; i < val->counter; i++)
        val->cell[i] = tlval_eval(env, val->cell[i]);

    for(int i = 0; i < val->counter; i++)
    {
        if(val->cell[i]->type == TLVAL_ERR)
            return tlval_take(val, i);
    }

    if(val->counter == 0)
        return val;

    if(val->counter == 1)
        return tlval_take(val, 0);

    tlval_T* element = tlval_pop(val, 0);
    if(element->type != TLVAL_FUN)
    {
        tlval_T* err = tlval_err(
                    "S-Expression start with incorrect type. "
                    "Got '%s', expected '%s'.",
                    tltype_nrepr(element->type), tltype_nrepr(TLVAL_FUN));

        tlval_del(val);
        tlval_del(element);

        return err;
    }

    tlval_T* res = element->builtin(env, val);
    tlval_del(element);

    return res;
}


/*
 * ---------- BUILT IN FUNCTIONS ----------
 */


/**
 * builtin_numop - Built-in numeric operations
 */
tlval_T* builtin_numop(tlenv_T* env, tlval_T* args, char* op)
{
    for(int i = 0; i < args->counter; i++)
        TLASSERT_TYPE(op, args, i, TLVAL_NUM);

    tlval_T* xval = tlval_pop(args, 0);

    if(strequ(op, "-") && xval->counter == 0)
        xval->number = -xval->number;

    while(args->counter > 0)
    {
        tlval_T* yval = tlval_pop(args, 0);

        if(strequ(op, "add"))
            xval->number += yval->number;

        if(strequ(op, "sub"))
            xval->number -= yval->number;

        if(strequ(op, "mul"))
            xval->number *= yval->number;

        if(strequ(op, "div"))
        {
            if(yval->number == 0)
            {
                tlval_del(xval);
                tlval_del(yval);

                xval = tlval_err(TLERR_DIV_ZERO);
                break;
            }

            xval->number /= yval->number;
        }

        if(strequ(op, "mod"))
            xval->number = fmodf(xval->number, yval->number);

        if(strequ(op, "pow"))
            xval->number = powf(xval->number, yval->number);

        if(strequ(op, "min"))
        {
            xval->number = (xval->number > yval->number)
                ? yval->number
                : xval->number;
        }

        if(strequ(op, "max"))
        {
            xval->number = (xval->number > yval->number)
                ? xval->number
                : yval->number;
        }

        tlval_del(yval);
    }

    tlval_del(args);
    return xval;
}


/**
 * btinfn_add - "add" built-in function
 */
tlval_T* btinfn_add(tlenv_T* env, tlval_T* args)
{
    return builtin_numop(env, args, "add");
}


/**
 * btinfn_sub - "sub" built-in function
 */
tlval_T* btinfn_sub(tlenv_T* env, tlval_T* args)
{
    return builtin_numop(env, args, "sub");
}


/**
 * btinfn_mul - "mul" built-in function
 */
tlval_T* btinfn_mul(tlenv_T* env, tlval_T* args)
{
    return builtin_numop(env, args, "mul");
}


/**
 * btinfn_div - "div" built-in function
 */
tlval_T* btinfn_div(tlenv_T* env, tlval_T* args)
{
    return builtin_numop(env, args, "div");
}


/**
 * btinfn_mod - "mod" built-in function
 */
tlval_T* btinfn_mod(tlenv_T* env, tlval_T* args)
{
    return builtin_numop(env, args, "mod");
}


/**
 * btinfn_pow - "pow" built-in function
 */
tlval_T* btinfn_pow(tlenv_T* env, tlval_T* args)
{
    return builtin_numop(env, args, "pow");
}


/**
 * btinfn_max - "max" built-in function
 */
tlval_T* btinfn_max(tlenv_T* env, tlval_T* args)
{
    return builtin_numop(env, args, "max");
}


/**
 * btinfn_min - "min" built-in function
 */
tlval_T* btinfn_min(tlenv_T* env, tlval_T* args)
{
    return builtin_numop(env, args, "min");
}


/**
 * btinfn_sqrt - "sqrt" built-in function
 */
tlval_T* btinfn_sqrt(tlenv_T* env, tlval_T* args)
{
    TLASSERT_NUM("sqrt", args, 1);
    TLASSERT_TYPE("sqrt", args, 0, TLVAL_NUM);

    tlval_T* val = tlval_pop(args, 0);
    val->number = sqrt(val->number);

    tlval_del(args);
    return val;
}


/**
 * btinfn_head - "head" built-in function
 *
 * Takes a Q-Expression and returns the first element of it.
 */
tlval_T* btinfn_head(tlenv_T* env, tlval_T* qexpr)
{
    TLASSERT_NUM("head", qexpr, 1);
    TLASSERT_TYPE("head", qexpr, 0, TLVAL_QEXPR);
    TLASSERT_NOT_EMPTY("head", qexpr, 0);

    tlval_T* val = tlval_take(qexpr, 0);
    while(val->counter > 1)
        tlval_del(tlval_pop(val, 1));

    return val;
}


/**
 * btinfn_tail - "tail" built-in function
 *
 * Takes a Q-Expression and return it minus the first element.
 */
tlval_T* btinfn_tail(tlenv_T* env, tlval_T* qexpr)
{
    TLASSERT_NUM("tail", qexpr, 1);
    TLASSERT_TYPE("tail", qexpr, 0, TLVAL_QEXPR);
    TLASSERT_NOT_EMPTY("tail", qexpr, 0);

    tlval_T* val = tlval_take(qexpr, 0);
    tlval_del(tlval_pop(val, 0));

    return val;
}


/**
 * btinfn_list - "list" built-in function
 *
 * Takes a S-Expression and converts it to a Q-Expression.
 */
tlval_T* btinfn_list(tlenv_T* env, tlval_T* sexpr)
{
    sexpr->type = TLVAL_QEXPR;
    return sexpr;
}


/**
 * btinfn_join - "join" built-in function
 *
 * Takes a Q-Expression and joins all of its arguments.
 */
tlval_T* btinfn_join(tlenv_T* env, tlval_T* qexprv)
{
    for(int i = 0; i < qexprv->counter; i++)
    {
        TLASSERT_TYPE("join", qexprv, i, TLVAL_QEXPR);
    }

    tlval_T* nexpr = tlval_pop(qexprv, 0);
    while(qexprv->counter)
        nexpr = tlval_join(nexpr, tlval_pop(qexprv, 0));

    tlval_del(qexprv);
    return nexpr;
}


/**
 * btinfn_eval - "eval" built-in function
 *
 * Takes a Q-Expression and evaluates it as a S-Expression.
 */
tlval_T* btinfn_eval(tlenv_T* env, tlval_T* qexpr)
{
    TLASSERT_NUM("eval", qexpr, 1);
    TLASSERT_TYPE("eval", qexpr, 0, TLVAL_QEXPR);

    tlval_T* nexpr = tlval_take(qexpr, 0);
    nexpr->type = TLVAL_SEXPR;

    return tlval_eval(env, nexpr);
}


/**
 * btinfn_eval - "eval" built-in function
 *
 * Takes a Q-Expression and evaluates it as a S-Expression.
 */
tlval_T* btinfn_let(tlenv_T* env, tlval_T* qexpr)
{
    TLASSERT_TYPE("let", qexpr, 0, TLVAL_QEXPR);

    tlval_T* symbols = qexpr->cell[0];

    for(int i = 0; i < symbols->counter; i++)
    {
        TLASSERT(qexpr, (symbols->cell[i]->type == TLVAL_SYM),
                "function 'let' cannot define non-symbol. "
                "Got '%s', expected '%s'.",
                tltype_nrepr(symbols->cell[i]->type), tltype_nrepr(TLVAL_SYM));
    }

    TLASSERT(qexpr, (symbols->counter == (qexpr->counter - 1)),
            "function 'let' has taken too many arguments. "
            "Got %i, expected %i.",
            symbols->counter, qexpr->counter);

    for(int i = 0; i < symbols->counter; i++)
        tlenv_put(env, symbols->cell[i], qexpr->cell[i + 1]);

    tlval_del(qexpr);
    return tlval_sexpr();
}
