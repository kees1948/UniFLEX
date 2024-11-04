<pre>
Here are the UniFLEX kernel versions.

LATEST is uf231101.img.zip, 2023-11-01


#######################################################################################

If you put a UniFLEX disk image on a drive or SD-card with 'dd' the commandline
should read:

dd if=<name_of_image> of=<name_of_disk_on_your_os> bs=512 conv=swab

The latter is important to get the image with the right byte sequence.

#######################################################################################
Older information

Your kernel may support the devices:

c 0 x   ttyxx  tty ports 00...08

c 1 0   smem  kernel memory

c 1 1   pmem  virtual memory

c 2 0   null  EOF on read, 'Blackhole' on write

c 2 1   zero  read '0' bytes, no write

c 2 2   random read pseudo random numbers, no write

c 3 x   wcx   character drivers for IDE disks 0...7

c 4 0   hwgetset  helper device for portbaud,  get/set baudrate on serial ports

c 5 x   ttyxx  tty ports 09 and up, via IOP

c 6 x   fdc floppy character drivers

c 7 x   uio user I/O drivers

c 8     gpp debug ports
 
c 9     Econet(r) devices

c 10    loop-control

b 0 x   wx    block device  for IDE disks 0...7

b 1 X   loop  loop device

b 2 x   fd    block device for floppy disks


All files are in UniFLEX text format, use 'ufless' to access them from linux.
The .tar image contains the whole source for the CPU09 UniFLEX kernel, in UniFLEX text format

Mix/UF_IOP_200509.dsk.zip. It has the most recent driver source tree in it as the latest version of portbaud.
bootable kernels" uniflex (IDE basic version), uniflex2 (with IOP drivers in it)


2022-11-02:  usrc_63_20221102 is the most recent kernel tree. By settings options in .../mach_m1/sysconfig.h the 
user can select the desired kernel version. i.e. with or without IOP. 

It is advised to only boot a kernel which your hardware configuration (at boot time) supports,  other the boot may
'hang' because the kernel can't handle the interrupts that it sees on it's IO locations.

Mix/uf20200708.dsk.zip contains a few bootable kernels:
- uniflex               IDE kernel, supports CPU09SR4 boards
- uniflex1              same, backup of former
- uniflex2              IDE kernel with IOP support, IOP supports CPU09SR4 boards
- uniflex9              IDE kernel which drops straight in /bin/sh

Note: edit /ect/ttylist after booting, disable tty09...tty12 if you don't have IOP and reboot after

#######################################################################################
2020-12-30:

Uploaded Older/usrc_63A, this clean tree contains the most recent improvements as: 
* partition table is now located in system tables space. Each partition table is
(re) loaded when a drive/partition is opened the first time
* creating a file always strips the s+ bit from the perms.
* the idedrvr.t now supports 2 CPU09IDE boards (need the latest patches)
* the build system got some improvements, all files touched when the sysconfig.h 
has been changed will be rebuild now.
* the kernel interrupt-handler handles unused hardware somewhat better now

2021-01-26:

The image "basic_uf_sys2A.dsk.zip" holds the lastest kernel sources in /src and has already prepared new
boot images in the root directory. 

'uniflex' and 'uniflex1' are identical and support IDE and extra serial ports via CPU09SR4.
'uniflex2' supports IDE and IOP. So you can connect with serial ports via the CPU09IOP.
'uniflex4' supports IDE/IOP and FLP. Here you can connect a floppy drive via CPU09GPP/09FLP.

I have changed the naming of the hard disks:
W00,W01,W02,W03 are the first master, W10,W11,W12,W13 are the first slave
W20,W21,W22,W23 are the second master, W30,W31,W32,W33 are the second slave

The same goes for the character devices: WC00,WC01,WC02......WC31,WC32,WC33


*******************************************************************************************
2021-08-14:

Created Older/usrc_63B and uploaded uf_b_kern.tar in that directory. It provides a clean source
tree to build your own UniFLEX kernel. in .../mach_m1/sysconfig.h you can set what 
hardware is to be supported with your build. 
This kernel tree supports the 63X09<->68X09 transient of the CPU mode. The kernel runs
_always_ in 63X09 mode, but a user process may alter that setting. Leave the FIRQ setting
on 'long stackframe' when meddling with the LDMD register. Failing to do so may result
in your process crashing and  being terminated.
**** For this kernel you NEED the UniBUG_B version of the UniBUG ROM ****


*******************************************************************************************
2022-03-07:

Created Older/usrc_63C, this contains the recent status of all files. The Older directrory holds
'tar' versions of the previous versions.

ufsrc_20220307.tar is the tar archive, identical to the files in usrc_63C.
 
*******************************************************************************************
2022-06-11:

Added wd0_kees.dsk.zip. This is mostly the wd0 disk from Mike Evenson's memulator.
I replaced the uniflex versions, re-created the /dev directory, added a few programs.
===>>> in /ksrc on this image is the latest kermel source present. <<<===
I build the uniflex versions from this source tree, while running UniFLEX from an SD-card.


*******************************************************************************************
2022-11-02:

Published the present kernel state, source tree  
In this source I added the loop device driver as block major 1. That implies that the floppy
driver (and devices) now sit on block major 2. Check your /dev directory.
Also I corrected a few bugs in the iop_han.t code. Lastly, while testing the loop device driver,
I found a few bugs in the kernel code. Calling dobfio happened sometimes with the wrong device
major, also setting the read-ahead disk addresses ommited the DEVICE the read-ahead was to be
on, added also the device for the 'unxtb' settings.



#######################################################################################
2023-11-02

Published a fresh snapshot of the root file system and the kernel source tree.
Be aware the file system size is 256000 blocks which is 131072000 bytes.
uf231101.img.zip

IDE/LOOP/RDSK/DBG/              uniflex         default kernel
IDE/LOOP/RDSK/DBG/              uniflex1        copy of default
IDE/LOOP/RDSK/FLP/GPP/DBG/      uniflex2        support for floppy
IDE/LOOP/RDSK/FLP/IOP/GPP/DBG/  uniflex3        support for IOP processor
IDE/LOOP/RDSK/IOP/DBG/          uniflex4        support for IOP processor new code
                                uniflex9        special kernel, boots straight into /bin/sh

The latest kernel source tree sits under /sys on the image.


Devices as present in the image:
crw----r-- 1 root root  8,  0 Jan 24  2022 dbgio0       virtual port for debug
crw----r-- 1 root root  8,  1 Jan 24  2022 dbgio1
crw----r-- 1 root root  8,  2 Jan 24  2022 dbgio2
crw----r-- 1 root root  8,  3 Jan 24  2022 dbgio3
crw----r-- 1 root root  8,  4 Jan 24  2022 dbgio4
crw----r-- 1 root root  8,  5 Jan 24  2022 dbgio5
brw----r-- 1 root root  3,  0 Dec 30  2023 fd0          floppy block device
brw----r-- 1 root root  3,  1 Dec 30  2023 fd1          
brw----r-- 1 root root  3,  2 Dec 30  2023 fd2
brw----r-- 1 root root  3,  3 Dec 30  2023 fd3
crw------- 1 root root  6,  0 Dec 30  2021 fdc0         floppy character devcice
crw----r-- 1 root root  6,  1 Dec  9  2022 fdc1
crw------- 1 root root  4,  0 Dec 30  2021 hwgetset     special device for baud control
brw----r-- 1 root root  1,  0 Jan 30  2024 loop0        loop block device
brw----r-- 1 root root  1,  1 Jan 26  2022 loop1
crw----r-- 1 root root 10,  0 Jan 26  2022 loop-control loop control device
crw----r-- 1 root root 13,  0 Feb  5  2024 net0
crw----r-- 1 root root  2,  0 Jan 27  2022 null         /dev/null
crw----r-- 1 root root  1,  1 Jan 27  2022 pmem         physical memory
brw----r-- 2 root root  2,  0 Jan 31  2024 ram0         ram block device
brw----r-- 1 root root  2,  1 Jan 31  2024 ram1
crw----r-- 2 root root 12,  0 Dec 30  2023 ramc0        ram character device
crw----r-- 1 root root 12,  1 Jan 29  2024 ramc1
crw----r-- 1 root root  2,  2 Jan 27  2022 random       /dev/random, reads random values
-rw----r-- 1 root root    318 Dec 30  2023 README
-rw----r-- 1 root root    281 Dec 30  2023 README.bak
brw----r-- 1 root root  0,  0 Jan 27  2022 root         /
crw----r-- 1 root root  1,  0 Jan 27  2022 smem         system memory
brw----r-- 1 root root  0,  0 Jan 27  2022 swap         swap device
crw-----w- 1 root root  0,  0 Sep 11  2024 tty00        terminal 0 IO port by CPU
crw-----w- 1 root root  0,  1 Sep 11  2024 tty01
crw----r-- 1 root root  0,  2 Sep 11  2024 tty02
crw----r-- 1 root root  0,  3 Sep 11  2024 tty03
crw----r-- 1 root root  0,  4 Sep 11  2024 tty04
crw----r-- 1 root root  5,  9 Sep 10  2024 tty09        termimal 9 IOP tty port
crw----r-- 1 root root  5, 10 Jan 27  2022 tty10
crw----r-- 1 root root  5, 11 Sep 11  2024 tty11
crw----r-- 1 root root  5, 12 Sep 11  2024 tty12
crw----r-- 1 root root  7,  0 Dec 30  2021 uiomem       user IO device
crw----r-- 1 root root 11,  0 Jan 26  2022 video        video driver
brw------- 1 root root  0,  0 Sep 10  2024 w00          /dev/w00  first partition first drive
brw------- 1 root root  0,  1 Jan 27  2022 w01          /dev/w01  second partition first drive
brw------- 1 root root  0,  2 Jan 29  2022 w02
brw------- 1 root root  0,  3 Sep 10  2024 w03
crw------- 2 root root  3,  0 Jan 26  2022 w0c0         /dev/w0c0 first partition character device
brw------- 1 root root  0,  4 Dec 30  2021 w10          /dev/w10  first partition second drive
brw------- 1 root root  0,  5 Dec 30  2021 w11
brw------- 1 root root  0,  6 Dec 30  2021 w12
brw------- 1 root root  0,  7 Dec 30  2021 w13
brw------- 1 root root  0,  8 Feb  3  2024 w20          /dev/w20  first partition third drive (2nd controller)
brw------- 1 root root  0,  9 Jan 26  2022 w21
brw------- 1 root root  0, 10 Jan 13  2023 w22
brw------- 1 root root  0, 11 Dec 30  2021 w23  
brw------- 1 root root  0, 12 Dec 30  2021 w30          /dev/w30  first partition forth drive (2nd controller)
brw------- 1 root root  0, 13 Dec 30  2021 w31
brw------- 1 root root  0, 14 Dec 30  2021 w32
brw------- 1 root root  0, 15 Dec 30  2021 w33
crw------- 2 root root  3,  0 Jan 26  2022 wc00         character devices for wxx blcok devices
crw------- 1 root root  3,  1 Dec 30  2021 wc01
crw------- 1 root root  3,  2 Dec 30  2021 wc02
crw------- 1 root root  3,  3 Dec 30  2021 wc03
crw------- 1 root root  3,  4 Dec 30  2021 wc10
crw------- 1 root root  3,  5 Dec 30  2021 wc11
crw------- 1 root root  3,  6 Dec 30  2021 wc12
crw------- 1 root root  3,  7 Dec 30  2021 wc13
crw------- 1 root root  3,  8 Dec 30  2021 wc20
crw------- 1 root root  3,  9 Dec 30  2021 wc21
crw------- 1 root root  3, 10 Dec 30  2021 wc22
crw------- 1 root root  3, 11 Dec 30  2021 wc23
crw------- 1 root root  3, 12 Dec 30  2021 wc30
crw------- 1 root root  3, 13 Dec 30  2021 wc31
crw------- 1 root root  3, 14 Dec 30  2021 wc32
crw------- 1 root root  3, 15 Dec 30  2021 wc33
cr-----r-- 1 root root  2,  1 Sep 24  2020 zero         /dev/zero  reads 00 bytes, no writes


*******************************************************************************************

</pre>
