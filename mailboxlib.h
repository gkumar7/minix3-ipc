/* ================================================= *
 * Define prototypes for message and mailbox structs *
 * ================================================= */

/* Message
 * received - initially 0, incremented each time a recipient receives its message
 * recipients_num - total number of recipients
 * recipients - recipients of this message
 * message - the message value
 */

typedef struct {
  int received;
  int recipients_num;
  int *recipients;
  char *message;
} message_t;

/* Mailbox
 * number_of_messages - current number of messages in the mailbox
 * messages - array of message_t values
 * open_slots - when a message is garbage collected, add its index here
 *              (if empty, use last_message_index for a new message)
 * last_message_index - index of the last message in the mailbox
 */

typedef struct {
  int number_of_messages;
  message_t *messages;
  int *open_slots;
  int last_message_index;
} mailbox_t;
