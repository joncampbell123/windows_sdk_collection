/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

// penui.h - Header file for penui


#define cbSzRcMax						256

#define DRV_COMBOBOXPROBLEM  		-32
#define DRV_INIFILEMODEL	 		-40
#define DRV_INIFILECOMPORTNUMBER -41

// configuration dialog defines
#define IDC_COMBOBOX	101
#define IDC_MODEL		102
#define IDC_BLACKFRAME	201
#define IDC_SERIALGROUP 301

#define IDC_COMPORTNUMBER 400
#define IDC_COM1		IDC_COMPORTNUMBER+1
#define IDC_COM2		IDC_COMPORTNUMBER+2
#define IDC_COM3		IDC_COMPORTNUMBER+3
#define IDC_COM4		IDC_COMPORTNUMBER+4

#define IDC_PRESSURE		600
#define IDC_HEIGHT		601
#define IDC_ANGLEXY		602
#define IDC_ANGLEZ		603
#define IDC_BARRELROTATION	604
#define IDC_OEMSPECIFIC 	605

#define IDC_OK			IDOK
#define IDC_CANCEL		IDCANCEL
#define IDC_APPLYNOW	501
#define IDC_MYHELP	502

#define IDC_FRIENDLYNAMEGROUP	701
#define IDC_FRIENDLYNAME	702

// calibration dialog defines
#define IDC_TEXT		101
#define IDC_ACCEPT		102
#define IDC_RECALIBRATE 103
#define IDC_FRAME		104

// String resources
#define rsNoTabletHardware	1000
#define rsComErr				1001
#define rsModelErr			1002
#define rsConfigDialog		1003
#define rsReboot				1004
#define rsUnknownGroup		1005
#define rsSerialGroup		1006
#define rsBiosGroup			1007


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// These constants are also defined in pendrv.h!

// The following are constants that define different types of supported
// hardware.
#ifdef CPQ
#define TYPE_CPQCONCERTO		2
#endif
#define TYPE_WACOMHD648A		6
#define TYPE_WACOMSD510C		8
#define TYPE_WACOMUD0608R		9
#define TYPE_UNKNOWN				11

#define PHW_PRESSURE		0x00000001	// report pressure in OEMdata if avail
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


// Function prototypes
// win40dlg.c
BOOL FAR PASCAL _loadds ConfigDlgProc( HWND hDlg, WORD message, WPARAM wParam,
										DWORD lParam );
// exports.c
DWORD FAR PASCAL _export _loadds ConfigDialog(HDRVR hPenDriver, HANDLE hParent );

#ifdef DEBUG
#ifdef DEBLEVEL
#define DBGTRACE(h,lpstr) MessageBox(h,(LPSTR)lpstr,(LPSTR)gszDllName,MB_OK)
#else
#define DBGTRACE(h,lpstr)
#endif

#define DBGERROR(h,i,lpstr) { char szDbg[80]; \
	wsprintf((LPSTR)szDbg,"%i: %s",i,(LPSTR)lpstr); \
	MessageBox(h,(LPSTR)szDbg,(LPSTR)gszDllName,MB_OK); }

#define DBGCOMP_DW_TRACE(h,dwi,dwj,lpstr) { if(dwi==dwj) { char szDbg[80]; \
	wsprintf((LPSTR)szDbg,"%li: %s",dwi,(LPSTR)lpstr); \
	MessageBox(h,(LPSTR)szDbg,(LPSTR)gszDllName,MB_OK); } }

#define DBGCOMP_W_TRACE(h,wi,wj,lpstr)	{ if(wi==wj) { char szDbg[80]; \
	wsprintf((LPSTR)szDbg,"%i: %s",wi,(LPSTR)lpstr); \
	MessageBox(h,(LPSTR)szDbg,(LPSTR)gszDllName,MB_OK); } }

#define DBGCOMP_i_TRACE(h,i,j,lpstr)	{ if(i==j) { char szDbg[80]; \
	wsprintf((LPSTR)szDbg,"%i: %s",i,(LPSTR)lpstr); \
	MessageBox(h,(LPSTR)szDbg,(LPSTR)gszDllName,MB_OK); } }

#else
#define DBGTRACE(h,lpstr)
#define DBGERROR(h,i,lpstr)
#define DBGCOMP_DW_TRACE(h,i,j,lpstr)
#define DBGCOMP_W_TRACE(h,i,j,lpstr)
#define DBGCOMP_i_TRACE(h,i,j,lpstr)
#endif

// length of instructions text
#define INST_STR_MAX_LEN (3*STR_MAX_LEN)
#define STR_MAX_LEN	(256)

// x,y as a double, useful for computation
typedef struct
	{
	double x,y;
	} 
	DPOINT, FAR *LPDPOINT;

#define PRIVATE NEAR PASCAL
#define DLGFN _export _loadds
