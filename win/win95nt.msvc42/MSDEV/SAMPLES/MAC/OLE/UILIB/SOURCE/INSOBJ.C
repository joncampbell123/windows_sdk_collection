/*
 * INSOBJ.C
 *
 * Implements the OleUIInsertObject function which invokes the complete
 * Insert Object dialog.
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



#ifndef _MSC_VER
#ifndef THINK_C
#include <Strings.h>
#endif
#endif

#include <Dialogs.h>
#include <Events.h>
#include <Fonts.h>
#include <Menus.h>
#include <OSEvents.h>
#include <Packages.h>
#include <Scrap.h>
#include <stdio.h>
#include <String.h>
#include <Traps.h>
#ifndef _MSC_VER
#include <AppleEvents.h>
#include <Processes.h>
#include <Quickdraw.h>
#include <Resources.h>
#include <StandardFile.h>
#include <ToolUtils.h>
#else
#include <AppleEve.h>
#include <Processe.h>
#include <Quickdra.h>
#include <Resource.h>
#include <Standard.h>
#include <ToolUtil.h>
#endif

#include <ole2.h>

#include "ole2ui.h"
#include "common.h"
#include "uidebug.h"
#include "insobj.h"


OLEDBGDATA

#ifdef __powerc

RoutineDescriptor gRDIOFileFilterProc =
   BUILD_ROUTINE_DESCRIPTOR(uppFileFilterYDProcInfo,IOFileFilterProc);
FileFilterYDUPP gIOFileFilterProc = &gRDIOFileFilterProc;

RoutineDescriptor gRDInsertObjectDialogProc =
   BUILD_ROUTINE_DESCRIPTOR(uppDlgHookYDProcInfo,InsertObjectDialogProc);
DlgHookYDUPP gInsertObjectDialogProc = &gRDInsertObjectDialogProc;

RoutineDescriptor gRDInsertObjectModalProc =
   BUILD_ROUTINE_DESCRIPTOR(uppModalFilterYDProcInfo,InsertObjectModalProc);
ModalFilterYDUPP gInsertObjectModalProc = &gRDInsertObjectModalProc;

RoutineDescriptor gRDIOActivateProc =
   BUILD_ROUTINE_DESCRIPTOR(uppActivateYDProcInfo,IOActivateProc);
ActivateYDUPP gIOActivateProc = &gRDIOActivateProc;

RoutineDescriptor gRDIOUserItemProc =
   BUILD_ROUTINE_DESCRIPTOR(uppUserItemProcInfo,IOUserItemProc);
UserItemUPP  gIOUserItemProc = &gRDIOUserItemProc;

enum {
   uppPack0PatchProcInfo = kPascalStackBased
};

Handle gThe68KPatch = NULL;
UniversalProcPtr   gUpp68KPatch = NULL;

#endif

char szAssertBuf[256];


// Unfortunately CustomGetFile uses the refCon field in the WindowPtr
// to indicate to the callback routine which dialog is currently active.
// This means that we don't have a good way of getting the PINSERTOBJECT
// structure into the UserItemProc without using a global variable.
static PINSERTOBJECT	gpIO;

// We patch two traps at specific times for this dialog. We patch _Pack0
// so we can stop the standard file package from dehiliting the current
// file in the file list when the user changes the focus to the icon or label
// and we patch GetMaxDevice because we mess with the origin and since
// System 7.5 doesn't take this into account when they call GetMaxDevice on
// the portRect it sometimes returns NULL and causes a crash.
#ifndef _MSC_VER
pascal GDHandle GetMaxDevicePatch(Rect *globalRect);
#else
GDHandle __pascal GetMaxDevicePatch(Rect *globalRect);
#endif
static UniversalProcPtr gfpOldGetMaxDevice;
#ifndef __powerc
#if defined(THINK_C) || defined(__MWERKS__)
void LSetSelectPatch(void);
#else
pascal void LSetSelectPatch(void);
#endif
UniversalProcPtr gfpOldPack0;
#else
enum {
	uppGetMaxDeviceProcInfo = kPascalStackBased
		 | RESULT_SIZE(SIZE_CODE(sizeof(GDHandle)))
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(Rect*)))
};
RoutineDescriptor gRDGetMaxDevicePatch =
   BUILD_ROUTINE_DESCRIPTOR(uppGetMaxDeviceProcInfo, GetMaxDevicePatch);
UniversalProcPtr gGetMaxDevicePatch = &gRDGetMaxDevicePatch;
#endif // __powerc



/*
 * OleUIInsertObject
 *
 * Purpose:
 *  Invokes the standard OLE Insert Object dialog box allowing the
 *  user to select an object source and classname as well as the option
 *  to display the object as itself or as an icon.
 *
 * Parameters:
 *  lpOIO           LPOLEUIINSERTOBJECT pointing to the in-out structure
 *                  for this dialog.
 *
 * Return Value:
 *  unsigned int    OLEUI_SUCCESS or OLEUI_OK if all is well, otherwise
 *                  an error value.
 */

#ifndef _MSC_VER
#pragma segment InsObjectSeg
#else
#pragma code_seg("InsObjectSeg", "SWAPPABLE")
#endif
STDAPI_(unsigned int) OleUIInsertObject(LPOLEUIINSERTOBJECT lpOIO)
{
	unsigned int		uRet;
	PINSERTOBJECT		pIO;
	DialogTHndl			hDialogT;
	Point				pt;
	HRESULT				hrErr;
	short				rgnActive[] = { 4, sfItemFileListUser, IO_UITEM_LBOXNEW, IO_UITEM_ICON, IO_TE_LABELEDIT };
	Boolean				fSetupObject = false;
	CursHandle			hCursor = nil;
	GrafPtr				gpSave;
	
	GetPort(&gpSave);

	InitCursor();

	// Validate lpOIO structure
	uRet = UStandardValidation((LPOLEUISTANDARD)lpOIO, sizeof(OLEUIINSERTOBJECT));

	if (uRet != OLEUI_SUCCESS) {
		SetPort(gpSave);	
		return uRet;
	}

	pIO = gpIO = (PINSERTOBJECT)NewPtrClear(sizeof(INSERTOBJECT));

	// PvStandardInit failed
	if (pIO == NULL) {
		SetPort(gpSave);	
		return OLEUI_ERR_LOCALMEMALLOC;
	}

	pIO->lpOIO = lpOIO;

	// Is there a callback routine to handle update events in case the
	// dialog is dragged? If not then we take away the titlebar to indicate
	// that this is a non-movable dialog.
	if (lpOIO->lpfnHook == NULL)
	{

#ifdef UIDLL
      short hostResNum = SetUpOLEUIResFile();
#endif

		hDialogT = (DialogTHndl)Get1Resource('DLOG', DIDInsertObject);
		if (ResError())
		{

#ifdef UIDLL
         ClearOLEUIResFile(hostResNum);
#endif

			DisposePtr((Ptr)pIO);
			SetPort(gpSave);	
			return OLEUI_ERR_DIALOGRES;
		}

#ifdef UIDLL
         ClearOLEUIResFile(hostResNum);
#endif

		(**hDialogT).procID = dBoxProc;		// Dialog type without a titlebar
	}

	// Check if a position was provided and if not center the dialog
	pt = lpOIO->ptPosition;
	if (!pt.h && !pt.v)
		pt.h = pt.v = -1;

	//We set this to false after initialization so we don't initialize twice
	pIO->fFirstCall = true;

	//This indicates if the listbox allocation failed and we need to cancel the dialog
	pIO->fFailure = false;

	gfpOldGetMaxDevice = GetToolTrapAddress(_GetMaxDevice);
#ifndef __powerc
	SetToolTrapAddress((UniversalProcPtr)GetMaxDevicePatch, _GetMaxDevice);
#else
	SetToolTrapAddress(gGetMaxDevicePatch, _GetMaxDevice);
#endif

#ifndef __powerc
	CustomGetFile((FileFilterYDProcPtr)IOFileFilterProc,
				  -1,
				  NULL,
				  &pIO->sfReply,
				  DIDInsertObject,
				  pt,
				  (DlgHookYDProcPtr)InsertObjectDialogProc,
				  (ModalFilterYDProcPtr)InsertObjectModalProc,
				  rgnActive,
				  (ActivateYDProcPtr)IOActivateProc,
				  pIO);
#else
	CustomGetFile(gIOFileFilterProc,
				  -1,
				  NULL,
				  &pIO->sfReply,
				  DIDInsertObject,
				  pt,
				  gInsertObjectDialogProc,
				  gInsertObjectModalProc,
				  rgnActive,
				  gIOActivateProc, pIO);
#endif

#ifndef __powerc
	SetToolTrapAddress((UniversalProcPtr)gfpOldGetMaxDevice, _GetMaxDevice);
#else
	SetToolTrapAddress(gfpOldGetMaxDevice, _GetMaxDevice);
#endif

	if (ResError())
		uRet = OLEUI_ERR_DIALOGRES;
	if (MemError() || pIO->fFailure)
		uRet = OLEUI_ERR_GLOBALMEMALLOC;
	else
		uRet = (pIO->sfReply.sfGood || pIO->fHitOK ? OLEUI_OK : OLEUI_CANCEL);

	if (uRet != OLEUI_OK)
	{
		DisposePtr((Ptr)pIO);
		SetPort(gpSave);	
		return uRet;
	}

	lpOIO->dwFlags = pIO->dwFlags;
	lpOIO->clsid = pIO->clsid;

	if (pIO->fFileSelected)
	{
		if (lpOIO->pfssFile)
			*lpOIO->pfssFile = pIO->sfReply.sfFile;
		if (lpOIO->pszFile)
		{
			Ole2UIPathNameFromDirID(pIO->sfReply.sfFile.parID, pIO->sfReply.sfFile.vRefNum, lpOIO->pszFile);
			p2cstr(pIO->sfReply.sfFile.name);
			strcat(lpOIO->pszFile, (char *)pIO->sfReply.sfFile.name);
		}
	}
	lpOIO->sc = S_OK;

	if (lpOIO->dwFlags & IOF_CHECKDISPLAYASICON)
		lpOIO->hMetaPict = OlePictFromIconAndLabel(pIO->hIcon, pIO->szIconLabel, nil);
	else
		lpOIO->hMetaPict = nil;		

	if (pIO->hIcon) {
		KillPicture(pIO->hIcon);
		pIO->hIcon = nil;
	}
	
	DisposePtr((Ptr)pIO);

	// Check if Create New was selected and we have IOF_CREATENEWOBJECT
	if ((lpOIO->dwFlags & IOF_SELECTCREATENEW) && (lpOIO->dwFlags & IOF_CREATENEWOBJECT))
	{
		hCursor = GetCursor(watchCursor);
		if (hCursor != nil)
			SetCursor(*hCursor);
		hrErr = OleCreate(&lpOIO->clsid, &lpOIO->iid, lpOIO->oleRender,
			lpOIO->lpFormatEtc, lpOIO->lpIOleClientSite, lpOIO->lpIStorage,
			lpOIO->ppvObj);
		lpOIO->sc = GetScode(hrErr);
		fSetupObject = (lpOIO->sc == S_OK);
	}

	// Try Create From File
	if (lpOIO->dwFlags & IOF_SELECTCREATEFROMFILE)
	{
		if (!(lpOIO->dwFlags & IOF_CHECKLINK) && (lpOIO->dwFlags & IOF_CREATEFILEOBJECT))
		{
			hCursor = GetCursor(watchCursor);
			if (hCursor != nil)
				SetCursor(*hCursor);
			hrErr = OleCreateFromFile(&CLSID_NULL, lpOIO->pszFile, &lpOIO->iid,
				lpOIO->oleRender, lpOIO->lpFormatEtc, lpOIO->lpIOleClientSite,
				lpOIO->lpIStorage, lpOIO->ppvObj);
			lpOIO->sc = GetScode(hrErr);
			fSetupObject = (lpOIO->sc == S_OK);
		}

		if ((lpOIO->dwFlags & IOF_CHECKLINK) && (lpOIO->dwFlags & IOF_CREATELINKOBJECT))
		{
			hCursor = GetCursor(watchCursor);
			if (hCursor != nil)
				SetCursor(*hCursor);
			hrErr = OleCreateLinkToFile(lpOIO->pszFile, &lpOIO->iid,
				lpOIO->oleRender, lpOIO->lpFormatEtc, lpOIO->lpIOleClientSite,
				lpOIO->lpIStorage, lpOIO->ppvObj);
			lpOIO->sc = GetScode(hrErr);
			fSetupObject = (lpOIO->sc == S_OK);
		}
	}

	if ((lpOIO->oleRender != OLERENDER_DRAW) && (lpOIO->oleRender != OLERENDER_FORMAT))
		fSetupObject = false;

	if (fSetupObject) {
		LPOLEOBJECT		pOleObject = nil;
		LPADVISESINK	pAdviseSink = nil;
		unsigned long	dvAspect;
		
		pAdviseSink = (LPADVISESINK)OleStdQueryInterface((LPUNKNOWN)lpOIO->lpIOleClientSite, &IID_IAdviseSink);
		ASSERTCOND(pAdviseSink != nil);

		pOleObject = (LPOLEOBJECT)OleStdQueryInterface((LPUNKNOWN)*lpOIO->ppvObj, &IID_IOleObject);
		ASSERTCOND(pOleObject != nil);
		
		if (pAdviseSink) {
			if (pOleObject) {
				if (lpOIO->lpFormatEtc)
					dvAspect = lpOIO->lpFormatEtc->dwAspect;
				else
					dvAspect = DVASPECT_CONTENT;
				
				// setup advises for notification
				OleStdSetupAdvises(
						pOleObject,
						dvAspect,
						NULL,
						NULL,
						pAdviseSink);
				
				if (lpOIO->dwFlags & IOF_CHECKDISPLAYASICON) {
					/* user has requested to display icon aspect instead of content
					**    aspect.
					*/
					OleStdSwitchDisplayAspect(
							pOleObject,
							&dvAspect,
							DVASPECT_ICON,
							lpOIO->hMetaPict,
							(dvAspect != DVASPECT_ICON),	/* fDeleteOldAspect */
							TRUE,							/* fSetupViewAdvise */
							pAdviseSink,
							NULL 							/*fMustUpdate*/			// this can be ignored; update
																					// for switch to icon not req'd
					);
					
					if (lpOIO->hMetaPict) {
						OleUIPictIconFree(lpOIO->hMetaPict);
						lpOIO->hMetaPict = nil;
					}
				}
				OleStdRelease((LPUNKNOWN)pOleObject);
			}
			OleStdRelease((LPUNKNOWN)pAdviseSink);
		}
	}

	// If we tried but failed a create option, return the appropriate error
	if (lpOIO->sc != S_OK)
		uRet = OLEUI_IOERR_SCODEHASERROR;

	SetCursor(&qd.arrow);

	SetPort(gpSave);	
	return uRet;
}




/*
 * UInsertObjectInit
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment InsObjectSeg
#else
#pragma code_seg("InsObjectSeg", "SWAPPABLE")
#endif
unsigned long UInsertObjectInit(DialogPtr pDialog, PINSERTOBJECT pIO)
{
	unsigned int		uRet;
	LPOLEUIINSERTOBJECT lpOIO = pIO->lpOIO;
	short				cClasses;
	short				itype;
	Handle				h;
	Rect				rc;

	// Store the return value but finish as much of the
	// initialization as possible before returning
	uRet = IOUInitUserItems(pDialog, pIO);

	// 3. Save the original pointer and copy necessary information.
	pIO->lpOIO   = lpOIO;
	pIO->dwFlags = lpOIO->dwFlags;
	pIO->clsid   = lpOIO->clsid;

	// 4. Fill the Object Type listbox with entries from the reg DB.
	if (pIO->hListNew)
		cClasses = UFillClassList(pIO->hListNew, lpOIO->cClsidExclude, lpOIO->lpClsidExclude,
								  (Boolean)(lpOIO->dwFlags & IOF_VERIFYSERVERSEXIST));

	// UFillClasssList selected the first class
	pIO->nSelectedIndex = -1;
	pIO->nOldIndex = -1;

    pIO->fDefaultIcon = true;

	// 4a. Initialize the result display.
	pIO->szResult[0] = 0;
	pIO->nResultIcon = RESULTIMAGE_NONE;

	// 5. Initialize the Display As Icon state
	IOUpdateDisplayAsIcon(pDialog, pIO);

	// 6. If an initial FSSpec is provided, copy it into our local buffer
	if (pIO->dwFlags & IOF_SELECTDEFAULTFILE)
		pIO->sfReply.sfFile = *lpOIO->pfssFile;

	// 7. Initialize the selected type radiobutton.
	CheckRadioButton(pDialog, IO_RBTN_NEW, IO_RBTN_FROMFILE,
					 pIO->dwFlags & IOF_SELECTCREATENEW ? IO_RBTN_NEW : IO_RBTN_FROMFILE);

	// ?. Init active item
	pIO->iActiveItem = sfItemFileListUser;
	pIO->fLabelEditInFileView = FALSE;

	// 8. Update flags and settings based on initial creation type
	if (pIO->dwFlags & IOF_SELECTCREATENEW)
	{
		pIO->fAsIconNew = ((pIO->dwFlags & IOF_CHECKDISPLAYASICON) != 0L);

		pIO->dwFlags &= ~IOF_SELECTCREATENEW;	// Force it to update
		IOToggleObjectSource(pDialog, pIO, IOF_SELECTCREATENEW);

		SetOrigin((short)(pDialog->portRect.left + 10000), pDialog->portRect.top);

		pIO->fDoToggle = true;
	}
	else
	{
		if (!(pIO->dwFlags & IOF_DISABLELINK))
		{
			GetDItem(pDialog, IO_CBOX_LINK, &itype, &h, &rc);
			SetCtlValue((ControlHandle)h, (short)((pIO->dwFlags & IOF_CHECKLINK) != 0L));
		}

		pIO->fAsIconFile = ((pIO->dwFlags & IOF_CHECKDISPLAYASICON) != 0L);
	}
	
	// 8a. Force activation of proper list box
	PostEvent(keyDown, TABKEY);

	// 9. Hide the link checkbox if we don't want it.
	if (pIO->dwFlags & IOF_DISABLELINK)
		HideDItem(pDialog, IO_CBOX_LINK);

	// 10. Hide the help button if we don't want it.
	if (!(lpOIO->dwFlags & IOF_SHOWHELP))
		HideDItem(pDialog, IO_BTN_HELP);

	// 11.  Initialize various other fields of pIO
	pIO->fSavedFSP = FALSE;

	// 12. Update the controls and results display
	IOUpdateAllControls(pDialog, pIO);

	// 13. Change the caption.
	if (lpOIO->lpszCaption != NULL)
		SetWTitle(pDialog, (StringPtr)lpOIO->lpszCaption);
		
	// 14. Force a check of the selected item in the active list
	if(pIO->dwFlags & IOF_SELECTCREATEFROMFILE)  {
		pIO->fCheckFileSelection = TRUE;
		pIO->fCheckClassSelection = FALSE;
	}
	else {
		pIO->fCheckFileSelection = FALSE;
		pIO->fCheckClassSelection = TRUE;
	}
	pIO->fForceIconUpdate = TRUE;

	ForeColor(whiteColor);
#ifdef dangerousPattern
	PenPat(qd.white);
#else
	PenPat(&qd.white);
#endif

	return uRet;
}




/*
 * IOUInitUserItems
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment InsObjectSeg
#else
#pragma code_seg("InsObjectSeg", "SWAPPABLE")
#endif
unsigned long IOUInitUserItems(DialogPtr pDialog, PINSERTOBJECT pIO)
{
	short		iitem;
	short		itype;
	Handle		h;
	Rect		rc,
				rcDataBounds;
	Point		ptSize;
	short		theProc;

	// This assumes that the user items are grouped together and that
	// IO_UITEM_FIRST and IO_UITEM_LAST are properly defined
	for (iitem = IO_UITEM_FIRST; iitem <= IO_UITEM_LAST; iitem++)
	{
		GetDItem(pDialog, iitem, &itype, &h, &rc);
		if ((itype & ~itemDisable) == userItem)
#ifndef __powerc
			SetDItem(pDialog, iitem, itype, (Handle)IOUserItemProc, &rc);
#else
	      SetDItem(pDialog, iitem, itype, (Handle)gIOUserItemProc, &rc);

#endif

#ifdef _DEBUG
		else
			ASSERTCOND(false);
#endif // _DEBUG
	}

	// Calculate the real size of the listbox
	GetDItem(pDialog, IO_UITEM_LBOXNEW, &itype, &h, &rc);
	InsetRect(&rc, 1, 1);
	rc.right -= 15;							// Add space for vertical scroll bar

	SetRect(&rcDataBounds, 0, 0, 2, 0);		// Two column listbox
	SetPt(&ptSize, (short)(rc.right-rc.left), 0);	// First column fills width of list (second isn't visible)

	// CUSTOM: Add your own ldef here
	theProc = 0;
	pIO->hListNew = LNew(&rc, &rcDataBounds, ptSize, theProc, (WindowPtr)pDialog, true, false, false, true);

	if (pIO->hListNew == NULL)
	{
    	return OLEUI_ERR_GLOBALMEMALLOC;
    }
		
   if (NULL == (*pIO->hListNew)->vScroll)  // bugbug 5447
   {
      ControlHandle theVScroll = NULL;
      Rect vScrollRect;

      SetRect(&vScrollRect, (short)rc.right, (short)rc.top,
                     (short)(rc.right + 16), (short)rc.bottom);
      theVScroll = NewControl((WindowPtr)pDialog, &vScrollRect, NULL, true,
                                 -7, 0, -7, scrollBarProc, 0L);
      (*pIO->hListNew)->vScroll = theVScroll;
   }

	(**pIO->hListNew).selFlags = lOnlyOne|lNoNilHilite;

	return OLEUI_SUCCESS;
}




/*
 * UFillClassList
 *
 * Purpose:
 *  Enumerates available OLE object classes from the registration
 *  database and fills a listbox with those names.
 *
 *  Note that this function removes any prior contents of the listbox.
 *
 * Parameters:
 *  hList			ListHandle to be filled with object types.
 *  cIDEx           unsigned int number of CLSIDs to exclude in lpIDEx
 *  lpIDEx          LPCLSID to CLSIDs to leave out of the listbox.
 *  fVerify         Boolean indicating if we are to validate existence of
 *                  servers before putting them in the list.
 *
 * Return Value:
 *  unsigned int    Number of strings added to the listbox, -1 on failure.
 */

#ifndef _MSC_VER
#pragma segment InsObjectSeg
#else
#pragma code_seg("InsObjectSeg", "SWAPPABLE")
#endif
unsigned int UFillClassList(ListHandle hList, unsigned int cIDEx, LPCLSID lpIDEx, Boolean fVerify)
{
#ifndef _MSC_VER
//#pragma unused(fVerify)
#endif

	unsigned int	cStrings;
	unsigned int	cClasses;
	unsigned int	i;
	long			lRet;
	LPMALLOC		pIMalloc;
	char			*pszExec;
	char			*pszClass;
	char			*pszKey;
	char			*pszID;
	unsigned long	dwFlags;
	CLSID			clsid;
  	Point			ptCell;
  	short			cch;
  	long			dw;

	// Get some work buffers
	if (NOERROR != CoGetMalloc(MEMCTX_TASK, &pIMalloc))
		return (unsigned int)-1;

	pszExec = pIMalloc->lpVtbl->Alloc(pIMalloc, OLEUI_CCHKEYMAX*4);

	if (pszExec == NULL)
	{
		pIMalloc->lpVtbl->Release(pIMalloc);
		return (unsigned int)-1;
	}

	pszClass = pszExec+OLEUI_CCHKEYMAX;
	pszKey   = pszClass+OLEUI_CCHKEYMAX;
	pszID    = pszKey+OLEUI_CCHKEYMAX;

	cStrings = 0;	// Used as index in registration database
	cClasses = 0;	// Used as real count of classes added to list

	while (true)
	{
		lRet = RegEnumProgID(HKEY_CLASSES_ROOT, cStrings++, pszID, OLEUI_CCHKEYMAX,
							 pszKey, OLEUI_CCHKEYMAX, &dwFlags);

		if (lRet != (long)ERROR_SUCCESS)
			break;

		// Check if this entry should be added, and if not continue to the next
		if (!(dwFlags & REGENUMPROGIDF_INSERTABLE))
			continue;

		// Check for \NotInsertable. if this is found then this overrides
		// all other keys; this class will NOT be added to the InsertObject
		// list.
		cch = strlen(pszID);
		strcpy(pszID+cch, "\\NotInsertable");

		dw = OLEUI_CCHKEYMAX;
		lRet = RegQueryValue(HKEY_CLASSES_ROOT, pszID, pszKey, &dw);
		
		if ((long)ERROR_SUCCESS == lRet)
			continue;   // NotInsertable IS found--skip this class
		
		// Remove \NotInsertable from the end of the string.
		pszID[cch] = 0;

		CLSIDFromProgID(pszID, &clsid);

		// REVIEW: If fVerify is true, we need to check that the server exists

		// Check if this CLSID is in the exclusion list.
		for (i = 0; i < cIDEx; i++)
			if (IsEqualCLSID(&clsid, (LPCLSID)(lpIDEx+i)))
				continue;

		SetPt(&ptCell, 0, 0);
		while (ptCell.v < (**hList).dataBounds.bottom)
		{
			cch = OLEUI_CCHKEYMAX;
			LGetCell(pszClass, &cch, ptCell, hList);

			if (IUMagString(pszClass, pszKey, cch, (short)strlen(pszKey)) > 0)
				break;

			ptCell.v++;
		}

		// Add a row and set the value to the class string
	  	LAddRow(1, ptCell.v, hList);
	  	LSetCell(pszKey, (short)strlen(pszKey), ptCell, hList);

		// Add the class ID to the invisible column
		ptCell.h++;
		LSetCell(&clsid, sizeof(clsid), ptCell, hList);

		cClasses++;
	}

	// Select the first item by default
	SetPt(&ptCell, 0, 0);
	LSetSelect(true, ptCell, hList);

	pIMalloc->lpVtbl->Free(pIMalloc, (void *)pszExec);
	pIMalloc->lpVtbl->Release(pIMalloc);

	return cClasses;
}


/*
 * InsertObjectDialogProc
 *
 * Purpose:
 *  Implements the OLE Insert Object dialog as invoked through the
 *  OleUIInsertObject function.
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment InsObjectSeg
pascal short
#else
#pragma code_seg("InsObjectSeg", "SWAPPABLE")
short __pascal
#endif
InsertObjectDialogProc(short iitem, DialogPtr pDialog, PINSERTOBJECT pIO)
{
	unsigned long	uRet;
	short			cch;
	Cell			cell;
	GrafPtr			gpSave;
	short			itype;
	Handle			h;
	Rect			rc;
	Boolean			fCheck;
	Boolean 		fOldSelected;

#ifdef __powerc
	if (iitem & 0x00008000)
	    iitem |= 0xFFFF0000;  // REVIEW: This hack needed for a compiler problem.
#endif


	// Only continue if we're in the main dialog
	if (GetWRefCon(pDialog) != sfMainDialogRefCon)
		return iitem;

	GetPort(&gpSave);
	SetPort(pDialog);

	switch (iitem)
	{
		case sfHookFirstCall:
			// sfHookFirstCall is sent once when the dialog is first brought
			// up and this is a good opportunity to do our initialization.
			// Unfortunately any time the user clicks on the background of
			// the dialog we're called with the value -1 which is equivalent
			// to sfHookFirstCall. We need to avoid doing our initialization
			// more than once so we guard against it.
			if (pIO->fFirstCall == true)
			{
				uRet = UInsertObjectInit(pDialog, pIO);
				if (uRet != OLEUI_SUCCESS)
					pIO->fFailure = true;
				pIO->fFirstCall = false;
			}
			break;

		case sfHookLastCall:
			if (pIO->hListNew)
			{
				// Get CLSID from second column
				if (pIO->dwFlags & IOF_SELECTCREATENEW)
				{
					if (pIO->fCheckClassSelection)
					{
						IOCheckClassSelection(pIO);
						pIO->fCheckClassSelection = false;
					}
					cch = sizeof(pIO->clsid);
					SetPt(&cell, 1, (short)pIO->nSelectedIndex);
					LGetCell(&pIO->clsid, &cch, cell, pIO->hListNew);				
				}
				LDispose(pIO->hListNew);
			}
			break;

		case sfHookNullEvent:
			if (pIO->fFailure)
			{
				iitem = sfItemCancelButton;
				break;
			}
			
			if(pIO->fJumpToList)
			{
				iitem = sfHookSetActiveOffset + ((pIO->dwFlags & IOF_SELECTCREATEFROMFILE) ? sfItemFileListUser : IO_UITEM_LBOXNEW);
				break;
			}

			if (pIO->fHitOK)
			{
				// save the FSSpec from the file list box so that an icon can be retrieved for the previously highlighted filename
				memcpy(&pIO->sfReply.sfFile, &pIO->fspSave, sizeof(FSSpec));
				pIO->fSavedFSP = false;
				pIO->fHitOK = false;
			}
			
			// check to see if file selection changed
			if(pIO->fCheckFileSelection)
			{
				fOldSelected = pIO->fFileSelected;
				pIO->fCheckFileSelection = false;

				if (IOCheckFileSelection(pIO))
				{
					IOUpdateAllControls(pDialog, pIO);

					if (pIO->iActiveItem == sfItemFileListUser)
					{
						if(pIO->fFileSelected && !fOldSelected)
							IOUpdateResults(pDialog, FALSE);
						else if(!pIO->fFileSelected && fOldSelected)
							IOUpdateResults(pDialog, TRUE);

						if(pIO->dwFlags & IOF_SELECTCREATEFROMFILE && pIO->fAsIconFile)
							IOUpdateIconArea(pDialog, pIO, TRUE);
					}
					else
					{
						if (pIO->iActiveItem == IO_TE_LABELEDIT)
							IOToggleIconLabel(pDialog, pIO, IO_UITEM_ICONTXT);
						iitem = sfHookSetActiveOffset + sfItemFileListUser;
						pIO->fCheckFileSelection = true;
						pIO->fSavedFSP = false;
					}
				}
			}

			// check to see if class selection changed
			if(pIO->fCheckClassSelection)
			{
				if(IOCheckClassSelection(pIO))
				{
					IOUpdateResults(pDialog, FALSE);
				
					if(pIO->dwFlags & IOF_SELECTCREATENEW && pIO->fAsIconNew)
						IOUpdateIconArea(pDialog, pIO, TRUE);
				}
				IOUpdateAllControls(pDialog, pIO);
				pIO->fCheckClassSelection = false;
			}
			break;

#ifdef _DEBUG
		case IO_BTN_HELP:
			ASSERTCOND(false);	// If you have this enabled, you must
							// intercept this in your hook
			break;
#endif // _DEBUG


		case IO_RBTN_NEW:
			IOToggleObjectSource(pDialog, pIO, IOF_SELECTCREATENEW);
			IOCheckResults(pIO);
			IOUpdateResults(pDialog, TRUE);
			iitem = sfHookSetActiveOffset + IO_UITEM_LBOXNEW;
		    pIO->fDefaultIcon = true;			
			break;

		case IO_RBTN_FROMFILE:
			IOToggleObjectSource(pDialog, pIO, IOF_SELECTCREATEFROMFILE);
			IOCheckResults(pIO);
			IOUpdateResults(pDialog, TRUE);
			iitem = sfHookSetActiveOffset + sfItemFileListUser;
		    pIO->fDefaultIcon = true;			
			break;

		case IO_CBOX_LINK:
			GetDItem(pDialog, IO_CBOX_LINK, &itype, &h, &rc);
			fCheck = !GetCtlValue((ControlHandle)h);
			SetCtlValue((ControlHandle)h, fCheck);

			if (fCheck)
				pIO->dwFlags |= IOF_CHECKLINK;
			else
				pIO->dwFlags &= ~IOF_CHECKLINK;

			IOCheckResults(pIO);
			IOUpdateResults(pDialog, TRUE);
			
			pIO->fCheckFileSelection = true;
			pIO->fForceIconUpdate = true;
						
			break;
			
		case IO_UITEM_ICON:
		case IO_TE_LABELEDIT:
		case sfItemFileListUser:
		case IO_UITEM_LBOXNEW:
			if(iitem != pIO->iActiveItem)
				iitem += sfHookSetActiveOffset;
			break;
	} // Switch

	SetPort(gpSave);

	return iitem;
}



/*
 * InsertObjectModalProc
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment InsObjectSeg
pascal Boolean
#else
#pragma code_seg("InsObjectSeg", "SWAPPABLE")
Boolean __pascal
#endif
InsertObjectModalProc(DialogPtr pDialog, EventRecord *pEvent, short *pItem, PINSERTOBJECT pIO)
{
	GrafPtr			gpSave;
	Boolean			fHooked;
	WindowPtr		pWindow;
	short			in;
	Point			ptWhere;
	char			chKey;
	short			itype;
	Handle			h;
	Rect			rc;
	ListHandle		hList;
	Cell			cell;
	Boolean			shifted;
	short 			itemHit;
	Boolean			bRetVal = FALSE;
	Boolean			fChangeOfFocus = FALSE;

	// Only continue if we're in the main dialog
	if (GetWRefCon(pDialog) != sfMainDialogRefCon)
		return false;

	GetPort(&gpSave);
	SetPort(pDialog);

	fHooked = FStandardHook((LPOLEUISTANDARD)pIO->lpOIO, pDialog, pEvent, pItem, pIO->lpOIO->lCustData);
	if (fHooked)
	{
		SetPort(gpSave);
		return true;
	}

	switch (pEvent->what)
	{
		case updateEvt:
			/* We need to set the font explicitly here as the TextEdit control
			 * has a different font which interferes the drawing of other items
			 */			
			if ((WindowPtr)pEvent->message == pDialog)
			{
				if (pIO->fDoToggle)
				{
					Rect	rTemp;

					SetOrigin((short)(pDialog->portRect.left - 10000), pDialog->portRect.top);

					ForeColor(blackColor);
					PenPat(&qd.black);

					rTemp = pDialog->portRect;
					rTemp.bottom = rTemp.top + 38;
					EraseRect(&rTemp);

					rTemp = pDialog->portRect;
					rTemp.top = (pIO->dwFlags & IOF_SELECTCREATENEW ? 1000 : 0);

					GetDItem(pDialog, IO_UITEM_RESULT, &itype, &h, &rc);
					rTemp.bottom = (pIO->dwFlags & IOF_SELECTCREATENEW ? rc.bottom : rc.bottom - 1000);

					ClipRect(&rTemp);

					pIO->fDoToggle = false;
				}

				TextFont(systemFont);
				TextSize(12);
			}			
			bRetVal = FALSE;
			break;
		
		case mouseDown:
			in = FindWindow(pEvent->where, &pWindow);
			if ((pWindow == (WindowPtr)pDialog) && (in == inDrag) && (pIO->lpOIO->lpfnHook != NULL))
			{
				DragWindow(pDialog, pEvent->where, &qd.screenBits.bounds);
				*pItem = 0;
				bRetVal = TRUE;
				break;
			}

			ptWhere = pEvent->where;
			GlobalToLocal(&ptWhere);

			itemHit = FindDItem(pDialog, ptWhere) + 1;

			// *** check if focus is leaving active object and react appropriately ***
	// case #1:  focus leaving file list box for one of icon or icon label
			if(pIO->iActiveItem == sfItemFileListUser && pIO->fAsIconFile && (itemHit == IO_UITEM_ICON || itemHit == IO_UITEM_ICONTXT)) {
				IOFileListLosingFocus(pDialog, pIO);
				fChangeOfFocus = TRUE;
			}

	// case #2:  focus leaving class list box for one of icon or icon label
			else if(pIO->iActiveItem == IO_UITEM_LBOXNEW && pIO->fAsIconNew && (itemHit == IO_UITEM_ICON || itemHit == IO_UITEM_ICONTXT)) {
				fChangeOfFocus = TRUE;
			}
	
	// case #3:  focus leaving icon for one of file list box, class list box or icon label
			else if(pIO->iActiveItem == IO_UITEM_ICON && (itemHit == sfItemFileListUser || itemHit == IO_UITEM_LBOXNEW || itemHit == IO_UITEM_ICONTXT)) {
				fChangeOfFocus = TRUE;
			}
			
	// case #4:  focus leaving icon label for one of file list box, class list box or icon
			else if(pIO->iActiveItem == IO_TE_LABELEDIT && (itemHit == sfItemFileListUser || itemHit == IO_UITEM_LBOXNEW || itemHit == IO_UITEM_ICON)) {
				IOToggleIconLabel(pDialog, pIO, IO_UITEM_ICONTXT);
				fChangeOfFocus = TRUE;
			}
			
// change of focus has been determined, now if determine if the click hit something we are managing
			switch(itemHit)
			{
				case IO_UITEM_ICON:
					if(fChangeOfFocus)
						*pItem = IO_UITEM_ICON;
					bRetVal = false;
					break;
		
				case IO_UITEM_ICONTXT:
					if(fChangeOfFocus)
					{
						IOToggleIconLabel(pDialog, pIO, IO_TE_LABELEDIT);
//						*pItem = IO_TE_LABELEDIT;
//						bRetVal = true;
					}
					else
						bRetVal = false;
					break;

				case IO_TE_LABELEDIT:
					bRetVal = FALSE;
					break;

				case IO_BTN_OK:
					GetDItem(pDialog, IO_BTN_OK, &itype, &h, &rc);
					if ((**((ControlHandle)h)).contrlHilite == 0)
					{
						if (TrackControl((ControlHandle)h, ptWhere, NULL))
						{
							*pItem = IO_BTN_CANCEL;
							pIO->fHitOK = true;
							bRetVal = true;
							break;
						}
					}
					bRetVal = false;
					break;
					
				case IO_UITEM_LBOXNEW:
					if(fChangeOfFocus)
						*pItem = IO_UITEM_LBOXNEW;
						
					hList = pIO->hListNew;

					pIO->fCheckClassSelection = TRUE;  // can't check directly, since deactivation
														// of icon label will copy TE text into label and
														// overwrite correct label
					
					if (LClick(ptWhere, pEvent->modifiers, hList))
					{
						FlashButton(pDialog, IO_BTN_OK);
						*pItem = IO_BTN_CANCEL;
						pIO->fHitOK = true;
						bRetVal = true;
						break;
					}
	
					IOUpdateAllControls(pDialog, pIO);

					*pItem = IO_UITEM_LBOXNEW;
					bRetVal = false;
					break;

				case sfItemFileListUser:
					if(fChangeOfFocus)
						*pItem = sfItemFileListUser;
								
					pIO->fCheckFileSelection = true;	
					bRetVal = false;
					break;

				case IO_BTN_OPEN:
					if (pIO->iActiveItem != sfItemFileListUser)
					{
						GetDItem(pDialog, IO_BTN_OPEN, &itype, &h, &rc);
						if (TrackControl((ControlHandle)h, ptWhere, NULL))
						{
							*pItem = IO_BTN_CANCEL;
							pIO->fHitOK = true;
							bRetVal = true;
						}
					}
					else
					{
						// it could potentially change the currrent file selection
						if(pIO->dwFlags & IOF_SELECTCREATEFROMFILE)
							pIO->fCheckFileSelection = true;	
						pIO->fHitOK = true;
						bRetVal = false;
					}
					break;
				
				case IO_UITEM_VOLUME:
				case IO_BTN_EJECT:
				case IO_BTN_DRIVE:
				case IO_UITEM_POPUP:
					// these are the controls which could potentially change the currrent file selection
					if(pIO->dwFlags & IOF_SELECTCREATEFROMFILE)
						pIO->fCheckFileSelection = true;	
					bRetVal = false;
					break;

				case IO_CBOX_DISPASICON:
					// Since this is a normal checkbox I should be able to return false
					// and rely on the dialog's default processing but when Directory
					// Assistance is enabled the default behavior of this checkbox stops
					// working so I have to track it myself.
					GetDItem(pDialog, IO_CBOX_DISPASICON, &itype, &h, &rc);
					if ((**(ControlHandle)h).contrlHilite == 0)
					{
						if (TrackControl((ControlHandle)h, ptWhere, NULL))
						{
							// if icon or label has focus, then shift the focus to the appropriate list box
							pIO->fJumpToList = (pIO->iActiveItem == IO_UITEM_ICON || pIO->iActiveItem == IO_TE_LABELEDIT);
	
							if(pIO->iActiveItem == IO_TE_LABELEDIT)
							{
								GetDItem(pDialog, IO_TE_LABELEDIT, &itype, &h, &rc);
								SetRect(&rc, (short)(rc.left - 3), (short)(rc.top - 3), (short)(rc.right + 3),
									(short)(rc.bottom + 3));
								IOToggleIconLabel(pDialog, pIO, IO_UITEM_ICONTXT);
							}
	
							if (pIO->dwFlags & IOF_CHECKDISPLAYASICON)
								pIO->dwFlags &= ~IOF_CHECKDISPLAYASICON;
							else
								pIO->dwFlags |= IOF_CHECKDISPLAYASICON;
							IOUpdateDisplayAsIcon(pDialog, pIO);
							IOCheckResults(pIO);
							IOUpdateResults(pDialog, TRUE);
						}
					}
					bRetVal = true;
					break;

				default:  // other items which we don't want to handle
					bRetVal = false;
					break;
					
			} // switch(itemHit)
		
			break;
							
		case keyDown:
		case autoKey:
			chKey = pEvent->message & charCodeMask;
			
			// case #1:  key is a TAB which signals possible change of focus
			if(chKey == TABKEY) {
				switch(pIO->iActiveItem) {
					case sfItemFileListUser:
						if (pIO->dwFlags & IOF_SELECTCREATENEW)			// handles initialization case
							*pItem = IO_UITEM_LBOXNEW;
						else if(pIO->fAsIconFile)
						{
							if(pIO->fFileSelected)
							{
								IOFileListLosingFocus(pDialog, pIO);
								*pItem = IO_UITEM_ICON;
							}
						}
						bRetVal = TRUE;
						break;
						
					case IO_UITEM_LBOXNEW:
						if (pIO->dwFlags & IOF_SELECTCREATEFROMFILE)	// handles initialization case
							*pItem = sfItemFileListUser;
						else if(pIO->fAsIconNew)
							*pItem = IO_UITEM_ICON;
						bRetVal = TRUE;
						break;
						
					case IO_UITEM_ICON:
						ASSERTCOND(pIO->dwFlags & IOF_CHECKDISPLAYASICON);
						IOToggleIconLabel(pDialog, pIO, IO_TE_LABELEDIT);
//						*pItem = IO_TE_LABELEDIT;
//						bRetVal = TRUE;
						break;
						
					case IO_TE_LABELEDIT:
						ASSERTCOND(pIO->dwFlags & IOF_CHECKDISPLAYASICON);
						if (pIO->dwFlags & IOF_SELECTCREATEFROMFILE)
						{
							*pItem = sfItemFileListUser;
							bRetVal = FALSE; // so that new highlight doesn't depend on mouse pos.
						}
						else
						{
							*pItem = IO_UITEM_LBOXNEW;
							bRetVal = TRUE;
						}
						IOToggleIconLabel(pDialog, pIO, IO_UITEM_ICONTXT);
						pIO->fCheckFileSelection = true;
						break;
						
					default:
						ASSERTCOND(0);
						bRetVal = TRUE;
						break;
				} // switch(pIO->iActiveItem)
				
			} // if(chKey == TABKEY)

			else if ((chKey == ENTERKEY) || (chKey == RETURNKEY)) {

				if ((pIO->dwFlags & IOF_SELECTCREATENEW && pIO->fObjSelected) ||
					(pIO->dwFlags & IOF_SELECTCREATEFROMFILE && pIO->fFileSelected)) {

					bRetVal = TRUE;  // we are handling the keystroke
				
					GetDItem(pDialog, IO_BTN_OK, &itype, &h, &rc);
					if ((**((ControlHandle)h)).contrlHilite == 0)
					{
						FlashButton(pDialog, IO_BTN_OK);
						*pItem = IO_BTN_CANCEL;
						pIO->fHitOK = true;
						
						break;
					}
				}			
			}
			
// case #2:  active item is text edit control, so we'll handle the keystroke
			else if(pIO->iActiveItem == IO_TE_LABELEDIT) {
				if (chKey == ESCAPEKEY)
					bRetVal = false;
				else {
					TEKey(chKey, ((DialogPeek)pDialog)->textH);
					bRetVal = true;
				}
			}

// case #2b:  active item is icon
			else if(pIO->iActiveItem == IO_UITEM_ICON) {

				bRetVal = true;

				switch (chKey) {
					case CKEY:
						if (pEvent->modifiers & cmdKey) {
							// copy icon to clipboard
							if(pIO->hIcon) {		
								HLock((Handle)pIO->hIcon);
								ZeroScrap();
								PutScrap(GetHandleSize((Handle)pIO->hIcon), 'PICT', (Ptr)(*pIO->hIcon));
								HUnlock((Handle)pIO->hIcon);
							}
						}
						break;
					
					case XKEY:
						if ((pEvent->modifiers & cmdKey) && !pIO->fDefaultIcon) {
						
							// copy icon to clipboard
							if(pIO->hIcon) {		
								HLock((Handle)pIO->hIcon);
								ZeroScrap();
								PutScrap(GetHandleSize((Handle)pIO->hIcon), 'PICT', (Ptr)(*pIO->hIcon));
								HUnlock((Handle)pIO->hIcon);
							}
							
							// switch to default icon
							pIO->fForceIconUpdate = TRUE;
							if (pIO->dwFlags & IOF_SELECTCREATENEW)
								IOCheckClassSelection(pIO);
							else if (pIO->dwFlags & IOF_SELECTCREATEFROMFILE)
								IOCheckFileSelection(pIO);
							GetDItem(pDialog, IO_UITEM_ICON, &itype, &h, &rc);
							EraseRect(&rc);
							InvalRect(&rc);
							pIO->fDefaultIcon = true;
						}
						break;
					
					case VKEY:
						if (pEvent->modifiers & cmdKey) {
							long lOffset, lPictSize;
							PicHandle hIcon;

							lPictSize = GetScrap(nil, 'PICT', &lOffset);

							if(lPictSize > 0) {
								if(pIO->hIcon) {
									KillPicture(pIO->hIcon);
								}
								hIcon = (PicHandle)NewHandle(lPictSize);
								if(hIcon) {
									GetScrap((Handle)hIcon, 'PICT', &lOffset);
									pIO->hIcon = OlePictFromIconAndLabel(hIcon, NULL, NULL);
									DisposeHandle((Handle)hIcon);
									GetDItem(pDialog, IO_UITEM_ICON, &itype, &h, &rc);
									EraseRect(&rc);
									InvalRect(&rc);
									pIO->fDefaultIcon = false;
								}
							}
						}
						break;

					case ESCAPEKEY:
						bRetVal = false;
						break;
				}
			}
		
// case #3:  active item is neither icon nor label, but Create From File is selected
			else if (pIO->dwFlags & IOF_SELECTCREATEFROMFILE) {
				pIO->fCheckFileSelection = true;	// Check if the file selection has changed
				bRetVal = FALSE;
			}
			
// default case:  active item must be class list box
			else {
				bRetVal = TRUE;  // we are handling the keystroke
				
				switch (chKey)
				{
					case PERIODKEY:
						if (!(pEvent->modifiers & cmdKey))
							break;
						// Else fall through...	

					case ESCAPEKEY:
						FlashButton(pDialog, IO_BTN_CANCEL);
						*pItem = IO_BTN_CANCEL;
						pIO->fHitOK = false;
						break;

					case UPKEY:
					case DOWNKEY:
						if ((**pIO->hListNew).dataBounds.bottom > 0)
						{
							shifted = false;
							SetPt(&cell, 0, 0);
							if (LGetSelect(true, &cell, pIO->hListNew))
							{
								if ((chKey == UPKEY && cell.v > 0) ||
									(chKey == DOWNKEY && cell.v < (**pIO->hListNew).dataBounds.bottom - 1))
								{
									LSetSelect(false, cell, pIO->hListNew);
									cell.v += (chKey == UPKEY ? -1 : 1);
									LSetSelect(true, cell, pIO->hListNew);	

									// If the selection is outside the list's view, scroll it into view
									LRect(&rc, cell, pIO->hListNew);
									if ((chKey == UPKEY && rc.top < (**pIO->hListNew).rView.top) ||
										(chKey == DOWNKEY && rc.bottom > (**pIO->hListNew).rView.bottom))
										LScroll(0, (short)(chKey == UPKEY ? -1 : 1), pIO->hListNew);
									shifted = true;
								}
							}
							else
							{
								SetPt(&cell, 0, 0);
								LSetSelect(true, cell, pIO->hListNew);
								LAutoScroll(pIO->hListNew);
								shifted = true;
							}
	
							if(IOCheckClassSelection(pIO)) {
								IOUpdateResults(pDialog, FALSE);
								IOUpdateIconArea(pDialog, pIO, TRUE);
							}
							
							if (shifted)
							{
								IOUpdateAllControls(pDialog, pIO);
							}
						}
						break;
				} // switch(chKey)
			} // else
			break;  // from case keyDown
			
		default:
			bRetVal = FALSE;
			break;

	} // switch(pEvent->what)

	if (pIO->fHitOK) {
		if (pIO->sfReply.sfFile.name[0]) {
			// save the file selected
			memcpy(&pIO->fspSave, &pIO->sfReply.sfFile, sizeof(FSSpec));
			pIO->fSavedFSP = TRUE;
		}
		if (pIO->iActiveItem == IO_TE_LABELEDIT) {
			TEHandle		hTE = ((DialogPeek)pDialog)->textH;
			CharsHandle		hIconLabel;
			unsigned long	cchLabel;
			
			hIconLabel = TEGetText(hTE);
			cchLabel = MIN((*hTE)->teLength, sizeof(pIO->szIconLabel) - 1);
			strncpy(pIO->szIconLabel, (char *)*hIconLabel, cchLabel);
			pIO->szIconLabel[cchLabel] = '\0';
		}
	}
	
		
	SetPort(gpSave);
	return bRetVal;
}


/*
 * IOFileFilterProc
 *
 * Purpose:
 *		get rid of non-insertable files in the file listbox
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment InsObjectSeg
pascal Boolean
#else
#pragma code_seg("InsObjectSeg", "SWAPPABLE")
Boolean __pascal
#endif
IOFileFilterProc(ParmBlkPtr p, PINSERTOBJECT pIO)
{
#ifndef _MSC_VER
//#pragma unused(p, pIO)
#endif

	Boolean		retVal = false;

#if LATER	
	FileParam	*pFP = (FileParam*)p;
	CLSID		clsid;
	FSSpec		fsp;

	if (FSMakeFSSpec(pFP->ioVRefNum, 0, pFP->ioNamePtr, &fsp) == noErr) {
		if (GetClassFSp(&fsp, &clsid) != NOERROR)
			retVal = true;
	}
#endif
			
	return retVal;
}


/*
 * IOCheckFileSelection
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment InsObjectSeg
#else
#pragma code_seg("InsObjectSeg", "SWAPPABLE")
#endif
Boolean IOCheckFileSelection(PINSERTOBJECT pIO)
{
	Boolean		fReplyHasFile;
	FSSpecPtr   pFSSpec;
	unsigned long 	cchLabel;
	Boolean		fFileSelectionChanged = false;
	CLSID		clsid;

// set the pIO->fFileSelected and pIO->fFolderSelected flags
	fReplyHasFile = pIO->sfReply.sfFile.name[0] && !pIO->sfReply.sfIsVolume && !pIO->sfReply.sfIsFolder;

	pIO->fFolderSelected = pIO->sfReply.sfIsFolder;

	// try to determine whether the selected file is insertable or not
	if (fReplyHasFile && (GetClassFSp(&pIO->sfReply.sfFile, &clsid) != NOERROR))
		fReplyHasFile = false;

	if(pIO->fFolderSelected || fReplyHasFile) {
		pIO->fSavedFSP = FALSE;
	}
	
	pIO->fFileSelected = fReplyHasFile ||
					((pIO->dwFlags & IOF_SELECTCREATEFROMFILE) && pIO->fAsIconFile && pIO->fSavedFSP);

// determine if the selected file has changed since the last invocation of this routine
	if(!pIO->fFileSelected) {
		fFileSelectionChanged = pIO->fHaveLastFSp ? TRUE : FALSE;
		pIO->fHaveLastFSp = FALSE;
	}
	else {
		// check if the file selection has changed
		fFileSelectionChanged = (pIO->fHaveLastFSp &&
								(memcmp(&pIO->fspLast, &pIO->sfReply.sfFile, sizeof(FSSpec)) == 0)) ?
									FALSE : TRUE;

		// save the selected file from this invocation
		memcpy(&pIO->fspLast, &pIO->sfReply.sfFile, sizeof(FSSpec));
		pIO->fHaveLastFSp = TRUE;
	}
	
// update the icon and icon label if neccessary
	if(pIO->fForceIconUpdate || (fFileSelectionChanged && pIO->dwFlags & IOF_SELECTCREATEFROMFILE)) {

		fFileSelectionChanged = TRUE;

		// deallocate the old icon
		if(pIO->hIcon) {
			KillPicture(pIO->hIcon);
			pIO->hIcon = NULL;
		}

		// clear out the old icon label
		pIO->szIconLabel[0] = '\0';
		
		// retrieve a new icon and icon label
		if(pIO->fFileSelected) {
		
			PicHandle hIconLabel = NULL;
			
			// retrieve the icon for the file selected
			pFSSpec = pIO->fSavedFSP ? &pIO->fspSave : &pIO->sfReply.sfFile;
			hIconLabel = OleGetIconOfFSp(pFSSpec, pIO->dwFlags & IOF_CHECKLINK ? TRUE : FALSE);
		
			if (hIconLabel != nil)
			{
				// retrieve the icon
				pIO->hIcon = OleUIPictExtractIcon(hIconLabel);

				// retrieve the label
				cchLabel = sizeof(pIO->szIconLabel);
				cchLabel = OleUIPictExtractLabel(hIconLabel, pIO->szIconLabel, cchLabel);
				pIO->szIconLabel[cchLabel] = '\0';

				OleUIPictIconFree(hIconLabel);
			}
		}
		
		pIO->fForceIconUpdate = FALSE;
	}
	
	if(fFileSelectionChanged) {
		IOCheckResults(pIO);
	}
		
	return fFileSelectionChanged;
}




/*
 * IOUserItemProc
 *
 * Purpose:
 *  This routine draws the different user items.
 *
 * Parameters:
 *  pWindow			WindowPtr to the dialog
 *  iitem			dialog item number
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment InsObjectSeg
pascal void
#else
#pragma code_seg("InsObjectSeg", "SWAPPABLE")
void __pascal
#endif
IOUserItemProc(DialogPtr pDialog, short iitem)
{
	GrafPtr			gpSave;
	short			itype;
	Handle			h;
	Rect			rc;
	PINSERTOBJECT	pIO;
	
	GetPort(&gpSave);
	SetPort(pDialog);

	GetDItem(pDialog, iitem, &itype, &h, &rc);

	pIO = gpIO;

	switch (iitem)
	{
		case IO_UITEM_RESULT:
		{
			Rect		rcPict;
			Rect		rcText;
			PicHandle	hPict;
			RgnHandle	hrgnClip;
			short		txFont = pDialog->txFont;
			short		txSize = pDialog->txSize;

			PenSize(1, 1);
			FrameRect(&rc);

			InsetRect(&rc, 4, 12);

			if ((pIO->dwFlags & IOF_SELECTCREATENEW && !pIO->fObjSelected) ||
				(pIO->dwFlags & IOF_SELECTCREATEFROMFILE && !pIO->fFileSelected))
			{
// 				EraseRect(&rc);		// REVIEW: how should this case be handled?
				break;
			}

			rcText = rc;

			if (pIO->nResultIcon != RESULTIMAGE_NONE)
			{
#ifdef UIDLL
            short hostResNum = SetUpOLEUIResFile();
#endif

				hPict = GetPicture(OLEUI_PICTID);

#ifdef UIDLL
            ClearOLEUIResFile(hostResNum);
#endif

				ASSERTCOND(hPict != nil);

				if (hPict != nil)
				{
					rc.top += 6;
					rc.bottom -= 6;
					rc.right = rc.left + 60;

					hrgnClip = NewRgn();
					ASSERTCOND(hrgnClip != nil);

					GetClip(hrgnClip);
					ClipRect(&rc);

					rcPict = (**hPict).picFrame;
					OffsetRect(&rcPict, rc.left, (short)(rc.top - pIO->nResultIcon * 40));

					// This is offset and clipped so only the appropriate
					// part of the image is visible.
					DrawPicture(hPict, &rcPict);

					SetClip(hrgnClip);
					DisposeRgn(hrgnClip);

					ReleaseResource((Handle)hPict);
				}
			}

			TextFont(geneva);
			TextSize(9);

			rcText.left += 62;
			rcText.right -= 4;
			TextBox(pIO->szResult, strlen(pIO->szResult), &rcText, teFlushLeft);

			TextFont(txFont);
			TextSize(txSize);
			break;
		}

		case IO_UITEM_LBOXNEW:
			if (pIO->hListNew)
				IODrawClassList(pDialog, pIO, false);
			break;

		case IO_UITEM_ICONTXT:
         IODrawIconLabel(pDialog, pIO);
			break;
			
		case IO_UITEM_ICON:
			IODrawIcon(pDialog, pIO, false);
			break;

   case IO_UITEM_OKOUTLINE:
			GetDItem(pDialog, (short)(pIO->dwFlags & IOF_SELECTCREATENEW ? IO_BTN_OK : IO_BTN_OPEN),
				&itype, &h, &rc);
			DrawDefaultBorder(pDialog, (short)IO_UITEM_OKOUTLINE,
				(Boolean)((**(ControlHandle)h).contrlHilite == 0));
			break;
	}

	SetPort(gpSave);
}



/*
 * IODrawIconLabel
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment InsObjectSeg
#else
#pragma code_seg("InsObjectSeg", "SWAPPABLE")
#endif
void IODrawIconLabel(DialogPtr pDialog, PINSERTOBJECT pIO)
{
	short		itype;
	Handle		h;
	Rect		rc;
	short		txFont;
	short		txSize;

	GetDItem(pDialog, IO_UITEM_ICONTXT, &itype, &h, &rc);

	txFont = pDialog->txFont;
	txSize = pDialog->txSize;

	TextFont(geneva);
	TextSize(9);

	TextBox(pIO->szIconLabel, strlen(pIO->szIconLabel), &rc, teCenter);

	TextFont(txFont);
	TextSize(txSize);
}


/*
 * IODrawIcon
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment InsObjectSeg
#else
#pragma code_seg("InsObjectSeg", "SWAPPABLE")
#endif
void IODrawIcon(DialogPtr pDialog, PINSERTOBJECT pIO, Boolean FocusRectOnly)
{
	Rect 		frame;
	short		itype;
	Handle		h;
	Rect		rc;
	PenState	savePenState;
	GrafPtr		gpSave;
	
	GetPort(&gpSave);
	SetPort(pDialog);

	GetDItem(pDialog, IO_UITEM_ICON, &itype, &h, &rc);
	GetPenState(&savePenState);

	if (!FocusRectOnly)
		DrawPicture(pIO->hIcon, &rc);
	
	if(pIO->iActiveItem != IO_UITEM_ICON)
	{
#ifdef dangerousPattern
		PenPat(qd.white);
#else
		PenPat(&qd.white);
#endif
		if (rc.left > 16384)	// erase focus rect even though icon is hidden
			OffsetRect(&rc, -16384, 0);
	}
		
	SetRect(&frame, (short)(rc.left - OLEUI_ICONFRAME), (short)(rc.top - OLEUI_ICONFRAME),
					(short)(rc.right + OLEUI_ICONFRAME), (short)(rc.bottom + OLEUI_ICONFRAME));
	PenSize(1, 1);
	FrameRect(&frame);
	
	SetPenState(&savePenState);

	SetPort(gpSave);
}


/*
 * IODrawClassList
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment InsObjectSeg
#else
#pragma code_seg("InsObjectSeg", "SWAPPABLE")
#endif
void IODrawClassList(DialogPtr pDialog, PINSERTOBJECT pIO, Boolean FocusRectOnly)
{
	short		itype;
	Handle		h;
	Rect		rc;
	PenState	savePenState;
	GrafPtr		gpSave;
	short		txFont;
	short		txSize;
	
	GetPort(&gpSave);
	SetPort(pDialog);

	txFont = pDialog->txFont;
	txSize = pDialog->txSize;

	TextFont(systemFont);
	TextSize(12);

	GetDItem(pDialog, IO_UITEM_LBOXNEW, &itype, &h, &rc);
	GetPenState(&savePenState);

	if (!FocusRectOnly)
	{
		LUpdate(pDialog->visRgn, pIO->hListNew);
		PenSize(1, 1);
		FrameRect(&rc);
	}

	SetRect(&rc, (short)(rc.left - OLEUI_LBOXFRAME), (short)(rc.top - OLEUI_LBOXFRAME),
				 (short)(rc.right + OLEUI_LBOXFRAME),(short)(rc.bottom + OLEUI_LBOXFRAME));
	PenSize(2, 2);
	if(pIO->iActiveItem != IO_UITEM_LBOXNEW)
#ifdef dangerousPattern
		PenPat(qd.white);
#else
		PenPat(&qd.white);
#endif

	FrameRect(&rc);
	SetPenState(&savePenState);

	TextFont(txFont);
	TextSize(txSize);

	SetPort(gpSave);
}




/*
 * IOUpdateOneControl
 *
 * Purpose:
 *  Updates the availability of the indicated control.
 *
 * Parameters:
 *  pDialog			DialogPtr to the dialog box.
 *  iitem			short which identifies the control to update
 *  fAvail			Boolean indicating new availability status
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment InsObjectSeg
#else
#pragma code_seg("InsObjectSeg", "SWAPPABLE")
#endif
void IOUpdateOneControl(DialogPtr pDialog, short iitem, Boolean fAvail)
{
	short		itype;
	Handle		h;
	Rect		rc;

	GetDItem(pDialog, iitem, &itype, &h, &rc);

	if ((**(ControlHandle)h).contrlHilite == (fAvail ? 0 : 255))
		return;

	HiliteControl((ControlHandle)h, (short)(fAvail ? 0 : 255));

	if (iitem == IO_BTN_OPEN || iitem == IO_BTN_OK)
		DrawDefaultBorder(pDialog, IO_UITEM_OKOUTLINE, fAvail);
}


/*
 * IOUpdateAllControls
 *
 * Purpose:
 *  Updates the availability of all the controls in our dialog box.
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment InsObjectSeg
#else
#pragma code_seg("InsObjectSeg", "SWAPPABLE")
#endif
void IOUpdateAllControls(DialogPtr pDialog, PINSERTOBJECT pIO)
{
	Boolean fShow;
	
	IOUpdateOneControl(pDialog, IO_BTN_OK, pIO->fObjSelected);
	IOUpdateOneControl(pDialog, IO_BTN_OPEN, pIO->fObjSelected);
	IOUpdateDisplayAsIcon(pDialog, pIO);

	fShow = (pIO->dwFlags & IOF_SELECTCREATENEW && pIO->fObjSelected) ||
			(pIO->dwFlags & IOF_SELECTCREATEFROMFILE && pIO->fFileSelected);
			
	IOUpdateOneControl(pDialog, IO_CBOX_LINK, fShow);
}


/*
 * IOCheckResults
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment InsObjectSeg
#else
#pragma code_seg("InsObjectSeg", "SWAPPABLE")
#endif
void IOCheckResults(PINSERTOBJECT pIO)
{
	Boolean			fAsIcon;
	short			iString;
	short			iImage;
	char			szString[256];

	fAsIcon = (pIO->dwFlags & IOF_SELECTCREATENEW) ? pIO->fAsIconNew : pIO->fAsIconFile;

	if (pIO->dwFlags & IOF_SELECTCREATENEW)
	{
		iString = fAsIcon ? IDS_IORESULTNEWICON : IDS_IORESULTNEW;
		iImage  = fAsIcon ? RESULTIMAGE_EMBEDICON : RESULTIMAGE_EMBED;
	}

	if (pIO->dwFlags & IOF_SELECTCREATEFROMFILE)
	{
		// Pay attention to Link checkbox
		if (pIO->dwFlags & IOF_CHECKLINK)
		{
			iString = fAsIcon ? IDS_IORESULTLINKFILEICON : IDS_IORESULTLINKFILE;
			iImage  = fAsIcon ? RESULTIMAGE_LINKICON : RESULTIMAGE_LINK;
		}
		else
		{
			iString = fAsIcon ? IDS_IORESULTFROMFILEICON : IDS_IORESULTFROMFILE;
			iImage  = fAsIcon ? RESULTIMAGE_EMBEDICON : RESULTIMAGE_EMBED;
		}
	}

	// Default is an empty string.
	pIO->szResult[0] = 0;

	GetIndString((unsigned char *)pIO->szResult, DIDInsertObject, iString);
	p2cstr((unsigned char *)pIO->szResult);

	if (pIO->dwFlags & IOF_SELECTCREATENEW)
	{
		strcpy(szString, pIO->szResult);
		sprintf(pIO->szResult, szString, pIO->szClass);
	}

	pIO->nResultIcon = iImage;
}
	

/*
 * IOUpdateResults
 *
 * Purpose:
 *  Centralizes setting of the Result in the Insert Object
 *  dialog.  Handles loading the appropriate string from the application's
 *  resources and setting the text and showing the proper icon.
 *
 * Parameters:
 *  pDialog			DialogPtr of the dialog box so we can access controls.
 *  pIO             PINSERTOBJECT in which we assume that the
 *                  current radiobutton and Display as Icon selections
 *                  are set.  We use the state of those variables to
 *                  determine which string we use.
 *
 * Return Value:
 *  None
 */

#ifndef _MSC_VER
#pragma segment InsObjectSeg
#else
#pragma code_seg("InsObjectSeg", "SWAPPABLE")
#endif
void IOUpdateResults(DialogPtr pDialog, Boolean fErase)
{
	short			itype;
	Handle			h;
	Rect			rc;

	GetDItem(pDialog, IO_UITEM_RESULT, &itype, &h, &rc);
	InsetRect(&rc, 4, 8);
	if(fErase)
		EraseRect(&rc);
 	InvalRect(&rc);			// Force a redraw of the result area.
}




/*
 * IOUpdateIconArea
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment InsObjectSeg
#else
#pragma code_seg("InsObjectSeg", "SWAPPABLE")
#endif
void IOUpdateIconArea(DialogPtr pDialog, PINSERTOBJECT pIO, Boolean fErase)
{
	short			itype;
	Handle			h;
	Rect			rc;

	if((pIO->dwFlags & IOF_SELECTCREATENEW && pIO->fObjSelected && pIO->fAsIconNew) ||
		(pIO->dwFlags & IOF_SELECTCREATEFROMFILE && pIO->fAsIconFile)) {
		// update the icon area
		GetDItem(pDialog, IO_UITEM_ICON, &itype, &h, &rc);
		if(fErase)
			EraseRect(&rc);
		InvalRect(&rc);
		
		// update the icon label
		if(pIO->iActiveItem != IO_TE_LABELEDIT) {
			GetDItem(pDialog, IO_UITEM_ICONTXT, &itype, &h, &rc);
			if(fErase)
				EraseRect(&rc);
			InvalRect(&rc);
		}
	}
}




/*
 * IOToggleObjectSource
 *
 * Purpose:
 *  Handles enabling, disabling, showing, and flag manipulation when the
 *  user changes between Create New, Insert File, and Link File in the
 *  Insert Object dialog.
 *
 * Parameters:
 *  pDialog			DialogPtr of the dialog
 *  pIO             PINSERTOBJECT pointing to the dialog structure
 *  dwOption        long flag indicating the option just selected:
 *                  IOF_SELECTCREATENEW or IOF_SELECTCREATEFROMFILE
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment InsObjectSeg
#else
#pragma code_seg("InsObjectSeg", "SWAPPABLE")
#endif
void IOToggleObjectSource(DialogPtr pDialog, PINSERTOBJECT pIO, unsigned int dwOption)
{
	Boolean		fTemp;
	short		itype;
	Handle		h;
	Rect		rc;
	short		iitem;
	
	// Skip all of this if we're already selected.
	if (pIO->dwFlags & dwOption)
		return;

	// 1. Change the Display As Icon checked state to reflect the
	//   selection for this option, stored in the fAsIcon flags.
	fTemp = (IOF_SELECTCREATENEW == dwOption) ? pIO->fAsIconNew : pIO->fAsIconFile;

	// 3. Clear out existing flags selection and set the new one
	pIO->dwFlags = (pIO->dwFlags & ~(IOF_SELECTCREATENEW | IOF_SELECTCREATEFROMFILE)) | dwOption;

	// 4. Switch between Object Type listbox on Create New and
	//   file open controls on Create From File.
	if (pIO->dwFlags & IOF_SELECTCREATENEW)
	{
		// We define the origin's top relative to the portRect in case
		// Directory Assistance is loaded. Since DA doesn't bother to
		// disable itself when CustomGetFile is called and since it screws
		// with the origin we have to take that into account.
		if (pDialog->portRect.left == 0)
			SetOrigin(1000, (short)(pDialog->portRect.top + 1000));

		// We also have to clip out the top and bottom of the dialog
		// so that Directory Assistance doesn't draw when we don't
		// want it to.
		{
			Rect	rTemp;

			rTemp = pDialog->portRect;
			rTemp.top = 1000;

			GetDItem(pDialog, IO_UITEM_RESULT, &itype, &h, &rc);
			rTemp.bottom = rc.bottom + 1000;

			ClipRect(&rTemp);
		}

		for (iitem=IO_BTN_OPEN; iitem<=IO_TEXT_CREATE; iitem++)
		{
			if ((iitem == IO_BTN_OPEN) ||
				(iitem >= IO_UITEM_VOLUME && iitem <= IO_BTN_OK) ||
				(iitem == IO_UITEM_LBOXNEW) || (iitem == IO_UITEM_OKOUTLINE) ||
				(iitem == IO_CBOX_LINK) || (iitem == IO_TEXT_TYPE) ||
				(iitem == IO_UITEM_ICONTXT))
				continue;
			GetDItem(pDialog, iitem, &itype, &h, &rc);
			OffsetRect(&rc, 1000, 1000);
			SetDItem(pDialog, iitem, itype, h, &rc);
			if ((itype & ~itemDisable) >= ctrlItem && (itype & ~itemDisable) <= ctrlItem+resCtrl)
			{
				rc = (**((ControlHandle)h)).contrlRect;
				MoveControl((ControlHandle)h, (short)(rc.left+1000), (short)(rc.top+1000));
			}
		}

		// Link is always hidden if IOF_DISABLELINK is set.
		if (!(IOF_DISABLELINK & pIO->dwFlags))
			HideDItem(pDialog, IO_CBOX_LINK);

		CheckRadioButton(pDialog, IO_RBTN_NEW, IO_RBTN_FROMFILE, IO_RBTN_NEW);

		pIO->fForceIconUpdate = TRUE;
		if (IOCheckClassSelection(pIO)) {
			IOUpdateAllControls(pDialog, pIO);
			IOUpdateResults(pDialog, TRUE);
			IOUpdateIconArea(pDialog, pIO, TRUE);
		}
	}
	else
	{
		// We define the origin's top relative to the portRect in case
		// Directory Assistance is loaded. Since DA doesn't bother to
		// disable itself when CustomGetFile is called and since it screws
		// with the origin we have to take that into account.
		if (pDialog->portRect.left == 1000)
			SetOrigin(0, (short)(pDialog->portRect.top - 1000));

		// We also have to clip out the top and bottom of the dialog
		// so that Directory Assistance doesn't draw when we don't
		// want it to.
		{
			Rect	rTemp;

			rTemp = pDialog->portRect;
			rTemp.top = 0;

			GetDItem(pDialog, IO_UITEM_RESULT, &itype, &h, &rc);
			rTemp.bottom = rc.bottom - 1000;

			ClipRect(&rTemp);
		}

		for (iitem=IO_BTN_OPEN; iitem<=IO_TEXT_CREATE; iitem++)
		{
			if ((iitem == IO_BTN_OPEN) ||
				(iitem >= IO_UITEM_VOLUME && iitem <= IO_BTN_OK) ||
				(iitem == IO_UITEM_LBOXNEW) || (iitem == IO_UITEM_OKOUTLINE) ||
				(iitem == IO_CBOX_LINK) || (iitem == IO_TEXT_TYPE) ||
				(iitem == IO_UITEM_ICONTXT))
				continue;
			GetDItem(pDialog, iitem, &itype, &h, &rc);
			OffsetRect(&rc, -1000, -1000);
			SetDItem(pDialog, iitem, itype, h, &rc);
			if ((itype & ~itemDisable) >= ctrlItem && (itype & ~itemDisable) <= ctrlItem+resCtrl)
			{
				rc = (**((ControlHandle)h)).contrlRect;
				MoveControl((ControlHandle)h, (short)(rc.left-1000), (short)(rc.top-1000));
			}
		}

		// Link is always hidden if IOF_DISABLELINK is set.
		if (!(IOF_DISABLELINK & pIO->dwFlags))
			ShowDItem(pDialog, IO_CBOX_LINK);

		CheckRadioButton(pDialog, IO_RBTN_NEW, IO_RBTN_FROMFILE, IO_RBTN_FROMFILE);

		pIO->fCheckFileSelection = TRUE;
		pIO->fForceIconUpdate = TRUE;
	}

	// 2. Show or hide the icon controls.
	if (fTemp)
		pIO->dwFlags |= IOF_CHECKDISPLAYASICON;
	else
		pIO->dwFlags &= ~IOF_CHECKDISPLAYASICON;
	
	IOUpdateDisplayAsIcon(pDialog, pIO);

	// 5. Force an update which toggles between the object list and file controls
	GetDItem(pDialog, IO_UITEM_LBOXNEW, &itype, &h, &rc);
	if (pIO->dwFlags & IOF_SELECTCREATEFROMFILE)
		OffsetRect(&rc, -1000, -1000);
	rc.top -= 16;
	SetRect(&rc, (short)(rc.left - OLEUI_LBOXFRAME), (short)(rc.top - OLEUI_LBOXFRAME),
				 (short)(rc.right + OLEUI_LBOXFRAME),(short)(rc.bottom + OLEUI_LBOXFRAME));
	EraseRect(&rc);
	InvalRect(&rc);

	// 6. Force update of OK/Open button
	GetDItem(pDialog, IO_UITEM_OKOUTLINE , &itype, &h, &rc);
	if (pIO->dwFlags & IOF_SELECTCREATEFROMFILE)
		OffsetRect(&rc, -1000, -1000);
	InvalRect(&rc);

	// 7. Make sure icon label text edit control is hidden
	IOToggleIconLabel(pDialog, pIO, IO_UITEM_ICONTXT);

	// 8. Update controls
	IOUpdateAllControls(pDialog, pIO);
}


/*
 * IOUpdateDisplayAsIcon
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment InsObjectSeg
#else
#pragma code_seg("InsObjectSeg", "SWAPPABLE")
#endif
void IOUpdateDisplayAsIcon(DialogPtr pDialog, PINSERTOBJECT pIO)
{
	short		fCheck;
	short		itype;
	Handle		h;
	Rect		rc;
	Boolean		fShow;
	
	fCheck = (pIO->dwFlags & IOF_CHECKDISPLAYASICON) ? 1 : 0;

	// Update the current value in the control.
	GetDItem(pDialog, IO_CBOX_DISPASICON, &itype, &h, &rc);
	SetCtlValue((ControlHandle)h, fCheck);

	// Store the current value in the appropriate fAsIcon flag.
	if (pIO->dwFlags & IOF_SELECTCREATENEW)
		pIO->fAsIconNew  = fCheck;
	else
		pIO->fAsIconFile = fCheck;
	
	fShow = (pIO->dwFlags & IOF_SELECTCREATENEW && pIO->fObjSelected) ||
			(pIO->dwFlags & IOF_SELECTCREATEFROMFILE && pIO->fFileSelected);
			
	IOUpdateOneControl(pDialog, IO_CBOX_DISPASICON, fShow);

	// Show or hide the controls as appropriate.
	if (fCheck && fShow)
	{
		ShowDItem(pDialog, IO_UITEM_ICON);
		ShowDItem(pDialog, IO_UITEM_ICONTXT);
	}
	else
	{
		HideDItem(pDialog, IO_UITEM_ICON);
		HideDItem(pDialog, IO_UITEM_ICONTXT);
	}
}


/*
 * IOActivateProc
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment InsObjectSeg
pascal void
#else
#pragma code_seg("InsObjectSeg", "SWAPPABLE")
void __pascal
#endif
IOActivateProc(DialogPtr pDialog, short iitem, Boolean fActivating, PINSERTOBJECT pIO)
{
	GrafPtr			gpSave;
	CharsHandle 	hIconLabel;
	unsigned long	cchLabel;

	GetPort(&gpSave);
	SetPort(pDialog);

	// update the active item field of INSERTOBJECT structure
	if(fActivating)
		pIO->iActiveItem = iitem;
	else if (iitem == pIO->iActiveItem)
		pIO->iActiveItem = 0;		// The active item is now deactivated

	// redraw the focus rect of the affected item
	switch (iitem) {
		case IO_UITEM_LBOXNEW:
			IODrawClassList(pDialog, pIO, true);
			break;
			
		case IO_UITEM_ICON:
			IODrawIcon(pDialog, pIO, true);
			break;
			
		case IO_UITEM_ICONTXT:
			IODrawIconLabel(pDialog, pIO);
			break;
	}
			
	// case:  (de)activation for icon text box
	if(iitem == IO_TE_LABELEDIT)
	{
		TEHandle	hTE = ((DialogPeek)pDialog)->textH;
			
		if(fActivating)
		{
			pIO->fLabelEditActive = TRUE;
			(*hTE)->txFont = geneva;
			(*hTE)->txSize = 9;
			TESetText(pIO->szIconLabel, strlen(pIO->szIconLabel), hTE);
			TESetSelect(0, 32767, hTE);			
			TEActivate(hTE);
		}
		else
		{
			pIO->fLabelEditActive = FALSE;
			TEDeactivate(hTE);
			hIconLabel = TEGetText(hTE);
			cchLabel = MIN((*hTE)->teLength, sizeof(pIO->szIconLabel) - 1);
			strncpy(pIO->szIconLabel, (char *)*hIconLabel, cchLabel);
			pIO->szIconLabel[cchLabel] = '\0';
		}
	}

	SetPort(gpSave);
}


/*
 * IOFileListLosingFocus
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment InsObjectSeg
#else
#pragma code_seg("InsObjectSeg", "SWAPPABLE")
#endif
void IOFileListLosingFocus(DialogPtr pDialog, PINSERTOBJECT pIO)
{
#ifndef _MSC_VER
#pragma unused(pDialog)
#endif
	
	// save the FSSpec from the file list box so that an icon can be retrieved for the previously highlighted filename
	if(!pIO->fFolderSelected)
	{
		memcpy(&pIO->fspSave, &pIO->sfReply.sfFile, sizeof(FSSpec));
		pIO->fSavedFSP = TRUE;
	}

#ifndef __powerc

#ifndef _MSC_VER
	// temporarily patch LSetSelect so the next call is skipped
	gfpOldPack0 = GetToolTrapAddress(_Pack0);
	SetToolTrapAddress((UniversalProcPtr)LSetSelectPatch, _Pack0);
#endif

#else
      {
         if (NULL == gThe68KPatch )
         {
            #ifdef UIDLL
            short hostResNum = SetUpOLEUIResFile();
            #endif

            gThe68KPatch = GetResource('PROC', 10042);
            if (NULL == gThe68KPatch || noErr != ResError())
            {

               #ifdef UIDLL
               ClearOLEUIResFile(hostResNum);
               #endif
               return;
            }

            DetachResource(gThe68KPatch);
            #ifdef UIDLL
            ClearOLEUIResFile(hostResNum);
            #endif
            MoveHHi(gThe68KPatch);
            HLock(gThe68KPatch);
         }

         if (NULL == gUpp68KPatch)
         {
            gUpp68KPatch =  NewRoutineDescriptor((ProcPtr)*gThe68KPatch,
                              uppPack0PatchProcInfo, kM68kISA);

            if (NULL == gUpp68KPatch)
            {
               if (NULL != gThe68KPatch)
               {
                  HUnlock(gThe68KPatch);
                  DisposHandle(gThe68KPatch);
                  gThe68KPatch = NULL;
               }

               return;
            }
         }

         CallUniversalProc(gUpp68KPatch, uppPack0PatchProcInfo);

      }

#endif // __powerc

   return;

}




/*
 * IOToggleIconLabel
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment InsObjectSeg
#else
#pragma code_seg("InsObjectSeg", "SWAPPABLE")
#endif
void IOToggleIconLabel(DialogPtr pDialog, PINSERTOBJECT pIO, short item)
// This procedure assumes that the display as icon checkbox is checked
{
	short		itype;
	Handle		h;
	Rect		rc;

	// ASSERTCOND(pIO->dwFlags & IOF_CHECKDISPLAYASICON);
	ASSERTCOND((item == IO_TE_LABELEDIT) || (item == IO_UITEM_ICONTXT));

	if(pIO->fLabelEditInFileView) {  // label edit control is in file list window
		if(((pIO->dwFlags & IOF_SELECTCREATEFROMFILE) && (item == IO_UITEM_ICONTXT)) ||
			((pIO->dwFlags & IOF_SELECTCREATENEW) && (item == IO_TE_LABELEDIT))) {

			// shift the icon text box to window at 0, 0
			GetDItem(pDialog, IO_UITEM_ICONTXT, &itype, &h, &rc);
			OffsetRect(&rc, -1000, -1000);
			SetDItem(pDialog, IO_UITEM_ICONTXT, itype, h, &rc);
			
			// shift the text edit box to window at 1000, 1000
			GetDItem(pDialog, IO_TE_LABELEDIT, &itype, &h, &rc);
			OffsetRect(&rc, 1000, 1000);
			SetDItem(pDialog, IO_TE_LABELEDIT, itype, h, &rc);

			pIO->fLabelEditInFileView = FALSE;
		}
	}
	else {  // label edit control is in class list window
		if(((pIO->dwFlags & IOF_SELECTCREATEFROMFILE) && (item == IO_TE_LABELEDIT)) ||
			((pIO->dwFlags & IOF_SELECTCREATENEW) && (item == IO_UITEM_ICONTXT))) {

			// shift the icon text box to window at 1000, 1000
			GetDItem(pDialog, IO_UITEM_ICONTXT, &itype, &h, &rc);
			OffsetRect(&rc, 1000, 1000);
			SetDItem(pDialog, IO_UITEM_ICONTXT, itype, h, &rc);
			
			// shift the text edit box to window at 0, 0
			GetDItem(pDialog, IO_TE_LABELEDIT, &itype, &h, &rc);
			OffsetRect(&rc, -1000, -1000);
			SetDItem(pDialog, IO_TE_LABELEDIT, itype, h, &rc);

			pIO->fLabelEditInFileView = TRUE;
		}
	}
			
	if ((pIO->fLabelEditInFileView && (pIO->dwFlags & IOF_SELECTCREATEFROMFILE)) ||
		(!pIO->fLabelEditInFileView && (pIO->dwFlags & IOF_SELECTCREATENEW)))
		TEActivate(((DialogPeek)pDialog)->textH);
	else
		TEDeactivate(((DialogPeek)pDialog)->textH);

	GetDItem(pDialog, IO_TE_LABELEDIT, &itype, &h, &rc);
	SetRect(&rc, (short)(rc.left - 3), (short)(rc.top - 3), (short)(rc.right + 3), (short)(rc.bottom + 3));
	
	if(pIO->dwFlags & IOF_SELECTCREATEFROMFILE && !pIO->fLabelEditInFileView)
		OffsetRect(&rc, -1000, -1000);
	else if(pIO->dwFlags & IOF_SELECTCREATENEW && pIO->fLabelEditInFileView)
		OffsetRect(&rc, 1000, 1000);

	// only need to erase the rect when the smaller ICONTXT item is being placed in view
	if(item == IO_UITEM_ICONTXT)
		EraseRect(&rc);

	InvalRect(&rc);
}




/*
 * IOCheckClassSelection
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment InsObjectSeg
#else
#pragma code_seg("InsObjectSeg", "SWAPPABLE")
#endif
Boolean IOCheckClassSelection(PINSERTOBJECT pIO)
{
	Cell			cell;
	short 			cch;
	unsigned long 	bufSize;
	Boolean			fClassSelectionChanged;
	char 			szClass[OLEUI_CCHKEYMAX];
	PicHandle		hPictIconLabel = NULL;

	if (!pIO->hListNew)
		return false;

	pIO->fObjSelected = FLGetSelectedCell(pIO->hListNew, &cell);
	
	// update the index of the selected cell
	pIO->nOldIndex = pIO->nSelectedIndex;
	pIO->nSelectedIndex = cell.v;

	if(pIO->fObjSelected) {
		// get the list cell just selected
		cch = sizeof(pIO->szClass);
		LGetCell(pIO->szClass, &cch, cell, pIO->hListNew);
		pIO->szClass[cch] = 0;

		// determine if the selection has changed
		fClassSelectionChanged = (pIO->nOldIndex != pIO->nSelectedIndex);

		if(pIO->fForceIconUpdate || (fClassSelectionChanged && pIO->dwFlags & IOF_SELECTCREATENEW)) {

			if(fClassSelectionChanged) {
				IOCheckResults(pIO);
			}
			
			fClassSelectionChanged = TRUE;

			// deallocate the old icon
			if(pIO->hIcon) {
				KillPicture(pIO->hIcon);
				pIO->hIcon = NULL;
			}
			
			// retrieve icon for selected class
			SetPt(&cell, 1, (short)pIO->nSelectedIndex);
			cch = sizeof(szClass);
			LGetCell(szClass, &cch, cell, pIO->hListNew);

			hPictIconLabel = OleGetIconOfClass((REFCLSID)szClass, NULL, true);
			ASSERTCOND(hPictIconLabel != nil);
			
			// retieve the icon for the class selected
			pIO->hIcon = OleUIPictExtractIcon(hPictIconLabel);						

			// retrieve the icon label for the class selected
			bufSize = sizeof(pIO->szIconLabel);
			bufSize = OleUIPictExtractLabel(hPictIconLabel, pIO->szIconLabel, bufSize);
			pIO->szIconLabel[bufSize] = '\0';
			
			if (hPictIconLabel)
				OleUIPictIconFree(hPictIconLabel);
			
			pIO->fForceIconUpdate = FALSE;
		}
	}
	else {
		// deallocate the old icon
		if(pIO->hIcon) {
			KillPicture(pIO->hIcon);
			pIO->hIcon = NULL;
		}
		
		pIO->szIconLabel[0] = '\0';
		pIO->nSelectedIndex = -1;
		fClassSelectionChanged = (pIO->nOldIndex != pIO->nSelectedIndex);
	}
	
	return fClassSelectionChanged;
}




#ifndef _MSC_VER
pascal GDHandle
#else
GDHandle __pascal
#endif
GetMaxDevicePatch(Rect *globalRect)
{
	return GetMainDevice();
}


#if !defined (__powerc) && (defined(THINK_C) || defined(__MWERKS__))
#if defined(__MWERKS__)
asm void LSetSelectPatch(void)
{
#else
void LSetSelectPatch(void)
{
	asm
	{
#endif
		cmpi.w		#0x5c, 4(sp)
		#if defined(THINK_C)
		bne.s		@JmpOldPack0
		#else
		bne.s		JmpOldPack0
		#endif

		// LSetSelect was called so restore trap and return
		move.w		#0x01e7, d0		// LSetSelect trap number
		move.l		gfpOldPack0, a0
#if defined(__MWERKS__)
		_SetToolTrapAddress
#else
		SetToolTrapAddress
#endif

		move.l		(a7)+, a0
		lea			12(a7), a7
		jmp			(a0)

JmpOldPack0:
		// Pass other _Pack0 calls to original trap
		move.l		gfpOldPack0, a0
		jmp			(a0)
#if defined(THINK_C)
	}
#endif
}
#endif // !defined (__powerc) && (defined(THINK_C) || defined(__MWERKS__))
