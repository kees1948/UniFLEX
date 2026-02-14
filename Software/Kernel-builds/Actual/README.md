<pre>
[2026-01-21]

Since I have made some changes and improvements in the code that affect both, kernel
and nwtork/iop, I have started a new actual source tree:  usrc_63

usrc_63_2025 contains the source tree up to the end of 2025, but it will not be updated
any more. (real bugs _will_ be corrected though) and has been moved higher up

**************************************************
This is a near realtime version of my kernel build 

***  check the contents of usrc_63_XXXX/mach_m1/sysconfig.h first ***
***  it is Mine and may not suit your requirements                ***


*** As in this source tree there have been changes in the communication via the DPR, ***
*** you SHOULD renew ROM's in CPU09GPP and CPU09IOP boards!                          ***




There may be bugs in the newer networking code, but for the most it works!

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

[2025-01-07]

Updated clib.l for msleep(). updated kernel sources for msleep(). and documentation
Newer kernels now report 6309/K10/......  to inform with which timer interval they are
expected to work. the KXX setting should match the modification on the CPU09MON board.
Unmodified CPU09MON should work properly with kernels which report 6309/K10/...

[2025-05-10]

Added a simple yet effective mechanism for shared memory (between processes) to th
kernel code. I added the  'ummap' call for this. Created a 'tar' image of the
source tree.

[2025-05-19]

Modified the gtid call that it returns the ppid in X


</pre>
