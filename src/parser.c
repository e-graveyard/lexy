#include "parser.h"

parser_T init_parser()
{
    parser_T p;

    p.Number = mpc_new("numb");
    p.Symbol = mpc_new("symb");
    p.SExpr  = mpc_new("sexp");
    p.QExpr  = mpc_new("qexp");
    p.PExpr  = mpc_new("pexp");
    p.TLisp  = mpc_new("lisp");

    mpca_lang(MPCA_LANG_DEFAULT,

            " numb: /-?[0-9]+\\.?[0-9]*/ ; "
            "                              "
            " symb: ('+' | \"add\") |      "
            "       ('-' | \"sub\") |      "
            "       ('*' | \"mul\") |      "
            "       ('/' | \"div\") |      "
            "       ('%' | \"mod\") |      "
            "       ('^' | \"pow\") |      "
            "       \"sqrt\" |             "
            "       \"min\" |              "
            "       \"max\" ;              "
            "                              "
            " sexp: '(' <pexp>* ')' ;      "
            "                              "
            " qexp: \"'(\" <pexp>* ')' ;   "
            "                              "
            " pexp: <numb> | <symb> |      "
            "       <sexp> | <qexp> ;      "
            "                              "
            " lisp: /^/ <pexp>* /$/ ;      ",

            p.Number,
            p.Symbol,
            p.SExpr,
            p.QExpr,
            p.PExpr,
            p.TLisp);

    return p;
}
