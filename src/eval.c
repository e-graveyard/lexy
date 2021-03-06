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

#include "env.h"
#include "fmt.h"
#include "type.h"


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
lval_T* btinfn_eval (lenv_T* env, lval_T* qexpr);
lval_T* btinfn_list (lenv_T* env, lval_T* sexpr);


/**
 * ltype_nrepr - TL type name representation
 */
char* ltype_nrepr(int type)
{
    switch(type)
    {
        case LTYPE_FUN:   return "Function";
        case LTYPE_NUM:   return "Number";
        case LTYPE_STR:   return "String";
        case LTYPE_ERR:   return "Error";
        case LTYPE_SYM:   return "Symbol";
        case LTYPE_SEXPR: return "S-Expression";
        case LTYPE_QEXPR: return "Q-Expression";
        default:          return "Unknown";
    }
}


lval_T* lval_new(void)
{
    lval_T* v    = malloc(sizeof(struct lval_S));
    v->condition = LCOND_UNSET;
    v->error     = NULL;

    return v;
}


/**
 * lval_fun - TL function representation
 *
 * Constructs a pointer to a new TL function representation.
 */
lval_T* lval_fun(char* name, char* description, lbtin func)
{
    lval_T* v  = lval_new();
    v->type    = LTYPE_FUN;
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
lval_T* lval_lambda(lval_T* formals, lval_T* body)
{
    lval_T* v  = lval_new();
    v->type    = LTYPE_FUN;
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
lval_T* lval_num(double n)
{
    lval_T* v = lval_new();
    v->type   = LTYPE_NUM;
    v->number = n;

    return v;
}


lval_T* lval_str(char* s)
{
    lval_T* v = lval_new();
    v->type   = LTYPE_STR;
    v->string = malloc(strlen(s) + 1);

    strcpy(v->string, s);
    return v;
}


/**
 * lval_err - TL error representation
 *
 * Constructs a pointer to a new TL error representation.
 */
lval_T* lval_err(const char* fmt, ...)
{
    lval_T* v = lval_new();
    v->type   = LTYPE_ERR;

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
lval_T* lval_sym(const char* s)
{
    lval_T* v = lval_new();
    v->type   = LTYPE_SYM;
    v->symbol = malloc(strlen(s) + 1);

    strcpy(v->symbol, s);
    return v;
}


/**
 * lval_sexpr - TL S-Expression representation
 *
 * Constructs a pointer to a new TL symbolic expression representation.
 */
lval_T* lval_sexpr(void)
{
    lval_T* v  = lval_new();
    v->type    = LTYPE_SEXPR;
    v->counter = 0;
    v->cell    = NULL;

    return v;
}


/**
 * lval_qexpr - TL Q-Expression representation
 *
 * Constructs a pointer to a new TL quoted expression representation.
 */
lval_T* lval_qexpr(void)
{
    lval_T* v  = lval_new();
    v->type    = LTYPE_QEXPR;
    v->counter = 0;
    v->cell    = NULL;

    return v;
}


/**
 * lval_read - TL value reading
 */
lval_T* lval_read(mpc_ast_t* t)
{
    if (strstr(t->tag, "number"))
        return lval_rnum(t);

    if (strstr(t->tag, "string"))
        return lval_rstr(t);

    if (strstr(t->tag, "symbol"))
        return lval_sym(t->contents);

    lval_T* x = NULL;
    if (strequ(t->tag, ">") || strstr(t->tag, "sexpr"))
        x = lval_sexpr();

    if (strstr(t->tag, "qexpr"))
        x = lval_qexpr();

    for (int i = 0; i < t->children_num; i++)
    {
        if (strequ(t->children[i]->contents, "("))  continue;
        if (strequ(t->children[i]->contents, ")"))  continue;
        if (strequ(t->children[i]->contents, "{"))  continue;
        if (strequ(t->children[i]->contents, "}"))  continue;
        if (strequ(t->children[i]->tag, "regex"))   continue;
        if (strstr(t->children[i]->tag, "comment")) continue;

        x = lval_add(x, lval_read(t->children[i]));
    }

    return x;
}


/**
 * lval_rnum - TL numeric value reading
 */
lval_T* lval_rnum(mpc_ast_t* t)
{
    errno = 0;
    double f = strtof(t->contents, NULL);

    return errno != ERANGE
        ? lval_num(f)
        : lval_err(TLERR_BAD_NUM);
}

lval_T* lval_rstr(mpc_ast_t* t)
{
    t->contents[strlen(t->contents) - 1] = '\0';
    char* unescaped = malloc(strlen(t->contents + 1) + 1);
    strcpy(unescaped, t->contents + 1);

    unescaped = mpcf_unescape(unescaped);
    lval_T* str = lval_str(unescaped);

    free(unescaped);
    return str;
}


/**
 * lval_add - TL value addition
 *
 * Adds a TL value to a S-Expression construct.
 */
lval_T* lval_add(lval_T* v, lval_T* x)
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
void lval_del(lval_T* v)
{
    switch(v->type)
    {
        case LTYPE_NUM: break;

        case LTYPE_FUN:
            if (!v->builtin)
            {
                lenv_del(v->environ);
                lval_del(v->formals);
                lval_del(v->body);
            }
            break;

        case LTYPE_STR:
            free(v->string);
            break;

        case LTYPE_ERR:
            free(v->error);
            break;

        case LTYPE_SYM:
            free(v->symbol);
            break;

        case LTYPE_SEXPR:
        case LTYPE_QEXPR:
            for (size_t i = 0; i < v->counter; i++)
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
lval_T* lval_copy(lval_T* val)
{
    lval_T* nval = malloc(sizeof(struct lval_S));
    nval->type = val->type;
    nval->condition = val->condition;

    switch(val->type)
    {
        case LTYPE_FUN:
            if (val->builtin)
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

        case LTYPE_NUM:
            nval->number = val->number;
            break;

        case LTYPE_STR:
            nval->string = malloc(strlen(val->string) +1);
            strcpy(nval->string, val->string);
            break;

        case LTYPE_ERR:
            nval->error = malloc(strlen(val->error) + 1);
            strcpy(nval->error, val->error);
            break;

        case LTYPE_SYM:
            nval->symbol = malloc(strlen(val->symbol) + 1);
            strcpy(nval->symbol, val->symbol);
            break;

        case LTYPE_SEXPR:
        case LTYPE_QEXPR:
            nval->counter = val->counter;
            nval->cell = malloc(sizeof(struct lval_S) * nval->counter);

            for (size_t i = 0; i < nval->counter; i++)
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
lval_T* lval_pop(lval_T* t, size_t i)
{
    lval_T* v = t->cell[i];

    memmove(&t->cell[i], &t->cell[i + 1], sizeof(lval_T*) * t->counter);

    t->counter--;
    t->cell = realloc(t->cell, sizeof(struct lval_S) * t->counter);

    return v;
}


/**
 * lval_take - TL value take operation
 *
 * Takes a S-Expression, extracts the element at index "i" from it and return it.
 * The "take" operation deletes the original input list;
 */
lval_T* lval_take(lval_T* t, size_t i)
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
lval_T* lval_join(lval_T* x, lval_T* y)
{
    while (y->counter)
        x = lval_add(x, lval_pop(y, 0));

    lval_del(y);
    return x;
}


lval_T* lval_call(lenv_T* env, lval_T* func, lval_T* args)
{
    if (func->builtin)
        return func->builtin(env, args);

    size_t given = args->counter;
    size_t total = func->formals->counter;

    while (args->counter)
    {
        if (func->formals->counter == 0)
        {
            lval_del(args);
            return lval_err(
                "function has taken too many arguments."
                "Got %lu, expected %lu", given, total);
        }

        lval_T* symbol = lval_pop(func->formals, 0);

        if (strequ(symbol->symbol, "&"))
        {
            if (func->formals->counter != 1)
            {
                lval_del(args);
                return lval_err(TLERR_UNBOUND_VARIADIC);
            }

            lval_T* nsym = lval_pop(func->formals, 0);
            lenv_put(func->environ, nsym, btinfn_list(env, args), LCOND_DYNAMIC);

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

    if (func->formals->counter > 0 && strequ(func->formals->cell[0]->symbol, "&"))
    {
        if (func->formals->counter != 2)
            return lval_err(TLERR_UNBOUND_VARIADIC);

        lval_del(lval_pop(func->formals, 0));

        lval_T* symbol = lval_pop(func->formals, 0);
        lval_T* value  = lval_qexpr();

        lenv_put(func->environ, symbol, value, LCOND_DYNAMIC);

        lval_del(symbol);
        lval_del(value);
    }

    if (func->formals->counter == 0)
    {
        func->environ->parent = env;
        return btinfn_eval(func->environ, lval_add(lval_sexpr(), lval_copy(func->body)));
    }

    return lval_copy(func);
}


/**
 * lval_eval - TL value evaluation
 */
lval_T* lval_eval(lenv_T* env, lval_T* value)
{
    if (value->type == LTYPE_SYM)
    {
        lval_T* nval = lenv_get(env, value);
        lval_del(value);
        return nval;
    }

    if (value->type == LTYPE_SEXPR)
        return lval_evsexp(env, value);

    return value;
}


/**
 * lval_evsexp - TL S-Expression evaluation
 */
lval_T* lval_evsexp(lenv_T* env, lval_T* val)
{
    for (size_t i = 0; i < val->counter; i++)
        val->cell[i] = lval_eval(env, val->cell[i]);

    for (size_t i = 0; i < val->counter; i++)
        if (val->cell[i]->type == LTYPE_ERR)
            return lval_take(val, i);

    if (val->counter == 0)
        return val;

    if (val->counter == 1)
        return lval_eval(env, lval_take(val, 0));

    lval_T* element = lval_pop(val, 0);
    if (element->type != LTYPE_FUN)
    {
        lval_T* err = lval_err(
            "S-Expression start with incorrect type. "
            "Got '%s', expected '%s'.",
            ltype_nrepr(element->type), ltype_nrepr(LTYPE_FUN));

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
        case LTYPE_NUM: return a->number == b->number;
        case LTYPE_STR: return strequ(a->string, b->string);
        case LTYPE_ERR: return strequ(a->error, b->error);
        case LTYPE_SYM: return strequ(a->symbol, b->symbol);

        case LTYPE_FUN:
            if (a->builtin || b->builtin)
                return a->builtin == b->builtin;
            else
                return (lval_eq(a->formals, b->formals) &&
                        lval_eq(a->body, b->body));

        case LTYPE_QEXPR:
        case LTYPE_SEXPR:
            if (a->counter != b->counter) return 0;

            for (size_t i = 0; i < a->counter; i++)
                if (!lval_eq(a->cell[i], b->cell[i])) return 0;

            return 1;
    }

    return 0;
}

void lval_exp_print(lenv_T* e, lval_T* t)
{
    for (size_t i = 0; i < t->counter; i++)
    {
        lval_print(e, t->cell[i]);
        if (i != (t->counter - 1))
            printf(" ");
    }
}


void lval_print(lenv_T* e, lval_T* t)
{
    unsigned short int exec_is_repl = e->exec_type == LEXEC_REPL;

    switch(t->type)
    {
        case LTYPE_FUN:
            if (t->builtin)
            {
                printf("<builtin(%s): %s>", t->btin_meta->name, t->btin_meta->description);
                return;
            }

            CYAN_TXT(exec_is_repl, "%s", "(");
            printf("lambda ");

            lval_print(e, t->formals);
            printf(" ");

            lval_print(e, t->body);
            CYAN_TXT(exec_is_repl, "%s", ")");
            break;

        case LTYPE_NUM:
            if (isvint(t->number)) {
                GREEN_TXT(exec_is_repl, "%ld", (long)round(t->number));
                break;
            }

            GREEN_TXT(exec_is_repl, "%lf", t->number);
            break;

        case LTYPE_STR:
            BLUE_TXT(exec_is_repl, "\"%s\"", t->string);
            break;

        case LTYPE_ERR:
            RED_TXT(exec_is_repl, "%s", "ILLEGAL INSTRUCTION: ");
            printf("%s", t->error);
            break;

        case LTYPE_SYM:
            printf("%s", t->symbol);
            break;

        case LTYPE_QEXPR:
            YELLOW_TXT(exec_is_repl, "%s", "{");
            lval_exp_print(e, t);
            YELLOW_TXT(exec_is_repl, "%s", "}");
            break;

        case LTYPE_SEXPR:
            CYAN_TXT(exec_is_repl, "%s", "(");
            lval_exp_print(e, t);
            CYAN_TXT(exec_is_repl, "%s", ")");
            break;
    }
}
