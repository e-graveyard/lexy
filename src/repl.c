/*              _
               | |
 _ __ ___ _ __ | |  ___
| '__/ _ \ '_ \| | / __|
| | |  __/ |_) | || (__
|_|  \___| .__/|_(_)___|
         | |
         |_|

 repl.c: Tmul's REPL

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

#include "repl.h"

#include "parser.h"
#include "type.h"
#include "fmt.h"


lval_T* lval_eval  (lenv_T* env, lval_T* value);
lval_T* lval_read  (mpc_ast_t* t);
void    lval_del   (lval_T* v);
void    lval_print (lenv_T* e, lval_T* t);


void
start_repl(lenv_T* env)
{
    BOLD_TXT(1, "\n%s %s\n", PROGRAM_NAME, PROGRAM_VERSION);
    GREY_TXT(1, "%s\n", "press CTRL+c to exit");

    env->exec_type = LEXEC_REPL;

    while(1)
    {
        printf("\n");

        char* input = readline(ANSI_STYLE_BOLD PROMPT_DISPLAY ANSI_RESET);
        add_history(input);

        mpc_result_t r;
        if(mpc_parse("<stdin>", input, Lisp, &r))
        {
            lval_T* t = lval_eval(env, lval_read(r.output));

            GREY_TXT(1, "%s", PROMPT_RESPONSE);
            lval_print(env, t);
            printf("\n");

            lval_del(t);
            mpc_ast_delete(r.output);
        }
        else
        {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }
}
