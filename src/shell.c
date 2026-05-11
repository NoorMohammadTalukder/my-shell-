#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include "lexer.h"
#include "exec.h"
#include "shell.h"

// print shell prompt
static void print_prompt(void) {
    const char *user = getenv("USER");
    const char *machine = getenv("HOSTNAME");
    const char *pwd = getenv("PWD");
    if (!user){
      user = "unknown";
    }
    if (!machine){
      machine = "unknown";
    } 
    if (!pwd){
      pwd = "";
    }	
    printf("%s@%s:%s> ", user, machine, pwd);
    fflush(stdout);
}



// struct to hold parsed cmd
typedef struct {
    char **argv;
    char *in_file;
    char *out_file;
} parsed_cmd;

// parse tokens into cmd with input and output redirection
parsed_cmd parse_command(tokenlist *tokens) {
    parsed_cmd cmd;
    cmd.in_file  = NULL;
    cmd.out_file = NULL;
    cmd.argv = malloc(sizeof(char *) * (tokens->size + 1));
    int argc = 0;

    for (size_t i = 0; i < tokens->size; i++) {
        if (strcmp(tokens->items[i], "<") == 0) {
            if (i + 1 < tokens->size)
                cmd.in_file = tokens->items[i++];
        }
        else if (strcmp(tokens->items[i], ">") == 0) {
            if (i + 1 < tokens->size)
                cmd.out_file = tokens->items[i++];
        }
        else {
            cmd.argv[argc++] = tokens->items[i];
        }
    }
    cmd.argv[argc] = NULL;
    return cmd;
}


//temporary shell loop
void run_shell(void) {
    while (1) {
        print_prompt();
        char *input = get_input();
        if (input == NULL) {
            printf("\n");
            break;
        }
        printf("you typed: %s\n", input);
        tokenlist *tl = get_tokens(input);
        expand_env_vars(tl);
        expand_tilde(tl);
        parsed_cmd cmd = parse_command(tl);
        printf("argv[0]: %s\n", cmd.argv[0]);
        printf("in_file: %s\n",  cmd.in_file  ? cmd.in_file  : "none");
        printf("out_file: %s\n", cmd.out_file ? cmd.out_file : "none");
        free(cmd.argv);
        free_tokens(tl);
        free(input);
    }
}