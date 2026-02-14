<pre>
Here I will try to provide information on printing with the CPU09 system.

In the past printing was done with either a parallel or serial printer.
As a parallel inteface (and driver) is not present for the CPU09 system
only two options remain:

*	serial printer

* 	network printer

For each category a number of sub options are there, depending on the
printer hardware that you have.

[serial printer]

- each of the /dev/tty ports can be used for a serial printer. In most
cases you may need hardware handshaking, the 'portbaud' utility can set
both, the baudrate and the handshake status, in one command. Note that
/dev/tty00 is excluded from portbaud set.

- in the past, most UniFLEX programs had their own specific printer driver
to control options like 'bold, italic, overstrike' and such. If you (still)
have a compatible printer, use it with success.

- if the output of your printer file is plain text, alomost any printer can 
be used. Maybe a simple post filter that strips out selected control characters
may help here.

- example

* a generic text based printer is connected to /dev/tty04
* the baudrate is to be 2400, handshake enabled
* portbaud /dev/tty04 2400 cts=enb
* now you can setup your printer spooler to use this printer
* it may be necessary to setup the printer in a startup file or in
  /etc/init.ctrl to have it done on each boot.

[network printing]

- here, most printers do well with plain text, the same remarks as for 
serial printing applies here too. But printing via the network can be done
in a multitude of ways, all depending on your network layout.

- options

a) printing data is directly forwarded to a dedicated receiving process that 
   can do processing of special characters and then forward it to 'cups'.
b) printing data can be directed to the network printer spooler (lpd)
c) printing data is forwarded to the UniFLEX printer spooler which prints
   to a virtual device (pipe). A dedicated process forwards all data from
   this pipe device over the network to a decicated receiving process as
   under a).
   
Method c) is th most attractive. Every output (*) on UniFLEX can use the 
local spooler. The ouput is than, without any intermediate action, printed on your regular printer.

(*) the only known exception is Stylograph, it uses a special device 'styprint'.
We can create a virtual device with that name and forward it's data. However
I expect that there are no Stylograph users on UniFLEX at current times.....

Setting up UniFLEX for c) with the 'Standard Spooler', we call the spooler 'npr1':

* create a directory  /usr/spooler/npr1
* set the owner to root, permissions --rw----
* link /etc/print with /bin/npr1

Run 'insp npr1' at some moment to start the spooler process. Most convenient is 
to run it from 'init' with an extra line in 'init.ctrl'.

Now, npr1 <somefile>, cat <somefile> | npr1, or relasmb <afile> +ls |npr1
forwards all data to the network 192.168.1.150 (default) at port 32180 (default).
The default can be overridden by commandline arguments or by changing the source
and compile your own version.

The last piece of software you need is the receiving end on your PC.

For Linux I would recommend this script:
<pre>
#!/bin/bash

MYPORT=32180

while(true)
do
     nc -l  -p $MYPORT |tr -s "\015" "\012"| pr -e4 -t -w80 -l56 | enscript -2 -r -i0 -MA4

     echo "completed print"
     sleep 1
#
# for test enable the following line, it will empty the queue
#  lprm -

done
</pre>
===

We use 'nc' to grab the data from the network. 'tr' does the EOL conversion by 
changing all 'CR' characters into 'LF'. 
Next, pr is a page formatter with tab stops at each 4 positions, limits line length
to 80 characters and pages to 56 lines. 'enscript' finally creates 2 columns on
an A4 page in landscape orientation and feeds it directly into the system spooler.


UniFLEX    data -->[system spooler]-->/dev/npr1-->[netpr]-->(network)
Linux PC   (network)-->nc-->tr-->pr->enscript-->cups
</pre>




