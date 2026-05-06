#include <stdio.h>
#include "lexer.h"

int main(void) {
    tokenlist *tl = new_tokenlist();
    printf("size: %d\n",(int) tl->size);
    return 0;
}
