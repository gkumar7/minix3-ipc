/* ================================================= *
 * Define prototypes for message and mailbox structs *
 * ================================================= */

#ifndef _MAILBOXLIB_H_
#define _MAILBOXLIB_H_

#include <stdlib.h>
#include <stdio.h>
#include <lib.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>

#define OK 0
#define ERROR -1


int update_privileges(char *username, int privileges)
{
	// Get UID for user (also check if user exists)
	struct passwd *pwd = getpwnam(username);

	  if (pwd) {
	    message m;
	    m.m1_i1 = pwd->pw_uid;
	    m.m1_i2 = privileges;

	    return(_syscall(PM_PROC_NR, PM_UPDATE_PRIVILEGES, &m));
	  }
}


int remove_user(char *username)
{
	// Get UID for user (also check if user exists)
	struct passwd *pwd = getpwnam(username);

	  if (pwd) {
	    message m;
	    m.m1_i1 = pwd->pw_uid;

	    return(_syscall(PM_PROC_NR, PM_REMOVE_USER, &m));
	  }
}

int add_user(char *username, int privileges){

  // Get UID for user (also check if user exists)
  struct passwd *pwd = getpwnam(username);

  if (pwd) {
    message m;
    m.m1_i1 = pwd->pw_uid;
    m.m1_i2 = privileges;

    return(_syscall(PM_PROC_NR, PM_ADD_USER, &m));
  }

  printf("Error: user does not exist.\n");
  return ERROR;
}

/* Add a new mailbox to the collection
 * mailbox_type - 0 or 1 (secure or public)
 * mailbox_name - name of the mailbox
 * send_access - space delimited string of uids
 * receive_access - space delimited string of uids
 */
int add_mailbox(int mailbox_type, char *mailbox_name, char *send_access, char *receive_access){

  message m;

  m.m1_i1 = getuid();
  m.m1_i2 = strlen(mailbox_name);

  m.m1_p1 = mailbox_name;
  m.m1_p2 = send_access;
  m.m1_p3 = receive_access;

  int send_access_len = strlen(send_access);
  int receive_access_len = strlen(receive_access);

  char *mailbox_type_buf = malloc(snprintf(NULL, 0, "%d", mailbox_type) + 1);
  snprintf(mailbox_type_buf, "%d", mailbox_type);

  // Concatenate lengths of send and receive into a char *
  char *buf1 = malloc(snprintf(NULL, 0, "%d", send_access_len) + 1);
  char *buf2 = malloc(snprintf(NULL, 0, "%d", receive_access_len) + 1);
  sprintf(buf1, "%d", send_access_len);
  sprintf(buf2, "%d", receive_access_len);

  char *send_receive_lens = strcat(mailbox_type_buf, " ");
  send_receive_lens = strcat(send_receive_lens, buf1);
  send_receive_lens = strcat(send_receive_lens, " ");
  send_receive_lens = strcat(send_receive_lens, buf2);

  m.m1_i3 = strlen(send_receive_lens);
  m.m1_p4 = send_receive_lens;

  return(_syscall(PM_PROC_NR, PM_ADD_MAILBOX, &m));
}


int send_message(char *mailbox_name, 
                 char *message_subject,
                 char *message_data,
                 size_t mailboxNameLen,
                 size_t subjectLen,
                 size_t messageLen
                 )
{	

	struct passwd *pwd = getpwnam(username);
	message m;

	m.m1_p1 = message_data;
	m.m1_p2 = message_subject;
	m.m1_p3 = mailbox_name;
	m.m1_i1 = (int) messageLen;
	m.m1_i2 = (int) subjectLen;
	m.m1_i3 = (int) mailboxNameLen;
	m.m1_ull1 = (uint64_t) getuid();

	return(_syscall(PM_PROC_NR, PM_DEPOSIT, &m));
}



int receive_message(char *destBuffer, size_t bufferSize, int recipient)
{

	struct passwd *pwd = getpwnam(username);
		
  message m;
	m.m1_p1 = destBuffer;
	m.m1_i1 = recipient;
	m.m1_i2 = (int) bufferSize;
	m.m1_i3 = getuid();

	int status = _syscall(PM_PROC_NR, PM_RETRIEVE, &m);
	if (status == ERROR)
	{
		printf("ERROR: The process couldn't retrieve any message\n");
	}
	else
	{
		printf("+User: message (%d bytes) \"%s\" received\n", m.m1_i2, destBuffer);
	}

		return status;
}

int delete_message (char *mailbox_name, char *subject) {
    int mailbox_name_len = strlen(mailbox_name);
    int subject_len = strlen(subject);
    message m;
    m.m1_p1 = mailbox_name;
    m.m1_p2 = subject;
    m.m1_i1 = getuid();
    m.m1_i2 = mailbox_name_len;
    m.m1_i3 = subject_len;
    return(_syscall(PM_PROC_NR, PM_DELETE_MESSAGE, &m));

}

int add_sender (char *mailbox_name,char *username) {
  struct passwd *pwd = getpwnam(username);
  int mailbox_name_len = strlen(mailbox_name);
  if (pwd) {
    message m;
    m.m1_i1 = pwd->pw_uid;
    m.m1_i2 = mailbox_name_len;
    m.m1_p1 = mailbox_name;

    return(_syscall(PM_PROC_NR, PM_ADD_SENDER, &m));
  }
  printf("Error: user does not exist.\n");
  return ERROR;
}

int add_receiver (char *mailbox_name,char *username) {
  struct passwd *pwd = getpwnam(username);
  int mailbox_name_len = strlen(mailbox_name);

  if (pwd) {
    message m;
    m.m1_i1 = pwd->pw_uid;
    m.m1_i2 = mailbox_name_len;
    m.m1_p1 = mailbox_name;

    return(_syscall(PM_PROC_NR, PM_ADD_RECEIVER, &m));
  }
  printf("Error: user does not exist.\n");
  return ERROR;
}

int remove_sender (char *mailbox_name,char *username) {
  struct passwd *pwd = getpwnam(username);
  int mailbox_name_len = strlen(mailbox_name);

  if (pwd) {
    message m;
    m.m1_i1 = pwd->pw_uid;
    m.m1_i2 = mailbox_name_len;
    m.m1_p1 = mailbox_name;

    return(_syscall(PM_PROC_NR, PM_REMOVE_SENDER, &m));
  }
  printf("Error: user does not exist.\n");
  return ERROR;
}

int remove_receiver (char *mailbox_name,char *username) {
  struct passwd *pwd = getpwnam(username);
  int mailbox_name_len = strlen(mailbox_name);

  if (pwd) {
    message m;
    m.m1_i1 = pwd->pw_uid;
    m.m1_i2 = mailbox_name_len;
    m.m1_p1 = mailbox_name;

    return(_syscall(PM_PROC_NR, PM_REMOVE_RECEIVER, &m));
  }
  printf("Error: user does not exist.\n");
  return ERROR;
}

#endif
