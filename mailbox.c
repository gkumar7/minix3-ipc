/* File mailbox.c */
/* Define system calls to the mailbox */

#define MAILBOX_SIZE 16



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
