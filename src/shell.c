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
        free(input);
    }
}
