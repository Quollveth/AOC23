#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <limits.h>

#define MAX_RANGES 31
#define MAX_SEEDS 20
#define MAX_MAPS 7

typedef struct{
    uint32_t sourceStart;
    uint32_t destStart;
    uint32_t length;
} Range;

bool inRange(uint32_t key, Range range);
uint32_t convertRange(uint32_t key, Range range);
void parse(char *string);
char **splitString(char *input,char delim);
uint32_t convertMap(uint32_t key, Range map[MAX_RANGES]);

int main(int argc, char *argv[])
{
  char *test = {"input"};
  FILE *input = fopen(test,"r");

  if(input == NULL){
    printf("failed :(");
    return 1;
  }

  Range almanac[MAX_MAPS][MAX_RANGES];
  for(int i=0;i<MAX_MAPS;i++){
	  for(int j=0;j<MAX_RANGES;j++){
	  	almanac[i][j].destStart = 0;
	  	almanac[i][j].sourceStart = 0;
	  	almanac[i][j].length = 0;
	  }
  }

  uint32_t seeds[MAX_SEEDS];
  for(int i=0;i<MAX_SEEDS;i++){
    seeds[i] = 0;
  }

  char buffer[251];
  buffer[0] = '\0';
  fgets(buffer,250,input);

  char **t1 = splitString(buffer,':');
  char **t2 = splitString(t1[1],' ');

  for(int i=0;t2[i]!=NULL;i++){
    seeds[i] = atoi(t2[i]);
  }

  int currentMap = -1; //it would be best to start at 0, but then i'd have to find a different way to do my things since the current way increments it as soon as the loop starts
  int i=0;
  while(1){
    buffer[0] = '\0';
    fgets(buffer,200,input);
    //each map has a empty line before it    
    if(strcmp(buffer,"\n")==0){ //compares to \n instead of isempty since fgets reads until it finds a newline
      currentMap++;
      //the line immediately following the empty one is which map we're at
      fgets(buffer,200,input);
      i=0;
      continue;
    }    
    t1 = splitString(buffer,' ');
    almanac[currentMap][i].destStart = atoi(t1[0]);
    almanac[currentMap][i].sourceStart = atoi(t1[1]);
    almanac[currentMap][i].length = atoi(t1[2]);
    i++;
    //
    if(feof(input)){
      break;
    }
  }
  fclose(input);

  for(int i=0;i<7;i++){
    //each map in the almanac
    for(int j=0;j<MAX_SEEDS;j++){
      //each seed
      if(seeds[j] != 0){
        seeds[j] = convertMap(seeds[j],almanac[i]);
      }
    }
  }

  printf("\n");
  int min = INT_MAX;
    for (int i = 0; i < MAX_SEEDS; i++) {
        if (seeds[i] > 0 && seeds[i] < min) {
            min = seeds[i];
        }
    }
  min == INT_MAX ? 0 : min;

  printf("%i\n",min);
}

uint32_t convertMap(uint32_t key, Range map[MAX_RANGES]){
  uint32_t temp;
  for(int i=0;i<MAX_RANGES;i++){
    temp = convertRange(key,map[i]);
    if(temp != key){
      return temp;
    }
  }
  return key;
}

char **splitString(char *input, char delim) {
    int i = 0;
    int capacity = 3; // Start with space for 3 tokens
    char **temp = malloc(sizeof(char*) * capacity);
    char delimStr[2] = {delim, '\0'}; // Make the delimiter a string
    char *token = strtok(input, delimStr);

    while(token != NULL) {
        if(i >= capacity) {
            capacity *= 2; // Double the capacity
            temp = realloc(temp, sizeof(char*) * capacity); // Allocate more memory
        }
        temp[i] = token;
        token = strtok(NULL, delimStr);
        i++;
    }
    temp[i] = NULL;
    return temp;
}

bool inRange(uint32_t key, Range range){
    return key >= range.sourceStart && key < range.sourceStart + range.length;
}

uint32_t convertRange(uint32_t key, Range range){
    if(!inRange(key,range)){
        return key;
    }
    return range.destStart + (key - range.sourceStart);
}