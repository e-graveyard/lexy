#include "mpc.h"

#define GRAMMAR \
    " \
    numb: /-?[0-9]+\\.?[0-9]*/ ;   \
                                   \
    oper: ('+' | \"add\") |        \
          ('-' | \"sub\") |        \
          ('*' | \"mul\") |        \
          ('/' | \"div\") |        \
          ('%' | \"mod\") |        \
          ('^' | \"pow\") |        \
          \"sqrt\" ;               \
                                   \
    expr: <numb> |                 \
          '(' <oper> <expr>+ ')' ; \
                                   \
    lisp: /^/ <oper> <expr>+ /$/ ; \
    "

static mpc_parser_t* number_symbol;
static mpc_parser_t* operator_symbol;
static mpc_parser_t* expression_definition;
static mpc_parser_t* lisp_expression;

mpc_parser_t* init_parser();
