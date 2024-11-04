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

</pre>
