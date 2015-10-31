/* File mailbox.c */
/* Define system calls to the mailbox */

#include "mailboxlib.h"


static mailbox_t *mailbox;

int create_mailbox(){
  mailbox = malloc(sizeof(mailbox_t));
  mailbox->messages = malloc(sizeof(message_t));
  mailbox->last_msg = malloc(sizeof(message_t));
  mailbox->number_of_messages = 0;
  return OK;
}

/* Creates mailbox if there is none
 * Add message to mailbox (if mailbox is not full)
 * Returns OK if message was successfully added
 * Returns ERROR if mailbox is full
 *
 * TODO: Remove parameters and use a message type
 */
int add_to_mailbox(char *message, int *recipients, int recipients_num){
  if (!mailbox){
    create_mailbox();
  }

  message_t *new_message = malloc(sizeof(message_t));
  new_message->recipients_num = recipients_num;
  new_message->recipients = recipients;
  new_message->message = message;
  new_message->next = NULL;

  if (mailbox->number_of_messages < MAX_MESSAGE_COUNT){
    if (!(mailbox->messages->message)){
      mailbox->messages = new_message;
      mailbox->last_msg = new_message;
    }
    else {
      mailbox->last_msg->next = new_message;
      mailbox->last_msg = new_message;
    }
    mailbox->number_of_messages += 1;
  }
  else {
    printf("Error: mailbox is full\n");
    return ERROR;
  }

  return OK;
}

/* Send system call */
int do_send()
{
	// No arguments. Message should be stored in the m_in message struct

	// Step 1: Retrieve the message from the process global struct m1

	// Step 2: Store the message in the queue

}

int do_receive()
{
	// Get the message from the mailbox

	// Store it in the global struct m

}
