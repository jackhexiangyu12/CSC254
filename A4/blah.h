#include <stdio.h>
#define NO_INLINE __attribute__((noinline))

inline int blah() {
    int x;
    scanf("%d", &x);
    return x;
}