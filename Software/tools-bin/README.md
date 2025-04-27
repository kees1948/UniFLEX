<pre>

Here some native UniFLEX programs which are very handy.


************************************************************************************************

'use2'  a slightly modified version of the use screen editor, this one runs also via the IOP.
It needs a correct /etc/termcap file entry  for it to work.



************************************************************************************************

'formatfd' is a UniFLEX diskette formatter. It's capabilities should match that of the 
flpdrvr.t in the kernel tree and that of gppflpdrv.t in the gpp_flp branch

B              Write boot sector only. Do not format disk.
d=<devicename> Format the device file name>.
f=blocs>       Establish blocks> fdn Blocks.
l=file name>   Take bad sector addresses from file name>.
L              Take bad sector addresses from terminal.
m=<model code> Use <model code> for disk parameters.
M              Take disk parameters from terminal.
n              Do not prompt for information on disk volume.
p              Prompt for disk parameters.
q              Use quiet mode.
r=(swap>       Reserve <swap> cylinders for swap space.
V              Verify disk after fomnatting.
D              Destroy, just erase the disk
O              unbiased sectors, no filesystem (msdos exchange)
S              unbiased sectors, UniFLEX filesystem
I              Use Interleaved sector arrangement on track
v              Verify disk
b              No filesystem written

************************************************************************************************

tar   is a UniFLEX executable that handles _uncompressed_ tar archive files.
tar: usage  tar [-]{txruc}[vwfblm][0-7[lmh]] [tapefile] [blocksize] file1 file2..

tar tvf tarfile       shows contents of tarfile

tar xvf tarfile       [<match_pattern>] retrieve files from tarfile

tar cvf tarfile       [<match_pattern>] creates tarfile


************************************************************************************************

'flexmulti' is a program simular to the 'flex' utility on UniFLEX. But this version can read
a multitude of diskette formats in a real floppy drive. 
 'flexmulti.s1' is the S1 hex formatted version of 'flexmulti'
 
flexmulti /dev/fd0
flexmulti /dev/fd1    filename  filename     copies file from FLEX disk to UniFLEX

************************************************************************************************

'losetup' is the (first) version of the tool for the new loop device.

losetup /dev/loopx  <disk_image_file>

will make the <disk_image_file> available as a block device under UniFLEX. All blockdevice
tools (diskrepair, blockcheck, fdncheck, devcheck, mkfs, mount, umount) can be applied on this 
device.


It is also possible to use a FLEX diskimage, but that CAN NOT BE MOUNTED. The tool 'flexloopx'
however can access the files in the image. flexloop0 will show the directory of the FLEX-image
made avialable on /dev/loop0.

flexloop0(flexloop1)  shows FLEX directory of disk accessed via loop device

flexloop0 filename filename     copies FLEX file over to UniFLEX

************************************************************************************************

'rm' is a more user friendly version of the 'kill' program. It can do recursive removel of
files AND directories. 

This 'rm' version need also the 'rmdir' program to function.

rm -rf *   WILL ERASE EVERYTHING from current directory and downwards!

************************************************************************************************


'mount' and 'umount' are essentially Unix variants. They maintain the '/etc/mtab' file to register
the status of mounted devices. It does NOT support umount on directories.

mount /dev/w03 /home
umount /dev/w03


************************************************************************************************

* UniFLEX Hard Disk Format Program
*
* usage: ++ mkfsu +nsdrfqB
* where:
*    mkfs2 - The name of the format program.
*
*    options:
*      +n - Don't prompt the user for the volume information
*      +d - Specify the [block] device to be formatted
*           Given as "+d=XXXX"
*      +r - Specify the number of swap blocks !!
*           Given by "+r=NNNNNN", where NN is a decimal number
*      +f - Specify the number of FDN blocks
*           Given by "+f=NN", where NN is a decimal number
*      +q - Don't issue the verification prompt.
*      +B - Don't actually format the disk - Only rewrite
*           the boot sector.
*      +s - Specify the total number of disk blocks for this
*           filesystem.

It will abort if the assigned disk is mounted (in use)

************************************************************************************************

relasmb2 is a modified relasmb. Here the tables setup has been altered to allow for more macro's.


************************************************************************************************

wasock is a program that allows you to upload data from UniFLEX to a server

wasock -i IP_of_SERVER -p PORT_OF_SERVER <filename>

will upload 'filename' to your server. Note{*} your server MUST have been started already.
Filenames used on server and on UniFLEX _may_ have different names.

source code is in ../Networking
************************************************************************************************

rasock is a program that allows you to download data from a server to UniFLEX

rasock -i IP_of_SERVER -p PORT_OF_SERVER <filename>

will download 'filename' from your server. Note{*} your server MUST have been started already.
Filenames used on server and on UniFLEX _may_ have different names.

source code is in ../Networking
************************************************************************************************

pstat is a very handy system tool. It provides a LOT of information about kernel tables and
process info.

It can show among others, process list, inode tables, file tables, tty tables.

This version has been adapted to various hardware and software environments and also getopt()
has been added. For callings arguments to be shown, a UniFLEX kernel >= 15.20 has to be
used. In this kernels I added a new uargptr. which points to the argument count in the 
calling stack.

pstat.1 is a brief manual pahe

************************************************************************************************

portbaud  allows one to report and/or alter the baudrate- and hardware handshake settings 
for serial ports on:  CPU09SR4, CPU09US4 boards as well as those controlled by the CPU09IOP.

When invoked without additional arguments it reports the commandline buildup and the version.

When invoked with a full device pathname it reports the actual baudrate and hardware handshake 
settings:

portbaud  /dev/tty01
9600   cts=dis

To change a setting on a port, invoke  portbaud like:

portbaud  /dev/tty01 9600
9600   cts=dis

portbaud  /dev/tty01 cts=enb
9600   cts=enb

portbaud  /dev/tty01 1200 cts=dis
1200   cts=dis

************************************************************************************************

ktune   allows one to change specific kernel parameters.  (*** KNOW WHAT YOU'RE DOING! ***)

ktune -f <kernel=image> [<-c>] [<-v>]

When invoked as ktune -v,  it show the program revision.

When invoked as   ktune -f <kernel-image>  it shows all the settings available.

19:49_#:ktune -f /uniflex

Info:  (adjustable settings) 

# of tty's in kernel space      : 12  
# of processes max.             : 48  
# of buffers space              : 64  
# of tty lists max              : 64  
# of active inodes max          : 64  
# of shared text segments       : 20  
# of mounted filesystems        : 5   
# of timeout events             : 24  
# of locked records             : 32  
root device major               : 0   
root device minor               : 0   
pipe device major               : 0   
pipe device minor               : 0   
swap device major               : 0   
swap device minor               : 0   
Task max exec time (in 25.6 sec): 0   
User file max size (kB, 0=off)  : 500 
Time zone w.r.t. UTC in minutes : -60 
Daylight savings flag (0/1)     : 0   
MAX. process/user (0=off)       : 20  
Auto update interval (* 4 sec)  : 0   
User process max mem blocks * 4K: 16  
Generic floppy seek rate        : 0   
MMU: how many hardware maps     : 64  

Info:  (fixed settings) 

Console ACIA address            : 0xf000
Console ACIA initial baudrate   : 0
Mappable IO memory page         : 0xfe
Fixed IO and monitor            : 0xff
NMI routine (0000 is none)      : 0x6b68
SWI2 routine (0000 is none)     : 0x6b78
Hardware flags  (machine type)  : 0x00
MMU: device access  mask (eor)  : 0x00
Non-zero for 50 Hz              : 0

When invoked as  ktune -f /uniflex -c  you are prompted for new values. The limits
are given between braces. (lower/upper). Some functions are disabled when the value
is set to 0.

19:52_#:ktune -f /uniflex -c

Info:  (adjustable settings) 

# of tty's in kernel space      : 12   (  1/12 ) 
# of processes max.             : 48   (  8/64 ) 
# of buffers space              : 64   (  8/64 ) 
# of tty lists max              : 64   ( 16/255) 
# of active inodes max          : 64   ( 16/255) 
# of shared text segments       : 20   (  2/20 ) 
# of mounted filesystems        : 5    (  2/8  ) 
# of timeout events             : 24   ( 16/64 ) 
# of locked records             : 32   ( 16/64 ) 
root device major               : 0    (  0/7  ) 
root device minor               : 0    (  0/31 ) 
pipe device major               : 0    (  0/7  ) 
pipe device minor               : 0    (  0/31 ) 
swap device major               : 0    (  0/7  ) 
swap device minor               : 0    (  0/31 ) 
Task max exec time (in 25.6 sec): 0    (  0/255) 
User file max size (kB, 0=off)  : 500  (  0/500) 
Time zone w.r.t. UTC in minutes : -60  (-1440/1440) 
Daylight savings flag (0/1)     : 0    (  0/1  ) 
MAX. process/user (0=off)       : 20   (  0/20 ) 
Auto update interval (* 4 sec)  : 0    (  0/255) 
User process max mem blocks * 4K: 16   (  1/16 ) 
Generic floppy seek rate        : 0    (  0/3  ) 
MMU: how many hardware maps     : 64   (  0/64 ) 

Info:  (fixed settings) 

Console ACIA address            : 0xf000
Console ACIA initial baudrate   : 0
Mappable IO memory page         : 0xfexxxx
Fixed IO and monitor            : 0xffxxxx
NMI routine (0000 is none)      : 0x6b68
SWI2 routine (0000 is none)     : 0x6b78
Hardware flags  (machine type)  : 0x00
MMU: device access  mask (eor)  : 0x00
Non-zero for 50 Hz              : 0

If nothing has been changed, the program just quits. If there were chane(s), you are
prompted to write these in the image.

When errors are made during input, '>>' denotes too high, '<<' denotes too low, '??' 
denotes invalid and '||' denotes that this value needs to be multiples of a value.
I.e. for buffers the value should be modulo 8.

************************************************************************************************



</pre>
