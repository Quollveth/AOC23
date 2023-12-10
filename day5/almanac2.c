#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <limits.h>

#define MAX_RANGES 31
#define MAX_MAPS 7

enum category {SEED,SOIL,FERTILIZER,WATER,LIGHT,TEMPERATURE,HUMIDITY,LOCATION};

typedef struct{
    uint32_t sourceStart;
    uint32_t destStart; //unused for seeds
    uint32_t length;
    enum category cat; //unused for maps
} Range;

#define END(X) ((X).sourceStart + (X).length)
#define PEND(X) ((X)->sourceStart + (X)->length) //pointer version

typedef struct node {
  Range data;
  struct node *next;
}Node;

typedef struct list{
  struct node *head;
  struct node *tail;
} List;

char **splitString(char *input,char delim); //creates an array of strings composed of an original string split around a delim character

bool inRange(uint32_t key, Range range); //is key inside range
uint32_t convertRange(uint32_t key, Range range); //passes a key through a range
uint32_t convertMap(uint32_t key, Range map[MAX_RANGES]); //passes a key through a map
int rangeOverlap(Range r1, Range r2); //is r1 inside r2

//list functions
Node *initNode(void);
List *initList(void);
Range deList(List *q, Node *element);
void enList(List *q, Range data);

int main(int argc, char *argv[])
{

  char *test = {"test"};
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

      almanac[i][j].cat = 0; //UNUSED
	  }
  }

  //since we can have an arbitrary number of ranges as they get split during the process seeds are a linked list instead of an array
  List *seedRanges = initList();    

  char buffer[251];
  buffer[0] = '\0';
  fgets(buffer,250,input);

  char **t1 = splitString(buffer,':');
  char **t2 = splitString(t1[1],' ');

  for(int i=0;t2[i]!=NULL;i+=2){
    Range temp;
    temp.sourceStart = atoi(t2[i]);
    temp.length = atoi(t2[i+1]);
    temp.cat = SEED;

    temp.destStart = 0; //UNUSED
    enList(seedRanges,temp);
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

/////////////////////////////////////////////////////////////////  
/////////////////////////////////////////////////////////////////  
//
  //each seed is a range in a category
  //- check all ranges starting in the current category
  //- if the seed range does not overlap any of these if moves to the next category unchanged
  //- if it does overlap one it's either a partial or a total overlap
  //- in a total overlap the seed is entirely inside the range, so it gets shifted by that range
  //- in a partial overlap the seed is partially inside the range, in that case the seed range is split in two, and both ranges are sent back to processing
  //- by sending a seed back to processing instead of shifting it on the spot we can handle a case where it overlaps multiple ranges, in this scenario it will be split in the order each range is processed
  //- a range can be sent back to processing by being removed from the List and added back
  //- when every range in the List has been processed (by reaching the end of the List) that category is finished, we can reset to the head of the List and move on to the next category

  //outer loop, go through each category once
  enum category currentCategory = SEED;
  while(currentCategory != LOCATION){
    Range *current;
    //navigate through the List
    Node *ptr = seedRanges->head;
    while(ptr != NULL){
      current = &ptr->data;

      //if a seed is not in this category ignore it
      if(current->cat != currentCategory){
        ptr = ptr->next;
        continue;
      }

      //go through each range in this category and see if the current seed overlaps it
      int overlap = -1; //which range is overlapped, -1 is none
      int overlapType;
      for(int i=0;i<MAX_RANGES;i++){
        overlapType = rangeOverlap(*current,almanac[currentCategory][i]);
        if(overlapType){
          //no overlap returns 0 and fails the condition
          overlap = i;
          break; //we only want one overlap at a time
        }
      }

      if(overlapType == 0){
        //there is no overlap, range passes unchanged
        current->cat++;
        ptr = ptr->next;
        continue;
      }
      Range *mappedRange = &almanac[currentCategory][overlap];
      if(overlapType == 1 && current->length <= mappedRange->length){
        //total overlap, seed range is entirely inside mapped range, it receives an offset and continue
        current->sourceStart = convertRange(current->sourceStart,*mappedRange);
        current->cat++;
        ptr = ptr->next;
        continue;
      }

      //partial overlap, remove range from queue, split it into 3 and throw these in the list
      Range tmp1, tmp2, tmp3, seedRange = ptr->data;

      deList(seedRanges,ptr);
      ptr = seedRanges->head;

      tmp1.destStart = 0; //unused, gets ird of garbage data
      tmp2.destStart = 0;
      tmp3.destStart = 0;

      if(seedRange.sourceStart <= mappedRange->sourceStart && (END(seedRange)) >= (PEND(mappedRange))){
        //seed range encompasses mapped range       
        tmp1.sourceStart = seedRange.sourceStart;
        tmp2.sourceStart = mappedRange->sourceStart;
        tmp3.sourceStart = mappedRange->sourceStart + mappedRange->length;

        tmp1.length = tmp2.sourceStart - tmp1.sourceStart;
        tmp2.length = mappedRange->length;
        tmp3.length = seedRange.length - (tmp1.length + tmp2.length);

        tmp1.cat = currentCategory;
        tmp2.cat = currentCategory;
        tmp3.cat = currentCategory;

        enList(seedRanges,tmp1);
        enList(seedRanges,tmp2);
        enList(seedRanges,tmp3);

        continue;
      }
      //we need to know if the start or the end of the seed range is overlapping the mapped range
      bool overlapStart = seedRange.sourceStart >= mappedRange->sourceStart;

      if(overlapStart){
        tmp1.sourceStart = mappedRange->sourceStart;
        tmp2.sourceStart = seedRange.sourceStart;

        tmp1.length = seedRange.sourceStart - mappedRange->sourceStart;
        tmp2.length = seedRange.length - tmp1.length;
      } else {
        tmp1.sourceStart = seedRange.sourceStart;
        tmp2.sourceStart = mappedRange->sourceStart;

        tmp1.length = mappedRange->sourceStart - seedRange.sourceStart;
        tmp2.length = seedRange.length - tmp1.length;
      }

      tmp1.cat = currentCategory;
      tmp2.cat = currentCategory;

      enList(seedRanges,tmp1);
      enList(seedRanges,tmp2);  

      printf("");
    }
    //entire List processed, increase category
    currentCategory++;
  }
/////////////////////////////////////////////////////////////////  
/////////////////////////////////////////////////////////////////  

  //and that's it, just get the smallest value in the List
  Node *temp = seedRanges->head;
  uint32_t min = INT32_MAX;
  
  while(temp != NULL){
    if(temp->data.sourceStart < min){
      min = temp->data.sourceStart;
    }
    temp = temp->next;
  }
  printf("%i\n",min); 

}

int rangeOverlap(Range r1, Range r2){
  //r1 starts inside r2
  bool r1St = r1.sourceStart >= r2.sourceStart && r1.sourceStart < END(r2);
  //r1 ends inside r2
  bool r1En = END(r1) > r2.sourceStart && END(r1) <= END(r2);
  //r2 starts inside r1
  bool r2St = r2.sourceStart >= r1.sourceStart && r2.sourceStart < END(r1);
  //r2 ends inside r1
  bool r2En = END(r2) > r1.sourceStart && END(r2) <= END(r1);

  //range 1 entirely inside range 2 or range 2 entirely inside range 1
  if((r1St && r1En) || (r2St && r2En)){
    return 1;
  }
  //range 1 entirely outside range 2 and range 2 entirely outside range 1
  if((!r1St && !r1En) && (!r2St && !r2En)){
    return 0;
  }
  //partial overlap
  return -1;
}

void enList(List *q, Range data){
  Node *newNode = initNode();
  if(newNode == NULL){
    printf("Memory allocation failed\n");
    return;
  }
  newNode->data = data;
  if(q->head == NULL){ //first in list
    q->head = newNode;
    q->tail = newNode;
  } else { //append to end
    q->tail->next = newNode;
    q->tail = newNode;
  }
}

Range deList(List *q, Node *element){
    Range emptyRange = {0,0,0};
    if (q->head == NULL) {
        // The list is empty
        return emptyRange;
    }

    if (q->head == element) {
        // The element to be removed is at the head of the list
        q->head = element->next;
        if (q->tail == element) {
            // The element is also the tail of the list
            q->tail = NULL;
        }
    } else {
        // The element is not at the head of the list
        Node *current = q->head;
        while (current->next != NULL && current->next != element) {
            current = current->next;
        }

        if (current->next == element) {
            // The element to be removed was found in the list
            current->next = element->next;
            if (q->tail == element) {
                // The element is the tail of the list
                q->tail = current;
            }
        } else {
            // The element to be removed was not found in the list
            return emptyRange;
        }
    }

    Range data = element->data;
    free(element);
    return data;
}

List *initList(void){
  List *tempQ = malloc(sizeof(List));
  if(tempQ == NULL){
    return NULL;
  }
  tempQ->head = NULL;
  tempQ->tail = NULL;

  return tempQ;
}

Node *initNode(void){
  Node *temp = malloc(sizeof(Node));
  if(temp == NULL){
    return NULL;
  }
  temp->data.destStart = 0;
  temp->data.sourceStart = 0;
  temp->data.length = 0;
  temp->next = NULL;

  return temp;
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