/*
** Header-File for SETUP
*/

/*  History
**
** 1.10 Windows =====================================================
** 10 jun 88	peterbe	Added txCompat[] screen.
** 09 jun 88	peterbe	Added [compatibility] section definitions.
**			N_COMPAT, uses data structure type
**			number SW_DFND
**
** 1.03 Windows =====================================================
**   ......
** 20 may 87    plb     created SUINF.H.
** 21 may 87    plb     added 'struct keyboard' and 'struct machine' to
*                       'union dfile'.
** 22 may 87    plb     added SW_DFHD,SW_DFPRINTFON record selector.
** 27 may 87    plb     changed the file pointer list to an array in
**                      dfile.machine
**                      define S_KERNEL, etc., moved BOOL etc. here.
** 29 may 87    plb     Add [harddisk] size defs
**  9 jun 87    plb     Remove [pif] section
** 14 jun 87    plb     Combine SUSCR.H, SUINF.H, SUAPPDAT.H.
**                      Added function and data extern declarations.
**                      Added S_APPS.
** 15 jun 87    plb     Added defs for InsertMenu[], InsertDisk().
**                      Added StoreStandardItem(), ClearStatusLine()
**                      smLoadErr, smWriteErr.
** 16 jun 87    plb     added sOemDisplayLoaded[], etc.
** 20 jun 87    mp      DefaultPath and WindowsPath moved to SUPATH and SUDEST
**                      Moved RecNPtr(int) decl. from external declaration section
** 18 jun 87    mp      change char to CHAR (defined as unsigned char)
**                      include of setup2.h in the middle of setup.h
** 15 jul 87    mp      join setup2.h and setup.h to setup.h, remove #ifdef's
** 18 jul 87    mp      Added S_386 and S_3EX
** 21 jul 87    mp      change or remove entries from SUAPPDAT.C
**  3 aug 87    mp      aliasing for 386 files
** 31 jan 89    wch     removed references to IsTwoDiskette, Is360K and 
**                      IsDiskette which were always FALSE
*/

#define LINT_ARGS

#include <malloc.h>
#include <memory.h>


/* some common #define's for NULL and BOOL, etc. */

#define TRUE 1
#define FALSE 0
#define CHAR unsigned char

#ifndef NULL
#define NULL (char *) 0
#endif

/* the following are borrowed from CONFIG.H from old setup */

#define FILEMAX 13              /* max length filename with extension */
#define PATHMAX 60              /* path length max */
#define DESCMAX 129             /* max description in newexe header */

#define MAX(x, y) ((x) < (y) ? (y) : (x))    /* maximum of x and y */
#define MIN(x, y) ((x) > (y) ? (y) : (x))    /* minimum of x and y */


/* TYPEDEFS */

typedef int BOOL;

typedef struct {                   /* struct for MS-DOS Find First/Next File */
	CHAR reserved[21];
	CHAR     attr;                 /* attribute of file */
	unsigned time;                 /* time file was last written */
	unsigned date;                 /* date file was last written */
	long     length;               /* file size */
	CHAR name[FILEMAX];            /* string of name and (if exist) type */
} FINDTYPE;

struct bl_info {                    /* informations about blocks in memory */
	struct bl_info far *b_next;     /* pointer to next bl_info */
	unsigned b_size;                /* size of block in bytes */
	unsigned b_null;                /* number of null bytes to write after bl*/
	unsigned b_bits;                /* whether block appears in BIN/OVL/both */
};



/* define type 'textline' for a row of a screen */

typedef struct txline
	{
	unsigned char   row;
	unsigned char   col;
	unsigned char   * line;
	} textline;

/* This table maps the number of the section to the type of record.
** The order of this table is the same as secname[]. */

/* ********************************************************************
** Structures/union for diskette/filename information.
******************************************************************** */


/* The first 12 definitions below are of indices to the machine.f pointer
** array, defined in the last struct in the following union.
** The last entry is the size of the array.
** Note: the order of these constants is the same as S_SYSTEM.. etc.
**/

#define M_SYSTEM 0
#define M_KEYBOARD 1
#define M_MOUSE 2
#define M_DISPLAY 3
#define M_OEMLOGO 4
#define M_SOUND 5
#define M_COMM 6
#define M_FONTS 7
#define M_OEMFONTS 8
#define M_MSDOSD 9
#define M_WINOLDAP 10
#define M_SPOOLER 11

#define NMFILES 12      /* count of the above #define's */

/* this data structure contains filename and diskette number information
** for a file, as well as optional fields for pointers to description
** strings, for resolution, and other things.
** The first 4 entries are the same in all struct's in this union, except
** for the length of the filename string.
*/

union dfile {
	/* basic part.  The length of fname must be added to the
	** structure size to obtain the memory size needed.
	** This is used for file lists which have only diskette,
	** filename, and (optionally) description information.
	*/
	struct tn{
		union dfile * p;        /* next file in chain */
		unsigned char * descr;  /* pointer to description string */
		int d;                  /* diskette number */
		unsigned char fname[1]; /* file name */
		} n;

	/* kb -- for [keyboard], contains OEM charset information */
	struct tkb{
		union dfile * p;        /* next file in chain */
		unsigned char * descr;  /* pointer to description string */
		int d;                  /* diskette number */
		unsigned char fname[13];        /* file name */
					/* additional entries: */
		int charset;            /* character set */
		} kb;

	/* font -- [sysfont], [font] -- contains resolution information */
	struct tfon{
		union dfile * p;        /* next file in chain */
		unsigned char * descr;  /* pointer to description string */
		int d;                  /* diskette number */
		unsigned char fname[13];        /* file name */
					/* additional entries: */
		int res[3];             /* resolution integers */
		} fon;

	/* structure for display adapter info */
	struct tdisplay {
		union dfile * p;        /* next file in chain */
		unsigned char * descr;  /* pointer to description string */
		int d;                  /* diskette number */
		unsigned char fname[13];        /* file name */
					/* additional entries: */
		int res[3];             /* resolution integers */
		int Id;						/* Id for Display Adapter */
		} display;


	/* oemfont -- [oemfont] --
	** contains OEM charset & resolution information */

	struct toemfon{
		union dfile * p;        /* next file in chain */
		unsigned char * descr;  /* pointer to description string */
		int d;                  /* diskette number */
		unsigned char fname[13];        /* file name */
					/* additional entries: */
		int res[3];             /* resolution integers */
		int charset;            /* character set */
		} oemfon;

	/* loggrb -- contains pointer and additional file name string
	** to handle aliasing of [logos] and [grabber] entries.
	** For allocation, add length of aname[] string to size of this.
	** If the filename is aliased, the diskette number will be the
	** location of the substituted version.
	*/
	struct tloggrb{
		union dfile * p;        /* next file in chain */
		unsigned char * descr;  /* pointer to description string */
		int d;                  /* diskette number */
		unsigned char fname[13];        /* actual file name */
					/* additional entries: */
		unsigned char aname[1]; /* name corresp. to display name */
		} loggrb;

	/* iodev -- information for printer or plotter driver. */
	struct tiodev{
		union dfile * p;        /* next file in chain */
		unsigned char * descr;  /* pointer to description string */
		int d;                  /* diskette number */
		unsigned char fname[13];        /* file name */
					/* additional entries: */
		int res1[3];            /* resolution integers */
		int res2[3];            /* resolution integers */
		}iodev;

	/* this is the same as the 'basic' type 'tn', but is for country
	** information.  The names have been changed..
	** This record has variable length. */
	struct tcoun{
		union dfile * p;        /* next file in chain */
		unsigned char * cname;  /* pointer to country name */
		int d;                  /* 0 or country number */
		unsigned char cinfo[1]; /* country information */
		} coun;

	struct tmachine{
		/* This struct ends with an array of pointers to records
		** for the drivers for this system configuration */
		union dfile * p;        /* next file in chain */
		unsigned char * descr;  /* pointer to description string */
		int d;                  /* arbitrary integer */
		union dfile * f[NMFILES];       /* pointers */
		int Id;
		} machine;
	};


/* sizes of several variations of this data structure */
#define DFSIZE sizeof(struct tn)
#define DFKBSIZE sizeof(struct tkb)
#define DFFONSIZE sizeof(struct tfon)
#define DFDISPLAYSIZE sizeof(struct tdisplay)
#define DFOEMSIZE sizeof(struct toemfon)
#define DFLOGGRBSIZE sizeof(struct tloggrb)
#define DFIODEVSIZE sizeof(struct tiodev)
#define DFCOUNSIZE sizeof(struct tcoun)
#define DFMACHINESIZE sizeof(struct tmachine)


/* values of parameter identifying which of the alternative data structures
** is used for a particular file.  Routines accessing these structures
** use these parameters to determine how much memory to allocate and which
** entries past 'fname' to access.
*/

#define SW_DFND         0       /* basic, with NO description string ptr. */
#define SW_DF           1       /* the basic one WITH description string */
#define SW_DFKB         2       /* keyboard driver */
#define SW_DFFON        3       /* a font with resolution only */
#define SW_DFOEMFON     4       /* a font with resolution and OEM set */
#define SW_DFLOGGRB     5       /* a [logos] or [grabber] file entry */
#define SW_DFIODEV      6       /* a printer or plotter */
#define SW_DFCOUN       7       /* a country description */
#define SW_DFMACHINE    8       /* a machine description */
#define SW_DFHD         9       /* [harddisk] entry, same as SW_DFND */
#define SW_DFPRINTFON   10      /* a font with resolution and no descr. */
#define SW_DFDATA       11      /* [data] entry, similar to SW_DFND */
#define SW_DFDISPLAY		12      /* display driver */

/* define sections in SETUP.INF by these numbers ... */
/* Use these to index pheads[] array */

#define N_DATA 0
#define N_DISKETTE 1
#define N_WINDOWS 2
#define N_SYSTEM 3
#define N_DISPLAY 4
#define N_LOGOS 5
#define N_OEMLOGOS 6
#define N_GRABBER 7
#define N_386EXE 8
#define N_386 9
#define N_KEYBOARD 10
#define N_POINTING_DEVICE 11
#define N_SYSFONTS 12
#define N_OEMFONTS 13
#define N_COUNTRY 14
#define N_IO_DEVICE 15
#define N_IO_DEPENDENT 16
#define N_PRINTFONTS 17
#define N_APPS 18
#define N_MACHINE 19
#define N_HARDDISK 20
#define N_COMPAT 21

/* definitions for indices to pselect[] array, which points to the
** SELECTED drivers and files. */

#define S_KERNEL        0
#define S_GDI           1
#define S_USER          2
#define S_MSDOS         3
#define S_RAMDRIVE		4
						/* IMPORTANT: */
#define S_SYSTEM        5      /* this group is same order as M_SYSTEM, etc */
#define S_KEYBOARD      6
#define S_MOUSE         7
#define S_DISPLAY       8
#define S_OEMLOGO       9
#define S_SOUND         10
#define S_COMM          11
#define S_SYSFONT       12
#define S_OEMFONT       13
#define S_MSDOSD        14
#define S_WINOLDAP      15
#define S_SPOOLER       16
								
#define S_LOGO          17      /* these entries are calculated */
#define S_GRABBER       18
#define S_386           19
#define S_3EX           20
				/* Selected printer drivers and fonts */
#define S_PRINTER       21      /* first in a list of printers */
#define S_FONT          22      /* first in a list from [fonts] */
#define S_APPS          23      /* first in a list from [apps] */
								
#define SSIZE           24      /* count of above */

/* Size definition of static data for [harddisk] information */

#define HDENTRIES 20
#define HDBYTES 512

/* size definition for menu array -- no. of files which can be selected
** with ScrSelectList
*/

#define MENUSIZE 200

/* indices for lines in [data] entry (also index for inserttext[]). */

#define D_COMFILE       0
#define D_EXEFILE       1
#define D_DIRECTORY     2
#define D_SHORTNAME     3
#define D_LONGNAME      4
#define D_STARTNAME     5
#define D_EXECPGM       6

#define DSIZE           7       /* count of above */


/* Size of buffer for reading information file SETUP.INF */

#define INFBUFSIZE 8192

/* line numbers for display of messages
*/
#define MAXWINBOT 22    /* lowest line used for menu scrolling window */
#define ERRLINE   23
#define STATLINE  24    /* status line (1-based) */



/* ***** function declarations from SUSCR.C ***** */

ScrClear();
ScrDisplay(textline *);
unsigned char * ScrInputPath(unsigned char *, unsigned char *, int);
int ScrSelectList(unsigned char **);
int ScrSelectChar(textline *, int);
PutNumber(int);
PutFileName(unsigned char *);
PutDestName(unsigned char *);
PutSecName(unsigned char *);
DisplayStatusLine(unsigned char *);
ClearStatusLine();
FatalError(unsigned char *, int /* errorcode */);


/* ***** functions defined in SUINF.C ***** */

BOOL Open_Info(unsigned char *);
/* BILLHU
Close_Info();
*/
void GetWinInfo();
int ConvertRes(unsigned char *, int *); /* convert resolution string */


/* ***** functions defined in SUSEL.C ***** */

BOOL GetWinFiles();
unsigned char * DiskToHardPath(int);
SelectMachine();
/* BILLHU
int FileNDisk();
unsigned char * FileNName();
*/
SelectPath();
StoreStandardItem(int /* sno */, union dfile * /* pdfile */);
/* BILLHU
union dfile * RecNPtr(int);
*/


/* **** functions defined in SUSOURCE.C ******* */

InsertDisk(int);
int InsertOemDisk(CHAR *);

/* **** functions defined in SUDRIVER.C ******* */

int GetOemDriverList(int /* iType */, unsigned char * /* pPathInput */);
		/* iType must be S_DISPLAY, S_MOUSE, S_KEYBOARD,
		** S_PRINTER, S_SYSFONT, S_OEMFONT */

/* function declarations from sudisk.c */
unsigned readf(int, CHAR far*, unsigned);      /* far read */
unsigned writef(int, CHAR far*, unsigned);      /* far write */
int ffirst(CHAR*, unsigned, FINDTYPE*); /* find first dir entry */
int fnext(FINDTYPE*);                   /* find next dir entry */
CHAR *FileName(CHAR *, CHAR *);         /* combine path and name */
BOOL IsNewExe(CHAR*, CHAR*, CHAR*, CHAR*); /* gets lib,descr from NEWEXE info */
BOOL CopyFile(CHAR *, CHAR *);          /* copies file */
void CreateWinCom();                    /* combines file to WIN.COM */
void CopyFileContents(int, int);        /* copies contents of opened files */
void WriteMsDos();                      /* write MSDOS.EXE */
void CopyWinCnf();                      /* copy WIN.CNF */

/* function declarations from subinovl.c
*/
void BinOpen();                         /* open BIN file */
BOOL ReadModule(CHAR *, struct bl_info far **); /* read module */
void PrepModule(struct bl_info far *);  /* prepare module */
					/* constants for type of WriteModule */
#define BL_BIN 1                        /* write to BIN/GRB file */
#define BL_OVL 2                        /* write to OVL file */
void WriteModule(struct bl_info far **, unsigned);      /* write module */
void BinClose();                        /* close BIN file */
void OvlOpen();                         /* open OVL file */
/* BILLHU
void OvlClose();                        ** close OVL file **
*/
BOOL ReadGrabber(CHAR *);               /* open, read and close GRB file */
void WriteGrabber();                    /* open, write and close GRB file */


/* function and constant declarations from sucopy.c
*/                                      /* constants for type of CopyDisk(): */
#define CD_OEMDRIVER 0                  /* copy at once */
#define CD_STARTUP 1                    /* copy to startup disk */
#define CD_SYSTEM 2                     /* copy to system disk */
					/* return value of CopyDisk(): */
#define CDR_OK        0 /* copy successful */
#define CDR_IMPORTANT 1 /* can't copy files, but important files on this disk*/
#define CDR_WRONGDISK 2 /* can't copy files */
int CopyDisk(int, int);                 /* copies/loads files from one disk */
void Copy(int);                         /* copies/loads files from anywhere */
void OvlWrite();                        /* writes complete OVL file */


/* function declaration from sufont.c
*/
void SelectFonts();                     /* select appropriate fonts for */
					/* display and printers */

/* function declarations from suwinini.c
*/
void GetProfileString(CHAR *, CHAR *);
void WriteProfileString(CHAR *, CHAR *, CHAR *);
void WinIniClose();                     /* opens, writes and closes WIN.INI */
void WinIniPrepare();                   /* loads WIN.INI if necessary, sets flags*/
void WinIniIntl();                      /* write county infos if any */
void WinIniFonts();                     /* write font info if any */


/* function declarations from sudest.c
*/
void DoInitDisk();               /* select medium, formatting, WindowsPath*/
void SetPath(textline *);        /* prompt for WindowsPath, create subdir */
void MakeWindowsPath();          /* create subdir if necessary */
void ChangeToWindowsPath();      /* change to WindowsPath */

/* function declaration from sucompat.c
*/
CheckDriverCompat(); /* check CONFIG.SYS for bad drivers */

/* ******************* External data declarations ******************* */


/* ******************* Externals from SUINF.C ******************* */

extern union dfile * pheads[];
extern int iMinDisk;
extern int iMaxDisk;
extern int fhInfo;		/* file handle for SETUP.INF */
extern unsigned char * buffer;	/* buffer for read() */

/* ******************* externals from SUSEL.C ******************* */

/* arrays for menu selection in SUSEL.C */

extern unsigned char * menu[MENUSIZE];  /* pointers to descriptions */
extern int menumap[MENUSIZE];           /* indices to menuptr[] */
extern union dfile * menuptr[MENUSIZE]; /* pointers to records for each
					** driver */

/* ******************* externals from SUSOURCE.C **************** */

extern unsigned char * pSourcePath;     /* pointer to source path */


/* ***********  externals from SUDATA.C ****************** */

/* Insertion strings for displaystring(), defined in SUDATA.C
** These strings may be substituted in the code by changing the
** entries in inserttext[] to point to other strings
*/

extern int ninserts;
extern unsigned char * inserttext[];    /* 31 entries in this array */

/* Menus/screens in SUDATA */

extern textline SetupBanner[];
extern textline IncorrectDOS20[];
extern textline IncorrectDOS386[];
extern textline NotA386Machine[];
extern textline ContQuitMsg[];          /* Continue/Quit selection */
extern textline ContQuitChoices[];
extern textline PathMenu[];
extern textline MachineMenu[];
extern textline DriversMenu[];
extern textline DisplayMenu[];
extern textline KeyboardMenu[];
extern textline MouseMenu[];
extern textline SysFontMenu[];
extern textline OemFontMenu[];
extern textline InsertMenu[];
extern textline InsertOemMenu[];
extern textline SelectOemMenu[];
extern textline FinalMenu[];
extern textline txCompat[];

/* *********** Strings in SUDATA *********** */

extern unsigned char nullfile[];
extern unsigned char siNumber[];
extern unsigned char MsgNoChange[];

/* ... Strings for status messages */
extern unsigned char smReading[];
extern unsigned char smWriting[];
extern unsigned char smCopying[];
extern unsigned char smLoading[];

/* Strings for FatalError() messages */
extern CHAR smInfErr[];
extern CHAR smInfMissingErr[];
extern CHAR smNoSection[];
extern CHAR smNoMemory[];
extern CHAR smNoFile[];
extern CHAR smLoadErr[];
extern CHAR smWriteErr[];
extern CHAR smOpenErr[];
extern CHAR smDirErr[];
extern CHAR smInfOpenErr[];
extern CHAR smCurrentDir[] ;
extern CHAR smBackUpFail[];
extern CHAR smMissingID[]; 
extern CHAR smIllegalID[];
extern CHAR smMissingNoMouseRec[];
extern CHAR smNoDisplayRec[]; 
extern CHAR smIncorrectDOSL1[]; 
extern CHAR smIncorrectDOSL2[]; 
extern CHAR smNot386MachineL1[]; 
extern CHAR smNot386MachineL2[]; 


/* default WINDOWS destination path */
extern unsigned char DefaultPath[];

/* Strings for insertion in InsertOemMenu */
extern unsigned char sInsertDisplay[];
extern unsigned char sInsertKB[];
extern unsigned char sInsertMouse[];
extern unsigned char sInsertPrinter[];
extern unsigned char sInsertSysFont[];
extern unsigned char sInsertOemFont[];

extern unsigned char sOemDisplayLoaded[];
extern unsigned char sOemKBLoaded[];
extern unsigned char sOemMouseLoaded[];
extern unsigned char sOemSysFontLoaded[];
extern unsigned char sOemOemFontLoaded[];





/* externals from sumain.c */
                     /* is setup to two 360K diskettes */
/* BILLHU
extern BOOL Is360K;
*/
                     /* is setup to two diskettes */
/* BILLHU
extern BOOL IsTwoDiskette;
*/
                     /* is setup to one/two diskettes */
/* BILLHU
extern BOOL IsDiskette;
*/
extern BOOL IsQuick;                    /* is quick setup */
extern BOOL IsRuntime;                  /* is setup for application */
extern CHAR *DosPath;                   /* path environment variable */

/* externals from suwinini.c */
extern BOOL IsReinstall;                /* WIN.INI is already there */

/* externals from sudest.c */
extern CHAR WindowsPath[];              /* where to install Windows */

