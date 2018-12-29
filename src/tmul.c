#include <signal.h>

#include "tmul.h"
#include "eval.h"
#include "parser.h"

void interrupt(int sign);
void tl_print(tlval_T* t);
void tl_sexp_print(tlval_T* t, char openc, char closec);
char* prompt();

static parser_T p;

int main(void)
{
    signal(SIGINT, interrupt);

    puts("TMUL: Version ");
    puts("Press CTRL+c to Exit\n");

    p = init_parser();

    for(;;)
    {
        char* input = prompt();

        mpc_result_t r;
        if(mpc_parse("<stdin>", input, p.TLisp, &r))
        {
            tlval_T* t = tlval_eval(tlval_read(r.output));
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
    mpc_cleanup(5,
            p.Number,
            p.Symbol,
            p.SExpr,
            p.PExpr,
            p.TLisp);

    exit(0);
}

char* prompt()
{
    char* input = readline("tmul > ");
    add_history(input);

    return input;
}

void tl_sexp_print(tlval_T* t, char openc, char closec)
{
    putchar(openc);
    for(int i = 0; i < t->counter; i++)
    {
        tl_print(t->cell[i]);

        if(i != (t->counter - 1))
            putchar(' ');
    }
    putchar(closec);
}

void tl_print(tlval_T* t)
{
    fputs("=> ", stdout);
    switch(t->type)
    {
        case TLVAL_NUM:
            printf("%f", t->number);
            break;

        case TLVAL_ERR:
            printf("Error: %s.", t->error);
            break;

        case TLVAL_SYM:
            printf("%s", t->symbol);
            break;

        case TLVAL_SEXPR:
            tl_sexp_print(t, '(', ')');
            break;
    }
    fputs("\n\n", stdout);
}
