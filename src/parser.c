#include <stdio.h>
#include <stdlib.h>

#include "mpc.h"

// If we are compiling on Windows...
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

char* readline(char* prompt)
{
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);

    char* cpy = malloc(strlen(buffer) + 1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy) -1] = '\0';

    return cpy;
}

void add_history(char* unused) {}

// Otherwise, include the 'editline' headers.
#else
#include <editline/readline.h>
#include <editline/history.h>
#endif


int main()
{
    mpc_parser_t* Number     = mpc_new("numb");
    mpc_parser_t* Operator   = mpc_new("oper");
    mpc_parser_t* Expression = mpc_new("expr");
    mpc_parser_t* Tmul       = mpc_new("tmul");

    mpca_lang(MPCA_LANG_DEFAULT,
            " \
                numb: /-?[0-9]+/ ;                      \
                oper: '+' | '-' | '*' | '/' ;           \
                expr: <numb> | '(' <oper> <expr>+ ')' ; \
                tmul: /^/ <oper> <expr>+ /$/ ;          \
            ",
            Number, Operator, Expression, Tmul);

    puts("TMUL: Version 0.1.0");
    puts("Press CTRL+c to Exit\n");

    while(1)
    {
        char* input = readline("tmul > ");
        add_history(input);

        mpc_result_t r;
        if(mpc_parse("<stdin>", input, Tmul, &r))
        {
            mpc_ast_print(r.output);
            mpc_ast_delete(r.output);
        }
        else
        {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }

    mpc_cleanup(4, Number, Operator, Expression, Tmul);

    return 0;
}
