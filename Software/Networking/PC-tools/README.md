
Here some tools that should run on a Linux (PC) platform.

===============================================================
'ufclient.c' is a simple telnet client that I found. All the
offical telnet clients gave problems, must notably with the
handling of ^C.

In  ufclient.c , I only disabled the negotiation at startup.
On my Linux PC it is a perfect tool.

At my UniFLEX system I have running:

 ptyserv -d /dev/ptys05 -p 12345  (started from init)

This starts a server type process on the UniFLEX system

Now run from another PC (linux)

ufclient 192.168.1.179 12345 

kees@schoenws:~/UFFLLIN/FLEXDIR/UNIFLEX/NET$ ufclient 192.168.1.179 12345
Connected...















Still running after 10(40!) years.....

   UniFLEX Operating System 1988
10:12:06 Thu Dec 19 1224      tty05


Login: 

This works as if you had a terminal connected... ttyset works like normal

Login: kees
Password: 
You have mail.
++ ttyset
-raw      +echo     +tabs     -case     
+alf      +becho    -schar    -cntrl    
+esc      -xon      -any      
8 Data    1 Stop    none                
crt       bs=7F     dl=18     
++ 

