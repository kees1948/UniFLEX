I have created a few test cases. Each one test a diferent characteristic.

crash1.t  test handling of access to non-existent memory. If nothing happens and you can end the process with ^C,
fine but you may not catch pointer errors. The CPU09 UniFLEX version catches this and terminates with a core dump.

crash2.t  test handling of interrupt blocking by a user process. If not acted on, this will freeze the whole machine.
With a _real_ 6809 CPU this would also catch some illegal instruction effects like halt_and_catch_fire. In such
an environment (user)RESET should be used. But with the HD6309 we can use (user)NMI instead.
The CPU09 UniFLEX version terminates this process with a core dump.

crash3.t  test handling of illegal instructions. As the HD6309 catches illegal instructions in emulation and native,
mode we can use the NMI to stop such a process. See above.

crash4.t  test handling of memory protection and more specifically the user process data that resides at $fe00-$ffff.
CPU09 UniFLEX inhibits access to memory $fe00-$ffff in user process context. If the data _could_ be modified,
the whole OS could crash.

All files are in UniFLEX text format.
