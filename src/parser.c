#include "parser.h"

parser_T init_parser()
{
    parser_T p;

    p.Number = mpc_new("number");
    p.Symbol = mpc_new("symbol");
    p.SExpr  = mpc_new("sexpr");
    p.QExpr  = mpc_new("qexpr");
    p.Atom  = mpc_new("atom");
    p.Lisp  = mpc_new("lisp");

    mpca_lang(MPCA_LANG_DEFAULT,

            " number: /-?[0-9]+\\.?[0-9]*/ ;                    "
            " symbol: /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;        "
            " sexpr:  '(' <atom>* ')' ;                         "
            " qexpr:  \"'(\" <atom>* ')' ;                      "
            " atom:  <number> | <symbol> | <sexpr> | <qexpr> ;  "
            " lisp:  /^/ <atom>* /$/ ;                          ",

            p.Number,
            p.Symbol,
            p.SExpr,
            p.QExpr,
            p.Atom,
            p.Lisp);

    return p;
}
