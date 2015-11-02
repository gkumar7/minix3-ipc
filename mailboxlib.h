/* ================================================= *
 * Define prototypes for message and mailbox structs *
 * ================================================= */

#ifndef _MAILBOXLIB_H_
#define _MAILBOXLIB_H_

#include <stdlib.h>
#include <stdio.h>
#include <lib.h>
#include <string.h>

#define MAX_MESSAGE_COUNT 16
#define OK 0
#define ERROR -1
#define MAX_MESSAGE_LEN 1024

/* PID LinkedList
 * pid: PID of a recipient process
 * prev: previous recipient process
 * next: following recipient process
 */

typedef struct pid_node {
    struct pid_node *prev;
    int pid;
    struct pid_node *next;
} pid_node_t;

/* Message LinkedList
 * recipients - recipients of this message
 * message - the message value
 * next - pointer to next message
 * prev - pointer to prev message
 */

typedef struct message_struct {
    pid_node_t *recipients;
    char *message;
    struct message_struct *prev;
    struct message_struct *next;
} message_t;

/* Mailbox
 * number_of_messages - current number of messages in the mailbox (limit is 16)
 * head - pointer to head of message linked list
 */

typedef struct {
  int number_of_messages;
  message_t *head;
} mailbox_t;

int create_mailbox();
int init_msg_pid_list(message_t *m);

int send_message(char *messageData, size_t messageLen, int *recipients, int recipientsLen)
{
	message m;
	int i;
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

	m.m1_p1 = messageData;
	m.m1_p2 = recipientsString;
	m.m1_i1 = (int) messageLen;
	m.m1_i2 = recipientsStringLen;

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
