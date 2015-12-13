/* ================================================= *
 *     Test: Testing multiple senders and receivers
 *     Sender: sends message every second
 *     Receiver: receives message every 5 seconds
 * ================================================= */

#include <stdlib.h>
#include <stdio.h>
#include <lib.h>
#include <string.h>
#include <mailboxlib.h>
#include <unistd.h>

static int *turn;
int main(int argc, char* argv[])
{

	if (argc <= 1)
	{
		printf ("Usage: %s PID\n", argv[0]);
		return 0;
	}

	char *message;

	/* printf("Enter your message (Max %d chars) : ", MAX_MESSAGE_LEN); */
	/* gets( message ); */

	// Hard-coded message
	message = "Hello World";

	int receiversPid[1] = { atoi(argv[1]) };
	int i=0;
	for (;i<2;i++) {
		if (fork() != 0) {
		  // Parent will send the message every second
		  while (1) {
		    send_message(message, strlen(message), receiversPid, 1);
		    sleep(1);
		  }
		}
		else {
		  // Child will receive the message every 5 seconds
		  while (1) {
		    char destBuffer[MAX_MESSAGE_LEN];
			receive_message(destBuffer, sizeof(destBuffer), receiversPid[0]);
		    sleep(5);
		  }
		}
	}

	return 0;
}
