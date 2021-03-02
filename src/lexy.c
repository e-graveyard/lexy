#include <signal.h>

#include "builtin.h"
#include "eval.h"
#include "parser.h"
#include "repl.h"
#include "type.h"


lenv_T* lexy_current_env;

static void read_from_file(int argc, char** argv);
static void interrupt(int sign);


int
main(int argc, char** argv)
{
    signal(SIGINT, interrupt);

    lexy_current_env = lenv_new();
    lenv_init(lexy_current_env);
    parser_init();

    if(argc < 2)
        start_repl(lexy_current_env);
    else
        read_from_file(argc, argv);

    return 0;
}


static void
read_from_file(int argc, char** argv)
{
    for(int i = 1; i < argc; i++)
    {
        lval_T* args = lval_add(lval_sexpr(), lval_str(argv[i]));
        lval_T* res  = btinfn_load(lexy_current_env, args);

        if(res->type == LTYPE_ERR)
            lval_print(res);

        lval_del(res);
    }
}


static void
interrupt(int sign)
{
    free(lexy_current_env);
    parser_cleanup();

    exit(0);
}
