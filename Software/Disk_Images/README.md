
When your UniFLEX system has the networking operating, making a total
backup of your disk (partitions) is a snap.

On UniFLEX, preparations:

* work only with one partition
* unmount other partitions when you want to backup the root partition
* goto root directory of the target partition

Actual transfer:
* on PC (Linux) side:  nc -l -p32100 -q0 > <NAME_OF_BACKUP>
* on UniFLEX side:   tar cvf - .  |w2sock

wait until done

Restoring (part) from backup.

* On PC (Linux) side: cat <NAME_OF_BACKUP> | nc -l -p32100 -q0
* On UniFLEX side:
-   goto the same root directory as where the backup was made
-   sock2r | tar xvf -      for full restore
-   sock2r | tar xvf -      [match pattern]

Where match pattern can be a full pathname or directory name
