/*

  _           _ _ _   _
 | |         (_) | | (_)
 | |__  _   _ _| | |_ _ _ __    ___
 | '_ \| | | | | | __| | '_ \  / __|
 | |_) | |_| | | | |_| | | | || (__
 |_.__/ \__,_|_|_|\__|_|_| |_(_)___|

 builtin.c: Built-in functionalities

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

#include <math.h>

#include "builtin.h"

#include "parser.h"
#include "type.h"
#include "fmt.h"


#define LASSERT(args, cond, fmt, ...) \
    if (!cond) { \
        lval_T* err = lval_err(fmt, __VA_ARGS__); \
        lval_del(args); \
        return err; \
    }


#define LASSERT_TYPE(fname, args, index, expect) \
    LASSERT(args, (args->cell[index]->type == expect), \
        "function '%s' has taken an incorrect type at argument %i. " \
        "Got '%s', expected '%s'", \
        fname, (index + 1), ltype_nrepr(args->cell[index]->type), ltype_nrepr(expect))


#define LASSERT_NUM(fname, args, num) \
    LASSERT(args, (args->counter == num), \
        "function '%s' has taken an incorrect number of arguments. " \
        "Got %i, expected %i", \
        fname, args->counter, num);


#define LASSERT_NOT_EMPTY(fname, args, index) \
    LASSERT(args, (args->cell[index]->counter != 0), \
        "function '%s' has taken nil value for argument %i", \
        fname, index);


lval_T* lenv_put      (lenv_T* env, lval_T* var, lval_T* value, lcond_E cond);
lval_T* lenv_putg     (lenv_T* env, lval_T* var, lval_T* value, lcond_E cond);
char*   ltype_nrepr   (int type);
void    lval_del      (lval_T* v);
int     lval_eq       (lval_T* a, lval_T* b);
void    lval_print    (lenv_T* e, lval_T* t);
lval_T* lval_new      (void);
lval_T* lval_num      (double n);
lval_T* lval_err      (const char* fmt, ...);
lval_T* lval_eval     (lenv_T* env, lval_T* value);
lval_T* lval_join     (lval_T* x, lval_T* y);
lval_T* lval_lambda   (lval_T* formals, lval_T* body);
lval_T* lval_pop      (lval_T* t, size_t i);
lval_T* lval_sexpr    (void);
lval_T* lval_take     (lval_T* t, size_t i);
lval_T* lval_read     (mpc_ast_t* t);
lval_T* btinfn_define (lenv_T* env, lval_T* qexpr, const char* fn);


/**
 * builtin_order - Built-in object equality
 */
lval_T* builtin_eq(lenv_T* env, lval_T* args, char* op)
{
    LASSERT_NUM(op, args, 2);

    double r;

    if (strequ(op, "eq"))
        r = lval_eq(args->cell[0], args->cell[1]);

    if (strequ(op, "ne"))
        r = !lval_eq(args->cell[0], args->cell[1]);

    lval_del(args);
    return lval_num(r);
}


/**
 * builtin_order - Built-in numeric comparisons
 */
lval_T* builtin_order(lenv_T* env, lval_T* args, char* op)
{
    LASSERT_NUM(op, args, 2);
    LASSERT_TYPE(op, args, 0, LTYPE_NUM);
    LASSERT_TYPE(op, args, 1, LTYPE_NUM);

    double r;

    if (strequ(op, "gt"))
        r = (args->cell[0]->number > args->cell[1]->number);

    if (strequ(op, "ge"))
        r = (args->cell[0]->number >= args->cell[1]->number);

    if (strequ(op, "lt"))
        r = (args->cell[0]->number < args->cell[1]->number);

    if (strequ(op, "le"))
        r = (args->cell[0]->number <= args->cell[1]->number);

    lval_del(args);
    return lval_num(r);
}


/**
 * builtin_numop - Built-in numeric operations
 */
lval_T* builtin_numop(lenv_T* env, lval_T* args, const char* op)
{
    for (size_t i = 0; i < args->counter; i++)
    {
        LASSERT_TYPE(op, args, i, LTYPE_NUM);
    }

    lval_T* xval = lval_pop(args, 0);

    if (strequ(op, "-") && xval->counter == 0)
        xval->number = -xval->number;

    while (args->counter > 0)
    {
        lval_T* yval = lval_pop(args, 0);

        if (strequ(op, "add"))
            xval->number += yval->number;

        if (strequ(op, "sub"))
            xval->number -= yval->number;

        if (strequ(op, "mul"))
            xval->number *= yval->number;

        if (strequ(op, "div"))
        {
            if (yval->number == 0)
            {
                lval_del(xval);
                lval_del(yval);

                xval = lval_err(TLERR_DIV_ZERO);
                break;
            }

            xval->number /= yval->number;
        }

        if (strequ(op, "mod"))
            xval->number = fmodf(xval->number, yval->number);

        if (strequ(op, "pow"))
            xval->number = powf(xval->number, yval->number);

        if (strequ(op, "min"))
        {
            xval->number = xval->number > yval->number
                ? yval->number
                : xval->number;
        }

        if (strequ(op, "max"))
        {
            xval->number = xval->number > yval->number
                ? xval->number
                : yval->number;
        }

        lval_del(yval);
    }

    lval_del(args);
    return xval;
}


/**
 * btinfn_cmp_gt - ">" built-in function
 */
lval_T* btinfn_cmp_gt(lenv_T* env, lval_T* args)
{
    return builtin_order(env, args, "gt");
}


/**
 * btinfn_cmp_ge - ">=" built-in function
 */
lval_T* btinfn_cmp_ge(lenv_T* env, lval_T* args)
{
    return builtin_order(env, args, "ge");
}


/**
 * btinfn_cmp_lt - "<" built-in function
 */
lval_T*
btinfn_cmp_lt(lenv_T* env, lval_T* args)
{
    return builtin_order(env, args, "lt");
}


/**
 * btinfn_cmp_le - "<=" built-in function
 */
lval_T* btinfn_cmp_le(lenv_T* env, lval_T* args)
{
    return builtin_order(env, args, "le");
}


/**
 * btinfn_cmp_eq - "==" built-in function
 */
lval_T* btinfn_cmp_eq(lenv_T* env, lval_T* args)
{
    return builtin_eq(env, args, "eq");
}


/**
 * btinfn_cmp_ne - "!=" built-in function
 */
lval_T* btinfn_cmp_ne(lenv_T* env, lval_T* args)
{
    return builtin_eq(env, args, "ne");
}


/**
 * btinfn_add - "add" built-in function
 */
lval_T* btinfn_add(lenv_T* env, lval_T* args)
{
    return builtin_numop(env, args, "add");
}


/**
 * btinfn_sub - "sub" built-in function
 */
lval_T* btinfn_sub(lenv_T* env, lval_T* args)
{
    return builtin_numop(env, args, "sub");
}


/**
 * btinfn_mul - "mul" built-in function
 */
lval_T* btinfn_mul(lenv_T* env, lval_T* args)
{
    return builtin_numop(env, args, "mul");
}


/**
 * btinfn_div - "div" built-in function
 */
lval_T* btinfn_div(lenv_T* env, lval_T* args)
{
    return builtin_numop(env, args, "div");
}


/**
 * btinfn_mod - "mod" built-in function
 */
lval_T* btinfn_mod(lenv_T* env, lval_T* args)
{
    return builtin_numop(env, args, "mod");
}


/**
 * btinfn_pow - "pow" built-in function
 */
lval_T* btinfn_pow(lenv_T* env, lval_T* args)
{
    return builtin_numop(env, args, "pow");
}


/**
 * btinfn_max - "max" built-in function
 */
lval_T* btinfn_max(lenv_T* env, lval_T* args)
{
    return builtin_numop(env, args, "max");
}


/**
 * btinfn_min - "min" built-in function
 */
lval_T* btinfn_min(lenv_T* env, lval_T* args)
{
    return builtin_numop(env, args, "min");
}


/**
 * btinfn_sqrt - "sqrt" built-in function
 */
lval_T* btinfn_sqrt(lenv_T* env, lval_T* args)
{
    LASSERT_NUM("sqrt", args, 1);
    LASSERT_TYPE("sqrt", args, 0, LTYPE_NUM);

    lval_T* val = lval_pop(args, 0);
    val->number = sqrt(val->number);

    lval_del(args);
    return val;
}


/**
 * btinfn_head - "head" built-in function
 *
 * Takes a Q-Expression and returns the first element of it.
 */
lval_T* btinfn_head(lenv_T* env, lval_T* qexpr)
{
    LASSERT_NUM("head", qexpr, 1);
    LASSERT_TYPE("head", qexpr, 0, LTYPE_QEXPR);
    LASSERT_NOT_EMPTY("head", qexpr, 0);

    lval_T* val = lval_take(qexpr, 0);
    while (val->counter > 1)
        lval_del(lval_pop(val, 1));

    return val;
}


/**
 * btinfn_tail - "tail" built-in function
 *
 * Takes a Q-Expression and return it minus the first element.
 */
lval_T* btinfn_tail(lenv_T* env, lval_T* qexpr)
{
    LASSERT_NUM("tail", qexpr, 1);
    LASSERT_TYPE("tail", qexpr, 0, LTYPE_QEXPR);
    LASSERT_NOT_EMPTY("tail", qexpr, 0);

    lval_T* val = lval_take(qexpr, 0);
    lval_del(lval_pop(val, 0));

    return val;
}


/**
 * btinfn_list - "list" built-in function
 *
 * Takes a S-Expression and converts it to a Q-Expression.
 */
lval_T* btinfn_list(lenv_T* env, lval_T* sexpr)
{
    sexpr->type = LTYPE_QEXPR;
    return sexpr;
}


/**
 * btinfn_join - "join" built-in function
 *
 * Takes a Q-Expression and joins all of its arguments.
 */
lval_T* btinfn_join(lenv_T* env, lval_T* qexprv)
{
    for (size_t i = 0; i < qexprv->counter; i++)
    {
        LASSERT_TYPE("join", qexprv, i, LTYPE_QEXPR);
    }

    lval_T* nexpr = lval_pop(qexprv, 0);
    while (qexprv->counter)
        nexpr = lval_join(nexpr, lval_pop(qexprv, 0));

    lval_del(qexprv);
    return nexpr;
}


/**
 * btinfn_eval - "eval" built-in function
 *
 * Takes a Q-Expression and evaluates it as a S-Expression.
 */
lval_T* btinfn_eval(lenv_T* env, lval_T* qexpr)
{
    LASSERT_NUM("eval", qexpr, 1);
    LASSERT_TYPE("eval", qexpr, 0, LTYPE_QEXPR);

    lval_T* nexpr = lval_take(qexpr, 0);
    nexpr->type = LTYPE_SEXPR;

    return lval_eval(env, nexpr);
}


lval_T* btinfn_let(lenv_T* env, lval_T* qexpr)
{
    return btinfn_define(env, qexpr, "let");
}


lval_T* btinfn_letc(lenv_T* env, lval_T* qexpr)
{
    return btinfn_define(env, qexpr, "letc");
}


lval_T* btinfn_global(lenv_T* env, lval_T* qexpr)
{
    return btinfn_define(env, qexpr, "global");
}


lval_T* btinfn_globalc(lenv_T* env, lval_T* qexpr)
{
    return btinfn_define(env, qexpr, "globalc");
}


lval_T* btinfn_define(lenv_T* env, lval_T* qexpr, const char* fn)
{
    LASSERT_TYPE(fn, qexpr, 0, LTYPE_QEXPR);

    lval_T* symbols = qexpr->cell[0];

    for (size_t i = 0; i < symbols->counter; i++)
    {
        LASSERT(qexpr, (symbols->cell[i]->type == LTYPE_SYM),
            "function '%s' cannot define non-symbol. "
            "Got '%s', expected '%s'.", fn,
            ltype_nrepr(symbols->cell[i]->type), ltype_nrepr(LTYPE_SYM));
    }

    LASSERT(qexpr, (symbols->counter == (qexpr->counter - 1)),
        "function '%s' has taken too many arguments. "
        "Got %i, expected %i.", fn,
        symbols->counter, qexpr->counter);

    for (size_t i = 0; i < symbols->counter; i++)
    {
        lval_T* p;

        if (strequ(fn, "let"))
            p = lenv_put(env, symbols->cell[i], qexpr->cell[i + 1], LCOND_DYNAMIC);

        if (strequ(fn, "letc"))
            p = lenv_put(env, symbols->cell[i], qexpr->cell[i + 1], LCOND_CONSTANT);

        if (strequ(fn, "global"))
            p = lenv_putg(env, symbols->cell[i], qexpr->cell[i + 1], LCOND_DYNAMIC);

        if (strequ(fn, "globalc"))
            p = lenv_putg(env, symbols->cell[i], qexpr->cell[i + 1], LCOND_CONSTANT);

        if (p->type == LTYPE_ERR) {
            return p;
        }
    }

    lval_del(qexpr);
    return lval_sexpr();
}


lval_T* btinfn_lambda(lenv_T* env, lval_T* qexpr)
{
    LASSERT_NUM("lambda", qexpr, 2);
    LASSERT_TYPE("lambda", qexpr, 0, LTYPE_QEXPR);
    LASSERT_TYPE("lambda", qexpr, 1, LTYPE_QEXPR);

    for (size_t i = 0; i < qexpr->cell[0]->counter; i++)
    {
        int type = qexpr->cell[0]->cell[i]->type;

        LASSERT(qexpr, (type == LTYPE_SYM),
            "Cannot define non-symbol. "
            "Got '%s', expected '%s'.",
            ltype_nrepr(type), ltype_nrepr(LTYPE_SYM));
    }

    lval_T* formals = lval_pop(qexpr, 0);
    lval_T* body = lval_pop(qexpr, 0);

    lval_del(qexpr);
    return lval_lambda(formals, body);
}


lval_T* btinfn_if(lenv_T* env, lval_T* args)
{
    LASSERT_NUM("if", args, 3);
    LASSERT_TYPE("if", args, 0, LTYPE_NUM);
    LASSERT_TYPE("if", args, 1, LTYPE_QEXPR);
    LASSERT_TYPE("if", args, 2, LTYPE_QEXPR);

    args->cell[1]->type = LTYPE_SEXPR;
    args->cell[2]->type = LTYPE_SEXPR;

    lval_T* v = lval_eval(env, lval_pop(args, args->cell[0]->number ? 1 : 2));

    lval_del(args);
    return v;
}


lval_T* btinfn_print(lenv_T* env, lval_T* args)
{
    for (size_t i = 0; i < args->counter; i++)
    {
        lval_print(env, args->cell[i]);
        putchar(' ');
    }
    putchar('\n');

    lval_del(args);
    return lval_sexpr();
}


lval_T* btinfn_error(lenv_T* env, lval_T* args)
{
    LASSERT_NUM("error", args, 1);
    LASSERT_TYPE("error", args, 0, LTYPE_STR);

    lval_T* err = lval_err(args->cell[0]->string);

    lval_del(args);
    return err;
}


lval_T* btinfn_load(lenv_T* env, lval_T* args)
{
    LASSERT_NUM("use", args, 1);
    LASSERT_TYPE("use", args, 0, LTYPE_STR);

    mpc_result_t r;
    if (mpc_parse_contents(strcat(args->cell[0]->string, ".lisp"), Lisp, &r))
    {
        lval_T* expr = lval_read(r.output);
        mpc_ast_delete(r.output);

        while (expr->counter)
        {
            lval_T* e = lval_eval(env, lval_pop(expr, 0));
            if (e->type == LTYPE_ERR)
                lval_print(env, e);

            lval_del(e);
        }

        lval_del(expr);
        lval_del(args);

        return lval_sexpr();
    }

    char* err_msg = mpc_err_string(r.error);
    mpc_err_delete(r.error);

    lval_T* err = lval_err("Could not load library %s", err_msg);
    free(err_msg);

    lval_del(args);
    return err;
}
