This is the extender backplane for the CPU09 system. It has space for 3 + 4 slots.

![backplane 10+4 slots](./Extender.jpg)

<pre>
It uses 2 pcs version 1.0 BP7.
But you can also use the version 1.2 BP7.

It gives 10 slots for UniFLEX and 3 slots for the CPU09IOP 
or 4 slots for FLEX in the same 19" frame.

The backplane wiring is flexible.
There are four locations for SR4 and/or US4 cards.
The GPP and RAM cards do not use external card selection 
and can therefore be placed anywhere, even in locations that do have selection, 
such as the IDE1 SR4 US4 and IOP.

The last part of the expansion backplane is wired for the IOP
with two locations for SR4 and/or US4 and also for a set of four FLEX cards, 
such as CMI - FLXMIN_M - RAM or SR4 - IDE.

The backplane connector for the CPU card uses pin C23 for the RESET.
To solve problems with IOBUS (IOP) and CMI card, 
connect a wire on the IOBUS card between J2 C23 and J2 A28.

</pre>
