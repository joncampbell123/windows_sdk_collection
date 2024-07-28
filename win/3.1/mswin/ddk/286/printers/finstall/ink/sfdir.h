/**[f******************************************************************
* sfdir.h -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*
**f]*****************************************************************/
  
/*
 * $Header: 
 */

/*
 * $Log:
 */

/*  29 Jul 90   rk(HP) Changed ch in SFDIRTKSTATE from char to BYTE
*/
  
/*  Directory status/types
*/
#define SF_DEL      0x0001      /* Struct marked for delete */
#define SF_UNIQ     0x0002      /* Struct can have only one owner */
#define SF_FILE     0x0010      /* Struct is a file */
#define SF_PATH     0x0020      /* Struct is a file path name */
#define SF_LDRV     0x0040      /* Struct is a logical drive */
#define SF_SCRN     0x0080      /* Struct is a screen font */
#define SF_TKST     0x0100      /* Struct is for token state */
#define SF_CART     0x0200      /* Struct is for a cartridge */
#define SF_TYPE     (SF_FILE | SF_PATH | SF_LDRV | SF_SCRN | SF_TKST)
  
  
/*  File status/attributes
*/
#define SF_FDEL     0x1000      /* File should be deleted from disk */
#define SF_MOVEABLE 0x2000      /* File is on moveable media */
#define SF_LOADED   0x4000      /* File has been loaded */
#define SF_FATTR    (SF_FDEL | SF_MOVEABLE | SF_LOADED)
  
  
/*  Screen display types.
*/
#define SCRN_UNDEF  0x0001      /* Undefined screen type */
#define SCRN_CGA    0x0002      /* 2:1 aspect ratio */
#define SCRN_EGA    0x0004      /* 4:3 aspect ratio */
#define SCRN_1TO1   0x0008      /* 1:1 aspect ratio */
  
  
/*  General constants
*/
#define SFMAXDRV    50          /* Maximum number of logical drives */
  
  
/*  Directory entry pointer
*/
typedef struct {
    WORD state;             /* Type/status of this dir entry */
    WORD usage;             /* Number of ptrs to this item */
    WORD size;              /* Size of entry in bytes */
    long offset;            /* Item's location in directory */
} SFDIRENTRY;
  
  
/*  Directory array of pointers
*
*  Points to a buffer containing the directory entries.  The entries
*  differ in size and structure based upon the bits set in the state
*  variable of each entry pointer.
*/
typedef struct {
    HANDLE hDir;            /* Handle to directory */
    LPSTR lpDir;            /* Pointer to directory */
    WORD len;               /* Number of entries available */
    DWORD size;             /* Size of sf buf in bytes */
    SFDIRENTRY sf[1];           /* Array of entries */
} SFDIR;
  
  
/*  File directory entry
*
*  If you change this struct, then also change the screen
*  font directory struct and the PCM dir struct.
*/
typedef struct {
    int ind;                /* SF dir ind to this struct */
    BYTE orient;            /* Font's orientation */
    BYTE fIsPCM;            /* FALSE==soft font, TRUE==cartridge */
    int indLOGdrv;          /* SF dir ind to logical drive name */
    int indScrnFnt;         /* SF dir ind to screen font */
    int indDLpath;          /* SF dir ind to download path name */
    int indPFMpath;         /* SF dir ind to PFM path name */
    WORD offsDLname;            /* Offset from s to DL name */
    WORD offsPFMname;           /* Offset from s to PFM name */
    BYTE s[1];              /* Description, DL name, PFM name */
} SFDIRFILE;
  
  
/*  Screen font directory entry
*
*  This exactly matches the SFDIRFILE definition, so we
*  can use all the same utilities (the screen font file
*  is treated like the PFM file when using the utilities).
*/
typedef struct {
    int ind;                /* SF dir ind to this struct */
    int unused1;            /* unused */
    int indLOGdrv;          /* SF dir ind to logical drive name */
    int width;              /* Screen width in lines/inch */
    int height;             /* Screen height in lines/inch */
    int indFNpath;          /* SF dir ind to scrn font path name */
    WORD scrnType;          /* Screen type (EGA, CGA, 1:1) */
    WORD offsFN;            /* Offset from s to file name */
    BYTE s[1];              /* Description, scrn font file name */
} SFDIRSCRFNT;
  
  
/*  Logical drive directory entry
*/
typedef struct {
    int ind;                /* SF dir ind to this struct */
    WORD priority;          /* Priority based on order in dir file */
    WORD offsLabel;         /* Offset from s to label file name */
    WORD offsDesc;          /* Offset from s to description */
    BYTE s[1];              /* Logical drive, label, description */
} SFDIRLOGDRV;
  
  
/*  Generic string directory entry
*/
typedef struct {
    int ind;                /* SF dir ind to this struct */
    BYTE s[1];              /* String */
} SFDIRSTRNG;
  
  
typedef enum {
    tk_null = 0,
    tk_fatalerr,
    tk_fontdef,
    tk_package,
    tk_family,
    tk_logdrive,
    tk_port,
    tk_land,
    tk_cartridge
} TOKENSTATE;
  
/*  Token-state directory entry (for reading sfinstal.dir).
*/
typedef struct {
    int ind;                /* SF dir ind to this struct */
    int prevind;            /* SF dir ind to prev chained struct */
    TOKENSTATE state;           /* Current parsing state */
    int hFile;              /* Handle to sfinstal.dir file */
    LPSTR lpBuf;            /* Buffer for receiving input */
    int bufsz;              /* Size of input buffer */
//  char ch;                /* Last read character */
    BYTE ch;                /* Last read character */
    HANDLE hMd;             /* Instance handle */
    HANDLE hSFlb;           /* Handle to listbox list */
    HWND hDB;               /* Handle to dialog box */
    WORD idLB;              /* ID value of listbox */
    WORD count;             /* Number of font entries read */
    BOOL reportErr;         /* TRUE if should report errors */
    int hErrFile;           /* Handle to error file */
    WORD scrnType;          /* Screen type (EGA, CGA, 1:1) */
    int scrnHeight;         /* Screen height in lines/inch */
    int scrnWidth;          /* Screen width in lines/inch */
    WORD bestScrnDif;           /* How close current scrn font is */
    char errfile[64];           /* Name of error file */
    char sline[80];         /* Status line for status dialog */
    int indScrnFnt;         /* Index to screen font */
    int numLOGdrv;          /* Number of logical drives */
    int indLOGdrv[SFMAXDRV];        /* Array of indices to logical drives */
} SFDIRTKSTATE;
  
typedef SFDIR FAR * LPSFDIR;
typedef SFDIRENTRY FAR * LPSFDIRENTRY;
typedef SFDIRFILE FAR * LPSFDIRFILE;
typedef SFDIRLOGDRV FAR * LPSFDIRLOGDRV;
typedef SFDIRSCRFNT FAR * LPSFDIRSCRFNT;
typedef SFDIRSTRNG FAR * LPSFDIRSTRNG;
typedef SFDIRTKSTATE FAR * LPSFDIRTKSTATE;
  
int FAR PASCAL addSFdirEntry(LPSFDIR, LPSTR, WORD, WORD);
int FAR PASCAL delSFdirEntry(LPSFDIR, int);
int FAR PASCAL delSFdirFile(LPSFDIR, int, LPSTR, int);
BOOL FAR PASCAL makeSFdirFileNm(LPSFDIR, int, BOOL, LPSTR, int);
BOOL FAR PASCAL addSFdirFile(LPSFDIR, int, BOOL, LPSTR, int);
BOOL FAR PASCAL chngSFdirDesc(LPSFDIR, int, LPSTR, LPSTR, int);
int FAR PASCAL chngSFdirPath(LPSFDIR, int, BOOL, LPSTR);
int FAR PASCAL addSFdirOwner(LPSFDIR, int);
void FAR PASCAL endSFdir(LPSFDIR);
LPSTR FAR PASCAL lockSFdirEntry(LPSFDIR, int);
void FAR PASCAL unlockSFdirEntry(int);
  
#ifdef DEBUG
#ifdef DUMP_SFBUF
#define DBGdumpSFbuf(sf) dumpSFbuf(sf)
void FAR PASCAL dumpSFbuf(LPSFDIR);
#else
#define DBGdumpSFbuf(sf) /*null*/
#endif
#else
#define DBGdumpSFbuf(sf) /*null*/
#endif
  
