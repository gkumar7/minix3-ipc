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

	if (receive_message(destBuffer, bufferSize, recipient) == ERROR)
	{
		printf("*Error when creating mailbox");
	}

	return 0;
}


//int receive_message(char *destBuffer, size_t bufferSize, int recipient)
