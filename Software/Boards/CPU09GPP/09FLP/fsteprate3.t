*
*
* simple program to mapipulate steprates
* for the CPU09GPP+09FLP hardware
*
        lib     sysdef

start   equ     *
*
* user MUST have id 0!, otherwise access will fail
*
        sys     open,memdev,2
        bes     err1
        std     memfdn
*
        ldd     memfdn
        sys     seek,0,$0E3F0,0
        bes     err1
*
        ldd     memfdn
        sys     write,datloc,1
        bes     err1
*
        ldd     #0
01      sys     term

err1    ldd     #2
        sys     write,errmsg,errlen
        ldd     #1
        bra     01b

errmsg  fcc     "*** Can't change settings!",$d,0
errlen  equ     *-errmsg

memdev  fcc     "/dev/smem",0

memfdn  rzb     2           file reference
*
* adjust this value to the appropriate value
* the setting applies for all drives!
*
* 0 = 3 mS
* 1 = 6 mS
* 2 = 10 mS
* 3 = 15 mS
*
datloc  fcb     0           this is the value to be set

        end     start
