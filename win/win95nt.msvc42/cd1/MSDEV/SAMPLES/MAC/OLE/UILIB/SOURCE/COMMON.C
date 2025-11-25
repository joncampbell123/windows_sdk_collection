/*
 * COMMON.C
 *
 * Implements a number of support routines of the OLE2UI library
 *
 * Copyright (c)1992-1994 Microsoft Corporation, All Right Reserved
 */

	
#if defined(USEHEADER)
#include "OleHeaders.h"
#endif

#if defined(__MWERKS__) && !defined(__powerc)
#pragma pointers_in_D0
#endif

#if defined(_MSC_VER) 
#include <msvcmac.h>
#endif

#include <Dialogs.h>
#include <Events.h>
#include <Menus.h>
#include <Script.h>
#ifndef _MSC_VER
#include <AppleEvents.h>
#include <Processes.h>
#include <Resources.h>
#include <string.h>
#include <ToolUtils.h>
#else
#include <AppleEve.h>
#include <Processe.h>
#include <Resource.h>
#include <string.h>
#include <ToolUtil.h>
#endif

#include <ole2.h>

#include "ole2ui.h"
#include "common.h"
#include "uidebug.h"

#ifdef UIDLL
#include "fragload.h"
FSSpec   gOLEUIResFSSpec;
short    gOLEUIResNum = 0;
#endif

OLEDBGDATA_MAIN(NULL)

#ifdef UIDLL
/*
 * CFMOLEUIInitialization
 *
 * Purpose: Opens the resource fork of the the UIDLL
 *
 * Parameters:
 *
 * Return Value:
 */

OSErr CFMOLEInitialization(InitBlockPtr initBlkPtr)
{
     short r = CurResFile();
     gOLEUIResFSSpec = *(initBlkPtr->fragLocator.u.onDisk.fileSpec);
     gOLEUIResNum = FSpOpenResFile(&gOLEUIResFSSpec, 0);
     UseResFile(r);
     return noErr;
}

/*
 * CFMOLEUITermination
 *
 * Purpose:  Closes the resource fork of the UIDLL
 *
 * Parameters:
 *
 * Return Value:
 */
void CFMOLEUITermination(void)
{
      CloseResFile(gOLEUIResNum);
}

short SetUpOLEUIResFile()
{
   short r = CurResFile();
   UseResFile(gOLEUIResNum);
   return r;
}

void ClearOLEUIResFile(short hostResNum)
{
   UseResFile(hostResNum);
}

#endif // UIDLL


/*
 * UStandardValidation
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment CommonSeg
#else
#pragma code_seg("CommonSeg", "SWAPPABLE")
#endif
unsigned int UStandardValidation(LPOLEUISTANDARD pUI, unsigned int cbExpectedSize)
{
	if (pUI == NULL)
		return OLEUI_ERR_STRUCTURENULL;
	if (cbExpectedSize != pUI->cbStruct)
		return OLEUI_ERR_CBSTRUCTINCORRECT;
	return OLEUI_SUCCESS;
}



/*
 * UStandardInvocation
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment CommonSeg
#else
#pragma code_seg("CommonSeg", "SWAPPABLE")
#endif
unsigned int UStandardInvocation(LPOLEUISTANDARD lpUI, DialogPtr *ppDialog, int idd)
{
	DialogTHndl		hDialogT;

#ifdef UIDLL
   short          hostResNum = SetUpOLEUIResFile();
   void*          oldQD = SetLpqdFromA5();
#endif

	if (*ppDialog == NULL)
	{
		//Is there a callback routine to handle update events in case the
		//dialog is dragged? If not then we take away the titlebar to indicate
		//that this is a non-movable dialog.
		if (lpUI->lpfnHook == NULL)
		{

			hDialogT = (DialogTHndl)GetResource('DLOG', (short)idd);

			if (ResError())
         {
#ifdef UIDLL
         ClearOLEUIResFile(hostResNum);
         RestoreLpqd(oldQD);
#endif
            return OLEUI_ERR_DIALOGRES;
         }

			(**hDialogT).procID = dBoxProc;		//Dialog type without a titlebar
		}

		*ppDialog = GetNewDialog((short)idd, nil, (WindowPtr)(-1));
		if (ResError())
      {
#ifdef UIDLL
         ClearOLEUIResFile(hostResNum);
         RestoreLpqd(oldQD);
#endif
         return OLEUI_ERR_DIALOGRES;
      }
			
		//If no initial position was passed in then center the dialog just
		//above the middle of the screen.
		if (lpUI->ptPosition.h == 0 && lpUI->ptPosition.v == 0)
		{
			lpUI->ptPosition.h = (qd.screenBits.bounds.right  - (*ppDialog)->portRect.right)/2;
			lpUI->ptPosition.v = (qd.screenBits.bounds.bottom - (*ppDialog)->portRect.bottom - 20 - GetMBarHeight())/3 + 20 + GetMBarHeight();
		}
		MoveWindow(*ppDialog, lpUI->ptPosition.h, lpUI->ptPosition.v, false);

		SetPort(*ppDialog);
// REVIEW: maybe the user stored something in the refCon in the resource. Do we need to do this?
		SetWRefCon(*ppDialog, 0);
	}

#ifdef UIDLL
         ClearOLEUIResFile(hostResNum);
         RestoreLpqd(oldQD);
#endif

	ASSERTCOND(*ppDialog != NULL);

	
	return OLEUI_SUCCESS;
}





/*
 * PvStandardInit
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment CommonSeg
#else
#pragma code_seg("CommonSeg", "SWAPPABLE")
#endif
void *PvStandardInit(DialogPtr pDialog, unsigned int cbStruct)
{
	Ptr		pv;

	pv = NewPtrClear(cbStruct);
	if (pv == NULL)
		return NULL;
		
	SetWRefCon(pDialog, (long)pv);
	
	return (void *)pv;
}




/*
 * UStandardHook
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment CommonSeg
#else
#pragma code_seg("CommonSeg", "SWAPPABLE")
#endif
Boolean FStandardHook(LPOLEUISTANDARD pUI, DialogPtr pDialog, EventRecord *pEvent, short *itemHit, long lCustData)
{
	Boolean		fHooked = false;

	ASSERTCOND(pUI != NULL);

	if (pUI->lpfnHook != NULL)
		fHooked = (*pUI->lpfnHook)(pDialog, pEvent, itemHit, lCustData);

	return fHooked;
}




/*
 * CheckRadioButton
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment CommonSeg
#else
#pragma code_seg("CommonSeg", "SWAPPABLE")
#endif
void CheckRadioButton(DialogPtr pDialog, int idFirst, int idLast, int idCheck)
{
	int				id;
	short			itype;
	ControlHandle	h;
	Rect			rc;

	ASSERTCOND(idFirst <= idLast && idCheck >= idFirst && idCheck <= idLast);
	ASSERTCOND(pDialog != NULL);

	for (id=idFirst; id<=idLast; id++)
	{
		GetDItem(pDialog, (short)id, &itype, (Handle *)&h, &rc);
		ASSERTCOND((itype & ~itemDisable) == (ctrlItem+radCtrl));
		SetCtlValue(h, (short)(id == idCheck));
	}
}


/*
 * FLGetSelectedCell
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment CommonSeg
#else
#pragma code_seg("CommonSeg", "SWAPPABLE")
#endif
Boolean FLGetSelectedCell(ListHandle hList, Cell *pcell)
{
	SetPt(pcell, 0, 0);
	return LGetSelect(true, pcell, hList);
}




/*
 * FLGetSelectedCell
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment CommonSeg
#else
#pragma code_seg("CommonSeg", "SWAPPABLE")
#endif
Boolean FLSetSelection(ListHandle hList, Ptr pData, short cbSize)
{
	Cell	cell = { 0, 0 };
	Boolean	fFound;

	fFound = LSearch(pData, cbSize, NULL, &cell, hList);

	if (fFound)
		LSetSelect(true, cell, hList);

	return fFound;
}




/*
 * Ole2UIPathNameFromWD
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment CommonSeg
#else
#pragma code_seg("CommonSeg", "SWAPPABLE")
#endif
char *Ole2UIPathNameFromWD(long vRefNum, char *s)
{
	WDPBRec		pbRec;

	pbRec.ioNamePtr  = nil;
	pbRec.ioVRefNum  = vRefNum;
	pbRec.ioWDIndex  = 0;
	pbRec.ioWDProcID = 0;

	PBGetWDInfo(&pbRec,false);

	return Ole2UIPathNameFromDirID(pbRec.ioWDDirID, pbRec.ioWDVRefNum, s);
}




/*
 * Ole2UIPathNameFromDirID
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment CommonSeg
#else
#pragma code_seg("CommonSeg", "SWAPPABLE")
#endif
char *Ole2UIPathNameFromDirID(long dirID, int vRefNum, char *s)
{
	CInfoPBRec	iob;
	Str255		stDir;

	*s = 0;
	iob.dirInfo.ioNamePtr = (StringPtr) stDir;
	iob.dirInfo.ioDrParID = dirID;
	iob.dirInfo.ioCompletion = 0L;

	do
	{
		iob.dirInfo.ioVRefNum = vRefNum;
		iob.dirInfo.ioFDirIndex = -1;
		iob.dirInfo.ioDrDirID = iob.dirInfo.ioDrParID;

		if (PBGetCatInfo(&iob, false) != noErr)
			break;

		stDir[++stDir[0]] = ':';
		stDir[++stDir[0]] = 0;
		strcat((char *)stDir+1, s);
		strcpy(s, (char *)stDir+1);
	} while (iob.dirInfo.ioDrDirID != 2);

	return s;
}




/*
 * DrawDefaultBorder
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment CommonSeg
#else
#pragma code_seg("CommonSeg", "SWAPPABLE")
#endif
void DrawDefaultBorder(DialogPtr pDialog, short iitem, Boolean enabled)
{
	short		itype;
	Handle		h;
	Rect		rc;
	PenState	psOld;
	short		nOval;
#ifdef UIDLL
   void*    oldQD =  SetLpqdFromA5();
#endif

	GetDItem(pDialog, iitem, &itype, &h, &rc);

	GetPenState(&psOld);
	PenNormal();

	if (!enabled)
		PenPat((ConstPatternParam)&qd.gray);

	//This can look better depending on the size of the button.
	nOval = (rc.bottom - rc.top) / 2 + 2;

	PenSize(3, 3);
	FrameRoundRect(&rc, nOval, nOval);

	SetPenState(&psOld);
#ifdef UIDLL
   RestoreLpqd(oldQD);
#endif

}




/*
 * FlashButton
 *
 * Purpose:
 *  Hilites the indicated button for 8 ticks to give the user feedback
 *	when they use the keyboard to select a button.
 *
 * Parameters:
 *  pDialog			DialogPtr to the dialog box.
 *  iitem			short which identifies the button to hilite
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment CommonSeg
#else
#pragma code_seg("CommonSeg", "SWAPPABLE")
#endif
void FlashButton(DialogPtr pDialog, short iitem)
{
	short		itype;
	Handle		h;
	Rect		rc;
	long		ticks;

	GetDItem(pDialog, iitem, &itype, &h, &rc);
	HiliteControl((ControlHandle)h, inButton);
	Delay(8, &ticks);	//Apple recommended delay amount
	HiliteControl((ControlHandle)h, 0);
}
