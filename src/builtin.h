#ifndef LEXY_BUILTIN
#define LEXY_BUILTIN

#include "type.h"


/* ... */
#define SEE_REF " | see: #"

#define BTIN_ADD_DESCR     "mathematical addition operator"              SEE_REF "add"
#define BTIN_SUB_DESCR     "mathematical subtraction operator"           SEE_REF "sub"
#define BTIN_MUL_DESCR     "mathematical multiplication operator"        SEE_REF "mul"
#define BTIN_DIV_DESCR     "mathematical division operator"              SEE_REF "div"
#define BTIN_MOD_DESCR     "mathematical modulo operator"                SEE_REF "mod"
#define BTIN_POW_DESCR     "mathematical exponentiation operator"        SEE_REF "pow"
#define BTIN_SQRT_DESCR    "mathematical square root operator"           SEE_REF "sqrt"
#define BTIN_MAX_DESCR     "gets the highest value from provided args"   SEE_REF "max"
#define BTIN_MIN_DESCR     "gets the lowest value from provided args"    SEE_REF "min"
#define BTIN_HEAD_DESCR    "gets the value at the head of a given list"  SEE_REF "head"
#define BTIN_TAIL_DESCR    "gets the values at the tail of a given list" SEE_REF "tail"
#define BTIN_LIST_DESCR    "makes a list from all provided args"         SEE_REF "new"
#define BTIN_JOIN_DESCR    "join provided lists into a single list"      SEE_REF "join"
#define BTIN_IF_DESCR      "conditional expression construct"            SEE_REF "if"
#define BTIN_EQ_DESCR      "equality operator"                           SEE_REF "eq"
#define BTIN_NE_DESCR      "inequality operator"                         SEE_REF "ne"
#define BTIN_GT_DESCR      "greater-than operator"                       SEE_REF "gt"
#define BTIN_GE_DESCR      "greater-or-equal-than operator"              SEE_REF "ge"
#define BTIN_LT_DESCR      "less-than operator"                          SEE_REF "lt"
#define BTIN_LE_DESCR      "less-or-equal-than operator"                 SEE_REF "le"
#define BTIN_USE_DESCR     "injects code into the execution environment" SEE_REF "use"
#define BTIN_LET_DESCR     "inner-scoped dynamic binding"                SEE_REF "let"
#define BTIN_LETC_DESCR    "inner-scoped constant binding"               SEE_REF "letc"
#define BTIN_GLOBAL_DESCR  "global-scoped dynamic binding"               SEE_REF "global"
#define BTIN_GLOBALC_DESCR "global-scoped constant binding"              SEE_REF "globalc"
#define BTIN_EVAL_DESCR    "evaluates a quoted value as an expression"   SEE_REF "eval"
#define BTIN_LAMBDA_DESCR  "lambda (anonymous) function operator"        SEE_REF "lambda"
#define BTIN_ERROR_DESCR   "raises an exception"                         SEE_REF "error"
#define BTIN_PRINT_DESCR   "sends a message to the STDOUT device"        SEE_REF "print"


/* ... */
lval_T* btinfn_add     (lenv_T* env, lval_T* args);
lval_T* btinfn_div     (lenv_T* env, lval_T* args);
lval_T* btinfn_eval    (lenv_T* env, lval_T* qexpr);
lval_T* btinfn_global  (lenv_T* env, lval_T* qexpr);
lval_T* btinfn_globalc (lenv_T* env, lval_T* qexpr);
lval_T* btinfn_head    (lenv_T* env, lval_T* qexpr);
lval_T* btinfn_join    (lenv_T* env, lval_T* qexprv);
lval_T* btinfn_lambda  (lenv_T* env, lval_T* qexpr);
lval_T* btinfn_let     (lenv_T* env, lval_T* qexpr);
lval_T* btinfn_letc    (lenv_T* env, lval_T* qexpr);
lval_T* btinfn_list    (lenv_T* env, lval_T* sexpr);
lval_T* btinfn_max     (lenv_T* env, lval_T* args);
lval_T* btinfn_min     (lenv_T* env, lval_T* args);
lval_T* btinfn_mod     (lenv_T* env, lval_T* args);
lval_T* btinfn_mul     (lenv_T* env, lval_T* args);
lval_T* btinfn_pow     (lenv_T* env, lval_T* args);
lval_T* btinfn_sqrt    (lenv_T* env, lval_T* args);
lval_T* btinfn_sub     (lenv_T* env, lval_T* args);
lval_T* btinfn_tail    (lenv_T* env, lval_T* qexpr);
lval_T* btinfn_cmp_gt  (lenv_T* env, lval_T* args);
lval_T* btinfn_cmp_ge  (lenv_T* env, lval_T* args);
lval_T* btinfn_cmp_lt  (lenv_T* env, lval_T* args);
lval_T* btinfn_cmp_le  (lenv_T* env, lval_T* args);
lval_T* btinfn_cmp_eq  (lenv_T* env, lval_T* args);
lval_T* btinfn_cmp_ne  (lenv_T* env, lval_T* args);
lval_T* btinfn_if      (lenv_T* env, lval_T* args);
lval_T* btinfn_load    (lenv_T* env, lval_T* args);
lval_T* btinfn_error   (lenv_T* env, lval_T* args);
lval_T* btinfn_print   (lenv_T* env, lval_T* args);

#endif
