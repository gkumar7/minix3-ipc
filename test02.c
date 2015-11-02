#include <stdio.h>
#include <lib.h>
#include <string.h>
#include <mailboxlib.h>


int main(int argc, char* argv[])
{


	if (argc <= 1)
	{
		printf ("Usage: %s PROCESS_PID\n", argv[0]);
		return 0;
	}

	int processPid = atoi(argv[1]);
	char destBuffer[MAX_MESSAGE_LEN];

	printf("Test 02: Calling get_message\n");
	recieve_message(destBuffer, sizeof(destBuffer), processPid );

	printf("***The process has received a new message***\n");
	printf("The message is: %s\n", destBuffer);
	return 0;
}
