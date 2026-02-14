#!/bin/bash

MYPORT=32180

while(true)
do
     nc -l  -p $MYPORT |tr -s "\015" "\012"| pr -e4 -t -w80 -l56 | enscript -2 -r -i0 -MA4

     echo "completed print"
     sleep 1
# for test enable the following line, it will empty the queue
  lprm -

done
