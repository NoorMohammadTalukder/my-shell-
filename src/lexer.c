#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//empty tokenlist
tokenlist *new_tokenlist(void) {
    tokenlist *tokens = (tokenlist *)malloc(sizeof(tokenlist));
    tokens->size = 0;
    tokens->items = (char **)malloc(sizeof(char *));
    tokens->items[0] = NULL;
    return tokens;
}
// add one word to tokenlist
void add_token(tokenlist *tokens, char *item) {
    int i = tokens->size;
    tokens->items = (char **)realloc(tokens->items, (i + 2) * sizeof(char *));
    tokens->items[i] = (char *)malloc(strlen(item) + 1);
    tokens->items[i + 1] = NULL;
    strcpy(tokens->items[i], item);
    tokens->size += 1;
}
// read input line from user
char *get_input(void) {
    char *buffer = NULL;
    int bufsize = 0;
    char line[8];

    while (fgets(line, 8, stdin) != NULL) {
        int addby = 0;
        char *newln = strchr(line,'\n');

        if (newln != NULL){
            addby = newln - line;
	}
        else{
            addby = 8 - 1;
	}

        buffer = (char *)realloc(buffer, bufsize + addby + 1);
        memcpy(&buffer[bufsize], line, addby);
        bufsize += addby;

        if (newln != NULL)
            break;
    }

    if (buffer == NULL && feof(stdin))
        return NULL;

    buffer[bufsize] = '\0';
    return buffer;
}
// split the input into tokens
tokenlist *get_tokens(char *input) {
    char *buf = (char *)malloc(strlen(input) + 1);
    strcpy(buf, input);

    tokenlist *tokens = new_tokenlist();
    char *word = strtok(buf, " ");

    while (word != NULL) {
        add_token(tokens, word);
        word = strtok(NULL, " ");
    }

    free(buf);
    return tokens;
}
// free memory in tokenlist
void free_tokens(tokenlist *tokens) {
    for (int i = 0; i < (int)tokens->size; i++)
        free(tokens->items[i]);
    free(tokens->items);
    free(tokens);
}
// madeup strdup, using in expand_env_vars
static char *str_copy(const char *s) {
    size_t len = strlen(s) + 1;
    char *copy = malloc(len);
    if (copy == NULL) {
        perror("malloc");
        exit(1);
    }
    memcpy(copy, s, len);
    return copy;
}

// replace $VAR tokens with actual env values
void expand_env_vars(tokenlist *tokens) {
    char *tok = NULL;
    char *val = NULL;

    for (int i = 0; i < (int)tokens->size; i++) {
        tok = tokens->items[i];

        if (tok[0] != '$'){
            continue;
        }

        val = getenv(tok+1);
        free(tokens->items[i]);
        tokens->items[i] = str_copy(val ? val : "");
    }
}
