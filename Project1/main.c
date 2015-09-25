//
//  main.c
//  Project1
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ARGS       20
#define MAX_COMMAND_LEN 4096
#define MAXLINE 1000
#define PROFILE "PROFILE"

/* Global Variables */
char PATH[MAXLINE], HOME[MAXLINE];

void printError (int error_code) {
  switch (error_code) {
  case 1: fprintf(stderr, "Error: %s\n", "Unrecognized variables in PROFILE.");
    break;
  case 2: fprintf(stderr, "Error: %s\n", "Could not read keyboard input.");
    break;
  case 3: fprintf(stderr, "Error: %s\n", "Unrecognized command.");
    break;
  default: break;
  }
}

int execute_profile (void) {
  FILE *profile_file = fopen(PROFILE, "r");
  char line[MAXLINE];
  char *var_type;
  while (fgets(line, MAXLINE, profile_file) != NULL){
    var_type = strtok(line, "=");

    char *rest = strtok(NULL, "");
    if (strcmp(var_type, "PATH") == 0) {
      memcpy(PATH, rest, strlen(rest));
    }
    else if (strcmp(var_type, "HOME") == 0) {
      memcpy(HOME, rest, strlen(rest));
    }
    else if (strcmp(var_type, "\n") != 0) {
      fclose(profile_file);
      return -1;
    }
  }

  fclose(profile_file);
  return 0;
}

int read_command (char *command_str) {
  //TODO: get complete command from std input and save it in the pointer

  if (fgets(command_str, sizeof(command_str), stdin) == NULL)
    return -1;

}

int execute_command (char ** command_args,int command_option) {
  //TODO: execute the command introduced by the user
  return 0;
}

int recognize_and_exec (char *command_str) {
  /*
   * TODO: -> determine type of command (program, if-else sentence, alarm set...)
   * 		 -> exec the command
   */

  char **command_args = (char **) malloc(sizeof(char *) * MAX_ARGS);
  int command_option;
  execute_command (command_args,command_option);
  return 0;
}

int main(int argc, const char * argv[]) {

  char *command_str = (char*) malloc(MAX_COMMAND_LEN * sizeof(char));

  if (execute_profile()<0) {
    //If PROFILE file cannot be read
    printError(1);
    exit(-1);
  }

  while (1) {

    fprintf(stdout,"%s ",">>");

    if (read_command(command_str)<0)
      break;

    if (recognize_and_exec(command_str)<0)
      printError(3);
  }
  return 0;
}
