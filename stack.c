#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "stack.h"
DEFINE_ELEMENT_TYPE(int, stack);

int MAX;

int main(int argc, char *argv[])
{
    char s[100];
    struct stack *S = calloc(1, sizeof(struct stack));
    printf("%d\n", init(S, MAX));
    int a ,b ;

    while (scanf("%s", s) != EOF) { /* linux 中 scanf 是自己抛弃换行啊 */
        switch (s[0])
        {
        case '+':
            push(S, pop(S) + pop(S));
            break;
        
        case '-':
            // a = pop(S);
            // b = pop(S); 
            push(S, - pop(S) + pop(S));
            break;
        
        case '*':
            push(S, pop(S) * pop(S));
            break;

        case '#':
            goto lab_ret;
            break;

        default:
            push(S, atoi(s));
            break;
        }
        
    }
lab_ret:
    printf("%d\n", pop(S));
    clear(S);
    free(S);
    return 0;
}