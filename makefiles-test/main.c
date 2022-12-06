#include <stdio.h>




int main(int argc,char* argv){
  int n= 96;
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
/*
int main(int argc,char* argv){
  int n= 100;
  for(int i =0 ;i<n;i++){
    printf("dep%d : dep%d\n",i,i+1);
    printf("  sleep 1 ; echo 'dep%d is executed'\n",i);
  }
  return 0;
}
*/