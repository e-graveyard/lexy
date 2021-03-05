#ifndef LEXY_PARSER
#define LEXY_PARSER

#include "mpc.h"


mpc_parser_t* Number;
mpc_parser_t* String;
mpc_parser_t* Comment;
mpc_parser_t* Symbol;
mpc_parser_t* SExpr;
mpc_parser_t* QExpr;
mpc_parser_t* Atom;
mpc_parser_t* Lisp;

void parser_init();
void parser_safe_cleanup();

#endif
