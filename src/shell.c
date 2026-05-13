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

// max background jobs
#define MAX_JOBS 10
// job struct
typedef struct {
    int job_id;
    pid_t pid;
    char *cmdline;
    int active;
} job;
static job job_table[MAX_JOBS];
static int next_job_id = 1;

//find empty slot
static int find_free_job_slot(void) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (!job_table[i].active){
          return i;
        } 
    }
    return -1;
}

//copy cmdline string
static char *dup_cmdline(const char *input) {
    size_t n = strlen(input);
    char *s = malloc(n + 1);
    if (!s) {
      return NULL;
    }
    memcpy(s, input, n);
    s[n] = '\0';
    return s;
}

//add job to job table
static void start_job(pid_t pid, const char *input_cmdline) {
    int slot = find_free_job_slot();
    if (slot < 0) {
        printf("error: too many background jobs\n");
        return;
    }
    job_table[slot].job_id  = next_job_id++;
    job_table[slot].pid     = pid;
    job_table[slot].cmdline = dup_cmdline(input_cmdline);
    job_table[slot].active  = 1;
    printf("[%d] %d\n", job_table[slot].job_id, (int)pid);
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

    for (size_t i = 0; i < tokens->size; ++i) {
        if (strcmp(tokens->items[i], "<") == 0) {
            if (i + 1 < tokens->size)
                {
                  cmd.in_file = tokens->items[++i];
                }
        }
        else if (strcmp(tokens->items[i], ">") == 0) {
            if (i + 1 < tokens->size)
                {
                  cmd.out_file = tokens->items[i++];
                }
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
    if (background) {
        start_job(pid, input_cmdline);  
        return 0;
    }
    waitpid(pid, NULL, 0);
    return 0;
}


//print all active background jobs
static void print_jobs(void) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (!job_table[i].active) {
          continue;
        };
        printf("[%d] %d %s\n",
               job_table[i].job_id,
               (int)job_table[i].pid,
               job_table[i].cmdline ? job_table[i].cmdline : "");
    }
    fflush(stdout);
}

//check for finished background jobs
static void reap_background_jobs(void) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (!job_table[i].active) {
          continue;
        };
        int status = 0;
        pid_t r = waitpid(job_table[i].pid, &status, WNOHANG);
        if (r == 0) {
          continue;
        };

        if (r == job_table[i].pid || (r < 0 && errno == ECHILD)) {
            printf("[%d] + done %s\n",
                   job_table[i].job_id,
                   job_table[i].cmdline ? job_table[i].cmdline : "");
            fflush(stdout);
            job_table[i].active = 0;
            free(job_table[i].cmdline);
            job_table[i].cmdline = NULL;
        }
    }
}
//check for & token at end
static int handle_background_token(tokenlist *tokens, int *background) {
    *background = 0;
    for (size_t i = 0; i < tokens->size; i++) {
        if (strcmp(tokens->items[i], "&") == 0) {
            if (i != tokens->size - 1) {
                printf("error: & must be at end\n");
                return 0;
            }
            *background = 1;
            tokens->items[i] = NULL;
            tokens->size--;
            return 1;
        }
    }
    return 1;
}
//temporary shell loop
void run_shell(void) {
    while (1) {
        reap_background_jobs();
        print_prompt();
        char *input = get_input();
        if (input == NULL) {
            printf("\n");
            break;
        }
        //printf("you typed: %s\n", input);
        tokenlist *tl = get_tokens(input);
        expand_env_vars(tl);
        expand_tilde(tl);
        int background = 0;
        handle_background_token(tl, &background);
        if (strcmp(tl->items[0], "exit") == 0) {
          free_tokens(tl);
          free(input);
          exit(0);
        }
        if (strcmp(tl->items[0], "jobs") == 0) {
            print_jobs();
            free_tokens(tl);
            free(input);
            continue;
        }
        if (strcmp(tl->items[0], "cd") == 0) {
            const char *path = tl->size > 1 ? tl->items[1] : getenv("HOME");
            if (path && chdir(path) != 0)
                perror("cd");
            free_tokens(tl);
            free(input);
            continue;
        }
        parsed_cmd cmd = parse_command(tl);
        // printf("argv[0]: %s\n", cmd.argv[0]);
        // printf("in_file: %s\n",  cmd.in_file  ? cmd.in_file  : "none");
        // printf("out_file: %s\n", cmd.out_file ? cmd.out_file : "none");
        char *fullpath = find_executable(cmd.argv[0]);
        if (fullpath == NULL) {
            printf("command not found: %s\n", cmd.argv[0]);
        }else if (!cmd.in_file && !cmd.out_file) {
            run_foreground(fullpath, cmd.argv);
        } else {
            execute_parsed_cmd(fullpath, &cmd, 0, input);
        }
        free(fullpath);
        free(cmd.argv);
        free_tokens(tl);
        free(input);
    }
}
