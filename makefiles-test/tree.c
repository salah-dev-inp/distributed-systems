#include <stdio.h>
#include <stdlib.h>

// generate makefiles of type tree
int main(int argc,char** argv){
  int n= atoi(argv[1]) ;
  int i= 0;
  int j= 1;
  while(j<n){
    printf("dep%d : dep%d dep%d \n",i,j,j+1);
    printf("  sleep 1 ; echo 'dep%d is executed'\n",i);
    i++;
    j++;
    j++;
  }
  for (int k =i; k<n; k++){
    printf("dep%d :\n",k);
    printf("  sleep 1 ; echo 'dep%d is executed'\n",k);
  }
}
