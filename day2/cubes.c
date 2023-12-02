#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define TOTAL_R 12
#define TOTAL_G 13
#define TOTAL_B 14

int checkGame(char *in);
int matchLUT(char *string);

char *LUT[3] = {"red","green","blue"};

int main(int argc, char *argv[]){
  char *test = "test";
  FILE *input = fopen(argv[1],"r");
  //FILE *input = fopen(test,"r");

  if(input == NULL){
    printf("failed :(");
    return 1;
  }

  char buffer[200];
  int sum = 0;

  while(1){
    buffer[0] = '\0';
    fgets(buffer,200,input);
    //
    sum += checkGame(buffer);
    //
    if(feof(input)){
      break;
    }
  }

  fclose(input);

  printf("\n\n%i",sum);
}

int checkGame(char *in){
  printf("\n%s",in);

  int
    gameId,
    maxR = 0,
    maxG = 0,
    maxB = 0,
    tmp = -1;

  bool counted = false;


  /*
  1- find a number in the string
  2- see what word comes after that number, it's either a color or a colon
  3- if it's a colon the number is the game id, set it and skip the loop
  3.5- if it's a color check if it's the highest value we got for that color, if yes set it to that color
  */
  for(int i=0;in[i]!='\0';i++){    
    /*
    find a number:
      two conditions are being checked, is the current character a digit, and do we already have a number?
    */

    if(isdigit(in[i]) && !counted){ //we have a digit and no number, start parsing the number
      tmp = 0;
      counted = true;
      //run through the string from now on and add every digit found to the number
      for(int j=i;isdigit(in[j]);j++){
        tmp = (tmp*10)+in[j]-'0';
      }
      //printf("found number: %i",tmp);
    }

    if(isdigit(in[i]) && counted){ //we have a digit and a number, the digit is from that number and we can ignore it
      continue;
    }

    //if we reached here we don't have a digit and have a number, so this is the number of something
    counted = false;

    if(in[i]==':'){ //if we reach a colon the number is the game id
      //printf(" game id\n");
      gameId = tmp;
      continue;
    }

    //modified from day 1 solution, builds a string and checks if it matches any in an array
    char buf[10] = "\0";
    int mLut;

    for(int j=i;j<i+6;j++){
      if(in[j]=='\0'){
        //don't continue past the end of string
        break;
      }
      //do the building and comparing here
      strncat(buf,&in[j],1);
      mLut = matchLUT(buf);   
      if(mLut != -1){
        //found a match, stop checking
        break;
      }
    }

    if(mLut != -1){
      //printf(" %s\n",LUT[mLut]);
    }

    switch(mLut){
      case 0:
        maxR = maxR>tmp?maxR:tmp;
        break;
      case 1:
        maxG = maxG>tmp?maxG:tmp;
        break;
      case 2:
        maxB = maxB>tmp?maxB:tmp;
        break;
      default:
        break;
    }

  }
  /////////////////////////////////////////////
  // once here we have all the numbers and can check if the game is possible or not

  printf("[id %i | %i red %i green %i blue]\n",gameId,maxR,maxG,maxB);

/*
  if(
    maxR <= TOTAL_R &&
    maxG <= TOTAL_G &&
    maxB <= TOTAL_B
  ){
    printf("possible\n");
    return gameId;
  }

  printf("impossible\n");
  return 0;
*/

}

int matchLUT(char *string){
  for(int i=0;i<3;i++){
    if(strcmp(string,LUT[i])==0){      
      return i;
    }
  }
  return -1;
}