<pre>
 ### General Networking Info ###

I have overhauled the GPP code for the network, in particular the scheduler.

Now the scheduler activates all the 8 socket handlers in a round robin
fashion, they have all equal priority, which guarantees that all sockets
get there fair share of time and network hogging sockets can't cause
others to be not served. Remember however that the device has a maximum
throughput it can handle, so that is distributed over all active sockets.

GPPNETPROM is a ready to apply image for your CPU09GPP/09NET combo.

It does NOT initialize the network device from the ROM.

ifconfig net0 will return:

inet      255.255.255.255  netmask 255.255.255.255    down
gateway   255.255.255.255  ether   FF:FF:FF:FF:FF:FF

Setting the ROM code for auto initializing of the network device

ROMADDRESS 
0000  FF FF FF FF FF FF          set  mac address for the W5500
0006  FF FF FF FF                IP address for W5500
000A  FF FF FF FF                NETMASK for IP address
000E  FF FF FF FF                IP address for gateway in this network

EXAMPLE
0000  54 53 43 55 46 01          
0006  C0 A8 01 A1                192.168.1.161
000A  FF FF FF 00                255.255.255.0
000E  C0 A8 01 01                192.168.1.1
 
ifconfig net0 will return:

inet      192.168.1.161  netmask 255.255.255.0    up
gateway   192.168.1.1    ether   54:53:43:55:46:01


From another computer on the same network one could use 'ping'
to check if the interface is working.

ping -c3 192.168.1.161
PING 192.168.1.161 (192.168.1.161) 56(84) bytes of data.
64 bytes from 192.168.1.161: icmp_seq=1 ttl=128 time=0.075 ms
64 bytes from 192.168.1.161: icmp_seq=2 ttl=128 time=0.096 ms
64 bytes from 192.168.1.161: icmp_seq=3 ttl=128 time=0.080 ms

When it is failing, 'ping' will not get replies and will timeout.


  *** Booting from the Network ***

To be able to boot from the network interface requires an UnBUG_D type ROM
or later. This UniBUG ROM has the 'N' command added.

Clearly you need also this version of GPPNETPROM (or later).

The N command instructs the GPP to present the bootsector code for the
netboot in it's fifo. The UniBUG ROM code copies that image over into RAM
and executes it. It will download the image 'uniflexn' from the netserver.
I choose 'uniflexn' as this can't be selected from the UniBUG boot commandline.
When completely loaded it will start that kernel.

Prerequistites:

* first, set the prom image such that the GPP will setup the network device
initially.

* second, you MUST set (also in the GPPNETPROM image) the IP details of your
'netserver'. The 'netserver' is the computer where to get the 'uniflexn'
image from and which you provide with the boot filesystem.

ROMADDRESS

1C03 C0 A8 01 F0                   192.168.1.240  IP of netserver is listening
1C07 7F 00                         32512          Port where netserver is listening

* third, your 'uniflexn' image MUST have the root/pipe/swap device set to 0400 
(1024 decimal), this can be checked with the 'tune' utility. ( +r  =read-only).
This instructs the booted kernel to use the netblock driver. (netblk0)

++ tune /uniflexn +r
UniFLEX Configuration Program 10/10/85
 -- UniFLEX file "/uniflexn" - Version 4.14

Maximum Number of Tasks in System = (48) 
Maximum Number of System Buffers = (64) 
Maximum Number of I/O Character Lists = (64) 
Maximum Number of Open Files in System = (64) 
Maximum Number of Active Shared TEXT Segments = (20) 
Maximum Number of Mounted Devices = (5) 
Maximum Number of Locked File Records = (32) 
Root Device Number = (1024) 
Pipe Device Number = (1024) 
Swap Device Number = (1024) 
Floppy Disk Seek Rate = (0) 
Maximum Number of Tasks per User = (20) 
Time Zone (minutes East of UTC) = (300) 
Daylight Savings Time observed locally = (1) 


* fourth, at least as important, it should have been build with the 'netblock' 
driver in it.
 
Once the UniFLEX is running, you can initialize netblk1 as well and use that
to access and mount another partition. i.e. for your home directory.



</pre>
 
