#include <stdio.h>
#include "lexer.h"

int main(void) {
    tokenlist *tl = new_tokenlist();
    printf("size: %d\n",(int) tl->size);
    add_token(tl, "ls");
    add_token(tl, "-la");
    printf("token[0]: %s\n", tl->items[0]);
    printf("token[1]: %s\n", tl->items[1]);
    printf("enter your input: ");
    char *input = get_input();
    printf("your input is: %s\n", input);
    free(input);
    return 0;
}
