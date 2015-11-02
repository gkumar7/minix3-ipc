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

	printf( "Enter your message (Max %d chars) : ", MAX_MESSAGE_LEN);
	gets( message );

	//char *message = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur in feugiat quam. Pellentesque eu gravida lacus. Phasellus enim tortor, malesuada ut lectus non, lobortis elementum lorem. Integer porttitor urna vitae elit imperdiet dignissim. Fusce sed elit id dolor cursus elementum. Nunc et congue leo. Donec ornare, nulla ac eleifend tincidunt, nulla tortor bibendum orci, nec cursus dolor velit et purus. Aliquam erat volutpat. Ut sagittis egestas gravida. Morbi mollis vestibulum eleifend. Nullam condimentum elit bibendum egestas fringilla. Suspendisse et felis in nunc interdum molestie. Phasellus hendrerit nisl ac purus malesuada, non tristique mi aliquam. Maecenas consectetur pellentesque nisi eu fermentum. Ut ornare dictum pharetra";

    //Nulla tortor ex, hendrerit vel interdum nec, rutrum a ante. Donec luctus bibendum vulputate. Donec a ex porta massa aliquam pretium eu nec ligula. Quisque gravida, lorem ac rutrum semper, odio enim interdum nunc, quis rutrum mi orci non dolor. Sed at sem gravida, laoreet metus.";
	//char *message = argv[1];
	int recieversPid[argc - 1];
	int i;

	for (i = 0; i < argc - 1; i++)
	{
		recieversPid[i] = atoi(argv[i + 1]);
	}

	printf("Test 01: Calling send_message\n");


	printf("*Debug: send_message( message %s; strlen %i; %i pids, which are %i ", message, strlen(message),argc-1 ,recieversPid[0]);

	for (i = 1; i < argc - 1; i++)
	{
		printf(", %i ", recieversPid[i]);
	}
	printf(".\n");
	//char *test = "Hello, this is a test!";

	/*int testPid[2];
	testPid[0] = 1000;
	testPid[1] = 1234;*/

	send_message (message, strlen(message), recieversPid, argc - 1 );

	return 0;
}
