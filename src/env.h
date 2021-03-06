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

#ifndef LEXY_ENV
#define LEXY_ENV

#include "type.h"


lenv_T* lenv_copy (lenv_T* env);
void    lenv_del  (lenv_T* e);
lval_T* lenv_get  (lenv_T* env, lval_T* val);
void    lenv_incb (lenv_T* env, char* fname, char* fdescr, lbtin fref);
void    lenv_init (lenv_T* env);
lenv_T* lenv_new  (void);
lval_T* lenv_put  (lenv_T* env, lval_T* var, lval_T* value, lcond_E cond);
lval_T* lenv_putg (lenv_T* env, lval_T* var, lval_T* value, lcond_E cond);

#endif
