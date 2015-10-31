/* ================================================= *
 * Define prototypes for message and mailbox structs *
 * ================================================= */

#ifndef _MAILBOXLIB_H_
#define _MAILBOXLIB_H_

#include <stdlib.h>
#include <stdio.h>

#define MAX_MESSAGE_COUNT 16
#define OK 0
#define ERROR -1

int create_mailbox();

/* Message LinkedList
 * recipients_num - total number of recipients (will be decremented each time
                    this message is retrieved by one of its recipients
 * recipients - recipients of this message
 * message - the message value
 * next - pointer to next message
 */

typedef struct message_struct {
  int recipients_num;
  int *recipients;
  char *message;
  struct message_struct *prev;
  struct message_struct *next;
} message_t;

/* Mailbox
 * number_of_messages - current number of messages in the mailbox (limit is 16)
 * head - pointer to head of message linked list
 * last_msg - pointer to last message of the linked list
 */

typedef struct {
  int number_of_messages;
  message_t *head;
} mailbox_t;


int send_message()
{


	return 0;
}



int recieve_message()
{

	return 0;
}

#endif
