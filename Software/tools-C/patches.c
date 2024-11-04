/* byte compare */
#asm
        info    compare for patches
        info    by Wijnand Lekkerkerker
        info    September 1988
#endasm
#include <stdio.h>
char    asciibuff[20];
int     c1, c2;
int     index;
int     printflag=0;
long    last_address = -1L;
long    pos=0L;
FILE *fp1, *fp2, *fopen();
main(argc, argv)
int     argc;
char    *argv[];
{
        pflinit();
        if (argc != 3) {
                printf("Usage: patches file1 file2\n");
                exit(1);
        }
        if ((fp1=fopen(argv[1],"r")) == NULL) {
                printf("patches: can't open %s.\n", argv[1]);
                exit(1);
        }
        if ((fp2=fopen(argv[2],"r")) == NULL) {
                printf("patches: can't open %s.\n", argv[2]);
                exit(1);
        }
        printf("\nPatches found in %s, changed from %s.\n",argv[2],argv[1]);
        while ((c1=getc(fp1)) != EOF && (c2=getc(fp2)) != EOF) {
                if (printflag) {
                        if (c1 != c2)
                                printc(c2);
                        else
                                skipc();
                } else if (c1 != c2)
                        startprintc();
                pos++;
        }
        if (printflag)
                leegbuffer();
        /* report lentgh differences */
        fclose(fp1);
        fclose(fp2);
}
startprintc()
{
        long    address;
        index = 0;
        address = pos - pos % 16L;
        if (address != last_address + 16L)
                putchar('\n');
        last_address = address;
        printf("%08lx ", address);
        if (address != pos)
                while (address++ < pos) {
                        printf("== ");
                        asciibuff[index++] = '_';
                }
        printflag = 1;
        printc(c2);
}
printc(c)
int     c;
{
        printf("%02x ", c);
        if (c >=32 && c < 127)
                asciibuff[index++] = c;
        else
                asciibuff[index++] = '_';
        if (index == 16) {
                asciibuff[index] = 0;
                printf(" %s\n", asciibuff);
                printflag = 0;
        }
}
skipc()
{
        printf("== ");
        asciibuff[index++] = '_';
        if (index == 16) {
                asciibuff[index] = 0;
                printf(" %s\n", asciibuff);
                printflag = 0;
        }
}
leegbuffer()
{
        while (index < 16) {
                printf("   ");
                asciibuff[index++] = ' ';
        }
        asciibuff[index] = 0;
        printf(" %s\n", asciibuff);
}
