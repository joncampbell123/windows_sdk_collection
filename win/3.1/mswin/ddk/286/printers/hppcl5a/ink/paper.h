/**[f******************************************************************
* paper.h -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/

// 27 jan 92    SD      Added PaperBits2Str()  
// 06 sep 89    peterbe Added ComputeLineBufSize()
  
BOOL FAR PASCAL GetPaperFormat(PAPERFORMAT FAR *, HANDLE, short, short, short);
BOOL FAR PASCAL GetPaperBits(HANDLE, WORD FAR *);
WORD FAR PASCAL Paper2Bit(short);
short FAR PASCAL PaperBit2Str(short);
#ifndef NO_PAPERBANDCRAP
WORD FAR PASCAL ComputeLineBufSize(PAPERFORMAT FAR *, LPPCLDEVMODE);
DWORD FAR PASCAL ComputeBandBitmapSize(LPDEVICE, PAPERFORMAT FAR *,
short, short, short);
void FAR PASCAL ComputeBandingParameters (LPDEVICE, short);
void FAR PASCAL ComputeBandStartPosition(LPPOINT, LPDEVICE, short);
BOOL FAR PASCAL ComputeNextBandRect(LPDEVICE, short, LPRECT);
#endif
