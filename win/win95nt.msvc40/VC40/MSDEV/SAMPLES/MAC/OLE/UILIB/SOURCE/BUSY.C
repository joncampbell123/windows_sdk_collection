/*
 * BUSY.C
 *
 * Implements the OleUIBusy function which invokes the "Server Busy"
 * dialog.
 *
 * Copyright (c)1992-1994 Microsoft Corporation, All Right Reserved
 */


#if defined(USEHEADER)
#include "OleHeaders.h"
#endif

#if defined(__MWERKS__) && !defined(__powerc)
#pragma pointers_in_D0
#endif

#include "Types.h"

#if defined(_MSC_VER) && defined(__powerc)
#include <msvcmac.h>
#endif

#include <Errors.h>
#include <stdio.h>
#ifndef _MSC_VER
#include <Resources.h>
#else
#include <Resource.h>
#endif

#include <ole2.h>

#include "ole2ui.h"
#include "common.h"
#include "uidebug.h"
#include "busy.h"

#ifdef __powerc

RoutineDescriptor gRDBusyDialogProc =
   BUILD_ROUTINE_DESCRIPTOR(uppModalFilterProcInfo,BusyDialogProc);
ModalFilterUPP gBusyDialogProc = &gRDBusyDialogProc;

RoutineDescriptor gRDBusyUserItemProc =
   BUILD_ROUTINE_DESCRIPTOR(uppUserItemProcInfo,BusyUserItemProc);
UserItemUPP gBusyUserItemProc = &gRDBusyUserItemProc;

#endif


OLEDBGDATA


/*
 * OleUIBusy
 *
 * Purpose:
 *  Invokes the standard OLE "Server Busy" dialog box which
 *  notifies the user that the server application is not receiving
 *  messages.  The dialog then asks the user to either cancel
 *  the operation, switch to the task which is blocked, or continue
 *  waiting.
 *
 * Parameters:
 *  pOBZ            LPOLEUIBUSY pointing to the in-out structure
 *                  for this dialog.
 *
 * Return Value:
 *              OLEUI_BZERR_HTASKINVALID  : Error
 *              OLEUI_BZ_SWITCHTOSELECTED : Success, user selected "switch to"
 *              OLEUI_BZ_RETRYSELECTED    : Success, user selected "retry"
 *              OLEUI_CANCEL              : Success, user selected "cancel"
 */

#ifndef _MSC_VER
#pragma segment BusySeg
#else
#pragma code_seg("BusySeg", "SWAPPABLE")
#endif
STDAPI_(unsigned int) OleUIBusy(LPOLEUIBUSY pOBZ)
{
   unsigned int     	uRet = 0;
   ProcessInfoRec		pir;
   DialogPtr			pDialog;
   short 				nItem;
	Boolean				endDialog = false;
	GrafPtr				gpSave;
#ifdef UIDLL
   short             hostResNum =  SetUpOLEUIResFile();
   void*             oldQD =  SetLpqdFromA5();
#endif


	GetPort(&gpSave);

   uRet = UStandardValidation((LPOLEUISTANDARD)pOBZ, sizeof(OLEUIBUSY));

   // Error out if the standard validation failed
   if (OLEUI_SUCCESS!=uRet)
   {
     SetPort(gpSave);
#ifdef UIDLL
     ClearOLEUIResFile(hostResNum);
     RestoreLpqd(oldQD);
#endif
     return uRet;
   }
	
   // Validate pPSN
   OleStdMemSet(&pir, 0, sizeof(pir));		// the standard memset in the library doesn't work

   pir.processInfoLength = sizeof(pir);
   if (GetProcessInformation(pOBZ->pPSN, &pir) == paramErr)
     uRet = OLEUI_BZERR_HTASKINVALID;

   // Error out if our secondary validation failed
   if (OLEUI_ERR_STANDARDMIN <= uRet)
   {
     SetPort(gpSave);
#ifdef UIDLL
     ClearOLEUIResFile(hostResNum);
     RestoreLpqd(oldQD);
#endif
     return uRet;
   }

    // Invoke the dialog.
    pDialog = NULL;
    uRet=UStandardInvocation((LPOLEUISTANDARD)pOBZ, &pDialog, DIDBusy);

	if (uRet != OLEUI_SUCCESS)
   {
#ifdef UIDLL
     ClearOLEUIResFile(hostResNum);
     RestoreLpqd(oldQD);
#endif
		SetPort(gpSave);
		return uRet;
	}

	uRet = FBusyInit(pDialog, pOBZ);

	if (uRet != OLEUI_SUCCESS)
	{
		BusyCleanup(pDialog);
		SetPort(gpSave);
#ifdef UIDLL
     ClearOLEUIResFile(hostResNum);
     RestoreLpqd(oldQD);
#endif
		return uRet;
	}

	SetCursor(&qd.arrow);
	ShowWindow((WindowPtr)pDialog);

	do
	{
#ifndef __powerc
		ModalDialog(BusyDialogProc, &nItem);
#else
	  ModalDialog(gBusyDialogProc, &nItem);
#endif

		switch (nItem) {
			case BZ_BTN_SWITCHTO:
				// If user selects "Switch To...", switch activation
				// directly to the window which is causing the problem.
				uRet = OLEUI_BZ_SWITCHTOSELECTED;
				endDialog = true;
				break;

			case BZ_BTN_RETRY:
				uRet = OLEUI_BZ_RETRYSELECTED;
				endDialog = true;
				break;

			case BZ_BTN_CANCEL:
				uRet = OLEUI_CANCEL;
				endDialog = true;
				break;
		}
	} while (!endDialog);

	BusyCleanup(pDialog);

	if (uRet == OLEUI_BZ_SWITCHTOSELECTED)
		SetFrontProcess(pOBZ->pPSN);
		
	SetPort(gpSave);
#ifdef UIDLL
     ClearOLEUIResFile(hostResNum);
     RestoreLpqd(oldQD);
#endif

   return uRet;
}


/*
 * BusyDialogProc
 *
 * Purpose:
 *  Implements the OLE Busy dialog as invoked through the OleUIBusy function.
 *
 * Parameters:
 *  Standard
 *
 * Return Value:
 *  Standard
 *
 */

#ifndef _MSC_VER
#pragma segment BusySeg
pascal Boolean
#else
#pragma code_seg("BusySeg", "SWAPPABLE")
Boolean __pascal
#endif
BusyDialogProc(DialogPtr pDialog, EventRecord *pEvent, short *nItem)
{
    LPBUSY         		pBZ;
    unsigned int        uRet = 0;
	Boolean				fHooked;
	char				theChar;
	short				in;
	GrafPtr				gpSave;
	WindowPtr			pWindow;


	pBZ = (LPBUSY)GetWRefCon(pDialog);

	GetPort(&gpSave);
	SetPort(pDialog);

    //If the hook processed the message, we're done.
	fHooked = FStandardHook((LPOLEUISTANDARD)pBZ->lpOBZ, pDialog, pEvent, nItem, pBZ->lpOBZ->lCustData);
	if (fHooked)
	{
		SetPort(gpSave);
		return true;
	}

	switch (pEvent->what) {
		case mouseDown:
			in = FindWindow(pEvent->where, &pWindow);
			if ((pWindow == (WindowPtr)pDialog) && (in == inDrag) && (pBZ->lpOBZ->lpfnHook != NULL))
			{
				DragWindow(pDialog, pEvent->where, &qd.screenBits.bounds);
				SetPort(gpSave);
				return true;
			}
			break;

		case autoKey:
		case keyDown:
			// if key was pressed, handle return key
			theChar = (pEvent->message) & charCodeMask;
			
			switch (theChar) {
				case RETURNKEY:	
				case ENTERKEY:	
					FlashButton(pDialog, BZ_BTN_SWITCHTO);
					*nItem = BZ_BTN_SWITCHTO;
					SetPort(gpSave);
					return true;
					
				case PERIODKEY:
					if (!(pEvent->modifiers & cmdKey))
						break;
					//Else fall through...

				case ESCAPEKEY:
					FlashButton(pDialog, BZ_BTN_CANCEL);		// Cancel
					*nItem = BZ_BTN_CANCEL;		
					SetPort(gpSave);
					return true;
					
			}

			break;
	}

	SetPort(gpSave);

	return false;	
}


/*
 * FBusyInit
 *
 * Purpose:
 *  WM_INITIDIALOG handler for the Busy dialog box.
 *
 * Parameters:
 *		pDialog
 *		pOBZ
 *
 * Return Value:
 *  	unsigned int
 */

#ifndef _MSC_VER
#pragma segment BusySeg
#else
#pragma code_seg("BusySeg", "SWAPPABLE")
#endif
unsigned int FBusyInit(DialogPtr pDialog, POLEUIBUSY pOBZ)
{
    PBUSY			pBZ;
    Str255			processName;
	unsigned int	uRet;
	short			itype;
	Handle			h;
	Rect			rc;

    pBZ = (PBUSY)PvStandardInit(pDialog, sizeof(BUSY));

	// PvStandardInit failed
	if (pBZ == NULL)
		return OLEUI_ERR_GLOBALMEMALLOC;

	// Initialize all the user items in our dialog box.
	uRet = BZInitUserItems(pDialog);

	if (uRet != OLEUI_SUCCESS)
		return uRet;

    // Copy it to our instance of the structure (in lpBZ)
    pBZ->lpOBZ = pOBZ;

    // Copy other information from lpOBZ that we might modify.
    pBZ->dwFlags = pOBZ->dwFlags;

    // Update text in text box --
    // GetTaskInfo will return two pointers, one to the task name
    // (file name) and one to the window name.  We need to call
    // OleStdFree on these when we're done with them.  We also
    // get the WindowPtr which is blocked in this call
    //
    // In the case where this call fails, a default message should already
    // be present in the dialog template, so no action is needed
    if (GetTaskInfo(pOBZ->pPSN, processName)) {
        // Build string to present to user, place in BZ_UITEM_MSG control
        BuildBusyDialogString(pDialog, pBZ->dwFlags, processName, pBZ->szMsg);
    }

    // Disable/Enable controls
    if ((pBZ->dwFlags & BZ_DISABLECANCELBUTTON) || (pBZ->dwFlags & BZ_NOTRESPONDINGDIALOG)) {
        // Disable cancel for "not responding" dialog
		GetDItem(pDialog, BZ_BTN_CANCEL, &itype, &h, &rc);
        HiliteControl((ControlHandle)h, 255);
	}

    if (pBZ->dwFlags & BZ_DISABLESWITCHTOBUTTON) {
		GetDItem(pDialog, BZ_BTN_SWITCHTO, &itype, &h, &rc);
        HiliteControl((ControlHandle)h, 255);
	}

    if (pBZ->dwFlags & BZ_DISABLERETRYBUTTON) {
		GetDItem(pDialog, BZ_BTN_RETRY, &itype, &h, &rc);
        HiliteControl((ControlHandle)h, 255);
	}
	
    return OLEUI_SUCCESS;
}


/*
 * BZInitUserItems
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment BusySeg
#else
#pragma code_seg("BusySeg", "SWAPPABLE")
#endif
unsigned int BZInitUserItems(DialogPtr pDialog)
{
	short			iitem;
	short			itype;
	Handle			h;
	Rect			rc;

	//This assumes that the user items are grouped together and that
	//PS_UITEM_FIRST and PS_UITEM_LAST are properly defined
	for (iitem = BZ_UITEM_FIRST; iitem <= BZ_UITEM_LAST; iitem++)
	{
		GetDItem(pDialog, iitem, &itype, &h, &rc);
		if ((itype & ~itemDisable) == userItem)
#ifndef __powerc
			SetDItem(pDialog, iitem, itype, (Handle)BusyUserItemProc, &rc);
#else
		   SetDItem(pDialog, iitem, itype, (Handle)gBusyUserItemProc, &rc);
#endif
#ifdef _DEBUG
		else
			ASSERTCOND(false);
#endif //_DEBUG
	}

	return OLEUI_SUCCESS;
}


/*
 * PSUserItemProc
 *
 * Purpose:
 *  This routine draws the different user items.
 *
 * Parameters:
 *  pWindow			WindowPtr to the dialog
 *  iItem			dialog item number
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment BusySeg
pascal void
#else
#pragma code_seg("BusySeg", "SWAPPABLE")
void __pascal
#endif
BusyUserItemProc(DialogPtr pDialog, short iitem)
{
	GrafPtr			gpSave;
	short			itype;
	Handle			h;
	Rect			rc;
	PBUSY			pBZ;

	GetPort(&gpSave);
	SetPort(pDialog);

	GetDItem(pDialog, iitem, &itype, &h, &rc);
	pBZ = (PBUSY)GetWRefCon(pDialog);
	
	switch (iitem)
	{
		case BZ_UITEM_MSG:
		{
			PenSize(1, 1);
			FrameRect(&rc);

			TextBox(pBZ->szMsg, strlen(pBZ->szMsg), &rc, teFlushLeft);

			break;
		}

		case BZ_UITEM_SWITCHTOOUTLINE:
			DrawDefaultBorder(pDialog, BZ_UITEM_SWITCHTOOUTLINE, TRUE);
			break;
	}

	SetPort(gpSave);
}



/*
 * BuildBusyDialogString
 *
 * Purpose:
 *  Builds the string that will be displayed in the dialog from the
 *  task name and window name parameters.
 *
 * Parameters:
 *  pDialog         DialogPtr of the dialog
 *  dwFlags         unsigned long containing flags passed into dialog
 *  iControl        Control ID to place the text string
 *  processName     StringPtr pointing to name of process
 *
 * Return Value:
 *  void
 */

#ifndef _MSC_VER
#pragma segment BusySeg
#else
#pragma code_seg("BusySeg", "SWAPPABLE")
#endif
void BuildBusyDialogString(DialogPtr pDialog, unsigned long dwFlags, StringPtr processName, char szBuf[])
{
#ifndef _MSC_VER
#pragma unused(pDialog)
#endif

	short 			stringID;
	Handle			hString;
	
    // Load the format string out of stringtable, choose a different
    // string depending on what flags are passed in to the dialog
    if (dwFlags & BZ_NOTRESPONDINGDIALOG)
        stringID = IDS_BZRESULTTEXTNOTRESPONDING;
    else
        stringID = IDS_BZRESULTTEXTBUSY;

#ifdef UIDLL
   {
      short hostResNum = SetUpOLEUIResFile();
	   hString = (Handle)GetResource('CSTR', stringID);
      ClearOLEUIResFile(hostResNum);
   }
#else
	hString = (Handle)GetResource('CSTR', stringID);
#endif

    if (hString == NULL)
		return;

    // Build the string. The format string looks like this:
    // "This action cannot be completed because the '%s' application
    // (%s) is [busy | not responding]. Choose \"Switch To\" to activate '%s' and
    // correct the problem."

	p2cstr(processName);
    sprintf(szBuf, (char*)*hString, (char*)processName, (char*)processName);
    c2pstr((char*)processName);

}



/*
 * BusyCleanup
 *
 * Purpose:
 *  Performs busy-specific cleanup before termination.
 *
 * Parameters:
 *  hDlg            WindowPtr of the dialog box so we can access controls.
 *
 * Return Value:
 *  None
 */
#ifndef _MSC_VER
#pragma segment BusySeg
#else
#pragma code_seg("BusySeg", "SWAPPABLE")
#endif
void BusyCleanup(WindowPtr pDialog)
{
	PBUSY	pBZ;

	pBZ = (PBUSY)GetWRefCon(pDialog);

	if (pBZ)
	{
		DisposePtr((Ptr)pBZ);
	}

	DisposDialog(pDialog);
}



/*
 * GetTaskInfo()
 *
 * Purpose:  Gets information about the specified task and places the
 * module name, window name and top-level WindowPtr for the task in the specified
 * pointers
 *
 * Parameters:
 *    pPSN             	ProcessSerialNumberPtr which we want to find out more info about
 *    processName   	location that the process name is returned
 *
 */
#ifndef _MSC_VER
#pragma segment BusySeg
#else
#pragma code_seg("BusySeg", "SWAPPABLE")
#endif
Boolean GetTaskInfo(ProcessSerialNumberPtr pPSN, StringPtr processName)
{
	ProcessInfoRec	pir;
	
    OleStdMemSet(&pir, 0, sizeof(pir));		// the standard memset in the library doesn't work

    pir.processInfoLength = sizeof(pir);
    pir.processName = processName;

    return (GetProcessInformation(pPSN, &pir) == noErr);
}

