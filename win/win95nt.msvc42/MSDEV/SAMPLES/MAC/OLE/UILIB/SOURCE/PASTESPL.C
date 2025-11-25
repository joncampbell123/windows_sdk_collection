/*
 * PASTESPL.C
 *
 * Implements the OleUIPasteSpecial function which invokes the complete
 * Paste Special dialog.
 *
 * Copyright (c)1992-1994 Microsoft Corporation, All Right Reserved
 */


#if defined(USEHEADER)
#include "OleHeaders.h"
#endif

#if defined(__MWERKS__) && !defined(__powerc)
#pragma pointers_in_D0
#endif


#if defined(_MSC_VER) //&& defined(__powerc)
#include <msvcmac.h>
#endif



#ifndef _MSC_VER
#ifndef THINK_C
#include <Strings.h>
#endif
#endif

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

#include <Events.h>
#include <Fonts.h>
#include <Menus.h>
#include <stdio.h>
#include <string.h>
#include <Scrap.h>

#include <ole2.h>

#include "ole2ui.h"
#include "common.h"
#include "uidebug.h"
#include "pastespl.h"

#ifdef __powerc

RoutineDescriptor gRDPasteSpecialDialogProc =
   BUILD_ROUTINE_DESCRIPTOR(uppModalFilterProcInfo, PasteSpecialDialogProc);
ModalFilterUPP gPasteSpecialDialogProc = &gRDPasteSpecialDialogProc;

RoutineDescriptor gRDPSUserItemProc =
   BUILD_ROUTINE_DESCRIPTOR(uppUserItemProcInfo, PSUserItemProc);
UserItemUPP gPSUserItemProc = &gRDPSUserItemProc;

#endif

OLEDBGDATA

#define ICONFOCUS		0		// this flag (if TRUE) allows the icon to receive focus and hence copy and paste

/*
 * OleUIPasteSpecial
 *
 * Purpose:
 *  Invokes the standard OLE Paste Special dialog box which allows the user
 *  to select the format of the clipboard object to be pasted or paste linked.
 *
 * Parameters:
 *  lpOPS           LPOLEUIPasteSpecial pointing to the in-out structure
 *                  for this dialog.
 *
 * Return Value:
 *  unsigned int    One of the following codes or one of the standard error codes (OLEUI_ERR_*)
 *                  defined in OLE2UI.H, indicating success or error:
 *                      OLEUI_OK                           User selected OK
 *                      OLEUI_CANCEL                       User cancelled the dialog
 *                      OLEUI_IOERR_SRCDATAOBJECTINVALID   lpSrcDataObject field of OLEUIPASTESPECIAL invalid
 *                      OLEUI_IOERR_ARRPASTEENTRIESINVALID arrPasteEntries field of OLEUIPASTESPECIAL invalid
 *                      OLEUI_IOERR_ARRLINKTYPESINVALID    arrLinkTypes field of OLEUIPASTESPECIAL invalid
 *                      OLEUI_PSERR_CLIPBOARDCHANGED       Clipboard contents changed while dialog was up
 */

#ifndef _MSC_VER
#pragma segment PasteSplSeg
#else
#pragma code_seg("PasteSplSeg", "SWAPPABLE")
#endif
STDAPI_(unsigned int) OleUIPasteSpecial(LPOLEUIPASTESPECIAL lpOPS)
{
	short 			nItem;
	DialogPtr		pDialog;
	PPASTESPECIAL	pPS;
	unsigned int	uRet;
	GrafPtr			gpSave;

#ifdef UIDLL
   short       hostResNum = 0;
#endif

	GetPort(&gpSave);

	InitCursor();

	//Validate pPS structure
	uRet = UStandardValidation((LPOLEUISTANDARD)lpOPS, sizeof(OLEUIPASTESPECIAL));

	if (uRet != OLEUI_SUCCESS) {
		SetPort(gpSave);
		return uRet;
	}

	pDialog = NULL;
	uRet = UStandardInvocation((LPOLEUISTANDARD)lpOPS, &pDialog, DIDPasteSpecial);

	if (uRet != OLEUI_SUCCESS) {
		SetPort(gpSave);
		return uRet;
	}

	uRet = UPasteSpecialInit(pDialog, lpOPS);

	if (uRet != OLEUI_SUCCESS)
	{
		PasteSpecialCleanup(pDialog);
		SetPort(gpSave);
		return uRet;
	}

	ShowWindow((WindowPtr)pDialog);
	pPS = (PPASTESPECIAL)GetWRefCon(pDialog);

	do
	{
#ifndef __powerc
		ModalDialog(PasteSpecialDialogProc, &nItem);
#else

#ifdef UIDLL
     hostResNum = SetUpOLEUIResFile();
#endif

		ModalDialog(gPasteSpecialDialogProc, &nItem);

#endif


		switch (nItem)
		{
#ifdef _DEBUG
			case PS_BTN_HELP:
				Assert(false);	//If you have this enabled, you must
								//intercept this in your hook
				break;
#endif //_DEBUG

			case PS_CBOX_DISPASICON:
				PSSetActiveItem(pDialog, pPS, PS_UITEM_LBOX);
				if (pPS->dwFlags & PSF_CHECKDISPLAYASICON)
					pPS->dwFlags &= ~PSF_CHECKDISPLAYASICON;
				else
					pPS->dwFlags |= PSF_CHECKDISPLAYASICON;
				PSUpdateDisplayAsIcon(pDialog, pPS);
				break;

			case PS_RBTN_PASTE:
				TogglePasteType(pDialog, pPS, PSF_SELECTPASTE);
				break;

			case PS_RBTN_PASTELINK:
				TogglePasteType(pDialog, pPS, PSF_SELECTPASTELINK);
				break;
		}
	} while (nItem != PS_BTN_OK && nItem != PS_BTN_CANCEL);

	// This is done is update the icon label in case the text edit was in focus
	// when the dialog is closed
	PSSetActiveItem(pDialog, pPS, PS_UITEM_LBOX);
	
	lpOPS->dwFlags = pPS->dwFlags;
	lpOPS->nSelectedIndex = pPS->nSelectedIndex;
	lpOPS->fLink = pPS->fLink;
	
	if (pPS->dwFlags & PSF_CHECKDISPLAYASICON) {
		if (lpOPS->fLink)
			lpOPS->hMetaPict = OlePictFromIconAndLabel(pPS->hIconLSD, pPS->szIconLabelLSD, NULL);
		else
			lpOPS->hMetaPict = OlePictFromIconAndLabel(pPS->hIconOD, pPS->szIconLabelOD, NULL);
	}
	else
		lpOPS->hMetaPict = NULL;
	
	PasteSpecialCleanup(pDialog);

	uRet = (nItem == PS_BTN_OK ? OLEUI_OK : OLEUI_CANCEL);

	SetPort(gpSave);
#ifdef UIDLL
      ClearOLEUIResFile(hostResNum);
#endif
	return uRet;
}




/*
 * UPasteSpecialInit
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment PasteSplSeg
#else
#pragma code_seg("PasteSplSeg", "SWAPPABLE")
#endif
unsigned int UPasteSpecialInit(DialogPtr pDialog, LPOLEUIPASTESPECIAL lpOPS)
{
	PPASTESPECIAL			pPS;
	short					itype;
	Handle					h;
	Rect					rc;
	STGMEDIUM				medium;
	LPOBJECTDESCRIPTOR		lpOD;
	LPLINKSRCDESCRIPTOR		lpLSD;
	ResType					cfFormat;
	ListHandle				hList;
	short					cch;
	Cell					cell;
	PASTELISTITEMDATA		ItemData;
	unsigned int			uRet;
	PicHandle				hPict;
	unsigned long			lsize;

#ifdef UIDLL
   short                hostResNum = 0;
#endif

	//1. Copy the structure at lpOPS
	pPS = (PPASTESPECIAL)PvStandardInit(pDialog, sizeof(PASTESPECIAL));

	//PvStandardInit failed
	if (pPS == NULL)
		return OLEUI_ERR_GLOBALMEMALLOC;

#ifdef UIDLL
   hostResNum = SetUpOLEUIResFile();
#endif

	ASSERTCOND(pPS == (PPASTESPECIAL)GetWRefCon(pDialog));

	//2. Initialize all the user items in our dialog box.
	uRet = PSUInitUserItems(pDialog);

	if (uRet != OLEUI_SUCCESS)
   {
#ifdef UIDLL
      ClearOLEUIResFile(hostResNum);
#endif
      return uRet;
   }
		

	//3. Save the original pointer and copy necessary information.
	pPS->lpOPS   = lpOPS;
	pPS->dwFlags = lpOPS->dwFlags;

	// Hide the help button if required
	if (!(lpOPS->dwFlags & PSF_SHOWHELP))
		HideDItem(pDialog, PS_BTN_HELP);

	// PSF_CHECKDISPLAYASICON is an OUT flag. Clear it if has been set on the way in.
	pPS->dwFlags = pPS->dwFlags & ~PSF_CHECKDISPLAYASICON;

	if (lpOPS->lpszCaption != NULL)
		SetWTitle(pDialog, (StringPtr)lpOPS->lpszCaption);

	// Load 'Unknown Source' and 'Unknown Type' strings

	GetIndString((unsigned char *)pPS->szUnknownType, DIDPasteSpecial, IDS_PSUNKNOWNTYPE);
	p2cstr((unsigned char *)pPS->szUnknownType);

	GetIndString((unsigned char *)pPS->szUnknownSource, DIDPasteSpecial, IDS_PSUNKNOWNSRC);
	p2cstr((unsigned char *)pPS->szUnknownSource);

	pPS->szAppName[0] = 0;

	// GetData CF_OBJECTDESCRIPTOR. If the object on the clipboard in an OLE1 object (offering CF_OWNERLINK),
	// an OBJECTDESCRIPTOR will be created from CF_OWNERLINK. See OBJECTDESCRIPTOR for more info.
	pPS->hObjDesc = OleStdFillObjectDescriptorFromData(lpOPS->lpSrcDataObj, &medium, &cfFormat);
	if (pPS->hObjDesc)
	{
		HLock(pPS->hObjDesc);
		lpOD = (LPOBJECTDESCRIPTOR)*pPS->hObjDesc;

		// Get FullUserTypeName, SourceOfCopy and CLSID
        if (lpOD->dwFullUserTypeName)
            pPS->szFullUserTypeNameOD = (char *)lpOD+lpOD->dwFullUserTypeName;
        else pPS->szFullUserTypeNameOD = pPS->szUnknownType;

        if (lpOD->dwSrcOfCopy)
            pPS->szSourceOfDataOD = (char *)lpOD+lpOD->dwSrcOfCopy;
        else pPS->szSourceOfDataOD = pPS->szUnknownSource;

		pPS->clsidOD = lpOD->clsid;

		// Does source specify DVASPECT_ICON?
		pPS->fSrcAspectIconOD = (lpOD->dwDrawAspect & DVASPECT_ICON) != 0;

		// Does source specify OLEMISC_ONLYICONIC?
		pPS->fSrcOnlyIconicOD = (lpOD->dwStatus & OLEMISC_ONLYICONIC) != 0;

		// Get application name of source from auxusertype3 in the registration database
		if (OleStdGetAuxUserType(&pPS->clsidOD, 3, pPS->szAppName, OLEUI_CCHKEYMAX, (HKEY)NULL) == 0)
		{
			// Use "the application which created it" as the name of the application
			GetIndString((unsigned char *)pPS->szAppName, DIDPasteSpecial, IDS_PSUNKNOWNAPP);
			p2cstr((unsigned char *)pPS->szAppName);
		}

		hPict = NULL;
		
		// Retrieve an icon from the object
		if (pPS->fSrcAspectIconOD)
			hPict = (PicHandle)OleStdGetData(lpOPS->lpSrcDataObj, 'PICT', NULL, DVASPECT_ICON, &medium);

		// If object does not offer icon, obtain it from the CLSID
		if (NULL == hPict)
			hPict = OleGetIconOfClass(&pPS->clsidOD, NULL, true);
			
		if (hPict) {
			pPS->hIconOD = OleUIPictExtractIcon(hPict);
			lsize = sizeof(pPS->szIconLabelOD);
			lsize = OleUIPictExtractLabel(hPict, pPS->szIconLabelOD, lsize);
			pPS->szIconLabelOD[lsize] = '\0';
			OleUIPictIconFree(hPict);
		}

	}

    // Does object offer CF_LINKSRCDESCRIPTOR?
	pPS->hLinkSrcDesc = OleStdGetData(lpOPS->lpSrcDataObj, cfLinkSrcDescriptor, NULL, DVASPECT_CONTENT, &medium);
    if (pPS->hLinkSrcDesc)
    {
        // Get FullUserTypeName, SourceOfCopy and CLSID
        HLock(pPS->hLinkSrcDesc);
        lpLSD = (LPLINKSRCDESCRIPTOR)*pPS->hLinkSrcDesc;
        if (lpLSD->dwFullUserTypeName)
            pPS->szFullUserTypeNameLSD = (char *)lpLSD+lpLSD->dwFullUserTypeName;
        else pPS->szFullUserTypeNameLSD = pPS->szUnknownType;

        if (lpLSD->dwSrcOfCopy)
            pPS->szSourceOfDataLSD = (char *)lpLSD+lpLSD->dwSrcOfCopy;
        else pPS->szSourceOfDataLSD = pPS->szUnknownSource;

        pPS->clsidLSD = lpLSD->clsid;

        // Does source specify DVASPECT_ICON?
        if (lpLSD->dwDrawAspect & DVASPECT_ICON)
           pPS->fSrcAspectIconOD = true;
        else pPS->fSrcAspectIconOD = false;

        // Does source specify OLEMISC_ONLYICONIC?
        if (lpLSD->dwStatus & OLEMISC_ONLYICONIC)
            pPS->fSrcOnlyIconicLSD = true;
        else pPS->fSrcOnlyIconicLSD = false;

		hPict = NULL;

        // Retrieve an icon from the object
        if (pPS->fSrcAspectIconLSD)
			hPict = (PicHandle)OleStdGetData(lpOPS->lpSrcDataObj, 'PICT', NULL,	DVASPECT_ICON, &medium);

		// If object does not offer icon, obtain it from the CLSID
		if (NULL == hPict)
			hPict = OleGetIconOfClass(&pPS->clsidLSD, NULL, true);

		if (hPict) {
			pPS->hIconLSD = OleUIPictExtractIcon(hPict);
			lsize = sizeof(pPS->szIconLabelLSD);
			lsize = OleUIPictExtractLabel(hPict, pPS->szIconLabelLSD, lsize);
			pPS->szIconLabelLSD[lsize] = '\0';
			OleUIPictIconFree(hPict);
		}
    }
    else if (pPS->hObjDesc)     // Does not offer CF_LINKSRCDESCRIPTOR but offers CF_OBJECTDESCRIPTOR
    {
        // Copy the values of OBJECTDESCRIPTOR
        pPS->szFullUserTypeNameLSD = pPS->szFullUserTypeNameOD;
        pPS->szSourceOfDataLSD     = pPS->szSourceOfDataOD;
        pPS->clsidLSD              = pPS->clsidOD;
        pPS->fSrcAspectIconLSD     = pPS->fSrcAspectIconOD;
        pPS->fSrcOnlyIconicLSD     = pPS->fSrcOnlyIconicOD;

		hPict = NULL;
		
        // Don't copy the hMetaPict; instead get a separate copy
        if (pPS->fSrcAspectIconLSD)
            hPict = (PicHandle)OleStdGetData(lpOPS->lpSrcDataObj, 'PICT', NULL, DVASPECT_ICON, &medium);

		if (hPict) {
			pPS->hIconLSD = OleUIPictExtractIcon(hPict);
			lsize = sizeof(pPS->szIconLabelLSD);
			lsize = OleUIPictExtractLabel(hPict, pPS->szIconLabelLSD, lsize);
			pPS->szIconLabelLSD[lsize] = '\0';
			OleUIPictIconFree(hPict);
		}
        else {
	        // If object does not offer icon, obtain it from the CLSID
			pPS->szIconLabelLSD[0] = '\0';
            pPS->hIconLSD = OleGetIconOfClass(&pPS->clsidLSD, NULL, false);
        }
    }

    // Not an OLE object
    if (pPS->hObjDesc == NULL && pPS->hLinkSrcDesc == NULL)
    {
         pPS->szFullUserTypeNameLSD = pPS->szFullUserTypeNameOD = pPS->szUnknownType;
         pPS->szSourceOfDataLSD = pPS->szSourceOfDataOD = pPS->szUnknownSource;
         pPS->hIconLSD = pPS->hIconOD = NULL;
         pPS->szIconLabelLSD[0] = pPS->szIconLabelLSD[0] = '\0';
    }

    // Allocate scratch memory to construct item names in the paste and pastelink listboxes
    pPS->hBuff = AllocateScratchMem(pPS);
    if (pPS->hBuff == NULL)
    {
#ifdef UIDLL
      ClearOLEUIResFile(hostResNum);
#endif
      return OLEUI_ERR_GLOBALMEMALLOC;
    }


    // Select the Paste Link Button if specified. Otherwise select
    //      Paste Button by default
    if (pPS->dwFlags & PSF_SELECTPASTELINK)
        pPS->dwFlags = (pPS->dwFlags & ~PSF_SELECTPASTE) | PSF_SELECTPASTELINK;
    else
        pPS->dwFlags = (pPS->dwFlags & ~PSF_SELECTPASTELINK) | PSF_SELECTPASTE;

    // Mark which PasteEntry formats are available from source data object
    OleStdMarkPasteEntryList(lpOPS->lpSrcDataObj,lpOPS->arrPasteEntries,lpOPS->cPasteEntries);

	// If an icon is passed along with the source object, allow the user to change the icon
	GetDItem(pDialog, PS_CBOX_DISPASICON, &itype, &h, &rc);

	if (!pPS->hIconOD)		// Icon not passed along with source object
	{
		// Disable DisplayAsIcon
		SetCtlValue((ControlHandle)h, false);
		HideDItem(pDialog, PS_CBOX_DISPASICON);
	}

	// Select the Paste Link Button if specified. Otherwise select
	//		Paste Button by default
	if (pPS->dwFlags & PSF_SELECTPASTELINK)
		pPS->dwFlags = (pPS->dwFlags & ~PSF_SELECTPASTE) | PSF_SELECTPASTELINK;
	else
		pPS->dwFlags = (pPS->dwFlags & ~PSF_SELECTPASTELINK) | PSF_SELECTPASTE;

	//Check if items are available to be pasted
	if (!FFillPasteList(pPS))
	{
		pPS->dwFlags &= ~PSF_SELECTPASTE;
		PSUpdateOneControl(pDialog, PS_RBTN_PASTE, false);	//Disable if no paste items
	}

	if (!FFillPasteLinkList(pPS))
	{
		pPS->dwFlags &= ~PSF_SELECTPASTELINK;
		PSUpdateOneControl(pDialog, PS_RBTN_PASTELINK, false);	//Disable if no link items
	}

	if (pPS->dwFlags & PSF_SELECTPASTE)
	{
		pPS->dwFlags &= ~PSF_SELECTPASTE;
		CheckRadioButton(pDialog, PS_RBTN_PASTE, PS_RBTN_PASTELINK, PS_RBTN_PASTE);
		TogglePasteType(pDialog, pPS, PSF_SELECTPASTE);
	}
	else if (pPS->dwFlags & PSF_SELECTPASTELINK)
	{
		pPS->dwFlags &= ~PSF_SELECTPASTELINK;
		CheckRadioButton(pDialog, PS_RBTN_PASTE, PS_RBTN_PASTELINK, PS_RBTN_PASTELINK);
		TogglePasteType(pDialog, pPS, PSF_SELECTPASTELINK);
	}

	pPS->iActiveItem = PS_UITEM_LBOX;		// initial active item
	HideDItem(pDialog, PS_TE_LABELEDIT);	// hide the edit control

	// Initialize nSelectedIndex with the current PasteEntries index
	hList = (pPS->dwFlags & PSF_SELECTPASTE ? pPS->hListPaste : pPS->hListPasteLink);
	pPS->fItemSelected = FLGetSelectedCell(hList, &cell);
	if (pPS->fItemSelected)
	{
		cell.h++;
		cch = sizeof(PASTELISTITEMDATA);
		LGetCell((Ptr)&ItemData, &cch, cell, hList);
		pPS->nSelectedIndex = ItemData.nPasteEntriesIndex;
	}

	PSUpdateAllControls(pDialog, pPS);
	PSUpdateResults(pDialog, pPS);

#ifdef UIDLL
      ClearOLEUIResFile(hostResNum);
#endif

	return OLEUI_SUCCESS;
}




/*
 * AllocateScratchMem
 *
 * Purpose:
 *  Allocates scratch memory for use by the PasteSpecial dialog. The memory is
 *  is used as the buffer for building up strings using wsprintf. Strings are built up
 *  using the buffer while inserting items into the Paste & PasteLink lists and while
 *  setting the help result text. It must be big  enough to handle the string that results after
 *  replacing the %s in the lpstrFormatName and lpstrResultText in arrPasteEntries[]
 *  by the FullUserTypeName. It must also be big enough to build the dialog's result text
 *  after %s substitutions by the FullUserTypeName or the ApplicationName.
 *
 * Parameters:
 *  pPS             Paste Special Dialog Structure
 *
 * Return Value:
 *  HGLOBAL         Handle to allocated global memory
 */

#ifndef _MSC_VER
#pragma segment PasteSplSeg
#else
#pragma code_seg("PasteSplSeg", "SWAPPABLE")
#endif
Handle AllocateScratchMem(PPASTESPECIAL pPS)
{
    LPOLEUIPASTESPECIAL	lpOPS = pPS->lpOPS;
    int					nLen, i;
    int					nSubstitutedText = 0;
    int					nAlloc = 0;

    // Get the maximum length of the FullUserTypeNames specified by OBJECTDESCRIPTOR
    //   and the LINKSRCDESCRIPTOR and the Application Name. Any of these may be substituted
    //   for %s in the result-text/list entries.
    if (pPS->szFullUserTypeNameOD)
        nSubstitutedText = strlen(pPS->szFullUserTypeNameOD);
    if (pPS->szFullUserTypeNameLSD)
        nSubstitutedText = MAX(nSubstitutedText, strlen(pPS->szFullUserTypeNameLSD));
    if (pPS->szAppName)
        nSubstitutedText = MAX(nSubstitutedText, strlen(pPS->szAppName));

    // Get the maximum length of lpstrFormatNames & lpstrResultText in arrPasteEntries
    nLen = 0;
    for (i = 0; i < lpOPS->cPasteEntries; i++)
    {
       nLen = MAX(nLen, strlen(lpOPS->arrPasteEntries[i].lpstrFormatName));
       nLen = MAX(nLen, strlen(lpOPS->arrPasteEntries[i].lpstrResultText));
    }

    // Get the maximum length of lpstrFormatNames and lpstrResultText after %s  has
    //   been substituted (At most one %s can appear in each string).
    //   Add 1 to hold NULL terminator.
    nAlloc = nLen+nSubstitutedText+1;

    // Allocate scratch memory to be used to build strings
    // nAlloc is big enough to hold any of the lpstrResultText or lpstrFormatName in arrPasteEntries[]
    //   after %s substitution.
    // We also need space to build up the help result text. 512 is the maximum length of the
    //   standard dialog help text before substitutions. 512+nAlloc is the maximum length
    //   after %s substition.
    // PSUpdateResults() requires 4 such buffers to build up the result text
    return NewHandle((long)4*(512+nAlloc));
}




/*
 * PSUInitUserItems
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment PasteSplSeg
#else
#pragma code_seg("PasteSplSeg", "SWAPPABLE")
#endif
unsigned int PSUInitUserItems(DialogPtr pDialog)
{
	PPASTESPECIAL	pPS;
	short			iitem;
	short			itype;
	Handle			h;
	Rect			rc,
					rcDataBounds;
	Point			ptSize;
	short			theProc;

	//This assumes that the user items are grouped together and that
	//PS_UITEM_FIRST and PS_UITEM_LAST are properly defined
	for (iitem = PS_UITEM_FIRST; iitem <= PS_UITEM_LAST; iitem++)
	{
		GetDItem(pDialog, iitem, &itype, &h, &rc);
		if ((itype & ~itemDisable) == userItem)
		{
#ifndef __powerc
			SetDItem(pDialog, iitem, itype, (Handle)PSUserItemProc, &rc);
#else
		   SetDItem(pDialog, iitem, itype, (Handle)gPSUserItemProc, &rc);
#endif
		}
#ifdef _DEBUG
		else
			ASSERTCOND(false);
#endif //_DEBUG
	}

	pPS = (PPASTESPECIAL)GetWRefCon(pDialog);

	//Create listbox
	GetDItem(pDialog, PS_UITEM_LBOX, &itype, &h, &rc);
	InsetRect(&rc, 1, 1);
	rc.right -= 15;							//Add space for vertical scroll bar
	SetRect(&rcDataBounds, 0, 0, 2, 0);		//Two column listbox
	SetPt(&ptSize, (short)(rc.right-rc.left), 0);	//First column fills width of list (second isn't visible)

	//CUSTOM: Add your own ldef here
	theProc = 0;
	pPS->hListPaste = LNew(&rc, &rcDataBounds, ptSize, theProc, (WindowPtr)pDialog, true, false, false, true);

	if (pPS->hListPaste == NULL)
		return OLEUI_ERR_GLOBALMEMALLOC;

	(**pPS->hListPaste).selFlags = lOnlyOne|lNoNilHilite;

	//CUSTOM: Add your own ldef here
	theProc = 0;
	pPS->hListPasteLink = LNew(&rc, &rcDataBounds, ptSize, theProc, (WindowPtr)pDialog, true, false, false, true);

	if (pPS->hListPasteLink == NULL){
		LDispose(pPS->hListPaste);
		return OLEUI_ERR_GLOBALMEMALLOC;
	}

	(**pPS->hListPasteLink).selFlags = lOnlyOne|lNoNilHilite;

	return OLEUI_SUCCESS;
}




/*
 * FAddPasteListItem
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment PasteSplSeg
#else
#pragma code_seg("PasteSplSeg", "SWAPPABLE")
#endif
Boolean FAddPasteListItem(ListHandle hList, Boolean fInsertFirst, Boolean fHilite,
					   int nPasteEntriesIndex, PPASTESPECIAL pPS, char *lpszBuf,
					   char *lpszFullUserTypeName)
{
	LPOLEUIPASTESPECIAL	lpOPS = pPS->lpOPS;
	PASTELISTITEMDATA	ItemData;
	Cell				cell = { 0, 0 };

    // Fill data associated with each list box item
    ItemData.nPasteEntriesIndex = nPasteEntriesIndex;
    ItemData.fCntrEnableIcon = (lpOPS->arrPasteEntries[nPasteEntriesIndex].dwFlags &
    							OLEUIPASTE_ENABLEICON) != 0;

	sprintf(lpszBuf, lpOPS->arrPasteEntries[nPasteEntriesIndex].lpstrFormatName, lpszFullUserTypeName);

	// If this results in a blank entry exit out and don't add to the list
	if (lpszBuf[0] == '\0')
		return false;

	// If fInsertFirst is false, insert at end of list
	if (!fInsertFirst)
		cell.v = (**hList).dataBounds.bottom;

  	LAddRow(1, cell.v, hList);
  	LSetCell((Ptr)lpszBuf, (short)strlen(lpszBuf), cell, hList);

	if (fHilite)
		LSetSelect(true, cell, hList);

	cell.h++;
  	LSetCell((Ptr)&ItemData, sizeof(PASTELISTITEMDATA), cell, hList);

	return true;
}




/*
 * FFillPasteList
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment PasteSplSeg
#else
#pragma code_seg("PasteSplSeg", "SWAPPABLE")
#endif
Boolean FFillPasteList(PPASTESPECIAL pPS)
{
	LPOLEUIPASTESPECIAL	lpOPS = pPS->lpOPS;
	LPMALLOC			pIMalloc = NULL;
	char				*lpszBuf = NULL;
	ListHandle			hList = NULL;
	int					i;
	Boolean				fTryObjFmt = false;
	Boolean				fHilite = true;
	Boolean				fInsertFirst;
	HRESULT				hrErr;

	hrErr = CoGetMalloc(MEMCTX_TASK, &pIMalloc);
	if (hrErr != NOERROR)
		goto error;

	HLock(pPS->hBuff);
	lpszBuf = *pPS->hBuff;
	hList   = pPS->hListPaste;

	// Loop over the target's priority list of formats
	for (i = 0; i < lpOPS->cPasteEntries; i++)
	{
		if (lpOPS->arrPasteEntries[i].dwFlags != OLEUIPASTE_PASTEONLY &&
			!(lpOPS->arrPasteEntries[i].dwFlags & OLEUIPASTE_PASTE))
			continue;

		fInsertFirst = false;

		if (lpOPS->arrPasteEntries[i].fmtetc.cfFormat == cfEmbeddedObject ||
			lpOPS->arrPasteEntries[i].fmtetc.cfFormat == cfEmbedSource)
		{
			if (!fTryObjFmt)
			{
				fTryObjFmt = true;		// only use 1st object format
				fInsertFirst = true;	// OLE obj format should always be 1st
			}
			else
				continue;   // already added an object format to list
		}

		// add to list if entry is marked true
		if (lpOPS->arrPasteEntries[i].dwScratchSpace)
		{
			if (FAddPasteListItem(hList, fInsertFirst, fHilite, i, pPS, lpszBuf, pPS->szFullUserTypeNameOD))
				fHilite = false;
		}
	}

	// Clean up
	if (pIMalloc)
		OleStdRelease((LPUNKNOWN)pIMalloc);
	if (lpszBuf)
	   HUnlock(pPS->hBuff);

	// If no items have been added to the list box (none of the formats
	//   offered by the source matched those acceptable to the container),
	//   return false
	return ((**hList).dataBounds.bottom > 0);

error:
	if (pIMalloc)
		OleStdRelease((LPUNKNOWN)pIMalloc);
	if (lpszBuf)
		HUnlock(pPS->hBuff);
	LDispose(hList);

	return false;
}




/*
 * FFillPasteLinkList
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment PasteSplSeg
#else
#pragma code_seg("PasteSplSeg", "SWAPPABLE")
#endif
Boolean FFillPasteLinkList(PPASTESPECIAL pPS)
{
	LPOLEUIPASTESPECIAL	lpOPS		 = pPS->lpOPS;
	LPDATAOBJECT		lpSrcDataObj = lpOPS->lpSrcDataObj;
	LPENUMFORMATETC		lpEnumFmtEtc = NULL;
	LPMALLOC			pIMalloc	 = NULL;
	char				*lpszBuf = NULL;
	OLEUIPASTEFLAG		pasteFlag;
	unsigned int		arrLinkTypesSupported[PS_MAXLINKTYPES]; // Array of flags that
																// indicate which link types
																// are supported by source.
	FORMATETC			fmtetc;
	int					i, j;
	Boolean				fLinkTypeSupported = false;
	ListHandle			hList = NULL;
	int					nDefFormat = -1;
	Boolean				fTryObjFmt = false;
	Boolean				fHilite = true;
	Boolean				fInsertFirst;
	char				*szFullUserTypeName = (pPS->fLink ? pPS->szFullUserTypeNameLSD : pPS->szFullUserTypeNameOD);
	HRESULT				hrErr;

	hrErr = CoGetMalloc(MEMCTX_TASK, &pIMalloc);
	if (hrErr != NOERROR)
		goto error;

	HLock(pPS->hBuff);
	lpszBuf = *pPS->hBuff;
	hList   = pPS->hListPasteLink;

	// Remember which link type formats are offered by lpSrcDataObj.
    OleStdMemSet(&fmtetc, 0, sizeof(FORMATETC));		// the standard memset in the library doesn't work
	for (i = 0; i < lpOPS->cLinkTypes; i++)
	{
		if (lpOPS->arrLinkTypes[i] == cfLinkSource)
		{
			OLEDBG_BEGIN2("OleQueryLinkFromData called\r\n")
			hrErr = OleQueryLinkFromData(lpSrcDataObj);
			OLEDBG_END2
			if(NOERROR == hrErr)
			{
				arrLinkTypesSupported[i] = 1;
				fLinkTypeSupported = true;
			}
			else arrLinkTypesSupported[i] = 0;
		}
		else
		{
			fmtetc.cfFormat = lpOPS->arrLinkTypes[i];
			fmtetc.dwAspect = DVASPECT_CONTENT;
			fmtetc.tymed	= 0xFFFFFFFF;	   // All tymed values
			fmtetc.lindex   = -1;
			OLEDBG_BEGIN2("IDataObject::QueryGetData called\r\n")
			hrErr = lpSrcDataObj->lpVtbl->QueryGetData(lpSrcDataObj,&fmtetc);
			OLEDBG_END2
			if(NOERROR == hrErr)
			{
				arrLinkTypesSupported[i] = 1;
				fLinkTypeSupported = true;
			}
			else arrLinkTypesSupported[i] = 0;
		}
	}
	// No link types are offered by lpSrcDataObj
	if (!fLinkTypeSupported)
		goto cleanup;

	// Enumerate the formats acceptable to container
	for (i = 0; i < lpOPS->cPasteEntries; i++)
	{
		fLinkTypeSupported = false;

		// If container will accept any link type offered by source object
		if (lpOPS->arrPasteEntries[i].dwFlags & OLEUIPASTE_LINKANYTYPE)
			fLinkTypeSupported = true;
		else
		{
			// Check if any of the link types offered by the source
			//	object are acceptable to the container
			// This code depends on the LINKTYPE enum values being powers of 2
			for (pasteFlag = OLEUIPASTE_LINKTYPE1, j = 0;
				 j < lpOPS->cLinkTypes;
				 pasteFlag*=2, j++)
			{
				if ((lpOPS->arrPasteEntries[i].dwFlags & pasteFlag) &&
						arrLinkTypesSupported[j])
				{
					fLinkTypeSupported = true;
					break;
				}
			}
		}

		fInsertFirst = false;

		if (lpOPS->arrPasteEntries[i].fmtetc.cfFormat == cfFileName ||
			lpOPS->arrPasteEntries[i].fmtetc.cfFormat == cfLinkSource)
		{
			if (! fTryObjFmt)
			{
				fTryObjFmt = true;	  // only use 1st object format
				fInsertFirst = true;	// OLE obj format should always be 1st
			}
			else
				continue;   // already added an object format to list
		}

		// add to list if entry is marked true
		if (fLinkTypeSupported && lpOPS->arrPasteEntries[i].dwScratchSpace)
		{
			if (FAddPasteListItem(hList, fInsertFirst, fHilite, i, pPS, lpszBuf, pPS->szFullUserTypeNameLSD))
				fHilite = false;
		}
	} // end FOR

cleanup:
	// Clean up
	if (pIMalloc)
		OleStdRelease((LPUNKNOWN)pIMalloc);
	if (lpszBuf)
		HUnlock(pPS->hBuff);

	// If no items have been added to the list box (none of the formats
	//   offered by the source matched those acceptable to the destination),
	//   return false
	return ((**hList).dataBounds.bottom > 0);

error:
	if (pIMalloc)
		OleStdRelease((LPUNKNOWN)pIMalloc);
	if (lpszBuf)
		HUnlock(pPS->hBuff);
	LDispose(hList);

	return false;
}




/*
 * FHasPercentS
 *
 * Purpose:
 *  Determines if string contains %s.
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment PasteSplSeg
#else
#pragma code_seg("PasteSplSeg", "SWAPPABLE")
#endif
Boolean FHasPercentS(char *lpsz, PPASTESPECIAL pPS)
{
	int		n = 0;
	char	*lpszTmp;

	if (!lpsz) return false;

	HLock(pPS->hBuff);
	lpszTmp = *pPS->hBuff;
	strcpy(lpszTmp, lpsz);

	while (*lpszTmp)
	{
		if (*lpszTmp == '%')
		{
			lpszTmp++;
			if (*lpszTmp == 's')			// If %s, return
			{
				HUnlock(pPS->hBuff);
				return true;
			}
			else if (*lpszTmp == '%')		// if %%, skip to next character
				lpszTmp++;
		}
		else lpszTmp++;
	}

	HUnlock(pPS->hBuff);
	return false;
}




/*
 * PasteSpecialDialogProc
 *
 * Purpose:
 *  Implements the OLE Paste Special dialog as invoked through the
 *  OleUIPasteSpecial function.
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment PasteSplSeg
pascal Boolean
#else
#pragma code_seg("PasteSplSeg", "SWAPPABLE")
Boolean __pascal
#endif
PasteSpecialDialogProc(DialogPtr pDialog, EventRecord *pEvent, short *nItem)
{
	PPASTESPECIAL		pPS;
	GrafPtr				gpSave;
	Boolean				fHooked;
	WindowPtr			pWindow;
	short				in;
	Point				ptWhere;
	char				chKey;
	ListHandle			hList;
	PASTELISTITEMDATA	ItemData;
	short				cch;
	Cell				cell;
	Cell				cellSelect;
	Boolean				fShifted;
	short				itype;
	Handle				h;
	Rect				rc;

	pPS = (PPASTESPECIAL)GetWRefCon(pDialog);

	GetPort(&gpSave);
	SetPort(pDialog);

	fHooked = FStandardHook((LPOLEUISTANDARD)pPS->lpOPS, pDialog, pEvent, nItem, pPS->lpOPS->lCustData);
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
			if ((pWindow == (WindowPtr)pDialog) && (in == inDrag) && (pPS->lpOPS->lpfnHook != NULL))
			{
				DragWindow(pDialog, pEvent->where, &qd.screenBits.bounds);
				SetPort(gpSave);
				return true;
			}

			ptWhere = pEvent->where;
			GlobalToLocal(&ptWhere);

			switch (FindDItem(pDialog, ptWhere) + 1)
			{
				case PS_UITEM_LBOX:
					PSSetActiveItem(pDialog, pPS, PS_UITEM_LBOX);
					hList = (pPS->dwFlags & PSF_SELECTPASTE ? pPS->hListPaste : pPS->hListPasteLink);

					FLGetSelectedCell(hList, &cellSelect);
					if (LClick(ptWhere, pEvent->modifiers, hList))
					{
						FlashButton(pDialog, PS_BTN_OK);
						*nItem = PS_BTN_OK;
						SetPort(gpSave);
						return true;
					}

					pPS->fItemSelected = FLGetSelectedCell(hList, &cell);
					if (pPS->fItemSelected)
					{
						cell.h++;
						cch = sizeof(PASTELISTITEMDATA);
						LGetCell((Ptr)&ItemData, &cch, cell, hList);
						pPS->nSelectedIndex = ItemData.nPasteEntriesIndex;
					}
					
					PSUpdateAllControls(pDialog, pPS);
					
					if (cellSelect.v != cell.v) {		// different item is selected
						PSUpdateResults(pDialog, pPS);
					}
					break;
					
#if ICONFOCUS					
				case PS_UITEM_ICON:
					PSSetActiveItem(pDialog, pPS, PS_UITEM_ICON);
					break;
#endif
					
				case PS_UITEM_ICONTXT:
					PSSetActiveItem(pDialog, pPS, PS_UITEM_ICONTXT);
					SetPort(gpSave);
					return true;
			}
			break;

		case keyDown:
		case autoKey:
			chKey = pEvent->message & charCodeMask;
			switch (chKey)
			{
				case ENTERKEY:
				case RETURNKEY:
					GetDItem(pDialog, PS_BTN_OK, &itype, &h, &rc);
					if ((**((ControlHandle)h)).contrlHilite == 0)
					{
						FlashButton(pDialog, PS_BTN_OK);
						*nItem = PS_BTN_OK;
					}
					SetPort(gpSave);
					return true;

				case PERIODKEY:
					if (!(pEvent->modifiers & cmdKey))
						break;
					//Else fall through...

				case ESCAPEKEY:
					FlashButton(pDialog, PS_BTN_CANCEL);
					*nItem = PS_BTN_CANCEL;
					SetPort(gpSave);
					return true;

				case UPKEY:
				case DOWNKEY:
					if (pPS->iActiveItem == PS_UITEM_LBOX)
					{
						hList = (pPS->dwFlags & PSF_SELECTPASTE ? pPS->hListPaste : pPS->hListPasteLink);
						if ((**hList).dataBounds.bottom > 0)
						{
							fShifted = false;
							SetPt(&cell, 0, 0);
							if (LGetSelect(true, &cell, hList))
							{
								if ((chKey == UPKEY && cell.v > 0) ||
									(chKey == DOWNKEY && cell.v < (**hList).dataBounds.bottom - 1))
								{
									LSetSelect(false, cell, hList);
									cell.v += (chKey == UPKEY ? -1 : 1);
									LSetSelect(true, cell, hList);
	
									//If the selection is outside the list's view, scroll it into view
									LRect(&rc, cell, hList);
									if ((chKey == UPKEY && rc.top < (**hList).rView.top) ||
										(chKey == DOWNKEY && rc.bottom > (**hList).rView.bottom))
										LScroll(0, (short)(chKey == UPKEY ? -1 : 1), hList);
									fShifted = true;
								}
							}
							else
							{
								SetPt(&cell, 0, 0);
								LSetSelect(true, cell, hList);
								LAutoScroll(hList);
								fShifted = true;
							}

                     cell.h++;                           // DavidWor Fix
							cch = sizeof(PASTELISTITEMDATA);
							LGetCell((Ptr)&ItemData, &cch, cell, hList);
							pPS->nSelectedIndex = ItemData.nPasteEntriesIndex;
							pPS->fItemSelected = true;
	
							if (fShifted)
							{
								PSUpdateAllControls(pDialog, pPS);
								PSUpdateResults(pDialog, pPS);
							}
						}
					}
					break;
					
				case TABKEY:
					PSTabFocus(pDialog, pPS);
					break;
					
				case VKEY:
				case CKEY:
					if (pPS->iActiveItem == PS_UITEM_ICON) {
						PicHandle hIcon;
						
						// only concerned w/ cut and paste key sequences
						if (pEvent->modifiers & cmdKey) {
							hIcon = (pPS->dwFlags & PSF_SELECTPASTE) ? pPS->hIconOD : pPS->hIconLSD;
								
							if(chKey == CKEY) {
								if(hIcon) {
									HLock((Handle)hIcon);
									ZeroScrap();
									PutScrap(GetHandleSize((Handle)hIcon), 'PICT', (Ptr)(*hIcon));
									HUnlock((Handle)hIcon);
								}
							}
							else if(chKey == VKEY) {
								long lOffset, lPictSize;
								
								lPictSize = GetScrap(nil, 'PICT', &lOffset);
		
								if(lPictSize > 0) {
									if(hIcon) {
										KillPicture(hIcon);
										if (pPS->dwFlags & PSF_SELECTPASTE)
											pPS->hIconOD = NULL;
										else
											pPS->hIconLSD = NULL;
									}
									hIcon = (PicHandle)NewHandle(lPictSize);
									if(hIcon) {
										GetScrap((Handle)hIcon, 'PICT', &lOffset);
										if (pPS->dwFlags & PSF_SELECTPASTE)
											pPS->hIconOD = OlePictFromIconAndLabel(hIcon, NULL, NULL);
										else
											pPS->hIconLSD = OlePictFromIconAndLabel(hIcon, NULL, NULL);
										DisposeHandle((Handle)hIcon);
										GetDItem(pDialog, PS_UITEM_ICON, &itype, &h, &rc);
										EraseRect(&rc);
										InvalRect(&rc);
									}
								}
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
#pragma segment PasteSplSeg
pascal void
#else
#pragma code_seg("PasteSplSeg", "SWAPPABLE")
void __pascal
#endif
PSUserItemProc(DialogPtr pDialog, short iitem)
{
	GrafPtr			gpSave;
	short			itype;
	Handle			h;
	Rect			rc;
	PPASTESPECIAL	pPS;
#ifdef UIDLL
   void*       oldQD = SetLpqdFromA5();
#endif

	GetPort(&gpSave);
	SetPort(pDialog);

	GetDItem(pDialog, iitem, &itype, &h, &rc);
	pPS = (PPASTESPECIAL)GetWRefCon(pDialog);
	
	switch (iitem)
	{
		case PS_UITEM_RESULT:
		{
			short		txFont = pDialog->txFont;
			short		txSize = pDialog->txSize;
			Rect		rcPict;
			Rect		rcText;
			PicHandle	hPict;
			RgnHandle	hrgnClip;

			PenSize(1, 1);
			FrameRect(&rc);

			InsetRect(&rc, 4, 12);

			if (!pPS->fItemSelected)
			{
				EraseRect(&rc);		//Review: how should this case be handled?
				break;
			}

			rcText = rc;

			if (pPS->nResultIcon != RESULTIMAGE_NONE)
			{
#ifdef UIDLL
            short hostResNum = SetUpOLEUIResFile();
#endif
				hPict = GetPicture(OLEUI_PICTID);

				rc.top += 6;
				rc.bottom -= 6;
				rc.right = rc.left + 60;

				hrgnClip = NewRgn();
				GetClip(hrgnClip);
				ClipRect(&rc);

				rcPict = (**hPict).picFrame;
				OffsetRect(&rcPict, rc.left, (short)(rc.top - pPS->nResultIcon * 40));

				//This is offset and clipped so only the appropriate
				//part of the image is visible.
				DrawPicture(hPict, &rcPict);

				SetClip(hrgnClip);
				DisposeRgn(hrgnClip);

				ReleaseResource((Handle)hPict);
#ifdef UIDLL
            ClearOLEUIResFile(hostResNum);
#endif

			}

			TextFont(geneva);
			TextSize(9);

			rcText.left += 62;
			rcText.right -= 4;
			TextBox(pPS->szResult, strlen(pPS->szResult), &rcText, teFlushLeft);

			TextFont(txFont);
			TextSize(txSize);
		
			break;
		}

		case PS_UITEM_LBOX:
			PSDrawList(pDialog, pPS, false);
			break;

		case PS_UITEM_SRCTEXT:
			TextBox(pPS->szSourceOfDataLSD, strlen(pPS->szSourceOfDataLSD), &rc, teFlushLeft);
			break;

		case PS_UITEM_ICON:
			PSDrawIcon(pDialog, pPS, false);
			break;
			
		case PS_UITEM_ICONTXT:
			PSDrawIconLabel(pDialog, pPS);
			break;
		
		case PS_UITEM_OKOUTLINE:
			DrawDefaultBorder(pDialog, PS_UITEM_OKOUTLINE, pPS->fItemSelected);
			break;
	}

	SetPort(gpSave);
#ifdef UIDLL
      RestoreLpqd(oldQD);
#endif

}




/*
 * PSUpdateOneControl
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
#pragma segment PasteSplSeg
#else
#pragma code_seg("PasteSplSeg", "SWAPPABLE")
#endif
void PSUpdateOneControl(DialogPtr pDialog, short iitem, Boolean fAvail)
{
	short		itype;
	Handle		h;
	Rect		rc;

	GetDItem(pDialog, iitem, &itype, &h, &rc);

	if ((**(ControlHandle)h).contrlHilite == (fAvail ? 0 : 255))
		return;

	HiliteControl((ControlHandle)h, (short)(fAvail ? 0 : 255));

	if (iitem == PS_BTN_OK)
		DrawDefaultBorder(pDialog, PS_UITEM_OKOUTLINE, fAvail);
}




/*
 * PSUpdateAllControls
 *
 * Purpose:
 *  Updates the availability of all the controls in our dialog box.
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment PasteSplSeg
#else
#pragma code_seg("PasteSplSeg", "SWAPPABLE")
#endif
void PSUpdateAllControls(DialogPtr pDialog, PPASTESPECIAL pPS)
{
	PSUpdateOneControl(pDialog, PS_BTN_OK, pPS->fItemSelected);
	EnableDisplayAsIcon(pDialog, pPS);
}




/*
 * PSUpdateResults
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment PasteSplSeg
#else
#pragma code_seg("PasteSplSeg", "SWAPPABLE")
#endif
void PSUpdateResults(DialogPtr pDialog, PPASTESPECIAL pPS)
{
	unsigned int		iString, iImage;
	int					nPasteEntriesIndex;
	Boolean				fDisplayAsIcon;
	Boolean				fIsObject;
	ListHandle			hList;
	short				cch;
	Cell				cell = { 0, 0 };
	PASTELISTITEMDATA	ItemData;
	LPOLEUIPASTESPECIAL	lpOPS = pPS->lpOPS;
	char				*szFullUserTypeName = (pPS->fLink ? pPS->szFullUserTypeNameLSD : pPS->szFullUserTypeNameOD);
	char				*szInsert;
	char				*psz1, *psz2;
	short				itype;
	Handle				h;
	Rect				rc;

	hList = (pPS->dwFlags & PSF_SELECTPASTE ? pPS->hListPaste : pPS->hListPasteLink);
	if ((**hList).dataBounds.bottom == 0)
		goto finish;

	if (!LGetSelect(true, &cell, hList))
		goto finish;

	cell.h++;
	cch = sizeof(PASTELISTITEMDATA);
	LGetCell((Ptr)&ItemData, &cch, cell, hList);
	nPasteEntriesIndex = ItemData.nPasteEntriesIndex;

	// Check if there is a '%s' in the lpstrFormatName, then an object is being
	//   pasted/pastelinked. Otherwise Data is being pasted-pastelinked.
	fIsObject = FHasPercentS((char *)lpOPS->arrPasteEntries[nPasteEntriesIndex].lpstrFormatName, pPS);

	// Is DisplayAsIcon checked?
	fDisplayAsIcon = (0L != (pPS->dwFlags & PSF_CHECKDISPLAYASICON));

	szInsert = szFullUserTypeName;

	if (pPS->dwFlags & PSF_SELECTPASTE)	 // If user selected Paste
	{
		if (fIsObject)
		{
			iString = fDisplayAsIcon ? IDS_PSPASTEOBJECTASICON : IDS_PSPASTEOBJECT;
			iImage  = fDisplayAsIcon ? RESULTIMAGE_EMBEDICON   : RESULTIMAGE_EMBED;
			szInsert = pPS->szAppName;
		}
		else
		{
			iString = IDS_PSPASTEDATA;
			iImage  = RESULTIMAGE_PASTE;
		}
	}
	else if (pPS->dwFlags & PSF_SELECTPASTELINK)   // User selected PasteLink
	{
		if (fIsObject)
		{
			iString = fDisplayAsIcon ? IDS_PSPASTELINKOBJECTASICON : IDS_PSPASTELINKOBJECT;
			iImage  = fDisplayAsIcon ? RESULTIMAGE_LINKICON : RESULTIMAGE_LINK;
		}
		else
		{
			iString = IDS_PSPASTELINKDATA;
			iImage  = RESULTIMAGE_LINK;
		}

	}
	else   // Should never occur.
	{
		iString = IDS_PSNONOLE;
		iImage = RESULTIMAGE_PASTE;
	}

	// hBuff contains enough space for the 4 buffers required to build up the help
	//   result text.
	cch = GetHandleSize(pPS->hBuff)/2;
	psz1 = *pPS->hBuff;
	psz2 = psz1+cch;

	GetIndString((unsigned char *)psz1, (short)DIDPasteSpecial, (short)iString);
	p2cstr((unsigned char *)psz1);

	sprintf(psz2, lpOPS->arrPasteEntries[nPasteEntriesIndex].lpstrResultText, szInsert);
	sprintf(pPS->szResult, psz1, psz2);

	pPS->nResultIcon = iImage;

finish:
	GetDItem(pDialog, PS_UITEM_RESULT, &itype, &h, &rc);
	InsetRect(&rc, 4, 12);

	SetPort(pDialog);
	InvalRect(&rc);			//Force a redraw of the result area.
}




/*
 * TogglePasteType
 *
 * Purpose:
 *  Toggles between Paste and Paste Link.
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment PasteSplSeg
#else
#pragma code_seg("PasteSplSeg", "SWAPPABLE")
#endif
void TogglePasteType(DialogPtr pDialog, PPASTESPECIAL pPS, unsigned long dwOption)
{
	Cell				cell;
	short				cch;
	PASTELISTITEMDATA	ItemData;
	short				itype;
	Handle				h;
	Rect				rc;
	ListHandle			hList;

	//Skip all of this if we're already selected.
	if (pPS->dwFlags & dwOption)
		return;

	PSSetActiveItem(pDialog, pPS, PS_UITEM_LBOX);
	
	//1. Change the Display As Icon checked state to reflect the
	//   selection for this option, stored in the fAsIcon flags.
	pPS->dwFlags = (pPS->dwFlags & ~(PSF_SELECTPASTE | PSF_SELECTPASTELINK)) | dwOption;

	if (pPS->dwFlags & PSF_SELECTPASTE)
	{
		PSShowListItem(pPS->hListPasteLink, false);
		PSShowListItem(pPS->hListPaste, true);
		CheckRadioButton(pDialog, PS_RBTN_PASTE, PS_RBTN_PASTELINK, PS_RBTN_PASTE);
		hList = pPS->hListPaste;
		pPS->fLink = false;
	}
	else
	{
		PSShowListItem(pPS->hListPaste, false);
		PSShowListItem(pPS->hListPasteLink, true);
		CheckRadioButton(pDialog, PS_RBTN_PASTE, PS_RBTN_PASTELINK, PS_RBTN_PASTELINK);
		hList = pPS->hListPasteLink;
		pPS->fLink = true;
	}

	pPS->fItemSelected = FLGetSelectedCell(hList, &cell);

	if (pPS->fItemSelected)
	{
		cell.h++;
		cch = sizeof(PASTELISTITEMDATA);
		LGetCell((Ptr)&ItemData, &cch, cell, hList);
		pPS->nSelectedIndex = ItemData.nPasteEntriesIndex;
	}

	GetDItem(pDialog, PS_UITEM_LBOX, &itype, &h, &rc);
	InsetRect(&rc, 1, 1);
	rc.right -= 15;

	SetPort(pDialog);
	EraseRect(&rc);
	InvalRect(&rc);

	EnableDisplayAsIcon(pDialog, pPS);
	PSUpdateAllControls(pDialog, pPS);
	PSUpdateResults(pDialog, pPS);
}




/*
 * PSShowListItem
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment PasteSplSeg
#else
#pragma code_seg("PasteSplSeg", "SWAPPABLE")
#endif
void PSShowListItem(ListHandle hList, Boolean fShow)
{
	Rect	rc;
	GrafPtr	gpSave;
#ifdef UIDLL
   void* oldQD = SetLpqdFromA5();
#endif

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

	//This forces the List Manager to recognize the move:
	rc = (**hList).rView;
	InsetRect(&rc, -1, -1);
	PenSize(1, 1);
	FrameRect(&rc);
	
	SetPort(gpSave);
#ifdef UIDLL
      RestoreLpqd(oldQD);
#endif

}


/*
 * EnableDisplayAsIcon
 *
 * Purpose:
 *  Enable or disable the DisplayAsIcon button depending on whether
 *  the current selection can be displayed as an icon or not. The following table describes
 *  the state of DisplayAsIcon. The calling application is termed CONTAINER, the source
 *  of data on the clipboard is termed SOURCE.
 *  Y = Yes; N = No; Blank = State does not matter;
 * =====================================================================
 * SOURCE          SOURCE             CONTAINER             DisplayAsIcon
 * specifies       specifies          specifies             Initial State
 * DVASPECT_ICON   OLEMISC_ONLYICONIC OLEUIPASTE_ENABLEICON
 *
 *                                    N                     Unchecked&Disabled
 *                 Y                  Y                     Checked&Disabled
 * Y               N                  Y                     Checked&Enabled
 * N               N                  Y                     Unchecked&Enabled
 * =====================================================================
 *
 * Parameters:
 *  pDialog         pointer to the dialog record
 *  pPS             Paste Special Dialog Structure
 *
 * Return Value:
 *  No return value
 */

void EnableDisplayAsIcon(DialogPtr pDialog, PPASTESPECIAL pPS)
{
	Cell				cell;
	short				cch;
	short				itype;
	Handle				h;
	Rect				rc;
	Boolean 			fCntrEnableIcon;
	Boolean 			fSrcOnlyIconic = (pPS->fLink) ? pPS->fSrcOnlyIconicLSD : pPS->fSrcOnlyIconicOD;
	Boolean 			fSrcAspectIcon = (pPS->fLink) ? pPS->fSrcAspectIconLSD : pPS->fSrcAspectIconOD;
	PASTELISTITEMDATA 	ItemData;
	PicHandle 			hIcon = (pPS->fLink) ? pPS->hIconLSD : pPS->hIconOD;
	ListHandle			hList = pPS->dwFlags & PSF_SELECTPASTE ? pPS->hListPaste : pPS->hListPasteLink;

	// Get data corresponding to the current selection in the listbox
	pPS->fItemSelected = FLGetSelectedCell(hList, &cell);

	if (pPS->fItemSelected)
	{
		cell.h++;
		cch = sizeof(PASTELISTITEMDATA);
		LGetCell((Ptr)&ItemData, &cch, cell, hList);
		fCntrEnableIcon = ItemData.fCntrEnableIcon;
		
	}
	else fCntrEnableIcon = FALSE;

	GetDItem(pDialog, PS_CBOX_DISPASICON, &itype, &h, &rc);

	// If there is an icon available
	if (hIcon != NULL)
	{
		if (!fCntrEnableIcon)          // Does CONTAINER specify OLEUIPASTE_ENABLEICON?
		{
			// Uncheck & Disable DisplayAsIcon
			pPS->dwFlags &= ~PSF_CHECKDISPLAYASICON;
			PSUpdateDisplayAsIcon(pDialog, pPS);
			PSUpdateOneControl(pDialog, PS_CBOX_DISPASICON, false);
		}
		else if (fSrcOnlyIconic)       // Does SOURCE specify OLEMISC_ONLYICONIC?
		{
			// Check & Disable DisplayAsIcon
			pPS->dwFlags |= PSF_CHECKDISPLAYASICON;
			PSUpdateDisplayAsIcon(pDialog, pPS);
			PSUpdateOneControl(pDialog, PS_CBOX_DISPASICON, false);
		}
		else if (fSrcAspectIcon)       // Does SOURCE specify DVASPECT_ICON?
		{
			 // Check & Enable DisplayAsIcon
			 pPS->dwFlags |= PSF_CHECKDISPLAYASICON;
			PSUpdateDisplayAsIcon(pDialog, pPS);
			PSUpdateOneControl(pDialog, PS_CBOX_DISPASICON, true);
		}
		else
		{
			 //Uncheck and Enable DisplayAsIcon
			 pPS->dwFlags &= ~PSF_CHECKDISPLAYASICON;
			PSUpdateDisplayAsIcon(pDialog, pPS);
			PSUpdateOneControl(pDialog, PS_CBOX_DISPASICON, true);

		}
	}
	else  // No icon available
	{
		// Unchecked & Disabled
		pPS->dwFlags &= ~PSF_CHECKDISPLAYASICON;
		PSUpdateDisplayAsIcon(pDialog, pPS);
		PSUpdateOneControl(pDialog, PS_CBOX_DISPASICON, true);
	}
}



/*
 * PSUpdateDisplayAsIcon
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
#pragma segment PasteSplSeg
#else
#pragma code_seg("PasteSplSeg", "SWAPPABLE")
#endif
void PSUpdateDisplayAsIcon(DialogPtr pDialog, PPASTESPECIAL pPS)
{
	short		fCheck;
	short		itype;
	Handle		h;
	Rect		rc;

	fCheck = (pPS->dwFlags & PSF_CHECKDISPLAYASICON) ? 1 : 0;

	//Update the current value in the control.
	GetDItem(pDialog, PS_CBOX_DISPASICON, &itype, &h, &rc);
	SetCtlValue((ControlHandle)h, fCheck);

	PSUpdateOneControl(pDialog, PS_CBOX_DISPASICON, pPS->fItemSelected);
	
	if (fCheck && pPS->fItemSelected)		// we don't want to show the items if no item is selected
	{
		ShowDItem(pDialog, PS_UITEM_ICON);
		ShowDItem(pDialog, PS_UITEM_ICONTXT);
	}
	else
	{
		HideDItem(pDialog, PS_UITEM_ICON);
		HideDItem(pDialog, PS_UITEM_ICONTXT);
		HideDItem(pDialog, PS_TE_LABELEDIT);
	}

	//Result help text and icon change here
	PSUpdateResults(pDialog, pPS);
}




/*
 * PasteSpecialCleanup
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
#pragma segment PasteSplSeg
#else
#pragma code_seg("PasteSplSeg", "SWAPPABLE")
#endif
void PasteSpecialCleanup(DialogPtr pDialog)
{
	PPASTESPECIAL	pPS;

	pPS = (PPASTESPECIAL)GetWRefCon(pDialog);

	if (pPS)
	{
		if (pPS->hListPaste)
			LDispose(pPS->hListPaste);
		if (pPS->hListPasteLink)
			LDispose(pPS->hListPasteLink);
		if (pPS->hObjDesc)
			DisposeHandle(pPS->hObjDesc);
		if (pPS->hLinkSrcDesc)
			DisposeHandle(pPS->hLinkSrcDesc);
		if (pPS->hBuff)
			DisposeHandle(pPS->hBuff);
		if (pPS->hIconOD)
			OleUIPictIconFree(pPS->hIconOD);
		if (pPS->hIconLSD)
			OleUIPictIconFree(pPS->hIconLSD);
		DisposePtr((Ptr)pPS);
	}

	DisposDialog(pDialog);
}


/*
 * PSDrawIconLabel
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment PasteSplSeg
#else
#pragma code_seg("PasteSplSeg", "SWAPPABLE")
#endif
void PSDrawIconLabel(DialogPtr pDialog, PPASTESPECIAL pPS)
{
	short		itype;
	Handle		h;
	Rect		rc;
	short		txFont;
	short		txSize;
	GrafPtr		gpSave;
#ifdef UIDLL
   void*    oldQD = SetLpqdFromA5();
#endif
	
	GetPort(&gpSave);
	SetPort(pDialog);

	GetDItem(pDialog, PS_UITEM_ICONTXT, &itype, &h, &rc);

	txFont = pDialog->txFont;
	txSize = pDialog->txSize;

	TextFont(geneva);
	TextSize(9);

	if (pPS->dwFlags & PSF_SELECTPASTE)
		TextBox(pPS->szIconLabelOD, strlen(pPS->szIconLabelOD), &rc, teCenter);
	else if (pPS->dwFlags & PSF_SELECTPASTELINK)
		TextBox(pPS->szIconLabelLSD, strlen(pPS->szIconLabelLSD), &rc, teCenter);

	TextFont(txFont);
	TextSize(txSize);
	
	SetPort(gpSave);
#ifdef UIDLL
      RestoreLpqd(oldQD);
#endif

}


/*
 * PSDrawIcon
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */
#ifndef _MSC_VER
#pragma segment PasteSplSeg
#else
#pragma code_seg("PasteSplSeg", "SWAPPABLE")
#endif
void PSDrawIcon(DialogPtr pDialog, PPASTESPECIAL pPS, Boolean FocusRectOnly)
{
	Rect 		frame;
	short		itype;
	Handle		h;
	Rect		rc;
	PenState	savePenState;
	GrafPtr		gpSave;
#ifdef UIDLL
   void*    oldQD = SetLpqdFromA5();
#endif
	
	GetPort(&gpSave);
	SetPort(pDialog);

	GetDItem(pDialog, PS_UITEM_ICON, &itype, &h, &rc);
	GetPenState(&savePenState);

	if (!FocusRectOnly) {
		if ((pPS->dwFlags & PSF_SELECTPASTE) && (pPS->hIconOD != NULL)) {
			DrawPicture(pPS->hIconOD, &rc);
		}
		else if ((pPS->dwFlags & PSF_SELECTPASTELINK) && (pPS->hIconLSD != NULL)) {
			DrawPicture(pPS->hIconLSD, &rc);
		}
	}
	if(pPS->iActiveItem != PS_UITEM_ICON)
		PenPat((ConstPatternParam)&((GrafPtr)pDialog)->bkPat);
		
	SetRect(&frame, (short)(rc.left - OLEUI_ICONFRAME),
					(short)(rc.top - OLEUI_ICONFRAME),
					(short)(rc.right + OLEUI_ICONFRAME),
					(short)(rc.bottom + OLEUI_ICONFRAME));
	PenSize(1, 1);
	FrameRect(&frame);
	
	SetPenState(&savePenState);

	SetPort(gpSave);
#ifdef UIDLL
      RestoreLpqd(oldQD);
#endif

}

/*
 * PSDrawList
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */
#ifndef _MSC_VER
#pragma segment PasteSplSeg
#else
#pragma code_seg("PasteSplSeg", "SWAPPABLE")
#endif
void PSDrawList(DialogPtr pDialog, PPASTESPECIAL pPS, Boolean FocusRectOnly)
{
	short		itype;
	Handle		h;
	Rect		rc;
	PenState	savePenState;
	GrafPtr		gpSave;
	short		txFont;
	short		txSize;
#ifdef UIDLL
   void*    oldQD = SetLpqdFromA5();
#endif
	
	GetPort(&gpSave);
	SetPort(pDialog);

	txFont = pDialog->txFont;
	txSize = pDialog->txSize;

	TextFont(systemFont);
	TextSize(12);

	GetDItem(pDialog, PS_UITEM_LBOX, &itype, &h, &rc);
	GetPenState(&savePenState);

	if (!FocusRectOnly) {	
		if (pPS->dwFlags & PSF_SELECTPASTE)
			LUpdate(pDialog->visRgn, pPS->hListPaste);
		else
			LUpdate(pDialog->visRgn, pPS->hListPasteLink);
		PenSize(1, 1);
		FrameRect(&rc);
	}
	SetRect(&rc, (short)(rc.left - OLEUI_LBOXFRAME),
				 (short)(rc.top - OLEUI_LBOXFRAME),
				 (short)(rc.right + OLEUI_LBOXFRAME),
				 (short)(rc.bottom + OLEUI_LBOXFRAME));
	PenSize(2, 2);
	if(pPS->iActiveItem != PS_UITEM_LBOX) {
		PenPat((ConstPatternParam)&((GrafPtr)pDialog)->bkPat);
	}
	FrameRect(&rc);
	SetPenState(&savePenState);

	TextFont(txFont);
	TextSize(txSize);

	SetPort(gpSave);
#ifdef UIDLL
      RestoreLpqd(oldQD);
#endif

}

/*
 * PSTabFocus
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */
#ifndef _MSC_VER
#pragma segment PasteSplSeg
#else
#pragma code_seg("PasteSplSeg", "SWAPPABLE")
#endif
void PSTabFocus(DialogPtr pDialog, PPASTESPECIAL pPS)
{
	switch (pPS->iActiveItem) {
		case PS_UITEM_LBOX:
		
#if ICONFOCUS		
			PSSetActiveItem(pDialog, pPS, PS_UITEM_ICON);
			break;
			
		case PS_UITEM_ICON:
#endif

			PSSetActiveItem(pDialog, pPS, PS_UITEM_ICONTXT);
			break;
			
		case PS_UITEM_ICONTXT:
			PSSetActiveItem(pDialog, pPS, PS_UITEM_LBOX);
			break;
			
	}
}

/*
 * PSSetActiveItem
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */
#ifndef _MSC_VER
#pragma segment PasteSplSeg
#else
#pragma code_seg("PasteSplSeg", "SWAPPABLE")
#endif
void PSSetActiveItem(DialogPtr pDialog, PPASTESPECIAL pPS, short item)
{
	short	itemLoseFocus  = 0;
	short	itype;
	Handle	h;
	Rect	rc;
	
	if (pPS->iActiveItem == item)		// focus at the item already
		return;
		
	switch (item) {
		case PS_UITEM_LBOX:
			GetDItem(pDialog, PS_UITEM_LBOX, &itype, &h, &rc);
			if (rc.left > 8192)		// item is invisiable
				return;
			itemLoseFocus = pPS->iActiveItem;
			pPS->iActiveItem = item;
			PSDrawList(pDialog, pPS, true);
			break;
			
#if ICONFOCUS			
		case PS_UITEM_ICON:
			GetDItem(pDialog, PS_UITEM_ICON, &itype, &h, &rc);
			if (rc.left > 8192)		// item is invisiable
				return;
			itemLoseFocus = pPS->iActiveItem;
			pPS->iActiveItem = item;
			PSDrawIcon(pDialog, pPS, true);
			break;
#endif
			
		case PS_UITEM_ICONTXT:
			GetDItem(pDialog, PS_UITEM_ICONTXT, &itype, &h, &rc);
			if (rc.left > 8192)		// item is invisiable
				return;
			itemLoseFocus = pPS->iActiveItem;
			pPS->iActiveItem = item;
			PSToggleIconLabel(pDialog, pPS, PS_TE_LABELEDIT);
			break;
	}
	
	switch (itemLoseFocus) {
		case PS_UITEM_LBOX:
			PSDrawList(pDialog, pPS, true);
			break;
			
#if ICONFOCUS			
		case PS_UITEM_ICON:
			PSDrawIcon(pDialog, pPS, true);
			break;
#endif
			
		case PS_UITEM_ICONTXT:
			PSToggleIconLabel(pDialog, pPS, PS_UITEM_ICONTXT);
			PSDrawIconLabel(pDialog, pPS);
			break;
	}
	
}


/*
 * PSToggleIconLabel
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment PasteSplSeg
#else
#pragma code_seg("PasteSplSeg", "SWAPPABLE")
#endif
void PSToggleIconLabel(DialogPtr pDialog, PPASTESPECIAL pPS, short item)
// This procedure assumes that the display as icon checkbox is checked
{
	TEHandle 	hEdit = ((DialogPeek)pDialog)->textH;
	char		*pszLabel = (pPS->dwFlags & PSF_SELECTPASTE) ? pPS->szIconLabelOD : pPS->szIconLabelLSD;
	CharsHandle	hChars;
	unsigned long		cchLabel;
	
	// ASSERTCOND(pPS->dwFlags & PSF_CHECKDISPLAYASICON);
	ASSERTCOND((item == PS_TE_LABELEDIT) || (item == PS_UITEM_ICONTXT));
	
	if(item == PS_TE_LABELEDIT) {
		HideDItem(pDialog, PS_UITEM_ICONTXT);
		ShowDItem(pDialog, PS_TE_LABELEDIT);
		(*hEdit)->txFont = geneva;
		(*hEdit)->txSize = 9;		
		TESetText(pszLabel, strlen(pszLabel), hEdit);
		TEActivate(hEdit);
		TESetSelect(0, 32767, hEdit);			
	}
	else {
		ShowDItem(pDialog, PS_UITEM_ICONTXT);
		HideDItem(pDialog, PS_TE_LABELEDIT);
		TEDeactivate(hEdit);
		hChars = TEGetText(hEdit);
		HLock((Handle)hChars);
		if (pPS->dwFlags & PSF_SELECTPASTE) {
			cchLabel = MIN((*hEdit)->teLength, sizeof(pPS->szIconLabelOD) - 1);
			strncpy(pPS->szIconLabelOD, (char *)*hChars, cchLabel);
			pPS->szIconLabelOD[cchLabel] = '\0';
		}
		else {
			cchLabel = MIN((*hEdit)->teLength, sizeof(pPS->szIconLabelLSD) - 1);
			strncpy(pPS->szIconLabelLSD, (char *)*hChars, cchLabel);
			pPS->szIconLabelLSD[cchLabel] = '\0';
		}
		HUnlock((Handle)hChars);
	}
}
