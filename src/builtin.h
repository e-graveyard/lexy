#ifndef TMUL_BUILTIN
#define TMUL_BUILTIN

#include "type.h"


tlval_T* btinfn_add    (tlenv_T* env, tlval_T* args);
tlval_T* btinfn_div    (tlenv_T* env, tlval_T* args);
tlval_T* btinfn_eval   (tlenv_T* env, tlval_T* qexpr);
tlval_T* btinfn_global (tlenv_T* env, tlval_T* qexpr);
tlval_T* btinfn_head   (tlenv_T* env, tlval_T* qexpr);
tlval_T* btinfn_join   (tlenv_T* env, tlval_T* qexprv);
tlval_T* btinfn_lambda (tlenv_T* env, tlval_T* qexpr);
tlval_T* btinfn_let    (tlenv_T* env, tlval_T* qexpr);
tlval_T* btinfn_list   (tlenv_T* env, tlval_T* sexpr);
tlval_T* btinfn_max    (tlenv_T* env, tlval_T* args);
tlval_T* btinfn_min    (tlenv_T* env, tlval_T* args);
tlval_T* btinfn_mod    (tlenv_T* env, tlval_T* args);
tlval_T* btinfn_mul    (tlenv_T* env, tlval_T* args);
tlval_T* btinfn_pow    (tlenv_T* env, tlval_T* args);
tlval_T* btinfn_sqrt   (tlenv_T* env, tlval_T* args);
tlval_T* btinfn_sub    (tlenv_T* env, tlval_T* args);
tlval_T* btinfn_tail   (tlenv_T* env, tlval_T* qexpr);

#endif
