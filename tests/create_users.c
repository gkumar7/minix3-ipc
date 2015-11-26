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
	int privileges = 1011;

	if (argc <= 1)
	{
		printf ("Usage: %s USERNAME [ADDITIONAL USERNAMES]\n", argv[0]);
		return 0;
	}


	for (i = 1; i < argc; i++)
	{
		printf("Setting up privileges to user %s\n", argv[i]);
		printf("Default privileges are %i\n", privileges);
		if (add_user(argv[i], privileges) == ERROR)
		{
			printf("*Error when giving privileges to user %s\n", argv[i]);
		}
		else
		{
			printf("+Privileges (%i) have been given to user %s\n", privileges, argv[i]);
		}
	}

	return 0;
}
