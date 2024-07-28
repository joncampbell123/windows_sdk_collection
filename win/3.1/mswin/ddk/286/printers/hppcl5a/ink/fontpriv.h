/**[f******************************************************************
* fontpriv.h -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/
  
/*  PRIVATE header file to be shared between the font manager and the
*  font builder (fontman.c and fontbld.c)
*
*  THIS FILE MUST BE PRECEDED BY RESOURCE.H
*
*  12-19-89    SD      Increased VERSION_LEN to 18. 
*   1-30-89    jimmat  Added finsName field to LoadFontState typedef.
*   2-20-89    jimmat  Removed finsName from LoadFontState typedef!
*/
  
  
#define LOCAL static
  
#undef MEMORY_MANAGER
  
#define MAX_FONTS 450
#define LOTSOF_FONTS 20
#define MAXFILE_MEM 100
#define MAXFILE_MAXMEM 1500
#define MAX_FONTIDS (MAX_FONTS*2+10)
#define APPNM_LEN 64
#define FNTBL_LEN 2048
#define FNTBL_MAXLEN 20480
#define FNTBL_MAXNM 1000
#define MEM_BLOCK 2048
#define WBUF_LEN 256
#define VERSION_LEN 18
#define PORT_LEN 128
#define PERIODIC_LOOP 32
#define MAX_BUFSIZE 5120
#define FSUM_FNMLEN 72
#define WRKBUF_LEN 64
  
#define FONTMAN_GENFAIL         -1
#define FM_RESOURCE_FAIL        -2
#define FM_STRINGALLOC_FAIL     -3
#define FM_MEMALLOC_FAIL        -4
#define FM_MEMLOCK_FAIL         -5
#define FM_SOFTLOAD_FAIL        -6
#define FM_SOFTLOAD_DUPID       -7
  
  
typedef struct {
    WORD dfVersion;
    DWORD dfSize;
    char dfCopyright[60];
} PFMSHORTHEADER;
  
typedef enum {
    fnt_fontname,
    fnt_escape,
    fnt_pfmpath,
    fnt_dlpath,
    fnt_pfmname,
    fnt_dlname
} FNTNameList;
  
typedef struct {
    SYMBOLSET symbolSet;                /* symbol set (USASCII, Roman8) */
    short tblSize;                      /* number of bytes in use by table */
    short availMem;                     /* available memory in name table */
    long memUsage;                      /* memory used by downloaded font */
    short offset;                       /* offset to font in resource file */
    short fontID;                       /* ID of soft font -1 if not soft */
    short indName;                      /* ind to name of current font */
    short indEscape;                    /* ind to escape for current font */
    short indPFMPath;                   /* ind to pathname of current font */
    short indDLPath;                    /* ind of DL pathname of curr font */
    short indPFMName;                   /* ind to filename of current font */
    short indDLName;                    /* ind of DL filename of curr font */
    BOOL ZCART_hack;                    /* Z cartridge hack */
    BOOL QUOTE_hack;                    /* Typographic quotes hack */
  
    /*** Tetra begin ***/
    SCALEINFO scaleInfo;                /* scalable font flag structure */
    /*** Tetra end ***/
  
    long lPCMOffset;                    /* offset into PCM file (craigc) */
    short numExistingIDs;               /* # of existing font ID's */
    short existingID[MAX_FONTIDS];      /* existing font ID's */
    short numExistingNames;             /* number of names in table */
    short indExistingName[FNTBL_MAXNM]; /* ind to existing names in table */
    char softFontBuf[WBUF_LEN];         /* soft font buffer */
    char workBuf[WBUF_LEN];             /* work buffer */
    HANDLE hNMTBL;                      /* handle to name table */
} FONTNAMETABLE;
  
typedef struct {
    short status;                       /* non-zero if failure */
    short numFontsLoaded;               /* number of fonts loaded */
    short currentCart;                  /* current cartridge font */
    short numCartridges;                /* number of cartridges remaining */
    short nextRFont;                    /* next ROM font */
    short lastRFont;                    /* last ROM font */
    short nextCFont[DEVMODE_MAXCART];   /* next cartridge font */
    short lastCFont[DEVMODE_MAXCART];   /* last cartridge font */
    HANDLE hWinSF;                      /* handle to keynames list */
    DWORD freemem;                      /* free memory in fontSummary */
    WORD softfonts;                     /* number of key words in win.ini */
    char appName[APPNM_LEN];        /* app name for GetProfileString */
} LOADFONTSTATE;
  
typedef FONTNAMETABLE far * LPFNT;
typedef LOADFONTSTATE far * LPLFS;
  
  
extern HANDLE FAR PASCAL buildFontSummary(LPPCLDEVMODE, HANDLE, LPSTR, HANDLE);
