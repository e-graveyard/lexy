#ifndef TMUL_EVAL
#define TMUL_EVAL

#include "mpc.h"
#include "type.h"


#define ERROR_MESSAGE_BYTE_LENGTH 512

// ...
void     tlenv_init (tlenv_T* env);
tlenv_T* tlenv_new  (void);
void     tlval_del  (tlval_T* v);
tlval_T* tlval_read (mpc_ast_t* t);
tlval_T* tlval_eval (tlenv_T* env, tlval_T* value);
int      tlval_eq   (tlval_T* a, tlval_T* b);

#endif
