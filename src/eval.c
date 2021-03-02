/*

                  _
                 | |
   _____   ____ _| |  ___
  / _ \ \ / / _` | | / __|
 |  __/\ V / (_| | || (__
  \___| \_/ \__,_|_(_)___|

 eval.c: Expression evaluation

 ---------------------------------------------------------------------

 Copyright 2018-2019 Caian R. Ertl <hi@caian.org>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

#include "eval.h"
#include "builtin.h"
#include "fmt.h"
#include "type.h"


lenv_T* lenv_copy   (lenv_T* env);
void    lenv_del    (lenv_T* e);
lval_T* lenv_get    (lenv_T* env, lval_T* val);
void    lenv_incb   (lenv_T* env, char* fname, char* fdescr, lbtin fref);
void    lenv_init   (lenv_T* env);
lenv_T* lenv_new    (void);
lval_T* lenv_put    (lenv_T* env, lval_T* var, lval_T* value, lcond_E cond);
lval_T* lenv_putg   (lenv_T* env, lval_T* var, lval_T* value, lcond_E cond);
char*   ltype_nrepr (int type);
lval_T* lval_add    (lval_T* v, lval_T* x);
lval_T* lval_new    (void);
lval_T* lval_copy   (lval_T* val);
void    lval_del    (lval_T* v);
lval_T* lval_err    (const char* fmt, ...);
lval_T* lval_eval   (lenv_T* env, lval_T* value);
lval_T* lval_evsexp (lenv_T* env, lval_T* val);
lval_T* lval_fun    (char* name, char* description, lbtin func);
lval_T* lval_join   (lval_T* x, lval_T* y);
lval_T* lval_lambda (lval_T* formals, lval_T* body);
lval_T* lval_num    (double n);
lval_T* lval_pop    (lval_T* t, size_t i);
lval_T* lval_qexpr  (void);
lval_T* lval_read   (mpc_ast_t* t);
lval_T* lval_rnum   (mpc_ast_t* t);
lval_T* lval_rstr   (mpc_ast_t* t);
lval_T* lval_sexpr  (void);
lval_T* lval_sym    (const char* s);
lval_T* lval_take   (lval_T* t, size_t i);
lval_T* lval_str    (char* s);

lval_T* btinfn_add     (lenv_T* env, lval_T* args);
lval_T* btinfn_div     (lenv_T* env, lval_T* args);
lval_T* btinfn_eval    (lenv_T* env, lval_T* qexpr);
lval_T* btinfn_global  (lenv_T* env, lval_T* qexpr);
lval_T* btinfn_globalc (lenv_T* env, lval_T* qexpr);
lval_T* btinfn_head    (lenv_T* env, lval_T* qexpr);
lval_T* btinfn_join    (lenv_T* env, lval_T* qexprv);
lval_T* btinfn_lambda  (lenv_T* env, lval_T* qexpr);
lval_T* btinfn_let     (lenv_T* env, lval_T* qexpr);
lval_T* btinfn_letc    (lenv_T* env, lval_T* qexpr);
lval_T* btinfn_list    (lenv_T* env, lval_T* sexpr);
lval_T* btinfn_max     (lenv_T* env, lval_T* args);
lval_T* btinfn_min     (lenv_T* env, lval_T* args);
lval_T* btinfn_mod     (lenv_T* env, lval_T* args);
lval_T* btinfn_mul     (lenv_T* env, lval_T* args);
lval_T* btinfn_pow     (lenv_T* env, lval_T* args);
lval_T* btinfn_sqrt    (lenv_T* env, lval_T* args);
lval_T* btinfn_sub     (lenv_T* env, lval_T* args);
lval_T* btinfn_tail    (lenv_T* env, lval_T* qexpr);
lval_T* btinfn_cmp_gt  (lenv_T* env, lval_T* args);
lval_T* btinfn_cmp_ge  (lenv_T* env, lval_T* args);
lval_T* btinfn_cmp_lt  (lenv_T* env, lval_T* args);
lval_T* btinfn_cmp_le  (lenv_T* env, lval_T* args);
lval_T* btinfn_cmp_eq  (lenv_T* env, lval_T* args);
lval_T* btinfn_cmp_ne  (lenv_T* env, lval_T* args);
lval_T* btinfn_if      (lenv_T* env, lval_T* args);


/*
 * ---------- TYPE REPRESENTATION ----------
 */


/**
 * ltype_nrepr - TL type name representation
 */
char*
ltype_nrepr(int type)
{
    switch(type)
    {
        case LVAL_FUN:   return "Function";
        case LVAL_NUM:   return "Number";
        case LVAL_STR:   return "String";
        case LVAL_ERR:   return "Error";
        case LVAL_SYM:   return "Symbol";
        case LVAL_SEXPR: return "S-Expression";
        case LVAL_QEXPR: return "Q-Expression";
        default:         return "Undefined";
    }
}


/*
 * ---------- VALUE CONSTRUCTORS ----------
 */


lval_T* lval_new(void)
{
    lval_T* v    = malloc(sizeof(struct lval_S));
    v->condition = LVAL_UNSET;

    return v;
}


/**
 * lval_fun - TL function representation
 *
 * Constructs a pointer to a new TL function representation.
 */
lval_T*
lval_fun(char* name, char* description, lbtin func)
{
    lval_T* v  = lval_new();
    v->type    = LVAL_FUN;
    v->builtin = func;

    v->btin_meta = malloc(sizeof(struct lbtin_meta_S));
    v->btin_meta->name = malloc(strlen(name) + 1);
    v->btin_meta->description = malloc(strlen(description) + 1);

    strcpy(v->btin_meta->name, name);
    strcpy(v->btin_meta->description, description);

    return v;
}


/**
 * lval_lambda - TL lambda representation
 *
 * Constructs a pointer to a new TL lambda representation.
 */
lval_T*
lval_lambda(lval_T* formals, lval_T* body)
{
    lval_T* v  = lval_new();
    v->type    = LVAL_FUN;
    v->builtin = NULL;
    v->formals = formals;
    v->body    = body;
    v->environ = lenv_new();

    return v;
}


/**
 * lval_num - TL number representation
 *
 * Constructs a pointer to a new TL number representation.
 */
lval_T*
lval_num(double n)
{
    lval_T* v = lval_new();
    v->type   = LVAL_NUM;
    v->number = n;

    return v;
}


lval_T*
lval_str(char* s)
{
    lval_T* v = lval_new();
    v->type   = LVAL_STR;
    v->string = malloc(strlen(s) + 1);

    strcpy(v->string, s);
    return v;
}


/**
 * lval_err - TL error representation
 *
 * Constructs a pointer to a new TL error representation.
 */
lval_T*
lval_err(const char* fmt, ...)
{
    lval_T* v = lval_new();
    v->type   = LVAL_ERR;

    va_list va;
    va_start(va, fmt);

    v->error = malloc(ERROR_MESSAGE_BYTE_LENGTH);
    vsnprintf(v->error, (ERROR_MESSAGE_BYTE_LENGTH - 1), fmt, va);

    v->error = realloc(v->error, strlen(v->error) + 1);
    va_end(va);

    return v;
}


/**
 * lval_sym - TL symbol representation
 *
 * Constructs a pointer to a new TL symbol representation.
 */
lval_T*
lval_sym(const char* s)
{
    lval_T* v = lval_new();
    v->type   = LVAL_SYM;
    v->symbol = malloc(strlen(s) + 1);

    strcpy(v->symbol, s);
    return v;
}


/**
 * lval_sexpr - TL S-Expression representation
 *
 * Constructs a pointer to a new TL symbolic expression representation.
 */
lval_T*
lval_sexpr(void)
{
    lval_T* v  = lval_new();
    v->type    = LVAL_SEXPR;
    v->counter = 0;
    v->cell    = NULL;

    return v;
}


/**
 * lval_qexpr - TL Q-Expression representation
 *
 * Constructs a pointer to a new TL quoted expression representation.
 */
lval_T*
lval_qexpr(void)
{
    lval_T* v  = lval_new();
    v->type    = LVAL_QEXPR;
    v->counter = 0;
    v->cell    = NULL;

    return v;
}


/*
 * ---------- ENVIRONMENT CONSTRUCTORS ----------
 */


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


/*
 * ---------- ENVIRONMENT OPERATIONS ----------
 */


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
 * lenv_incbin - TL environment include built-in
 */
void
lenv_incb(lenv_T* env, char* fname, char* fdescr, lbtin fref)
{
    lval_T* symb = lval_sym(fname);
    lval_T* func = lval_fun(fname, fdescr, fref);
    lenv_put(env, symb, func, LVAL_CONSTANT);

    lval_del(symb);
    lval_del(func);
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

            if(env->values[i]->condition == LVAL_CONSTANT)
                return lval_err("cannot assign to a constant variable");

            lval_del(env->values[i]);
            env->values[i] = lval_copy(value);

            return lval_sexpr();
        }
    }

    env->counter++;
    env->values  = realloc(env->values, sizeof(struct lval_S) * env->counter);
    env->symbols = realloc(env->symbols, sizeof(char*) * env->counter);

    if (value->condition == LVAL_UNSET)
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


/*
 * ---------- PARSED REPRESENTATION ----------
 */


/**
 * lval_read - TL value reading
 */
lval_T*
lval_read(mpc_ast_t* t)
{
    if(strstr(t->tag, "number"))
        return lval_rnum(t);

    if(strstr(t->tag, "string"))
        return lval_rstr(t);

    if(strstr(t->tag, "symbol"))
        return lval_sym(t->contents);

    lval_T* x = NULL;
    if(strequ(t->tag, ">") || strstr(t->tag, "sexpr"))
        x = lval_sexpr();

    if(strstr(t->tag, "qexpr"))
        x = lval_qexpr();

    for(int i = 0; i < t->children_num; i++)
    {
        if(strequ(t->children[i]->contents, "("))  continue;
        if(strequ(t->children[i]->contents, ")"))  continue;
        if(strequ(t->children[i]->contents, "{"))  continue;
        if(strequ(t->children[i]->contents, "}"))  continue;
        if(strequ(t->children[i]->tag, "regex"))   continue;
        if(strstr(t->children[i]->tag, "comment")) continue;

        x = lval_add(x, lval_read(t->children[i]));
    }

    return x;
}


/**
 * lval_rnum - TL numeric value reading
 */
lval_T*
lval_rnum(mpc_ast_t* t)
{
    errno = 0;
    double f = strtof(t->contents, NULL);

    return errno != ERANGE
        ? lval_num(f)
        : lval_err(TLERR_BAD_NUM);
}

lval_T*
lval_rstr(mpc_ast_t* t)
{
    t->contents[strlen(t->contents) - 1] = '\0';
    char* unescaped = malloc(strlen(t->contents + 1) + 1);
    strcpy(unescaped, t->contents + 1);

    unescaped = mpcf_unescape(unescaped);
    lval_T* str = lval_str(unescaped);

    free(unescaped);
    return str;
}


/*
 * ---------- EXPRESSION OPERATIONS ----------
 */


/**
 * lval_add - TL value addition
 *
 * Adds a TL value to a S-Expression construct.
 */
lval_T*
lval_add(lval_T* v, lval_T* x)
{
    v->counter++;
    v->cell = realloc(v->cell, sizeof(struct lval_S) * v->counter);
    v->cell[v->counter - 1] = x;

    return v;
}


/**
 * lval_del - TL value deletion
 *
 * Recursively deconstructs a TL value.
 */
void
lval_del(lval_T* v)
{
    switch(v->type)
    {
        case LVAL_NUM: break;

        case LVAL_FUN:
            if(!v->builtin)
            {
                lenv_del(v->environ);
                lval_del(v->formals);
                lval_del(v->body);
            }
            break;

        case LVAL_STR:
            free(v->string);
            break;

        case LVAL_ERR:
            free(v->error);
            break;

        case LVAL_SYM:
            free(v->symbol);
            break;

        case LVAL_SEXPR:
        case LVAL_QEXPR:
            for(size_t i = 0; i < v->counter; i++)
                lval_del(v->cell[i]);

            free(v->cell);
            break;
    }

    free(v);
}


/**
 * lval_copy - TL value copying
 *
 * Takes an expression, copy it's content to a new memory location and returns it.
 */
lval_T*
lval_copy(lval_T* val)
{
    lval_T* nval = malloc(sizeof(struct lval_S));
    nval->type = val->type;
    nval->condition = val->condition;

    switch(val->type)
    {
        case LVAL_FUN:
            if(val->builtin)
            {
                nval->builtin = val->builtin;
                nval->btin_meta = val->btin_meta;
            }
            else
            {
                nval->builtin = NULL;
                nval->environ = lenv_copy(val->environ);
                nval->formals = lval_copy(val->formals);
                nval->body = lval_copy(val->body);
            }
            break;

        case LVAL_NUM:
            nval->number = val->number;
            break;

        case LVAL_STR:
            nval->string = malloc(strlen(val->string) +1);
            strcpy(nval->string, val->string);
            break;

        case LVAL_ERR:
            nval->error = malloc(strlen(val->error) + 1);
            strcpy(nval->error, val->error);
            break;

        case LVAL_SYM:
            nval->symbol = malloc(strlen(val->symbol) + 1);
            strcpy(nval->symbol, val->symbol);
            break;

        case LVAL_SEXPR:
        case LVAL_QEXPR:
            nval->counter = val->counter;
            nval->cell = malloc(sizeof(struct lval_S) * nval->counter);

            for(size_t i = 0; i < nval->counter; i++)
                nval->cell[i] = lval_copy(val->cell[i]);

            break;
    }

    return nval;
}


/**
 * lval_pop - TL pop value operation
 *
 * Takes a S-Expression, extracts the element at index "i" from it and returns it.
 * The "pop" operation does not delete the original input list.
 */
lval_T*
lval_pop(lval_T* t, size_t i)
{
    lval_T* v = t->cell[i];

    memmove(&t->cell[i], &t->cell[i + 1], (sizeof(struct lval_S) * t->counter));

    t->counter--;
    t->cell = realloc(t->cell, (sizeof(struct lval_S) * t->counter));

    return v;
}


/**
 * lval_take - TL value take operation
 *
 * Takes a S-Expression, extracts the element at index "i" from it and return it.
 * The "take" operation deletes the original input list;
 */
lval_T*
lval_take(lval_T* t, size_t i)
{
    lval_T* v = lval_pop(t, i);
    lval_del(t);

    return v;
}


/**
 * lval_join - TL value join
 *
 * Takes two expressions and joins together all of its arguments.
 */
lval_T*
lval_join(lval_T* x, lval_T* y)
{
    while(y->counter)
        x = lval_add(x, lval_pop(y, 0));

    lval_del(y);
    return x;
}


lval_T*
lval_call(lenv_T* env, lval_T* func, lval_T* args)
{
    if(func->builtin)
        return func->builtin(env, args);

    size_t given = args->counter;
    size_t total = func->formals->counter;

    while(args->counter)
    {
        if(func->formals->counter == 0)
        {
            lval_del(args);
            return lval_err(
                "function has taken too many arguments."
                "Got %lu, expected %lu", given, total);
        }

        lval_T* symbol = lval_pop(func->formals, 0);

        if(strequ(symbol->symbol, "&"))
        {
            if(func->formals->counter != 1)
            {
                lval_del(args);
                return lval_err(TLERR_UNBOUND_VARIADIC);
            }

            lval_T* nsym = lval_pop(func->formals, 0);
            lenv_put(func->environ, nsym, btinfn_list(env, args), LVAL_DYNAMIC);

            lval_del(symbol);
            lval_del(nsym);

            break;
        }

        lval_T* value  = lval_pop(args, 0);

        lenv_put(func->environ, symbol, value, value->condition);

        lval_del(symbol);
        lval_del(value);
    }

    lval_del(args);

    if(func->formals->counter > 0 && strequ(func->formals->cell[0]->symbol, "&"))
    {
        if(func->formals->counter != 2)
            return lval_err(TLERR_UNBOUND_VARIADIC);

        lval_del(lval_pop(func->formals, 0));

        lval_T* symbol = lval_pop(func->formals, 0);
        lval_T* value  = lval_qexpr();

        lenv_put(func->environ, symbol, value, LVAL_DYNAMIC);

        lval_del(symbol);
        lval_del(value);
    }

    if(func->formals->counter == 0)
    {
        func->environ->parent = env;
        return btinfn_eval(func->environ, lval_add(lval_sexpr(), lval_copy(func->body)));
    }

    return lval_copy(func);
}


/*
 * ---------- EXPRESSION EVALUATION ----------
 */


/**
 * lval_eval - TL value evaluation
 */
lval_T*
lval_eval(lenv_T* env, lval_T* value)
{
    if(value->type == LVAL_SYM)
    {
        lval_T* nval = lenv_get(env, value);
        lval_del(value);

        return nval;
    }

    if(value->type == LVAL_SEXPR)
        return lval_evsexp(env, value);

    return value;
}


/**
 * lval_evsexp - TL S-Expression evaluation
 */
lval_T*
lval_evsexp(lenv_T* env, lval_T* val)
{
    for(size_t i = 0; i < val->counter; i++)
        val->cell[i] = lval_eval(env, val->cell[i]);

    for(size_t i = 0; i < val->counter; i++)
        if(val->cell[i]->type == LVAL_ERR)
            return lval_take(val, i);

    if(val->counter == 0)
        return val;

    if(val->counter == 1)
        return lval_eval(env, lval_take(val, 0));

    lval_T* element = lval_pop(val, 0);
    if(element->type != LVAL_FUN)
    {
        lval_T* err = lval_err(
            "S-Expression start with incorrect type. "
            "Got '%s', expected '%s'.",
            ltype_nrepr(element->type), ltype_nrepr(LVAL_FUN));

        lval_del(val);
        lval_del(element);

        return err;
    }

    lval_T* res = lval_call(env, element, val);
    lval_del(element);

    return res;
}


int lval_eq(lval_T* a, lval_T* b)
{
    if (a->type != b->type)
        return 0;

    switch(a->type)
    {
        case LVAL_NUM: return (a->number == b->number);
        case LVAL_STR: return strequ(a->string, b->string);
        case LVAL_ERR: return strequ(a->error, b->error);
        case LVAL_SYM: return strequ(a->symbol, b->symbol);

        case LVAL_FUN:
            if(a->builtin || b->builtin)
                return a->builtin == b->builtin;
            else
                return lval_eq(a->formals, b->formals) &&
                       lval_eq(a->body, b->body);

        case LVAL_QEXPR:
        case LVAL_SEXPR:
            if(a->counter != b->counter) return 0;

            for(size_t i = 0; i < a->counter; i++)
                if(!lval_eq(a->cell[i], b->cell[i])) return 0;

            return 1;
    }

    return 0;
}

void
lval_exp_print(lval_T* t, char* openc, char* closec)
{
    printf("%s", openc);
    for(size_t i = 0; i < t->counter; i++)
    {
        lval_print(t->cell[i]);

        if(i != (t->counter - 1))
            printf(" ");
    }
    printf("%s", closec);
}

void
lval_num_print(double n)
{
    if(isvint(n))
        printf("%ld", (long)round(n));
    else
        printf("%f", n);
}

void
lval_print(lval_T* t)
{
    switch(t->type)
    {
        case LVAL_FUN:
            if(t->builtin)
            {
                printf("<builtin(%s): %s>", t->btin_meta->name, t->btin_meta->description);
                return;
            }

            printf("(lambda ");
            lval_print(t->formals);
            printf(" ");
            lval_print(t->body);
            printf(")");
            break;

        case LVAL_NUM:
            lval_num_print(t->number);
            break;

        case LVAL_STR:
            printf("%s", t->string);
            break;

        case LVAL_ERR:
            printf("error: %s", t->error);
            break;

        case LVAL_SYM:
            printf("%s", t->symbol);
            break;

        case LVAL_QEXPR:
            lval_exp_print(t, "{", "}");
            break;

        case LVAL_SEXPR:
            lval_exp_print(t, "(", ")");
            break;
    }
}
