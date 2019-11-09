# include "sample2.h"

static int sfunc() {
   int a = 1;
   if (a < 2) {
      printf("sfunc in sample2");
   }
}

void pf() {
   int a = 1;
   printf("print func");
   sfunc();
}