//---------------------------------------------------------------------------
//
//  Module:   pipe.h
//
//  Description:
//      VxD pipe internal definitions
//
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1994 - 1995	Microsoft Corporation.	All Rights Reserved.
 *
 **************************************************************************/

#define QUOTE(x) #x
#define QQUOTE(y) QUOTE(y)
#define REMIND(str) __FILE__ "(" QQUOTE(__LINE__) ") : " str

typedef VMMLIST *PVMMLIST  ;
typedef struct cb_s  *PVMCB ;

DWORD pipeCallProc
(
    VOID PIPEFAR* pProc,
    HPIPE         hp,
    DWORD         dwMsg,
    DWORD         dwParam1,
    DWORD         dwParam2
) ;

HPIPE pipeOpen
(
    DWORD               dn,
    PVMMLIST            phlp,
    PCHAR               psz,
    PPIPEOPENSTRUCT     pos
) ;

VOID pipeClose
(
    PVMMLIST        phlp,
    PPIPENODE       ppn
) ;

UINT CDECL vmmGetCurVMHandle( VOID ) ;
VOID CDECL vmmSimulatePush( DWORD ) ;
VOID CDECL vmmSimulateFarCall( DWORD, DWORD ) ;
VOID CDECL vmmSaveClientState( PCRS ) ;
VOID CDECL vmmRestoreClientState( PCRS ) ;
VOID CDECL vmmBeginNestExec( VOID ) ;
VOID CDECL vmmEndNestExec( VOID ) ;
VOID CDECL vmmResumeExec( VOID ) ;

//---------------------------------------------------------------------------
//  End of File: pipe.h
//---------------------------------------------------------------------------
