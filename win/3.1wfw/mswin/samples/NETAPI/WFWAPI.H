//*****************************************************************************
//                                                                             
// wfwapi.h  -  Sample Application includes
//                                                                                            Version 3.1                                                   *
//                                                                             
//        Copyright (c) 1992-1993, Microsoft Corp.  All rights reserved.       
//                                                                             
//
//*****************************************************************************/



//************************************************************************
// Program headers
//************************************************************************

#include "dialogs.h"                // for dialogs


//************************************************************************
//       Program define's
//************************************************************************

#define IDM_SHAREAS_DIALOG             101
#define IDM_SHARES_DIALOG              102
#define IDM_EXIT                       408
#define IDM_SERVER_BROWSE_DIALOG       700
#define IDM_STOPSHARE_DIALOG           701

#define IDM_NETWKSTAGETINFO            702
#define IDM_NETSERVERENUM2             703
#define IDM_MULTINET                   704
#define IDM_BROWSE_DIALOG              705
#define IDM_CONNECT_DIALOG             706
#define IDM_CONNECTION_DIALOG          707
#define IDM_DISCONNECT_DIALOG          708
#define IDM_GETLASTCONNECTION          709
#define IDM_GETSHARECOUNT              710
#define IDM_GETSHARENAME               711
#define IDM_GETSHAREPATH               712


#define WNTYPE_INVALID           9


#define CANCELLED                0
#define CALLING                  1
#define PARAMETERS               3
#define RETURN_CODE              4
                                       

#define STRING_NULL             '\0'

#define MAX_LENGTH              66

#define MAX_PRINTERS            10
#define PMAX_LENGTH             40

#define BUFFER_LENGTH          128

#define QUIT                    99
#define NUMBER_PRINTER_PORTS     3
#define NUMBER_RESOURCES         5
#define NO_FUNCTION              0

#define BFCHECK             0x0003

#define INIFILE            "WFWAPI.INI"


#define WFW_DETECTED             1
#define WFW_NOTDETECTED          0




//************************************************************************
//************************************************************************
//       Function Prototyping Statements
//************************************************************************
//************************************************************************



//************************************************************************
// Functions in WFWAPI.C
//************************************************************************

BOOL Init(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpszCmdLine, int CmdShow);

int MainLoop(HANDLE hInstance);



//**********************************************************************
// functions in multinet.c
//**********************************************************************

void Multinet (void);


//**********************************************************************
// functions in utility.c
//**********************************************************************

void SetLBSelection (void); 
int  GetResourceType(HANDLE hDlg);
void GetPrinterPort (HANDLE hDlg);
void GetPrinterDriver (char printer_buffer[][PMAX_LENGTH]);

void TranslateIndex (WORD wRT, WORD wIndex);
int Detect_WFW (void);

//**********************************************************************
// functions in net.c
//**********************************************************************
void WkstaGetInfo (void);
void NetEnum2 (void);

//**********************************************************************
// functions in wnet.c
//**********************************************************************

void NetBrowseDialog (void);
void NetServerBrowseDialog (void);

void NetStopShareDialog (void);
void NetShareAsDialog (void);
void NetConnectDialog (void);
void NetConnectionDialog (void);
void NetDisconnectDialog (void);
void GetLastConnection (void);
void GetShareCount (void);
void GetShareName (void);
void GetSharePath (void);

//**********************************************************************
// functions in output.c
//**********************************************************************

void Output(WORD function, WORD error_code, char *fname, char *str1, char *str2);


//**********************************************************************
// Windows functions in wfwapi.c
//**********************************************************************


long FAR PASCAL OverlappedWindowProc1 (HWND    hWnd,     
                                       UINT    wMsgID,
                                       WPARAM  wParam,
                                       LPARAM  lParam);

long FAR PASCAL DlgProc(HWND    hWndDlg,
                        UINT    wMsgID,
                        WPARAM  wParam,
                        LPARAM  lParam);


long FAR PASCAL DlgProc2(HWND    hWndDlg,
                         UINT    wMsgID,
                         WPARAM  wParam,
                         LPARAM  lParam);
