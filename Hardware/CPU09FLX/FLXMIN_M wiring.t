
BACKPLANE

Screen layer text.
Wire:
  CPU09CMI  C1A  > FLXMIN   C1B   (DIV5)
  CPU09CMI  C28A > FLXMIN   C28B  (DIV3)
  CPU09CMI  A29  > FLXMIN   BCA   (U10_CE)
  CPU09CMI  C29A > FLXMIN   C29B  (DIV7)
  connected already               (DIV4)
  CPU09CMI  C2A  > CPU09ID6 D23C  (DIV6)

Connecor numbering.
Wire:
  CPU09CMI  A4  > FLXMIN   A4     (DIV5)
  CPU09CMI  C28 > FLXMIN   C28    (DIV3)
  CPU09CMI  C29 > FLXMIN   C29    (U10_CE)
  CPU09CMI  A4  > FLXMIN   A4     (DIV7)
  CPU09CMI  A3    to All   A3     (DIV4)
  CPU09CMI  C3  > CPU09ID6 C23    (DIV6)


CPU09CMI
  U14-18 > J1-C29                 (U10_CE)

FLXMIN_M 
Solder side wire:
  U2-13 > J3-1, J3-5 > J1-C29     (U10_CE)
  U2-14 > J3-2, J3-6 > J1-C28     (DIV7)
  U2-15 > J3-3, J3-7 > J1-A28     (DIV3)
  U2-16 > J3-4, J3-8 > J1-A4      (DIV4)
  U2-19 > J1-A3                   (DIV5)


Solder side wire (true a hole) Top side:
  U2-17  > 74HC125-p2  (MRDY_)

Top side wire:
  J1-C24 > JP1-p1  (B_MRDY)


