This is an implementation of Fuse (Filesystem in User space) that allows you
to mount UniFLEX formatted .dsk images and have full access to the contents.

mount the UniFLEX filesystem (example)

fuse_uf wd0.dsk UF

Assuming that mount point UF (a directory) already existed.

df may give:  fuse_uf   14586  2432  12154  17% /home/kees/fuse/UF
<pre>
ls -l UF may give you:

total 0
drwx---r-x 2 root   root      48 Mar  8  1990 act
drwx---r-x 2 daemon daemon  1008 Mar  8  1990 bin
-rw----r-- 1 root   root    4608 Mar 10  1990 core
drwx---r-x 2 root   root     560 Mar  8  1990 dev
drwx---r-x 3 root   root     512 Mar  8  1990 etc
drwx---r-x 4 daemon daemon    64 Mar  8  1990 gen
drwx---r-x 3 daemon daemon   624 Jun 11  1986 include
drwx---rwx 2 root   root       0 Mar  8  1990 kees
drwx---r-x 3 daemon daemon   240 Mar  8  1990 lib
drwx---r-x 2 root   root      32 Mar  8  1990 lost+found
drwx---rwx 2 root   root       0 Mar 10  1990 mnt
drwx---rwx 4 root   root      64 Mar  8  1990 src
drwx---rwx 2 daemon daemon    80 Mar  8  1990 tmp
-rw------- 1 root   root   25999 Mar  8  1990 uniflex
drwx---r-x 6 daemon daemon   112 Mar  8  1990 usr
drwx---rwx 2 root   root       0 Mar  8  1990 usr0
drwx---rwx 2 root   root      32 Mar  8  1990 usr3
</pre>

You can add/delete files, create/remove directories, change permissions, change owner,
create device files (but _only_ if mounted as root and run mknod as root!)

I created it to ease my access to the UniFLEX disk images. The Fuse implementation may (will) have bugs
but it is in a very usable state. 

Building  it goes like:

  apt-get install libfuse-dev

  gcc -g  -Wall fuse_uf.c -fms-extensions  `pkg-config fuse --cflags --libs` -o fuse_uf

warnings may be ignored

If you plan to run gdb with it, your set args should be something like

<pre>
set args <name_of_uniflex.dsk> -s -f \<mountpoint\>
set breakpoints
run
</pre> 

If you need to exit gdb, don't forget to run fusermount -u <mountpoint>

fusermount -u <mountpoint> will unmount the UniFLEX disk image.

Have Fun!


[2022-11-23]
I took the time to figure out why it would not build properly on Linux 64 bit. You may ignore the
warnings the compiler may throw at you. It turns out that the size of the UniFLEX SIR was defined 
wrong. Corrected that. Now I can build and use fuse_uf on a 64 bit Linux again.


