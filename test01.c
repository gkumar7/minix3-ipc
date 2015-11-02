/* ================================================= *
 *     Test 01: Testing send_message function        *
 * ================================================= */

#include <stdlib.h>
#include <stdio.h>
#include <lib.h>
#include <string.h>
#include <mailboxlib.h>


int main(int argc, char* argv[])
{
	if (argc <= 2)
	{
		printf ("Usage: %s MESSAGE RECIEVER_PID [ADDITIONAL_RECIEVER_PID]\n", argv[0]);
		return 0;
	}

	char *message = argv[1];
	int recieversPid[argc - 2];
	int i;

	for (i = 0; i < argc - 2; i++)
	{
		recieversPid[i] = atoi(argv[i + 2]);
	}

	printf("Test 01: Calling send_message\n");


	printf("*Debug: send_message( message %s; strlen %i; %i pids, which are %i ", message, strlen(message),argc-2 ,recieversPid[0]);

	for (i = 1; i < argc - 2; i++)
	{
		printf(", %i ", recieversPid[i]);
	}
	printf(".\n");
	//char *test = "Hello, this is a test!";

	/*int testPid[2];
	testPid[0] = 1000;
	testPid[1] = 1234;*/

	send_message (message, strlen(message), recieversPid, argc - 2 );

	return 0;
}
