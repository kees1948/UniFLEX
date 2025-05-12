
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

[with recent clib.l]   cc netserver.c

***  recent clib.l is present in ../tools-C/LIB_CLIB ***

Lin4UF.c is a version of 'netserver' that runs on a Linux system.

=========================================================================

[2024-06-30]
[depreciated] Added 2 utilities, setnb0.t and setnb1.t. These are to be used to
setup the netblock devices to connect to the right netserver.

You should create the devices in /dev if not already there.
crw-r-- 1 root     13,  0 Feb 04  2023 net0
brw-r-- 1 root      4,  0 Jun 26 13:38 netblk0
brw-r-- 1 root      4,  1 Jun 22 15:32 netblk1
crw-r-- 1 root     14,  0 Mar 27 17:44 netblkc0
crw-r-- 1 root     14,  1 Jun 22 15:32 netblkc1

[2024-11-17]
setnbx.t had been replaced by setnbdev.c. Compile setnbdev.c against
the recent clib.l. (cc setnbdev.c)

setnbdev -d <netblkX> [-p <portno> -i <ip_address>]

[2024-10-12]
Booting from the network:

Look in the NetBoot folder for further details

===========================================================================

[2024-11-22]
Added  getIPbyname.c, this allows one to select destination address(es)
by name. It is a simple mechanism, no DNS is involved!

On your UniFLEX system create /etc/hosts, the sample file uf_etc_hosts is an
example about its format. No line should be longer than 64 characters as a
size limit is imposed for efficiency. 

The /etc/hosts file consists of (at least) 2 fiekds, the first is the IP address
of the host, the next field(s) should contain the NAME of the host. It may be just
the hostname or the FQDN, followed by the HOSTNAME, or just the HOSTNAME

<pre>
1.1.1.1  bonny.mydomain.us  bonny
2.2.2.2  clide.mydomain.us
3.3.3.3  laptop
</pre>
are all valid name references. As the HOSTNAME is at first derived from the FQDN,
the name following the FQDN is in fact ignored.

the call is:

uint32_t getIPbyname(hostname)
char *hostname;

The return value is the full (IPV4) IP address or -1 (which is equivalent to
INADDR_NONE. The latter is returned when the hostname can not be resolved
via /etc/hosts, or line size errors in that file.

=============================================================================

For UDP sockets one can use sendto() and recvfrom(), these calls contain the 
remote IP and port. As you need to specify the local port as well, you can use
bind() for this. bind(socket, &local_ip, local_ip_len);
here local_ip is a struct sockaddr_in where the address is 0.0.0.0 and the port
is 0 too. The system will use the first available portnumber in this case.

=============================================================================

[241206]
Added ping3.c. This is a much improved version, it needs the clib.l from
241203 or later. ping3 can now handle timeouts.

=============================================================================

[250512]
Added w2n.c and n2r.c. These are basically the same as w2sock.c and sock2r.c,
but differ in how the network credentials are passed. Where w2sock.c en sock2r.c
have the settings _fixed_ in the C code, w2n and n2r use a small file in the
user home directory for this. The file ".mynet" holds the server IP address and
port number. Comment lines _starting_ with '*' are ignored.

* this is a .mynet example
192.168.1.1 12345

=============================================================================



