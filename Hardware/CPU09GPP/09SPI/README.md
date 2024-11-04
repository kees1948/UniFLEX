
2022-05-06

It finally happened, I found time and inspiration to populate the 09SPI board. This 
board was collecting dust for almost two tears. While the shown version is 
revision 0.1, I updated the design files already.

This circuit part I had cut out of the original CPU09SPI board and made it a pop-up 
board for the CPU09GPP (or CPU09GPD). Even a CPU09FLX board would be doable 
although not at very high data speeds.

The only part absent on te photo is the 20 MHz clock generator device. It has the same
footprint is the ones on the CPU09MON board. But it is in the mail.

Hidden under the TXB0100 sit the only SMT components of this board: the 3V3 power
regulator. The chip, 2 capacitors (0603) and 2 resistors (0603). This LT3080 is a 
programmable regulator, which creates the 3V3 out of the 5VDC for the 
TXB0100 bi-directional level-switcher/buffer and the target circuit.

Most, if not all SPI devices are not at 5VDC but at 3V3 DC.

One of the first components to test is the W5500 network component. I will perform 
tests if the clock generator can work reliably on 33 MHz eventually.

2022-05-10
Update, I tried to connect the board, but the CPU09GPP was put at /halt with
the board on...... Murphy stroke in 2019, when I was doing the layout. the IO connector 
had it even/odd pin's swapped :-(

So, revised the board design, added some last minute additions and ordered a new board.


