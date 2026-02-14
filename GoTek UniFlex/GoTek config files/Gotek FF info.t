Some GOTEK behavior with two different GOTEK's

1 = GOTEK SFRC922B, SFRC922D
    In FF.CFG:
              display-type = auto
    or
              display-type = oled-128x32

  A CRUZER USB stick works without problems.


2 = GOTEK SFRKC30.AT4.35
    In FF.CFG:
              display-type = oled-128x32-rotate

    First time, connect USB with FF.CFG
    Do a factory reset

  A CRUZER USB stick.
    With partion 'U3 System' works in SFRKC30.AT4.35 
    only with a factory reset.
    Removed the 'U3 System' and reformat the CRUZER.
    Still the same problem.
    Needs an FF.CFG on the USB stick ***

*** But there is also a FF.CFG problem
when using both GOTEK models with the same USB stick.

The GOTEK reads the FF.CFG for the correct settings.
Use the correct USB stick with the FF.CFG for each GOTEK type.

Since the GOTEK stores the last FF.CFG settings,
remove the FF.CFG or commend in FF.CFG the line:

#display-type = oled-128x32-rotate

And the USB stick works in both GOTEK types,
with the correct display.

