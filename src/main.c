#include <stdio.h>
#include "lexer.h"

int main(void) {
    printf("enter your input: ");
    char *input = get_input();
    printf("your input is: %s\n", input);
    tokenlist *tl = get_tokens(input);
    expand_env_vars(tl);
    expand_tilde(tl);
    for (int i = 0; i < (int)tl->size; i++){
        printf("token[%d]: %s\n", i, tl->items[i]);
    }
    free_tokens(tl);
    free(input);
    return 0;
}
