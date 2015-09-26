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
struct alias_node alias_list;

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
  default: break;
  }
}

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
  printf("Debug: %s\n", "alias by name search");
  // Visit first node of the list
  struct alias_node *current_node = &alias_list;
  // Iterate until current node is null = end of list
  while (current_node!=NULL) {
    // If both names are equals copy the alias and return true
    if (!strcmp(original_name,current_node->original_name)) {
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
  printf("Debug: %s\n", "name by alias search");
  // Visit first node of the list
  struct alias_node *current_node = &alias_list;
  // Iterate until current node is null = end of list
  while (current_node!=NULL) {
    // If both names are equals copy the alias and return true
    if (!strcmp(alias_name,current_node->alias_name)) {
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
  pnode->next=&alias_list;
  alias_list.next=NULL;
  alias_list.previous=pnode;
  alias_list = *pnode;

  /* Debug */
  struct alias_node *p = &alias_list;
  int n_node = 0;
  while (p!=NULL) {
    printf("Debug: Alias node %i = %s/%s\n",n_node,p->original_name,p->alias_name);
    p=p->next;
    n_node++;
  }

  /* End Debug */
  
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
  printf("Debug: %s\n", "looking for original file in current directory");
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
   printf("Debug: %s\n", "looking for original file in PATH");
  /* get the first folder */
  folder_name = strtok(PATH, PATH_SEPARATOR);
  d = opendir(folder_name);
  printf("Debug: %s opened\n", folder_name);
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
    d = opendir(folder_name);
    printf("Debug: %s opened\n", folder_name);
    if (d) {
      while ((dir = readdir(d)) != NULL) {
        if (!strcmp(dir->d_name,original_name)) {
          return add_alias_node (original_name, alias_name);
        }
      }
      closedir(d);
    }
  }

  /*File not found at this point = Error*/
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
  int n=0;
  char *c=(char*) malloc(sizeof(char)); 
  char *p=command_str;
  while ((*c)!='\n') {
    fgets(c, 2, stdin);
    if (*c!='\0') {
      if (n==MAX_COMMAND_LEN) return -1;
      memcpy(p,c,sizeof(char));
      n++;
      p++;
    }
  }
  /*
  if (fgets(command_str, sizeof(command_str), stdin) == NULL)
    return -1;
  printf("Debug: fgets received %s\n",command_str);*/
  return 0;
}

int execute_command (int argc, char ** argv,int command_option) {
  if (command_option == ALIAS) {
    if (argc>2 || argc<=1)
      print_error(7);
    else {
      char *alias_arg = *(argv+1);

      /* Identify original name and alias */
      char *original_name,*alias_name;
      original_name = strtok(alias_arg, "=");
      alias_name = strtok(alias_arg, "\0");
      if ((original_name!=NULL)||(alias_name!=NULL)) {
        /* Eliminate "" */
        char *alias_wo_quot = (char*) malloc(MAXLINE*sizeof(char));
        while (alias_name!=NULL) {
          if (*alias_name!='\"') {
            *alias_wo_quot=*alias_name;
            alias_wo_quot++;
          }
          alias_name++;
        }
        /* Test alias regex */
        regex_t regex;
        int reti = regcomp(&regex, "[a-zA-Z]+", 0);
        if (reti) {
          fprintf(stderr, "Internal error: Could not compile regex\n");
          exit(-1);
        }
        if ((reti = regexec(&regex, alias_name, 0, NULL, 0))) {
          print_error(8);
          return -1;
        }
        printf("Debug: original_name: %s alias_name: %s\n", original_name, alias_name);
        return alias_command(original_name,alias_name);
      }
    }
  }
  else if (command_option == IF) {
    /* TODO: Execute IF with received params. Preprocess them if necessary.*/
    return 0;
  }
  else if (command_option == EXEC_PROGRAM) {
    /* TODO: Execute EXEC_PROGRAM with received params. Preprocess them if necessary.*/
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
  char **argv = (char **) malloc(sizeof(char *) * MAX_ARGS);
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
      printf("Debug: %s\n", "Alias command detected");
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
    arg = strtok(command_str, " ");
    *argv = arg;
    printf("Debug: program name=>%s\n",arg);
    // Remaining args
    while(arg != NULL) {
      argv++;
      arg = strtok(NULL, " ");
      if (arg!=NULL) {
       *argv=arg;
        printf("Debug: argv %i=>%s\n",argc,arg);
        argc++;
      }
    }
  }
  printf("Debug: argc %i\n",argc);
  printf("Debug: command option %i\n",command_option);
  return execute_command (argc,argv,command_option);
}

/*int single_execute (char *argv[], int argc) // Executes a program
{
  pid_t pid;
  int status;
  char *new_env[] = { NULL }; // This has to be changed

  
  // Fork
  pid = fork();

  
  if (pid == 0)
  {
    // Child
    //int execve(const char *name, char *const argv[], char *const envp[])
    execve (argv[0], &argv[0] , new_env); // If it returns from the exec then it has been an error
    printf("Error executing command %s\n", argv[0]);
  }
  else if (pid > 0)
  {
    // Parent
    pid = wait(&status);
  
    if (pid == -1) // Error. Possible errors: [ECHILD] [EFAULT] or [EAGAIN]
    {

    }
    else // Child ended with no errors (Normal way?) Maybe we don't have to do anything at all in this branch
    {

    }
  }
  else // Error when calling fork
  { 
    // error
    // Possible errors: [EAGAIN]  or [ENOMEM]   
               
  }

}*/


int main(int argc, const char * argv[]) {

  /* Initialize alias list */

  alias_list.previous = NULL;
  alias_list.next = NULL;

  char *command_str = (char*) malloc(MAX_COMMAND_LEN * sizeof(char));
  if (execute_profile()<0) {
    //If PROFILE file cannot be read
    print_error(1);
    exit(-1);
  }

  /* TODO: cd HOME */


  /* TODO: set alarms according to PROFILE */


  while (1) {

    fprintf(stdout,"%s ",">>");

    if (read_command(command_str)<0)
      break;
    printf("Debug: introduced command %s\n",command_str);
    if (recognize_and_exec(command_str)<0)
      print_error(3);
  }
  return 0;
}