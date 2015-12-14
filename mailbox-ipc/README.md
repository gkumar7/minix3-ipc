# Mailbox IPC

#### Description
1. A user process is able to send to a mailbox (which is initialized at system startup). The message consists of the content and the recipients (list of pids).

2. Another user process is able to receive its messages (associated with its pid).

3. Messages from common senders are received in the same order. For example, if receiver r1 receives messages m1 from sender s1 and m2 from s2 in that order,  then receiver r2 must also receive the messages in the same order.

#### File Structure
##### Makefiles

* header_Makefile - include mailboxlib.h
* pm_Makefile - include mailbox.c
* libminc_Makefile - include strtok library

##### Header Files

* callnr.h - defines new system calls PM_RETRIEVE and PM_DEPOSIT
* proto.h - specify the prototypes for the system call handlers add_to_mailbox()
          and get_from_mailbox()
* mailboxlib.h - specify the structs needed for the mailbox and messages
               system calls send_message() and receive_message() are defined here

##### Source Files

* table.c - defines the system call handlers for each respective system call
          defined in callnr.h.
* mailbox.c - defines the system call handlers

##### Test files
tests/

#### Getting Started
```sh
# Will move files to appropriate directories in the MINIX 3 hierarchy and re-compile the kernel
./update-files.sh
```