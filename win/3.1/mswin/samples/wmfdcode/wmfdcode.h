#include "commdlg.h"
/* resource file for Windows Metafile Decoder */

#define  APPNAME         "Windows Metafile Decoder"  /* app name */

/* control IDs */
#define  IDGO            99
#define  IDE_RECNUM      100
#define  IDE_RECSIZE     101
#define  IDE_FUNCTION    102
#define  IDL_PARAMETERS  103

#define  IDM_ABOUT       104
#define  IDM_OPEN        105
#define  IDM_ENUM        106

#define  IDC_FILENAME    107
#define  IDC_EDIT        108
#define  IDC_FILES       109
#define  IDC_LISTBOX     110
#define  IDC_PATH        111

#define  IDM_HEADER      112
#define  IDS_VER         113
#define  IDS_SIZE        114
#define  IDS_OBJECTS     115
#define  IDS_MAXREC      116
#define  IDM_EXIT        117
#define  IDM_ALLREC      118
#define  IDM_GOTOREC     119
#define  IDM_CLEAR       120
#define  IDM_ENUMRANGE   121
#define  IDE_FROM        122
#define  IDE_TO          123
#define  IDCB_ALL        124
#define  IDL_PLAY        125
#define  IDL_PRINT       126
#define  IDM_LIST        127
#define  IDL_LBREC       128
#define  IDM_MM          129
#define  IDE_EDITMM      130
#define  IDCHANGE        131
#define  IDQUITENUM      132
#define  IDM_PRINT       133
#define  IDM_CLIPHDR     134
#define  IDM_ALDUSHDR    135
#define  IDE_MM          136
#define  IDE_XEXT        137
#define  IDE_YEXT        138
#define  IDE_HMF         139

#define  IDS_KEY         140
#define  IDS_LEFT        141
#define  IDS_RIGHT       142
#define  IDS_TOP         143
#define  IDS_BOT         144
#define  IDS_INCH        145
#define  IDS_CHKSUM      146
#define  IDCB_SEL        147
#define  IDCB_UNSEL      148

#define  IDM_DESTDISPLAY 149
#define  IDM_DESTMETA    150

#define  IDB_HEX         151
#define  IDB_DEC         152
#define  IDB_CHAR        153
#define  HEX             16
#define  DEC             10

#define  WMFDISPLAY      0
#define  WMFPRINTER      1

#define  IDM_VIEW        1
#define  IDM_PLAY        2

#define  ENUMMFSTEP      0
#define  ENUMMFLIST      1



#define  DESTDISPLAY     0
#define  DESTMETA        1
int      iDestDC;


/* common dialog structures and constants */

#define MAXFILTERLEN 256
#define FILEOPENDLG  0
#define FILESAVEDLG  1

typedef struct tagFOCHUNK  {
        OPENFILENAME of;
        char szFile[256];
        char szFileTitle[256];
} FOCHUNK;

typedef FOCHUNK FAR *LPFOCHUNK;
typedef FOCHUNK FAR *LPFSCHUNK; 
typedef WORD (CALLBACK* FARHOOK)(HWND,UINT,WPARAM,LPARAM);

/* clipboard data definitions */

#define  CLP_ID          0xC350

/* clipboard file header */
typedef struct {
        WORD FileIdentifier;
        WORD FormatCount;
} CLIPFILEHEADER;

/* clipboard format header */
typedef struct {
        WORD  FormatID;
        DWORD DataLen;
        DWORD DataOffset;
        char  Name[79];
} CLIPFILEFORMAT;

/* placeable metafile data definitions */

#define  ALDUSKEY        0x9AC6CDD7

/* placeable metafile header */
typedef struct {
        DWORD   key;
        HANDLE  hmf;
        RECT    bbox;
        WORD    inch;
        DWORD   reserved;
        WORD    checksum;
}ALDUSMFHEADER;

/* metafile function table lookup data definitions */

#define  NUMMETAFUNCTIONS 71

typedef struct tagMETAFUNCTIONS {
        char *szFuncName;
        WORD value;
} METAFUNCTIONS;

typedef WORD FAR *LPPARAMETERS;


/* global vars for main module */

#ifdef MAIN

HANDLE hInst;
HANDLE CurrenthDlg;
HANDLE hSaveCursor;
HWND   hWndMain;
HWND   hWndList;

LPPARAMETERS lpMFParams;
HANDLE hMem;
HANDLE hSelMem;
int FAR *lpSelMem;

FARPROC lpWMFRecDlgProc;
FARPROC lpprocEnumMF;
FARPROC lpPlayFromListDlgProc;
FARPROC lpProcAbout;
FARPROC lpClpHeaderDlg;
FARPROC lpHeaderDlg;
FARPROC lpAldusHeaderDlg;
FARPROC lpEnumRangeDlg;
FARPROC lpListDlgProc;

//flags

BOOL bInPaint;
BOOL bPlayRec;
BOOL bPlayItAll;
BOOL bBadFile      = FALSE;
BOOL bValidFile    = FALSE;
BOOL bMetaFileOpen = FALSE;
BOOL bMetaInRam    = FALSE;
BOOL bAldusMeta    = FALSE;
BOOL bPlayList     = FALSE;
BOOL bPlaySelList  = TRUE;
BOOL bEnumRange;

int  iEnumAction;
int  iStartRange;
int  iEndRange;

DWORD iCount = 0;               //index into lpSelMem
DWORD iNumSel = 0;               //number of listbox selections


//common fo dialog vars

char gszFilter[MAXFILTERLEN]="MetaFiles\0*.WMF;*.CLP\0";
char gszBuffer[MAXFILTERLEN];
int  nFileOffset;
int  nExtOffset;

//file io related vars

char                  OpenName[144];
char                  SaveName[144];
char                  str[255];
OFSTRUCT              ofStruct;
DWORD                 iLBItemsInBuf;
char                  fnameext[20];

//metafile related vars

HANDLE                hMF;
METAFILEPICT          MFP;
METARECORD            MetaRec;
METAHEADER            mfHeader;
ALDUSMFHEADER         aldusMFHeader;
WORD                  iRecNum = 0;

/* lookup table for metafile functions */

METAFUNCTIONS MetaFunctions[] = {

     "SETBKCOLOR",           0x0201,
     "SETBKMODE",            0x0102,
     "SETMAPMODE",           0x0103,
     "SETROP2",              0x0104,
     "SETRELABS",            0x0105,
     "SETPOLYFILLMODE",      0x0106,
     "SETSTRETCHBLTMODE",    0x0107,
     "SETTEXTCHAREXTRA",     0x0108,
     "SETTEXTCOLOR",         0x0209,
     "SETTEXTJUSTIFICATION", 0x020A,
     "SETWINDOWORG",         0x020B,
     "SETWINDOWEXT",         0x020C,
     "SETVIEWPORTORG",       0x020D,
     "SETVIEWPORTEXT",       0x020E,
     "OFFSETWINDOWORG",      0x020F,
     "SCALEWINDOWEXT",       0x0400,
     "OFFSETVIEWPORTORG",    0x0211,
     "SCALEVIEWPORTEXT",     0x0412,
     "LINETO",               0x0213,
     "MOVETO",               0x0214,
     "EXCLUDECLIPRECT",      0x0415,
     "INTERSECTCLIPRECT",    0x0416,
     "ARC",                  0x0817,
     "ELLIPSE",              0x0418,
     "FLOODFILL",            0x0419,
     "PIE",                  0x081A,
     "RECTANGLE",            0x041B,
     "ROUNDRECT",            0x061C,
     "PATBLT",               0x061D,
     "SAVEDC",               0x001E,
     "SETPIXEL",             0x041F,
     "OFFSETCLIPRGN",        0x0220,
     "TEXTOUT",              0x0521,
     "BITBLT",               0x0922,
     "STRETCHBLT",           0x0B23,
     "POLYGON",              0x0324,
     "POLYLINE",             0x0325,
     "ESCAPE",               0x0626,
     "RESTOREDC",            0x0127,
     "FILLREGION",           0x0228,
     "FRAMEREGION",          0x0429,
     "INVERTREGION",         0x012A,
     "PAINTREGION",          0x012B,
     "SELECTCLIPREGION",     0x012C,
     "SELECTOBJECT",         0x012D,
     "SETTEXTALIGN",         0x012E,
     "DRAWTEXT",             0x062F,
     "CHORD",                0x0830,
     "SETMAPPERFLAGS",       0x0231,
     "EXTTEXTOUT",           0x0a32,
     "SETDIBTODEV",          0x0d33,
     "SELECTPALETTE",        0x0234,
     "REALIZEPALETTE",       0x0035,
     "ANIMATEPALETTE",       0x0436,
     "SETPALENTRIES",        0x0037,
     "POLYPOLYGON",          0x0538,
     "RESIZEPALETTE",        0x0139,
     "DIBBITBLT",            0x0940,
     "DIBSTRETCHBLT",        0x0b41,
     "DIBCREATEPATTERNBRUSH",0x0142,
     "STRETCHDIB",           0x0f43,
     "DELETEOBJECT",         0x01f0,
     "CREATEPALETTE",        0x00f7,
     "CREATEBRUSH",          0x00F8,
     "CREATEPATTERNBRUSH",   0x01F9,
     "CREATEPENINDIRECT",    0x02FA,
     "CREATEFONTINDIRECT",   0x02FB,
     "CREATEBRUSHINDIRECT",  0x02FC,
     "CREATEBITMAPINDIRECT", 0x02FD,
     "CREATEBITMAP",         0x06FE,
     "CREATEREGION",         0x06FF,
};

/* printer variables */

HDC                   hPr;                 // handle for printer device context
POINT                 PhysPageSize;        // information about the page
BOOL                  bAbort;              // FALSE if user cancels printing
HWND                  hAbortDlgWnd;
FARPROC               lpAbortDlg;
FARPROC               lpAbortProc;

#endif /* if defined MAIN */

/* externs for modules other than main module */

#ifndef MAIN

extern HANDLE         hInst;
extern HANDLE         CurrenthDlg;
extern HANDLE         hSaveCursor;
extern HWND           hWndMain;
extern HWND           hWndList;

extern LPPARAMETERS   lpMFParams;
extern HANDLE         hMem;
extern HANDLE         hSelMem;
extern int FAR        *lpSelMem;

extern FARPROC        lpWMFRecDlgProc;
extern FARPROC        lpprocEnumMF;
extern FARPROC        lpPlayFromListDlgProc;

//flags

extern BOOL           bInPaint;
extern BOOL           bPlayRec;
extern BOOL           bPlayItAll;
extern BOOL           bBadFile;
extern BOOL           bValidFile;
extern BOOL           bMetaFileOpen;
extern BOOL           bMetaInRam;
extern BOOL           bAldusMeta;
extern BOOL           bPlayList;
extern BOOL           bPlaySelList;
extern BOOL           bEnumRange;

extern int            iEnumAction;
extern int            iStartRange;
extern int            iEndRange;

extern DWORD          iCount;              //index into lpSelMem
extern DWORD          iNumSel;             //number of listbox selections

//common dialog vars

extern char gszFilter[MAXFILTERLEN];
extern char gszBuffer[MAXFILTERLEN];
extern int  nFileOffset;
extern int  nExtOffset;

//file io related vars

extern char           OpenName[144];
extern char           SaveName[144];
extern char           str[256];
extern OFSTRUCT       ofStruct;
extern DWORD          iLBItemsInBuf;
extern char           fnameext[20];

//metafile related vars

extern HANDLE         hMF;
extern METAFILEPICT   MFP;
extern METARECORD     MetaRec;
extern METAHEADER     mfHeader;
extern ALDUSMFHEADER  aldusMFHeader;
extern WORD           iRecNum;
extern METAFUNCTIONS  MetaFunctions[];

//printer variables

extern HDC            hPr;                 // handle for printer device context
extern POINT          PhysPageSize;        // information about the page
extern BOOL           bAbort;              // FALSE if user cancels printing
extern HWND           hAbortDlgWnd;
extern FARPROC        lpAbortDlg;
extern FARPROC        lpAbortProc;

#endif /* if !defined MAIN */

/* function prototypes */

/* WMFDCODE.C */

int    PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL   InitApplication(HANDLE);
BOOL   InitInstance(HANDLE, int);
long   FAR PASCAL MainWndProc(HWND, UINT, WPARAM, LPARAM);
HANDLE FAR PASCAL OpenDlg(HWND, unsigned, WORD, LONG);
void   WaitCursor(BOOL);

/* WMFMETA.C */

int    FAR PASCAL MetaEnumProc(HDC, LPHANDLETABLE, LPMETARECORD, int, BYTE FAR *);
void   GetMetaFileAndEnum(HDC);
BOOL   LoadParameterLB(HWND, DWORD, int);
BOOL   PlayMetaFileToDest(HWND, int);
BOOL   RenderClipMeta(CLIPFILEFORMAT *, int);
BOOL   RenderPlaceableMeta(int);
void   SetPlaceableExts(HDC, ALDUSMFHEADER, int);
VOID   SetClipMetaExts(HDC, METAFILEPICT, int);
BOOL   ProcessFile(HWND, LPSTR);

/* DLGPROC.C */

BOOL FAR PASCAL WMFRecDlgProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL HeaderDlgProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL ClpHeaderDlgProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL AldusHeaderDlgProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL EnumRangeDlgProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL ListDlgProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL PlayFromListDlgProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL MMDlgProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);

/* WMFPRINT.C */

BOOL   PrintWMF(void);
HANDLE GetPrinterDC(void);
int    FAR PASCAL AbortDlg(HWND, unsigned, WORD, LONG);
int    FAR PASCAL AbortProc(HDC, int);

/* CMNDLG.C */

void InitializeStruct(WORD, LPSTR);
int  OpenFileDialog(LPSTR);
int  SaveFileDialog(LPSTR);
void SplitPath( LPSTR, LPSTR, LPSTR, LPSTR, LPSTR);
