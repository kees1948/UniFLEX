Here are some handy tools for interaction with the UniFLEX files

Shell scripts that ease life.



One should not miss this one (see photo) when wanting to transfer .dsk images to a real (IDE) disk. For this to work the user MUST be
in the 'disk' group. Add him and re-login the user. Copying is done with:

dd of=/dev/sde if=MYDISK.dsk  conv=swab
or
dd if=/dev/sde of=MYDISK.dsk bs=512 count=31212 conv=swab

The first example is writing your image to the real hardware. The second is from the real hardware back into an image.
The "conv=swab" is important, it swaps the byte of words while copying. Forgetting it results in weirs things...

The size parameter should reflect the real size of the filesystem on the disk, omitting it will give  a (too) large image file. The device
should be a proper name, check dmesg.

