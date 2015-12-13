 # build an executable named myprog from myprog.c
  all: create_mailbox.c create_users.c delete_message.c receive_message.c remove_users.c send_message.c remove_mailbox.c show_system_status.c add_user_to_mb_senders.c
	clang create_mailbox.c -o create_mailbox
	clang create_users.c -o create_users
	clang delete_message.c -o delete_message
	clang receive_message.c -o receive_message
	clang remove_users.c -o remove_users
	clang send_message.c -o send_message
	clang remove_mailbox.c -o remove_mailbox
        clang show_system_status.c -o show_system_status
	clang add_user_to_mb_senders.c -o add_user_to_mb_senders
  clean:
	  $(RM) myprog
