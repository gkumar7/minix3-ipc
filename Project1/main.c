//
//  main.c
//  Project1
//

#include <stdio.h>
#include <stdlib.h> 

#define MAX_ARGS       20
#define MAX_COMMAND_LEN 4096

/*
 *
 * Here we should declare some global variables as alarm option, internal PATH, alias...
 *
 */

void printError (int error_code) {
	switch (error_code) {
		case 1: fprintf(stderr, "Error: %s\n", "Could not execute PROFILE.");
				break;
		case 2: fprintf(stderr, "Error: %s\n", "Could not read keyboard input.");
				break;
		case 3: fprintf(stderr, "Error: %s\n", "Unrecognized command.");
				break;
		default: break;
	}
}

int execute_profile (void) {
	// TODO: execute PROFILE 
	return 0;
}

int read_command (char *command_str) {
	//TODO: get complete command from std input and save it in the pointer
	return 0;
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
		// exit(-1); ???? 
	} 
    
    while (1) {

    	fprintf(stdout,"%s ",">>");

    	if (read_command(command_str)<0) 
    		printError(2);

    	if (recognize_and_exec(command_str)<0) 
    		printError(3);
    }
    return 0;
}
