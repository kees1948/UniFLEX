
2023-04-04

Posted this version of 63X09 UniBUG.

It has all of UniBUG_B but has been carefully checked all code at $FE00 and up.
Extensively tested.

* revamped the High-Level Interrupt code. Removed fiddling with DP register.
* added separate TRAP actions for Illegal Instructions and Divide-by-Zero
* optimized the code, removed duplicate code

This version succeeds the UniBUG_B release.

Now a divide-by-zero occasion in the code results in signal 12.
