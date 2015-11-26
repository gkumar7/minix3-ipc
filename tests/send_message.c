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
    size_t mailboxNameLen = strlen(mailbox_name);
    size_t subjectLen = strlen(message_subject);
    size_t messageLen = strlen(message_data);

	if (send_message (mailbox_name, message_subject, message_data, mailboxNameLen, subjectLen, messageLen));
	{
		printf("*Error when creating mailbox");
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
