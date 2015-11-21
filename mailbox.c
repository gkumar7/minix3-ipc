/* File mailbox.c */
/* Define system calls to the mailbox */

#include "mailbox.h"

static mailbox_collection_t *mailbox_collection;
static uid_node_t *users;

/* Need to remove this */
static mailbox_t *mailbox;

/* Project 3 */

/* Initialize users list */
int init_users(){

  // Sentinel value
  users = malloc(sizeof(uid_node_t));
  users->uid = -1;

  // add superuser to users list
  uid_node_t *superuser = malloc(sizeof(uid_node_t));
  superuser->privileges = 0b1111;
  superuser->prev = users;
  superuser->next = users;

  users->next = superuser;
  users->prev = superuser;

  return OK;
}

/* Add a user to the user list
 * Can be called only by the superuser
 */
int do_add_user() {

  int uid, privileges;

  uid = m_in.m1_i1;
  privileges = m_in.m1_i2;

  if (!users){
    init_users();
  }

  // Add new user to the users list
  uid_node_t *new_user = malloc(sizeof(uid_node_t));
  new_user->uid = uid;
  new_user->privileges = privileges;

  new_user->next = users;
  new_user->prev = users->prev;

  users->prev->next = new_user;
  users->prev = new_user;

  printf("Mailbox: Added user with uid %d\n", new_user->uid);

  return OK;
}

/* Create a new mailbox
 * Check uid in users list
 * Check user's privilege
 */
int do_add_mailbox(){

  char *mailbox_name, *send_access, *receive_access, *send_receive_lens;

  int uid = m_in.m1_i1;
  int mailbox_name_len = m_in.m1_i2;
  int send_receive_lens_len = m_in.m1_i3;

  int mailbox_name_bytes = mailbox_name_len * sizeof(char);

  int send_receive_bytes = send_receive_lens_len * sizeof(char);

  mailbox_name = malloc(mailbox_name_bytes);
  send_receive_lens = malloc(send_receive_bytes);

  sys_datacopy(who_e, (vir_bytes)m_in.m1_p1, SELF, (vir_bytes)mailbox_name, mailbox_name_bytes);

  sys_datacopy(who_e, (vir_bytes)m_in.m1_p4, SELF, (vir_bytes)send_receive_lens, send_receive_bytes);

  printf("Mailbox name is: %s\n", mailbox_name);
  printf("Send_receive number of bytes: %s\n", send_receive_lens);

  return OK;
}

/* From Project 2 */

/* Used for debugging purposes
 * Print all messages which are currently in the mailbox
*/
int print_all_messages()
{

	int i;
	message_t *message_ptr = mailbox->head;

	for (i = 1; i <= mailbox->number_of_messages; i++)
	{
		 message_ptr = message_ptr -> next;

		 pid_node_t *pids = message_ptr-> recipients;
		 char *message = message_ptr -> message;

		 printf("**Message number %d\n", i);
		 printf("**Message content %s\n", message);
		 printf("**Recipients: ");

		 pids = pids -> next;

		 while(pids->pid != -1)
		 {
			 printf(" %d, ", pids->pid);
			 pids = pids -> next;
		 }
		 printf("\n");
	}

	return 0;
}

/* Used to create a new mailbox */
int create_mailbox(){
	mailbox = malloc(sizeof(mailbox_t));
	mailbox->head = malloc(sizeof(message_t));

	// Sentinel value
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

    // Sentinel value
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
	int recipientsStringLen;

	// If message size > MAX_MESSAGE_LEN return error
	messageLen = m_in.m1_i1;
	recipientsStringLen = m_in.m1_i2;


	if (messageLen > MAX_MESSAGE_LEN)
	{
		printf("Error: received message size exceeds %d chars\n", MAX_MESSAGE_LEN);
		return ERROR;
	}

	int messageBytes = messageLen * sizeof(char);
	int recipientsStringBytes = recipientsStringLen * sizeof(char);

	message = malloc(messageBytes);
	stringRecipients = malloc(recipientsStringBytes);

	sys_datacopy(who_e, (vir_bytes)m_in.m1_p1, SELF, (vir_bytes)message, messageBytes);
	sys_datacopy(who_e, (vir_bytes)m_in.m1_p2, SELF, (vir_bytes)stringRecipients, recipientsStringBytes);

	printf("Mailbox: New message received. Message content with %d bytes: %s\n", messageBytes, message);
	printf("Mailbox: *stringRecipients is %s\n", stringRecipients);
	printf("Mailbox: *recipientsStringLen is %d\n", recipientsStringLen);

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

	  printf("*Debug: first pid is %s\n", rec_p);
	  while (rec_p != NULL)
	  {
            pid_node_t *new_recipient = malloc(sizeof(pid_node_t));

            new_recipient->pid = atoi(rec_p);

            new_recipient->next = new_message->recipients;
            new_recipient->prev = new_message->recipients->prev;

            new_message->recipients->prev->next = new_recipient;
            new_message->recipients->prev = new_recipient;

            rec_p = strtok(NULL, delim);
            if (rec_p != NULL)
            {
            	printf("*Debug: next pid will be %s\n", rec_p);
            }

	  }

	  new_message->next = mailbox->head;
	  new_message->prev = mailbox->head->prev;

	  mailbox->head->prev->next = new_message;
	  mailbox->head->prev = new_message;

	  mailbox->number_of_messages += 1;


	  printf("Mailbox: Current amount of messages in mailbox: %d\n", mailbox->number_of_messages);

	  /* print_all_messages(); */
	}
	else
	{
		printf("Error: mailbox is full\n");
		return ERROR;
	}

	return OK;
}

/* Retrieve a process' messages from the mailbox
 * Garbage collect a given message if all designated processes have received the message
 * Deadlock can occur if the mailbox is full, and no process retrieves the messages from the mailbox
 */
int get_from_mailbox()
{
    char *message;
    int recipient = m_in.m1_i1;
    int bufferSize = m_in.m1_i2;

    printf("Mailbox: get_mail request received. Receiver pid: %d. Buffer size: %d\n", recipient, bufferSize);


    if (bufferSize < MAX_MESSAGE_LEN)
    {
        printf("Error: insufficient buffer size, should be %d chars\n", MAX_MESSAGE_LEN);
        return(ERROR);
    }

    // Return error if there are no messages in the mailbox
    if (!mailbox || mailbox->number_of_messages == 0)
    {
        printf("Error: mailbox is empty or has not been created\n");
        return ERROR;
    }
    else
    {
        int i = 0;
        message_t *message_ptr = mailbox->head->next;

        // Iterate over existing messages
        while (i < mailbox->number_of_messages)
        {
        	printf("Mailbox: Checking message number %d\n", i);
            pid_node_t *recipient_p = message_ptr->recipients->next;
        	//pid_node_t *recipient_p = message_ptr->recipients;

            // Iterate over messages assigned recipients
            while (recipient_p->pid != -1)
            {
            	printf("Mailbox:Checking pid %d\n", recipient_p->pid);
                // If the message was sent to current recipient consume it
                if (recipient_p->pid == recipient)
                {
                	printf("Mailbox: Pid %d success\n", recipient_p->pid);

					int messageBytes = strlen(message_ptr->message) * sizeof(char);
					// Copy the content of the message
					sys_datacopy(SELF, (vir_bytes)message_ptr->message, who_e, (vir_bytes)m_in.m1_p1, messageBytes);

					printf("Mailbox: Message obtained is %s\n", m_in.m1_p1);

                    // Remove recipient
                    recipient_p->prev->next = recipient_p->next;
                    recipient_p->next->prev = recipient_p->prev;
                    pid_node_t *next_node = recipient_p->next;
                    pid_node_t *prev_node = recipient_p->prev;
                    free(recipient_p);

                    // Test if the message has to be garbage collected
                    if ((next_node->pid == -1) && (prev_node->pid == -1))
                    {
                        message_ptr->prev->next = message_ptr->next;
                        message_ptr->next->prev = message_ptr->prev;
                        printf("+Mailbox: Message \"%s\" has been garbage collected\n", message_ptr->message);
                        free(message_ptr);
                        mailbox->number_of_messages--;
                    }

                    return OK;
                }
                //Otherwise increment recipient's pointer
                else
                {
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
