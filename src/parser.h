#include "mpc.h"


typedef struct parser_S
{
    mpc_parser_t* Number;
    mpc_parser_t* Symbol;
    mpc_parser_t* SExpr;
    mpc_parser_t* QExpr;
    mpc_parser_t* Atom;
    mpc_parser_t* Lisp;
}
parser_T;

parser_T parser_init();
