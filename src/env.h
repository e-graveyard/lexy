#ifndef LEXY_ENV
#define LEXY_ENV

#include "type.h"


lenv_T* lenv_copy   (lenv_T* env);
void    lenv_del    (lenv_T* e);
lval_T* lenv_get    (lenv_T* env, lval_T* val);
void    lenv_incb   (lenv_T* env, char* fname, char* fdescr, lbtin fref);
void    lenv_init   (lenv_T* env);
lenv_T* lenv_new    (void);
lval_T* lenv_put    (lenv_T* env, lval_T* var, lval_T* value, lcond_E cond);
lval_T* lenv_putg   (lenv_T* env, lval_T* var, lval_T* value, lcond_E cond);

#endif
