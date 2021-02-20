/*

  _ __   __ _ _ __ ___  ___ _ __ ___
 | '_ \ / _` | '__/ __|/ _ \ '__/ __|
 | |_) | (_| | |  \__ \  __/ | | (__
 | .__/ \__,_|_|  |___/\___|_|(_)___|
 | |
 |_|

 parser.c: Language parser

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

#include "parser.h"


void
parser_init()
{
    Number  = mpc_new("number");
    String  = mpc_new("string");
    Comment = mpc_new("comment");
    Symbol  = mpc_new("symbol");
    SExpr   = mpc_new("sexpr");
    QExpr   = mpc_new("qexpr");
    Atom    = mpc_new("atom");
    Lisp    = mpc_new("lisp");

    mpca_lang(MPCA_LANG_DEFAULT,
        " number  : /-?[0-9]+\\.?[0-9]*/ ;             "
        " string  : /\"(\\\\.|[^\"])*\"/ ;             "
        " comment : /;[^\\r\\n]*/ ;                    "
        " symbol  : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ; "
        " sexpr   :  '(' <atom>* ')' ;                 "
        " qexpr   :  '{' <atom>* '}' ;                 "
        " atom    :  <number> | <string> | <symbol> |  "
        "            <sexpr>  | <qexpr>  | <comment> ; "
        " lisp    :  /^/ <atom>* /$/ ;                 ",

        Number, String, Comment, Symbol, SExpr, QExpr, Atom, Lisp);
}

void
parser_cleanup()
{
    mpc_cleanup(8, Number, String, Comment, Symbol, SExpr, QExpr, Atom, Lisp);
}
