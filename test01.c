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
	/*if (argc <= 1)
	{
		printf ("Usage: %s RECIEVER_PID [ADDITIONAL_RECIEVER_PID]\n", argv[0]);
		return 0;
	}*/

	printf("Adding user1\n");
    if (add_user("user1", 0000) == ERROR)
    {
    	printf ("Fail\n");
    }

	printf("Adding user2\n");
    add_user("user2", 0000);

	printf("Adding user3\n");
    add_user("user3", 0000);

	printf("Adding user4\n");
    add_user("user4", 0000);

	printf("Adding user5\n");
    add_user("user5", 0000);

    printf("\n**REMOVING USERS**");

    printf("Removing user user1\n");
    remove_user("user1");


    printf("Removing user user2\n");
    remove_user("user2");


    printf("Removing user user3\n");
    remove_user("user3");


    printf("Removing user user4\n");
    remove_user("user4");


    printf("Removing user user5\n");
    remove_user("user5");

	return 0;
}
