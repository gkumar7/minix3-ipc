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

	int i;

	if (argc <= 1)
	{
		printf ("Usage: %s USERNAME [ADDITIONAL USERNAMES]\n", argv[0]);
		return 0;
	}


	for (i = 1; i < argc; i++)
	{
		printf("Removing user %s\n", argv[i]);
		if (remove_user(argv[i]) == ERROR)
		{
			printf("*Error when removing user %s\n", argv[i]);
		}
		else
		{
			printf("+User %s removed\n", argv[i]);
		}
	}

	return 0;
}
