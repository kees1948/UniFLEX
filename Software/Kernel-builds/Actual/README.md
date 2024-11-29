
This is a near realtime version of my kernel build 

***  check the contents of usrc_63/mach_m1/sysconfig.h first ***
***  it is Mine and may not suit your requirements           ***


There may (there are!) bugs in the newer networking code, be fore-warned!!!!

But if you want to follow the progress it is a nice place and I wil regularely 
update the contents. It includes also some expermintal excersises.

On Linx:     nc -l  -p 32100 |tar xvf -

On UniFLEX:  tar cvf - usrc_63 |w2sock

It took about 2 and a half minute to transfer 479 files and directories with
a total size of about 3.8 MB, which is a nice speed. We are spoiled by nowadays
blazing speeds that some machines show.

For me it is very rewarding, as the alternatives are much worse.

The current sysconfig.h is setup for: IDE, LOOP, NET, GPPDBG

2024-02-15:

Remodelled the socket calls a bit. Now rcvfrom and sendto (UDP) both work and
can handle 5 parameters in the call.

assembler

    ldd sfdn
    sys sendto,*buffer,count,*sockaddr,sockaddrlen

C
    sendto(sfdn, buffer, count, *server, serveraddrlen);

    ldd sfdn
    sys recvfrom,*buffer,count,*sockaddr,*sockaddrlen

C
    recvfrom(sfdn, buffer, count, *server, *serveraddrlen);


[2024-06-12]

Corrected buffer handling code in the kernel, that made the netblock device finally
work without errors.


[2024-08-02]

Corrected the floppy driver so that it behaves similar to the netblock code, the 
earlier version sometimes produced 00 bytes in data when copying.
Update the GPP ROM also.

current sysconfig.h is set for IDE/LOOP/FLP/GPP/DBG/NET

[2024-08-20]

Released new kernel verion 15.01. This should play well with the video board.

***  Needs UniBUG D type ROM to work properly !!! Don't run kernels 15.01 and
later with older UniBUG ROM's. Eventually a memory table panic may occur. So first
update the UniBUG ROM and after rebuild all your boot images.

UniBUG D ROM is backwards compatible with previous UniFLEX kernels. So it is advised
to update the UniBUG ROM anyway. See UniBUG release notes,

[2024-11-29]

Updated kernel tree to support ptys, pseudo ttys.


