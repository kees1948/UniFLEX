#ifndef __UF_DFDN_H__
#define __UF_DFDN_H_

#include "UF_types.h"

#pragma pack (1)
typedef struct {
	uint8_t		fmode;
	uint8_t		faccess;
	uint8_t 	flinks;
	uint16_t	fownuid;
	uint32_t	fsize;
	DSKADS		fblocks[13];
	uint32_t	ftime;
	uint8_t		filler[12];
} DFDN;
#pragma pack(0)

#define DFM_BUSY	0b00000001		// fdn is used (busy)
#define DFM_SBLK    0b00000010		// block special
#define DFM_SCHR	0b00000100		// char special
#define DFM_SDIR    0b00001000		// directory 
#define DFM_PRDF    0b00010000		// pipe read
#define DFM_PWRF    0b00100000		// pipe write

#define DFA_CUR		0b00000001		// user read
#define DFA_CUW     0b00000010		// user write
#define DFA_CUE		0b00000100      // user execute
#define DFA_COR		0b00001000		// other read
#define DFA_COW		0b00010000      // other write
#define DFA_COE		0b00100000      // other execute
#define DFAXSET     0b01000000      // uid execute set

#endif // __UF_DFDN_H__
