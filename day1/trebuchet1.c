#include <stdio.h>
#include <ctype.h>

int addDigits(char *string);

int main(int argc, char *argv[]){
  FILE *input = fopen(argv[1],"r");
  if(input == NULL){
    printf("failed");
    return 1;
  }

  char buffer[100];
  int sum = 0;

  while(1){
    buffer[0] = '\0';
    fgets(buffer,100,input);
    sum += addDigits(buffer);
    if(feof(input)){
      break;
    }
  }

  fclose(input);
  printf("%i\n",sum);
}

int addDigits(char *string){
  int n1 = 0, n2 = 0,i,j;
  char c;
  for(i=0,j=0;string[i]!='\0';i++){
    c = string[i];
    if(isdigit(c)){
      if(!j){
        n1 = c-'0';
        j++;
      }
      n2 = c-'0';
    }
  }
  printf("%i|%i = %i\n",n1,n2,(n1*10)+n2);
  return (n1*10) + n2;
}