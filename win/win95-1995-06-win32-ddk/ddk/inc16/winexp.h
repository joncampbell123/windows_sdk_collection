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

/*****************************************************************************\
*                                                                             *
* winexp.h								      *
*                                                                             *
\*****************************************************************************/
#ifndef NOATOM
/* atom manager internals */
#define ATOMSTRUC struct atomstruct
typedef ATOMSTRUC NEAR *PATOM;
typedef ATOMSTRUC {
    PATOM chain;
    WORD  usage;             /* Atoms are usage counted. */
    BYTE  len;               /* length of ASCIZ name string */
    BYTE  name;              /* beginning of ASCIZ name string */
} ATOMENTRY;

typedef struct {
    int     numEntries;
    PATOM   pAtom[ 1 ];
} ATOMTABLE;
ATOMTABLE * PASCAL pAtomTable;
#endif

LPSTR	WINAPI lstrbscan(LPSTR, LPSTR);
LPSTR	WINAPI lstrbskip(LPSTR, LPSTR);

int	WINAPI OpenPathName(LPSTR, int);
int	WINAPI DeletePathName(LPSTR);
WORD	WINAPI _ldup(int);


/* scheduler things that the world knows not */
BOOL	WINAPI WaitEvent( HANDLE );
BOOL	WINAPI PostEvent( HANDLE );
BOOL	WINAPI KillTask( HANDLE );

/* print screen hooks */
BOOL	WINAPI SetPrtScHook(FARPROC);
FARPROC WINAPI GetPrtScHook(void);

