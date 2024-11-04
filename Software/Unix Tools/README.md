In the 80's and 90's we ported a lot of Unix tools to UniFLEX. Some made their way into the disks of the 
SWTPCemu and memulator environment. But mostly without the sources. I started to dig in my archives and 
found a lot of those source code files, either in C or in assembly language.

As one of those gems I found a working version of the famous 'tar' utility. 'pstat' I recompiled to adapt it via the sysinfo 
systemcall for the various DAT rams mechanisms. The same for 'prm'. 'qdb' need to be changed to work with the longer stack
of the 63X09, but that source code is in the TSC archives. I also found the source of the initial shell version in the TSC
archives. 
Further more I found my crossassemblers back, for 6800, 6801, 6502, 8085. I am still looking for th macro set which allowed
me to assemble code for the SC/MP with the 6809 relasmb. It produced very nice and correct code for this chip. 
I know, the SC/MP is no longer around, but it gives a good insight in the power that the macro assembler provides.

<pre>
ls.c
tar.c
pstat.c
cu.c
a good termcap library in C
cal.c
cron.c
hexbin.c
init.c
lgrep.c
nohup.c
od.c
pg.c
prio.c
sort.c
strip.c
su.c
tree.c
lower.c 
upper.c
wc.c

to name a few. 

Further more other assembly and C programs that we made and used for various purposes. As I can go thru these I will post 
them here too.

</pre>
