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