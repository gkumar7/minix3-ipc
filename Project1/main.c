//
//  main.c
//  Project1
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <unistd.h>
#include <regex.h>

#define MAX_ARGS       20
#define MAX_COMMAND_LEN 4096
#define MAXLINE 100
#define PROFILE "PROFILE"
#define TRUE 1
#define FALSE 0

// Command options

#define IF 1
#define ALIAS 2
#define EXEC_PROGRAM 3
#define ALARM 4

/*
 * Struct definitions
 */

struct alias_node {
   char *original_name;
   char *alias_name;
   struct alias_node *previous;
   struct alias_node *next;
};

/* Global Variables */
char PATH[MAXLINE], HOME[MAXLINE];
char *PATH_SEPARATOR = ":";
struct alias_node *alias_list;

/* ERRORS */

void print_error (int error_code) {
  switch (error_code) {
  case 1: fprintf(stderr, "Error: %s\n", "Unrecognized variables in PROFILE.");
    break;
  case 2: fprintf(stderr, "Error: %s\n", "Could not read keyboard input.");
    break;
  case 3: fprintf(stderr, "Error: %s\n", "The command could not be executed");
    break;
  case 4: fprintf(stderr, "Error: %s\n", "The specified alias already exists.");
    break;
  case 5: fprintf(stderr, "Error: %s\n", "The specified name already has an associated alias.");
    break;
  case 6: fprintf(stderr, "Error: %s\n", "Original file name not found. Alias cannot be added.");
    break;
  case 7: fprintf(stderr, "Error: %s\n", "Invalid number arguments for alias command");
    break;
  case 8: fprintf(stderr, "Error: %s\n", "Alias name contains forbidden characters. Allowed expression: [aA-bB]+.");
    break;
  case 9: fprintf(stderr, "Error: %s\n", "Specified file not found");
    break;
  default: break;
  }
}

/* FUNCTION HEADERS */

int single_execute (char *argv[]);

/* PROFILE */

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

/* ALIAS */

/*
 * If an alias exists then copy it in destString and return true.
 * Otherwise return false.
 */

int find_alias_by_name (char *original_name, char *dest_string) {
  // Visit first node of the list
  struct alias_node *current_node = alias_list;
  // Iterate until current node is null = end of list
  while (current_node!=NULL) {
    // If both names are equals copy the alias and return true
    if (current_node->original_name!=NULL && !strcmp(original_name,current_node->original_name)) {
      // The function could be used just as a confirmation of existance
      if (dest_string!=NULL) {
        memcpy(dest_string,current_node->alias_name,MAX_COMMAND_LEN);
      }
      return TRUE;
    }
    current_node = current_node->next;
  }
  // If the alias doesn't exist return false
  return FALSE;
}

int find_name_by_alias (char *alias_name, char *dest_string) {
  // Visit first node of the list
  struct alias_node *current_node = alias_list;
  // Iterate until current node is null = end of list
  while (current_node!=NULL) {
    // If both names are equals copy the alias and return true
    if (current_node->alias_name!=NULL && !strcmp(alias_name,current_node->alias_name)) {
      // The function could be used just as a confirmation of existance
      if (dest_string!=NULL) {
        memcpy(dest_string,current_node->original_name,MAX_COMMAND_LEN);
      }
      return TRUE;
    }
    current_node = current_node->next;
  }
  // If the alias doesn't exist return false
  return FALSE;
}

/*
 * Add node to the alias_list (Add first).
 */

int add_alias_node (char *original_name, char *alias_name) {
  /*create node*/
  struct alias_node *pnode = (struct alias_node*)malloc(sizeof(struct alias_node));
  char *o_name = (char*) malloc(MAXLINE*sizeof(char));
  char *a_name = (char*) malloc(MAXLINE*sizeof(char));
  memcpy(o_name,original_name,MAXLINE*sizeof(char));
  memcpy(a_name,alias_name,MAXLINE*sizeof(char));
  pnode->original_name=o_name;
  pnode->alias_name=a_name;
  /*Update list*/
  pnode->previous=NULL;
  pnode->next=alias_list;
  alias_list->next=NULL;
  alias_list->previous=pnode;
  alias_list = pnode;

  return 0;
}

/*
 * If an executable file called as original_name can be found
 * then, alias is added to the list.
 *
 * Otherwise, print and return error (-1).
 *
 */

int add_alias (char *original_name, char *alias_name) {
  DIR *d;
  struct dirent *dir;
  char *folder_name = (char *) malloc(MAXLINE*sizeof(char));

  /* Search in current dir first */
  d = opendir(".");

  if (d) {
    while ((dir = readdir(d)) != NULL) {
      if (!strcmp(dir->d_name,original_name)) {
        return add_alias_node (original_name, alias_name);
      }
    }
    closedir(d);
  }

  /* Search in PATH */
  /* get the first folder */
  folder_name = strtok(PATH, PATH_SEPARATOR);
  d = opendir(folder_name);
  if (d)
  {
    while ((dir = readdir(d)) != NULL)
    {
      if (!strcmp(dir->d_name,original_name)) {
        return add_alias_node (original_name, alias_name);
      }
    }
    closedir(d);
  }
  /* Search in other PATH folders*/
  while(folder_name != NULL) {
    folder_name = strtok(NULL, PATH_SEPARATOR);
    if (folder_name!=NULL) {
      d = opendir(folder_name);
      if (d) {
        while ((dir = readdir(d)) != NULL) {
          if (!strcmp(dir->d_name,original_name)) {
            return add_alias_node (original_name, alias_name);
          }
        }
        closedir(d);
      }
    }
  }

  /*File not found at this point = Error*/
  print_error(9);
  return -1;
}

int alias_command (char *original_name, char *alias_name) {
  if (!find_alias_by_name(original_name,NULL)) {
    if (!find_name_by_alias(alias_name,NULL)) {
      return add_alias(original_name,alias_name);
    }
    print_error(5);
    return -1;
  }
  print_error(4);
  return -1;
}

/* Entry point for all the commands */

int read_command (char *command_str) {
  if ((fgets(command_str, sizeof(command_str), stdin)) == NULL) return -1;

  command_str[strlen(command_str) - 1] = '\0';
  return 0;
}

int execute_command (int argc, char * argv[], int command_option) {
  if (command_option == ALIAS) {
    if (argc>2 || argc<=1)
      print_error(7);
    else {
      char *alias_arg = (char*) malloc(MAXLINE*sizeof(char));
      strcpy(alias_arg,argv[1]);

      /* Identify original name and alias */
      char *original_name = (char *) malloc(MAXLINE*sizeof(char));
      char *alias_name = (char *) malloc(MAXLINE*sizeof(char));
      char *token;
      token = strtok(alias_arg, "=");
      strcpy(original_name,token);
      while( token != NULL )
      {
        token = strtok(NULL, "\"");
        if (token!=NULL && *token!='\n') {
          strcpy(alias_name,token);
        }
      }

      /* Test alias regex */
      regex_t regex;
      int reti = regcomp(&regex, "[a-zA-Z]*", 0);
      if (reti) {
        fprintf(stderr, "Internal error: Could not compile regex\n");
        exit(-1);
      }
      if ((reti = regexec(&regex, alias_name, 0, NULL, 0))>0) {
        print_error(8);
        return -1;
      }
      return alias_command(original_name,alias_name);
    }
  }
  else if (command_option == IF) {
    /* TODO: Execute IF with received params. Preprocess them if necessary.*/
    return 0;
  }
  else if (command_option == EXEC_PROGRAM) {
    /* TODO: Execute EXEC_PROGRAM with received params. Preprocess them if necessary.*/
	single_execute(argv);
    return 0;
  }
  else if (command_option == ALARM) {
    /* TODO: Execute ALARM ON/OFF with received params. Preprocess them if necessary.*/
    return 0;
  }
  return -1;
}

int recognize_and_exec (char *command_str) {
  /*
   * TODO: -> determine type of command (program, if-else sentence, alarm set...)
   *     -> exec the command
   */

  int argc=1;
  //char **argv = (char **) malloc(sizeof(char *) * MAX_ARGS);
  char * argv[MAX_ARGS];
  int command_option;

  //Case IF command (special case: argument is a command itself)
  if ((strlen(command_str)>=3) && (!strncmp(command_str,"if ",3))) {
    /* TODO: prepare args to call IF */
    command_option=IF;
  }
  else {
    //Case ALIAS command
    if ((strlen(command_str)>=6) && (!strncmp(command_str,"alias ",6))) {
      command_option=ALIAS;
    }
    //Case ALARM ON/OFF
    else if ((strlen(command_str)>=6) && (!strncmp(command_str,"alarm ",6))) {
       /* TODO: prepare args to call ALARM ON/OFF */
      command_option=ALARM;
    } else {
       /* TODO: prepare args to call EXEC_PROGRAM */
      command_option=EXEC_PROGRAM;
    }
    /* Identify args */
    char *arg;
    // First arg
    argv[0] = strtok(command_str, " ");
    /* *argv = arg; */

    // Remaining args
    while ((arg = strtok(NULL, " ")) != NULL) {
      strcat(*argv, arg);
      argc++;
    }

  }
  return execute_command (argc,argv,command_option);
}

int single_execute (char *argv[]) // Executes a program
{
	pid_t pid;
	int status, i;
	int argc = sizeof(argv) / sizeof(char*); // Getting the number of arguments
	char *exec_args [argc + 1]; // Adding one for inserting NULL at the end


	for (i = 0; i < argc; i++)
	{
		exec_args[i] = argv[i];
	}
	exec_args[argc] = NULL;

	//printf ("Size before %i, size after %i\n", sizeof(argv) / sizeof(char*), argc);


	// Fork
	pid = fork();


	if (pid == 0) // Child branch
	{
		execvp (exec_args[0], &exec_args[0]); // If it returns from the exec then it has been an error
		printf("Error executing command %s ", argv[0]);
		perror(NULL);
		return -1;
	}
	else if (pid > 0) // Parent branch
	{

		pid = wait(&status);

		if (pid == -1) // Error. Possible errors: [ECHILD] [EFAULT] or [EAGAIN]
		{
			perror("Error when waiting for command execution ");
			return -1;
		}
		else // Child ended with no errors (Normal way?) Maybe we don't have to do anything at all in this branch
		{

		}
	}
	else // Error when calling fork
	{
		// error
		// Possible errors: [EAGAIN]  or [ENOMEM]
		perror("Error when calling fork ");
		return -1;

	}

	return 0;
}


int main(int argc, const char * argv[]) {

  /* Initialize alias list */

  alias_list = (struct alias_node*) malloc(sizeof(struct alias_node));
  alias_list->previous = NULL;
  alias_list->next = NULL;

  char *command_str = (char*) malloc(MAX_COMMAND_LEN * sizeof(char));
  if (execute_profile()<0) {
    //If PROFILE file cannot be read
    print_error(1);
    exit(-1);
  }

  /* TODO: cd HOME */


  /* TODO: set alarms according to PROFILE */


  while (1) {
    memset(command_str, 0, (MAX_COMMAND_LEN * sizeof(char)));
    fprintf(stdout,"%s ",">>");

    if (read_command(command_str)<0)
      break;
    if (recognize_and_exec(command_str)<0)
      print_error(3);
  }
  return 0;
}
