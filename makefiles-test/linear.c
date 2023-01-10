#include <stdio.h>
#include <stdlib.h>

// generate makefiles of type linear
int main(int argc,char** argv){
  int n= atoi(argv[1]);
  printf("dep0 :");
  for(int i =1 ;i<n;i++){
    printf(" dep%d",i);
  }
  printf("\n");
  printf("  sleep 1 ; echo 'dep0 is executed'\n");
  for(int i =1 ;i<n;i++){
    printf("dep%d :\n",i);
    printf("  sleep 1 ; echo 'dep%d is executed'\n",i);
  }
  return 0;
}