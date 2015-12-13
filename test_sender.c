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
	if (argc <= 1)
	{
		printf ("Usage: %s RECIEVER_PID [ADDITIONAL_RECIEVER_PID]\n", argv[0]);
		return 0;
	}

	char message[MAX_MESSAGE_LEN];

	printf("Enter your message (Max %d chars) : ", MAX_MESSAGE_LEN);
	gets( message );

	int recieversPid[argc - 1];
	int i;

	for (i = 0; i < argc - 1; i++)
	{
		recieversPid[i] = atoi(argv[i + 1]);
	}

	printf("+Test: Calling send_message\n");


	//printf("*Debug: send_message( message %s; strlen %i; %i pids, which are %i ", message, strlen(message),argc-1 ,recieversPid[0]);

	for (i = 1; i < argc - 1; i++)
	{
		printf(", %i ", recieversPid[i]);
	}
	printf(".\n");

	if (send_message (message, strlen(message), recieversPid, argc - 1 ) == OK)
	{
		printf("+Test: Message %s was sent successfully\n", message);
	}
	else
	{
		printf("+Test: ERROR. Message %s could not be sent\n", message);
	}
	return 0;
}
