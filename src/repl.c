/*

  _                   _
 | |                 | |
 | |_ _ __ ___  _   _| |  ___
 | __| '_ ` _ \| | | | | / __|
 | |_| | | | | | |_| | || (__
  \__|_| |_| |_|\__,_|_(_)___|

 tmul.c: TMUL main routines

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

#include <signal.h>

#include "repl.h"
#include "eval.h"
#include "fmt.h"
#include "parser.h"


static void interrupt(int sign);
static void tl_print(tlval_T* t);
static void tl_sexp_print(tlval_T* t, char* openc, char* closec);
static char* prompt();

static parser_T p;
static tlenv_T* env;


int
main(void)
{
    signal(SIGINT, interrupt);

    printf("%s: version %s\n", PROGRAM_NAME, PROGRAM_VERSION);
    psout("press CTRL+c to exit");

    p = parser_init();

    env = tlenv_new();
    tlenv_init(env);

    for(;;)
    {
        char* input = prompt();

        mpc_result_t r;
        if(mpc_parse("<stdin>", input, p.Lisp, &r))
        {
            tlval_T* t = tlval_eval(env, tlval_read(r.output));

            psout("=> ");
            tl_print(t);
            tlval_del(t);

            mpc_ast_delete(r.output);
        }
        else
        {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }

    return 0;
}

static void
interrupt(int sign)
{
    free(env);
    mpc_cleanup(6,
            p.Number,
            p.Symbol,
            p.SExpr,
            p.QExpr,
            p.Atom,
            p.Lisp);

    exit(0);
}

static char*
prompt()
{
    psout("\n\n");
    char* input = readline(PROMPT_DISPLAY);
    add_history(input);

    return input;
}

static void
tl_sexp_print(tlval_T* t, char* openc, char* closec)
{
    psout(openc);
    for(size_t i = 0; i < t->counter; i++)
    {
        tl_print(t->cell[i]);

        if(i != (t->counter - 1))
            psout(" ");
    }
    psout(closec);
}

static void
tl_num_print(float n)
{
    if(isfint(n))
    {
        printf("%ld", (long)round(n));
    }
    else
    {
        printf("%f", n);
    }
}

static void
tl_print(tlval_T* t)
{
    switch(t->type)
    {
        case TLVAL_FUN:
            if(t->builtin)
            {
                psout("<function>");
            }
            else
            {
                psout("(lambda ");
                tl_print(t->formals);
                psout(" ");
                tl_print(t->body);
                psout(")");
            }
            break;

        case TLVAL_NUM:
            tl_num_print(t->number);
            break;

        case TLVAL_ERR:
            printf("Error: %s.", t->error);
            break;

        case TLVAL_SYM:
            printf("%s", t->symbol);
            break;

        case TLVAL_QEXPR:
           tl_sexp_print(t, "'(", ")");
            break;

        case TLVAL_SEXPR:
            tl_sexp_print(t, "(", ")");
            break;
    }
}
