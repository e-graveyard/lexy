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

#include "eval.h"
#include "fmt.h"
#include "parser.h"
#include "repl.h"
#include "type.h"


void
start_repl(tlenv_T* env)
{
    printf("%s %s\n", PROGRAM_NAME, PROGRAM_VERSION);
    psout("press CTRL+c to exit");

    while(1)
    {
        psout("\n\n");

        char* input = readline(PROMPT_DISPLAY);
        add_history(input);

        mpc_result_t r;
        if(mpc_parse("<stdin>", input, Lisp, &r))
        {
            tlval_T* t = tlval_eval(env, tlval_read(r.output));

            psout("=> ");
            tlval_print(t);
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
}
