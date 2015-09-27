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
#include <signal.h>
#include <errno.h>

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

// Alarm states

#define ON 1
#define OFF 0
#define ALARM_TIME 1

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
pid_t pid;
int alarm_state;
sigset_t new_set, old_set;

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
  case 10: fprintf(stderr, "Error: %s\n", "Invalid HOME.");
    break;
  default: break;
  }
}

/* FUNCTION HEADERS */

int single_execute (char *argv[], int argc);
int recognize_and_exec(char *command_str);

/* PROFILE */

int execute_profile (void) {
  FILE *profile_file = fopen(PROFILE, "r");
  char line[MAXLINE];
  char *var_type;
  while (fgets(line, MAXLINE, profile_file) != NULL){
    var_type = strtok(line, "=");

    char *rest = strtok(NULL, "");

    if (strcmp(var_type, "PATH") == 0) {
      if (rest[strlen(rest)-1]=='\n') strtok(rest,"\n");
      memcpy(PATH, rest, strlen(rest));
    }
    else if (strcmp(var_type, "HOME") == 0) {
      if (rest[strlen(rest)-1]=='\n') strtok(rest,"\n");
      memcpy(HOME, rest, strlen(rest));
    }
    else if (strcmp(var_type, "ALARM") == 0) {
      if (rest[strlen(rest)-1]=='\n') strtok(rest,"\n");
      if (strcmp(rest, "ON") == 0) {
      alarm_state=ON;
      }
      else if (strcmp(rest, "OFF") == 0) {
        alarm_state=OFF;
      } else {
        fclose(profile_file);
        return -1;
      }
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
        memcpy(dest_string,current_node->original_name,MAXLINE);
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
  if ((fgets(command_str, MAX_COMMAND_LEN * sizeof(char), stdin)) == NULL) return -1;

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
      strcpy(alias_name,token);
      while( token != NULL )
      {
        token = strtok(NULL, "\"");
        if (token!=NULL && *token!='\n') {
          strcpy(original_name,token);
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
    /* Form: if <command1>; then <command2>; else <command3> fi*/

    /*strtok(argv[0], " "); // taking 'if' out
    char *condition = strtok(NULL, ";");
    strtok(NULL, "t");
    strtok(NULL, " ");
    char *true_command = strtok(NULL, ";");
    strtok(NULL, "e");
    strtok(NULL, " ");
    char *false_command = strtok(NULL, "fi");*/

    char *condition = strtok(*argv, ";") + 3;
    char *true_command = strtok(NULL, ";") + 6;
    char *false_command = strtok(NULL, "fi") + 6;
    //false_command[strlen(false_command) - 3] = '\0';

    if (recognize_and_exec(condition) == 0)
      recognize_and_exec(true_command);
    else
      recognize_and_exec(false_command);

    return 0;
  }
  else if (command_option == EXEC_PROGRAM) {
    /* TODO: Execute EXEC_PROGRAM with received params. Preprocess them if necessary.*/
    return single_execute(argv, argc);
  }
  else if (command_option == ALARM) {

    if (argc>2 || argc<=1) {
      print_error(7);
      return -1;
    }
    else {
      char *alarm_arg = (char*) malloc(MAXLINE*sizeof(char));
      strcpy(alarm_arg,argv[1]);
      if (!strcmp(alarm_arg,"ON")||!strcmp(alarm_arg,"on")) {
        alarm_state=ON;
        return 0;
      }
      else if (!strcmp(alarm_arg,"OFF")||!strcmp(alarm_arg,"off")) {
        alarm_state=OFF;
        return 0;
      }
    }
    print_error(7);
  }
  return -1;
}

int recognize_and_exec (char *command_str) {
  /*
   * TODO: -> determine type of command (program, if-else sentence, alarm set...)
   *     -> exec the command
   */
//str = (char *) malloc(15);
  int argc=1;
  //char **argv = (char **) malloc(sizeof(char *) * MAX_ARGS);
  char * argv[MAX_ARGS];
  int command_option;

  //Case IF command (special case: argument is a command itself)
  if ((strlen(command_str)>=3) && (!strncmp(command_str,"if ",3))) {
    /* TODO: prepare args to call IF */
    argv[0] = (char *) malloc (MAXLINE);
    strncpy(argv[0], command_str, strlen(command_str));
    //snprintf(argv[0], "%s", command_str);
    command_option=IF;
  }
  else
  {
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

    // Remaining args
    while ((arg = strtok(NULL, " ")) != NULL) {
      //strcat(*argv, arg);
  argv[argc] = arg;
        argc++;
    }

  }
  return execute_command (argc,argv,command_option);
}

void kill_child(int sig) {
  //TODO: show confirmation alert.
  printf("test\n");
  //kill(pid,SIGKILL);
}

int single_execute (char *argv[], int argc) // Executes a program
{
  //pid_t pid;
  int status, i;
  //int argc = sizeof(argv) / sizeof(char*); // Getting the number of arguments
  char *exec_args [argc + 1]; // Adding one for inserting NULL at the end
  char name_from_alias[MAXLINE];

  // alias thing
  if (find_name_by_alias (argv[0], name_from_alias)) // If its an alias, change its value for the original one
  {
    exec_args[0] = name_from_alias;
  }
  else // If not, just use that argument for the exec
  {
    exec_args[0] = argv[0];
  }

  for (i = 1; i < argc; i++)
  {
    exec_args[i] = argv[i];
  }
  exec_args[argc] = NULL;

  //printf ("Size before %i, size after %i\n", sizeof(argv) / sizeof(char*), argc);

	signal(SIGINT, SIG_IGN); //ignore SIGINT
  // Fork
  pid = fork();


  if (pid == 0) // Child branch
  {
    execvp (exec_args[0], &exec_args[0]); // If it returns from the exec then it has been an error
    printf("Error executing command %s\n", argv[0]);
    perror(NULL);
    return -1;
  }
  else if (pid != 0) // Parent branch
  {
    //sigprocmask( SIG_UNBLOCK, &old_set, &new_set); //Unblock signal in parent
		signal(SIGINT, SIG_DFL); //Unblock signal in parent
    if (alarm_state == ON) alarm(ALARM_TIME);

    // Ignore any EINTR errors
    do
      {
	pid = wait(&status);
      }
    while (pid == -1 && errno == EINTR);

    if (alarm_state == ON) alarm(0);
		//sigprocmask( SIG_BLOCK, &new_set, &old_set); //Block signal in main process

    if (pid == -1) // Error. Possible errors: [ECHILD] [EFAULT] or [EAGAIN]
    {
      perror("Error when waiting for command execution ");
      return -1;
    }
  }
  else // Error when calling fork
  {
    // Possible errors: [EAGAIN]  or [ENOMEM]
    perror("Error when calling fork ");
    return -1;
  }

  return 0;
}

void signal_handler( int sig ) {
		char *response = (char*) malloc(MAX_COMMAND_LEN * sizeof(char));
    printf( "Kill process? [Y/N]: ");
		read_command(response);
		strtok(response,"/n");
		if (!strcmp(response,"Y")) {
			kill(pid,SIGTERM);
		}
		else {
		  // If the user does not response with a yes, create new alarm
		  alarm(ALARM_TIME);
		}
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

  /* Go to user specified HOME directory */
  if (chdir(HOME)!=0){
    print_error(10);
    exit(-1);
  }

	/* Signals configuration */

	/*struct sigaction sact;

	sigemptyset( &sact.sa_mask ); //initialize set
  sact.sa_flags = 0;
  sact.sa_handler = signal_handler; //set handler
  sigaction( SIGALRM, &sact, NULL ); //associate action to SIGALRM

	sigemptyset( &new_set ); //initialize new set
  sigaddset( &new_set, SIGALRM ); //add SIGALRM to the new set
  sigprocmask( SIG_BLOCK, &new_set, &old_set); //block new set and store old set*/
  signal(SIGALRM, &signal_handler);

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
