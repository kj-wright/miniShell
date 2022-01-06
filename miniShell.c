// Kyle Wright - Systems Programming
// Program2: Minishell
// 11 / 8 / 2020

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include <signal.h>

void parseArgs(char*, char**, int, int*);

int main() {
  int done = 0;
  int bufferSize = 100;
  
  while (!done) {
    //print current directoty
    char cwd[bufferSize];
    getcwd(cwd, bufferSize);
    printf("%s> ", cwd);

    //get input
    char str[bufferSize];
    fgets(str, bufferSize, stdin);

    //prossess input into tokens
    char* tokens[bufferSize];
    int nargs;
    parseArgs(str, tokens, bufferSize, &nargs);

    //search for special tokens

    int in = 0;
    int out = 0;
    int bg = 0;

    char* inFile;
    char* outFile;
    
    for(int i = 0; i < nargs; i++){
      if(tokens[i][0] == '<'){
	in = 1;
	inFile = tokens[i+1];
	tokens[i] = NULL;
	
      } else if(tokens[i][0] == '>'){
	out = 1;
	outFile = tokens[i+1]; 
	tokens[i] = NULL;
	
      } else if(tokens[i][0] == '&'){
	bg = 1;
	tokens[i] = NULL;
      }
      
    }

    //process tokens without loop
    if (strcmp(tokens[0], "exit") == 0) {
      done = 1;
    } else if (strcmp(tokens[0], "cd") == 0) {
      if (chdir(tokens[1]) != 0) {
        printf("Invalid directory.\n");
      }
    } else { //fork and commands
      int pid = fork();
      
      if (pid == 0) { //child
	if (in == 1) {
	  in = 0;
	  freopen(inFile, "r", stdin);
	}

	if (out == 1) {
	  out = 0;
	  freopen(outFile, "w", stdout); 
	}
	
        int rc = execvp(tokens[0], tokens);
        printf("Program not found.\n");
	exit(1);
	
      } else { //parent
	if (bg == 0) {
	  int status;
	  int result = waitpid(pid, &status, 0); 

	} else {	  
	  signal(SIGCHLD,SIG_IGN);

	}
	
      }

    }
    
  }
  
}

// buffer is the input string of characters
// args is the output array of arguments.  It has already been created with argsSize slots.
// nargs as the number filled in that is passed back
void parseArgs(char *buffer, char** args, int argsSize, int *nargs) {
  char *bufArgs[argsSize];
  char **cp;
  char *wbuf;
  int i, j;

  wbuf=buffer;
  bufArgs[0]=buffer;
  args[0]=buffer;

  for(cp=bufArgs; (*cp=strsep(&wbuf, " \n\t")) != NULL ;){
    if ((*cp != '\0') && (++cp >= &bufArgs[argsSize]))
      break;
  }

  for (j=i=0; bufArgs[i]!=NULL; i++){
    if(strlen(bufArgs[i])>0)
      args[j++]=bufArgs[i];
  }

  // Add the NULL as the end argument because we need that for later
  *nargs=j;
  args[j]=NULL;
}
