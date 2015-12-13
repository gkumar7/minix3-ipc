/* ================================================= *
 *              Test to create users                 *
 * ================================================= */

#include <stdlib.h>
#include <stdio.h>
#include <lib.h>
#include <string.h>
#include <mailboxlib.h>


int main(int argc, char* argv[])
{

	int i;
	int privileges = 0b1011;

	if (argc <= 1)
	{
		printf ("Usage: %s USERNAME [ADDITIONAL USERNAMES]\n", argv[0]);
		return 0;
	}


	for (i = 1; i < argc; i++)
	{
		printf("Creating user %s\n", argv[i]);
		printf("Default privileges are %i\n", privileges);
		if (add_user(argv[i], privileges) == ERROR)
		{
			printf("*Error when creating user %s\n", argv[i]);
		}
		else
		{
			printf("+User %s created with privileges %i\n", argv[i], privileges);
		}
	}

	return 0;
}
