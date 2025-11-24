//---------------------------------------------------------------------------
//
//  Module:   vxdpipe.h
//
//  Description:
//
//
//---------------------------------------------------------------------------
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1994 - 1995 Microsoft Corporation.	All Rights Reserved.
//
//---------------------------------------------------------------------------

typedef DWORD HPIPE ;

#ifdef	Not_VxD

#ifdef	IS_32

#define	PIPEFAR

#else

#define	PIPEFAR	_far

#endif

#else	// Not_VxD

#define	PIPEFAR

#endif	// Not_VxD

typedef struct tagPIPEOPENSTRUCT
{
   DWORD         cbSize ;
   VOID PIPEFAR  *pClientProc ;

} PIPEOPENSTRUCT, PIPEFAR *PPIPEOPENSTRUCT ;

#ifndef Not_VxD

typedef struct tagPIPENODE
{
   VOID PIPEFAR  *apClientProc[ 2 ] ;
   DWORD         dn ;
   char          szName[ 9 ] ;

} PIPENODE, *PPIPENODE ;

#endif

#ifdef Not_VxD

typedef LRESULT (CALLBACK* FNPIPECALLBACK)( HPIPE, DWORD, DWORD, DWORD ) ;
typedef HPIPE (FAR CDECL *FNPIPEOPEN)( DWORD, LPSTR, PPIPEOPENSTRUCT ) ;
typedef VOID (FAR CDECL *FNPIPECLOSE)( DWORD, HPIPE ) ;

#endif

#define PIPE_API_Open            0x0200
#define PIPE_API_Close           0x0201

#define PIPE_MSG_OPEN            0x00000000
#define PIPE_MSG_INIT            0x00010000
#define PIPE_MSG_CONTROL         0x00020000
#define PIPE_MSG_NOTIFY          0x00030000
#define PIPE_MSG_CLOSE           0x00040000

#define PIPE_ERR_NOERROR         0
#define PIPE_ERR_INVALIDPROC     (DWORD) -1 
#define PIPE_ERR_INVALIDPARAM    (DWORD) -2

#ifdef WANT_MSOPL
#define MSOPL_NFY_CONTROL_CHANGE           0
#define MSOPL_NFY_LINE_CHANGE              1

#define MSOPL_CTL_GETDEVCAPS               0
#define MSOPL_CTL_DISABLE_SOFTWARE_VOLUME  1
#define MSOPL_CTL_SET_SYNTH_VOLUME         2
#define MSOPL_CTL_GET_SYNTH_VOLUME         3
#define MSOPL_CTL_SET_MASTER_VOLUME        4
#endif

#ifdef WANT_MSMPU401
#define MSMPU401_CTL_SET_ISR               0
#define MSMPU401_CTL_SET_INSTANCE          1
#endif

//---------------------------------------------------------------------------
//  End of File: vxdpipe.h
//---------------------------------------------------------------------------
