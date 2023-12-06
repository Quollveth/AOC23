

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_CARDS 220
#define NUMS_PER_CARD 10
#define WINNERS_PER_CARD 25

typedef struct card{
  int id;
  int points;
  int numbers[NUMS_PER_CARD];
  int winners[WINNERS_PER_CARD];
}Card;


void parseCard(char *input,Card *CARDS);
bool isInArray(int key, int *array);

int main(int argc, char *argv[])
{
  char *test = "input";
  FILE *input = fopen(test,"r");

  if(input == NULL){
    printf("failed :(");
    return 1;
  }

  char buffer[201];

  Card CARDS[MAX_CARDS];

  for(int i=0;i<MAX_CARDS;i++){
    CARDS[i].id = 0;
  }

  while(1){
    buffer[0] = '\0';
    fgets(buffer,200,input);
    //process each line
    parseCard(buffer,CARDS);
//    printf("%s\n",buffer);
    //
    if(feof(input)){
      break;
    }
  }
  fclose(input);

  //cards tallied up
/*
we start with 1 of each card
when card X is evaluated we`ll get 1 copy of the next N cards
N being the number of matches in X
since X+1 will run right next and do the same process we only need to add 1 copy per card
so the number of copies of X we have will be added to the next N cards
*/
  //start with an array initialized to 1
  //each index is the amount we have of that card
  int cardTotals[MAX_CARDS];
  for(int i=0;i<MAX_CARDS;i++){
    cardTotals[i] = 1;
  }
  int matches;

  for(int i=1;i<=MAX_CARDS;i++){
    if(CARDS[i].id == 0){
      break;
    }
    matches = 0;
    for(int j=0;j<NUMS_PER_CARD;j++){
      int num = CARDS[i].numbers[j];
      if(isInArray(num,CARDS[i].winners)){
        matches++;
      }
    }
    //after this card has been evaluated we add the number of copies we have of it to the next N = matches cards
    for(int k=1;k<=matches;k++){
      if(i+k<MAX_CARDS){ //don't go beyond the end of the array
        cardTotals[i+k] += cardTotals[i];
      }
    }
  }  
  int sum = 0;
  for (int i=0; i < MAX_CARDS; i++) {
      if (CARDS[i].id == 0)
          continue;
      sum += cardTotals[i];
  }
  printf("%i",sum);
}

bool isInArray(int key, int *array){
  for(int i=0;i<WINNERS_PER_CARD;i++){
    if(array[i]==key){
      return true;
    }
  }
  return false;
}

void parseCard(char *string,Card *CARDS){
  bool counted = false, card;

  int temp = 0;
  int currentCard;
  int currentNumber;
  for(int i=0;string[i]!='\0';i++){
    char c = string[i];

    //find a number
    if(isdigit(c) && !counted){
      temp = 0;
      counted = true;

      for(int j=i;isdigit(string[j]);j++){
        temp = (temp*10)+string[j]-'0';
      }
    }

    if(isdigit(c) && counted){
      continue;
    }

    counted = false;

    switch (c)
    {
    case ':': //found colon, number is card ID
      currentCard = temp;
      CARDS[currentCard].id = temp;
      currentNumber = 0;
      card = true;
      temp = 0;
      break;

    case '|': //found separating bar, we are now counting winning numbers
      temp = 0;
      card = false;
      currentNumber = 0;
      break;
    
    default:
      break;
    }

    //add number to the proper array
    if(temp==0){
      continue;
    }
    if(card){
      CARDS[currentCard].numbers[currentNumber] = temp;
      currentNumber++;
//      printf("card ");
    } else {
      CARDS[currentCard].winners[currentNumber] = temp;
      currentNumber++;
//      printf("winning ");
    }
//    printf("number [%i] added to card [%i]\n",temp,currentCard);

    if(c == ' ' || c == ':' || c == '|'){
      temp = 0;
    }
  }
}

