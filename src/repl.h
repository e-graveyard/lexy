#include <stdio.h>
#include <stdlib.h>


#define PROGRAM_NAME    "lexy"
#define PROGRAM_VERSION "v0.1.0"
#define PROMPT_DISPLAY  " ] "

void start_repl();

#ifdef _WIN32
#include <string.h>

static char buffer[2048];

void add_history(char* unused) {}

char* readline(char* prompt)
{
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);

    char* cpy = malloc(strlen(buffer) + 1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy) -1] = '\0';

    return cpy;
}

#else
#include <editline/readline.h>

#endif
