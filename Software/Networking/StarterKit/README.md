 
<pre>
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UniFLEX 6309 Networking starter kit for the CPU09XXX system.
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Hardware setup:


* CPU09BP   backplane 4 or 7 slots
            have wire from A3 pin of CPU09MON slot to C23 of CPU09IDE slot
            
* CPU09MM3  CPU board

* CPU09MON  MONitor board
            make sure that the hardware handshake is disabled for the serial port
            have the UniBUG_D version in the ROM. Have the ROM image addressed
            properly.
            
* CPU09IDE  IDE controller board
            have the board selected at (F)F100-(F)F11F, all select jumpers on
            
* CPU09GPP/09NET complete TCP/IP capable hardware
            have the board selected at (F)E800-(F)EBFF, only C1 jumper on
            make sure that the CPU09GPP is equiped with NETROMGPP.BIN
            Have jumpers J2 and J3, on 09NET, both initialy set for pins 2-3 
            (towards the 40 pin connector)

            
Software setup:            


Dump the contents of MiniNetwork onto a SD-card or an IDE drive, 
unzip it first and remember that the bytes of each word must be 
swapped during that.  i.e.

dd if=MiniNetwork of=/dev/sdc {*} conv=swab bs=10240

{*} the _real_ name may differ on your system, check it.

After the image has been transferred to your boot device, set all up.


Power up, the UniBUG ROM should present the prompt.
hit 'd', the UniFLEX kernel with the networking drivers should come up.

check that the IDE board and drive are present and ready

%E F100-F13F
F100  00 00 00 01 00 01 00 01 00 00 00 00 00 00 00 50   ...............P   <<<===
F110  50 50 50 50 50 50 50 50 07 07 07 07 00 50 00 00   .............P..
F120  7C 7C 7C 7C 7C 7C 7C 7C 7C 7C 7C 7C 7C 7C 7C 7C   ||||||||||||||||
F130  7C 7C 7C 7C 7C 7C 7C 7C 7C 7C 7C 7C 7C 7C 7C 7C   ||||||||||||||||

The '50' at F10F is indicating that everything is OK.

%D 
image: uniflex



UniFLEX Operating System
Copyright (C) 1983 by
Technical Systems Consultants, Inc.

Version 14.01  Released Jan 21, 2020
Configuration: CPU09 System, 6309/IDE/LOOP/DBG/NET/NBLK/ 


Total user memory = 884K

++ 

If the user memory reads 916K you don't have the UniBUG_D ROM in the
MONitor board. With UniBUG_D it will read 884K

Your shell on the MiniNetwork disk does include /etc in its search path.
So it will find utilities in /etc /bin and /usr/bin

++ ifconfig
Usage:  ifconfig <device> [<command> <data>] 
++ ifconfig net0
inet      0.0.0.0  netmask 0.0.0.0    down
gateway   0.0.0.0  ether   00:00:00:00:00:00

The network device has not yet been initialized.

We set it up like:

++ ifconfig net0 inet 192.168.1.123 netmask 255.255.255.0 gateway 192.168.1.150 ether 12:34:56:78:9A:BC up

'inet' specifies the IP address of _THIS_ interface

'netmask' specifies the IP mask that your network is using

'gateway' specifies the IP address of your network GATEWAY, this way you can reach the Internet too.

'ether'  specifies the mac address that _THIS_ interface will advertise on the network. Every network
card has it's mac address preset at the factory. Here you should define it yourself.

Hint: 54:53:43:55:46:21  :-)  (is 'TSCUF!' and the last byte is the individual one)

We check the result, if there is any difference between what you 'set' and what
is 'reported' it may be that you have to play with J2 or J3 on the 09NET board. 

++ /etc/ifconfig net0
inet      192.168.1.123  netmask 255.255.255.0    up
gateway   192.168.1.150  ether   12:34:56:78:9A:BC
++ ping 8.8.8.8 (Google nameserver IP address)
PING 8.8.8.8 40 bytes of data
40 bytes from 8.8.8.8: icmp_seq=1 ttl=128
40 bytes from 8.8.8.8: icmp_seq=2 ttl=128
40 bytes from 8.8.8.8: icmp_seq=3 ttl=128
40 bytes from 8.8.8.8: icmp_seq=4 ttl=128
40 bytes from 8.8.8.8: icmp_seq=5 ttl=128

INTERRUPT!
++ 

 You are now in UniFLEX 6309 Networking business............
 
 
 
With wasock and rasock you can exchange data with another PC that runs 'nc'  (netcat) 
The PC side is the server side and that one should run first always.

nc -l -p 32100 -q0 > TO-FILE  or   cat FROM_FILE |nc -l -p 32100 -q3 on your PC

wasock -i IP-of-server -p 32100 < Uniflexfile-to-send    or

rasock -i IP-of-server -p 32100 > Unifilefile-to-get

Many, many more things are possible.




MiniNetwork.7z   7z compressed disk image (expands to 128MB (!!)

NETROMGPP.BIN.zip compressed 8K ROM image for CPU09GPP/09NET combo

C-Tools  source of various networking tools that compile with the 
Mc-Cosh C compiler.

Assembly-Tools   source od 6309 assembler programs for networking. 
{Note} some may have preset IP and port, change to your own.


Added a first version of 'netstat'. 
netstat -t -u -w -a
-t = TCP setup
-u = UDP setup
-w = IPRAW setup
-a = all of the above

++ netstat -a

Proto  Local Address            Foreign Address          State
tcp    192.168.1.179:32775      192.168.1.155:11000      ESTABLISHED

Proto  Local Address            Foreign Address          State
udp    0.0.0.0:10000            0.0.0.0:*                UDP SOCKET

Proto  Local Address            Foreign Address          State
++






































</pre>
