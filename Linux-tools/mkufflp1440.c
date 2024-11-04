//
// 	  mak_uf_filesys: build a UniFLEX formatted disk image 
//    Copyright (C) 2019,2020  C.W.P. Schoenmakers
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.
//


#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "UF_types.h"
# include "UF_sir.h"
#include "UF_dfdn.h"

uint8_t temp = 0;

char *outfile = "";
char *fsname  = "";

uint32_t volbc = -1;	// volume block count
uint32_t frebc = 0;     // free block count
uint16_t fdnbc = 0;		// fdn blocks count
uint8_t  blkic = 0;     // free in core blocks
uint32_t frseq = 0;
uint8_t  fdnic = 0;		// free in core fdns
uint16_t fdnfc = 0;		// free fdn count
uint32_t frblk = 0;		// free block count`
uint16_t sswpsz = 0;	// size  of swap
uint32_t sswpbg = 0;	// starting block swap

int ofdn = -1;

#define EPOCHOFFSET 347155200

//
// initial files/directories
//
DIRENTRY rootdir[] =
{
	{0x0001, '.'},
	{0x0001, '.', '.'},
	{0x0002, '.', 'b', 'a', 'd', 'b', 'l', 'o', 'c', 'k', 's'}
}; 

//
// disk fdns, size and date is filled in later
//
DFDN rootfdn[] =
{
	{0x09, 0x3f, 0x02, 0x0000, 0x00000000,  },
	{0x01, 0x80, 0x01, 0x0000, 0x00000000, },
};

//
// work area for free list
//
DSKADS freelist[CDBLK] ={0} ;

// 
// SIR in memory
//
UF_SIR mysir = {0};

//
// conversion routine, store 3 byte vals (BE)
//
void ul2dskad(void *var, uint32_t val) 
{  
 	uint8_t *p = var;	
	*(p+0)  = (val >> 16); 
    *(p+1)  = (val >> 8);
    *(p+2)  = (val); 
} 

static uint32_t fs_uf4ll4(uint8_t* target)
{
    uint32_t myvar = 0;

    myvar += (uint8_t)*(target + 0) << 24;
    myvar += (uint8_t)*(target + 1) << 16;
    myvar += (uint8_t)*(target + 2) << 8;
    myvar += (uint8_t)*(target + 3) << 0;
    return myvar;
}

static void fs_ll4uf4(uint8_t* target, uint32_t val)
{

        *(target + 0) = val >> 24;
        *(target + 1) = val >> 16;
        *(target + 2) = val >> 8;   
        *(target + 3) = val >> 0;
}

static uint32_t fs_uf3ll4(uint8_t* target)
{
    uint32_t myvar = 0;

    myvar += (uint8_t)*(target + 0) << 16;  
    myvar += (uint8_t)*(target + 1) << 8;    
    myvar += (uint8_t)*(target + 2) << 0;
    return myvar;
}

static void fs_ll4uf3(uint8_t* target, uint32_t val)
{
  
    *(target + 0) = (val >> 16) & 0xff;
    *(target + 1) = (val >> 8)  & 0xff;
    *(target + 2) = (val >> 0)  & 0xff;
}


static uint16_t fs_uf2ll2(uint8_t* target)
{
    return ((*(target + 0) << 8) + (*(target + 1)));
}

static void fs_ll2uf2(uint8_t* target, uint16_t val)
{
    *(target + 0) = val >> 8;
    *(target + 1) = val & 0xff;
}

//
// conversion routine, return 3 byte vals (BE) 
//
uint32_t dskad2ul(void *var)
{
	uint32_t val = 0;
	uint8_t *p = var;

	val = (*(p+0) << 16)+(*(p+1) << 8)+(*(p+2));

	return val;
} 

// 
// grow the target file
//
int add0blk(int fdn)
{
	uint8_t buffer[DBLKSIZ];

	bzero((void*) &buffer, DBLKSIZ);

	return (write (fdn, (char*)&buffer, DBLKSIZ));
}

//
// make in core fdn list
//
void mffd(void)
{
	int i, j;

    fdnic = (fdnfc > 50) ? 50 : fdnfc;
    mysir.snfdn = fdnic;
    j = (sizeof(rootfdn)/64) + 1;
    for (i = 0; i < fdnic; i++)
    {
        mysir.scfdn[i] = htons(j);
        j++;
    }
}

//
// make in core free block list
//
void mfbl(void)
{
	int i;

    mysir.snfree = blkic;
	for (i = blkic; ;)
	{
		if (frseq != volbc)			// no forward link if blocks <100
		{
			ul2dskad((uint8_t*)&mysir.sfree[i -1], frseq);
		}
		if (--i == 0)
		{
			break;
		}
		frseq++;
	}
    frblk = frseq;
}

//
// build the SIR
//
void mksir(void)
{
	int i, j;
//
	fs_ll2uf2((uint8_t*)&mysir.sinitid, 0);
	fs_ll4uf4((uint8_t*)&mysir.scrtim, ((uint32_t) time(NULL) -EPOCHOFFSET));
	fs_ll4uf4((uint8_t*)&mysir.sutime, ((uint32_t) time(NULL) -EPOCHOFFSET));
//
	fs_ll2uf2((uint8_t*)&mysir.sszfdn, fdnbc);
	fs_ll4uf3((uint8_t*)&mysir.ssizfr, volbc -1);
	fs_ll4uf3((uint8_t*)&mysir.sfreec, frebc);
//
	strncpy (mysir.sfname , fsname, 14);
	strncpy (mysir.spname , "UniFLEX", 14);

	fs_ll2uf2((uint8_t*)&mysir.sfnumb, 8888);
	fs_ll2uf2((uint8_t*)&mysir.sflawc, 0);
//
	fs_ll2uf2((uint8_t*)&mysir.sfdnc, fdnbc * 8 - 2);
	mysir.sdenf = 0x81;
	mysir.ssidf = 0x13;
//
	fs_ll4uf3((uint8_t*)&mysir.sswpbg, 0);
	fs_ll2uf2((uint8_t*)&mysir.sswpsz, 0);
	mysir.s64k = 0;
//
	mffd();	
// 
	mfbl();
//  
	if (sswpsz)
	{
		sswpbg = (volbc + 7) & 0xfffffff8UL;
		fs_ll4uf3((uint8_t*)&mysir.sswpbg, sswpbg); 		
		fs_ll2uf2((uint8_t*)&mysir.sswpsz, sswpsz);
	}

// set value for multiple images
	fs_ll4uf4((uint8_t*)&mysir.spartsiz, volbc);

	lseek(ofdn,(ulong)( 1*DBLKSIZ) , 0);
	write(ofdn, (char*)&mysir, DBLKSIZ);
}

//
// create root FDNS
//
void mkrfdn(void)
{

	fs_ll4uf4((uint8_t*)&rootfdn[0].fsize, sizeof(rootdir));
	fs_ll4uf3((uint8_t*)&rootfdn[0].fblocks[0], fdnbc + 2);
	fs_ll4uf4((uint8_t*)&rootfdn[0].ftime, (uint32_t) time(NULL) -EPOCHOFFSET);
	fs_ll4uf4((uint8_t*)&rootfdn[1].ftime, (uint32_t) time(NULL) -EPOCHOFFSET);
// write fdn
	lseek(ofdn,(ulong)( 2*DBLKSIZ) , 0);
	write(ofdn, (char*)&rootfdn, sizeof(rootfdn));
// write root dir
	lseek(ofdn,(ulong)((fdnbc + 2)*DBLKSIZ) , 0);
	write(ofdn, (char*)&rootdir, sizeof(rootdir));
}

//
// do free block list
//
void dofb(void)
{
	int i;

// zero out buffer
	for (i = CDBLK; i >= 0 ; i--)
	{
		ul2dskad((char*)&freelist[i], 0);
	}
// build list
	for (i = 100; i > 0 ; i--)
	{
		if (frseq == (volbc - 1))
		{
			goto FBOUT;
		}
		frseq++;
		ul2dskad((char*)&freelist[i-1], frseq);
	}
// next list?
	if ((frseq) != (volbc - 1))
	{
		ul2dskad((char*)&freelist[0], frseq);
	}

FBOUT:
	if (i == 100)	// no link in this block
	{
		return;
	}
	lseek(ofdn, (ulong)(frblk * DBLKSIZ), 0);
	write(ofdn, (char*)&freelist, sizeof(freelist));
	frblk = frseq;
}

//
// make free list
//
void mkfree(void)
{
	
	do
	{
		dofb();
	}
	while (frseq < (volbc - 1));
}

//
// build disk structures and write them
//
void strd(void)
{

	fs_ll2uf2((uint8_t*)&rootdir[0].dfdn, 1);
//	rootdir[0].dfdn = htons(1);	
	fs_ll2uf2((uint8_t*)&rootdir[1].dfdn, 1);
//	rootdir[1].dfdn = htons(1);	
	fs_ll2uf2((uint8_t*)&rootdir[2].dfdn, 2);
//	rootdir[2].dfdn = htons(2);

//	fdnbc = (int)(volbc * 3/100);
//	fdnbc = (fdnbc > 0) ? fdnbc : 1;
//	frebc = (volbc) - (fdnbc + 3);				// boot,sir,rootdir
//	fdnfc = (fdnbc * 8) - (sizeof(rootfdn)/64);
//	blkic = ((frebc + 1) % 100) ?  : 100;
//	frseq = fdnbc + 3; // boot, sir, rootdir

	mksir();		// make the SIR block
	mkrfdn();		// make the root fdn
	mkfree();		// free block list

/**/
	printf("frebc = %d \n", (unsigned)frebc);
	printf("blkic = %d \n", (unsigned)blkic);
	printf("frseq = %d \n", (unsigned)frseq);
	printf("volbc = %d \n", (unsigned)volbc);
	printf("fdnbc = %d \n", (unsigned)fdnbc);
	printf("fdnfc = %d \n", (unsigned)fdnfc);
	printf("sswpsz = %d \n", (unsigned)sswpsz);
/**/
}

//
//
//
int main(int argc, char **argv)
{
	int i, opt;

	while ((opt = getopt(argc, argv, "f:s:n:")) != -1)
//	while ((opt = getopt(argc, argv, "b:f:s:n:")) != -1)
	{
		switch (opt)
		{
//			case 'b':
//				volbc = atoi(optarg);
//				break;
			case 'f':
				outfile = optarg;
				break;
			case 'n':
				fsname = optarg;
				break;
			case 's':
				sswpsz = atoi(optarg);
				break;
			default:
				fprintf(stderr, "usage: xxx -b #blocks -f <outputname> -n <diskname> -s #swap size>\n");
				exit(-1);
				break;
		}
	}
        volbc = 2840;
	if ((volbc == -1) || (outfile == ""))
	{
				fprintf(stderr, "usage: xxx -b #blocks -f <outputname> -n <diskname> -s #swap size>\n");
				exit(-1);
	}

	volbc = ((volbc + 7 ) / 8) * 8;

	if (volbc < 8)
	{
				fprintf(stderr, "# blocks should be a multiple of 8 \n");
				exit(-1);
	}

	if (sswpsz)
	{
		sswpsz = ((sswpsz + 7) / 8) * 8;

	}

	if((ofdn = open (outfile, O_RDWR|O_CREAT|O_TRUNC, 0600)) < 0)
	{
		fprintf( stderr, "error opening file\n");
		exit(1);
	}
// calculate values
	fdnbc = (int)(volbc * 3/100);
	fdnbc = (fdnbc > 0) ? fdnbc : 1;
	frebc = (volbc) - (fdnbc + 3);				// boot,sir,rootdir
	fdnfc = (fdnbc * 8) - (sizeof(rootfdn)/64);
	blkic = ((frebc + 1) % 100) ?  : 100;
	frseq = fdnbc + 3; // boot, sir, rootdir
// build zeroed target file
//	for (i = 0; i < volbc; i++)
	for (i = 0; i < fdnbc + 5; i++)
	{
		add0blk(ofdn);
	}
	for (i = 0; i < sswpsz; i++)
	{
		add0blk(ofdn);
	}

	strd();		// setup all disk structures

	close(ofdn);
}
