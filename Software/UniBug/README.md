## UniBug

Here is the code for the UniBug ROMs that is to be used with the CPU09 system. It is in a CR/LF delimiter format.

The ROM provides a few basic functions. It provides the basic serial communication with the console port.

<pre>
The supported commands are:

C, configure. Checks the RAM pages and memory size and the functioning of the system timer

E, examine memory. Enable hex/ascii dump of memory area's.

M, examine and change. Provide a simple single memory location show, next, previous and change function
   Use '.' for next and '^' for previous location. Enter valid hex to change byte content.
   
V, set mapper page D. When followed by a 2 digit hex character that page is mapped in at $D000-$DFFF.

J, jump to execute. Will start a program at the 4 hex value typed after the 'J'. 

D, disk boot. After typing the 'D', the program waits for about 2 seconds, In that time a '0' or '1' may be typed
   as drive select from which it will boot. When the bootsector shows the 'uniflex' message, you have
   about 2 seconds to enter a character '0' up to '9'. This will be appended the 'uniflex' as the name
   of the bootfile.

F, floppy boot, After typing the 'F', and the bootsector shows the 'uniflex' message, you have
   about 2 seconds to enter a character '0' up to '9'. This will be appended the 'uniflex' as the name
   of the bootfile.
   
K, shows the amount of memory available. (8K is used for zero page and stack)

</pre>


The ROM code also provides the context switching between processes and kernel on interrupt.

2020-12-19:

Minor addition. The kernel debugger, when present, is initialized properly.

####################################################################################
2021-08-14:

Added UniBUG_B, this version of UniBUG supports the CPU mode switching between
63X09<->68X09 mode of the CPU. You _NEED_ this version (or later) if you are to use
the uf_b_kern.tar source tree (in usrc_63B) in the Software/Kernel-builds directory.

####################################################################################

2023-04-04:

Posted UniBUG_C, this version supports all features of UniBUG_B with the addition of 
special traps for Illegal Instruction and Dive by Zero. A careful reviewed and optimized
version of the interrupt handlers.

####################################################################################

2024-06-30:

Posted UniBUG_D, this version is UniBUG_C with the 'netboot' code added. The 'N' command
attempts to boot the system over TCP/IP from a 'netserver'. This is a TCP/IP based 
server that provides a diskimage file or partition as a blockdevice.

2024-08-20:

Updated the files a bit, reorganized the memory layout to completely leave the 'F' page
out of the setup. Memory in UniBUG reports 960 kB. This is prework to use the CPU09VID
board under UniFLEX with the full 32 kB videobuffer mapped in.

2025-06-08:

unibug_d4, modified the code so that a failed boot _can_ return to UniBUG

####################################################################################

2025-06-09:

Posted UniBUG_E. this version has all the features of unibug_d4 and in addition the
whole memory detect has been overhauled. UniBUG now can initialize with zero memory
in the MON board and will be able to report that!  Further more, any combination of
memory devices, 512K, 256K or 128K is allowed, they should be 32 pin types.

####################################################################################
