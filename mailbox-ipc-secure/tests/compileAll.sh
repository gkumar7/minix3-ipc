echo 'Compile create_mailbox'
rm create_mailbox
clang create_mailbox.c -o create_mailbox

echo 'Compile create_users'
rm create_users
clang create_users.c -o create_users

echo 'Compile delete_message'
rm delete_message
clang delete_message.c -o delete_message

echo 'Compile receive_message'
rm receive_message
clang receive_message.c -o receive_message

echo 'Compile remove_users'
rm remove_users
clang remove_users.c -o remove_users

echo 'Compile send_message'
rm send_message
clang send_message.c -o send_message

echo 'Compile remove_mailbox'
rm remove_mailbox
clang remove_mailbox.c -o remove_mailbox

echo 'Compile add_user_to_mb_receivers'
rm add_user_to_mb_receivers
clang add_user_to_mb_receivers.c -o add_user_to_mb_receivers

echo 'Compile add_user_to_mb_sender'
rm add_user_to_mb_senders
clang add_user_to_mb_senders.c -o add_user_to_mb_senders

echo 'Compile remove_user_from_mb_receivers'
rm remove_user_from_mb_receivers
clang remove_user_from_mb_receivers.c -o remove_user_from_mb_receivers

echo 'Compile remove_user_from_mb_senders'
rm remove_user_from_mb_senders
clang remove_user_from_mb_senders.c -o remove_user_from_mb_senders

echo 'Compile show_system_status'
rm show_system_status
clang show_system_status.c -o show_system_status
