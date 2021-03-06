/*

   Copyright (c) 2018-2021 Caian R. Ertl <hi@caian.org>

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation
   files (the "Software"), to deal in the Software without
   restriction, including without limitation the rights to use,
   copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following
   conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
   OTHER DEALINGS IN THE SOFTWARE.

 */

#include "parser.h"
#include "type.h"


bool parser_has_been_initialized = FALSE;


void parser_init()
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

    parser_has_been_initialized = TRUE;
}

void parser_safe_cleanup()
{
    if (parser_has_been_initialized)
        mpc_cleanup(8, Number, String, Comment, Symbol, SExpr, QExpr, Atom, Lisp);
}
