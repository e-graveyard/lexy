#include <stdio.h>
#include <stdlib.h>


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


#define PROGRAM_NAME      "tmul"
#define PROGRAM_VERSION   "v0.1.0"
#define PROMPT_DISPLAY    PROGRAM_NAME " > "
