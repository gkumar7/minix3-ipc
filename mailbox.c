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
    m->recipients->prev = recipients;
    m->recipients->pid = NULL;
    m->recipients->next = recipients;
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
	int* recipients;
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
        new_message->recipients_num = recipients_num;
        //new_message->recipients = recipients;
        new_message->message = message;
        init_msg_pid_list(new_message);
        
        int *rec_p = recipients;
        
        while (rec_p!=NULL) {
            pid_node_t *new_recipient = malloc(sizeof(pid_node_t));
            
            new_recipient->pid=*rec_p;
            
            new_recipient->next = new_message->recipients;
            new_recipient->prev = new_message->recipients->prev;
            
            new_message->recipients->next = new_recipient;
            new_message->recipients->prev = new_recipient;
            
            rec_p++;
        }

		new_message->next = mailbox->head;
		new_message->prev = mailbox->head->prev;

		mailbox->head->prev->next = new_message;
		mailbox->head->prev = new_message;

		mailbox->number_of_messages += 1;
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
    int recipient = m_in.m1_p2;
    int bufferSize = m_in.m1_p3;
    
    if (bufferSize<MAX_MESSAGE_LEN) {
        printf("Error: insuficient buffer size, should be %i chars\n",MAX_MESSAGE_LEN);
        return(ERROR);
    }
    
    // Return error if there are no messages in the mailbox
    if (!mailbox || mailbox->number_of_messages == 0) {
        printf("Error: mailbox is empty or has not been created\n");
        return ERROR;
    }
    else {
        int i = 0;
        message_t *message_ptr = mailbox->head;
        // Iterate over existing messages
        while (i < mailbox->number_of_messages) {
            pid_node_t *recipient_p = message_ptr->recipients;
            // Iterate over messages assigned recipients
            while (recipient_p->next != recipient_p &&
                   recipient_p->prev != recipient_p)
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
                    if (next_node -> next == next &&
                        next_node -> prev == next) {
                        message_ptr->prev->next = message_ptr->next;
                        message_ptr->next->prev = message_ptr->prev;
                        free(message_ptr);
                    }
                    return OK;
                }
                //Otherwise increment recivers pointer
                else {
                    recipients+=sizeof(pid_node_t);
                }
            }
            // Increment message pointer and counter
            message_ptr += sizeof(message_t);
            i++;
        }
    }
    // In case of not find a message for the recipient return error
    return ERROR;
}
