#!/bin/sh

# Copy files to appropriate location in Minix

# Copy process manager's files
pmFiles="mailbox.h mailbox.c proto.h table.c"
for fl in $pmFiles; do
    cp $fl /usr/src/minix/servers/pm
done

# Copy strtok files
cp /usr/src/lib/libc/string/strtok.c /usr/src/minix/lib/libminc/
cp /usr/src/lib/libc/string/strtok_r.c /usr/src/minix/lib/libminc/

# Copy Makefiles
cp header_Makefile /usr/src/minix/include/Makefile
cp pm_Makefile /usr/src/minix/servers/pm/Makefile
cp libminc_Makefile /usr/src/minix/lib/libminc/Makefile

# Copy callnr.h to include/

cp callnr.h /usr/src/minix/include/minix
cp mailboxlib.h /usr/src/minix/include

# Build
cd /usr/src/releasetools
make hdboot
