#pragma once

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    char   **items;  
    size_t   size;   
} tokenlist;

char *get_input(void); //read user input
tokenlist *get_tokens(char *input); //split input into tokens
tokenlist *new_tokenlist(void); //create empty tokenlist
void add_token(tokenlist *tokens, char *item); //add one token to list
void free_tokens(tokenlist *tokens); //free all token memory
void expand_env_vars(tokenlist *tokens); //expand $VAR tokens
void expand_tilde(tokenlist *tokens); //expand ~ to home directory
