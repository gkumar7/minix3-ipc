#include <minix/syslib.h>
#include "glo.h"
#include <stdlib.h>
#include <stdio.h>
#include <lib.h>
#include <string.h>

#define MAX_MESSAGE_COUNT 16
#define OK 0
#define ERROR -1
#define MAX_MESSAGE_LEN 1024
#define SECURE 0
#define PUBLIC 1

/* UID LinkedList
 * uid: UID of a user
 * privileges: bitstring of privileges
 * prev: previous recipient process
 * next: following recipient process
 */

typedef struct uid_node {
    struct uid_node *prev;
    int uid;
    int privileges;
    struct uid_node *next;
} uid_node_t;

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
    char *subject;
    struct message_struct *prev;
    struct message_struct *next;
} message_t;

/* Mailbox
 * number_of_messages - current number of messages in the mailbox (limit is 16)
 * head - pointer to head of message linked list
 */

typedef struct {
  int owner;
  int number_of_messages;
  int mailbox_type;
  char *mailbox_name;
  uid_node_t *send_access;
  uid_node_t *receive_access;
  message_t *head;
} mailbox_t;

typedef struct {
    int number_of_mailboxes;
    mailbox_t *head;
} mailbox_collection_t;

int create_mailbox();
int init_msg_pid_list(message_t *m);
