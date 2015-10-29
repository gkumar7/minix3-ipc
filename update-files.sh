#!/bin/sh

# Copy files to appropriate location in Minix

# Copy process manager's files
pmFiles="mailbox.c proto.h table.c"
for fl in $pmFiles; do
    cp $fl /usr/src/minix/servers/pm
done

# Copy callnr.h to include/

cp callnr.h /usr/src/minix/include/minix
