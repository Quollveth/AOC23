/*
- throw the entire input into a huge 2d array
- navigate the array until we find a number
- check if each digit has a symbol in a 3x3 section of the array around it
- if yes add to sum
*/

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_WIDTH 300
#define MAX_HEIGHT 300

char ENGINE[MAX_WIDTH][MAX_HEIGHT];

bool isPart(int line, int start, int digits,int tLine, int tCol);
bool inLimit(int x, int y,int maxX, int maxY);

int main(int argc, char *argv[]){
  char *test = {"input"};
  FILE *input = fopen(test,"r");

  if(input == NULL){
    printf("failed :(");
    return 1;
  }

  int line=0, col=0;
  char c;
  while((c = fgetc(input)) != EOF){
    ENGINE[line][col] = c;
    col++;
    if(c=='\n'){
      col=0;
      line++;
    }
  }
  if (c != '\n') {
    line++;
  }

  //now we go through it again but looking for numbers
  //adapted from day 2 solution
  bool counted = false;
  int tmp, sum = 0;

  for(int i=0;i<line;i++){
    for(int j=0;j<col;j++){
      if(ENGINE[i][j]=='\n'){
        break;
      }
      
      
      if(isdigit(ENGINE[i][j]) && !counted){
        counted = true;
        int temp = 0;  
        int l = 0;
        for(int k=j;isdigit(ENGINE[i][k]);k++,l++){
          temp = (temp*10) + (ENGINE[i][k] - '0');
        }
        printf("found number: %i",temp);

        if(isPart(i,j,l,line,col)){
          printf(" part\n");
          sum += temp;
        } else {
          printf("\n");
        }
      }

      if(isdigit(ENGINE[i][j]) && counted){
        continue;
      }      
      counted = false;
    }
  }
  fclose(input);
  printf("\n\n%i\n",sum);
}

bool isPart(int line, int start, int digits,int tLine, int tCol){
  /*
  -iterate over each digit checking if it has something that's neither a period or a digit in all adjacent squares
  -this effectvely works like a blur filter, go through every pixel around the main one and average it's colors, but in here we are simply checking if they meet a condition or not
  -basic solution: starting at [x,y] we check [x-1,y-1] [x-1,y] [x-1,y+1], etc
  -we loop 9 times to know which position we're at and via some simple logic can find an offset value for i and j that are either -1, 0 or 1
  -main problem: if we are at the edges or in a corner we can't offset in certain directions as that will take us out of bounds, so an additional check is required before actually attempting to access the array 

  -this is probably a very stupid way to solve this problem, for once we're checking the same places multiple times, i am however, too bad at this to make it better
  */
  int x = line, y; //y will be the starting pixel + an offset from the main loop
  int digitOffset, xOffset, yOffset;
  char c;

  for(int i=0;i<digits;i++){
    y = start + i;
    for(int j=1;j<=9;j++){
      /*
      -here we set the offsets, the offset for the y coordinate will increase by 1 every 3 iterations while the offset for the x coordinate will increase by 1 three times and then reset (when the y offset increases)
      -so y goes -1 -1 -1 0 0 0 1 1 1
      -and x goes -1 0 1 -1 0 1 -1 0 1
      */
      xOffset = (j%3) - 1;

      if(j<4){yOffset = 1;}
      else if(j>6){yOffset = -1;}
      else {yOffset = 0;}

      if(inLimit(x+xOffset,y+yOffset,tLine,tCol)){
        c = ENGINE[x+xOffset][y+yOffset];
        if(!isdigit(c) && c!='.' && !isspace(c)){
          return true;
        }
      }
    }
  }
  return false;
}

bool inLimit(int x, int y,int maxX, int maxY){
    return (x >= 0 && x < maxX) && (y >= 0 && y < maxY);
}