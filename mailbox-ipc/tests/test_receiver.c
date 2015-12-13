#include <stdio.h>
#include <lib.h>
#include <string.h>
#include <mailboxlib.h>


int main(int argc, char* argv[])
{


	if (argc <= 1 || argc > 2)
	{
		printf ("Usage: %s PROCESS_PID\n", argv[0]);
		return 0;
	}

	int processPid = atoi(argv[1]);
	char destBuffer[MAX_MESSAGE_LEN];

	printf("+Test: Calling get_message system call. Your PID is %d\n", processPid);

	if (receive_message(destBuffer, sizeof(destBuffer), processPid) == OK)
	{
		printf("+Test: The process has received a new message\n");
		printf("*The message is: %s\n", destBuffer);
	}
	else
	{
		printf("+Test: There are no messages for this process in the mailbox\n");
	}

	return 0;
}
