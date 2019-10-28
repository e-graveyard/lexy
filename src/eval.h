#ifndef TMUL_EVAL
#define TMUL_EVAL

#include "mpc.h"
#include "type.h"


#define ERROR_MESSAGE_BYTE_LENGTH 512

// ...
int      tlval_eq    (tlval_T* a, tlval_T* b);
void     tlval_print (tlval_T* t);
void     tlenv_init  (tlenv_T* env);
void     tlval_del   (tlval_T* v);
tlenv_T* tlenv_new   (void);
tlval_T* tlval_sexpr (void);
tlval_T* tlval_read  (mpc_ast_t* t);
tlval_T* tlval_add   (tlval_T* v, tlval_T* x);
tlval_T* tlval_eval  (tlenv_T* env, tlval_T* value);
tlval_T* tlval_err   (const char* fmt, ...);
tlval_T* tlval_str   (char* s);

#endif
