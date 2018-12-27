#include "mpc.h"

static mpc_parser_t* number_symbol;
static mpc_parser_t* operator_symbol;
static mpc_parser_t* expression_definition;
static mpc_parser_t* lisp_expression;

mpc_parser_t* init_parser();
