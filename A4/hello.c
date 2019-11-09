#include <stdio.h>
#include "fib.h"
#define A_CONST_NUM 10
 
// A quick test program
 
 
int main(int argc, char* argv[]) {
    int a = 4;
    if (a < A_CONST_NUM) {
        for (int i = 0; i < 10; i++) {
            printf("Hello! %d \n", i);
            for (int i = 0; i < 10; i++) {
            }
        }
        fib(a);
        return 0;
    }
}