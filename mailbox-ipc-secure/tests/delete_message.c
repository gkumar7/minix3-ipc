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
	char *mailboxName = "mailboxTest";
	char *messageName = "hello_world";

	if (delete_message(mailboxName, messageName) == ERROR)
	{
		printf("*Error when creating mailbox\n");
	}
	else
	{
		printf("+Message %s from mailbox %s has been deleted\n", mailboxName, messageName);
	}

	return 0;
}
