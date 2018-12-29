#include "mpc.h"

typedef struct parser_S
{
    mpc_parser_t* Number;
    mpc_parser_t* Symbol;
    mpc_parser_t* SExpr;
    mpc_parser_t* PExpr;
    mpc_parser_t* TLisp;
}
parser_T;

parser_T init_parser();
