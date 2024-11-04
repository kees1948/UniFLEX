/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>
  Copyright (C) 2011       Sebastian Pipping <sebastian@pipping.org>
  Copyright (C) 2019		Kees Schoenmakers

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.

	gcc -g  -Wall fuse_uf.c -fms-extensions  `pkg-config fuse --cflags --libs` -o fuse_uf


	This is a Fuse implementation to access a 6809 UniFLEX disk image
	No protection against concurrent access however.
	It's main implementation objective was to be able to
	create, read, write, remove files and to create directories
	in order to be able to build a custom UniFLEX disk image

*/

#define FUSE_USE_VERSION 26

#define HAVE_UTIMENSAT

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
/* For pread()/pwrite()/utimensat() */
#define _XOPEN_SOURCE 700
#endif

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/sysmacros.h>
#include <stdint.h>
#include <libgen.h>
#include <linux/limits.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif
#include "fuse_UniFLEX.h"

char *index(const char *s, int32_t c);
//unsigned int major(dev_t dev);
//unsigned int minor(dev_t dev);


// the disk image reference
static int32_t fs_fdn = -1;

//
// various temp storages
uint8_t filebuf[UFBLKSIZ];			// temp for data
// empty image
const uint8_t zerobuf[UFBLKSIZ] = {0};	// empty buffer 

// prototypes
static int32_t fs_getblk(void);
static int32_t fs_putblk(int32_t block);
static int32_t fs_errno = -1;

static LINSIR lhdsir = {0};			// Linux 'SIR'
static uint16_t ucrdir = -1;

char pathcopy[PATH_MAX];

//
// initial files/directories
//
const UDIRENT newdir[] =
{
    {0x00, 0x00, "."},
    {0x00, 0x00, ".."},
};

//----------------------------------------------------------
//
// the lowest level interface
//
static int32_t fs_readblock(int32_t blockno, uint8_t* buffer)
{
	int32_t count;

	if (lseek(fs_fdn,(off_t)( blockno * UFBLKSIZ), 0) < 0)
	{
		fs_errno = errno;
		return -1;
	}
	if ((count = read(fs_fdn, (char*)buffer, UFBLKSIZ)) < 0)
	{
		fs_errno = errno;
		return -1;
	}
	return count;
}

static int32_t fs_writeblock(int32_t blockno, uint8_t* buffer)
{
	int32_t count;

	if (lseek(fs_fdn,(off_t)( blockno * UFBLKSIZ), 0) < 0)
	{
		fs_errno = errno;
		return -1;
	}
	if ((count = write(fs_fdn, (char*)buffer, UFBLKSIZ)) < 0)
	{
		fs_errno = errno;
		return -1;
	}
	return count;
}

//----------------------------------------------------------
//
// conversion routines Linux <->  UniFLEX format and types
//
static uint16_t fs_uf2ll2(uint8_t* target)
{
	return ((*(target + 0) << 8) + (*(target + 1)));
}

static void fs_ll2uf2(uint8_t* target, uint16_t val)
{
	*(target + 0) = val >> 8;
	*(target + 1) = val & 0xff;
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

//static uint16_t fs_ll2ul2(uint8_t* target)
//{
//	return (uint16_t)(((*target + 1) << 8) + *(target+1));
//}

static int32_t fs_isdir(LINODE* ino)
{
	return ((ino->f_mode & S_IFDIR) == S_IFDIR);
}

static int32_t fs_isfile(LINODE* ino)
{
	return (ino->f_mode & S_IFREG);
}

static int32_t fs_isspec(LINODE* ino)
{
	return (ino->f_mode &(S_IFCHR|S_IFBLK));
}

//---------------------------------------------------------------------
//
// read an UniFLEX diskblock and extract the file node
// convert it immediately to a Linux format
//
static int32_t fs_readfdn(uint16_t d_ino, LINODE* linoptr)
{
	DINODE 	fdnbuf[8];					// temp for 8 disk inodes in buffer
	DINODE uf_ino;
	uint16_t i = (d_ino + 15) % 8;
	uint16_t b = (d_ino + 15) / 8;
	int32_t l;

	if (fs_readblock(b, (uint8_t*)&fdnbuf) < 0)
	{	
		return -1;
	}
	memcpy((char*)&uf_ino, (char*)(&fdnbuf) + (i * sizeof(DINODE)), sizeof(DINODE));	

	linoptr->f_mode = 0;

	if(uf_ino.f_mode & DIM_FBUSY)
	{
		linoptr->f_mode |= S_IFREG;
	}
	if(uf_ino.f_mode & DIM_FSCHR)
	{
		linoptr->f_mode |= S_IFCHR;
		linoptr->f_mode &= ~S_IFREG;
	}
	else if(uf_ino.f_mode & DIM_FSBLK)
	{
		linoptr->f_mode |= S_IFBLK;
		linoptr->f_mode &= ~S_IFREG;
	}
	else if (uf_ino.f_mode & DIM_FSDIR)
	{
		linoptr->f_mode |= S_IFDIR;
		linoptr->f_mode &= ~S_IFREG;
	}
	if (linoptr->f_mode)	// used fdn?
	{
		linoptr->f_mode |= ((uf_ino.f_access & DIA_FACUR) ? S_IRUSR:0);
		linoptr->f_mode |= ((uf_ino.f_access & DIA_FACUW) ? S_IWUSR:0);
		linoptr->f_mode |= ((uf_ino.f_access & DIA_FACUE) ? S_IXUSR:0);
		linoptr->f_mode |= ((uf_ino.f_access & DIA_FACOR) ? S_IROTH:0);
//		linoptr->f_mode |= ((uf_ino.f_access & DIA_FACUR) ? S_IRGRP:0);
//		linoptr->f_mode |= ((uf_ino.f_access & DIA_FACUW) ? S_IWGRP:0);
//		linoptr->f_mode |= ((uf_ino.f_access & DIA_FACUE) ? S_IXGRP:0);
		linoptr->f_mode |= ((uf_ino.f_access & DIA_FACOW) ? S_IWOTH:0);
		linoptr->f_mode |= ((uf_ino.f_access & DIA_FACOE) ? S_IXOTH:0);
		linoptr->f_mode |= ((uf_ino.f_access & DIA_FXSET) ? S_ISUID:0);
	}	
	//
	linoptr->f_dirlc 	= uf_ino.f_dirlc;
	linoptr->f_ouid		= fs_uf2ll2((uint8_t*) & uf_ino.f_ouid_h);
	linoptr->f_fsize	= fs_uf4ll4((uint8_t*) &uf_ino.f_fsize_hh);
	for (l = 0; l < MAPSIZ; l++)
	{	
		linoptr->f_ffmap[l] = fs_uf3ll4((uint8_t*) &uf_ino.f_ffmap[l].blkad_h);
	}
	linoptr->f_sutime = fs_uf4ll4((uint8_t*)&uf_ino.f_sutime_hh) + EPOCHOFFSET;
	//
	linoptr->l_ino = d_ino; // update ino info
	
	return 0;
}

//
// take a UniFLEX file node in Linux format, convert it back to UniFLEX format
// and write it to the disk image
//
// flag: 1 = delete fdn, 0 = update fdn
static int32_t fs_writefdn(LINODE* linoptr, int32_t flag)
{
	DINODE 	fdnbuf[8];					// temp for 8 disk inodes in buffer
	DINODE uf_ino = {0};
	uint16_t i = (linoptr->l_ino + 15) % 8;
	uint16_t b = (linoptr->l_ino + 15) / 8;
	uint8_t	 mtemp = 0, mmode;
	int32_t l;

	if (fs_readblock(b, (uint8_t*)&fdnbuf) < 0)
	{
		return -1;
	}
	memcpy((char*)&uf_ino, (char*)(&fdnbuf) + (i * sizeof(DINODE)), sizeof(DINODE));	

	if (flag)	// discard fdn
	{
		memset((char*)&uf_ino, 0, sizeof(DINODE));	
		linoptr->f_mode = 0;
		linoptr->f_dirlc = 0;
		linoptr->f_fsize = 0;
	   	for (l = 0; l < MAPSIZ; l++)
   		{   
       		linoptr->f_ffmap[l] = 0;
   		}
	}

	if(linoptr->f_mode & S_IFREG)
	{
		mtemp |= DIM_FBUSY;
	}
	else if (S_ISCHR(linoptr->f_mode))
	{
		mtemp |= (DIM_FSCHR|DIM_FBUSY);
	}
	else if (S_ISBLK(linoptr->f_mode))
	{
		mtemp |= (DIM_FSBLK|DIM_FBUSY);
	}
	else if (linoptr->f_mode & S_IFDIR)
	{
		mtemp |= (DIM_FSDIR|DIM_FBUSY);
	}
	if (mtemp)  // used?
	{
		uf_ino.f_mode = mtemp;
		mmode = 0;
		mmode |= (linoptr->f_mode & S_IRUSR) ? DIA_FACUR: 0;
		mmode |= (linoptr->f_mode & S_IWUSR) ? DIA_FACUW: 0;
		mmode |= (linoptr->f_mode & S_IXUSR) ? DIA_FACUE: 0;
		mmode |= (linoptr->f_mode & S_IROTH) ? DIA_FACOR: 0;
		mmode |= (linoptr->f_mode & S_IWOTH) ? DIA_FACOW: 0;
		mmode |= (linoptr->f_mode & S_IXOTH) ? DIA_FACOE: 0;
		mmode |= (linoptr->f_mode & S_ISUID) ? DIA_FXSET: 0;
		uf_ino.f_access = mmode;
	}
	uf_ino.f_dirlc = linoptr->f_dirlc;
	fs_ll2uf2((uint8_t*) & uf_ino.f_ouid_h, linoptr->f_ouid);
	fs_ll4uf4((uint8_t*) &uf_ino.f_fsize_hh, linoptr->f_fsize);
   	for (l = 0; l < MAPSIZ; l++)
   	{   
       	fs_ll4uf3((uint8_t*) &uf_ino.f_ffmap[l].blkad_h, linoptr->f_ffmap[l]);
   	}
   	fs_ll4uf4((uint8_t*)&uf_ino.f_sutime_hh, linoptr->f_sutime - EPOCHOFFSET);
	memcpy((char*)(&fdnbuf) + (i * sizeof(DINODE)), (char*)&uf_ino, sizeof(DINODE));	

	if (fs_writeblock(b, (uint8_t*)&fdnbuf) < 0)
	{
		return -1;
	}
	return 0;
}

// 
//  return absolute blockno for relative blockno
//
static int32_t fs_mapfile(LINODE* file, uint32_t relblock)
{
	UDSKAD mybuf[UFINDBLKS+UFINDFILL];
	uint32_t ind1;

	if (relblock < 10)
	{
		return file->f_ffmap[relblock];
	}
	else if (relblock < 138)		// single indirect
	{
		if (file->f_ffmap[10])
		{
			if (fs_readblock(file->f_ffmap[10], (uint8_t*)&mybuf) < 0)
			{
				return -1;
			}
			return (fs_uf3ll4(&mybuf[relblock-10].blkad_h));
		}
	}	
	else if (relblock < 16522)		// double indirect
	{
		if (file->f_ffmap[11])
		{
			if (fs_readblock(file->f_ffmap[11], (uint8_t*)&mybuf) < 0)
			{
				return -1;
			}
			ind1 = fs_uf3ll4((uint8_t*)&mybuf[((relblock - 138) /128)].blkad_h);
			if (ind1)
			{
				if (fs_readblock(ind1, (uint8_t*)&mybuf) < 0)
				{
					return -1;
				}
				return (fs_uf3ll4((uint8_t*)&mybuf[((relblock -138) % 128)].blkad_h));
			}		
		}
	}
	else							// triple indirect
	{
		if (file->f_ffmap[12])
		{
			if (fs_readblock(file->f_ffmap[12], (uint8_t*)&mybuf) < 0)
			{
				return -1;
			}
			ind1 = fs_uf3ll4((uint8_t*)&mybuf[((relblock - 16522) /16384)].blkad_h);
			if (ind1)
			{
				if (fs_readblock(ind1, (uint8_t*)&mybuf) < 0)
				{
					return -1;
				}
				ind1 = fs_uf3ll4((uint8_t*)&mybuf[((relblock - 16522) /128)].blkad_h);
				if (ind1)
				{			
					if (fs_readblock(ind1, (uint8_t*)&mybuf) < 0)
					{
						return -1;
					}
					return (fs_uf3ll4((uint8_t*)&mybuf[((relblock -16522) % 128)].blkad_h));
				}
			}
		}
	} 

	return 0;
}

//
// free all blocks in indirect block(s) (recursive)
// 
static int32_t fs_indirect(int32_t depth, uint32_t blockno)
{
	int32_t index;
	uint32_t myblock;
	UDSKAD mybuf[UFINDBLKS+UFINDFILL];
	
	if (blockno)
	{
		if (fs_readblock(blockno, (uint8_t*)&mybuf) < 0)
		{
			return -1;
		}
		for (index = UFINDBLKS-1 ; index >= 0 ; index--)
		{
			if (((myblock = (fs_uf3ll4((uint8_t*)&mybuf[index].blkad_h))) & 0x00ffffff) != 0)
			{
				switch (depth)
				{
					case 0:				
								if (fs_putblk(myblock) < 0) 	// free blocks
								{
									return -1;
								}
								break;
					case 1:
								fs_indirect(0, myblock);
								break;
					case 2:
								fs_indirect(1, myblock);
								break;
				}
			}
		}
		if (fs_putblk(blockno) < 0)						// free the block itself
		{
			return -1;
		}
	}
	return 0;
}

//
// give back all blocks in file
//
static int32_t	fs_freefile(LINODE* lin)
{
	int32_t index;	
	uint32_t myblock;
	
	if (lin->f_ffmap[12])
	{
		fs_indirect(2, lin->f_ffmap[12]);
	}
	if (lin->f_ffmap[11])
	{
		fs_indirect(1, lin->f_ffmap[11]);
	}
	if (lin->f_ffmap[10])
	{
		fs_indirect(0, lin->f_ffmap[10]);
	}
	for (index = 9; index >=0 ; index--)
	{	
		if ((myblock = lin->f_ffmap[index] & 0x00ffffff) != 0)
		{
			if (fs_putblk(lin->f_ffmap[index]) < 0)
			{
				return -1;
			}
		}	
	}
	return 0;
}

//
// give back relative block to free list
//
static int32_t fs_fileblk2free(LINODE* file, uint32_t  relblock)
{
	UDSKAD mybuf[UFINDBLKS+UFINDFILL];
	uint32_t ind1, ind2;

	// direct
	if (relblock < 10)
	{
		if (fs_putblk(file->f_ffmap[relblock]) < 0)
		{
			return -1;
		}
		file->f_ffmap[relblock] = 0;
	}
	//
	else if (relblock < 138)		// single indirect
	{
		if (file->f_ffmap[10])
		{
			if (fs_readblock(file->f_ffmap[10], (uint8_t*)&mybuf) < 0)
			{
				return -1;
			}
			if (fs_putblk(fs_uf3ll4((uint8_t*)&mybuf[relblock-10])) < 0)
			{
				return -1;
			}
			fs_ll4uf3((uint8_t*)&mybuf[relblock-10].blkad_h, 0);
			if (fs_writeblock(file->f_ffmap[10], (uint8_t*)&mybuf) < 0)		
			{
				return -1;
			}
		}
	}
	//
	else if (relblock < 16522)
	{
		if (file->f_ffmap[11])
		{
			if (fs_readblock(file->f_ffmap[11], (uint8_t*)&mybuf) < 0)		
			{
				return -1;
			}
			ind1 = fs_uf3ll4((uint8_t*)&mybuf[((relblock - 138)/128)].blkad_h);
			if (fs_readblock(ind1, (uint8_t*)&mybuf) < 0)
			{
				return -1;
			}
			if (fs_putblk(fs_uf3ll4((uint8_t*)&mybuf[((relblock - 138) % 128)])) < 0)
			{
				return -1;
			}
			fs_ll4uf3((uint8_t*)&mybuf[((relblock -138) % 128)].blkad_h, 0);
			if (fs_writeblock(ind1, (uint8_t*)&mybuf) < 0)
			{
				return -1;
			}
		}
	}
	//
	else 
	{
		if (file->f_ffmap[12])
		{
			if (fs_readblock(file->f_ffmap[12], (uint8_t*)&mybuf) < 0)
			{
				return -1;
			}
			ind1 = fs_uf3ll4((uint8_t*)&mybuf[((relblock - 16522) /16384)].blkad_h);
			if (fs_readblock(ind1, (uint8_t*)&mybuf) < 0)
			{
				return -1;
			}
			ind2 = fs_uf3ll4((uint8_t*)&mybuf[((relblock - 16522) /128)].blkad_h);
			if (fs_readblock(ind2, (uint8_t*)&mybuf) < 0)
			{
				return -1;
			}
			if (fs_putblk(fs_uf3ll4((uint8_t*)&mybuf[((relblock -16522) % 128)].blkad_h)) < 0)
			{
				return -1;
			}
			fs_ll4uf3((uint8_t*)&mybuf[((relblock -16522) % 128)].blkad_h , 0);
			if (fs_writeblock(ind2, (uint8_t*)&mybuf) < 0)
			{
				return -1;
			}
		}
	}	
	return 0;
}

//
// allocate absblock to fdn[relblock]
//
static int32_t fs_blk2file(LINODE* file, uint32_t relblock, uint32_t absblock)
{
	UDSKAD mybuf[UFINDBLKS+UFINDFILL];
	int32_t ind1, ind2;
	int32_t tmpblock =0;

	// direct
	if (relblock < 10)
	{
		file->f_ffmap[relblock] = absblock;
	}
	//
	else if (relblock < 138)		// single indirect
	{
		if (file->f_ffmap[10] == 0)
		{
			if ((tmpblock = fs_getblk()) < 0)
			{
				fs_errno = ENOSPC;
				return -1;
			}
			else
			{
				file->f_ffmap[10] = tmpblock;
			}
		}
		if (fs_readblock(file->f_ffmap[10], (uint8_t*)&mybuf) < 0)
		{
			return -1;
		}
		fs_ll4uf3((uint8_t*)&mybuf[relblock-10].blkad_h,  absblock);
		//
		if (fs_writeblock(file->f_ffmap[10], (uint8_t*)&mybuf) < 0)
		{
			return -1;
		}
	}
	//
	else if (relblock < 16522)		// double indirect
	{
		// first level
		if (file->f_ffmap[11] == 0)
		{
			if ((tmpblock = fs_getblk()) < 0)
			{
				fs_errno = ENOSPC;
				return -1;
			}
			else
			{	
				file->f_ffmap[11] = tmpblock;
			}
		}
		if (fs_readblock(file->f_ffmap[11], (uint8_t*)&mybuf) < 0)
		{
			return -1;
		}
		ind1 = fs_uf3ll4((uint8_t*)&mybuf[((relblock - 138) /128)].blkad_h);
		// second level
		if (ind1 == 0)
		{
			if ((ind1 = fs_getblk()) < 0)
			{
				fs_errno = ENOSPC;
				return -1;
			}
			fs_ll4uf3((uint8_t*)&mybuf[((relblock - 138) /128)].blkad_h, ind1);
		}
		if (fs_writeblock(file->f_ffmap[11], (uint8_t*)&mybuf) < 0)
		{
			return -1;
		}
		//	
		if (fs_readblock(ind1, (uint8_t*)&mybuf) < 0)
		{
			return -1;
		}
		fs_ll4uf3((uint8_t*)&mybuf[((relblock -138) % 128)].blkad_h, absblock);
		if (fs_writeblock(ind1, (uint8_t*)&mybuf) < 0)
		{
			return -1;
		}
	}
	//
	else							// triple indirect
	{
		// first level
		if (file->f_ffmap[12] == 0)			// first level
		{
			if ((tmpblock = fs_getblk()) < 0)
			{
				fs_errno = ENOSPC;
				return -1;
			}
			else
			{
				file->f_ffmap[12] = tmpblock;
			} 
		}
		if (fs_readblock(file->f_ffmap[12], (uint8_t*)&mybuf) < 0)	
		{
			return -1;
		}
		// second level
		ind1 = fs_uf3ll4((uint8_t*)&mybuf[((relblock - 16522) /16384)].blkad_h);
		if (ind1 == 0)		// second level
		{
			if ((ind1 = fs_getblk()) < 0)
			{
				fs_errno = ENOSPC;
				return -1;
			}
			fs_ll4uf3((uint8_t*)&mybuf[((relblock - 16522) /16384)].blkad_h, ind1);
		}
		if (fs_writeblock(file->f_ffmap[12], (uint8_t*)&mybuf) < 0)
		{
			return -1;
		}
		//
		if (fs_readblock(ind1, (uint8_t*)&mybuf) < 0)
		{
			return -1;
		}
		ind2 = fs_uf3ll4((uint8_t*)&mybuf[((relblock - 16522) /128)].blkad_h);
		// third level
		if (ind2 == 0)
		{
			if ((ind2 = fs_getblk()) < 0)
			{
				fs_errno = ENOSPC;
				return -1;
			}
			fs_ll4uf3((uint8_t*)&mybuf[((relblock - 16522) /128)].blkad_h, ind2);
		}
		if (fs_writeblock(ind1, (uint8_t*)&mybuf) < 0)
		{
			return -1;
		}
		//
		if (fs_readblock(ind2, (uint8_t*)&mybuf) < 0)
		{
			return -1;
		}
		fs_ll4uf3((uint8_t*)&mybuf[((relblock -16522) % 128)].blkad_h , absblock);
		if (fs_writeblock(ind2, (uint8_t*)&mybuf) < 0)
		{
			return -1;
		}
	}
	return 0;
}

//
// truncate file
//
static int32_t fs_truncate(LINODE* linoptr, size_t size)
{
	uint32_t relblock = size / UFBLKSIZ;
	uint32_t lstblock = linoptr->f_fsize / UFBLKSIZ;
	uint32_t i;


	for (i = lstblock; i > relblock; i--)
	{
		fs_fileblk2free(linoptr, i);
	}
	//
	if (relblock < 10)
	{
		if (fs_putblk(linoptr->f_ffmap[12]) < 0)
		{
			return -1;
		}
		linoptr->f_ffmap[12] = 0;
		if (fs_putblk(linoptr->f_ffmap[11]) < 0)
		{
			return -1;
		}
		linoptr->f_ffmap[11] = 0;
		if (fs_putblk(linoptr->f_ffmap[10]) < 0)
		{
			return -1;
		}
		linoptr->f_ffmap[10] = 0;
		for (i = relblock; i < 10; i++)
		{
			if (fs_putblk(linoptr->f_ffmap[i]) < 0)
			{
				return -1;
			}
			linoptr->f_ffmap[i] = 0;
		}
	}
	else if (relblock < 138)
	{
		if (fs_putblk(linoptr->f_ffmap[12]) < 0)
		{
			return -1;
		}
		linoptr->f_ffmap[12] = 0;
		if (fs_putblk(linoptr->f_ffmap[11]) < 0)
		{
			return -1;
		}
		linoptr->f_ffmap[11] = 0;
	}
	else if (relblock < 16522)
	{
		if (fs_putblk(linoptr->f_ffmap[12]) < 0)
		{
			return -1;
		}
		linoptr->f_ffmap[12] = 0;
	}	
	linoptr->f_fsize = size;

	return 0;
}

//
// read file
//
static int32_t fs_readfile(uint64_t fh, char* buf, size_t size, off_t offset)
{
	LINODE fileino;
	char* myptr = buf;
	int32_t readsize = 0, filesize;
	int32_t fileblock = offset / UFBLKSIZ, absblock;
	uint16_t i, buf_offset = offset % UFBLKSIZ;

	memset(&fileino, 0, sizeof(LINODE));

	if (fs_readfdn((uint16_t) fh, &fileino) < 0)
	{
		return -1;
	}

	filesize = fileino.f_fsize;
	if(offset > filesize)
	{
		return 0;
	}

	do
	{
		absblock = fs_mapfile(&fileino, fileblock);
		if (absblock < 0)
		{
			return -1;
		}
		else if (absblock == 0)
		{
			return readsize;
		}
		else
		{
			if (fs_readblock(absblock, (uint8_t*)&filebuf) < 0)
			{
				return -1;
			}
		}
		for (i = buf_offset ; i < UFBLKSIZ; i++)
		{
			*myptr++ = *((char*)&filebuf+buf_offset+i);
			readsize++;
			size--;
			if (readsize >= filesize)
			{
				return readsize;
			}
		}
		buf_offset = 0;
		fileblock++;
	} while (size);	
	return readsize;
}

//
// write file
//
static int32_t fs_writefile(uint64_t fh, const char* buf, size_t size, off_t offset)
{
	LINODE fileino;
	char* myptr = (char* )buf;
	int32_t absblock = 0;
	int32_t relblock = offset / UFBLKSIZ;
	uint16_t i, buf_offset = offset % UFBLKSIZ;
	int32_t writtenbytes = 0;

	memset(&fileino, 0, sizeof(LINODE));

	// get file fdn
	if (fs_readfdn((uint16_t) fh, &fileino) < 0)
	{
		return -1;
	}

	do
	{
		absblock = fs_mapfile(&fileino, relblock);
		if (absblock < 0)
		{
			return -1;
		}
		else if (absblock == 0)
		{	// allocate one
			if ((absblock = fs_getblk()) < 0)			// get block from free chain
			{
				fs_errno = ENOSPC;
				return -1;
			}
			if (fs_blk2file(&fileino, relblock, absblock) < 0)  // update file fdn
			{
				fs_errno = ENOSPC;
				return -1;
			}
		}
		if (fs_readblock(absblock, (uint8_t*)&filebuf) < 0)
		{
			return -1;
		}
		for (i = buf_offset; i < UFBLKSIZ; i++)
		{
			*((char*)&filebuf+buf_offset+i) = *myptr++;
			writtenbytes++;
			size--;
			if (size <= 0)
			{
				break;
			}
		}
		if (fs_writeblock(absblock, (uint8_t*)&filebuf) < 0)
		{
			return -1;
		}
		if (size <= 0)
		{
			break;
		}
		buf_offset = 0;
		relblock++;
	} while (size);	
	fileino.f_fsize += writtenbytes;
	fileino.f_sutime = (uint32_t) time(NULL);
	if (fs_writefdn(&fileino, 0) < 0)	
	{
		return -1;
	}
	return writtenbytes;
}

//----------------------------------------------------------------------------------------
//
// read UniFLEX SIR and convert data into Linux format
//
static int32_t fs_readsir(void)
{
	UFSIR  ufsir  = {0};			// UniFLEX SIR
	int32_t i;

	if (fs_readblock(1, (uint8_t*)&ufsir) < 0)
	{
		return -1;
	}

	lhdsir.ls_supdt  = ufsir.us_supdt;
	lhdsir.ls_swprot = ufsir.us_swprot;
	lhdsir.ls_slkfr  = ufsir.us_slkfr;
	lhdsir.ls_slkfdn = ufsir.us_slkfdn;
	lhdsir.ls_sintid = fs_uf4ll4(&ufsir.us_sintid_hh);
	lhdsir.ls_scrtim = fs_uf4ll4(&ufsir.us_scrtim_hh) + EPOCHOFFSET;
	lhdsir.ls_sutime = fs_uf4ll4(&ufsir.us_sutime_hh) + EPOCHOFFSET;
 	lhdsir.ls_sszfdn = fs_uf2ll2(&ufsir.us_sszfdn_h);
	lhdsir.ls_ssizfr = fs_uf3ll4(&ufsir.us_ssizfr.blkad_h);
	lhdsir.ls_sfreec = fs_uf3ll4(&ufsir.us_sfreec.blkad_h);
	lhdsir.ls_sfdnc  = fs_uf2ll2(&ufsir.us_sfdnc_h );
	lhdsir.ls_sdenf = ufsir.us_sdenf;
	lhdsir.ls_ssidf  = ufsir.us_ssidf;
	strncpy((char*)&lhdsir.ls_sfname, (char*)&ufsir.us_sfname, 14);
	strncpy((char*)&lhdsir.ls_spname, (char*)&ufsir.us_spname, 14);
	lhdsir.ls_sswpbg = fs_uf3ll4(&ufsir.us_sswpbg.blkad_h );
	lhdsir.ls_sswpsz = (ufsir.us_sswpsz_h << 8) + ufsir.us_sswpsz_l;
	lhdsir.ls_snfdn  = ufsir.us_snfdn;
	for(i = 0; i < CFDN; i++)
	{
		lhdsir.ls_cfdn[i] = fs_uf2ll2(&ufsir.us_scfdn[i].fdn_h);
	}
	lhdsir.ls_snfree = ufsir.us_snfree;
	for (i = 0; i < CDBLKS; i++)
	{
		lhdsir.ls_sfree[i] = fs_uf3ll4(&ufsir.us_sfree[i].blkad_h);
	}
	return 0;
}

//
// put Linux SIR info back in UniFLEX format and write it to image
//
static int32_t fs_writesir(void)
{
	UFSIR  ufsir  = {0};			// UniFLEX SIR
	int32_t i;

	ufsir.us_supdt 	= lhdsir.ls_supdt;
	ufsir.us_swprot = lhdsir.ls_swprot;
	ufsir.us_slkfr  = lhdsir.ls_slkfr;
	ufsir.us_slkfdn = lhdsir.ls_slkfdn;
	fs_ll4uf4(&ufsir.us_sintid_hh, ufsir.us_sintid_hh);	
	fs_ll4uf4(&ufsir.us_scrtim_hh, lhdsir.ls_scrtim - EPOCHOFFSET);
	fs_ll4uf4(&ufsir.us_sutime_hh, lhdsir.ls_sutime - EPOCHOFFSET);
	fs_ll2uf2(&ufsir.us_sszfdn_h, lhdsir.ls_sszfdn);
	fs_ll4uf3(&ufsir.us_ssizfr.blkad_h, lhdsir.ls_ssizfr);
	fs_ll4uf3(&ufsir.us_sfreec.blkad_h, lhdsir.ls_sfreec);
	fs_ll2uf2(&ufsir.us_sfdnc_h, lhdsir.ls_sfdnc);
	// names
	strncpy((char*)&ufsir.us_sfname, (char*)&lhdsir.ls_sfname, 14);
	strncpy((char*)&ufsir.us_spname, (char*)&lhdsir.ls_spname, 14);
	// swap info
	fs_ll4uf3(&ufsir.us_sswpbg.blkad_h, lhdsir.ls_sswpbg);	
	fs_ll2uf2(&ufsir.us_sswpsz_h, lhdsir.ls_sswpsz);
	//
	ufsir.us_sdenf = lhdsir.ls_sdenf;
	ufsir.us_ssidf = lhdsir.ls_ssidf;
	//
	ufsir.us_snfdn = lhdsir.ls_snfdn;
    for(i = 0; i < CFDN; i++)
    {
        fs_ll2uf2(&ufsir.us_scfdn[i].fdn_h, lhdsir.ls_cfdn[i]);
    }
	ufsir.us_snfree = lhdsir.ls_snfree;
    for (i = 0; i < CDBLKS; i++)
    {
        fs_ll4uf3(&ufsir.us_sfree[i].blkad_h, lhdsir.ls_sfree[i]);
    }
	if (fs_writeblock(1, (uint8_t*)&ufsir) < 0)
	{
		return -1;
	}

    return 0;
}

//
// retrieve file information to supply 'stat'
//
static void fs_ino2stat(LINODE* ino, struct stat* rstat)
{
	uint16_t dev = (uint16_t)(ino->f_ffmap[0] >> 8);

	rstat->st_mode = ino->f_mode;
	if((rstat->st_mode & S_IFCHR) || (rstat->st_mode & S_IFBLK))
	{
		rstat->st_rdev = dev;
	}

	rstat->st_nlink = (nlink_t) ino->f_dirlc;
	rstat->st_size =  ino->f_fsize;

	rstat->st_uid = ino->f_ouid;
	rstat->st_gid = ino->f_ouid;

	rstat->st_mtim.tv_sec = ino->f_sutime;
	rstat->st_atim.tv_sec = ino->f_sutime;
	rstat->st_ctim.tv_sec = ino->f_sutime;
	rstat->st_blksize = UFBLKSIZ;
	rstat->st_ino = ino->l_ino;
}

//
int32_t fs_get_step(char *out, const char **cur, char sep, size_t max)
{
	int rc = 0;
	if(**cur != '\0')
	{
		*out++ = **cur;
		++*cur;
		--max;

		while((max != 0) && **cur != '\0' && **cur != sep)
		{
			*out++ = **cur;
			++*cur;
			--max;
		}
		*out = '\0';
		rc = 1;
	}
	return rc;
}

//---------------------------------------------------------------------
//
// search path until file is found, return the inode info
//
static int32_t fs_searchpath(const char* path, LINODE* lin)
{
	UDIRENT	dirbuf[32];					// temp for name scanning
	int32_t index = 0;
	uint8_t mapidx = 0;
	uint16_t entries;
	int32_t i;
	char step[NAMELENGTH+2] = {0};
	const char *curstep = path;

	if (fs_readfdn(ROOTFDN, lin) < 0)	
	{
		return -1;
	}
	ucrdir = ROOTFDN;

	index = fs_mapfile(lin, mapidx);
	if (index < 0)
	{
		return -1;
	}
	else if (index == 0)
	{
		fs_errno = ENOENT;
		return -1;
	}
	else
	{
		if (fs_readblock(index, (uint8_t*)&dirbuf) < 0)
		{
			return -1;
		}
	}

	if (strcmp(path, "/") == 0) 
	{
		ucrdir = lin->l_ino;
		return 0;
	}

	while(fs_get_step(step, &curstep, '/', PATH_DEPTH))
	{
		mapidx = 0;
		index = fs_mapfile(lin, mapidx);
		if (index < 0)
		{
			return -1;
		}
		else if (index == 0)
		{
			fs_errno = ENOENT;
			return -1;
		}
		else
		{
			if (fs_readblock(index, (uint8_t*)&dirbuf) < 0)
			{
				return -1;
			}
		}
		entries = lin->f_fsize/sizeof(UDIRENT);
		do
		{
			for(i = 0; i < 32; i++)
			{
				if ((dirbuf[i].dino_h != 0) || (dirbuf[i].dino_l != 0))
				{
					if(strncmp(dirbuf[i].fname, step+1, 14) == 0)
					{
						if (fs_readfdn(fs_uf2ll2((uint8_t*)&dirbuf[i].dino_h), lin)	< 0)
						{
							return -1;
						}
						if (lin->f_mode & S_IFDIR)
						{
							ucrdir = lin->l_ino;
						}
						goto NEXTLEVEL;	
					}
				}
				entries--;
				if (entries == 0)
				{
					break;
				}
			}
			if (entries == 0)
			{
				break;
			}
			mapidx++;
			index = fs_mapfile(lin, mapidx);
			if (index < 0)
			{
				return -1;
			}
			else if (index == 0)
			{
				fs_errno = ENOENT;
				return -1;
			}
			else
			{
				if (fs_readblock(index, (uint8_t*)&dirbuf) < 0)
				{
					return -1;
				}
			}
		} while (entries);
		fs_errno = ENOENT;
		return-1;
// found it
NEXTLEVEL: ;
	}		
	return 0;
}

//
//
//
static int32_t fs_opendir(const char *path, LINODE* lin)
{
	UDIRENT	dirbuf[32];					// temp for name scanning
	int32_t index = 0;
	uint8_t mapidx = 0;
	uint16_t entries;
	int32_t i;
	char step[NAMELENGTH+2] = {0};
	const char *curstep = path;

	if (fs_readfdn(ROOTFDN, lin) < 0)
	{
		return -1;
	}
	if ((index = fs_mapfile(lin, mapidx)) == 0)
	{
		return EINVAL;
	}
	else
	{
		if (fs_readblock(index, (uint8_t*)&dirbuf) < 0)
		{
			return -1;
		}
	}

	if (strcmp(path, "/") == 0) 
	{
		return 0;
	}

	while(fs_get_step(step, &curstep, '/', PATH_DEPTH))
	{
		mapidx = 0;

		index = fs_mapfile(lin, mapidx);
		if (index < 0)
		{
			return -1;
		}
		else if (index == 0)
		{
			return EINVAL;
		}
		else
		{
			if (fs_readblock(index, (uint8_t*)&dirbuf) < 0)
			{
				return -1;
			}
		}
		entries = lin->f_fsize/sizeof(UDIRENT);
		do
		{
			for(i = 0; i < 32; i++)
			{
				if ((dirbuf[i].dino_h != 0) || (dirbuf[i].dino_l != 0))
				{
					if(strncmp(dirbuf[i].fname, step+1, 14) == 0)
					{
						if (fs_readfdn(fs_uf2ll2((uint8_t*)&dirbuf[i].dino_h), lin) < 0)	
						{
							return -1;
						}
						if (lin->f_mode & S_IFDIR)
						{
							ucrdir = lin->l_ino;
						}
						goto OPENDIR;
					}
				}
				entries--;
				if (entries == 0)
				{
					break;
				}
			}
			if (entries == 0)
			{
				break;
			}
			mapidx++;
			index = fs_mapfile(lin, mapidx);
			if (index < 0)
			{
				return -1;
			}
			else if (index == 0)
			{
				return EINVAL;
			}
			else
			{
				if (fs_readblock(index, (uint8_t*)&dirbuf) < 0)
				{
					return -1;
				}
			}
		} while (entries);
		return EINVAL;

OPENDIR: ;
	}
	return 0;
}

//
// basic free blocks handler, return new free block (erased)
//
static int32_t fs_getblk(void)
{
	uint8_t mybuf[UFBLKSIZ];
	int32_t myblock = 0, i;	

	fs_errno = ENOSPC;
	// only at zero when disk is full
	if (lhdsir.ls_snfree == 0)
	{
		return -1;
	}
	lhdsir.ls_snfree--;
	// end of list
	if ((myblock = lhdsir.ls_sfree[lhdsir.ls_snfree]) == 0)
	{
		fs_writesir();
		return -1;
	}
	if (lhdsir.ls_snfree == 0)
	{
		// fill free list
		if (fs_readblock(myblock,(uint8_t*)&mybuf) < 0)
		{
			return -1;
		}
		// update Linux SIR
		lhdsir.ls_snfree = CDBLKS;
    	for (i = 0; i < CDBLKS; i++)
    	{
        	lhdsir.ls_sfree[i] = fs_uf3ll4(&mybuf[i * sizeof(UDSKAD)]);
    	}
	}
	lhdsir.ls_sfreec--;
	// update UF sir
	if (fs_writesir() < 0)
	{
		return -1;
	}
	fs_errno = 0;
	if (fs_writeblock(myblock, (uint8_t*)&zerobuf) < 0)	// guarantee that block is empty
	{
		return -1;
	}
	return myblock;
}

//
// return block back to free list
//
static int32_t fs_putblk(int32_t block)
{
	uint8_t mybuf[UFBLKSIZ];
	int32_t i;

	if (block != 0)
	{
		if (lhdsir.ls_snfree == 0)
		{
			lhdsir.ls_sfree[lhdsir.ls_snfree] = 0;
			lhdsir.ls_snfree++;
		}
		else if (lhdsir.ls_snfree == CDBLKS)
		{
			// update UF sir
			memset((void*)&mybuf, 0, UFBLKSIZ);
		    for (i = 0; i < CDBLKS; i++)
  		 	{
   		    	fs_ll4uf3(&mybuf[i * sizeof(UDSKAD)], lhdsir.ls_sfree[i]);
   		 	}
			if (fs_writeblock(block, (uint8_t*)&mybuf) < 0)
			{
				return -1;
			}
			lhdsir.ls_snfree = 0;
		}
		lhdsir.ls_sfree[lhdsir.ls_snfree] = block;
		lhdsir.ls_snfree++;
		lhdsir.ls_sfreec++;
		// update UF sir
		if (fs_writesir() < 0)
		{
			return -1;
		}
	}
	return 0;
}

//
// scan for SIR to allocate a fresh file node from list
//
static int32_t fs_allfdn(LINODE* lin)
{
	int32_t i;

	if (lhdsir.ls_snfdn == 0)
	{
		// scan fdn's for free ones, skip root + .badblocks
		for (i = 2; i < lhdsir.ls_sszfdn * 8; i++)
		{
			if (fs_readfdn( i, lin) < 0)
			{
				return -1;
			}
			if (lin->f_mode == 0)
			{
				lhdsir.ls_cfdn[lhdsir.ls_snfdn] = i;
				lhdsir.ls_snfdn++;
				if (lhdsir.ls_snfdn == CFDN)
				{
					break;
				}
			}
		}
	}
	if (lhdsir.ls_snfdn)
	{
		memset(lin, 0, sizeof(LINODE));
        lhdsir.ls_snfdn--;
        lin->l_ino =  lhdsir.ls_cfdn[lhdsir.ls_snfdn];
        lhdsir.ls_sfdnc--;
		if (fs_writesir() < 0)
		{
			return -1;
		}
        return 0;
    }
	fs_errno = ENOSPC;
	return -1;
}

//
// put back file inode to SIR list of free nodes
//
static int32_t fs_putfdn(uint16_t lin)
{
	
	lhdsir.ls_sfdnc++;
	if (lhdsir.ls_snfdn != CFDN)
	{
		lhdsir.ls_cfdn[lhdsir.ls_snfdn] = lin;
		lhdsir.ls_snfdn++;
	}
	if (fs_writesir() < 0)
	{
		return -1;
	}
	return 0;
}

// 
// write name into directory file at an unused place
//
// flag: 0 = file, 1 = dir
//
static int32_t fs_cpname2dir(LINODE* dest, uint16_t target_lino, char * name, int32_t flag)
{
	UDIRENT	dirbuf[32];					// temp for name scanning
	int32_t mapidx = 0, dirent = 0;
	int32_t used = 0;

	// we know all the details of the directory
	for (mapidx = 0; mapidx < 10 ; mapidx++) // impose a maximum of 640 entries!
	{
		if (dest->f_ffmap[mapidx] != 0)
		{
			if (fs_readblock(dest->f_ffmap[mapidx], (uint8_t*)&dirbuf) < 0)
			{
				return -1;
			}
			for (dirent = 0; dirent < MAXDIRENT; dirent++)
			{
				if ((dirbuf[dirent].dino_h + dirbuf[dirent].dino_l) == 0)
				{
					dirbuf[dirent].dino_h = target_lino >> 8;
					dirbuf[dirent].dino_l = target_lino & 0xff;
					strncpy((char*)&dirbuf[dirent].fname, name, NAMELENGTH);
					if (fs_writeblock(dest->f_ffmap[mapidx], (uint8_t*)&dirbuf) < 0)
					{
						return -1;
					}
					// size does not shrink when entries are deleted
					if (dest->f_fsize <= (used * (NAMELENGTH + 2)))
					{
						dest->f_fsize += (NAMELENGTH + 2);		// increase directory size
					}
					if (flag)
					{
						dest->f_dirlc++;
					}
					if (fs_writefdn(dest, 0) < 0)
					{
						return -1;
					}
					goto FOUNDSLOT;
				}
				else
				{
					used++;
				}
			}
		}
		else
		{ 
			// expand directory
			dirent = 0;	
			if ((dest->f_ffmap[mapidx] = fs_getblk()) < 0)
			{
				fs_errno = ENOSPC;
				return -1;
			}
			if (fs_readblock(dest->f_ffmap[mapidx], (uint8_t*)&dirbuf) < 0)
			{
				return -1;
			}
			dirbuf[dirent].dino_h = target_lino >> 8;
			dirbuf[dirent].dino_l = target_lino & 0xff;
			strncpy((char*)&dirbuf[dirent].fname, name, NAMELENGTH);
			if (fs_writeblock(dest->f_ffmap[mapidx], (uint8_t*)&dirbuf) < 0)
			{
				return -1;
			}
			dest->f_fsize += (NAMELENGTH + 2);		// increase directory size
			if (flag)
			{
				dest->f_dirlc++;
			}
			if (fs_writefdn(dest, 0) < 0)
			{
				return -1;
			}
			goto FOUNDSLOT;
		}
	}
	// need to expand directory
	fs_errno = EPERM;
	return -1;

FOUNDSLOT:
	return 0;	
}

//
// remove directory reference 
//
static int32_t fs_rmlink(LINODE* dest, LINODE* target)
{
	UDIRENT	dirbuf[32];					// temp for name scanning
	int32_t mapidx = 0, dirent;

	for (mapidx = 0 ; mapidx < 10; mapidx++)
	{
		if (dest->f_ffmap[mapidx] != 0)
		{
			if (fs_readblock(dest->f_ffmap[mapidx], (uint8_t*)&dirbuf) < 0)
			{
				return -1;
			}
			for (dirent = 0; dirent < MAXDIRENT; dirent++)
			{
				if ((dirbuf[dirent].dino_h + dirbuf[dirent].dino_l) != 0)
				{
					if ((dirbuf[dirent].dino_h == target->l_ino >> 8) 
					 && (dirbuf[dirent].dino_l == (target->l_ino & 0xff)))
					{
						// erase entry in parent dir
						memset((char*)&dirbuf[dirent].dino_h, 0, NAMELENGTH + 2);
						if (fs_writeblock(dest->f_ffmap[mapidx], (uint8_t*)&dirbuf) < 0)
						{
							return -1;
						}
						if (fs_writefdn(dest, 0) < 0)
						{
							return -1;
						}
						return 0;
					}
				}
			}
		}
	}
	return -1;
}

//
// create new directory
//
static int32_t fs_mkdir(const char* path, mode_t mode)
{
	UDIRENT	dirbuf[32];					// temp for name scanning
	LINODE mydp , pardir ;
	char mydirname[NAMELENGTH + 1];
	char *parpath = NULL;
	int32_t myblock;
	int32_t res;
	
	memset(&mydp, 0, sizeof(LINODE));
	memset(&pardir, 0, sizeof(LINODE));

	strncpy((char*)&mydirname, basename((char*) path), NAMELENGTH);

	parpath = dirname((char*) path);
	res = fs_searchpath(parpath, &pardir);

	if (res)
	{
		return res;
	}
	if (fs_allfdn(&mydp) != 0)	// get free fdn	
	{
		fs_errno = ENOSPC;
		return -1;
	}
    if (fs_cpname2dir(&pardir, mydp.l_ino, mydirname, 1) < 0)       // and update fdn
	{
		return -1;
	}
	if ((myblock = fs_getblk()) < 0)
	{
		fs_errno = ENOSPC;
		return -1;
	}
	if (fs_blk2file(&mydp, 0, myblock) < 0)
	{
		fs_errno = ENOSPC;
		return -1;
	}
	if (fs_readblock(myblock, (uint8_t*)&dirbuf) < 0)
	{
		return -1;
	}
	memcpy((char*)&dirbuf, (char*)&newdir, sizeof(newdir));
	dirbuf[0].dino_h = mydp.l_ino >> 8;
	dirbuf[0].dino_l = mydp.l_ino & 0xff;
	dirbuf[1].dino_h = pardir.l_ino >> 8;
	dirbuf[1].dino_l = pardir.l_ino & 0xff;
	//
	mydp.f_mode = mode | S_IFDIR;
	mydp.f_fsize = sizeof(newdir);
	mydp.f_dirlc = 2;
	mydp.f_sutime = (uint32_t) time(NULL);

	if (fs_writefdn(&mydp, 0) < 0)
	{
		return -1;
	}
	if (fs_writeblock(myblock, (uint8_t*)&dirbuf) < 0)	
	{
		return -1;
	}
	return 0;		
}

//
// remove directory
//
static int32_t fs_rmdir(const char* path)
{
	UDIRENT	dirbuf[32];					// temp for name scanning
	LINODE mydp, pardir;
	char *parpath = NULL;
	int32_t used = 0, res;
	uint8_t mapidx, dirent;

	memset(&mydp, 0, sizeof(LINODE));
	memset(&pardir, 0, sizeof(LINODE));

	strncpy((char*)&pathcopy, path, PATH_MAX-1);

	parpath = dirname((char*) pathcopy);
	res = fs_searchpath(parpath, &pardir);

	if (res)
	{
		fs_errno = ENOTDIR;
		return -1;
	}
	if((res = fs_searchpath(path, &mydp)) != 0)
	{
		fs_errno = ENOTDIR;
		return -1;
	}
	if (!fs_isdir(&mydp))
	{
		fs_errno = ENOTDIR;
		return -1;
	}

	for (mapidx = 0; mapidx < 10 ; mapidx++) // impose a maximum of 640 entries!
	{
		if (mydp.f_ffmap[mapidx] != 0)
		{
			if (fs_readblock(mydp.f_ffmap[mapidx], (uint8_t*)&dirbuf) < 0)
			{
				return -1;
			}
			for (dirent = 0; dirent < MAXDIRENT; dirent++)
			{
				if ((dirbuf[dirent].dino_h + dirbuf[dirent].dino_l) != 0)
				{
					used++;
				}
			}
		}
	}
	if (used != 2)
	{
		fs_errno = ENOTEMPTY;
		return -1;
	}
    for (mapidx = 0; mapidx < 10 ; mapidx++) // impose a maximum of 640 entries!
    {   
        if (mydp.f_ffmap[mapidx] != 0)
        {   
			if (fs_putblk(mydp.f_ffmap[mapidx]) < 0)
			{
				return -1;
			}
		}
	}

	// target directory has only . and ..
	if (fs_rmlink(&pardir, &mydp) != 0)
	{
		fs_errno = EINVAL;
		return -1;
	}
	pardir.f_dirlc--;	
	if (fs_writefdn(&pardir, 0) < 0)
	{
		return -1;
	}
	if (fs_writefdn(&mydp, 1) < 0)
	{
		return -1;
	}
	if (fs_putfdn(mydp.l_ino) < 0)
	{
		return -1;
	}
	return 0;
}

//
// create file
//
static int32_t fs_create(const char* path, LINODE* lin, int32_t flags, mode_t mode)
{
	static LINODE pardir;
	char *parpath = NULL;
	char myfilename[NAMELENGTH + 1];
	int32_t res;
	
	memset(&pardir, 0, sizeof(LINODE));

	strncpy((char*)&pathcopy, path, PATH_MAX-1);
	strncpy((char*)&myfilename, basename((char*) pathcopy), NAMELENGTH);

	parpath = dirname((char*) pathcopy);
	// lookup pardir inode 
	res = fs_searchpath(parpath, &pardir);

	if (res)
	{
		if (fs_errno == ENOENT)
		{		
			return res;
		}
	}
	fs_errno = 0;

	if((res = fs_searchpath(path, lin)) == 0)
	{
		if (lin->f_mode & S_IFDIR)
		{
			fs_errno = EISDIR;
			return -1; // invalid path
		}	
		if (!(lin->f_mode & S_IWUSR) && (!(lin->f_mode & S_IWOTH)))
		{
			fs_errno = EACCES;
			return -1;
		}
		fs_truncate(lin, 0);		// remove file blocks
	}
	else
	{
		// reset perm bits
		strncpy((char*)&myfilename, basename((char*) path), NAMELENGTH);
		if ((res = fs_allfdn(lin)) != 0)	// get free fdn
		{
			return res;
		}			
		// make entry in directory file
		if ((res = fs_cpname2dir(&pardir, lin->l_ino, myfilename, 0)) != 0)
		{
			return res;
		}
	}
	lin->f_mode = mode;
	lin->f_dirlc = 1;
	lin->f_fsize = 0;
	lin->f_sutime = (uint32_t)time(NULL);
	if (fs_writefdn(lin, 0) < 0)
	{
		return -1;
	}
	return 0;
}

static int32_t fs_close(LINODE* lin)
{
	
	return fs_writefdn(lin, 0);
}

static int32_t fs_link(const char* from, const char* to)
{
	LINODE myfrom, myto, pardir;
    char *parpath = NULL;
    char myfilename[NAMELENGTH + 1];
    int32_t res;
    
	memset(&myfrom, 0, sizeof(LINODE));
	memset(&myto, 0, sizeof(LINODE));
	memset(&pardir, 0, sizeof(LINODE));

    strncpy((char*)&pathcopy, to, PATH_MAX-1); 
    strncpy((char*)&myfilename, basename((char*) pathcopy), NAMELENGTH);
    
    parpath = dirname((char*) pathcopy);

	if ((fs_searchpath(from, &myfrom)) != 0)
	{
		fs_errno = ENOENT;
		return -1;
	}
	if (fs_isdir(&myfrom))
	{
		fs_errno = EPERM;
		return -1;
	}	
	if ((fs_searchpath(to, &myto)) == 0)
	{
		fs_errno = EEXIST;
		return -1;
	}

    if ((res = fs_searchpath(parpath, &pardir)) != 0)
	{
		fs_errno = EINVAL;
		return -1;
	}
	if ((res = fs_cpname2dir(&pardir, myfrom.l_ino, myfilename, 0)) != 0)
	{
		return res;
	}
	
	if (fs_writefdn(&pardir, 0) < 0)
	{
		return -1;
	}
	return 0;	
}

//
// remove file reference and delete it when it was the last one
//
static int32_t fs_unlink(LINODE* lin)
{
	LINODE mydir;
	int32_t res = EINVAL;
	uint16_t ino = lin->l_ino;

	memset(&mydir, 0, sizeof(LINODE));

	if (fs_readfdn(ucrdir, &mydir) < 0)
	{
		return -1;
	}

	// remove the entry in the parent directory file
	if ((res = fs_rmlink(&mydir, lin)) < 0)
	{
		return res;
	}		

	if (!--lin->f_dirlc)	// last link?
	{
		fs_freefile(lin);
		if (fs_writefdn(lin, 1) < 0)
		{
			return -1;
		}
		if (fs_putfdn(ino) < 0)
		{
			return -1;
		}
	}
	return 0;
}

static int32_t fs_makdev(const char* path, mode_t mode, dev_t devtyp)
{
	LINODE mydevice;
	int32_t res = EINVAL;
	int32_t flags = 0;
	uint32_t temp = 0;
	
	memset(&mydevice, 0, sizeof(LINODE));

	if (!((S_ISCHR(mode)) || (S_ISBLK(mode))))
	{
		return res;
	}

	res = fs_create(path, &mydevice, flags, mode);
	if (res)
	{
		return res;
	}

	temp = ((major(devtyp) & 0xff) << 16) | (minor(devtyp) << 8 );
	mydevice.f_ffmap[0] = temp;
	if (fs_writefdn( &mydevice,0) < 0)
	{
		return -1;
	}

	return 0;
}

//############################################################################################################

static void xmp_init(struct fuse_conn_info *conn)
{
	(void) conn;

	fs_readsir();

}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
	LINODE  lp ;

	memset(&lp, 0, sizeof(LINODE));
	memset(stbuf, 0, sizeof(struct stat));

	if((fs_searchpath(path, &lp)) == 0)
	{
		fs_ino2stat(&lp, stbuf);		
		return 0;
	}
	return -ENOENT;
}

// not implemented because if difference in size for uid
// between Linux (32 bit) and UniFLEX (16 bit)
static int xmp_access(const char *path, int mask)
{
	int res;

	res = access(path, mask);
	if (res == -1)
		return -fs_errno;

	return 0;
}

static int xmp_readlink(const char *path, char *buf, size_t size)
{
	int res;

	res = readlink(path, buf, size - 1);
	if (res == -1)
		return -fs_errno;

	buf[res] = '\0';
	return 0;
}


static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	UDIRENT	dirbuf[32];					// temp for name scanning
	LINODE  curdir;
	LINODE  curfile ;
	int32_t index = 0;

	uint16_t entries = 0;
	uint8_t mapidx = 0;
	int i = 0;	


	(void) offset;
	(void) fi;

	memset(&curdir, 0, sizeof(LINODE));
	memset(&curfile, 0, sizeof(LINODE));

	if ((fs_opendir(path, &curdir)) != 0)
	{
		return -ENOTDIR;
	}
	
	index = fs_mapfile(&curdir, mapidx);
	if (index < 0)
	{
		return -1;
	}
	else if (index == 0)
	{
		return -ENOTDIR;
	}
	else
	{
		if (fs_readblock(index, (uint8_t*)&dirbuf) < 0)
		{
			return -1;
		}
	}
	entries = curdir.f_fsize/sizeof(UDIRENT);

	do
	{
		for(i = 0; i < 32; i++)
		{
			if ((dirbuf[i].dino_h + dirbuf[i].dino_l) != 0)
			{
				struct stat st;
				char nambuf[NAMELENGTH+1] = {0};
				strncpy((char*)&nambuf, (char*)dirbuf[i].fname, NAMELENGTH);
				memset(&st, 0, sizeof(st));
				if (fs_readfdn(fs_uf2ll2((uint8_t*)&dirbuf[i].dino_h), &curfile) < 0)	
				{
					return -1;
				}
				fs_ino2stat(&curfile, &st);
				if ( filler (buf, (char*)&nambuf, &st,  0))
				{
					goto READDIREXIT;
				}
			}
			entries--;
			if (entries == 0)
			{
				break;
			}
		}
		if (entries == 0)
		{
			break;
		}
		mapidx++;
		index = fs_mapfile(&curdir, mapidx);
		if (index < 0)
		{
			return -1;
		}
		else if (index == 0)
		{
			return -ENOTDIR;
		}
		else
		{
			if (fs_readblock(index, (uint8_t*)&dirbuf) < 0)
			{
				return -1;
			}
		}
	} while (entries);
//	return -ENOTDIR;

READDIREXIT: ;
	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
	LINODE mylin;
	int res;

	memset(&mylin, 0, sizeof(LINODE));

	/* On Linux this could just be 'mknod(path, mode, rdev)' but this
	   is more portable */
	if (S_ISREG(mode)) {
		res = fs_create(path, &mylin, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res)
		{
			return -fs_errno;
		}
		else
		{
			res = fs_close(&mylin);
		}
	} 
	else if (S_ISFIFO(mode))
	{
//		res = mkfifo(path, mode);
	}
	else
	{
		res = fs_makdev(path, mode, rdev);
	}
	if (res == -1)
		return -fs_errno;

	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	LINODE  lp;
	int res;

	memset(&lp, 0, sizeof(LINODE));

	if((fs_searchpath(path, &lp)) == 0)
	{
		return -fs_errno;
	}
	
	res = fs_mkdir(path, mode);	
	
	if (res == -1)
		return -fs_errno;

	return 0;
}

static int xmp_unlink(const char *path)
{
	LINODE  lp;
	int res;

	memset(&lp, 0, sizeof(LINODE));

	if((fs_searchpath(path, &lp)) != 0)
	{
		return -fs_errno;
	}

	if (lp.f_mode & S_IFDIR)
	{
		return -EINVAL;
	}

	res = fs_unlink(&lp);

	if (res == -1)
		return -fs_errno;

	return 0;
}

static int xmp_rmdir(const char *path)
{
	int res;

	res = fs_rmdir(path);
	if (res == -1)
		return -fs_errno;

	return 0;
}

static int xmp_symlink(const char *from, const char *to)
{
	int res;

	res = symlink(from, to);
	if (res == -1)
		return -fs_errno;

	return 0;
}

static int xmp_rename(const char *from, const char *to)
{
	int res;

	res = rename(from, to);
	if (res == -1)
		return -fs_errno;

	return 0;
}

static int xmp_link(const char *from, const char *to)
{
	int res;

	res = fs_link(from, to);
	if (res == -1)
		return -fs_errno;

	return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
	LINODE  lp;
	int res;

	memset(&lp, 0, sizeof(LINODE));

	if((fs_searchpath(path, &lp)) != 0)
	{
		return -fs_errno;
	}
	
	lp.f_mode = mode;

	
	res = fs_writefdn(&lp, 0);
	
	if (res == -1)
		return -fs_errno;

	return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
	LINODE  lp;
	int res;

	memset(&lp, 0, sizeof(LINODE));

	if((fs_searchpath(path, &lp)) != 0)
	{
		return -fs_errno;
	}

	lp.f_ouid  = uid;	

	res = fs_writefdn(&lp, 0);

	if (res == -1)
		return -fs_errno;

	return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
	LINODE  lp;
	int res;

	memset(&lp, 0, sizeof(LINODE));

	if((fs_searchpath(path, &lp)) != 0)
	{
		return -EINVAL;
	}
	if (!fs_isfile(&lp))
	{
		return -EINVAL;
	}

	res = fs_truncate(&lp, size);
	if (res == -1)
		return -fs_errno;

	if (fs_writefdn(&lp, 0) < 0)
	{
		return -fs_errno;
	}

	return 0;
}

#ifdef HAVE_UTIMENSAT
static int xmp_utimens(const char *path, const struct timespec ts[2])
{
	LINODE myinode;

	memset(&myinode, 0, sizeof(LINODE));

	if((fs_searchpath(path, &myinode)) != 0)
	{
		return -ENOENT;
	}
	myinode.f_sutime = (uint32_t)time(NULL);
	if (fs_writefdn(&myinode, 0) < 0)
	{
		return -1;
	}

	return 0;
}
#endif

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
	LINODE myinode;

	memset(&myinode, 0, sizeof(LINODE));

	if((fs_searchpath(path, &myinode)) != 0)
	{
		return -ENOENT;
	}
	if (!(fs_isfile(&myinode)))
	{
		return -EINVAL;
	}	

	fi->fh = (uint64_t)myinode.l_ino;
	fi->flags |= 0x1ff;
//	fi->direct_io = 1;
//	fi->flush = 1;
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	int res = -1;

	res = fs_readfile(fi->fh, buf, size, offset);

	return res;
}


static int xmp_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	int res;

	res = fs_writefile(fi->fh, buf, size, offset);

	if (res == -1)
		res = -fs_errno;
	return res;
}


static int xmp_statfs(const char *path, struct statvfs *stbuf)
{
	int res;
	
	res = fs_readsir();
	if (res == -1)
		return -fs_errno;

	stbuf->f_bsize 	= UFBLKSIZ;
	stbuf->f_frsize	= UFBLKSIZ;
	stbuf->f_blocks	= lhdsir.ls_ssizfr;
	stbuf->f_bfree	= lhdsir.ls_sfreec;
	stbuf->f_bavail	= lhdsir.ls_sfreec;
	stbuf->f_files	= lhdsir.ls_sszfdn;
	stbuf->f_ffree	= lhdsir.ls_sfdnc;
	stbuf->f_favail	= lhdsir.ls_sfdnc;
	stbuf->f_fsid	= lhdsir.ls_sfnumb;
//	stbuf->f_flag	= ST_NOEXEC|ST_SYNCHRONOUS|ST_NODEV|ST_NOATIME|ST_RDONLY;
	stbuf->f_flag	= 8|16|4|1024|1;
	stbuf->f_namemax = NAMELENGTH;	

	return 0;
}

static int xmp_release(const char *path, struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */

	(void) path;
	(void) fi;
	return 0;
}

static int xmp_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */

	(void) path;
	(void) isdatasync;
	(void) fi;
	return 0;
}

static struct fuse_operations xmp_oper = {
	.init		= xmp_init,
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.mkdir		= xmp_mkdir,
	.open		= xmp_open,
	.read		= xmp_read,
	.statfs		= xmp_statfs,
	.release	= xmp_release,
	.mknod		= xmp_mknod,
	.unlink		= xmp_unlink,
	.rmdir		= xmp_rmdir,
	.link		= xmp_link,
	.chmod		= xmp_chmod,
	.chown		= xmp_chown,
	.truncate	= xmp_truncate,
#ifdef HAVE_UTIMENSAT
	.utimens	= xmp_utimens,
#endif
	.write		= xmp_write,
//	.rename		= xmp_rename,
//	.symlink	= xmp_symlink,
//	.access		= xmp_access,
//	.readlink	= xmp_readlink,
//	.fsync		= xmp_fsync,
#ifdef HAVE_POSIX_FALLOCATE
//	.fallocate	= xmp_fallocate,
#endif
#ifdef HAVE_SETXATTR
//	.setxattr	= xmp_setxattr,
//	.getxattr	= xmp_getxattr,
//	.listxattr	= xmp_listxattr,
//	.removexattr	= xmp_removexattr,
#endif
};

int main(int argc, char *argv[])
{
	int i;

	if ((fs_fdn = open(argv[1], O_RDWR)) < 0)
	{
		fprintf(stderr, "xxx: can't open filesystem file! \n");
		return(1);
	}
	for (i = 1 ; i < argc; i++)
	{
		argv[i] = argv[i+1];
	}
	argc--;	

	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
