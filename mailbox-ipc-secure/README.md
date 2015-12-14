# Protection for the Mailbox IPC

#### Description
1. Messages are now stored in multiple mailboxes and mailboxes are created by users who have been designated with the "owner" privilege.

2. Owners are assigned by the superuser (by default, the superuser is the root user in MINIX 3). The owner can specifiy the senders and receivers of a mailbox. Also, the owner has the privilege to remove messages in a mailbox and remove the access privileges that he has given to the senders and receivers to the mailbox or specific messages.

3. The superuser may also update privileges which have been assigned to owners and has full access to all mailboxes in the system.

4. The users who can create mailboxes can create two kinds of mailboxes.
 * A “secure mailbox” can be used only by designated users, and each user is given the capability to send to or receive from the mailbox or both.
 * A “public mailbox” can be used by any user, except those who are specifically denied send or receive access.
 * A mailbox can be removed by its owner and the super user and no one else.