#include "eval.h"
#include "parser.h"
#include "prompt.h"

int main()
{
    puts("TMUL: Version 0.1.0");
    puts("Press CTRL+c to Exit\n");

    mpc_parser_t* parser = init_parser();

    for(;;)
    {
        char* input = prompt();

        mpc_result_t r;
        if(mpc_parse("<stdin>", input, parser, &r))
        {
            long result = eval(r.output);

            printf("=> %ld\n\n", result);
            mpc_ast_delete(r.output);
        }
        else
        {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }

    mpc_cleanup(4,
            number_symbol,
            operator_symbol,
            expression_definition,
            lisp_expression);

    return 0;
}
