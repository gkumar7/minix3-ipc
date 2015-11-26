/* ================================================= *
 *            Test to create mailboxes               *
 * ================================================= */

#include <stdlib.h>
#include <stdio.h>
#include <lib.h>
#include <string.h>
#include <mailboxlib.h>



int main(int argc, char* argv[])
{

	/* Modify call manually */
	int mailboxType = 0;
	char *mailboxName = "mailboxTest";
	char *sendersPids = "0 1000 1001";
	char *receiversPids = "0 1002 1003";

	if (add_mailbox(mailboxType, mailboxName, sendersPids, receiversPids) == ERROR)
	{
		printf("*Error when creating mailbox");
	}

	return 0;
}
