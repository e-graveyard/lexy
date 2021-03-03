#include <string.h>

#include "env.h"

#include "builtin.h"
#include "fmt.h"
#include "type.h"


void    lval_del    (lval_T* v);
lval_T* lval_sym    (const char* s);
lval_T* lval_err    (const char* fmt, ...);
lval_T* lval_fun    (char* name, char* description, lbtin func);
lval_T* lval_copy   (lval_T* val);
lval_T* lval_sexpr  (void);


/**
 * lenv_new - TL environment creation
 *
 * Constructs a pointer to a new TL environment representation.
 */
lenv_T*
lenv_new(void)
{
    lenv_T* e = malloc(sizeof(struct lenv_S));

    e->counter = 0;
    e->symbols = NULL;
    e->values  = NULL;
    e->parent  = NULL;

    return e;
}


/**
 * lenv_incbin - TL environment include built-in
 */
void
lenv_incb(lenv_T* env, char* fname, char* fdescr, lbtin fref)
{
    lval_T* symb = lval_sym(fname);
    lval_T* func = lval_fun(fname, fdescr, fref);
    lenv_put(env, symb, func, LCOND_CONSTANT);

    lval_del(symb);
    lval_del(func);
}


void
lenv_init(lenv_T* env)
{
    /* math operations */
    lenv_incb(env, "add",  BTIN_ADD_DESCR,  btinfn_add);
    lenv_incb(env, "sub",  BTIN_SUB_DESCR,  btinfn_sub);
    lenv_incb(env, "mul",  BTIN_MUL_DESCR,  btinfn_mul);
    lenv_incb(env, "div",  BTIN_DIV_DESCR,  btinfn_div);
    lenv_incb(env, "mod",  BTIN_MOD_DESCR,  btinfn_mod);
    lenv_incb(env, "pow",  BTIN_POW_DESCR,  btinfn_pow);
    lenv_incb(env, "max",  BTIN_MAX_DESCR,  btinfn_max);
    lenv_incb(env, "min",  BTIN_MIN_DESCR,  btinfn_min);
    lenv_incb(env, "sqrt", BTIN_SQRT_DESCR, btinfn_sqrt);

    /* list operations */
    lenv_incb(env, "head", BTIN_HEAD_DESCR, btinfn_head);
    lenv_incb(env, "tail", BTIN_TAIL_DESCR, btinfn_tail);
    lenv_incb(env, "list", BTIN_LIST_DESCR, btinfn_list);
    lenv_incb(env, "join", BTIN_JOIN_DESCR, btinfn_join);

    /* logical operators */
    lenv_incb(env, "if", BTIN_IF_DESCR, btinfn_if);
    lenv_incb(env, "eq", BTIN_EQ_DESCR, btinfn_cmp_eq);
    lenv_incb(env, "ne", BTIN_NE_DESCR, btinfn_cmp_ne);
    lenv_incb(env, "gt", BTIN_GT_DESCR, btinfn_cmp_gt);
    lenv_incb(env, "ge", BTIN_GE_DESCR, btinfn_cmp_ge);
    lenv_incb(env, "lt", BTIN_LT_DESCR, btinfn_cmp_lt);
    lenv_incb(env, "le", BTIN_LE_DESCR, btinfn_cmp_le);

    /* variables declatation */
    lenv_incb(env, "let",     BTIN_LET_DESCR,     btinfn_let);
    lenv_incb(env, "letc",    BTIN_LETC_DESCR,    btinfn_letc);
    lenv_incb(env, "global",  BTIN_GLOBAL_DESCR,  btinfn_global);
    lenv_incb(env, "globalc", BTIN_GLOBALC_DESCR, btinfn_globalc);

    /* functions, output etc */
    lenv_incb(env, "use",    BTIN_USE_DESCR,    btinfn_load);
    lenv_incb(env, "eval",   BTIN_EVAL_DESCR,   btinfn_eval);
    lenv_incb(env, "lambda", BTIN_LAMBDA_DESCR, btinfn_lambda);
    lenv_incb(env, "error",  BTIN_ERROR_DESCR,  btinfn_error);
    lenv_incb(env, "print",  BTIN_PRINT_DESCR,  btinfn_print);
}


/**
 * lenv_put - Put variable to an inner environment
 */
lval_T*
lenv_put(lenv_T* env, lval_T* var, lval_T* value, lcond_E cond)
{
    for(size_t i = 0; i < env->counter; i++)
    {
        if(strequ(env->symbols[i], var->symbol))
        {
            if(env->values[i]->condition != cond)
                return lval_err("cannot reassign the variable condition");

            if(env->values[i]->condition == LCOND_CONSTANT)
                return lval_err("cannot assign to a constant variable");

            lval_del(env->values[i]);
            env->values[i] = lval_copy(value);

            return lval_sexpr();
        }
    }

    env->counter++;
    env->values  = realloc(env->values, sizeof(struct lval_S) * env->counter);
    env->symbols = realloc(env->symbols, sizeof(char*) * env->counter);

    if (value->condition == LCOND_UNSET)
        value->condition = cond;

    env->values[env->counter - 1]  = lval_copy(value);
    env->symbols[env->counter - 1] = malloc(strlen(var->symbol) + 1);

    strcpy(env->symbols[env->counter - 1], var->symbol);
    return lval_sexpr();
}


/**
 * lenv_put - Put variable to the global environment
 */
lval_T*
lenv_putg(lenv_T* env, lval_T* var, lval_T* value, lcond_E cond)
{
    while(env->parent)
        env = env->parent;

    return lenv_put(env, var, value, cond);
}


/**
 * lenv_del - Environment creation
 */
void
lenv_del(lenv_T* e)
{
    for(size_t i = 0; i < e->counter; i++)
    {
        free(e->symbols[i]);
        lval_del(e->values[i]);
    }

    free(e->symbols);
    free(e->values);
    free(e);
}


/**
 * lenv_get - Get from environment
 *
 * Takes an environment and returns a given expression if it exists.
 */
lval_T*
lenv_get(lenv_T* env, lval_T* val)
{
    for(size_t i = 0; i < env->counter; i++)
    {
        if(strequ(val->symbol, env->symbols[i]))
            return lval_copy(env->values[i]);
    }

    if(env->parent)
        return lenv_get(env->parent, val);

    return lval_err(TLERR_UNBOUND_SYM, val->symbol);
}


lenv_T*
lenv_copy(lenv_T* env)
{
    lenv_T* nenv  = malloc(sizeof(struct lenv_S));
    nenv->parent  = env->parent;
    nenv->counter = env->counter;
    nenv->symbols = malloc(sizeof(char*) * nenv->counter);
    nenv->values  = malloc(sizeof(struct lval_S) * nenv->counter);

    for(size_t i = 0; i < nenv->counter; i++)
    {
        nenv->symbols[i] = malloc(strlen(env->symbols[i]) + 1);
        strcpy(nenv->symbols[i], env->symbols[i]);

        nenv->values[i] = lval_copy(env->values[i]);
    }

    return nenv;
}
