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
