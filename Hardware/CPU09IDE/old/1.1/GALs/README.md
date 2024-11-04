These are the GAL spec's for this board. All made available under GPL V3. Copyright (c) C. Schoenmakers 2019


2020-03-11: IDE_1.PDS has been slightly changed to make sure that /ADEN only happens during DACK. Some drives like that better.

2020-04-13: IDE_G1.PDS has been modified to minimize delay times
IDE_G2.PDS has been modified, DMARQ is now synchronized with QE clock, this eliminates some ugly glitches. I did some more research / debugging. 

Many IDE drives work hapily with DMACK- permanently tied to gnd, the IDE card design has it done that way. 

However some drives like to have DMACK- switched only during a cycle. Furtheremore some IDE drives do a sector transfer as one multi-word (DMA) transfer. 

But I came accross drives that do the first data word as single-word transfer, than up to 125 words in multy-word mode and the last one(s) again as single-word transfer. 

The DMA state  machine had some problems with that. So I modified the GAL's a bit to have behave better and now it does. 

A possible modification is to re-route the DMACK- pin to ADEN- on the board, this requires one trace cut and one wire. See the photo in the V1.1_mod.pdf, in the parent directory.

