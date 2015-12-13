# minix3-ipc
Secure IPC between user processes

#### Information regarding Minix3
* [MINIX 3 Github Mirror](https://github.com/minix3/minix)
* [MINIX 3 Official Docs](http://www.minix3.org/)

#### shell - Shell Implementation
A simple shell which incorporates a few extra features other than the classic fork and execution of processes.

#### mailbox-ipc - Mailbox IPC
MINIX currently supports kernel IPCs that allows a sender to send a message to a single receiver and does not allow user processes to send to one another. This mailbox IPC consists of a new set of system calls which allow user processes to communicate with one another.

#### mailbox-ipc-secure - Protection for the Mailbox IPC
Added security paradigms to the mailbox IPC utilizing access control lists and the notion of a superuser who may assign mailbox owners and their respective privileges.