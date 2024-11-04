[2023-09-30]

This is the assembler code for the CPU09GPP board when fitted with the 09FLP board. 

It controls up to 4 drives and supports 3.5", 5 1/4" and 8" drives.
Because the older drives had not the standard connections as the later IBM PC
drives had, extra jumper options are added to accomodate the drive select needs.

One can adapt the drive select patterns to his own needs.
Drive 0 and 1 have been preset to use the famous ribbon cable with the wire twist.
Drive 2 and have a basic select, motor on should be jumpered on these drives.

Be aware that the step rate is to be controlled from the UniFLEX CPU side.
Location $E3F0 (kernel side) (03F0, GPP side)holds the steprate select bits. 
A simple tool to manipulate these bits is to be added.


[*] note, the previous code release did not support drive 2 and 3 properly.


