
As already announced a few times, in recent weeks I was able to 
implement TCP/IP and UDP networking in UniFLEX 6309.

The implementation uses a CPU09GPP+09NET hardware to provide up
to 8 sockets. That may be enough for this environment.

Additionally a socklib.t / socklib.t.r is created which supports
the new socket() calls as weel some addtional functions.

    bzero((char) *address, count);

Clear a block of (user) memory.

    unsigned long inet_addr((char) *p);

Parse an IPV4 address in ascii and return an unisgned long.
Error checking is done on the format.


netserver: is a server that allows to be accessed from another system
via the netblk device. It is a convenient way of transferring data 
quickly between UniFLEX systems.

System A:
netserver -p 30000 -f w0.dsk    __or__      (disk is file)
netserver -p 30000 -f /dev/w01              (disk is device)

System B:
setnbdev -p 30000 -i <IP_of_systemA>        (setnbdev is in development....)

Now on system B you can access the contents of the 'disk'
on System A as was it local via /dev/netblk0

[depreciated] cc netserver.c getopt.r socklib.t.r +o=netserver 

[with clib.l.240920]   cc netserver.c

clib.l.240920 (or later) is present in ../tools-C

Lin4UF.c is a version of 'netserver' that runs on a Linux system.

=========================================================================

[2024-06-30]
Added 2 utilities, setnb0.t and setnb1.t. These are to be used to
setup the netblock devices to connect to the right netserver.

You should create the devices in /dev if not already there.
crw-r-- 1 root     13,  0 Feb 04  2023 net0
brw-r-- 1 root      4,  0 Jun 26 13:38 netblk0
brw-r-- 1 root      4,  1 Jun 22 15:32 netblk1
crw-r-- 1 root     14,  0 Mar 27 17:44 netblkc0
crw-r-- 1 root     14,  1 Jun 22 15:32 netblkc1

Booting from the network:

The basic pieces are nwp.bin which is the core code for the GPP+NET 
hardware and nwpboot.bin is the code for the boot sector. De ROM code in
UniBUG is not much, around 91 bytes!! This is enough to get the specific
512 bootsector code loaded from the GPP and start it.

When you assemble nwpboot.t you should specify the IP address and PORT 
of your netserver in the source code, so it can connect. Also in the nwpboot.t
file is the name of the kernel to load, I choose 'uniflexn' as this can't be
selected from the commandline/ 

By modifying nwpboot.t one can boot specific code into memory and start it,
that is not necessarely an UniFLEX kernel. FLEX or OS9 are also likely
candidates. Or even a dedicated driver executable like for a video board.


Creating NWP ROM:

The binary nwpboot.bin is an overlay in the GPP rom code at $1C00. Be careful
to instruct your programming tool properly:
1)  load the nwp.bin image 
2)  load the nwpboot.bin image but DO NOT ERASE YOUR BUFFER!
So both binaries should go into the buffer. Don't change sequence of loading,
it will fail.

=========================================================================



