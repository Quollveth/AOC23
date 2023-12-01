#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int addDigits(char *string);
int findNumber(char *input);

char *NUMBERS[10] = {"zero","one","two","three","four","five","six","seven","eight","nine"};

int main(int argc, char *argv[]){

  FILE *input = fopen(argv[1],"r");

  if(input == NULL){
    printf("failed :(");
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

int matchesAny(char *input){ //strcmp with an array of strings
  for(int i=0;i<=9;i++){
    if(strcmp(input,NUMBERS[i])==0){
      return i;
    }
  }
  return -1;
}

int findNumber(char *input){
  /*
  returns first digit in string, either in actual digits or written down
  if no numbers can be find returns -1
  eg:
    1zzzzzz9 - 1
    zzzzsevenzzz4 - 7
    eightwo - 8
    five000000 - 5   
  */
  if(isdigit(input[0])){ //we only need to check first digit since this function is called for each character anyways
    return input[0] - '0';
  }
  char str[100] = "\0";
  /*
  we slowly build a string from each character and check if it's a match
  so it received string is two each step is:
    str = "t" - false
    str = "tw" - false
    str = "two" - true
  */
  for(int i=0;input[i]!='\0';i++){
    strncat(str,&input[i],1);
    int match = matchesAny(str);
    if(match != -1){
      return match;
    }
    
  }

  return -1;
}

int addDigits(char *string){
  int n1 = 0, n2 = 0,i,j;

  for(i=0,j=0;string[i]!='\0';i++){

    int temp = findNumber(&string[i]);
    if(temp == -1){
      continue;
    }

    if(!j){
      n1 = temp;
      j++;
    }
    n2 = temp;
  }
  printf("%i|%i = %i\n",n1,n2,(n1*10)+n2);
  return (n1*10) + n2;
}
