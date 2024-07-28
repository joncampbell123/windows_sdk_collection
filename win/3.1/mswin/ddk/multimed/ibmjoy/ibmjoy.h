/****************************************************************************
 *
 *   ibmjoy.h
 *
 *   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved.
 *
 ***************************************************************************/

#define DEF_AXES    2            /* default number of axes */
#define DEF_TIMEOUT 0x3000       /* will be multiplied by 16 (x << 4) */

extern HANDLE ghInstance;        /* our instance handle */
extern DWORD  gdwTimeout;        /* timeout value */
extern WORD   gwcAxes;           /* how many axes we have */
extern WORD   gwDebugLevel;      /* debug level */
extern char   gszDriverName[];   /* driver name */
extern char   gszIniFile[];      /* where the szDriverName section is */
extern char   gszAxes[];         /* keyword used for number of axes */

extern WORD PASCAL Poll(WORD id, int zOnly);
extern void FAR PASCAL ibmjoyGetTimeoutValue(void);

/****************************************************************************

    Configuration support

 ***************************************************************************/

extern int FAR PASCAL Config(HWND hWnd, HANDLE hInstance);
extern int FAR PASCAL ConfigDlgProc(HWND hDlg, unsigned msg, WORD wParam, LONG lParam);

#define IDC_2AXES     0x0010
#define IDC_3AXES     0x0011

/****************************************************************************

    Debug support

 ***************************************************************************/

#ifdef DEBUG
   extern void FAR PASCAL OutputDebugStr(LPCSTR szString);    /* in mmsystem */

   #define D1(sz) if (gwDebugLevel >= 1) (OutputDebugStr("\r\nIBMJOY: "),OutputDebugStr(sz))
   #define D2(sz) if (gwDebugLevel >= 2) (OutputDebugStr(" "),OutputDebugStr(sz))
   #define D3(sz) if (gwDebugLevel >= 3) (OutputDebugStr(" "),OutputDebugStr(sz))
   #define D4(sz) if (gwDebugLevel >= 4) (OutputDebugStr(" "),OutputDebugStr(sz))

#else
   #define D1(sz) 0
   #define D2(sz) 0
   #define D3(sz) 0
   #define D4(sz) 0
#endif
