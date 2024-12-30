#ifndef __UF_SIR_H__
#define __UF_SIR_H__


#include "UF_types.h"


/*
*
* UniFLEX sir 
*
* System Information Record
*
* Each mounted device requires space for the system
* info record.  It contains all necessary info for
* dealing with the space on the device.
*
*/
#pragma pack(1)
typedef struct UF_SIR {
	uint8_t	supdt;			// sir update flag
	uint8_t swprot; 		// mounted read only flag
	uint8_t	slkfr;			// lock free list manipulation
	uint8_t slkfdn;  		// lock fdn list manipulation
	uint32_t  sinitid;		// initializing system identifier
	uint32_t  scrtim;		// creation time
	uint32_t  sutime;		// last update
	uint16_t sszfdn;		// size in blocks of FDN list
	DSKADS   ssizfr;		// size in blocks of volume
	DSKADS   sfreec;		// total free blocks
	uint16_t sfdnc;			// free fdn count
	uint8_t	 sfname[14];    // file system name
	uint8_t	 spname[14];    // file system pack name
	uint16_t	 sfnumb;		// file system number
	uint16_t   sflawc;		// flawed block count
	uint8_t	 sdenf;			// density flag 0=single (floppy)
	uint8_t  ssidf;			// side flag 0 = single (floppy)
	DSKADS	 sswpbg;		// swap starting block #
	uint16_t sswpsz;		// swap block count
	uint8_t	 s64k;			// non-zero = swap count is mult of 64K
	uint8_t	 swinc[11];		// Winchester config info
	uint8_t	 sspare[11];    // spare bytes
	uint8_t	 snfdn;			// # of in core FDNS
	uint16_t scfdn[CFDN];   // in core free fdns
	uint8_t	 snfree;		// # of in core free blcoks	
	DSKADS	 sfree[CDBLK];  // in core free blocks	
	uint8_t	 filler[14];		// 
	// should be @ 0x3f8 in SIR!
	uint32_t	 spartfst;		// first block of partition
	uint32_t	 spartsiz;		// total size of partition
} UF_SIR;
#pragma pack(0)

#endif // __UF_SIR_H__
