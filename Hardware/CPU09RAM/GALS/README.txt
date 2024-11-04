<pre>
This GAL version is for UniFLEX, where the blocksize is 512 bytes 

UniFLEX compatible GAL's RAM_U.XXX


FLEX compatible GAL's RAM_F.XXX

Memory pages are now 256 bytes, need a special trick to address them properly.


For FLEX there are mac 16384 256 Byte pages  EC00-ECFF,
As the hardware can't change much I used a trick to address all.


Assume  D contains  FLEX sector address  i.e.  12345 ($3039) %   0011 0000 0011 1001
The latch should be addressed as if the block is 512 Byte 
So shift the value one bit to the right                          0001 1000 0001 1100 (1)

The (1) bit must replace the most significant bit of A           1001 1000 0001 1100
                                                                 ^

In code:

        lsra           shift FLEX sector address to the right
        rorb           lsb in carry
        bcc    l0      
        ora    $80     if carry set, set bit & of 'A'
l0      std    $EE00   set address select


</pre>
