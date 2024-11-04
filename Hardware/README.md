This folder contains the hardware components of my CPU09 system.

If you like to tinker with hardware and UniFLEX or FLEX this system
may be attractive for you. The designs are made such that the boards
will not get damaged quickly and they withstand quite some level of abuse :-)
They are ideal for a Computer Sience class at school.

For a minimal system you need:
* CPU09BP       this is the system backplane and accommodates up to 7 slots. It has an ATX compatible power connector.
* CPU09MM3      CPU and memory management
* CPU09MON      provides RAM (1MB), ROM (2K), timer, console port
* CPU09ID6      DMA driven IDE interface, up to 2 drives

Further extentions include:
* CPU09SR4          4 ACIA serial ports, baudrate selection per port, RS232 interface
* CPU09US4          4 ACIA serial ports, baudrate selection per port, USB/TTL interface
* CPU09GPP          general purpose IO processor, RAM, ROM, timer, IO expansion connector 1K range
* CPU09GPP/09FLP    separate IO processor, with additional WD2793 floppy disk interface
* CPU09IOP          serial IO processor, serves up to 12 ports (3 CPU09SR4)
* CPU09FLX          carrier board for 09FLP, intended for direct IO based floppy access (FLEX)
* CPU09EXP          experimenters board, fully buffered with large prototype area
* CPU09BP3          3 slot backplane with simple 5V power connector
* CPU09BP4          4 slot backplane with simple 5V power connector
* CPU09GPP/09NET    TCP/IP network interface, provides socket calls to the kernel
* CPU09RAM          RAM disk board with up to 4096 KByte of RAM on board
* CPUXXCMI          board for 6802/6809 CPU, 65K RAM, 2K ROM, console port (FLEX)
With a CPUXXCMI, CPU09FLX/09FLP you would have a simple full blown FLEX compatible system

In development are:
* CPU09VID          512x472 B/W video board.
* CPU09GPP/09ECN    Econet compatible (hardware) interface with MC68B54 ADLC
* CPU09KDB          kernel debugger, fun to step a running kernel, trap on address etc.
* CPU09GPP/09SPI    generic SPI interface (i.e. sd-card)
* CPU09SDC          variant of IDE board where SD-card can be used with minimal extra depth.
* CPU09GPD          variant of GPP with on-board DMA controller, for DMA type peripherals

The hardware provides a good UniFLEX capable system, with integrated user/system separation
and protection, no user process can bring down the OS.


Essentials skills: good in soldering!


2021-08-14: Guus Assmann set himself to the tedious job to create a spreadsheet with all the 
components for the CPU boards and most of the components have even a stock number for 
various webshops. I owe him great thanks.

##################################################################################################

General NOTE:

Choosing  GAL's, the Lattice brand is strongly preferred!

##################################################################################################
