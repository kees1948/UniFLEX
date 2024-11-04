#ifndef __FUSE_UNIFLEX_H__
#define __FUSE_UNIFLEX_H__


#define EPOCHOFFSET 347155200              
//315532800

#define ROOTFDN 1
#define SIRBLK  1

#define PATH_DEPTH 32

#define CFDN        50
#define CDBLKS      100
#define MAPSIZ		13
#define NAMELENGTH	14
#define MAXDIRENT	32				// 512/16

#define UFBLKSIZ	512
#define UFINDBLKS	128				// nr of indirect entries in block
#define UFINDFILL	43				// (128 + 43) * 3 = 513

// status
#define DIS_FLOCK	0B00000001
#define DIS_FMOD	0B00000010
#define DIS_FTEXT	0B00000100
#define DIS_FMNT	0B00001000
#define DIS_FWLCK	0B00010000

// mode
#define DIM_FBUSY	0B00000001
#define DIM_FSBLK	0B00000010
#define DIM_FSCHR	0B00000100
#define DIM_FSDIR	0B00001000
#define DIM_FPRDF	0B00010000
#define DIM_FPWRF	0B00100000
// access
#define DIA_FACUR	0B00000001 // user read
#define DIA_FACUW	0B00000010 
#define DIA_FACUE	0B00000100
#define DIA_FACOR	0B00001000 // other read
#define DIA_FACOW	0B00010000
#define DIA_FACOE   0B00100000
#define DIA_FXSET	0B01000000 // uid exec set


typedef struct {
uint8_t		fdn_h;
uint8_t		fdn_l;
} UFDN;

typedef struct {
uint8_t		blkad_h;
uint8_t		blkad_m;
uint8_t		blkad_l;
} UDSKAD;


// 
// UniFLEX INO
//
typedef struct
{
	uint8_t	f_mode;
	uint8_t	f_access;
	uint8_t	f_dirlc;
	uint8_t	f_ouid_h;
	uint8_t	f_ouid_l;
	uint8_t	f_fsize_hh;
	uint8_t	f_fsize_mh;
	uint8_t	f_fsize_ml;
	uint8_t	f_fsize_ll;
	UDSKAD	f_ffmap[MAPSIZ];
	uint8_t	f_sutime_hh;
	uint8_t	f_sutime_mh;
	uint8_t	f_sutime_ml;
	uint8_t	f_sutime_ll;
	uint8_t f_filler[12];
} DINODE;	

typedef struct {
	struct {
		mode_t	f_mode;
		uint8_t	f_dirlc;
		uint16_t f_ouid;
		uint32_t f_fsize;
		int32_t f_ffmap[MAPSIZ];
		uint32_t f_sutime;
		uint8_t f_filler[12];
	};
	uint16_t l_ino;
	uint16_t l_dirino;
} LINODE;

typedef struct {
	uint8_t dino_h;
	uint8_t	dino_l;
	char	fname[NAMELENGTH];
} UDIRENT;

#define UDIRENTS	UFBLKSIZ/sizeof(UDIRENT)

typedef struct {
	uint16_t	dirents;
	uint16_t  	diridx;
	UDIRENT;
} LDIRENT;


//
// UniFLEX SIR
//
typedef struct {
uint8_t		us_supdt;
uint8_t 	us_swprot;
uint8_t 	us_slkfr;
uint8_t 	us_slkfdn;
uint8_t		us_sintid_hh;
uint8_t		us_sintid_mh;
uint8_t		us_sintid_ml;
uint8_t		us_sintid_ll;
uint8_t		us_scrtim_hh;
uint8_t		us_scrtim_mh;
uint8_t		us_scrtim_ml;
uint8_t		us_scrtim_ll;
uint8_t		us_sutime_hh;
uint8_t		us_sutime_mh;
uint8_t		us_sutime_ml;
uint8_t		us_sutime_ll;
uint8_t		us_sszfdn_h;
uint8_t		us_sszfdn_l;
UDSKAD		us_ssizfr;
UDSKAD		us_sfreec;
uint8_t		us_sfdnc_h;
uint8_t		us_sfdnc_l;
uint8_t		us_sfname[14];
uint8_t		us_spname[14];
uint8_t		us_sfnumb_h;
uint8_t		us_sfnumb_l;
uint8_t		us_sflawc_h;
uint8_t		us_sflawc_l;
uint8_t		us_sdenf;
uint8_t		us_ssidf;
UDSKAD		us_sswpbg;
uint8_t		us_sswpsz_h;
uint8_t		us_sswpsz_l;
uint8_t		us_s64k;
uint8_t		us_swinc[11];
uint8_t		us_sspare[11];
uint8_t		us_snfdn;	
UFDN		us_scfdn[CFDN];
uint8_t		us_snfree;
UDSKAD		us_sfree[CDBLKS];
uint8_t		us_filler[22];
} UFSIR;

// 
// UniFLEX SIR in Linux format
//
typedef struct {
uint8_t		ls_supdt;
uint8_t 	ls_swprot;
uint8_t 	ls_slkfr;
uint8_t 	ls_slkfdn;
uint32_t	ls_sintid;
uint32_t	ls_scrtim;
uint32_t	ls_sutime;
uint16_t	ls_sszfdn; // blocks fdn list
uint32_t	ls_ssizfr; //blocks in volume
uint32_t	ls_sfreec;	// total free blocks
uint16_t	ls_sfdnc;  // free fdn count
char		ls_sfname[14]; // file system name
char		ls_spname[14]; // pack name
uint16_t	ls_sfnumb;	// file system number
uint16_t	ls_sflawc;	// flawed block count
uint8_t		ls_sdenf; // single(0) density
uint8_t		ls_ssidf; // single(0) side
uint32_t	ls_sswpbg; // swap starting blokc
uint16_t	ls_sswpsz; // swap size
uint8_t		ls_s64k;	// swap block multiple of 64k
uint8_t		ls_swinc[11]; // Winchester config
uint8_t     ls_sspare[11]; // spare
uint8_t		ls_snfdn;	// # in core fdns
uint16_t	ls_cfdn[CFDN]; // in core free fdns
uint8_t		ls_snfree; // in core free blocks
uint32_t 	ls_sfree[CDBLKS]; // in core free blocks
} LINSIR;













#endif // __FUSE_UNIFLEX_H__
