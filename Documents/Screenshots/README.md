Here some photo's and screenshots taken from my Linux machine with 3 picocom
terminals talking via USB to RS232 with tty00, tty09 and tty12. The latter 2
go via the CPU09IOP processor with a separate backplane.

photo 1 shows the floppy drive, the 2 IDE disks, the power supply, and from
left to right: CPU, MON, IDE, GPP/FLP, IOP and SR4.

photo 2 shows the boards better with the serial port connectors

boot1, the three logins on the picoterm windows

running2, after loggin in, formatting a diskette

running3, almost starting formatting...

running4, format completed. This is the smallest 80 track disk it can handle.

running5, running 'diskinfo' on the newly created disk

running6, two 'diskrepair' sessions on the new disk.

running7, with the 'prm' tool examining a running kernel (task 0), the memory
allocation is shown, dump of pasrt of kernel. yes, you _CAN_ modify it.....

running8, have the debugger run the formatting program...

running9, shot of the 'dsd' program

running10, in my working directory, run the crash test programs. Yes the kernel 
survives all. :-) Try these on your machine too.



