//William Dahl
//001273655
//ICSI 402 Systems Programming
//April 17th, 2018

//imports
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>

//pipe function
//pipes the commands given together
//takes in a 2d string array containing the commands given
int pipe_command(char ***cmds){
  pid_t process; //process
  int   file_descriptor = 0; //file discrptor
  int   pipe_ptr[2]; //pipe pointer
  //loops through the array
  while(*cmds != NULL){
    pipe(pipe_ptr);//prepares the pipe pointer to for piping
    //forks the processes
    //chactes a fork error
    if ((process = fork()) == -1){
      perror("fork");
      return 0;
    }

    //checks if process is the child process
    if(process == 0){
      //makes the file discrptor as std in
      dup2(file_descriptor, 0);
      //checks if the next index in the array is not null
      if(*(cmds + 1) != NULL){
        //makes pipe_ptr[1] stdout
        dup2(pipe_ptr[1], 1);
      }

      close(pipe_ptr[0]);//closes unused stdin
      //exectues the command
      execvp((*cmds)[0], *cmds);
      perror("bad exe");
    }

    //exectues if process is a child process
    else{
      wait(NULL);//waits for printing to help formatnig
      close(pipe_ptr[1]);//closes the unused pipe
      file_descriptor = pipe_ptr[0]; // sets the file discrptor to stdin
      cmds++;//goes to next cmd 
    }
  }

  return 0;
}

//main function
//gets the commands into a 2d string array and ready for pipeing
//takes in the commands on exuctution
void main(int argc, char * argv[]){ 
  //checks if no arguments are given
  if(argc == 1){
    fprintf(stderr, "Usage: pipe cmd cmd-arg1 ... [ @ cmd cmd-arg1 ... ]\n");
    exit(0);
  }

  //checks if there are pipes where there should be commands
  if(strcmp(argv[1], "@") == 0 || strcmp(argv[argc-1], "@") == 0){
    fprintf(stderr, "pipe: syntax error near \'@\'\n\n");
    fprintf(stderr, "Usage: pipe cmd cmd-arg1 ... [ @ cmd cmd-arg1 ... ]\n");
    exit(0);
  }

  int pipe_count = 0;//counts pipes
  char buffer[10];//holds commands
  int command_count = 1; // counts comands
  //loops through the comand given and counts the number od pipes
  for (int i = 1; i < argc; i++){
    //checks if command is a pipe
    if(strcmp(argv[i], "@") == 0){
      //checks if next command is also a pipe
      //if so prints error message
      if((i+1) != argc){
        if(strcmp(argv[i+1], "@") == 0){
          fprintf(stderr, "pipe: syntax error near \'@\'\n\n");
          fprintf(stderr, "Usage: pipe cmd cmd-arg1 ... [ @ cmd cmd-arg1 ... ]\n");
          exit(0);
        }
      }

      pipe_count++;//increases pipe count
      command_count++; // increases command count
    }
  }
  
  //checsks if there are to amny pipes for the number of commands
  if(pipe_count >= command_count){
    fprintf(stderr, "pipe: syntax error near \'@\'\n\n");
    fprintf(stderr, "Usage: pipe cmd cmd-arg1 ... [ @ cmd cmd-arg1 ... ]\n");
    exit(0);
  }
  
  //if no pipes are given
  if(pipe_count == 0){
    //creates the command
    for(int j = 1; j < argc; j++){
      strcat(buffer, argv[j]);
      strcat(buffer, " ");
    }

    //executes the command
    system(buffer);
  }

  else{
    //allocates space for a 2d string array
    char ***cmds = (char***)malloc(command_count * sizeof(char*));
    for(int i = 0; i < command_count; i++){
      cmds[i] = (char**)malloc(command_count * sizeof(char **));
      for(int j = 0; j < command_count; j++){
        cmds[i][j] = (char*)malloc(command_count * sizeof(char));
      }
    }

    int i = 0;//command in array
    //loops through commands
    for(int k = 1; k < argc; k++){
      int j = 0;//argument in command
      //creates the commands
      while(strcmp((argv[k]),"@") != 0){
        cmds[i][j] = argv[k];
        k++;
        j++;

        if(k == argc){
          break;
        }
      }

      //goes to next command
      cmds[i][j] = NULL;
      i++;
    }

    cmds[i] = NULL;
    //calls pipe function with commands
    pipe_command(cmds);
  }
}