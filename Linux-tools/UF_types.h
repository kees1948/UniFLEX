#ifndef __UF_TYPES_H__
#define __UF_TYPES_H__

#include <arpa/inet.h>     //  htons, htonl, ntohl, noths

#define DBLKSIZ		512
#define CFDN 		50  
#define CDBLK 		100
#define DIRSIZ		14          // directory name size
#define DIRENT		16			// directory entry size
#define MAPSIZ		13			// 10 is indir, 11 is double indir, 12 is triple indir



typedef struct { uint8_t hh; uint8_t mm; uint8_t ll;} DSKADS;
typedef struct { uint16_t dfdn; uint8_t dnam[DIRSIZ];} DIRENTRY;




#endif // __UF_TYPES_H__
