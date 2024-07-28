/**[f******************************************************************
* printer.h -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1988-1990 Microsoft Corporation.
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

/*  printer.h
contains the definitions of the functions in _SORT,
_BRUTE, _SPOOL.
*/
  
  
#define PASCAL
#define LONG        long
#define NULL        0
#define TRUE        1
#define FALSE       0
#define ERROR       (-1)
#define FAR  far
#define NEAR near
#define VOID void
#define REGISTER    register
  
  
/*error defs, these will eventually go in errdefs.h*/
#define ERROR       (-1)
#define SUCCESS     0
  
/* Flags for OpenFile */
  
#define OF_REOPEN       0x8000
#define OF_EXIST        0x4000
#define OF_PROMPT       0x2000
#define OF_CREATE       0x1000
#define OF_CANCEL       0x0800
#define OF_VERIFY       0x0400
#define OF_DELETE       0x0200
#define OF_PARSE        0x0100
  
#define OF_READ         0
#define OF_WRITE        1
#define OF_READWRITE    2
  
typedef int (FAR * FARPROC)();
typedef int (NEAR * NEARPROC)();
typedef unsigned LONG     DWORD;
typedef DWORD (FAR * DWORDFARPROC)();
typedef unsigned short int WORD;
typedef unsigned char      BYTE;
typedef WORD HANDLE;
typedef HANDLE HWND;
typedef HANDLE HDC;
typedef WORD ATOM;
typedef int  BOOLEAN;
typedef char *NEARP;
typedef HANDLE GLOBALHANDLE;
typedef HANDLE LOCALHANDLE;
typedef unsigned char     *PSTR;
typedef unsigned char far *LPSTR;
typedef short     BOOL;
typedef long FAR *LPLONG;
  
#define MAX(a,b)        ((a)>(b)?(a):(b))
#define MIN(a,b)        ((a)<=(b)?(a):(b))
#define ABS(x)          (((x) >= 0) ? (x) : (-(x)))
#define LWORD(x)        ((short)((x)&0xFFFF))
#define HWORD(y)        ((short)(((y)>>16)&0xFFFF))
#define MAKELONG(h,l)  ((long)(((WORD)l)|(((long)h)<<16)))
#define LOBYTE(w)       ((BYTE)w)
#define HIBYTE(w)       (((WORD)w >> 8) & 0xff)
#define MAKEPOINT(l)    (*((POINT *)&l))
  
/* Interface to global memory manager */
  
#define GMEM_DDESHARE       0x2000
#define GMEM_LOWER          0x1000
#define GMEM_FIXED          0x00
#define GMEM_MOVEABLE       0x02
#define GMEM_ZEROINIT       0x40
#define GMEM_DISCARDABLE    0x0F00
#define GHND    (GMEM_MOVEABLE | GMEM_ZEROINIT)
#define GPTR    (GMEM_FIXED    | GMEM_ZEROINIT)
  
/* Exported procedures for KERNEL module */
  
VOID          far PASCAL FatalExit( int );
  
HANDLE        far PASCAL LoadModule(LPSTR, LPSTR);
VOID          far PASCAL FreeModule(HANDLE);
HANDLE        far PASCAL GetModuleHandle(LPSTR);
int           far PASCAL GetModuleFileName( HANDLE, LPSTR, int );
FARPROC       far PASCAL GetProcAddress(HANDLE, LPSTR);
  
HANDLE far PASCAL GlobalAlloc( WORD, DWORD );
HANDLE far PASCAL GlobalReAlloc( HANDLE, DWORD, WORD );
HANDLE far PASCAL GlobalFree( HANDLE );
LPSTR  far PASCAL GlobalLock( HANDLE );
BOOL   far PASCAL GlobalUnlock( HANDLE );
LONG   far PASCAL GlobalSize( HANDLE );
LONG   far PASCAL GlobalFlags( HANDLE );
  
HANDLE far PASCAL LockSegment(WORD);
HANDLE far PASCAL UnlockSegment(WORD);
  
/* task scheduler routines */
  
extern void          far PASCAL Yield(void);
extern BOOL          far PASCAL WaitEvent(HANDLE);
extern BOOL          far PASCAL PostEvent(HANDLE);
extern HANDLE        far PASCAL GetCurrentTask(void);
  
/* MessageBox stuff */
  
#define MB_OK                   0x0000
#define MB_OKCANCEL             0x0001
#define MB_ABORTRETRYIGNORE     0x0002
#define MB_YESNOCANCEL          0x0003
#define MB_YESNO                0x0004
#define MB_RETRYCANCEL          0x0005
  
#define MB_ICONHAND             0x0010
#define MB_ICONQUESTION         0x0020
#define MB_ICONEXCLAMATION      0x0030
#define MB_ICONASTERISK         0x0040
  
#define SHOW_OPENWINDOW 1
  
int         FAR PASCAL MessageBox(HWND, LPSTR, LPSTR, WORD);
HWND        FAR PASCAL CreateDialog(HANDLE, LPSTR, HWND, FARPROC);
BOOL        FAR PASCAL DestroyWindow(HWND);
BOOL        FAR PASCAL ShowWindow(HWND, int);
void        FAR PASCAL UpdateWindow(HWND);
HWND        FAR PASCAL GetSysModalWindow(void);
HWND        FAR PASCAL FindWindow(LPSTR, LPSTR);
void        FAR PASCAL SetDlgItemInt(HWND, int, unsigned, BOOL);
void        FAR PASCAL SetDlgItemText(HWND, int, LPSTR);
FARPROC     FAR PASCAL MakeProcInstance( FARPROC, HANDLE );
void        FAR PASCAL FreeProcInstance( FARPROC );
int         FAR PASCAL GetProfileInt( LPSTR, LPSTR, int );
short       FAR PASCAL GetProfileString(LPSTR,  LPSTR,  LPSTR,  LPSTR,  short);
BOOL        FAR PASCAL WriteProfileString( LPSTR, LPSTR, LPSTR );
long        FAR PASCAL SendMessage(HWND, unsigned, WORD, LONG);
void        FAR PASCAL SetWindowText(HWND, LPSTR);
int         FAR PASCAL GetWindowText(HWND, LPSTR, int);
  
/* Interface to the resource manager */
  
HANDLE      FAR PASCAL FindResource( HANDLE, LPSTR, LPSTR );
HANDLE      FAR PASCAL LoadResource( HANDLE, HANDLE );
int         FAR PASCAL LoadString( HANDLE, unsigned, LPSTR, int );
BOOL        FAR PASCAL FreeResource( HANDLE );
  
char FAR *  FAR PASCAL LockResource( HANDLE );
  
FARPROC     FAR PASCAL SetResourceHandler( HANDLE, LPSTR, FARPROC );
HANDLE      FAR PASCAL AllocResource( HANDLE, HANDLE, DWORD );
WORD        FAR PASCAL SizeofResource( HANDLE, HANDLE );
int         FAR PASCAL AccessResource( HANDLE, HANDLE );
  
  
#define WM_INITDIALOG       0x0110
#define WM_COMMAND          0x0111
#define WM_ENDDIALOG        0x0088
#define WM_DEVMODECHANGE    0x001b
  
typedef struct {
    BYTE    cBytes;                 /* length of structure */
    BYTE    fFixedDisk;             /* non-zero if file located on non- */
    /* removeable media */
    WORD    nErrCode;               /* DOS error code if OpenFile fails */
    BYTE    reserved[ 4 ];
    BYTE    szPathName[ 120 ];
} OFSTRUCT;
  
typedef OFSTRUCT FAR * LPOFSTRUCT;
  
int         FAR PASCAL OpenFile( LPSTR, LPOFSTRUCT, WORD );
BYTE        FAR PASCAL GetTempDrive( BYTE );
int         far PASCAL OpenPathname( LPSTR, int );
int         far PASCAL DeletePathname( LPSTR );
int         far PASCAL _lopen( LPSTR, int );
void        far PASCAL _lclose( int );
int         far PASCAL _lcreat( LPSTR, int );
BOOL        far PASCAL _ldelete( LPSTR );
WORD        far PASCAL _ldup( int );
LONG        far PASCAL _llseek( int, long, int );
WORD        far PASCAL _lread( int, LPSTR, int );
WORD        far PASCAL _lwrite( int, LPSTR, int );
  
LPSTR       far PASCAL lmemcpy( LPSTR, LPSTR, WORD );
LPSTR       far PASCAL lmemset( LPSTR, BYTE, WORD );
  
int         far PASCAL lstrcmp( LPSTR, LPSTR );
LPSTR       far PASCAL lstrcpy( LPSTR, LPSTR );
LPSTR       far PASCAL lstrcat( LPSTR, LPSTR );
int         far PASCAL lstrlen( LPSTR );
LPSTR       far PASCAL lstrbscan( LPSTR, LPSTR );
LPSTR       far PASCAL lstrbskip( LPSTR, LPSTR );
  
BYTE        FAR PASCAL AnsiUpper( LPSTR );
BYTE        FAR PASCAL AnsiLower( LPSTR );
  
/*  GDI internal routines */
  
far PASCAL GetEnvironment(LPSTR, LPSTR, short);
far PASCAL SetEnvironment(LPSTR, LPSTR, short);
  
/*  _SORT export routines */
  
HANDLE FAR  PASCAL CreatePQ(short);
short  FAR  PASCAL MinPQ(HANDLE);
short  FAR  PASCAL ExtractPQ(HANDLE);
short  FAR  PASCAL InsertPQ(HANDLE, short, short);
short  FAR  PASCAL SizePQ(HANDLE, short);
void   FAR  PASCAL DeletePQ(HANDLE);
  
/*  _SPOOL export routines */
  
HANDLE FAR  PASCAL OpenJob(LPSTR, LPSTR, HANDLE);
short FAR  PASCAL StartSpoolPage(HANDLE);
short FAR  PASCAL EndSpoolPage(HANDLE);
short FAR  PASCAL WriteSpool(HANDLE, LPSTR, short);
short FAR  PASCAL CloseJob(HANDLE);
short FAR  PASCAL DeleteJob(HANDLE, short);
short FAR  PASCAL WriteDialog(HANDLE, LPSTR, short);
long  FAR  PASCAL QueryJob(HANDLE, short);
short FAR  PASCAL QueryAbort(HANDLE, short);
  
/* _SPOOL constants for queryjob */
#define SP_QUERYVALIDJOB    30
#define SP_QUERYDISKAVAIL   0x1004
  
#define USA_COUNTRYCODE     1
#define FRCAN_COUNTRYCODE   2
