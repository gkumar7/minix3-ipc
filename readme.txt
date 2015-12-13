Project 2

-- Makefiles --

header_Makefile - include mailboxlib.h
pm_Makefile - include mailbox.c
libminc_Makefile - include strtok library

-- Header Files --

callnr.h - defines new system calls PM_RETRIEVE and PM_DEPOSIT

proto.h - specify the prototypes for the system call handlers add_to_mailbox()
          and get_from_mailbox()

mailboxlib.h - specify the structs needed for the mailbox and messages
               system calls send_message() and receive_message() are defined here


-- Source Files --

table.c - defines the system call handlers for each respective system call
          defined in callnr.h.

mailbox.c - defines the system call handlers

-- Test files --
test01.c
test02.c
