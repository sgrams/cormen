#include <stdio.h>
#include <stdlib.h>

long long fib (long long n) {
  int i;
  long long size = n+1;
  long long fibTab[size];
  fibTab[0]=1;
  fibTab[1]=1;
  fibTab[2]=2;
  
  for (i=n; ; i++){
    fibTab[i] = fibTab[i/2] + fibTab[(i/2)+1];
  }

  return fibTab[n];
}

int main (void) {
  long long n;
  scanf("%lli", &n);
  printf("%lli\n", fib(n));
  return 0;
}

