This is the code for the boot sector on the IDE disk. It has to reside on block 0 of the disk (first block).
It's purpose is to load the OS into memory.

I made a few changes in the code with respect to the SWTPC/GIMIX versions from the past. 

1)   I moved the bootsector code to $0800 in kernel space, that is well above the initial tables that the ROM
     have set up. This area is wiped as soon as the kernel code is started. The kernel initialisation code may be
     larger and up to $BCFF. All code that is loaded in kernel page B is inaccessible as soon the kernel is running.
     So, the initalisation code (mach/Init) could start before or at $B000 and may end somewhere around $BCFF
     
2)   I added provisions for a delay in the boot process (~ 1.5 sec) In this time one can enter a single digit from 0 to 9
     or give CR or just wait. The entered figure is appended to the basic boot filename. That allows for to boot
     and test different kernel versions quickly. i.e.  uniflex uniflex3 uniflex7
     
3)   At the end of the bootsector space (upper 16 bytes) there is a simple but efficient partition table.
     The boot process would copy this table over to a kernel table.
     
The partition table is not in this source code. Basically it is an array of four 32 bit values.

$a5000000, $a5ffffff, $a5ffffff, $a5ffffff

Each entry starts with $a5, the next 3 bytes are block offset. The first entry would normally start at $000000, but
we _can_ move even the first partition up, in case we need larger boot code. An $a5ffffff signals a unused partition.
If using multiple partitions we need to check the total filesystem size(s) to set the new partition start!

I have planned a simple partition tool which does just that. 

If a partition table is absent _but_ the values are 00000000,the kernel driver accepts such a partition but 
_only_ for the first partition on a drive. WX1/WX2/WX3 would return error on access in such situation.


2019-11-18: correction in diskboot code. First disable DMA before asking drive status. Some drives start to issue DRQ requests if DMA is not disabled......

