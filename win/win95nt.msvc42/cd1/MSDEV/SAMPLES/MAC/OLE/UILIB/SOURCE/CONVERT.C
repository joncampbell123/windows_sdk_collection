/*
 * CONVERT.C
 *
 * Implements the Activate function which invokes the complete
 * Convert dialog.
 *
 * Copyright (c)1992-1994 Microsoft Corporation, All Right Reserved
 */


#if defined(USEHEADER)
#include "OleHeaders.h"
#endif

#if defined(__MWERKS__) && !defined(__powerc)
#pragma pointers_in_D0
#endif

#if defined(_MSC_VER) && defined(__powerc)
#include <msvcmac.h>
#endif



#ifndef _MSC_VER
#ifndef THINK_C
#include <Strings.h>
#endif
#endif

#include <Events.h>
#include <Fonts.h>
#include <Menus.h>
#include <Script.h>
#include <stdio.h>
#include <string.h>
#include <Scrap.h>

#ifndef _MSC_VER
#include <AppleEvents.h>
#include <Processes.h>
#include <Resources.h>
#include <ToolUtils.h>
#else
#include <AppleEve.h>
#include <Processe.h>
#include <Resource.h>
#include <ToolUtil.h>
#endif

#include <ole2.h>

#include "ole2ui.h"
#include "common.h"
#include "uidebug.h"
#include "convert.h"


OLEDBGDATA


#ifdef __powerc

RoutineDescriptor gRDConvertDialogProc =
   BUILD_ROUTINE_DESCRIPTOR(uppModalFilterProcInfo,ConvertDialogProc);
ModalFilterUPP	gConvertDialogProc = &gRDConvertDialogProc;

RoutineDescriptor gRDCVUserItemProc =
   BUILD_ROUTINE_DESCRIPTOR(uppUserItemProcInfo,CVUserItemProc);
UserItemUPP gCVUserItemProc = &gRDCVUserItemProc;

#endif


/*
 * OleUIConvert
 *
 * Purpose:
 *  Invokes the standard OLE Change Type dialog box allowing the user
 *  to change the type of the single specified object, or change the
 *  type of all OLE objects of a specified type.
 *
 * Parameters:
 *  lpOCV           LPOLEUIConvert pointing to the in-out structure
 *                  for this dialog.
 *
 * Return Value:
 *  unsigned int    One of the following codes, indicating success or error:
 *                      OLEUI_SUCCESS           Success
 *                      OLEUI_ERR_STRUCTSIZE    The dwStructSize value is wrong
 */

#ifndef _MSC_VER
#pragma segment ConvertSeg
#else
#pragma code_seg("ConvertSeg","SWAPPABLE")
#endif
STDAPI_(unsigned int) OleUIConvert(LPOLEUICONVERT lpOCV)
{
	short 			nItem;
	DialogPtr		pDialog;
	PCONVERT		pCV;
	ListHandle		hList;
	short			cch;
	Cell			cell;
	char			szCLSID[OLEUI_CCHKEYMAX];	
	unsigned int	uRet;
	GrafPtr			gpSave;

	GetPort(&gpSave);

	InitCursor();

	//Validate lpOCV structure
	uRet = UStandardValidation((LPOLEUISTANDARD)lpOCV, sizeof(OLEUICONVERT));

	if (uRet != OLEUI_SUCCESS) {
		SetPort(gpSave);
		return uRet;
	}

	// Validate structure members passed in.

	if (!IsValidClassID(lpOCV->clsid))
		uRet = OLEUI_CTERR_CLASSIDINVALID;

	if ((lpOCV->dwFlags & CF_SETCONVERTDEFAULT) && (!IsValidClassID(lpOCV->clsidConvertDefault)))
		uRet = OLEUI_CTERR_CLASSIDINVALID;

	if ((lpOCV->dwFlags & CF_SETACTIVATEDEFAULT) && (!IsValidClassID(lpOCV->clsidActivateDefault)))
		uRet = OLEUI_CTERR_CLASSIDINVALID;

	if ((lpOCV->dvAspect != DVASPECT_ICON) && (lpOCV->dvAspect != DVASPECT_CONTENT))
		uRet = OLEUI_CTERR_DVASPECTINVALID;

	if (uRet != OLEUI_SUCCESS) {
		SetPort(gpSave);
		return uRet;
	}

	pDialog = NULL;
	uRet = UStandardInvocation((LPOLEUISTANDARD)lpOCV, &pDialog, DIDConvert);

	if (uRet != OLEUI_SUCCESS) {
		SetPort(gpSave);
		return uRet;
	}

	uRet = UConvertInit(pDialog, lpOCV);

	if (uRet != OLEUI_SUCCESS)
	{
		ConvertCleanup(pDialog);
		SetPort(gpSave);
		return uRet;
	}

	ShowWindow((WindowPtr)pDialog);
	pCV = (PCONVERT)GetWRefCon(pDialog);

	do
	{
#ifndef __powerc
		ModalDialog(ConvertDialogProc, &nItem);
#else

#ifdef UIDLL
      short hostResNum = SetUpOLEUIResFile();
#endif

		ModalDialog(gConvertDialogProc, &nItem);

#ifdef UIDLL
      ClearOLEUIResFile(hostResNum);
#endif

#endif

		switch (nItem)
		{
#ifdef _DEBUG
			case CV_BTN_HELP:
				ASSERTCOND(false);	//If you have this enabled, you must
								//intercept this in your hook
				break;
#endif //_DEBUG

			case CV_CBOX_DISPASICON:
				CVSetActiveItem(pDialog, pCV, CV_UITEM_LBOX);
				pCV->dvAspect = (pCV->dvAspect == DVASPECT_ICON) ? DVASPECT_CONTENT : DVASPECT_ICON;
				CVUpdateDisplayAsIcon(pDialog, pCV);
				break;

			case CV_RBTN_CONVERT:
				ToggleConvertType(pDialog, pCV, CF_SELECTCONVERTTO);
				ShowDItem(pDialog, CV_CBOX_DISPASICON);
				break;

			case CV_RBTN_ACTIVATE:
				CVSetActiveItem(pDialog, pCV, CV_UITEM_LBOX);
				ToggleConvertType(pDialog, pCV, CF_SELECTACTIVATEAS);
				pCV->dvAspect = DVASPECT_CONTENT;
				CVUpdateDisplayAsIcon(pDialog, pCV);
				HideDItem(pDialog, CV_CBOX_DISPASICON);
				break;
		}
	} while (nItem != CV_BTN_OK && nItem != CV_BTN_CANCEL);

	// This is done is update the icon label in case the text edit was in focus
	// when the dialog is closed
	CVSetActiveItem(pDialog, pCV, CV_UITEM_LBOX);
	
	/* Set OUT parameters */

	//
	// Set output flags to current ones
	//
	lpOCV->dwFlags = pCV->dwFlags;

	// Update the dvAspect and fObjectsIconChanged members
	// as appropriate.
	//
	if (pCV->dwFlags & CF_SELECTACTIVATEAS)
	{
	  // DON'T update aspect if activate as was selected.
	  lpOCV->fObjectsIconChanged = FALSE;
	}
	else
	  lpOCV->dvAspect = pCV->dvAspect;
	
	//
	// Get the new clsid
	//
	hList = (pCV->dwFlags & CF_SELECTCONVERTTO ? pCV->hListConvert : pCV->hListActivate);

	FLGetSelectedCell(hList, &cell);
	cell.h++;

	cch = sizeof(szCLSID);
	LGetCell(szCLSID, &cch, cell, hList);
	CLSIDFromString(szCLSID, &lpOCV->clsidNew);

	//
	// Get the hMetaPict (if display as icon is checked)
	//
	if (DVASPECT_ICON == pCV->dvAspect)
		lpOCV->hMetaPict = OlePictFromIconAndLabel(pCV->hIcon, pCV->szIconLabel, NULL);
	else
		lpOCV->hMetaPict = NULL;

	ConvertCleanup(pDialog);

	uRet = (CV_BTN_OK == nItem ? OLEUI_OK : OLEUI_CANCEL);

	SetPort(gpSave);
	return uRet;
}




/*
 * UConvertInit
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment ConvertSeg
#else
#pragma code_seg("ConvertSeg","SWAPPABLE")
#endif
unsigned int UConvertInit(DialogPtr pDialog, LPOLEUICONVERT lpOCV)
{
	PCONVERT		pCV;
    LPMALLOC		pIMalloc;
	Cell			cell;
	long			cb;
	HKEY			hKey;
	Boolean			fFound;
	short			itype;
	Handle			h;
	Rect			rc;
	unsigned int	uRet;
	long			lRet;

	//1. Copy the structure at lpOCV
	pCV = (PCONVERT)PvStandardInit(pDialog, sizeof(CONVERT));

	//PvStandardInit failed
	if (NULL == pCV)
		return OLEUI_ERR_GLOBALMEMALLOC;

	ASSERTCOND(pCV == (PCONVERT)GetWRefCon(pDialog));

	//2. Initialize all the user items in our dialog box.
	uRet = CVUInitUserItems(pDialog);

	if (uRet != OLEUI_SUCCESS)
		return uRet;

    pCV->fDefaultIcon = false;

	//3. Save the original pointer and copy necessary information.
	pCV->lpOCV             = lpOCV;
    pCV->dwFlags           = lpOCV->dwFlags;
    pCV->clsid             = lpOCV->clsid;
    pCV->dvAspect          = lpOCV->dvAspect;
    pCV->lpszCurrentObject = lpOCV->lpszUserType;

    lpOCV->clsidNew            = CLSID_NULL;
    lpOCV->fObjectsIconChanged = false;

    //Allocate space for our strings
    if (NOERROR != CoGetMalloc(MEMCTX_TASK, &pIMalloc))
       return OLEUI_ERR_GLOBALMEMALLOC;

    pCV->lpszConvertDefault = (char *)pIMalloc->lpVtbl->Alloc(pIMalloc, OLEUI_CCHLABELMAX);
    pCV->lpszActivateDefault = (char *)pIMalloc->lpVtbl->Alloc(pIMalloc, OLEUI_CCHLABELMAX);

	pIMalloc->lpVtbl->Release(pIMalloc);

	//Hide the help button if required
	if (!(lpOCV->dwFlags & CF_SHOWHELP))
		HideDItem(pDialog, CV_BTN_HELP);

	if (NULL != lpOCV->lpszCaption)
		SetWTitle(pDialog, (StringPtr)lpOCV->lpszCaption);

    //Fill the Convert To and Activate As listboxes
    FillClassList(pCV);

#if OBSOLETE
	// NOTE: this is not possible as the original object is always in the convert list
	
	
    //Disable the "Convert" button if the Convert list doesn't
    //have any objects in it.
	if ((**pCV->hListConvert).dataBounds.bottom == 0)
	{
		pCV->dwFlags &= ~CF_SELECTCONVERTTO;
		CVUpdateOneControl(pDialog, CV_RBTN_CONVERT, false);	//Disable if no activate items
	}
#endif

    //Disable the "Activate As" button if the Activate list doesn't
    //have any objects in it.
	if ((**pCV->hListActivate).dataBounds.bottom <= 1)
	{
		pCV->dwFlags &= ~CF_SELECTACTIVATEAS;
		CVUpdateOneControl(pDialog, CV_RBTN_ACTIVATE, false);	//Disable if no activate items
	}

	//Only select Activate As if flag says to, otherwise default to Convert To
	if (pCV->dwFlags & CF_SELECTACTIVATEAS)
	{
		pCV->dwFlags &= ~CF_SELECTACTIVATEAS;	//Force update
		CheckRadioButton(pDialog, CV_RBTN_CONVERT, CV_RBTN_ACTIVATE, CV_RBTN_ACTIVATE);
		ToggleConvertType(pDialog, pCV, CF_SELECTACTIVATEAS);
	}
	else
	{
		pCV->dwFlags &= ~CF_SELECTCONVERTTO;	//Force update
		CheckRadioButton(pDialog, CV_RBTN_CONVERT, CV_RBTN_ACTIVATE, CV_RBTN_CONVERT);
		ToggleConvertType(pDialog, pCV, CF_SELECTCONVERTTO);
	}

	// Initialize Default strings.

	// Default convert string is easy...just use the user type name from
	// the clsid we got, or the current object
	if ((pCV->dwFlags & CF_SETCONVERTDEFAULT) && (IsValidClassID(pCV->lpOCV->clsidConvertDefault)))
	{
		cb = OleStdGetUserTypeOfClass(&pCV->lpOCV->clsidConvertDefault,
									  pCV->lpszConvertDefault,
									  OLEUI_CCHLABELMAX,
									  0L);

		if (0 == cb)
			strcpy(pCV->lpszConvertDefault, pCV->lpszCurrentObject);
	}
	else
		strcpy(pCV->lpszConvertDefault, pCV->lpszCurrentObject);

	// Default activate is a bit trickier.  We want to use the user type
	// name if from the clsid we got (assuming we got one), or the current
	// object if it fails or we didn't get a clsid.  But...if there's a
	// Treat As entry in the reg db, then we use that instead.  So... the
	// logic boils down to this:
	//
	// if ("Treat As" in reg db)
	//     use it;
	// else
	//     if (CF_SETACTIVATEDEFAULT)
	//        use it;
	//     else
	//        use current object;

	lRet = RegOpenKey(HKEY_CLASSES_ROOT, "CLSID", &hKey);

	if (ERROR_SUCCESS == lRet)
	{
		char	*lpszCLSID;
		char	szKey[OLEUI_CCHKEYMAX];
		CLSID	clsid;
		char	szValue[OLEUI_CCHKEYMAX];

		StringFromCLSID(&(pCV->lpOCV->clsid), &lpszCLSID);
		strcpy(szKey, lpszCLSID);
		strcat(szKey, "\\TreatAs");

		OleStdFreeString(lpszCLSID, NULL);

		cb = OLEUI_CCHKEYMAX;
		lRet = RegQueryValue(hKey, szKey, szValue, &cb);

		if (ERROR_SUCCESS == lRet)
		{
			CLSIDFromString(szValue, &clsid);
			if (0 == OleStdGetUserTypeOfClass(&clsid,
											  pCV->lpszActivateDefault,
											  OLEUI_CCHLABELMAX,
											  0L))
				lRet = -1;
		}

		RegCloseKey(hKey);
	}
	
	if (ERROR_SUCCESS != lRet)
	{
		if ((pCV->dwFlags & CF_SETACTIVATEDEFAULT) &&
			(IsValidClassID(pCV->lpOCV->clsidActivateDefault)))
		{
			cb = OleStdGetUserTypeOfClass(&pCV->lpOCV->clsidActivateDefault,
										  pCV->lpszActivateDefault,
										  OLEUI_CCHLABELMAX,
										  0L);

			if (0 == cb)
				strcpy(pCV->lpszActivateDefault, pCV->lpszCurrentObject);
		}
		else
			strcpy((pCV->lpszActivateDefault), pCV->lpszCurrentObject);
	}

	SetPt(&cell, 0, 0);

	fFound = FLSetSelection(pCV->hListConvert, pCV->lpszConvertDefault,
								(short)strlen(pCV->lpszConvertDefault));
	if (!fFound)
		LSetSelect(true, cell, pCV->hListConvert);

	fFound = FLSetSelection(pCV->hListActivate, pCV->lpszActivateDefault,
						(short)strlen(pCV->lpszActivateDefault));
	if (!fFound)
		LSetSelect(true, cell, pCV->hListActivate);

	pCV->fItemSelected = FLGetSelectedCell(pCV->hListConvert, &cell);

	pCV->iActiveItem = CV_UITEM_LBOX;		// initial active item
	HideDItem(pDialog, CV_TE_LABELEDIT);	// hide the edit control
	
	// Initialize icon stuff
	if (DVASPECT_ICON == pCV->dvAspect )
	{
		GetDItem(pDialog, CV_CBOX_DISPASICON, &itype, &h, &rc);
		SetCtlValue((ControlHandle)h, true);

		if (NULL != lpOCV->hMetaPict)
		{
			unsigned long	cch;


			// Get the icon label
			cch = sizeof(pCV->szIconLabel);
			cch= OleUIPictExtractLabel(lpOCV->hMetaPict, pCV->szIconLabel, cch);
			pCV->szIconLabel[cch] = '\0';
			
			// Get the icon
			pCV->hIcon = OleUIPictExtractIcon(lpOCV->hMetaPict);

			// Get the icon source
			OleUIPictExtractIconSource(lpOCV->hMetaPict, &pCV->IconSource);
		}
		else
			CVUpdateClassIcon(pDialog, pCV, TRUE, TRUE);
	}
	else
		CVUpdateClassIcon(pDialog, pCV, TRUE, TRUE);

	CVUpdateAllControls(pDialog, pCV);

	return OLEUI_SUCCESS;
}




/*
 * CVUInitUserItems
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment ConvertSeg
#else
#pragma code_seg("ConvertSeg","SWAPPABLE")
#endif
unsigned int CVUInitUserItems(DialogPtr pDialog)
{
	PCONVERT		pCV;
	short			iitem;
	short			itype;
	Handle			h;
	Rect			rc,
					rcDataBounds;
	Point			ptSize;
	short			theProc;

	//This assumes that the user items are grouped together and that
	//CV_UITEM_FIRST and CV_UITEM_LAST are properly defined
	for (iitem = CV_UITEM_FIRST; iitem <= CV_UITEM_LAST; iitem++)
	{
		GetDItem(pDialog, iitem, &itype, &h, &rc);
		if ((itype & ~itemDisable) == userItem)
#ifndef __powerc
			SetDItem(pDialog, iitem, itype, (Handle)CVUserItemProc, &rc);
#else
		   SetDItem(pDialog, iitem, itype, (Handle)gCVUserItemProc, &rc);
#endif

#ifdef _DEBUG
		else
			ASSERTCOND(false);
#endif //_DEBUG
	}

	pCV = (PCONVERT)GetWRefCon(pDialog);

	//Create listbox
	GetDItem(pDialog, CV_UITEM_LBOX, &itype, &h, &rc);
	InsetRect(&rc, 1, 1);
	rc.right -= 15;									//Add space for vertical scroll bar
	SetRect(&rcDataBounds, 0, 0, 2, 0);				//Two column listbox
	SetPt(&ptSize, (short)(rc.right-rc.left), 0);	//First column fills width of list (second isn't visible)
					
	//CUSTOM: Add your own ldef here
	theProc = 0;
	pCV->hListConvert = LNew(&rc, &rcDataBounds, ptSize, theProc, (WindowPtr)pDialog, true, false, false, true);

	if (NULL == pCV->hListConvert)
		return OLEUI_ERR_GLOBALMEMALLOC;

	(**pCV->hListConvert).selFlags = lOnlyOne|lNoNilHilite;
//	pCV->nConvertCurSel = 0;

	//CUSTOM: Add your own ldef here
	theProc = 0;
	pCV->hListActivate = LNew(&rc, &rcDataBounds, ptSize, theProc, (WindowPtr)pDialog, true, false, false, true);

	if (NULL == pCV->hListActivate)
		return OLEUI_ERR_GLOBALMEMALLOC;

	(**pCV->hListActivate).selFlags = lOnlyOne|lNoNilHilite;
//	pCV->nActivateCurSel = 0;

	return OLEUI_SUCCESS;
}




/*
 * IsValidClassID
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment ConvertSeg
#else
#pragma code_seg("ConvertSeg","SWAPPABLE")
#endif
Boolean IsValidClassID(CLSID cID)
{
	if (0 == memcmp(&cID, &CLSID_NULL, sizeof(CLSID)))  // if (CLSID_NULL == cID)
		return false;
	else
		return true;
}




/*
 * FillClassList
 *
 * Purpose:
 *  Enumerates available OLE object classes from the registration
 *  database that we can convert or activate the specified clsid from.
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment ConvertSeg
#else
#pragma code_seg("ConvertSeg","SWAPPABLE")
#endif
void FillClassList(PCONVERT pCV)
{
	long			cb;
	unsigned long	cStrings;
	HKEY			hKey;
	long			lRet;
	char			szFormatKey[OLEUI_CCHKEYMAX];
	char			szClass[OLEUI_CCHKEYMAX];	
	char			szFormat[OLEUI_CCHKEYMAX];
	char			szHRClassName[OLEUI_CCHKEYMAX];
	char			*lpszCLSID;

	//Open up the root key.
	lRet = RegOpenKey(HKEY_CLASSES_ROOT, "CLSID", &hKey);

	if (ERROR_SUCCESS != lRet)
		return;

	if (NULL == pCV->lpszCurrentObject)
	{
		LPMALLOC	pIMalloc = NULL;
		HRESULT		hrErr;

		hrErr = CoGetMalloc(MEMCTX_TASK, &pIMalloc);

		if (hrErr != NOERROR)
		{
			RegCloseKey(hKey);
			return;
		}

		// Allocate space for lpszCurrentClass
		pCV->lpszCurrentObject = (char *)pIMalloc->lpVtbl->Alloc(pIMalloc, OLEUI_CCHKEYMAX);

		lRet = OleStdGetUserTypeOfClass(&pCV->clsid,
										pCV->lpszCurrentObject,
										OLEUI_CCHLABELMAX,
										0L);

		if (0 == lRet)
		{
			RegCloseKey(hKey);
			return;
		}
	}

	// Get the class name of the original class.
	StringFromCLSID(&pCV->clsid, &lpszCLSID);

	// Always add the original class to both lists
	LAddClassToList(pCV->hListConvert, pCV->lpszCurrentObject, lpszCLSID);
	LAddClassToList(pCV->hListActivate, pCV->lpszCurrentObject, lpszCLSID);
	
	// Here, we step through the entire registration db looking for
	// class that can read or write the original class' format.  We
	// maintain two lists - an activate list and a convert list.  The
	// activate list is a subset of the convert list - activate == read/write
	// and convert == read. We swap the listboxes as needed with
	// ToggleConvertType, and keep track of which is which in the pCV structure.

	cStrings = 0;
	lRet = RegEnumKey(hKey, cStrings++, szClass, OLEUI_CCHKEYMAX);

	while (ERROR_SUCCESS == lRet)
	{
		Boolean		fIncluded;

		// don't need to check if it is the original class
		// as it is added to the list already
		if (!strcmp(lpszCLSID, szClass)) {
			//Continue with the next key.
			lRet = RegEnumKey(hKey, cStrings++, szClass, OLEUI_CCHKEYMAX);
			continue;
		}
			
		// Check for a \Conversion\ReadWriteable\Main entry first - if its
		// readwriteable, then we don't need to bother checking to see if
		// its readable.

		strcpy(szFormatKey, szClass);
		strcat(szFormatKey, "\\Conversion\\Readwritable\\Main");

		cb = OLEUI_CCHKEYMAX;

		lRet = RegQueryValue(hKey, szFormatKey, szFormat, &cb);

		if (ERROR_SUCCESS != lRet)
		{
			// Try \\DataFormats\DefaultFile too

			strcpy(szFormatKey, szClass);
			strcat(szFormatKey, "\\DataFormats\\DefaultFile");

			cb = OLEUI_CCHKEYMAX;

			lRet = RegQueryValue(hKey, szFormatKey, szFormat, &cb);
		}

		if (ERROR_SUCCESS == lRet)
		{
			// Here, we've got a list of formats that this class can read
			// and write. We need to see if the original class' format is
			// in this list.  We do that by looking for either dwOle2Format
			// or lpszOle1Format in szFormat, depending on whether it's an
			// OLE 1.0 or 2.0 object.  If it's in there we add this class to
			// both lists and continue.  If not, we look at the class'
			// readable formats.

			if (pCV->lpOCV->fIsOle1Object)
			{
				UpperText(szFormat, (short)strlen(szFormat));
				fIncluded = FormatIncludedSZ(szFormat, pCV->lpOCV->fmt.lpszOle1Format);
			}
			else
				fIncluded = FormatIncludedDW(szFormat, pCV->lpOCV->fmt.dwOle2Format);

			if (fIncluded)
			{
				cb = OLEUI_CCHKEYMAX;
				lRet = RegQueryValue(hKey, szClass, szHRClassName, &cb);

				if (ERROR_SUCCESS == lRet)
				{
					// Add to convert and activate lists
					LAddClassToList(pCV->hListConvert, szHRClassName, szClass);
					LAddClassToList(pCV->hListActivate, szHRClassName, szClass);
				}
			}
		}

		// We either didn't find the readwritable key, or the
		// list of readwritable formats didn't include the
		// original class format.  So, here we'll check to
		// see if its in the readable list.

		// We've got a special case for a linked object here.
		// If an object is linked, then the only class that
		// should appear in the convert list is the object's
		// class.  So, here we check to see if the object is
		// linked.  If it is, then we compare the classes.  If
		// they aren't the same, then we just go to the next key.

		else if ((!pCV->lpOCV->fIsLinkedObject) || (0 == strcmp(lpszCLSID, szClass)))
		{
			//Check for a \Conversion\Readable\Main entry
			strcpy(szFormatKey, szClass);
			strcat(szFormatKey, "\\Conversion\\Readable\\Main");
	
			cb = OLEUI_CCHKEYMAX;

			// Check to see if this class can read the original class
			// format.  If it can, add the string to the listbox as
			// CONVERT_LIST.

			lRet = RegQueryValue(hKey, szFormatKey, szFormat, &cb);

			if (ERROR_SUCCESS == lRet)
			{
				if (pCV->lpOCV->fIsOle1Object)
				{
					UpperText(szFormat, (short)strlen(szFormat));
					fIncluded = FormatIncludedSZ(szFormat, pCV->lpOCV->fmt.lpszOle1Format);
				}
				else
					fIncluded = FormatIncludedDW(szFormat, pCV->lpOCV->fmt.dwOle2Format);

				if (fIncluded)
				{
					cb = OLEUI_CCHKEYMAX;
					lRet = RegQueryValue(hKey, szClass, szHRClassName, &cb);

					if (ERROR_SUCCESS == lRet)
						LAddClassToList(pCV->hListConvert, szHRClassName, szClass);
				} // end if
			}
		} // end else

		//Continue with the next key.
		lRet = RegEnumKey(hKey, cStrings++, szClass, OLEUI_CCHKEYMAX);

	}  // end while

	// Free the string we got from StringFromCLSID.
	OleStdFreeString(lpszCLSID, NULL);

	RegCloseKey(hKey);
}




/*
 * LAddClassToList
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment ConvertSeg
#else
#pragma code_seg("ConvertSeg","SWAPPABLE")
#endif
void LAddClassToList(ListHandle hList, char *szClassName, char *szClassID)
{
	Cell	cell;

	//Add at the bottom of the list
	SetPt(&cell, 0, (**hList).dataBounds.bottom);
	LAddRow(1, cell.v, hList);

	//First add the class name
  	LSetCell(szClassName, (short)strlen(szClassName), cell, hList);

	//Then add the clsid in the second column
  	cell.h++;
  	LSetCell(szClassID, (short)strlen(szClassID), cell, hList);
}




/*
 * FormatIncludedDW
 *
 * Purpose:
 *  Search a string for a specific format represented by
 *  an unsigned long.
 *
 * Parameters:
 *  szStringToSearch  String to parse
 *  dwFormat          format to find
 *
 * Return Value:
 *  Boolean     true if format is found in string,
 *              false otherwise.
 */

#ifndef _MSC_VER
#pragma segment ConvertSeg
#else
#pragma code_seg("ConvertSeg","SWAPPABLE")
#endif
Boolean FormatIncludedDW(char *szStringToSearch, unsigned long dwFormat)
{
	char	szFormat[5];

	if (dwFormat == 0)
		return false;

	*(unsigned long *)szFormat = dwFormat;
	szFormat[4] = 0;

	return (strstr(szStringToSearch, szFormat) != NULL);
}




/*
 * FormatIncludedSZ
 *
 * Purpose:
 *  Search a string for a specific format string. This function
 *  surrounds the format string with quotes before searching.
 *
 * Parameters:
 *  szStringToSearch  String to parse
 *  szFormatSearch    format to find
 *
 * Return Value:
 *  Boolean     true if format is found in string,
 *              false otherwise.
 */

#ifndef _MSC_VER
#pragma segment ConvertSeg
#else
#pragma code_seg("ConvertSeg","SWAPPABLE")
#endif
Boolean FormatIncludedSZ(char *szStringToSearch, char *szFormatSearch)
{
	char	szQuoted[OLEUI_CCHKEYMAX] = "\"";

	if (szFormatSearch[0] == '\0')
		return false;

	strcat(szQuoted, szFormatSearch);
	strcat(szQuoted, "\"");

	// Convert to uppercase
	UpperText(&szQuoted[1], (short)strlen(szFormatSearch));

	return (strstr(szStringToSearch, szQuoted) != NULL);
}




/*
 * ConvertDialogProc
 *
 * Purpose:
 *  Implements the OLE Convert dialog as invoked through the
 *  OleUIConvert function.
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment ConvertSeg
pascal Boolean
#else
#pragma code_seg("ConvertSeg","SWAPPABLE")
Boolean __pascal
#endif
ConvertDialogProc(DialogPtr pDialog, EventRecord *pEvent, short *nItem)
{
	PCONVERT			pCV;
	GrafPtr				gpSave;
	Boolean				fHooked;
	WindowPtr			pWindow;
	short				in;
	Point				ptWhere;
	char				chKey;
	ListHandle			hList;
	Cell				cell;
	short				oldSelect;
	Boolean				shifted;
	short				itype;
	Handle				h;
	Rect				rc;

	pCV = (PCONVERT)GetWRefCon(pDialog);

	GetPort(&gpSave);
	SetPort(pDialog);

	fHooked = FStandardHook((LPOLEUISTANDARD)pCV->lpOCV, pDialog, pEvent, nItem, pCV->lpOCV->lCustData);
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
			if ((WindowPtr)pEvent->message == pDialog) {
				TextFont(systemFont);
				TextSize(12);
			}			
			break;
		
		case mouseDown:
			in = FindWindow(pEvent->where, &pWindow);
			if ((pWindow == (WindowPtr)pDialog) && (in == inDrag) && (pCV->lpOCV->lpfnHook != NULL))
			{
				DragWindow(pDialog, pEvent->where, &qd.screenBits.bounds);
				SetPort(gpSave);
				return true;
			}

			ptWhere = pEvent->where;
			GlobalToLocal(&ptWhere);

			switch (FindDItem(pDialog, ptWhere) + 1)
			{
				case CV_UITEM_LBOX:
					CVSetActiveItem(pDialog, pCV, CV_UITEM_LBOX);
					hList = (pCV->dwFlags & CF_SELECTCONVERTTO ? pCV->hListConvert : pCV->hListActivate);

					oldSelect = pCV->nSelectedIndex;

					if (LClick(ptWhere, pEvent->modifiers, hList))
					{
						FlashButton(pDialog, CV_BTN_OK);
						*nItem = CV_BTN_OK;
						SetPort(gpSave);
						return true;
					}

					pCV->fItemSelected = FLGetSelectedCell(hList, &cell);
					pCV->nSelectedIndex = (pCV->fItemSelected) ? cell.v : -1;

					if (pCV->fItemSelected && (pCV->nSelectedIndex != oldSelect))
						CVUpdateClassIcon(pDialog, pCV, TRUE, TRUE);

					CVUpdateAllControls(pDialog, pCV);
					break;

				case CV_UITEM_ICON:
					CVSetActiveItem(pDialog, pCV, CV_UITEM_ICON);
					break;
					
				case CV_UITEM_ICONTXT:
					CVSetActiveItem(pDialog, pCV, CV_UITEM_ICONTXT);
					SetPort(gpSave);
					return true;
					break;
			}
			break;

		case keyDown:
		case autoKey:
			chKey = pEvent->message & charCodeMask;
			switch (chKey)
			{
				case ENTERKEY:
				case RETURNKEY:
					GetDItem(pDialog, CV_BTN_OK, &itype, &h, &rc);
					if (0 == (**((ControlHandle)h)).contrlHilite)
					{
						FlashButton(pDialog, CV_BTN_OK);
						*nItem = CV_BTN_OK;
					}
					SetPort(gpSave);
					return true;

				case PERIODKEY:
					if (!(pEvent->modifiers & cmdKey))
						break;
					// Else fall through...

				case ESCAPEKEY:
					FlashButton(pDialog, CV_BTN_CANCEL);
					*nItem = CV_BTN_CANCEL;
					SetPort(gpSave);
					return true;

				case UPKEY:
				case DOWNKEY:
					hList = (pCV->dwFlags & CF_SELECTCONVERTTO ? pCV->hListConvert : pCV->hListActivate);
					if ((**hList).dataBounds.bottom > 0)
					{
						shifted = false;
						SetPt(&cell, 0, 0);
						if (LGetSelect(true, &cell, hList))
						{
							if ((UPKEY == chKey && cell.v > 0) ||
								(DOWNKEY == chKey && cell.v < (**hList).dataBounds.bottom - 1))
							{
								LSetSelect(false, cell, hList);
								cell.v += (UPKEY == chKey ? -1 : 1);
								LSetSelect(true, cell, hList);

								//If the selection is outside the list's view, scroll it into view
								LRect(&rc, cell, hList);
								if ((UPKEY == chKey && rc.top < (**hList).rView.top) ||
									(DOWNKEY == chKey && rc.bottom > (**hList).rView.bottom))
									LScroll(0, (short)(UPKEY == chKey ? -1 : 1), hList);
								shifted = true;
							}
						}
						else
						{
							SetPt(&cell, 0, 0);
							LSetSelect(true, cell, hList);
							LAutoScroll(hList);
							shifted = true;
						}

						if (shifted)
						{
							CVUpdateAllControls(pDialog, pCV);
						}
					}
					break;

				case TABKEY:
					CVTabFocus(pDialog, pCV);
					break;

				case VKEY:
				case CKEY:
				case XKEY:
					if (pCV->iActiveItem == CV_UITEM_ICON) {
						PicHandle 	hIcon;
						long 		lOffset;
						long 		lPictSize;
									
						
						// only concerned w/ cut and paste key sequences
						if (pEvent->modifiers & cmdKey)  {
						
							switch (chKey) {
							
								case CKEY:	
									if(pCV->hIcon) {
										HLock((Handle)pCV->hIcon);
										ZeroScrap();
										PutScrap(GetHandleSize((Handle)pCV->hIcon), 'PICT', (Ptr)(*pCV->hIcon));
										HUnlock((Handle)pCV->hIcon);
									}
									break;
									
								case XKEY:
									if (!pCV->fDefaultIcon) {		// switch to the default icon
										if(pCV->hIcon) {
											HLock((Handle)pCV->hIcon);
											ZeroScrap();
											PutScrap(GetHandleSize((Handle)pCV->hIcon), 'PICT', (Ptr)(*pCV->hIcon));
											HUnlock((Handle)pCV->hIcon);
										}
										CVUpdateClassIcon(pDialog, pCV, TRUE, FALSE);
										GetDItem(pDialog, CV_UITEM_ICON, &itype, &h, &rc);
										EraseRect(&rc);
										InvalRect(&rc);
										pCV->fDefaultIcon = true;
										pCV->lpOCV->fObjectsIconChanged = true;
									}
									break;
									
								case VKEY:
									lPictSize = GetScrap(nil, 'PICT', &lOffset);
			
									if(lPictSize > 0) {
										if(pCV->hIcon) {
											KillPicture(pCV->hIcon);
											pCV->hIcon = NULL;
										}
										hIcon = (PicHandle)NewHandle(lPictSize);
										if(hIcon) {
											GetScrap((Handle)hIcon, 'PICT', &lOffset);
											pCV->hIcon = OlePictFromIconAndLabel(hIcon, NULL, NULL);
											DisposeHandle((Handle)hIcon);
											GetDItem(pDialog, CV_UITEM_ICON, &itype, &h, &rc);
											EraseRect(&rc);
											InvalRect(&rc);
											pCV->fDefaultIcon = false;
											pCV->lpOCV->fObjectsIconChanged = true;
										}
									}
									break;
							}
						}
					}
					break;
			}
			break;
	}

	SetPort(gpSave);

	return false;
}




/*
 * CVUserItemProc
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
#pragma segment ConvertSeg
pascal void
#else
#pragma code_seg("ConvertSeg","SWAPPABLE")
void __pascal
#endif
CVUserItemProc(DialogPtr pDialog, short iitem)
{
	GrafPtr			gpSave;
	short			itype;
	Handle			h;
	Rect			rc;
	PCONVERT		pCV;
	short			txFont = pDialog->txFont;
	short			txSize = pDialog->txSize;

	GetPort(&gpSave);
	SetPort(pDialog);

	GetDItem(pDialog, iitem, &itype, &h, &rc);
	pCV = (PCONVERT)GetWRefCon(pDialog);

	// use 12 Pt System Font by default
	TextFont(systemFont);
	TextSize(12);

	switch (iitem)
	{
		case CV_UITEM_RESULT:
		{
			PenSize(1, 1);
			FrameRect(&rc);

			InsetRect(&rc, 8, 12);

			if (!pCV->fItemSelected)
			{
				EraseRect(&rc);		//Review: how should this case be handled?
				break;
			}

			TextFont(geneva);
			TextSize(9);

			TextBox(&pCV->szResult[1], pCV->szResult[0], &rc, teFlushLeft);

			break;
		}

		case CV_UITEM_LBOX:
			CVDrawList(pDialog, pCV, false);
			break;

		case CV_UITEM_SRCTEXT:
			TextBox(pCV->lpszCurrentObject, strlen(pCV->lpszCurrentObject), &rc, teFlushLeft);
			break;

		case CV_UITEM_ICON:
			CVDrawIcon(pDialog, pCV, false);
			break;

		case CV_UITEM_ICONTXT:
			CVDrawIconLabel(pDialog, pCV);
			break;

		case CV_UITEM_OKOUTLINE:
			DrawDefaultBorder(pDialog, CV_UITEM_OKOUTLINE, pCV->fItemSelected);
			break;
	}

	TextFont(txFont);
	TextSize(txSize);

	SetPort(gpSave);
}




/*
 * CVUpdateOneControl
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
#pragma segment ConvertSeg
#else
#pragma code_seg("ConvertSeg","SWAPPABLE")
#endif
void CVUpdateOneControl(DialogPtr pDialog, short iitem, Boolean fAvail)
{
	short		itype;
	Handle		h;
	Rect		rc;

	GetDItem(pDialog, iitem, &itype, &h, &rc);

	if ((**(ControlHandle)h).contrlHilite == (fAvail ? 0 : 255))
		return;

	HiliteControl((ControlHandle)h, (short)(fAvail ? 0 : 255));

	if (CV_BTN_OK == iitem)
		DrawDefaultBorder(pDialog, CV_UITEM_OKOUTLINE, fAvail);
}




/*
 * CVUpdateAllControls
 *
 * Purpose:
 *  Updates the availability of all the controls in our dialog box.
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment ConvertSeg
#else
#pragma code_seg("ConvertSeg","SWAPPABLE")
#endif
void CVUpdateAllControls(DialogPtr pDialog, PCONVERT pCV)
{
	CVUpdateOneControl(pDialog, CV_BTN_OK, pCV->fItemSelected);
	CVUpdateDisplayAsIcon(pDialog, pCV);
}




/*
 * CVUpdateResults
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment ConvertSeg
#else
#pragma code_seg("ConvertSeg","SWAPPABLE")
#endif
void CVUpdateResults(DialogPtr pDialog, PCONVERT pCV)
{
	char			*pszOutput,		//Final output for result area
					*pszDefObj,		//String containing default object class
					*pszSelObj,		//String containing selected object class
					*pszString;		//String we get from GetIndString
	ListHandle		hList;
	Cell			cell;
	short			cch;
	short			itype;
	Handle			h;
	Rect			rc;

	pszOutput = NewPtrClear(4 * 512);
	if (NULL == pszOutput)
		return;

	pszSelObj = pszOutput + 512;
	pszDefObj = pszSelObj + 512;
	pszString = pszDefObj + 512;
	
	hList = (pCV->dwFlags & CF_SELECTCONVERTTO ? pCV->hListConvert : pCV->hListActivate);

	// Get selected string
	cch = 512;
	FLGetSelectedCell(hList, &cell);
	LGetCell(pszSelObj, &cch, cell, hList);

	strcpy(pszDefObj, (pCV->dwFlags & CF_SELECTCONVERTTO ? pCV->lpszConvertDefault : pCV->lpszActivateDefault));

	// Default is an empty string.
	*pszOutput = 0;

	if (pCV->dwFlags & CF_SELECTCONVERTTO)
	{
		if (pCV->lpOCV->fIsLinkedObject)  // working with linked object
		{
			GetIndString((unsigned char *)pszOutput, DIDConvert, IDS_CVRESULTCONVERTLINK);
			p2cstr((unsigned char *)pszOutput);
		}
		else
		{
			// converting to a new class
			if (0 != strcmp(pszDefObj, pszSelObj))
			{
				GetIndString((unsigned char *)pszString, DIDConvert, IDS_CVRESULTCONVERTTO);
				p2cstr((unsigned char *)pszString);
				sprintf(pszOutput, pszString, pszDefObj, pszSelObj);
			}
			else  // converting to the same class (no conversion)
		 	{
				GetIndString((unsigned char *)pszString, DIDConvert, IDS_CVRESULTNOCHANGE);
				p2cstr((unsigned char *)pszString);
				sprintf(pszOutput, pszString, pszDefObj);
			}
		}

		if (pCV->dvAspect == DVASPECT_ICON)  // Display as icon is checked
		{
			GetIndString((unsigned char *)pszString, DIDConvert, IDS_CVRESULTDISPLAYASICON);
			p2cstr((unsigned char *)pszString);
			strcat(pszOutput, pszString);
		}
	}

	if (pCV->dwFlags & CF_SELECTACTIVATEAS)
	{
		GetIndString((unsigned char *)pszString, DIDConvert, IDS_CVRESULTACTIVATEAS);
		p2cstr((unsigned char *)pszString);
		sprintf(pszOutput, pszString, pszDefObj, pszSelObj);

		// activating as a new class
		if (0 != strcmp(pszDefObj, pszSelObj))
		{
			GetIndString((unsigned char *)pszString, DIDConvert, IDS_CVRESULTACTIVATEDIFF);
			p2cstr((unsigned char *)pszString);
			strcat(pszOutput, pszString);
		}
		else // activating as itself.
			strcat(pszOutput, ".");
	}

	strcpy((char *)pCV->szResult, pszOutput);
	c2pstr((char *)pCV->szResult);

	DisposePtr(pszOutput);

	GetDItem(pDialog, CV_UITEM_RESULT, &itype, &h, &rc);
	InsetRect(&rc, 4, 12);

	SetPort(pDialog);
	InvalRect(&rc);			//Force a redraw of the result area.
}




/*
 * ToggleConvertType
 *
 * Purpose:
 *  Toggles between Convert To and Activate As.
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment ConvertSeg
#else
#pragma code_seg("ConvertSeg","SWAPPABLE")
#endif
void ToggleConvertType(DialogPtr pDialog, PCONVERT pCV, unsigned long dwOption)
{
	Cell		cell;
	short		itype;
	Handle		h;
	Rect		rc;

	//Skip all of this if we're already selected.
	if (pCV->dwFlags & dwOption)
		return;

	//1. Change the Display As Icon checked state to reflect the
	//   selection for this option, stored in the fAsIcon flags.
	pCV->dwFlags = (pCV->dwFlags & ~(CF_SELECTCONVERTTO | CF_SELECTACTIVATEAS)) | dwOption;

	if (pCV->dwFlags & CF_SELECTCONVERTTO)
	{
		CVShowListItem(pCV->hListActivate, false);
		CVShowListItem(pCV->hListConvert, true);
		CheckRadioButton(pDialog, CV_RBTN_CONVERT, CV_RBTN_ACTIVATE, CV_RBTN_CONVERT);
		pCV->fItemSelected = FLGetSelectedCell(pCV->hListConvert, &cell);
	}
	else
	{
		CVShowListItem(pCV->hListConvert, false);
		CVShowListItem(pCV->hListActivate, true);
		CheckRadioButton(pDialog, CV_RBTN_CONVERT, CV_RBTN_ACTIVATE, CV_RBTN_ACTIVATE);
		pCV->fItemSelected = FLGetSelectedCell(pCV->hListActivate, &cell);
	}

	GetDItem(pDialog, CV_UITEM_LBOX, &itype, &h, &rc);
	InsetRect(&rc, 1, 1);
	rc.right -= 15;

	SetPort(pDialog);
	EraseRect(&rc);
	InvalRect(&rc);

	CVUpdateAllControls(pDialog, pCV);
}




/*
 * CVShowListItem
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment ConvertSeg
#else
#pragma code_seg("ConvertSeg","SWAPPABLE")
#endif
void CVShowListItem(ListHandle hList, Boolean fShow)
{
	Rect	rc;
	GrafPtr	gpSave;

	GetPort(&gpSave);
	SetPort((*hList)->port);

	if (fShow)
	{
		(**hList).rView.left  &= ~0x4000;
		(**hList).rView.right &= ~0x4000;
	}
	else
	{
		(**hList).rView.left  |= 0x4000;
		(**hList).rView.right |= 0x4000;
	}

	// This forces the List Manager to recognize the move:
	rc = (**hList).rView;
	InsetRect(&rc, -1, -1);
	PenSize(1, 1);
	FrameRect(&rc);

	SetPort(gpSave);
}




/*
 * CVUpdateDisplayAsIcon
 *
 * Purpose:
 *  Update the DisplayAsIcon checkbox. Hides or shows the Icon Display and
 *  the Change Icon button and changes the help result text and bitmap.
 *
 * Parameters:
 *
 * Return Value:
 *  None
 *
 */

#ifndef _MSC_VER
#pragma segment ConvertSeg
#else
#pragma code_seg("ConvertSeg","SWAPPABLE")
#endif
void CVUpdateDisplayAsIcon(DialogPtr pDialog, PCONVERT pCV)
{
	Boolean		fCheck;
	short		itype;
	Handle		h;
	Rect		rc;

	fCheck = (pCV->dvAspect == DVASPECT_ICON);

	//Update the current value in the control.
	GetDItem(pDialog, CV_CBOX_DISPASICON, &itype, &h, &rc);
	SetCtlValue((ControlHandle)h, fCheck);

	CVUpdateOneControl(pDialog, CV_CBOX_DISPASICON, pCV->fItemSelected);
	
	if (fCheck && pCV->fItemSelected)		// we don't want to show the items if no item is selected
	{
		ShowDItem(pDialog, CV_UITEM_ICON);
		ShowDItem(pDialog, CV_UITEM_ICONTXT);
	}
	else
	{
		HideDItem(pDialog, CV_UITEM_ICON);
		HideDItem(pDialog, CV_UITEM_ICONTXT);
		HideDItem(pDialog, CV_TE_LABELEDIT);
	}

	// Result help text and icon change here
	CVUpdateResults(pDialog, pCV);
}




/*
 * ConvertCleanup
 *
 * Purpose:
 *  Handles any final cleanup necessary.
 *
 * Parameters:
 *  pDialog			DialogPtr to the dialog box.
 *
 * Return Value:
 *  None
 */

#ifndef _MSC_VER
#pragma segment ConvertSeg
#else
#pragma code_seg("ConvertSeg","SWAPPABLE")
#endif
void ConvertCleanup(DialogPtr pDialog)
{
	PCONVERT	pCV;

	pCV = (PCONVERT)GetWRefCon(pDialog);

	if (pCV)
	{
		if (pCV->hListConvert)  LDispose(pCV->hListConvert);
		if (pCV->hListActivate) LDispose(pCV->hListActivate);
		if (pCV->hIcon) OleUIPictIconFree(pCV->hIcon);
		DisposePtr((Ptr)pCV);
	}

	DisposDialog(pDialog);
}




/*
 * CVDrawIconLabel
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment ConvertSeg
#else
#pragma code_seg("ConvertSeg","SWAPPABLE")
#endif
void CVDrawIconLabel(DialogPtr pDialog, PCONVERT pCV)
{
	short		itype;
	Handle		h;
	Rect		rc;
	short		txFont;
	short		txSize;
	GrafPtr		gpSave;
	
	GetPort(&gpSave);
	SetPort(pDialog);

	GetDItem(pDialog, CV_UITEM_ICONTXT, &itype, &h, &rc);

	txFont = pDialog->txFont;
	txSize = pDialog->txSize;

	TextFont(geneva);
	TextSize(9);

	TextBox(pCV->szIconLabel, strlen(pCV->szIconLabel), &rc, teCenter);

	TextFont(txFont);
	TextSize(txSize);
	
	SetPort(gpSave);
}




/*
 * CVDrawIcon
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment ConvertSeg
#else
#pragma code_seg("ConvertSeg","SWAPPABLE")
#endif
void CVDrawIcon(DialogPtr pDialog, PCONVERT pCV, Boolean FocusRectOnly)
{
	Rect 		frame;
	short		itype;
	Handle		h;
	Rect		rc;
	PenState	savePenState;
	GrafPtr		gpSave;
	
	GetPort(&gpSave);
	SetPort(pDialog);

	GetDItem(pDialog, CV_UITEM_ICON, &itype, &h, &rc);
	GetPenState(&savePenState);

	if (!FocusRectOnly)
		DrawPicture(pCV->hIcon, &rc);

	if(pCV->iActiveItem != CV_UITEM_ICON)
		PenPat((ConstPatternParam)&pDialog->bkPat);
		
	SetRect(&frame, (short)(rc.left - OLEUI_ICONFRAME), (short)(rc.top - OLEUI_ICONFRAME),
					(short)(rc.right + OLEUI_ICONFRAME), (short)(rc.bottom + OLEUI_ICONFRAME));
	PenSize(1, 1);
	FrameRect(&frame);
	
	SetPenState(&savePenState);

	SetPort(gpSave);
}




/*
 * CVDrawList
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment ConvertSeg
#else
#pragma code_seg("ConvertSeg","SWAPPABLE")
#endif
void CVDrawList(DialogPtr pDialog, PCONVERT pCV, Boolean FocusRectOnly)
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

	GetDItem(pDialog, CV_UITEM_LBOX, &itype, &h, &rc);
	GetPenState(&savePenState);

	if (!FocusRectOnly) {
		if (pCV->dwFlags & CF_SELECTCONVERTTO)
			LUpdate(pDialog->visRgn, pCV->hListConvert);
		else
			LUpdate(pDialog->visRgn, pCV->hListActivate);

		PenSize(1, 1);
		FrameRect(&rc);
	}

	SetRect(&rc, (short)(rc.left - OLEUI_LBOXFRAME), (short)(rc.top - OLEUI_LBOXFRAME),
				 (short)(rc.right + OLEUI_LBOXFRAME), (short)(rc.bottom + OLEUI_LBOXFRAME));
	PenSize(2, 2);
	if(pCV->iActiveItem != CV_UITEM_LBOX) {
		PenPat((ConstPatternParam)&pDialog->bkPat);
	}
	FrameRect(&rc);
	SetPenState(&savePenState);

	TextFont(txFont);
	TextSize(txSize);

	SetPort(gpSave);
}




/*
 * CVUpdateClassIcon
 *
 * Purpose:
 *  Handles selection change for the Object Type listbox.  On a selection
 *  change, we extract an icon from the server handling the currently
 *  selected object type using the utility function HIconFromClass.
 *
 * Parameters
 *  pDialog
 *  pCV
 *  hList
 *
 * Return Value:
 *  None
 */

#ifndef _MSC_VER
#pragma segment ConvertSeg
#else
#pragma code_seg("ConvertSeg","SWAPPABLE")
#endif
void CVUpdateClassIcon(DialogPtr pDialog, PCONVERT pCV, Boolean fUpdateIcon, Boolean fUpdateLabel)
{
#ifndef _MSC_VER
#pragma unused(pDialog)
#endif

	Cell		cell;	
	short		cch;
	char		szCLSID[OLEUI_CCHKEYMAX];
	long		len;
	CLSID       clsid;
	PicHandle	hPict;
	ListHandle	hList;

	/*
	 * When we change object type selection, get the new icon for that
	 * type into our structure and update it in the display.
	 */
	if (pCV->nSelectedIndex < 0)
		return;

	hList = (pCV->dwFlags & CF_SELECTCONVERTTO ? pCV->hListConvert : pCV->hListActivate);

	cch = sizeof(szCLSID);
	SetPt(&cell, 1, pCV->nSelectedIndex);
	
	LGetCell(szCLSID, &cch, cell, hList);
	CLSIDFromString(szCLSID, &clsid);

	// Create the class ID with this string.
	CLSIDFromString(szCLSID, &clsid);

	if (pCV->hIcon) {
		KillPicture(pCV->hIcon);
		pCV->hIcon = NULL;
	}
		
	hPict = OleGetIconOfClass(&clsid, NULL, true);
	if (hPict) {
		if (fUpdateIcon)
	 		pCV->hIcon = OleUIPictExtractIcon(hPict);
	 	if (fUpdateLabel) {
			len = sizeof(pCV->szIconLabel);
			len = OleUIPictExtractLabel(hPict, pCV->szIconLabel, len);
			pCV->szIconLabel[len] = '\0';
		}
		OleUIPictIconFree(hPict);
	}

	// Get the label
	if (pCV->lpOCV->pszDefLabel && fUpdateLabel) {
		cch = sizeof(pCV->szIconLabel);
		strncpy(pCV->szIconLabel, pCV->lpOCV->pszDefLabel, cch - 1);
		pCV->szIconLabel[cch] = '\0';
	}
}




/*
 * CVTabFocus
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment ConvertSeg
#else
#pragma code_seg("ConvertSeg","SWAPPABLE")
#endif
void CVTabFocus(DialogPtr pDialog, PCONVERT pCV)
{
	switch (pCV->iActiveItem) {
		case CV_UITEM_LBOX:
			CVSetActiveItem(pDialog, pCV, CV_UITEM_ICON);
			break;
			
		case CV_UITEM_ICON:
			CVSetActiveItem(pDialog, pCV, CV_UITEM_ICONTXT);
			break;
			
		case CV_UITEM_ICONTXT:
			CVSetActiveItem(pDialog, pCV, CV_UITEM_LBOX);
			break;
			
	}
}




/*
 * CVSetActiveItem
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment ConvertSeg
#else
#pragma code_seg("ConvertSeg","SWAPPABLE")
#endif
void CVSetActiveItem(DialogPtr pDialog, PCONVERT pCV, short item)
{
	short	itemLoseFocus;
	short	itype;
	Handle	h;
	Rect	rc;
	
	if (pCV->iActiveItem == item)		// focus at the item already
		return;
		
	switch (item) {
		case CV_UITEM_LBOX:
			GetDItem(pDialog, CV_UITEM_LBOX, &itype, &h, &rc);
			if (rc.left > 8192)		// item is invisiable
				return;
			itemLoseFocus = pCV->iActiveItem;
			pCV->iActiveItem = item;
			CVDrawList(pDialog, pCV, true);
			break;
			
		case CV_UITEM_ICON:
			GetDItem(pDialog, CV_UITEM_ICON, &itype, &h, &rc);
			if (rc.left > 8192)		// item is invisiable
				return;
			itemLoseFocus = pCV->iActiveItem;
			pCV->iActiveItem = item;
			CVDrawIcon(pDialog, pCV, true);
			break;
			
		case CV_UITEM_ICONTXT:
			GetDItem(pDialog, CV_UITEM_ICONTXT, &itype, &h, &rc);
			if (rc.left > 8192)		// item is invisiable
				return;
			itemLoseFocus = pCV->iActiveItem;
			pCV->iActiveItem = item;
			CVToggleIconLabel(pDialog, pCV, CV_TE_LABELEDIT);
			break;
	}
	
	switch (itemLoseFocus) {
		case CV_UITEM_LBOX:
			CVDrawList(pDialog, pCV, true);
			break;
			
		case CV_UITEM_ICON:
			CVDrawIcon(pDialog, pCV, true);
			break;
			
		case CV_UITEM_ICONTXT:
			CVToggleIconLabel(pDialog, pCV, CV_UITEM_ICONTXT);
			CVDrawIconLabel(pDialog, pCV);
			break;
	}
	
}




/*
 * CVToggleIconLabel
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment ConvertSeg
#else
#pragma code_seg("ConvertSeg","SWAPPABLE")
#endif
void CVToggleIconLabel(DialogPtr pDialog, PCONVERT pCV, short item)
// This procedure assumes that the display as icon checkbox is checked
{
	TEHandle 	hEdit = ((DialogPeek)pDialog)->textH;
	CharsHandle	hChars;
	unsigned long		cchLabel;
	
	// ASSERTCOND(pCV->dwFlags & CVF_CHECKDISPLAYASICON);
	ASSERTCOND((item == CV_TE_LABELEDIT) || (item == CV_UITEM_ICONTXT));
	
	if(item == CV_TE_LABELEDIT) {  // label edit control is in file list window
		HideDItem(pDialog, CV_UITEM_ICONTXT);
		ShowDItem(pDialog, CV_TE_LABELEDIT);
		(*hEdit)->txFont = geneva;
		(*hEdit)->txSize = 9;		
		TESetText(pCV->szIconLabel, strlen(pCV->szIconLabel), hEdit);
		TEActivate(hEdit);
		TESetSelect(0, 32767, hEdit);			
	}
	else {  // label edit control is in class list window
		ShowDItem(pDialog, CV_UITEM_ICONTXT);
		HideDItem(pDialog, CV_TE_LABELEDIT);
		TEDeactivate(hEdit);
		hChars = TEGetText(hEdit);
		HLock((Handle)hChars);
		cchLabel = MIN((*hEdit)->teLength, sizeof(pCV->szIconLabel) - 1);
		strncpy(pCV->szIconLabel, (char *)*hChars, cchLabel);
		pCV->szIconLabel[cchLabel] = '\0';
		HUnlock((Handle)hChars);

		pCV->lpOCV->fObjectsIconChanged = true;
	}
}




/*
 * OleUICanConvertOrActivateAs
 *
 * Purpose:
 *  Determine if there is any OLE object class from the registration
 *  database that we can convert or activate the specified clsid from.
 *
 * Parameters:
 *  rClsid          REFCLSID Class ID for class to find convert classes for
 *  fIsLinkedObject BOOL is the original object a linked object
 *  wFormat         WORD specifying the format of the original class.
 *
 * Return Value:
 *  BOOL            TRUE if Convert command should be enabled, else FALSE
 */

#ifndef _MSC_VER
#pragma segment ConvertSeg
#else
#pragma code_seg("ConvertSeg","SWAPPABLE")
#endif
STDAPI_(Boolean) OleUICanConvertOrActivateAs(
		REFCLSID    	rClsid,
		Boolean        	fIsLinkedObject,
		Boolean			fIsOle1Object,
		OLEUIOLEFORMAT 	Format
)
{

	long			dw;
	unsigned int   	cStrings=0;
	HKEY        	hKey;
	long        	lRet;
	char        	szFormatKey[OLEUI_CCHKEYMAX];
	char        	szClass[OLEUI_CCHKEYMAX];
	char        	szFormat[OLEUI_CCHKEYMAX];
	char        	szHRClassName[OLEUI_CCHKEYMAX];
	Boolean        	fEnableConvert = FALSE;
	Boolean			fIncluded = FALSE;

	char*       	pszCLSID;

	//Open up the root key.
	lRet = RegOpenKey(HKEY_CLASSES_ROOT, "CLSID", &hKey);

	if ((long)ERROR_SUCCESS != lRet)
		return FALSE;

	// Get the class name of the original class.
	StringFromCLSID(rClsid, &pszCLSID);

	// Here, we step through the entire registration db looking for
	// class that can read or write the original class' format.
	// This loop stops if a single class is found.

	cStrings = 0;
	lRet = RegEnumKey(hKey, cStrings++, szClass, OLEUI_CCHKEYMAX);

	while ((long)ERROR_SUCCESS == lRet)
	{
		if (strcmp(pszCLSID, szClass) == 0)
			goto next;   // we don't want to consider the source class

		// Check for a \Conversion\ReadWriteable\Main entry first - if its
		// readwriteable, then we don't need to bother checking to see if
		// its readable.

		strcpy(szFormatKey, szClass);
		strcat(szFormatKey, "\\Conversion\\Readwritable\\Main");

		dw = OLEUI_CCHKEYMAX;

		lRet = RegQueryValue(hKey, szFormatKey, szFormat, &dw);

		if (ERROR_SUCCESS != lRet)
		{
		  // Try \\DataFormats\DefaultFile too

		  strcpy(szFormatKey, szClass);
		  strcat(szFormatKey, "\\DataFormats\\DefaultFile");

		  dw=OLEUI_CCHKEYMAX;

		  lRet = RegQueryValue(hKey, szFormatKey, szFormat, &dw);
		}


		if ((long)ERROR_SUCCESS == lRet)
		{
			if (fIsOle1Object)
			{
				UpperText(szFormat, (short)(strlen(szFormat)));
				fIncluded = FormatIncludedSZ(szFormat, Format.lpszOle1Format);
			}
			else
				fIncluded = FormatIncludedDW(szFormat, Format.dwOle2Format);

			// Here, we've got a list of formats that this class can read
			// and write. We need to see if the original class' format is
			// in this list.  We do that by looking for wFormat in
			// szFormat - if it in there, then we add this class to the
			// both lists and continue.  If not, then we look at the
			// class' readable formats.
			if (fIncluded) {
				dw = OLEUI_CCHKEYMAX;
				lRet = RegQueryValue(hKey, szClass, szHRClassName, &dw);
	
				if (ERROR_SUCCESS == lRet)
				{
					fEnableConvert = TRUE;
					break;  // STOP -- found one!
				}
			}

		}


		// We either didn't find the readwritable key, or the
		// list of readwritable formats didn't include the
		// original class format.  So, here we'll check to
		// see if its in the readable list.


		// We've got a special case for a linked object here.
		// If an object is linked, then the only class that
		// should appear in the convert list is the object's
		// class.  So, here we check to see if the object is
		// linked.  If it is, then we compare the classes.  If
		// they aren't the same, then we just go to the next key.

		else if ( (!fIsLinkedObject) || (strcmp(pszCLSID, szClass) == 0) )
		{

			//Check for a \Conversion\Readable\Main entry
			strcpy(szFormatKey, szClass);
			strcat(szFormatKey, "\\Conversion\\Readable\\Main");

			dw=OLEUI_CCHKEYMAX;

			// Check to see if this class can read the original class
			// format.  If it can, add the string to the listbox as
			// CONVERT_LIST.

			lRet = RegQueryValue(hKey, szFormatKey, szFormat, &dw);

			if (ERROR_SUCCESS == lRet)
			{
				if (fIsOle1Object)
				{
					UpperText(szFormat, (short)(strlen(szFormat)));
					fIncluded = FormatIncludedSZ(szFormat, Format.lpszOle1Format);
				}
				else
					fIncluded = FormatIncludedDW(szFormat, Format.dwOle2Format);

				if (fIncluded) {
					dw = OLEUI_CCHKEYMAX;
					lRet = RegQueryValue(hKey, szClass, szHRClassName, &dw);
	
					if (ERROR_SUCCESS == lRet)
					{
	
						fEnableConvert = TRUE;
						break;  // STOP -- found one!
					}  // end if
				}

			} // end if
		} // end else
next:
		//Continue with the next key.
		lRet = RegEnumKey(hKey, cStrings++, szClass, OLEUI_CCHKEYMAX);

	}  // end while

	// Free the string we got from StringFromCLSID.
	// OLE2NOTE:  StringFromCLSID uses your IMalloc to alloc a
	// string, so you need to be sure to free the string you
	// get back, otherwise you'll have leaky memory.

	OleStdFreeString(pszCLSID, NULL);

	RegCloseKey(hKey);

	return fEnableConvert;
}
