#include <signal.h>

#include "fmt.h"
#include "tmul.h"
#include "eval.h"
#include "parser.h"

void interrupt(int sign);
void tl_print(tlval_T* t);
void tl_sexp_print(tlval_T* t, char* openc, char* closec);
char* prompt();

static parser_T p;
static tlenv_T* env;

int main(void)
{
    signal(SIGINT, interrupt);

    printf("%s: version %s\n", PROGRAM_NAME, PROGRAM_VERSION);
    psout("press CTRL+c to exit");

    p = parser_init();

    env = tlenv_new();
    tlenv_init(env);

    for(;;)
    {
        char* input = prompt();

        mpc_result_t r;
        if(mpc_parse("<stdin>", input, p.Lisp, &r))
        {
            tlval_T* t = tlval_eval(env, tlval_read(r.output));

            psout("=> ");
            tl_print(t);
            tlval_del(t);

            mpc_ast_delete(r.output);
        }
        else
        {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }

    return 0;
}

void interrupt(int sign)
{
    free(env);
    mpc_cleanup(6,
            p.Number,
            p.Symbol,
            p.SExpr,
            p.QExpr,
            p.Atom,
            p.Lisp);

    exit(0);
}

char* prompt()
{
    psout("\n\n");
    char* input = readline(PROMPT_DISPLAY);
    add_history(input);

    return input;
}

void tl_sexp_print(tlval_T* t, char* openc, char* closec)
{
    psout(openc);
    for(int i = 0; i < t->counter; i++)
    {
        tl_print(t->cell[i]);

        if(i != (t->counter - 1))
            psout(" ");
    }
    psout(closec);
}

void tl_num_print(float n)
{
    if(isfint(n))
    {
        printf("%ld", (long)round(n));
    }
    else
    {
        printf("%f", n);
    }
}

void tl_print(tlval_T* t)
{
    switch(t->type)
    {
        case TLVAL_FUN:
            if(!t->builtin)
            {
                psout("<function>");
            }
            else
            {
                psout("(lambda ");
                tl_print(t->formals);
                psout(" ");
                tl_print(t->body);
                psout(")");
            }
            break;

        case TLVAL_NUM:
            tl_num_print(t->number);
            break;

        case TLVAL_ERR:
            printf("Error: %s.", t->error);
            break;

        case TLVAL_SYM:
            printf("%s", t->symbol);
            break;

        case TLVAL_QEXPR:
           tl_sexp_print(t, "'(", ")");
            break;

        case TLVAL_SEXPR:
            tl_sexp_print(t, "(", ")");
            break;
    }
}
