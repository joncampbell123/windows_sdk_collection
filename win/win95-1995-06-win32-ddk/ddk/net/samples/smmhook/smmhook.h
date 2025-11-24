//THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
//ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
//THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//
// Copyright  1993-1995  Microsoft Corporation.  All Rights Reserved.
//
//      MODULE:         smmhook.h
//
//      PURPOSE:        Global header files, data types and function prototypes
//
//	PLATFORMS:	Windows 95
//
//      FUNCTIONS:      N/A
//
//	SPECIAL INSTRUCTIONS: N/A
//

#ifndef _SMMHOOK_H_
#define _SMMHOOK_H_


//****************************************************************************
// Global Include File
//****************************************************************************

#include <windows.h>		// also includes windowsx.h
#include <windowsx.h>

#include <ras.h>                // Dial-Up Networking Session API
#include <raserror.h>           // Dial-Up Networking Session API
#include <rnaspi.h>             // Service Provider Interface

//****************************************************************************
// Macros
//****************************************************************************

#define ENTERCRITICALSECTION(x)         EnterCriticalSection(&x)
#define LEAVECRITICALSECTION(x)         LeaveCriticalSection(&x)

//****************************************************************************
// Type definitions
//****************************************************************************

typedef struct tagACB_Header {
    struct tagACB_Header FAR *pnext;
    HANDLE                   hConn;
}   ACB_HEADER, *PACB_HEADER, FAR *LPACB_HEADER;

typedef struct  tagAECB {
    ACB_HEADER              hdr;
    SESS_CONFIGURATION_INFO sci;
    HANDLE                  hThread;
    DWORD                   idThread;
    HANDLE                  hStop;
}   AECB, * LPAECB;

//****************************************************************************
// Global Parameters
//****************************************************************************

extern HANDLE    ghInstance;
extern RNA_FUNCS gRnaFuncs;

//****************************************************************************
// SMM error
//****************************************************************************

#define SESS_GETERROR_FUNC          "RnaSessGetErrorString"
typedef DWORD (WINAPI * SESSERRORPROC)(UINT, LPSTR, DWORD);

//****************************************************************************
// Function Prototypes
//****************************************************************************

DWORD WINAPI SMMSessStart (HANDLE hConn, LPSESS_CONFIGURATION_INFO lpSCI);
DWORD WINAPI SMMSessStop  (HANDLE hConn);
void  WINAPI SMMSessThread (LPAECB lpAECB);

VOID         NEAR PASCAL InitACBList();
VOID         NEAR PASCAL DeInitACBList();
LPACB_HEADER NEAR PASCAL FindACBFromConn(HANDLE hConn, DWORD dwSize);
VOID         NEAR PASCAL CleanupACB (LPACB_HEADER lpACB);
BOOL         NEAR PASCAL CloseThreadWindows(DWORD tid);

#endif  //_SMMHOOK_H_
