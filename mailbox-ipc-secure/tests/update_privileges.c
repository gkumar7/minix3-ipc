/* ================================================= *
 *         Test to update user privileges            *
 * ================================================= */

#include <stdlib.h>
#include <stdio.h>
#include <lib.h>
#include <string.h>
#include <mailboxlib.h>


int main(int argc, char* argv[])
{

	int privileges;

	if (argc <= 1)
	{
		printf ("Usage: %s USERNAME NEW_PRIVILEGES\n", argv[0]);
		return 0;
	}


	printf("Updating privileges to user %s\n", argv[1]);
	printf("New privileges will be %s\n", argv[2]);

	privileges = atoi(argv[2]);

	if(update_privileges(argv[1], privileges) == ERROR)
	{
		printf("*Error when updating privileges to user %s\n", argv[1]);
	}
	else
	{
		printf("+User %s has now privileges %s\n", argv[1], argv[2]);
	}


	return 0;
}
