
This is for the CPU09RAM board in a CPU09CMI system.

You can find the card data at the UniFlex section.


The CPU09RAM is an Eurocard size board with logic and space for up to 8 pieces 
of AS6C4008-55PCN on it, giving a total of 4194304 bytes of volatile storage.

The board provides:
    * up to 4 MB of RAM storage as RAM DISK
    * will be located at EC00...EFFF in the address range
    * provides RAM access in 512 Byte pages
    * has address latches to select any of the max 8192 available pages
    * has dipswitches that can be read out to inform the driver
    * options, with switches, 1 or 2 disk images. 
  

The first device is to be U4 and the last device U11.
1 16V8 GAL serve as address decoding and bus signal buffering.

The 2 x 4 position dipswitches can be used to signal the driver and initialization
code to act on it. Some of the switch signals are also available as jumpers on the 
front edge of thwe board.

As the memory is volatile, the contents are lost on power off. But the RAM disk
can be very helpful when you run from an SD-card. Copy your work over to the RAM disk,
run edit and assembly on the RAM disk contents and save results back to SD-card 
when done. This will prevent the SD-card to wear-out fast.


At present the dip-switches have the following assigments:

    SW2-4,  ON = Format protection. Vdisk will not format.
    SW2-3,  ON = Reserve room for disk cache (SW1-4 must be OFF). 
    SW2-2,  ON = Try for 2 virtual drives if SW1-3 or SW1-2 is ON. 
    SW2-1,  ON = FLEX 512 byte pages, card present flag.

    SW1-4,  ON = FLEX 4MB disk
    SW1-3,  ON = FLEX 8" DSDD
    SW1-2,  ON = FLEX 5" DSDD
    SW1-1,  ON = Battery low set, Off = Sense Battery low 

[2024-04-23] The first board version (0.9.1) had still an error, the clocks to the address
latches were in reverse order (lowbyte-highbyte).
I corrected that in 0.9.5. The 0.9.1. boards can be fixed easily by 2 trace cuts and 2 small wires.

Board version 0.9.5 has this fixed.

[2024-09-25] 9 options for RAM standby voltage,
             included UPS Battery standby.

[2024-10-03] some UPS PCB design options.

(2024-12-20) 10 options for RAM standby.
             SMD RAM UPS print use more copper for SMD chip.
             Change/move some footprints for more space.
             C5 can be SMD on solder side or C5 can be on top side. 
             Change footprint for 2 types trimpot.
             Corrected footprint for standart TL431.
             Simpler adjustment procedures. 

(2024-12-24) Change the R7 and P2 value for better adjustment.

(2025-03-07) Place C3 (10nF) across R2 for better reset.
             New PCB layout with 0805 SMD pad for C3.

(2025-06-20) Change R9 into 1K8, reduce alarm current.
             Easy PCB layout support more SMD parts.
