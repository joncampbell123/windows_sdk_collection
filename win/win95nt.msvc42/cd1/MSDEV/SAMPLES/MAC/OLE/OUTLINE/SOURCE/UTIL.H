/*****************************************************************************\
*                                                                             *
*    KeyUtilities.h                                                           *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#ifndef __UTIL_H__
#define __UTIL_H__

#include <AppleEvents.h>
#include "Ole2.h"
#include "Dialogs.h"

// OLDNAME: KeyUtilities.h

Boolean IsThisKeyDown(const char theKey);
Boolean	IsCommandKeyDown(void);
Boolean	IsControlKeyDown(void);
Boolean	IsOptionKeyDown(void);
Boolean	IsShiftKeyDown(void);

// OLDNAME: Utilities.h

#if !defined(_MSC_VER) && !defined(__MWERKS__)

#pragma parameter bzero(__A0, __D0)
void bzero(void *p, short bytes) = {0x6002, 0x4218, 0x51C8, 0xFFFC };
	//		bra.s	@2
	//	@1	clr.b	(a0)+
	//	@2	dbra	D0, @1

#pragma parameter bcopy(__A0, __A1, __D0)
void bcopy(void *pSrc, void *pDest, short bytes) = {0x6002, 0x12D8, 0x51C8, 0xFFFC};
	//		bra.s	@1
	//	@2	move.b	(a0)+, (A1)+
	//	@1	dbra	D0, @2

#pragma parameter __A1 pStrCopy(__A1, __A0)
StringPtr pStrCopy(StringPtr pDest, StringPtr pSrc) = {0x7000, 0x1010, 0x12D8, 0x51C8, 0xFFFC};
	//		moveq	#0, D0
	//		move.b	(A0), D0
	//	@1	move.b	(A0)+, (A1)+
	//		dbra	D0, @1

#else    // _MSC_VER

void bzero(void *p, short bytes);
void bcopy(void *pSrc, void *pDest, short bytes);
StringPtr pStrCopy(StringPtr pDest, StringPtr pSrc);

#endif   // _MSC_VER

void pStrCat(StringPtr pBase, StringPtr pStr);
void strinsert(char* dst, char* src);

Boolean TrapAvailable(short tNumber, TrapType tType);
void DisableAllItems(MenuHandle theMenu);
Boolean AppleEventsInstalled(void);
short QuickdrawVersion(void);
Boolean WaitNextEventAvailable(void);
Boolean haveAUX(void);
OSErr MyGotRequiredParams(AppleEvent* theAppleEvent);
void PathNameFromDirID(long dirID, short vrefNum, char *s);
void PathNameFromFSSpec(FSSpec *fs, char *s);
Boolean FSSpecCompare(FSSpecPtr pSrc, FSSpecPtr pDest);
void CreateTemporaryFSSpec(StringPtr pBase, FSSpecPtr p);

#if !defined(_MSC_VER) && !defined(__MWERKS__)
#pragma parameter LocalToGlobalRect(__A0)
void LocalToGlobalRect(Rect *r) = {0x2F08, 0x5848, 0x2F08, 0xA870, 0xA870};

#pragma parameter GlobalToLocalRect(__A0)
void GlobalToLocalRect(Rect *r) = {0x2F08, 0x5848, 0x2F08, 0xA871, 0xA871};
#else
void LocalToGlobalRect(Rect *r);
void GlobalToLocalRect(Rect *r);
#endif

#define kDefFlashItemTime		8
void FlashItem(DialogPtr theDialog, short item, short delay);

// returns union of all StrucRgn for windows in current layer
void GetUnionWindowStrucRgns(RgnHandle rgn);

// returns the minimum depth of all intersecting devices
short GetWindowMinDepth(WindowPtr pWindow);

#if !defined(_MSC_VER) && !defined(__MWERKS__)
#pragma parameter __D0 SwapWord(__D0)
unsigned short SwapWord(unsigned short oldVal) = { 0xE158 };	// rol.w	#8, D0

#pragma parameter __D0 SwapLong(__D0)
unsigned long SwapLong(unsigned long oldVal) = { 0xE158, 0x4840, 0xE158 };
#else
unsigned short SwapWord(unsigned short oldVal);
unsigned long SwapLong(unsigned long oldVal);
#endif

Boolean AmIFrontProcess(void);
Boolean IsAppFrontProcess(OSType appSig);

#define EQUALPROCESS(psn1, psn2)		((psn1)->highLongOfPSN == (psn2)->highLongOfPSN && (psn1)->lowLongOfPSN == (psn2)->lowLongOfPSN)
#define ABS(x)							(((x) > 0) ? (x) : -(x))

#ifndef THINK_C
	#define topLeft(r)		(((Point *) &(r))[0])
	#define botRight(r)		(((Point *) &(r))[1])
#endif

void DrawTextCentered(Ptr textBuf, short byteCount, Rect* r);
OSErr FindPrefFolder(short *foundVRefNum, long *foundDirID);

void PrintHResult(HRESULT hr);
char* szFromScode(SCODE sc);

#define kOriginalQD		0
#define k8BitQD			1
#define k32BitQD		2

// OLDNAME: Vtbls.h

void InitVtbls(void);
void DisposeVtbls(void);

// OLDNAME: VtblUtilities.h


void InheritFromVtbl(void* pSuperClass, void* pBaseClass);

Boolean ValidVtbl(void* p, long size);

#endif
