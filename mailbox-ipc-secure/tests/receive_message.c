/* ================================================= *
 *            Test for recieving messages            *
 * ================================================= */

#include <stdlib.h>
#include <stdio.h>
#include <lib.h>
#include <string.h>
#include <mailboxlib.h>


int main(int argc, char* argv[])
{

	/* Modify call manually */
	char *destBuffer;
	size_t bufferSize;
	int recipient;

	if (receive_message(destBuffer, bufferSize) == ERROR)
	{
		printf("*Error when creating mailbox");
	}
	else
	{
		printf("+Message received. The content of the message is %s\n", destBuffer);
	}

	return 0;
}
