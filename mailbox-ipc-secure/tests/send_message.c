/* ================================================= *
 *            Test to send messages                  *
 * ================================================= */

#include <stdlib.h>
#include <stdio.h>
#include <lib.h>
#include <string.h>
#include <mailboxlib.h>


int main(int argc, char* argv[])
{

	/* Modify call manually */

	char* mailbox_name = "mailboxTest";
	char* message_subject = "messageTestSubject";
	char* message_data = "This is the content of the message";

	if (send_message (mailbox_name, message_subject, message_data) == ERROR)
	{
		printf("*Error when creating mailbox");
	}
	else
	{
		printf("+Message %s sent to mailbox %s\n", message_subject, mailbox_name);
	}

	return 0;
}



/*int send_message(char *mailbox_name,
                 char *message_subject,
                 char *message_data,
                 size_t mailboxNameLen,
                 size_t subjectLen,
                 size_t messageLen
                 )*/
