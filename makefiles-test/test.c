#include <stdio.h>
#include <stdlib.h>

int main(int argc,char** argv){
  double t1,t2;
  t1=0;
  t2=2;
  FILE *fp = fopen(argv[1] , "a");
  if (fp == NULL)
    exit(EXIT_FAILURE);
  fprintf(fp,"%f\n",t2 - t1);
  fclose(fp);
}