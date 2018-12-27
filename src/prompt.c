#include "prompt.h"

char* prompt()
{
    char* input = readline("tmul > ");
    add_history(input);

    return input;
}
