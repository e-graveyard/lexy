#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

int main()
{
    puts("TMUL: Version 0.1.0");
    puts("Press CTRL+c to Exit\n");

    while(1)
    {
        char* input = readline("tmul > ");
        add_history(input);

        printf("=> %s\n\n", input);
        free(input);
    }

    return 0;
}
