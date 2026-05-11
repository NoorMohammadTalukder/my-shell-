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

// run cmd with redirection and fork
int execute_parsed_cmd(const char *fullpath, parsed_cmd *cmd, int background, const char *input_cmdline) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        // handle input redirection
        if (cmd->in_file) {
            int fd = open(cmd->in_file, O_RDONLY);
            if (fd < 0) {
                perror("input redirection");
                exit(1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        // handle output redirection
        if (cmd->out_file) {
            int fd = open(cmd->out_file, O_WRONLY | O_CREAT | O_TRUNC, 0600);
            if (fd < 0) {
                perror("output redirection");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        execv(fullpath, cmd->argv);
        perror("execv");
        exit(1);
    }

    // no redirection
    if (!cmd->in_file && !cmd->out_file) {
        return run_foreground(fullpath, cmd->argv);
    }

    waitpid(pid, NULL, 0);
    return 0;
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
        // printf("argv[0]: %s\n", cmd.argv[0]);
        // printf("in_file: %s\n",  cmd.in_file  ? cmd.in_file  : "none");
        // printf("out_file: %s\n", cmd.out_file ? cmd.out_file : "none");
        char *fullpath = find_executable(cmd.argv[0]);
        if (fullpath == NULL) {
            printf("command not found: %s\n", cmd.argv[0]);
        } else {
            execute_parsed_cmd(fullpath, &cmd, 0, input);
        }
        free(cmd.argv);
        free_tokens(tl);
        free(input);
    }
}