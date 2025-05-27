
[2025-05-27]

uf_fl_boot.img  contains 3 bootable uniflex versions

* uniflex       runs completely from floppy.                /etc/init
* uniflex8      boots from floppy but runs from IDE disk.   /etc/init
* uniflex9      runs completely from floppy. into /bin/sh

former image had the root/pipe/swap majors stil set for very old kernels (1),
now corrected to correct values (3).

