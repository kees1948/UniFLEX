Here are the kernel versions, other special files and UniFLEX utilities


[2020-05-15]
basic_uf_sys.dsk.zip is updated. The image expands to an over 15MByte filesystem. It is basically w0.dsk from Mike's SWPTCemu release, but with other boot kernels, another IPL loader and the /dev/entries corrected.
The shell in /bin understands cd instead of chd :-)
The source tree for kernel build sits in /src/usrc_63.
You can boot 3 different kernels:
* uniflex, an IDE kernel that supports tty00 up to tty08, and drives w0 up to w7
* uniflex2, same a above but additionally handles an IOP and tty09 up to tty16
* uniflex9, this one drops directly into a shell, no /etc/init involved

Booting a kernel without the corresponding hardware may hang the boot as the kernel believes to see interrupts
from non-existent locations and can't reset these so it is stuck in this loop ...... 
(i.e. booting a IOP-aware kernel without IOP present.)

######################################################################################
[USERIO]   2020-05-22
I am researching the concept of 'user-IO'. This is a solution where one can add/remove IO boards to a designated backplane and 
this without desturbing the running kernel. My idea is having a hardware setup as is for the IOP. So a (second) CPU09IOP, but
with completely different ROM and a matching driver in the kernel. The IO side is, for now, completely without interrupts.
The swapping of IO boards is a kind of hot-swap.

From the UniFLEX side a process can open '/dev/userio' for read or read/write. As the IO range is 256 bytes (00-FF) the process
should 'seek' to the desired location and then 'read' or 'write' bytes of data. Of course the speed of response will not be 
very high, but suitable for most control purposes. The other response factor is how busy the kernel is at that moment.

When I have progressed I will publish some numbers here. At this moment the kernel driver begins to function.

######################################################################################
[2021-02-09]

basic_uf_sys2A.dsk has dbgio drivers built-in. These drivers may assist in debugging a CPU09GPP type application. I also
updated the SPM09 monitor ROM code

you can boot the following kernels:
uniflex or uniflex   an IDE/DBG kernel
uniflex2             an IDE/IOP/DBG kernel
uniflex4             an IDE/IOP/FLP/DBG kernel
uniflex9             drops straight in /bin/sh

The drivers are now more resillient against hardware not present for the booted kernel.
Also the lastest source tree is present.

######################################################################################
[2022-03-07]

re-organized the Kernel-builds folder
posted the lates formatfd in the tools folder
posted a new source tree, the floppy driver now does swap. That addition has
no consequences for the ROM in the CPU09GPP/09FLP. A tar file is also added.
UniFLEX does normally not have a 'tar' utility, but there is one present in 
the tools folder on GitHub. 

######################################################################################
2024-03-19

WARNING when building your own kernel !!  I have noticed sometimes that when building
kernels and trying to boot them, they don't. In such cases the boot process appears to
hang after the 'Total user memory' message. In these cases rebuilding with do_make 
does not reveal any errors. The deeper cause maybe a bug in the relasmb/link-edit combo.

In very particular cases during the build process, a 'CONSTANT define' is not  
resolved and is silently replaced by 0000. In normaal cases link-edit should produce an
error for this, sometimes it does not. It needs no explanation that such a value
will confuse the kernel and it is unable to start properly. 

FINDING the culprit can be challenging.....  

Advise: also check your files in /lib, sysdef, syserrors, macdefs and such if they 
are up to date.
  
######################################################################################

[2024-10-29]

I had quite a struggle with GitHub. When I tried to publish the StarterKit, I accidentally
has a large (>100MB) disk image in it. The push failed and dispite a lot of attempts
I could not get it corrected. So I had to re-create my local repository by cloning it
from GitHub. That showed to work and now things are back in order. I may have lost _some_ 
history but I have not noticed such yet.

I have published the Software->Networking->StarterKit. That comprises of software pieces,
ROM images, disk images and a short guide to get it working.

Also in recent days I published clib.l.241028 in Software->tools-C. That version of clib.l
holds all functions for networking. (For Mc Cosh).


######################################################################################

