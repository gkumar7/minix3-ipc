/* ================================================= *
 *            Test to remove mailboxes               *
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

	if (remove_mailbox(mailboxName) == ERROR)
	{
		printf("*Error when removing mailbox\n");
	}
	else
	{
		printf("+Mailbox %s has been remove successfully\n", mailboxName);
	}

	return 0;
}
