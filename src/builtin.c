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
#include "fmt.h"
#include "parser.h"
#include "type.h"


tlval_T* tlenv_put     (tlenv_T* env, tlval_T* var, tlval_T* value, boolean freezed);
tlval_T* tlenv_putg    (tlenv_T* env, tlval_T* var, tlval_T* value, boolean freezed);
char*    tltype_nrepr  (int type);
void     tlval_del     (tlval_T* v);
int      tlval_eq      (tlval_T* a, tlval_T* b);
void     tlval_print   (tlval_T* t);
tlval_T* tlval_new     (void);
tlval_T* tlval_num     (float n);
tlval_T* tlval_err     (const char* fmt, ...);
tlval_T* tlval_eval    (tlenv_T* env, tlval_T* value);
tlval_T* tlval_join    (tlval_T* x, tlval_T* y);
tlval_T* tlval_lambda  (tlval_T* formals, tlval_T* body);
tlval_T* tlval_pop     (tlval_T* t, size_t i);
tlval_T* tlval_sexpr   (void);
tlval_T* tlval_take    (tlval_T* t, size_t i);
tlval_T* tlval_read    (mpc_ast_t* t);
tlval_T* btinfn_define (tlenv_T* env, tlval_T* qexpr, const char* fn);


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


/**
 * builtin_order - Built-in object equality
 */
tlval_T*
builtin_eq(tlenv_T* env, tlval_T* args, char* op)
{
    TLASSERT_NUM(op, args, 2);

    float r;

    if(strequ(op, "eq"))
        r = tlval_eq(args->cell[0], args->cell[1]);

    if(strequ(op, "ne"))
        r = !tlval_eq(args->cell[0], args->cell[1]);

    tlval_del(args);
    return tlval_num(r);
}


/**
 * builtin_order - Built-in numeric comparisons
 */
tlval_T*
builtin_order(tlenv_T* env, tlval_T* args, char* op)
{
    TLASSERT_NUM(op, args, 2);
    TLASSERT_TYPE(op, args, 0, TLVAL_NUM);
    TLASSERT_TYPE(op, args, 1, TLVAL_NUM);

    float r;

    if(strequ(op, "gt"))
        r = (args->cell[0]->number > args->cell[1]->number);

    if(strequ(op, "ge"))
        r = (args->cell[0]->number >= args->cell[1]->number);

    if(strequ(op, "lt"))
        r = (args->cell[0]->number < args->cell[1]->number);

    if(strequ(op, "le"))
        r = (args->cell[0]->number <= args->cell[1]->number);

    tlval_del(args);
    return tlval_num(r);
}


/**
 * builtin_numop - Built-in numeric operations
 */
tlval_T*
builtin_numop(tlenv_T* env, tlval_T* args, const char* op)
{
    for(size_t i = 0; i < args->counter; i++)
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
 * btinfn_cmp_gt - ">" built-in function
 */
tlval_T*
btinfn_cmp_gt(tlenv_T* env, tlval_T* args)
{
    return builtin_order(env, args, "gt");
}


/**
 * btinfn_cmp_ge - ">=" built-in function
 */
tlval_T*
btinfn_cmp_ge(tlenv_T* env, tlval_T* args)
{
    return builtin_order(env, args, "ge");
}


/**
 * btinfn_cmp_lt - "<" built-in function
 */
tlval_T*
btinfn_cmp_lt(tlenv_T* env, tlval_T* args)
{
    return builtin_order(env, args, "lt");
}


/**
 * btinfn_cmp_le - "<=" built-in function
 */
tlval_T*
btinfn_cmp_le(tlenv_T* env, tlval_T* args)
{
    return builtin_order(env, args, "le");
}


/**
 * btinfn_cmp_eq - "==" built-in function
 */
tlval_T*
btinfn_cmp_eq(tlenv_T* env, tlval_T* args)
{
    return builtin_eq(env, args, "eq");
}


/**
 * btinfn_cmp_ne - "!=" built-in function
 */
tlval_T*
btinfn_cmp_ne(tlenv_T* env, tlval_T* args)
{
    return builtin_eq(env, args, "ne");
}


/**
 * btinfn_add - "add" built-in function
 */
tlval_T*
btinfn_add(tlenv_T* env, tlval_T* args)
{
    return builtin_numop(env, args, "add");
}


/**
 * btinfn_sub - "sub" built-in function
 */
tlval_T*
btinfn_sub(tlenv_T* env, tlval_T* args)
{
    return builtin_numop(env, args, "sub");
}


/**
 * btinfn_mul - "mul" built-in function
 */
tlval_T*
btinfn_mul(tlenv_T* env, tlval_T* args)
{
    return builtin_numop(env, args, "mul");
}


/**
 * btinfn_div - "div" built-in function
 */
tlval_T*
btinfn_div(tlenv_T* env, tlval_T* args)
{
    return builtin_numop(env, args, "div");
}


/**
 * btinfn_mod - "mod" built-in function
 */
tlval_T*
btinfn_mod(tlenv_T* env, tlval_T* args)
{
    return builtin_numop(env, args, "mod");
}


/**
 * btinfn_pow - "pow" built-in function
 */
tlval_T*
btinfn_pow(tlenv_T* env, tlval_T* args)
{
    return builtin_numop(env, args, "pow");
}


/**
 * btinfn_max - "max" built-in function
 */
tlval_T*
btinfn_max(tlenv_T* env, tlval_T* args)
{
    return builtin_numop(env, args, "max");
}


/**
 * btinfn_min - "min" built-in function
 */
tlval_T*
btinfn_min(tlenv_T* env, tlval_T* args)
{
    return builtin_numop(env, args, "min");
}


/**
 * btinfn_sqrt - "sqrt" built-in function
 */
tlval_T*
btinfn_sqrt(tlenv_T* env, tlval_T* args)
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
tlval_T*
btinfn_head(tlenv_T* env, tlval_T* qexpr)
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
tlval_T*
btinfn_tail(tlenv_T* env, tlval_T* qexpr)
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
tlval_T*
btinfn_list(tlenv_T* env, tlval_T* sexpr)
{
    sexpr->type = TLVAL_QEXPR;
    return sexpr;
}


/**
 * btinfn_join - "join" built-in function
 *
 * Takes a Q-Expression and joins all of its arguments.
 */
tlval_T*
btinfn_join(tlenv_T* env, tlval_T* qexprv)
{
    for(size_t i = 0; i < qexprv->counter; i++)
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
tlval_T*
btinfn_eval(tlenv_T* env, tlval_T* qexpr)
{
    TLASSERT_NUM("eval", qexpr, 1);
    TLASSERT_TYPE("eval", qexpr, 0, TLVAL_QEXPR);

    tlval_T* nexpr = tlval_take(qexpr, 0);
    nexpr->type = TLVAL_SEXPR;

    return tlval_eval(env, nexpr);
}


tlval_T*
btinfn_let(tlenv_T* env, tlval_T* qexpr)
{
    return btinfn_define(env, qexpr, "let");
}


tlval_T*
btinfn_letc(tlenv_T* env, tlval_T* qexpr)
{
    return btinfn_define(env, qexpr, "letc");
}


tlval_T*
btinfn_global(tlenv_T* env, tlval_T* qexpr)
{
    return btinfn_define(env, qexpr, "global");
}


tlval_T*
btinfn_globalc(tlenv_T* env, tlval_T* qexpr)
{
    return btinfn_define(env, qexpr, "globalc");
}


tlval_T*
btinfn_define(tlenv_T* env, tlval_T* qexpr, const char* fn)
{
    TLASSERT_TYPE(fn, qexpr, 0, TLVAL_QEXPR);

    tlval_T* symbols = qexpr->cell[0];

    for(size_t i = 0; i < symbols->counter; i++)
    {
        TLASSERT(qexpr, (symbols->cell[i]->type == TLVAL_SYM),
            "function '%s' cannot define non-symbol. "
            "Got '%s', expected '%s'.", fn,
            tltype_nrepr(symbols->cell[i]->type), tltype_nrepr(TLVAL_SYM));
    }

    TLASSERT(qexpr, (symbols->counter == (qexpr->counter - 1)),
        "function '%s' has taken too many arguments. "
        "Got %i, expected %i.", fn,
        symbols->counter, qexpr->counter);

    for(size_t i = 0; i < symbols->counter; i++)
    {
        tlval_T* p;

        if(strequ(fn, "let"))
            p = tlenv_put(env, symbols->cell[i], qexpr->cell[i + 1], TLVAL_DYNAMIC);

        if(strequ(fn, "letc"))
            p = tlenv_put(env, symbols->cell[i], qexpr->cell[i + 1], TLVAL_CONSTANT);

        if(strequ(fn, "global"))
            p = tlenv_putg(env, symbols->cell[i], qexpr->cell[i + 1], TLVAL_DYNAMIC);

        if(strequ(fn, "globalc"))
            p = tlenv_putg(env, symbols->cell[i], qexpr->cell[i + 1], TLVAL_CONSTANT);

        if (p->type == TLVAL_ERR) {
            return p;
        }
    }

    tlval_del(qexpr);
    return tlval_sexpr();
}


tlval_T*
btinfn_lambda(tlenv_T* env, tlval_T* qexpr)
{
    TLASSERT_NUM("lambda", qexpr, 2);
    TLASSERT_TYPE("lambda", qexpr, 0, TLVAL_QEXPR);
    TLASSERT_TYPE("lambda", qexpr, 1, TLVAL_QEXPR);

    for(size_t i = 0; i < qexpr->cell[0]->counter; i++)
    {
        int type = qexpr->cell[0]->cell[i]->type;

        TLASSERT(qexpr, (type == TLVAL_SYM),
            "Cannot define non-symbol. "
            "Got '%s', expected '%s'.",
            tltype_nrepr(type), tltype_nrepr(TLVAL_SYM));
    }

    tlval_T* formals = tlval_pop(qexpr, 0);
    tlval_T* body = tlval_pop(qexpr, 0);

    tlval_del(qexpr);
    return tlval_lambda(formals, body);
}


tlval_T*
btinfn_if(tlenv_T* env, tlval_T* args)
{
    TLASSERT_NUM("if", args, 3);
    TLASSERT_TYPE("if", args, 0, TLVAL_NUM);
    TLASSERT_TYPE("if", args, 1, TLVAL_QEXPR);
    TLASSERT_TYPE("if", args, 2, TLVAL_QEXPR);

    args->cell[1]->type = TLVAL_SEXPR;
    args->cell[2]->type = TLVAL_SEXPR;

    tlval_T* v = args->cell[0]->number
        ? tlval_eval(env, tlval_pop(args, 1))
        : tlval_eval(env, tlval_pop(args, 2));

    tlval_del(args);
    return v;
}


tlval_T*
btinfn_print(tlenv_T* env, tlval_T* args)
{
    for(size_t i = 0; i < args->counter; i++)
    {
        tlval_print(args->cell[i]);
        putchar(' ');
    }

    putchar('\n');
    tlval_del(args);

    return tlval_sexpr();
}


tlval_T*
btinfn_error(tlenv_T* env, tlval_T* args)
{
    TLASSERT_NUM("error", args, 1);
    TLASSERT_TYPE("error", args, 0, TLVAL_STR);

    tlval_T* err = tlval_err(args->cell[0]->string);

    tlval_del(args);
    return err;
}


tlval_T*
btinfn_load(tlenv_T* env, tlval_T* args)
{
    TLASSERT_NUM("use", args, 1);
    TLASSERT_TYPE("use", args, 0, TLVAL_STR);

    mpc_result_t r;
    if(mpc_parse_contents(strcat(args->cell[0]->string, ".lisp"), Lisp, &r))
    {
        tlval_T* expr = tlval_read(r.output);
        mpc_ast_delete(r.output);

        while(expr->counter)
        {
            tlval_T* e = tlval_eval(env, tlval_pop(expr, 0));
            if(e->type == TLVAL_ERR)
                tlval_print(e);

            tlval_del(e);
        }

        tlval_del(expr);
        tlval_del(args);

        return tlval_sexpr();
    }

    char* err_msg = mpc_err_string(r.error);
    mpc_err_delete(r.error);

    tlval_T* err = tlval_err("Could not load library %s", err_msg);
    tlval_del(args);
    free(err_msg);

    return err;
}
