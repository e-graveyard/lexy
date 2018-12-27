#include<stdio.h>

static char input[2048];

int main()
{
    puts("TMUL: Version 0.1.0");
    puts("Press CTRL+c to Exit\n");

    while(1)
    {
        fputs("tmul > ", stdout);

        fgets(input, 2048, stdin);

        printf("=> %s", input);
    }

    return 0;
}
