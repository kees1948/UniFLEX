                    FLXMIN + 09FLP - MRDY Signal.


After many tests it appears that the FDC GOTEK combination works only reliably 
on a bus frequency of 3 MHz!

So the FLXMIN + 09FLP + Gotek works well at 3 MHz.
The easiest thing to do now is to generate an MRDY on the FLXMIN.
See "FLXMIN_M.pdf" for schematics BOM's and pictures.

After the MRDY adjustment the system works perfectly on the 5 MHz and 4 MHz bus
with all standard FLEX images
     5" images, SSSD, SSDD, DSSD, DSDD up to 80 tracks.
     8" images, SSSD, SSDD, DSSD, DSDD with 77 tracks.

Also with
     5" images, SSSD, SSDD, DSSD, DSDD up to 255 tracks.
     8" images, SSSD, SSDD up to 255 tracks.

But 8" images DS with tracks 78 and higher will not work!
The sideselect on the GOTEK does not work with 8" images with more than 77 tracks!?!?


A system consisting of CPU09CMI, CPU09IDE, CPU09RAM, and FLXMIN + 09FLP + GOTEK
works now perfectly with a bus frequency of 2, 3, 4 and 5 MHz.

2026-01-18 
    Update.
       Add jumper select for CPU09CMI-ACIA, DIV3, DIV4, DIV7.
     
CdeJ

