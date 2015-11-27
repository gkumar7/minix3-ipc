/* ================================================= *
 *            Test for deleting messages             *
 * ================================================= */

#include <stdlib.h>
#include <stdio.h>
#include <lib.h>
#include <string.h>
#include <mailboxlib.h>


int main(int argc, char* argv[])
{

	/* Modify call manually */
	if (delete_message("mailboxTest", "hello_world") == ERROR)
	{
		printf("*Error when creating mailbox");
	}

	return 0;
}
