#include <signal.h>

#include "builtin.h"
#include "eval.h"
#include "parser.h"
#include "repl.h"
#include "type.h"


tlenv_T* env;

static void read_from_file(int argc, char** argv);
static void interrupt(int sign);

int
main(int argc, char** argv)
{
    signal(SIGINT, interrupt);

    env = tlenv_new();
    tlenv_init(env);
    parser_init();

    if(argc < 2)
        start_repl(env);
    else
        read_from_file(argc, argv);

    return 0;
}

static void
read_from_file(int argc, char** argv)
{
    for(int i = 1; i < argc; i++)
    {
        tlval_T* args = tlval_add(tlval_sexpr(), tlval_str(argv[i]));
        tlval_T* res  = btinfn_load(env, args);

        if(res->type == TLVAL_ERR)
            tlval_print(res);

        tlval_del(res);
    }
}

static void
interrupt(int sign)
{
    free(env);
    parser_cleanup();

    exit(0);
}
