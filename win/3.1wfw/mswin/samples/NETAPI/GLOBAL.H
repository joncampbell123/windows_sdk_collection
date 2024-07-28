//*****************************************************************************
//                                                                             
// Global.h  -  Global variable includes               
//                                                                                            Version 3.1                                                   *
//                                                                             
//        Copyright (c) 1992-1993, Microsoft Corp.  All rights reserved.       
//                                                                             
//
//*****************************************************************************/


// ****************************************************************************
//
//  Global Variables
//
// ****************************************************************************



HWND hWnd1;                 // handle to the window we create in Init()

HANDLE hInst;               // handle to the windows instance

HANDLE hLBhwnd;             // handle to the ListBox Window

char szPath[MAX_LENGTH];    // used in many places to hold input 
char szPrinterPort[6];      // holds the selected printer port

char *szResourceType;       // points to a WFW resource type

WORD wResourceType;         // WFW resource types: 0=drive  1=printer  2=File  3=COMM

WORD wFunction;             // Used to keep track of the selected function


BOOL bWin4x;                // Windows version 4.x or greater?

FARPROC lpfnDlgProc;        // dialog procedure #1
FARPROC lpfnDlgProc2;       // dialog procedure #2


