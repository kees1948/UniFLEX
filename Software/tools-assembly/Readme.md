*

Here some tools with the source code

Note: for some Networking files look in ../Networking

===============================================================
dbgio.t

Simple IO program to connect with debug rom part of a CPU09GPP
or CPU09IOP board.

dbgio <number>    where number is
0    CPU09IOP   at F200
1    CPU09IOP   at F300
2    CPU09GPP   at E000    (floppy)
3    CPU09GPP   at E400
4    CPU09GPP   at E800    (network)
5    CPU09GPP   at EC00    

The rom SHOULD have specific debug monitor code for this!

===============================================================

dfm.s

Show a binary file as it had been loaded into actual memory

===============================================================

flp2file.t

Read a FLEX 8" SS/SD disk into a file. Use the floppy interface 
for this.

flp2file2.t

Read a FLEX 8" DS/SD disk into a file. Use the floppy interface
for this.

===============================================================

set_xxxx.t

Change the root/pipe/swap  major/minor in a uniflex boot image.
The file MUST be edited as the default settings point to an
illegal device.

===============================================================

sock2r.t

Read from a network socket and redirect data to a file. The source
has settings that can/should be changed to your particular network
settings. (default: 192.168.1.155 port 32100)

w2sock.t

Read from a file and redirect to a network socket. Same as for
sock2r.t

On Linux/MAC  'nc (netcat) can be used as the server instance.
nc -l -p 32100 -q0 >my_file      will wait until a file is sent
from UniFLEX with 'w2sock my_file'.
cat my_file | nc -l -p 32100 -q0   will attempt to send a file and
wait unyil UniFLEX takes it 'sock2r  my_file.

===============================================================

