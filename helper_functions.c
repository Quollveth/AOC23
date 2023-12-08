////////////////////////////////////////////////
///////// SPLIT A STRING AT A CHARACTER ////////
////////////////////////////////////////////////

char **splitString(char *input, char delim) {
    int i = 0;
    char **temp = malloc((sizeof(char*) * strlen(input)) + 1);
    char *token = strtok(input, &delim);

    while(token != NULL) {
        temp[i] = token;
        token = strtok(NULL, &delim);
        i++;
    }
    
    temp[i] = NULL;
    return temp;
}

/* example usage
  char *testString = strdup("test-string"); //cannot be a string literal as it cannot be modified
  char **strings = splitString(testString,'-'); //already returns pointer to start of array, can be assigned to char **
  free(testString); 
  free(strings); //remember to free all memory after
*/

///////////////////////////////////////////////////////////
///// FINDS EVERY NUMBER IN THE STRING AND USES IT ////////
///////////////////////////////////////////////////////////

void parse(char *string){
    bool counted;
    int temp;
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
        //use the number here
    }
}