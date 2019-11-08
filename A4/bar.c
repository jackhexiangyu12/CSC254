#include "blah.h"

static int foo() {
    // Notice how there's a 4:7 ratio of asm to source lines here - this ratio
    // can be aribtrary
    int x = 0;
    for (int i = 0; i < 100; ++i) {
        x += i;
    }
    printf("%d\n", x);
    return x;
}

static void NO_INLINE bar() {
    // This is just to make sure this function doesn't get optimized out
    asm("");
}

static void NO_INLINE complicated_func() {
    int x;
    scanf("%d\n", &x);
    volatile int y = x;

    do {
        for (int i = 0; i < 2; ++i) {
            y += i;
            printf("%d\n", y);
        }
    } while (y == 7);

    printf("%d\n", y);
}

// int main() {
//     // which bar?
//     bar();
//     complicated_func();
//     // Notice that the source before this comes from a different file
//     blah();
//     return 0;
// }