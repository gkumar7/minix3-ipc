#include <stdlib.h>
#include <stdio.h>
#include <lib.h>
#include <string.h>
#include <mailboxlib.h>

int main(int argc, char* argv[])
{

  char *mailbox_name = "mailboxTest";
  char *username = "user2";
  remove_receiver(mailbox_name, username);
  return 0;
}
