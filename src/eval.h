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

#ifndef LEXY_EVAL
#define LEXY_EVAL

#include "mpc.h"
#include "type.h"


#define ERROR_MESSAGE_BYTE_LENGTH 512

int     lval_eq    (lval_T* a, lval_T* b);
void    lval_print (lenv_T* e, lval_T* t);
void    lenv_init  (lenv_T* env);
void    lval_del   (lval_T* v);
lenv_T* lenv_new   (void);
lval_T* lval_new   (void);
lval_T* lval_sexpr (void);
lval_T* lval_read  (mpc_ast_t* t);
lval_T* lval_add   (lval_T* v, lval_T* x);
lval_T* lval_eval  (lenv_T* env, lval_T* value);
lval_T* lval_err   (const char* fmt, ...);
lval_T* lval_str   (char* s);

#endif
