/*

   Copyright (c) 2018-2021 Caian R. Ertl <hi@caian.org>

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation
   files (the "Software"), to deal in the Software without
   restriction, including without limitation the rights to use,
   copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following
   conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
   OTHER DEALINGS IN THE SOFTWARE.

 */

#include <getopt.h>
#include <signal.h>

#include "meta.h"

#include "eval.h"
#include "parser.h"
#include "fmt.h"
#include "type.h"

#define PROMPT_DISPLAY  " ] "
#define PROMPT_RESPONSE "~> "


#if __has_include(<editline/readline.h>)
#include <editline/readline.h>

#else
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
#endif


lenv_T* lexy_current_env = NULL;

lval_T* btinfn_load (lenv_T* env, lval_T* args);
lval_T* lval_pop    (lval_T* t, size_t i);


static void lexy_clean_exit(int sign)
{
    parser_safe_cleanup();

    if (lexy_current_env != NULL)
        free(lexy_current_env);

    exit(0);
}


static int lexy_help_message(int ret_code, char* bin_filename)
{
    printf("\nUsage %s [options] [script.lisp] [arguments]\n\n"
           "-h : show this help\n"
           "-v : print the lexy version\n"
           "-r : print release information\n"
           "-d : enable the debug mode\n"
           "-e code : evaluate and execute a string of lexy\n"
           "\nThis project can be found at <https://github.com/caian-org/lexy>\n\n",
           bin_filename);

    return ret_code;
}


static int lexy_version()
{
    printf("%s\n", PROGRAM_VERSION);
    return 0;
}


static int lexy_release()
{
    printf("compiled @ %s, %s\n"
           "build w/ : %s\n"
           "target   : %s/%s\n",
           COMPILATION_TIME, COMPILATION_DATE, COMPILED_WITH, TARGET_KERNEL, TARGET_ARCH);

    return 0;
}


static void lexy_ast_parse(char* input, void (*inline_routine)(lval_T*, lval_T**), lval_T** err)
{
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lisp, &r))
    {
        lval_T* parsed_input = lval_read(r.output);
        mpc_ast_delete(r.output);

        inline_routine(parsed_input, err);
        return;
    }

    *err = lval_err(mpc_err_string(r.error));
    mpc_err_delete(r.error);
}


static void lexy_repl_inline_seg(lval_T* parsed_input, lval_T** err)
{
    lval_T* t = lval_eval(lexy_current_env, parsed_input);

    GREY_TXT(1, "%s", PROMPT_RESPONSE);
    lval_print(lexy_current_env, t);

    lval_del(t);
}


static void lexy_repl_start()
{
    BOLD_TXT(TRUE, "\nlexy %s %s/%s - '(help)' for documentation\n",
             PROGRAM_VERSION, TARGET_KERNEL, TARGET_ARCH);

    GREY_TXT(TRUE, "%s\n", "press CTRL+c to exit");

    lexy_current_env->exec_type = LEXEC_REPL;

    while (TRUE)
    {
        printf("\n");

        char* input = readline(ANSI_STYLE_BOLD PROMPT_DISPLAY ANSI_RESET);
        add_history(input);

        lval_T* err = NULL;
        lexy_ast_parse(input, lexy_repl_inline_seg, &err);

        if (err != NULL) {
            lval_print(lexy_current_env, err);
            lval_del(err);
        }

        free(input);
    }
}


static void lexy_cli_eval_inline_seg(lval_T* parsed_input, lval_T** err)
{
    while (parsed_input->counter)
    {
        lval_T* e = lval_eval(lexy_current_env, lval_pop(parsed_input, 0));

        if (e->type == LTYPE_ERR)
        {
            if (*err != NULL)
                lval_del(*err);

            *err = lval_err(e->error);
        }

        lval_del(e);
    }
}

static int lexy_cli_eval_code(char* input)
{
    lval_T* err = NULL;
    lexy_ast_parse(input, lexy_cli_eval_inline_seg, &err);

    if (err != NULL)
    {
        lval_print(lexy_current_env, err);
        lval_del(err);

        return 1;
    }

    return 0;
}


static int lexy_file_exec(char* filep)
{
    lexy_current_env->exec_type = LEXEC_FILE;

    lval_T* args = lval_add(lval_sexpr(), lval_str(filep));
    lval_T* res  = btinfn_load(lexy_current_env, args);

    if (res->type == LTYPE_ERR)
        lval_print(lexy_current_env, res);

    int retcode = res->error != NULL ? 1 : 0;

    lval_del(res);
    return retcode;
}


int main(int argc, char** argv)
{
    signal(SIGINT, lexy_clean_exit);

    char* input_code = NULL;
    bool cli_flag_debug = FALSE;

    char* bin_filename = argv[0];
    int choice;

    /* ... */
    while ((choice = getopt(argc, argv, ":hvrde:")) != -1)
    {
        switch(choice)
        {
            case 'h':
                return lexy_help_message(0, bin_filename);

            case 'v':
                return lexy_version();

            case 'r':
                return lexy_release();

            case 'd':
                cli_flag_debug = TRUE;
                break;

            case 'e':
                input_code = optarg;
                break;

            case ':': /* -e without operand */
                printf("\nOption -%c requires a string operand\n", optopt);
                return lexy_help_message(2, bin_filename);

            case '?':
                printf("Unknown flag -%c\n", optopt);
                return lexy_help_message(2, bin_filename);
        }
    }

    /* ... */
    lexy_current_env = lenv_new();
    lenv_init(lexy_current_env);
    parser_init();

    /* ... */
    if (input_code != NULL) {
        int retcode = lexy_cli_eval_code(input_code);

        return retcode;
    }

    /* ... */
    short int remaining_args = argc - optind;
    switch(remaining_args)
    {
        case 0:
            lexy_repl_start();
            break;

        case 1:
            return lexy_file_exec(argv[argc - 1]);

        default:
            RED_TXT(TRUE, "\n%s\n", "Arguments to lexy programs are not implemented yet.");
            exit(2);
            break;
    }

    return 0;
}
