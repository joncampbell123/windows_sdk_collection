/**[f******************************************************************
* printer.h -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/
  
/*
* $Header: printer.h,v 3.890 92/02/06 16:13:09 dtk FREEZE $
*/
  
/*
* $Log:	printer.h,v $
 * Revision 3.890  92/02/06  16:13:09  16:13:09  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.870  91/11/08  11:44:55  11:44:55  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:52:54  13:52:54  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:48:13  13:48:13  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:49:47  09:49:47  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  15:00:41  15:00:41  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:50:57  16:50:57  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:18:10  14:18:10  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:34:30  16:34:30  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:35:34  10:35:34  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:13:10  14:13:10  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.812  91/08/22  14:33:03  14:33:03  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.807  91/08/08  10:32:19  10:32:19  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.802  91/07/22  12:55:27  12:55:27  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.799  91/07/02  11:52:49  11:52:49  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.796  91/06/26  11:27:09  11:27:09  stevec (Steve Claiborne)
 * BETA
 * 
 * Revision 3.790  91/06/11  16:04:22  16:04:22  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.786  91/06/11  15:45:35  15:45:35  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.785  91/05/22  14:57:59  14:57:59  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.780  91/05/15  15:58:07  15:58:07  stevec (Steve Claiborne)
* Beta
*
* Revision 3.775  91/04/05  14:32:00  14:32:00  stevec (Steve Claiborne)
* Beta release to MS
*
* Revision 3.770  91/03/25  15:36:59  15:36:59  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.760  91/03/12  07:53:48  07:53:48  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/03  07:47:04  07:47:04  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.720  91/02/11  09:16:22  09:16:22  stevec (Steve Claiborne)
* Aldus version
*
* Revision 3.710  91/02/04  15:48:36  15:48:36  stevec (Steve Claiborne)
* Aldus freeze
*
* Revision 3.700  91/01/19  09:01:17  09:01:17  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:44:16  15:44:16  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.681  91/01/14  10:18:28  10:18:28  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.680  91/01/10  16:17:35  16:17:35  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.670  90/12/14  14:55:06  14:55:06  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:36:46  15:36:46  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.660  90/12/07  14:51:15  14:51:15  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.650  90/11/30  08:13:07  08:13:07  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 3.603  90/10/25  17:16:14  17:16:14  oakeson (Ken Oakeson)
* Added GetVersion declaration
*
* Revision 3.602  90/08/24  13:24:25  13:24:25  daniels (Susan Daniels)
* ../message.txt
*
* Revision 3.601  90/08/14  15:35:00  15:35:00  oakeson (Ken Oakeson)
* Changed HIWORD and LOWORD def for TrueType
*
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
#define LWORD(x)    ((short)((DWORD)(x)&0xFFFF))
#define HWORD(y)    ((short)(((DWORD)(y)>>16)&0xFFFF))
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
#define GMEM_MODIFY     0x0080
  
/* Exported procedures for KERNEL module */
  
VOID          far PASCAL FatalExit( int );
  
WORD          far PASCAL GetVersion(void);
  
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
int     FAR PASCAL GetWindowText(HWND, LPSTR, int);
  
int     FAR PASCAL GetPrivateProfileInt( LPSTR, LPSTR, int, LPSTR);
short       FAR PASCAL GetPrivateProfileString(LPSTR, LPSTR, LPSTR,
LPSTR, short, LPSTR);
BOOL        FAR PASCAL WritePrivateProfileString( LPSTR, LPSTR, LPSTR, LPSTR );
  
  
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
void        far PASCAL _lclose( int );
int         far PASCAL _lcreat( LPSTR, int );
BOOL        far PASCAL _ldelete( LPSTR );
WORD        far PASCAL _ldup( int );
LONG        far PASCAL _llseek( int, long, int );
WORD        far PASCAL _lread( int, LPSTR, int );
WORD        far PASCAL _lwrite( int, LPSTR, int );
  
/* TETRA -- added lstrncpy() declaration -- KLO */
LPSTR       far PASCAL lstrncpy( LPSTR, LPSTR, WORD );
LPSTR       far PASCAL lmemcpy( LPSTR, LPSTR, WORD );
LPSTR       far PASCAL lmemset( LPSTR, BYTE, WORD );
  
int         far PASCAL lstrcmp( LPSTR, LPSTR );
int         far PASCAL lstrcmpi( LPSTR, LPSTR );    // new
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
// sjc - ms 7-90
int  far pascal ProfInsChk(void);
void far pascal ProfSetup(int,int);
void far pascal ProfSampRate(int,int);
void far pascal ProfStart(void);
void far pascal ProfStop(void);
void far pascal ProfClear(void);
void far pascal ProfFlush(void);
void far pascal ProfFinish(void);
void far pascal hmemset( LPSTR, BYTE, long );
LPSTR far PASCAL lstrncpy(LPSTR, LPSTR, WORD);
