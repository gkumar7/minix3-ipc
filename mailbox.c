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
  superuser->uid = 0;
  superuser->privileges = 0b1111;
  superuser->prev = users;
  superuser->next = users;

  users->next = superuser;
  users->prev = superuser;

  return OK;
}

int userExists(int uid) {
  uid_node_t *head = users->next;
  while (head->uid != -1){
    if (head->uid == uid) {
      return 1;
    }

    head = head->next;
  }
  return 0;
}

/* Get the user node from the list (pointer) */

uid_node_t* getUser(int uid)
{
	uid_node_t *head = users->next;

	while (head->uid != -1)
	{
		if (head->uid == uid)
		{
			return head;
		}

		head = head->next;
	}

	return NULL;
}

/* Update privileges of a user */
int do_update_privileges()
{
	int uid, privileges, processUID;

	uid = m_in.m1_i1;
	privileges = m_in.m1_i2;
	processUID = m_in.m1_i3;

	if (processUID != 0)
	{
		printf("Mailbox: You are not superuser. Access denied.\n");
		return ERROR;
	}

	// Updating user privileges
	uid_node_t *user_to_update = getUser(uid);

	if (user_to_update == NULL)
	{
		printf("Mailbox: The user with uid %d does not exist and can not be updated.\n", uid);
		return ERROR;
	}

	user_to_update->privileges = privileges;

	printf("Mailbox: Privileges of user with uid %d have been updated\n", user_to_update->uid);

	return OK;
}



/* Remove a user from the user list
 * Can only be done by the superuser
 */
int do_remove_user()
{
	int uid, processUID;

	uid = m_in.m1_i1;
	processUID = m_in.m1_i3;

	if (processUID != 0)
	{
		printf("Mailbox: You are not superuser. Access denied.\n");
		return ERROR;
	}

	// Removing user from the list
	uid_node_t *user_to_remove = getUser(uid);

	if (user_to_remove == NULL)
	{
		printf("Mailbox: The user with uid %d does not exist and can not be removed.\n", uid);
		return ERROR;
	}

	user_to_remove->prev->next = user_to_remove->next;
	user_to_remove->next->prev = user_to_remove->prev;

	user_to_remove->prev = NULL;
	user_to_remove->next = NULL;

	free (user_to_remove);

	printf("Mailbox: Removed user with uid %d\n", user_to_remove->uid);

	return OK;
}



/* Add a user to the user list
 * Can be called only by the superuser
 */
int do_add_user() {

  int privileges, uid, processUID;

  uid = m_in.m1_i1;
  privileges = m_in.m1_i2;
  processUID = m_in.m1_i3;

  if (processUID != 0)
  {
  	printf("Mailbox: You are not superuser. Access denied.\n");
  	return ERROR;
  }

  if (!users){
    init_users();
  }

  // Check if user already exists
  if (userExists(uid)) {
    printf("Mailbox: The user with uid %d already exists.\n", uid);
    return ERROR;
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

/* Create sentinel mailbox if none exists
 * Check if the mailbox with the given name already exists
 */

int mailboxExists(char *mailbox_name) {
  // Empty mailbox collection
  if (!mailbox_collection) {
    mailbox_collection = malloc(sizeof(mailbox_collection_t));
    mailbox_collection->number_of_mailboxes = 0;

    // Sentinel mailbox
    mailbox_t *sentinel = malloc(sizeof(mailbox_t));
    sentinel->mailbox_name = "HEAD";

    sentinel->prev = sentinel;
    sentinel->next = sentinel;

    mailbox_collection->head = sentinel;
    return 0;
  }

  mailbox_t *head = mailbox_collection->head->next;
  while (strcmp(head->mailbox_name, "HEAD") != 0) {
    if (strcmp(head->mailbox_name, mailbox_name) == 0){
      return 1;
    }

    head = head->next;
  }

  return 0;
}

/* Check to make sure user has correct privileges to create the mailbox */
int create_mailbox_privileges(int uid){
  if (!users){
    init_users();
  }

  uid_node_t *head = users->next;

  while (head->uid != -1){
    if (head->uid == uid){
      if (head->privileges == 0b1111 || head->privileges == 0b1011){
	return 1;
      }
    }

    head = head->next;
  }

  // User does not exist or does not have correct privileges
  return 0;
}

/* Debugging */
int print_access_list(uid_node_t *access_list){
  uid_node_t *head = access_list->next;
  while (head->uid != -1){
    printf("%d", head->uid);

    head = head->next;
  }
  printf("NULL\n");

  return OK;
}

int init_uid_access_list(uid_node_t *access_list) {
  // Sentinel value
  access_list = malloc(sizeof(uid_node_t));
  access_list->uid = -1;

  access_list->prev = access_list;
  access_list->next = access_list;
  return OK;
}

int get_privileges_for_user(int uid) {
  uid_node_t *head = users->next;
  while (head->uid != -1){
    if (head->uid == uid) {
      return head->privileges;
    }

    head = head->next;
  }
  return ERROR;
}

int create_list(char *access_list_str, uid_node_t *access_list) {
  if (!access_list){
    init_uid_access_list(access_list);
  }

  const char delim[2] = " ";
  char *access_p = strtok(access_list_str, delim);
  int uid = atoi(access_p);

  while (access_p != NULL){
    int privileges = get_privileges_for_user(uid);
    if (privileges == ERROR) {
      // Skip this user
      printf("No user found for user id %d\n", uid);
    }
    else {
      // If privileges were found (user exists)
      uid_node_t *new_uid = malloc(sizeof(uid_node_t));
      new_uid->uid = uid;
      new_uid->privileges = privileges;

      new_uid->next = access_list;
      new_uid->prev = access_list->prev;

      access_list->prev->next = new_uid;
      access_list->prev = new_uid;
    }

    access_p = strtok(NULL, delim);
  }
  return OK;
}

/* Create a new mailbox
 * Check uid in users list
 * Check user's privilege
 * send_receive_lens = "mailbox_type send_len receive_len"
 */
int do_add_mailbox(){

  char *mailbox_name, *send_access, *receive_access, *send_receive_lens;

  int uid = m_in.m1_i1;

  if (!create_mailbox_privileges(uid)){
    printf("The user with uid %d does not have the appropriate privileges to create a mailbox.\n", uid);
    return ERROR;
  }

  // Correct privileges
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

  // Copy send_access & receive_access strings
  const char delim[2] = " ";
  int mailbox_type = atoi(strtok(send_receive_lens, delim));
  int send_access_bytes = atoi(strtok(NULL, delim)) * sizeof(char);
  int receive_access_bytes = atoi(strtok(NULL, delim)) * sizeof(char);

  send_access = malloc(send_access_bytes);
  receive_access = malloc(receive_access_bytes);

  sys_datacopy(who_e, (vir_bytes)m_in.m1_p2, SELF, (vir_bytes)send_access, send_access_bytes);
  sys_datacopy(who_e, (vir_bytes)m_in.m1_p3, SELF, (vir_bytes)receive_access, receive_access_bytes);

  printf("The mailbox_type is: %d\n", mailbox_type);
  printf("The value of send_access is: %s\n", send_access);
  printf("The value of receive_access is: %s\n", receive_access);


  // Check if mailbox already exists
  if (mailboxExists(mailbox_name)) {
    printf("Error: mailbox %s already exists.\n", mailbox_name);
    return ERROR;
  }

  // Create a new mailbox
  // Assumes that the uid's that the user provides are valid

  mailbox_t *new_mailbox = malloc(sizeof(mailbox_t));
  new_mailbox->owner = uid;
  new_mailbox->number_of_messages = 0;
  new_mailbox->mailbox_type = mailbox_type;
  new_mailbox->mailbox_name = mailbox_name;

  // Add send and receive access lists
  create_list(send_access, new_mailbox->send_access);
  create_list(receive_access, new_mailbox->receive_access);

  // Sentinel message for mailbox
  message_t *head = malloc(sizeof(message_t));
  head->message = "HEAD";
  head->prev = head;
  head->next = head;

  new_mailbox->head = head;

  new_mailbox->next = mailbox_collection->head;
  new_mailbox->prev = mailbox_collection->head->prev;

  mailbox_collection->head->prev->next = new_mailbox;
  mailbox_collection->head->prev = new_mailbox;

  mailbox_collection->number_of_mailboxes++;
  return OK;
}

/* Removes a mailbox from the mailbox_collection
 * Returns OK if the mailbox was removed
 * Returns ERROR if the mailbox was not found
 */
int do_remove_mailbox(){

  char *mailbox_name;

  int uid = m_in.m1_i1;
  int mailbox_name_len = m_in.m1_i2;

  int mailbox_name_bytes = mailbox_name_len * sizeof(char);
  sys_datacopy(who_e, (vir_bytes)m_in.m1_p1, SELF, (vir_bytes)mailbox_name, mailbox_name_bytes);

  if (!mailbox_collection) {
    return ERROR;
  }

  mailbox_t *head = mailbox_collection->head->next;
  while (strcmp(head->mailbox_name, "HEAD") != 0) {
    if (strcmp(head->mailbox_name, mailbox_name) == 0){
      head->prev->next = head->next;
      head->next->prev = head->prev;

      free(head);
      mailbox_collection->number_of_mailboxes--;
    }

    head = head->next;
  }

  return OK;
}
/* Creates mailbox if there is none
 * Add message to mailbox (if mailbox is not full)
 * Returns OK if message was successfully added
 * Returns ERROR if mailbox is full
 */
int do_add_to_mailbox()
{
	char* message;
	char* subject;
	char* mailboxName;

	int messageLen;
	int subjectLen;
	int mailboxNameLen;

	messageLen = m_in.m1_i1;
	subjectLen = m_in.m1_i2;
	mailboxNameLen = m_in.m1_i3;

  if (messageLen > MAX_MESSAGE_LEN) {
    printf("Error: Length of the message > %i\n",MAX_MESSAGE_LEN);
    return ERROR;
  }

	int messageBytes = messageLen * sizeof(char);
	message = malloc(messageBytes);
	sys_datacopy(who_e, (vir_bytes)m_in.m1_p1, SELF, (vir_bytes)message, messageBytes);

	int subjectBytes = subjectLen * sizeof(char);
	subject = malloc(subjectBytes);
	sys_datacopy(who_e, (vir_bytes)m_in.m1_p2, SELF, (vir_bytes)subject, subjectBytes);

	printf("Mailbox: New message received. Subject with %d bytes: %s,message content with %d bytes: %s\n",subjectBytes,subject,messageBytes, message);

  int mailboxNameBytes = mailboxNameLen * sizeof(char);
  mailboxName = malloc(mailboxNameBytes);
  sys_datacopy(who_e, (vir_bytes)m_in.m1_p3, SELF, (vir_bytes)mailboxName, mailboxNameBytes);

  // search mailbox by name, if it does not exist -> Error
  // store mailbox pointer in mailbox var

  mailbox_t *mailbox = mailbox_collection->head;
  int found = 0;
  do {
    if (!strcmp(mailboxName,mailbox->mailbox_name)) {
      found = 1;
    } else {
      mailbox = mailbox->next;
    }
  } while (!found && strcmp(mailbox_collection->head->mailbox_name,mailbox->mailbox_name));

  if (!found) {
    printf("Error: not found mailbox with given name\n");
    return ERROR;
  }

  // Permission to write?

  int uid = (int) m_in.m1_ull1;
  int in_permission_list=0;

  uid_node_t *uid_p = mailbox->send_access->next;

  while ((uid_p->uid != -1) && !in_permission_list)
  {
    if (uid == uid_p->uid) {
      in_permission_list=1;
    }
    uid_p = uid_p->next;
  }

  int permission = ((uid==0) || ((mailbox->mailbox_type==SECURE)&&in_permission_list)|| ((mailbox->mailbox_type==PUBLIC)&&!in_permission_list)) ? 1 : 0;

  if (!permission) {
    printf("The user is not allowed to write in the specified mailbox\n");
    return ERROR;
  }

	if (mailbox->number_of_messages < MAX_MESSAGE_COUNT)
	{
	  message_t *new_message = malloc(sizeof(message_t));

    new_message->message = message;
    new_message->subject = subject;

	  new_message->next = mailbox->head;
	  new_message->prev = mailbox->head->prev;

	  mailbox->head->prev->next = new_message;
	  mailbox->head->prev = new_message;

	  mailbox->number_of_messages += 1;

	  printf("Mailbox: Current amount of messages in mailbox: %d\n", mailbox->number_of_messages);
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
int do_get_from_mailbox()
{
    char *message;
    int bufferSize = m_in.m1_i1;
    int recipient = m_in.m1_i2;

    printf("Mailbox: get_mail request received from recipient %d. Buffer size: %d\n", recipient, bufferSize);


    if (bufferSize < MAX_MESSAGE_LEN)
    {
        printf("Error: insufficient buffer size, should be %d chars\n", MAX_MESSAGE_LEN);
        return(ERROR);
    }
    //Look for messages in mailboxes
    mailbox_t *mailbox = mailbox_collection->head;
    //int found = 0;

    //int recipient = m_in.m1_i3;

    do {

      // Permission to read?
      int in_permission_list=0;

      uid_node_t *uid_p = mailbox->receive_access->next;

      while ((uid_p->uid != -1) && !in_permission_list)
      {
        if (recipient == uid_p->uid) {
          in_permission_list=1;
        }
        uid_p = uid_p->next;
      }

      int permission = ((recipient==0) || ((mailbox->mailbox_type==SECURE)&&in_permission_list)|| ((mailbox->mailbox_type==PUBLIC)&&!in_permission_list)) ? 1 : 0;

      if(mailbox->number_of_messages > 0 && permission)
      {
          int i = 0;
          message_t *message_ptr = mailbox->head->next;
          // Iterate over existing messages
          while (i < mailbox->number_of_messages)
          {
          	printf("Mailbox: Checking message number %d\n", i);
            pid_node_t *recipient_p = message_ptr->recipients->next;
              // Iterate over messages assigned recipients
              while (recipient_p->pid != -1)
              {
              	printf("Mailbox:Checking pid %d\n", recipient_p->pid);
                  // If the message has not beer read yet bey current recipient, just consume it
                  if (recipient_p->pid != recipient)
                  {
                      //found = 1;
                  	  printf("Mailbox: Pid %d success\n", recipient_p->pid);

            					int messageBytes = strlen(message_ptr->message) * sizeof(char);

                      // Copy the content of the message

                      sys_datacopy(SELF, (vir_bytes)message_ptr->message, who_e, (vir_bytes)m_in.m1_p1, messageBytes);

            					printf("Mailbox: Message obtained is %s\n", m_in.m1_p1);

                      // Add recipient (received message notification)

                      pid_node_t *new_recipient = malloc(sizeof(pid_node_t));
                      new_recipient->pid = recipient;

                      new_recipient->next = message_ptr->recipients;
                      new_recipient->prev = message_ptr->recipients->prev;

                      message_ptr->recipients->prev->next = new_recipient;
                      message_ptr->recipients->prev = new_recipient;

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
      mailbox = mailbox->next;
    } while (strcmp(mailbox_collection->head->mailbox_name,mailbox->mailbox_name));
    //} while (!found && strcmp(mailbox_collection->head->mailbox_name,mailbox->mailbox_name));
    // In case of not find a message for the recipient return error

    return ERROR;
}

int do_delete_message() {
  int pid = m_in.m1_i1;
  int mailboxNameLen = m_in.m1_i2;
  int subjectLen = m_in.m1_i3;

  char *mailboxName;
  char *subject;

  int mailboxNameBytes = mailboxNameLen * sizeof(char);
  mailboxName = malloc(mailboxNameBytes);
  sys_datacopy(who_e, (vir_bytes)m_in.m1_p1, SELF, (vir_bytes)mailboxName, mailboxNameBytes);

  int subjectBytes = subjectLen * sizeof(char);
  subject = malloc(subjectBytes);
  sys_datacopy(who_e, (vir_bytes)m_in.m1_p2, SELF, (vir_bytes)subject, subjectBytes);

  //find mailbox
  mailbox_t *mailbox = mailbox_collection->head;
  int found = 0;
  do {
    if (!strcmp(mailboxName,mailbox->mailbox_name)) {
      found = 1;
    } else {
      mailbox = mailbox->next;
    }
  } while (!found && strcmp(mailbox_collection->head->mailbox_name,mailbox->mailbox_name));

  if (!found) {
    printf("Error: not found mailbox with given name: %s\n",mailboxName);
    return ERROR;
  }

  //check owner
  if ((pid!=0) && (mailbox->owner!=pid))
  {
    printf("Error: Only superuser or owner can remove a mailbox\n");
    return ERROR;
  }

  //Find message by subject and remove

  if (mailbox->number_of_messages == 0)
  {
        printf("Error: mailbox %s is empty\n",mailboxName);
        return ERROR;
  }

  int i = 0;
  message_t *message_ptr = mailbox->head->next;
  while (i < mailbox->number_of_messages)
  {
    if (!strcmp(message_ptr->subject,subject)) {
      message_ptr->prev->next = message_ptr->next;
      message_ptr->next->prev = message_ptr->prev;
      printf("+Mailbox: Message with subject %s has been deleted\n", subject);
      free(message_ptr);
      mailbox->number_of_messages--;
      return OK;
    }
    message_ptr = message_ptr->next;
    i++;
  }
  printf("Error: message with subject %s not found in mailbox %s\n",subject,mailboxName);
  return ERROR;
}

int do_add_sender () {
  char *mailboxName;
  int uid;

  uid = m_in.m1_i1;
  int mailboxNameLen = m_in.m1_i2;

  int mailboxNameBytes = mailboxNameLen * sizeof(char);
  mailboxName = malloc(mailboxNameBytes);
  sys_datacopy(who_e, (vir_bytes)m_in.m1_p3, SELF, (vir_bytes)mailboxName, mailboxNameBytes);

  //find mailbox
  mailbox_t *mailbox = mailbox_collection->head;
  int found = 0;
  do {
    if (!strcmp(mailboxName,mailbox->mailbox_name)) {
      found = 1;
    } else {
      mailbox = mailbox->next;
    }
  } while (!found && strcmp(mailbox_collection->head->mailbox_name,mailbox->mailbox_name));

  if (!found) {
    printf("Error: not found mailbox with given name: %s\n",mailboxName);
    return ERROR;
  }

  // Find the user in senders list
  int in_permission_list=0;

  uid_node_t *uid_p = mailbox->send_access->next;

  while ((uid_p->uid != -1) && !in_permission_list)
  {
    if (uid == uid_p->uid) {
      in_permission_list=1;
    }
    uid_p = uid_p->next;
  }

  // Return error if the users is already in the list
  if (in_permission_list) {
    printf("Error: Users is already in the senders list.\n");
    return ERROR;
  }

  // Add the user to the list

  uid_node_t *new_user = malloc(sizeof(uid_node_t));
  new_user->uid = uid;

  new_user->next = mailbox->send_access;
  new_user->prev = mailbox->send_access->prev;

  mailbox->send_access->prev->next = new_user;
  mailbox->send_access->prev = new_user;

  return OK;
}

int do_add_receiver () {
  char *mailboxName;
  int uid;

  uid = m_in.m1_i1;
  int mailboxNameLen = m_in.m1_i2;

  int mailboxNameBytes = mailboxNameLen * sizeof(char);

  mailboxName = malloc(mailboxNameBytes);
  sys_datacopy(who_e, (vir_bytes)m_in.m1_p3, SELF, (vir_bytes)mailboxName, mailboxNameBytes);

  //find mailbox
  mailbox_t *mailbox = mailbox_collection->head;
  int found = 0;
  do {
    if (!strcmp(mailboxName,mailbox->mailbox_name)) {
      found = 1;
    } else {
      mailbox = mailbox->next;
    }
  } while (!found && strcmp(mailbox_collection->head->mailbox_name,mailbox->mailbox_name));

  if (!found) {
    printf("Error: not found mailbox with given name: %s\n",mailboxName);
    return ERROR;
  }

  // Find the user in receivers list
  int in_permission_list=0;

  uid_node_t *uid_p = mailbox->receive_access->next;

  while ((uid_p->uid != -1) && !in_permission_list)
  {
    if (uid == uid_p->uid) {
      in_permission_list=1;
    }
    uid_p = uid_p->next;
  }

  // Return error if the users is already in the list
  if (in_permission_list) {
    printf("Error: Users is already in the senders list.\n");
    return ERROR;
  }

  // Add the user to the list

  uid_node_t *new_user = malloc(sizeof(uid_node_t));
  new_user->uid = uid;

  new_user->next = mailbox->receive_access;
  new_user->prev = mailbox->receive_access->prev;

  mailbox->receive_access->prev->next = new_user;
  mailbox->receive_access->prev = new_user;

  return OK;
}

int do_remove_sender () {
  char *mailboxName;
  int uid;

  uid = m_in.m1_i1;
  int mailboxNameLen = m_in.m1_i2;

  int mailboxNameBytes = mailboxNameLen * sizeof(char);
  mailboxName = malloc(mailboxNameBytes);
  sys_datacopy(who_e, (vir_bytes)m_in.m1_p3, SELF, (vir_bytes)mailboxName, mailboxNameBytes);

  //find mailbox
  mailbox_t *mailbox = mailbox_collection->head;
  int found = 0;
  do {
    if (!strcmp(mailboxName,mailbox->mailbox_name)) {
      found = 1;
    } else {
      mailbox = mailbox->next;
    }
  } while (!found && strcmp(mailbox_collection->head->mailbox_name,mailbox->mailbox_name));

  if (!found) {
    printf("Error: not found mailbox with given name: %s\n",mailboxName);
    return ERROR;
  }

  // Find the user in senders list

  uid_node_t *uid_p = mailbox->send_access->next;

  while (uid_p->uid != -1)
  {
    if (uid == uid_p->uid) {
      // Remove recipient
      uid_p->prev->next = uid_p->next;
      uid_p->next->prev = uid_p->prev;
      free(uid_p);
      return OK;
    }
    uid_p = uid_p->next;
  }

  printf("Error: user not found in mailbox with given name: %s\n",mailboxName);
  return ERROR;

}

int do_remove_receiver () {
  char *mailboxName;
  int uid;

  uid = m_in.m1_i1;
  int mailboxNameLen = m_in.m1_i2;

  int mailboxNameBytes = mailboxNameLen * sizeof(char);
  mailboxName = malloc(mailboxNameBytes);
  sys_datacopy(who_e, (vir_bytes)m_in.m1_p3, SELF, (vir_bytes)mailboxName, mailboxNameBytes);

  //find mailbox
  mailbox_t *mailbox = mailbox_collection->head;
  int found = 0;
  do {
    if (!strcmp(mailboxName,mailbox->mailbox_name)) {
      found = 1;
    } else {
      mailbox = mailbox->next;
    }
  } while (!found && strcmp(mailbox_collection->head->mailbox_name,mailbox->mailbox_name));

  if (!found) {
    printf("Error: not found mailbox with given name: %s\n",mailboxName);
    return ERROR;
  }

  // Find the user in receivers list

  uid_node_t *uid_p = mailbox->receive_access->next;

  while (uid_p->uid != -1)
  {
    if (uid == uid_p->uid) {
      // Remove recipient
      uid_p->prev->next = uid_p->next;
      uid_p->next->prev = uid_p->prev;
      free(uid_p);
      return OK;
    }
    uid_p = uid_p->next;
  }

  printf("Error: user not found in mailbox with given name: %s\n",mailboxName);
  return ERROR;
}

/* Debugging
 * Used for debugging purposes
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
