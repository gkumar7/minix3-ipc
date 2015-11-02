/* File mailbox.c */
/* Define system calls to the mailbox */

#include "mailboxlib.h"
#include "glo.h"

static mailbox_t *mailbox;

/* Maybe we don't need this, but leave it for now */
int stringToArray(char* stringListOfPids)
{

	return 0;
}


/* Used to create a new mailbox */
int create_mailbox(){
	mailbox = malloc(sizeof(mailbox_t));
	mailbox->head = malloc(sizeof(message_t));

	// Sentinel
	message_t *head = malloc(sizeof(message_t));
	head->message = "HEAD";
	head->prev = head;
	head->next = head;

	mailbox->head = head;
	mailbox->number_of_messages = 0;

	return OK;
}

int init_msg_pid_list(message_t *m) {
    m->recipients = malloc(sizeof(pid_node_t));
    m->recipients->prev = m->recipients;

    // Use negative -1 for head pid
    m->recipients->pid = -1;
    m->recipients->next = m->recipients;

    return OK;
}

/* Creates mailbox if there is none
 * Add message to mailbox (if mailbox is not full)
 * Returns OK if message was successfully added
 * Returns ERROR if mailbox is full
 */
int add_to_mailbox()
{
	char* message;
	char* stringRecipients;
	int messageLen;

	message = m_in.m1_p1;  //Maybe this one doesn't work because we have to do malloc first. Check later
	stringRecipients = m_in.m1_p2;

	// If message size > MAX_MESSAGE_LEN return error
	messageLen = m_in.m1_i1;

	if (messageLen > MAX_MESSAGE_LEN) {
	  printf("Error: received message size exceds %i chars\n", MAX_MESSAGE_LEN);
	  return ERROR;
	}

	/* TODO: Function to take the pids in the string and insert them in the recipients array */

	//If the mailbox doesn't exist, we create it first
	if (!mailbox)
	{
		create_mailbox();
	}

	if (mailbox->number_of_messages < MAX_MESSAGE_COUNT)
	{
	  message_t *new_message = malloc(sizeof(message_t));
	  new_message->message = message;

	  // Create a head node for the recipients linked list
	  init_msg_pid_list(new_message);

	  const char delim[2] = " ";
	  char *rec_p = strtok(stringRecipients, delim);

	  while (rec_p != NULL) {
            pid_node_t *new_recipient = malloc(sizeof(pid_node_t));

            new_recipient->pid = atoi(rec_p);

            new_recipient->next = new_message->recipients;
            new_recipient->prev = new_message->recipients->prev;

            new_message->recipients->next = new_recipient;
            new_message->recipients->prev = new_recipient;

            rec_p = strtok(NULL, delim);
	  }

	  new_message->next = mailbox->head;
	  new_message->prev = mailbox->head->prev;

	  mailbox->head->prev->next = new_message;
	  mailbox->head->prev = new_message;

	  mailbox->number_of_messages += 1;

	  printf("Current amount of messages in mailbox: %d\n", mailbox->number_of_messages);
	  printf("Recipients of this message %s\n", stringRecipients);
	}
	else
	{
		printf("Error: mailbox is full\n");
		return ERROR;
	}

	return OK;
}

int get_from_mailbox()
{
    char *message = m_in.m1_p1;
    int recipient = m_in.m1_i1;
    int bufferSize = m_in.m1_i2;

    if (bufferSize < MAX_MESSAGE_LEN) {
        printf("Error: insufficient buffer size, should be %i chars\n", MAX_MESSAGE_LEN);
        return(ERROR);
    }

    // Return error if there are no messages in the mailbox
    if (!mailbox || mailbox->number_of_messages == 0) {
        printf("Error: mailbox is empty or has not been created\n");
        return ERROR;
    }
    else {
        int i = 0;
        message_t *message_ptr = mailbox->head->next;

        // Iterate over existing messages
        while (i < mailbox->number_of_messages) {
            pid_node_t *recipient_p = message_ptr->recipients->next;

            // Iterate over messages assigned recipients
            while (recipient_p->pid != -1)
            {
                // If the message was sent to current recipient consume it
                if (recipient_p->pid == recipient) {
                    // Reserve memory for the destination string
                    message = malloc(MAX_MESSAGE_LEN*sizeof(char));
                    // Copy the content of the message
                    memcpy(message, message_ptr->message, MAX_MESSAGE_LEN);

                    // Remove recipient
                    recipient_p->prev->next = recipient_p->next;
                    recipient_p->next->prev = recipient_p->prev;
                    pid_node_t *next_node = recipient_p->next;
                    free(recipient_p);

                    // Test if the message has to be garbage collected
                    if (next_node->next->pid == -1) {
                        message_ptr->prev->next = message_ptr->next;
                        message_ptr->next->prev = message_ptr->prev;
                        free(message_ptr);
                    }
                    return OK;
                }
                //Otherwise increment recipient's pointer
                else {
		  recipient_p = recipient_p->next;
                }
            }
            // Increment message pointer and counter
	    message_ptr = message_ptr->next;
	    i++;
	}

    }
    // In case of not find a message for the recipient return error
    return ERROR;
}
