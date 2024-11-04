As this sourcefile contains macros for relamsb, this must be assembled with relasmb.

I created a small UniFLEX diskimage with makeuffs and mount that under Fuse. Put the
source files on it, unmount and mount it with SWTPCemu. There use the relasmb.
relasmb +ls unibugall.t >unibugall.list, next s1 unibugall.r >unibugall.S1
Stop SWTPCemu, remount the image with Fuse, retrieve the .s1/.S1 file for the
programmer.

A second, more convenient method is:

Use Linux kermit to establish connection to the UniFLEX console port, set port @ 9600 baud.
set port /dev/ttyWHATEVER
set baud 9600
set modem direct

conn 

This connects you with the console port.

For file upload:

kermit si FILEANME 

this will automagically transfer the file from UniFLEX to Linux.

For file download:

kermit ri

give ^\^c  this brings you back to Linux Kermit.
put FILENAME      will download the file

conn

for downloading you need to repeat that for each file




