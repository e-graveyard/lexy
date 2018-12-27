#include "parser.h"

mpc_parser_t* init_parser()
{
    number_symbol         = mpc_new("numb");
    operator_symbol       = mpc_new("oper");
    expression_definition = mpc_new("expr");
    lisp_expression       = mpc_new("lisp");

    mpca_lang(MPCA_LANG_DEFAULT,

            " numb: /-?[0-9]+\\.?[0-9]*/ ;   "
            "                                "
            " oper: ('+' | \"add\") |        "
            "       ('-' | \"sub\") |        "
            "       ('*' | \"mul\") |        "
            "       ('/' | \"div\") |        "
            "       ('%' | \"mod\") |        "
            "       ('^' | \"pow\") |        "
            "       \"sqrt\" |               "
            "       \"min\" |                "
            "       \"max\" ;                "
            "                                "
            " expr: <numb> |                 "
            "       '(' <oper> <expr>+ ')' ; "
            "                                "
            " lisp: /^/ <oper> <expr>+ /$/ ; ",

            number_symbol,
            operator_symbol,
            expression_definition,
            lisp_expression);

    return lisp_expression;
}
