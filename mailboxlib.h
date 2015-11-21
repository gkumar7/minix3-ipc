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
 * mailbox_name - name of the mailbox
 * send_access - space delimited string of uids
 * receive_access - space delimited string of uids
 */
int add_mailbox(char *mailbox_name, char *send_access, char *receive_access){

  message m;

  m.m1_i1 = getuid();
  m.m1_i2 = strlen(mailbox_name);

  m.m1_p1 = mailbox_name;
  m.m1_p2 = send_access;
  m.m1_p3 = receive_access;

  int send_access_len = strlen(send_access);
  int receive_access_len = strlen(receive_access);

  // Concatenate lengths of send and receive into a char *
  char *buf1 = malloc(snprintf(NULL, 0, "%d", send_access_len) + 1);
  char *buf2 = malloc(snprintf(NULL, 0, "%d", receive_access_len) + 1);
  sprintf(buf1, "%d", send_access_len);
  sprintf(buf2, "%d", receive_access_len);

  char *send_receive_lens = strcat(buf1, ";");
  send_receive_lens = strcat(send_receive_lens, buf2);

  m.m1_i3 = strlen(send_receive_lens);
  m.m1_p4 = send_receive_lens;

  return(_syscall(PM_PROC_NR, PM_ADD_MAILBOX, &m));
}


int send_message(char *mailbox_name, 
                 char *message_subject,
                 char *message_data) 
                 //size_t messageLen, 
                 //int *recipients, 
                 //int recipientsLen)
{
	
	/*int i;
	//char recipientsString[128];  // 6 [5 from pid + 1 from separator] * 16, will be always lower than 128
	char *recipientsString;

	recipientsString = (char *) malloc(6);
	int written = snprintf(recipientsString, 6, "%d ", *recipients);

	int recipientsStringLen = 6;
	for (i = 1; i < recipientsLen; i ++)
	{
		recipientsString = (char *) realloc (recipientsString, 6 * (i+1));
		written = snprintf(recipientsString + (written*i), 6, "%d ", *(recipients+i));
		recipientsStringLen += 6;
	}
	//snprintf(recipientsString, 128, "%d", recipients[recipientsLen-1]);

	printf("User: Mapping message %s to pids %s\n", messageData, recipientsString);
	*/
	/*m.m1_p1 = messageData;
	m.m1_p2 = recipientsString;
	m.m1_i1 = (int) messageLen;
	m.m1_i2 = recipientsStringLen;*/
	
	message m;

	m.m1_p1 = message_data;
	m.m1_p2 = message_subject;
	m.m1_p3 = mailbox_name;

	return(_syscall(PM_PROC_NR, PM_DEPOSIT, &m));
}



int receive_message(char *destBuffer, size_t bufferSize ,int recipient)
{
    message m;
	m.m1_p1 = destBuffer;
	m.m1_i1 = recipient;
	m.m1_i2 = (int) bufferSize;
	int status = _syscall(PM_PROC_NR, PM_RETRIEVE, &m);
	if (status == ERROR)
	{
		printf("ERROR: There is no message for the process\n");
	}
	else
	{
		printf("+User: message (%d bytes) \"%s\" received\n", m.m1_i2, destBuffer);
	}

	return status;
}

#endif
