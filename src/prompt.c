#include "eval.h"
#include "prompt.h"

char* prompt()
{
    char* input = readline("tmul > ");
    add_history(input);

    return input;
}

char* err_msg(int e)
{
    char* msg;

    switch(e)
    {
        case LERR_DIV_ZERO:
            msg = "Division by zero.";
            break;

        case LERR_BAD_OP:
            msg = "Invalid operator.";
            break;

        case LERR_BAD_NUM:
            msg = "Invalid number.";
            break;
    }

    return msg;
}

void printlv(tlval v)
{
    fputs("=> ", stdout);
    switch(v.type)
    {
        case TLVAL_NUM:
            printf("%f", v.number);
            break;

        case TLVAL_ERR:
            printf("Error: %s", err_msg(v.error));
            break;
    }
    fputs("\n\n", stdout);
}
