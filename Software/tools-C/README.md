In this directory you may find (handy) pieces of
code for use.

Some are target-ed for networking use.

nettime.c    syncronize your UniFLEX clock with real-time.
the code has been (timezone) adjusted for Amsterdam but can be
adapted easily.

server2ra.c  server like application that is respawned after 
connection closing. When connection is closed from remote
SIGPIPE is sent to the process

server2rb.c  similar to server2ra.c   but here no SIGPIPE on close
and a (exclusive UniFLEX) option is used to handle conversion 
of EOL characters between remote and UniFLEX

place.c is for Scultor. It allows for to pickup fields in the
screen design and reposition them


mkramdsk calls some other tools to setup ramdisks for UniFLEX
with the CPU09RAM board in the system


for the networking applcations, consult the Software/Networking folder
 for includes and sub functions
 
[2024-09-20]

clib.l.240920. This replaces the older clib.l.  This clib has a working
scanf, getopt and socket library included. Building C programs no 
longer require you to refer to getopt.r and libsock.t.r in the 
cc command line.

posted tar.c. This version compiles with McCosh C without issues and 
should replace the current, older, tar version. You should build it
against the new clib.l described above.

[2025-05-11]

login.c, this versiom has a lot of extra's _and_ it displays the date
properly. It sleeps after a few failed attempts, it honors 'nologin'.


