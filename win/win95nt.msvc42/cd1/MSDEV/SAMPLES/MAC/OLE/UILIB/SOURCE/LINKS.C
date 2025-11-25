/*
 * LINKS.C
 *
 * Implements the OleUIEditLinks function which invokes the complete
 * Edit Links dialog.
 *
 * Copyright (c)1992-1994 Microsoft Corporation, All Right Reserved
 */




#if defined(USEHEADER)
#include "OleHeaders.h"
#endif

#if defined(__MWERKS__) && !defined(__powerc)
#pragma pointers_in_D0
#endif


#include <Fonts.h>
#include <stdio.h>
#include <Packages.h>
#ifndef _MSC_VER
#include <Resources.h>
#include <StandardFile.h>
#include <ToolUtils.h>
#else
#include <Resource.h>
#include <Standard.h>
#include <ToolUtil.h>
#endif

#include <ole2.h>

#include "ole2ui.h"
#include "common.h"
#include "uidebug.h"
#include "links.h"
#include "utility.h"


#ifdef __powerc

RoutineDescriptor gRDEditLinksDialogProc =
   BUILD_ROUTINE_DESCRIPTOR(uppModalFilterProcInfo,EditLinksDialogProc);
ModalFilterUPP gEditLinksDialogProc = &gRDEditLinksDialogProc;

RoutineDescriptor gRDChangeSourceDialogProc =
   BUILD_ROUTINE_DESCRIPTOR(uppDlgHookYDProcInfo,ChangeSourceDialogProc);
DlgHookYDUPP gChangeSourceDialogProc = &gRDChangeSourceDialogProc;

RoutineDescriptor gRDChangeSourceModalProc =
   BUILD_ROUTINE_DESCRIPTOR(uppModalFilterYDProcInfo,ChangeSourceModalProc);
ModalFilterYDUPP gChangeSourceModalProc =  &gRDChangeSourceModalProc;

RoutineDescriptor gRDChangeSourceActivateProc =
   BUILD_ROUTINE_DESCRIPTOR(uppActivateYDProcInfo,ChangeSourceActivateProc);
static ActivateYDUPP gChangeSourceActivateProc = &gRDChangeSourceActivateProc;

RoutineDescriptor gRDELUserItemProc =
   BUILD_ROUTINE_DESCRIPTOR(uppUserItemProcInfo,ELUserItemProc);
static UserItemUPP gELUserItemProc = &gRDELUserItemProc;

RoutineDescriptor gRDCSUserItemProc =
   BUILD_ROUTINE_DESCRIPTOR(uppUserItemProcInfo,CSUserItemProc);
static UserItemUPP gCSUserItemProc = &gRDCSUserItemProc;

#ifdef _MSC_VER
void __pascal EditLinksLDEF(short lMessage, Boolean lSelect, Rect *lRect, Cell lCell,
	 short lDataOffset, short lDataLen, ListHandle lHandle);
#else
pascal void  EditLinksLDEF(short lMessage, Boolean lSelect, Rect *lRect, Cell lCell,
	 short lDataOffset, short lDataLen, ListHandle lHandle);
#endif

RoutineDescriptor gRDEditLinksLDEF =
   BUILD_ROUTINE_DESCRIPTOR(uppListDefProcInfo,EditLinksLDEF);

#endif


#if defined(_DEBUG) && !defined(_OLE_ASSERTS)
extern void __UIASSERTCONDSZ(char *, char *, char *, int);
#endif


OLEDBGDATA


static DialogPtr	gLinksDialog;


/*
 * OleUIEditLinks
 *
 * Purpose:
 *  Invokes the standard OLE Edit Links dialog box allowing the user
 *  to manipulate ole links (delete, update, change source, etc).
 *
 * Parameters:
 *  lpOEL           LPOLEUIEditLinks pointing to the in-out structure
 *                  for this dialog.
 *
 * Return Value:
 *  unsigned int    One of the following codes, indicating success or error:
 *                      OLEUI_SUCCESS           Success
 *                      OLEUI_ERR_STRUCTSIZE    The dwStructSize value is wrong
 */

#ifndef _MSC_VER
#pragma segment LinksSeg
#else
#pragma code_seg("LinksSeg", "SWAPPABLE")
#endif
STDAPI_(unsigned int) OleUIEditLinks(LPOLEUIEDITLINKS lpOEL)
{
	short 			nItem;
	DialogPtr		pDialog;
	PEDITLINKS		pEL;
    HRESULT         hErr;
	unsigned int	uRet;
	GrafPtr			gpSave;
	
	GetPort(&gpSave);
	
	InitCursor();

	//Validate pEL structure
	uRet = UStandardValidation((LPOLEUISTANDARD)lpOEL, sizeof(OLEUIEDITLINKS));

	if (uRet != OLEUI_SUCCESS) {
		SetPort(gpSave);
		return uRet;
	}

	pDialog = NULL;
	uRet = UStandardInvocation((LPOLEUISTANDARD)lpOEL, &pDialog, DIDEditLinks);

	if (uRet != OLEUI_SUCCESS)
	{
		SetPort(gpSave);
		return uRet;
	}

	uRet = UEditLinksInit(pDialog, lpOEL);

	if (uRet != OLEUI_SUCCESS)
	{
		EditLinksCleanup(pDialog);
		SetPort(gpSave);
		return uRet;
	}

	gLinksDialog = pDialog;

	ShowWindow((WindowPtr)pDialog);
	pEL = (PEDITLINKS)GetWRefCon(pDialog);

	do
	{
#ifndef __powerc
		ModalDialog(EditLinksDialogProc, &nItem);
#else

#ifdef UIDLL
      short hostResNum = SetUpOLEUIResFile();
#endif

		ModalDialog(gEditLinksDialogProc, &nItem);

#ifdef UIDLL
      ClearOLEUIResFile(hostResNum);
#endif

#endif	

		switch (nItem)
		{
			case EL_BTN_UPDATENOW:
				Container_UpdateNow(pDialog, pEL);
				break;

			case EL_BTN_OPENSOURCE:
				hErr = Container_OpenSource(pDialog, pEL);
            if (hErr == NOERROR)
            {
					//Close dialog if open was successful
					nItem = EL_BTN_CLOSE;
					break;
				}
            else
				{
					OleUIPromptUser(IDD_LINKSOURCEUNAVAILABLE);
					nItem = -1;		// So we don't close the dialog
				}
				ELUpdateAllControls(pDialog, pEL);
				break;

			case EL_BTN_CHANGESOURCE:
				Container_ChangeSource(pDialog, pEL);
				ELUpdateAllControls(pDialog, pEL);
				break;

			case EL_BTN_BREAKLINK:
				Container_BreakLink(pDialog, pEL);
				ELUpdateAllControls(pDialog, pEL);
				break;

#ifdef _DEBUG
			case EL_BTN_HELP:
				ASSERTCOND(false);	//If you have this enabled, you must
									//intercept this in your hook
				break;
#endif //_DEBUG

			case EL_RBTN_AUTOMATIC:
				CheckRadioButton(pDialog, EL_RBTN_AUTOMATIC, EL_RBTN_MANUAL, EL_RBTN_AUTOMATIC);
				hErr = Container_AutomaticManual(pDialog, true, pEL);
				if (hErr != NOERROR)
#ifdef LATER
					PopupMessage(hDlg, IDS_LINKS, IDS_FAILED, MB_ICONEXCLAMATION | MB_OK);
#else
					SysBeep(0);
#endif
				ELUpdateAllControls(pDialog, pEL);
				break;

			case EL_RBTN_MANUAL:
				CheckRadioButton(pDialog, EL_RBTN_AUTOMATIC, EL_RBTN_MANUAL, EL_RBTN_MANUAL);
				hErr = Container_AutomaticManual(pDialog, false, pEL);
				if (hErr != NOERROR)
#ifdef LATER
					PopupMessage(hDlg, IDS_LINKS, IDS_FAILED, MB_ICONEXCLAMATION | MB_OK);
#else
					SysBeep(0);
#endif
				ELUpdateAllControls(pDialog, pEL);
				break;
		}
	} while (nItem != EL_BTN_CLOSE && nItem != EL_BTN_OPENSOURCE);

	EditLinksCleanup(pDialog);

	uRet = (nItem == EL_BTN_CLOSE ? OLEUI_OK : OLEUI_CANCEL);

	gLinksDialog = NULL;

	SetPort(gpSave);
	return uRet;
}




/*
 * UEditLinksInit
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment LinksSeg
#else
#pragma code_seg("LinksSeg", "SWAPPABLE")
#endif
unsigned int UEditLinksInit(DialogPtr pDialog, LPOLEUIEDITLINKS lpOEL)
{
	PEDITLINKS		pEL;
    unsigned long	cLinks;
	unsigned int	uRet;

	//1. Copy the structure at lpOEL
	pEL = (PEDITLINKS)PvStandardInit(pDialog, sizeof(EDITLINKS));

	//PvStandardInit failed
	if (pEL == NULL)
		return OLEUI_ERR_GLOBALMEMALLOC;

	//2. Initialize all the user items in our dialog box.
	uRet = ELUInitUserItems(pDialog);

	if (uRet != OLEUI_SUCCESS)
		return uRet;

	//3. Save the original pointer.
	pEL->lpOEL = lpOEL;

	// Hide the help button if required
	if (!(lpOEL->dwFlags & ELF_SHOWHELP))
		HideDItem(pDialog, EL_BTN_HELP);

	if (lpOEL->lpszCaption != NULL)
		SetWTitle(pDialog, (StringPtr)lpOEL->lpszCaption);

    cLinks = LoadLinkLB(pEL->hListLinks, pEL->lpOEL->lpOleUILinkContainer, pEL);
    if (cLinks < 0)
        return OLEUI_ERR_GLOBALMEMALLOC;

	ELUpdateAllControls(pDialog, pEL);

	return OLEUI_SUCCESS;
}




/*
 * ELUInitUserItems
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment LinksSeg
#else
#pragma code_seg("LinksSeg", "SWAPPABLE")
#endif
unsigned int ELUInitUserItems(DialogPtr pDialog)
{
	PEDITLINKS		pEL;
	short			iitem;
	short			itype;
	Handle			h;
	Rect			rc,
					rcDataBounds;
	short			nIndent;
	short			txFont;
	Point			ptSize;

#ifdef UIDLL
   void*       oldQD = SetLpqdFromA5();
#endif

	//This assumes that the user items are grouped together and that
	//EL_UITEM_FIRST and EL_UITEM_LAST are properly defined
	for (iitem = EL_UITEM_FIRST; iitem <= EL_UITEM_LAST; iitem++)
	{
		GetDItem(pDialog, iitem, &itype, &h, &rc);
		if ((itype & ~itemDisable) == userItem)
#ifndef __powerc
			SetDItem(pDialog, iitem, itype, (Handle)ELUserItemProc, &rc);
#else
			SetDItem(pDialog, iitem, itype, (Handle)gELUserItemProc, &rc);
#endif
#ifdef _DEBUG
		else
			ASSERTCOND(false);
#endif //_DEBUG
	}

	pEL = (PEDITLINKS)GetWRefCon(pDialog);

	GetDItem(pDialog, EL_UITEM_LBOX, &itype, &h, &rc);
	nIndent = rc.left;

	for (iitem = EL_TEXT_LINKS; iitem <= EL_TEXT_UPDATE; iitem++)
	{
		GetDItem(pDialog, iitem, &itype, &h, &rc);
		pEL->nColPos[iitem - EL_TEXT_LINKS] = rc.left - nIndent - 2;
	}

	//Temporarily switch to geneva when creating listbox
	SetPort(pDialog);
	txFont = pDialog->txFont;
	TextFont(geneva);

	//Create listbox
	GetDItem(pDialog, EL_UITEM_LBOX, &itype, &h, &rc);
	InsetRect(&rc, 1, 1);
	rc.right -= 15;							//Add space for vertical scroll bar
	SetRect(&rcDataBounds, 0, 0, 1, 0);		//One column listbox
	SetPt(&ptSize, (short)(rc.right-rc.left), 0);	//First column fills width of list (second isn't visible)

#ifdef __powerc
	{
		ListHandle aLHandle = NULL;
		RoutineDescriptor** uppHandle = NULL;
		pEL->hListLinks = LNew(&rc, &rcDataBounds, ptSize, 0, (WindowPtr)pDialog, true, false, false, true);
	    aLHandle = pEL->hListLinks;
	    uppHandle = (RoutineDescriptor**)NewHandle(sizeof(RoutineDescriptor));
	   if (NULL != uppHandle)
	   {
	      HLock((Handle)uppHandle);
	      **uppHandle = gRDEditLinksLDEF;
	      HUnlock((Handle)uppHandle);
	      (*aLHandle)->listDefProc = (Handle)uppHandle;
	   }
	}
#else
	pEL->hListLinks = LNew(&rc, &rcDataBounds, ptSize, IDListDef, (WindowPtr)pDialog, true, false, false, true);
#endif

	if (pEL->hListLinks == NULL)
   {
#ifdef UIDLL
      RestoreLpqd(oldQD);
#endif
      return OLEUI_ERR_GLOBALMEMALLOC;
   }
	

	(**pEL->hListLinks).selFlags = lNoNilHilite;

	//Switch back to the default font
	TextFont(txFont);

#ifdef UIDLL
      RestoreLpqd(oldQD);
#endif

	pEL->hMenuSource = NewMenu(kLinkPathMenu, "\p");

	if (pEL->hMenuSource == NULL)
		return OLEUI_ERR_GLOBALMEMALLOC;

	InsertMenu(pEL->hMenuSource, -1);

	return OLEUI_SUCCESS;
}




/*
 * LoadLinkLB
 *
 * Purpose:
 *  Enumerate all links from the Link Container and build up the Link
 *  ListBox
 *
 * Parameters:
 *  hListBox        window handle of
 *  lpOleUILinkCntr pointer to OleUI Link Container
 *
 * Returns:
 *  number of link items loaded, -1 if error
 */

#ifndef _MSC_VER
#pragma segment LinksSeg
#else
#pragma code_seg("LinksSeg", "SWAPPABLE")
#endif
int LoadLinkLB(ListHandle hListBox, LPOLEUILINKCONTAINER lpOleUILinkCntr, PEDITLINKS pEL)
{
	unsigned long	dwLink = 0L;
	LPLINKINFO		lpLI;
	int				nIndex;
	int				cLinks;
	Cell			cell;

	cLinks = 0;

	while ((dwLink = lpOleUILinkCntr->lpVtbl->GetNextLink(lpOleUILinkCntr, dwLink)) != 0L) {

		lpLI = (LPLINKINFO)OleStdMalloc(sizeof(LINKINFO));
		if (NULL == lpLI)
			return -1;

		lpLI->fIsMarked   = false;
		lpLI->fIsSelected = false;
		lpLI->fDontFree   = false;
		lpLI->lpszAMX     = (char *)OleStdMalloc(LINKTYPELEN+1);
		BlockMove(pEL->nColPos, lpLI->nColPos, sizeof(lpLI->nColPos));

		lpLI->dwLink = dwLink;
		cLinks++;
		if ((nIndex = AddLinkLBItem(hListBox, lpOleUILinkCntr, lpLI, true)) < 0)
			// can't load list box
			return -1;

		if (lpLI->fIsSelected)
		{
			SetPt(&cell, 0, (short)nIndex);
			LSetSelect(true, cell, hListBox);
		}
	}

	return cLinks;
}




/*
 * AddLinkLBItem
 *
 * Purpose:
 *  Add the item pointed to by lpLI to the Link ListBox and return
 *  the index of it in the ListBox
 *
 * Parameters:
 *
 *  Returns:
 */

#ifndef _MSC_VER
#pragma segment LinksSeg
#else
#pragma code_seg("LinksSeg", "SWAPPABLE")
#endif
int AddLinkLBItem(ListHandle hListBox, LPOLEUILINKCONTAINER lpOleUILinkCntr, LPLINKINFO lpLI, Boolean fGetSelected)
{
	HRESULT			hErr;
	unsigned long	dwUpdateOpt;
	LPLINKINFO		lpLITemp;
	Cell			cell;
	short			cch;

	OleDbgAssert(lpOleUILinkCntr && hListBox && lpLI);

	lpLI->fDontFree = false;

	OLEDBG_BEGIN2("IOleUILinkContainer::GetLinkSource called\r\n");
	hErr = lpOleUILinkCntr->lpVtbl->GetLinkSource(
			lpOleUILinkCntr,
			lpLI->dwLink,
			(char * *)&lpLI->lpszDisplayName,
			(unsigned long *)&lpLI->clenFileName,
			(char * *)&lpLI->lpszFullLinkType,
			(char * *)&lpLI->lpszShortLinkType,
			(Boolean *)&lpLI->fSourceAvailable,
			fGetSelected ? (Boolean *)&lpLI->fIsSelected : NULL);
	OLEDBG_END2

	if (hErr != NOERROR)
	{
		OleDbgOutHResult("WARNING: IOleUILinkContainer::GetLinkSource returned",hErr);
#ifdef LATER
		PopupMessage(hListBox, IDS_LINKS, IDS_ERR_GETLINKSOURCE, MB_ICONEXCLAMATION | MB_OK);
#else
		SysBeep(0);
#endif
		goto cleanup;
	}

	OLEDBG_BEGIN2("IOleUILinkContainer::GetLinkUpdateOptions called\r\n");
	hErr=lpOleUILinkCntr->lpVtbl->GetLinkUpdateOptions(
			lpOleUILinkCntr,
			lpLI->dwLink,
			(unsigned long *)&dwUpdateOpt);
	OLEDBG_END2

	if (hErr != NOERROR)
	{
		OleDbgOutHResult("WARNING: IOleUILinkContainer::GetLinkUpdateOptions returned",hErr);
#ifdef LATER
		PopupMessage(hListBox, IDS_LINKS, IDS_ERR_GETLINKUPDATEOPTIONS, MB_ICONEXCLAMATION | MB_OK);
#else
		SysBeep(0);
#endif
		goto cleanup;
	}

	if (lpLI->fSourceAvailable)
	{
		if (dwUpdateOpt == OLEUPDATE_ALWAYS)
		{
			lpLI->fIsAuto = true;
			GetIndString((unsigned char *)lpLI->lpszAMX, DIDEditLinks, IDS_LINK_AUTO);
			p2cstr((unsigned char *)lpLI->lpszAMX);
		}
		else
		{
			lpLI->fIsAuto = false;
			GetIndString((unsigned char *)lpLI->lpszAMX, DIDEditLinks, IDS_LINK_MANUAL);
			p2cstr((unsigned char *)lpLI->lpszAMX);
		}
	}
	else
	{
		GetIndString((unsigned char *)lpLI->lpszAMX, DIDEditLinks, IDS_LINK_UNKNOWN);
		p2cstr((unsigned char *)lpLI->lpszAMX);
	}

	BreakString(lpLI);

	//Determine the correct location for this entry
	//based on sorting order
	SetPt(&cell, 0, 0);
	while (cell.v < (**hListBox).dataBounds.bottom)
	{
		cch = sizeof(LPLINKINFO);
		LGetCell(&lpLITemp, &cch, cell, hListBox);
		if (IUMagString(lpLI->lpszDisplayName,
						lpLITemp->lpszDisplayName,
						(short)strlen(lpLI->lpszDisplayName),
						(short)strlen(lpLITemp->lpszDisplayName)) < 0)
			break;
		cell.v++;
	}

	//Add the LinkInfo at this location
	LAddRow(1, cell.v, hListBox);
  	LSetCell(&lpLI, sizeof(lpLI), cell, hListBox);

	return cell.v;

cleanup:
	if (lpLI->lpszDisplayName)
		OleStdFree((void *)lpLI->lpszDisplayName);

	if (lpLI->lpszShortLinkType)
		OleStdFree((void *)lpLI->lpszShortLinkType);

	if (lpLI->lpszFullLinkType)
		OleStdFree((void *)lpLI->lpszFullLinkType);

	return -1;
}




/*
 * RefreshLinkLB
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment LinksSeg
#else
#pragma code_seg("LinksSeg", "SWAPPABLE")
#endif
void RefreshLinkLB(ListHandle hListBox, LPOLEUILINKCONTAINER lpOleUILinkCntr)
{
	int			cItems;
	int			nIndex;
	short		cch;
	Cell		cell;
	LPLINKINFO	lpLI;
	Rect		rCell;
	Boolean		bStop;

	OleDbgAssert(hListBox);

	cItems = (**hListBox).dataBounds.bottom;
	OleDbgAssert(cItems >= 0);

	LDoDraw(false, hListBox);

	do
	{
		bStop = true;
		for (nIndex = 0; nIndex < cItems; nIndex++)
		{
			cch = sizeof(LPLINKINFO);
			SetPt(&cell, 0, (short)nIndex);
			LGetCell(&lpLI, &cch, cell, hListBox);

			if (lpLI->fIsMarked)
			{
				lpLI->fIsMarked = false;
				lpLI->fDontFree = true;

				SetPt(&cell, 0, (short)nIndex);
				LRect(&rCell, cell, hListBox);
				InvalRect(&rCell);

				LDelRow(1, (short)nIndex, hListBox);

				nIndex = AddLinkLBItem(hListBox, lpOleUILinkCntr, lpLI, false);
				if (lpLI->fIsSelected)
				{
					SetPt(&cell, 0, (short)nIndex);
					LSetSelect(true, cell, hListBox);
				}

				SetPt(&cell, 0, (short)nIndex);
				LRect(&rCell, cell, hListBox);
				InvalRect(&rCell);

				bStop = false;
				break;
			}
		}
	} while (!bStop);

	LDoDraw(true, hListBox);
}




/*
 * UpdateLinkLBItem
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment LinksSeg
#else
#pragma code_seg("LinksSeg", "SWAPPABLE")
#endif
void UpdateLinkLBItem(ListHandle hListBox, int nIndex, LPEDITLINKS pEL, Boolean bSelect)
{
	short					cch;
	Cell					cell;
	LPLINKINFO				lpLI;
	LPOLEUILINKCONTAINER	lpOleUILinkCntr;

	if (!hListBox || (nIndex < 0) || !pEL)
		return;

	lpOleUILinkCntr = pEL->lpOEL->lpOleUILinkContainer;

	cch = sizeof(LPLINKINFO);
	SetPt(&cell, 0, (short)nIndex);
	LGetCell(&lpLI, &cch, cell, hListBox);

	lpLI->fDontFree = true;
	LDelRow(1, (short)nIndex, hListBox);

	nIndex = AddLinkLBItem(hListBox, lpOleUILinkCntr, lpLI, false);
	if (bSelect)
	{
		SetPt(&cell, 0, (short)nIndex);
		LSetSelect(true, cell, hListBox);
	}
}




/*
 * ChangeAllLinks
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment LinksSeg
#else
#pragma code_seg("LinksSeg", "SWAPPABLE")
#endif
void ChangeAllLinks(ListHandle hListBox, LPOLEUILINKCONTAINER lpOleUILinkCntr, char * lpszFrom, char * lpszTo)
{
	int			cItems;
	int			nIndex;
	int 		cFrom;
	short		cch;
	Cell		cell;
	LPLINKINFO	lpLI;
	char		szTmp[OLEUI_CCHPATHMAX];
	Boolean		bFound;

	cFrom = strlen(lpszFrom);

	cItems = (**hListBox).dataBounds.bottom;
	OleDbgAssert(cItems >= 0);

	bFound = false;

	for (nIndex = 0; nIndex < cItems; nIndex++) {
		
		cch = sizeof(LPLINKINFO);
		SetPt(&cell, 0, (short)nIndex);	//Get data in fourth column
		LGetCell(&lpLI, &cch, cell, hListBox);

		// unmark the item
		lpLI->fIsMarked = false;

		/* if the corresponding position for the end of lpszFrom in the
		**	display name is not a separator. We stop comparing this
		**	link.
		*/
		if (!*(lpLI->lpszDisplayName + cFrom) ||
			(*(lpLI->lpszDisplayName + cFrom) == ':') ||
			(*(lpLI->lpszDisplayName + cFrom) == '!')) {

			strncpy(szTmp, lpLI->lpszDisplayName, cFrom + 1);
			if (!strcmp(szTmp, lpszFrom)) {
				HRESULT			hErr;
				int				nFileLength;
				unsigned long	ulDummy;

				if (!bFound) {
					char	szTitle[256];
					char	szMsg[256];
#ifdef LATER
					char	szBuf[256];
#endif

					GetIndString((unsigned char *)szTitle, DIDEditLinks, IDS_CHANGESOURCE);
					p2cstr((unsigned char *)szTitle);

					GetIndString((unsigned char *)szMsg, DIDEditLinks, IDS_CHANGEADDITIONALLINKS);
					p2cstr((unsigned char *)szMsg);

#ifdef LATER
					sprintf(szBuf, szMsg, lpszFrom);
					uRet = MessageBox(hListBox, szBuf, szTitle,
							MB_ICONQUESTION | MB_YESNO);
					if (uRet == IDYES)
						bFound = true;
					else
						return;		 // exit function
#endif
				}

				strcpy(szTmp, lpszTo);
				strcat(szTmp, lpLI->lpszDisplayName + cFrom);
				nFileLength = strlen(szTmp) - (lpLI->lpszItemName ? strlen(lpLI->lpszItemName) : 0);

				hErr = lpOleUILinkCntr->lpVtbl->SetLinkSource(
						lpOleUILinkCntr,
						lpLI->dwLink,
						szTmp,
						(unsigned long)nFileLength,
						(unsigned long *)&ulDummy,
						true);
				if (hErr != NOERROR)
					lpOleUILinkCntr->lpVtbl->SetLinkSource(
							lpOleUILinkCntr,
							lpLI->dwLink,
							szTmp,
							(unsigned long)nFileLength,
							(unsigned long *)&ulDummy,
							false);
				lpLI->fIsMarked = true;
			}
		}
	}

	/* have to do the refreshing after processing all links, otherwise
	**	the item positions will change during the process as the
	**	listbox stores items in order
	*/
	if (bFound)
		RefreshLinkLB(hListBox, lpOleUILinkCntr);
}




/*
 * BreakString
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment LinksSeg
#else
#pragma code_seg("LinksSeg", "SWAPPABLE")
#endif
void BreakString(LPLINKINFO lpLI)
{
	char	*lpsz;
	short	txFont;
#ifdef UIDLL
   void* oldQD =  SetLpqdFromA5();
#endif

	if (!lpLI->clenFileName || (strlen(lpLI->lpszDisplayName)==(int)lpLI->clenFileName))
		lpLI->lpszItemName = NULL;
	else
		lpLI->lpszItemName = lpLI->lpszDisplayName + lpLI->clenFileName;

	// search from last character of filename
	lpsz = lpLI->lpszDisplayName + lpLI->clenFileName;
	while (lpsz > lpLI->lpszDisplayName)
	{
		lpsz--;
		if ((*lpsz == '\\') || (*lpsz == '/') || (*lpsz == ':'))
			break;
	}

	//REVIEW: this algorithm *never* produces the correct "shortfilename"!
	if (lpsz == lpLI->lpszDisplayName)
		lpLI->lpszShortFileName = lpsz;
	else
		lpLI->lpszShortFileName = lpsz+1;

	txFont = qd.thePort->txFont;
	TextFont(geneva);

	strcpy(lpLI->szChoppedName, lpLI->lpszShortFileName);
	lpLI->lpszChoppedName = ChopText(lpLI->nColPos[1] - lpLI->nColPos[0] - 4, lpLI->szChoppedName);

	if (lpLI->lpszShortLinkType) {
		strcpy(lpLI->szChoppedLink, lpLI->lpszShortLinkType);
		lpLI->lpszChoppedLink = ChopText(lpLI->nColPos[2] - lpLI->nColPos[1] - 4, lpLI->szChoppedLink);
	}
	else
		lpLI->lpszChoppedLink = NULL;

	TextFont(txFont);
#ifdef UIDLL
   RestoreLpqd(oldQD);
#endif
}




/*
 * EditLinksDialogProc
 *
 * Purpose:
 *  Implements the OLE Edit Links dialog as invoked through the
 *  OleUIEditLinks function.
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment LinksSeg
pascal Boolean
#else
#pragma code_seg("LinksSeg", "SWAPPABLE")
Boolean __pascal
#endif
EditLinksDialogProc(DialogPtr pDialog, EventRecord *pEvent, short *nItem)
{
	PEDITLINKS		pEL;
	GrafPtr			gpSave;
	Boolean			fHooked;
	WindowPtr		pWindow;
	short			in;
	Point			ptWhere;
	char			chKey;
	Cell			cell;
	short			cch;
	LPLINKINFO		lpLI;
	Boolean			fShifted;
	short			itype;
	Handle			h;
	Rect			rc;

	pEL = (PEDITLINKS)GetWRefCon(pDialog);

	GetPort(&gpSave);
	SetPort(pDialog);

	fHooked = FStandardHook((LPOLEUISTANDARD)pEL->lpOEL, pDialog, pEvent, nItem, pEL->lpOEL->lCustData);
	if (fHooked)
	{
		SetPort(gpSave);
		return true;
	}

	switch (pEvent->what)
	{
		case mouseDown:
			in = FindWindow(pEvent->where, &pWindow);
			if ((pWindow == (WindowPtr)pDialog) && (in == inDrag) && (pEL->lpOEL->lpfnHook != NULL))
			{
				DragWindow(pDialog, pEvent->where, &qd.screenBits.bounds);
				SetPort(gpSave);
				return true;
			}

			ptWhere = pEvent->where;
			GlobalToLocal(&ptWhere);

			switch (FindDItem(pDialog, ptWhere) + 1)
			{
				case EL_UITEM_LBOX:
					LClick(ptWhere, pEvent->modifiers, pEL->hListLinks);

					for (cell.h = cell.v = 0; cell.v < (**pEL->hListLinks).dataBounds.bottom; cell.v++)
					{
						cch = sizeof(LPLINKINFO);
						LGetCell(&lpLI, &cch, cell, pEL->hListLinks);
						lpLI->fIsSelected = LGetSelect(false, &cell, pEL->hListLinks);
					}

					ELUpdateAllControls(pDialog, pEL);
					break;

				case EL_UITEM_LINKSOURCE:
					if (CountMItems(pEL->hMenuSource))
					{
						GetDItem(pDialog, EL_UITEM_LINKSOURCE, &itype, &h, &rc);
						LocalToGlobal((Point *)&rc.top);
						PopUpMenuSelect(pEL->hMenuSource, rc.top, rc.left, CountMItems(pEL->hMenuSource));
					}
					break;
			}
			break;

		case keyDown:
		case autoKey:
			chKey = pEvent->message & charCodeMask;
			switch (chKey)
			{
				case PERIODKEY:
					if (!(pEvent->modifiers & cmdKey))
						break;
					//Else fall through...

				case ENTERKEY:
				case RETURNKEY:
				case ESCAPEKEY:
					FlashButton(pDialog, EL_BTN_CLOSE);
					*nItem = EL_BTN_CLOSE;
					SetPort(gpSave);
					return true;

				case UPKEY:
				case DOWNKEY:
					if ((**pEL->hListLinks).dataBounds.bottom > 0)
					{
						fShifted = false;
						SetPt(&cell, 0, 0);
						if (LGetSelect(true, &cell, pEL->hListLinks))
						{
							if ((chKey == UPKEY && cell.v > 0) ||
								(chKey == DOWNKEY && cell.v < (**pEL->hListLinks).dataBounds.bottom - 1))
							{
								LSetSelect(false, cell, pEL->hListLinks);
								cell.v += (chKey == UPKEY ? -1 : 1);
								LSetSelect(true, cell, pEL->hListLinks);

								//If the selection is outside the list's view, scroll it into view
								LRect(&rc, cell, pEL->hListLinks);
								if ((chKey == UPKEY && rc.top < (**pEL->hListLinks).rView.top) ||
									(chKey == DOWNKEY && rc.bottom > (**pEL->hListLinks).rView.bottom))
									LScroll(0, (short)(chKey == UPKEY ? -1 : 1), pEL->hListLinks);
								fShifted = true;
							}
						}
						else
						{
							SetPt(&cell, 0, 0);
							LSetSelect(true, cell, pEL->hListLinks);
							LAutoScroll(pEL->hListLinks);
							fShifted = true;
						}

						if (fShifted)
							ELUpdateAllControls(pDialog, pEL);
					}
					break;
			}
			break;

		case kHighLevelEvent:
			AEProcessAppleEvent(pEvent);
			break;
	}

	SetPort(gpSave);
	return false;
}




/*
 * ELUserItemProc
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
#pragma segment LinksSeg
pascal void
#else
#pragma code_seg("LinksSeg", "SWAPPABLE")
void __pascal
#endif
ELUserItemProc(DialogPtr pDialog, short iitem)
{
	GrafPtr			gpSave;
	short			itype;
	Handle			h;
	Rect			rc;
	PEDITLINKS		pEL;

	GetPort(&gpSave);
	SetPort(pDialog);

	GetDItem(pDialog, iitem, &itype, &h, &rc);
	pEL = (PEDITLINKS)GetWRefCon(pDialog);

	switch (iitem)
	{
		case EL_UITEM_LBOX:
			LUpdate(pDialog->visRgn, pEL->hListLinks);
			PenSize(1, 1);
			FrameRect(&rc);
			break;

		case EL_UITEM_LINKSOURCE:
			InsetRect(&rc, -2, -2);
			EraseRect(&rc);
			InsetRect(&rc, 1, 1);

			if (*pEL->szLinkSource)
			{
				rc.right = rc.left + pEL->nPopupWidth;
				FrameRect(&rc);
				MoveTo(rc.right, (short)(rc.top+2));
				LineTo(rc.right, rc.bottom);
				LineTo((short)(rc.left+2), rc.bottom);

				InsetRect(&rc, 1, 1);
				rc.left += 13;
				TextBox(pEL->szLinkSource, strlen(pEL->szLinkSource), &rc, teFlushLeft);
			}
			break;

		case EL_UITEM_LINKTYPE:
			TextBox(pEL->szLinkType, strlen(pEL->szLinkType), &rc, teFlushLeft);
			break;

		case EL_UITEM_OKOUTLINE:
			DrawDefaultBorder(pDialog, EL_UITEM_OKOUTLINE, true);
			break;
	}

	SetPort(gpSave);
}




/*
 * ELUpdateOneControl
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
#pragma segment LinksSeg
#else
#pragma code_seg("LinksSeg", "SWAPPABLE")
#endif
void ELUpdateOneControl(DialogPtr pDialog, short iitem, Boolean fAvail)
{
	short		itype;
	Handle		h;
	Rect		rc;

	GetDItem(pDialog, iitem, &itype, &h, &rc);

	if ((**(ControlHandle)h).contrlHilite == (fAvail ? 0 : 255))
		return;

	HiliteControl((ControlHandle)h, (short)(fAvail ? 0 : 255));
}




/*
 * ELUpdateAllControls
 *
 * Purpose:
 *  Updates the availability of all the controls in our dialog box.
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment LinksSeg
#else
#pragma code_seg("LinksSeg", "SWAPPABLE")
#endif
void ELUpdateAllControls(DialogPtr pDialog, PEDITLINKS pEL)
{
	short				nNumSel;	// 0 = none, 1 = one, 2 = two or more
	ListHandle			hListBox = pEL->hListLinks;
	Cell				cell;
	short				cch;
	LPLINKINFO			lpLI;
	char				*lpszType = NULL;
	char				*lpszSource = NULL;
	int					cAuto = 0;
	int					cManual = 0;
	Boolean				bSameType = true;
	Boolean				bSameSource = true;
	char				sz[OLEUI_CCHPATHMAX];
	char				szItem[OLEUI_CCHPATHMAX];
	short				itype;
	ControlHandle		h;
	Rect				rc;
	short				iitem;

	SetPt(&cell, 0, 0);
	nNumSel = (short)LGetSelect(true, &cell, hListBox);
	cell.v++;
	if (LGetSelect(true, &cell, hListBox))
		nNumSel++;

	ELUpdateOneControl(pDialog, (short)EL_BTN_UPDATENOW, (Boolean)((nNumSel > 0) &&
								!(pEL->lpOEL->dwFlags & ELF_DISABLEUPDATENOW)));
	ELUpdateOneControl(pDialog, (short)EL_BTN_OPENSOURCE, (Boolean)((nNumSel == 1) &&
								!(pEL->lpOEL->dwFlags & ELF_DISABLEOPENSOURCE)));
	ELUpdateOneControl(pDialog, (short)EL_BTN_CHANGESOURCE, (Boolean)((nNumSel == 1) &&
								!(pEL->lpOEL->dwFlags & ELF_DISABLECHANGESOURCE)));
	ELUpdateOneControl(pDialog, (short)EL_BTN_BREAKLINK, (Boolean)((nNumSel > 0) &&
								!(pEL->lpOEL->dwFlags & ELF_DISABLECANCELLINK)));
	ELUpdateOneControl(pDialog, (short)EL_RBTN_AUTOMATIC, (Boolean)(nNumSel > 0));
	ELUpdateOneControl(pDialog, (short)EL_RBTN_MANUAL, (Boolean)(nNumSel > 0));

	SetPt(&cell, 0, 0);
	while (LGetSelect(true, &cell, hListBox))
	{
		cch = sizeof(LPLINKINFO);
		LGetCell(&lpLI, &cch, cell, hListBox);

		if (lpszSource && lpLI->lpszDisplayName)
		{
			if (bSameSource && strcmp(lpszSource, lpLI->lpszDisplayName))
				bSameSource = false;
		}
		else
			lpszSource = lpLI->lpszDisplayName;

		if (lpszType && lpLI->lpszFullLinkType)
		{
			if (bSameType && strcmp(lpszType, lpLI->lpszFullLinkType))
				bSameType = false;
		}
		else
			lpszType = lpLI->lpszFullLinkType;
		
		if (lpLI->fIsAuto)
			cAuto++;
		else
			cManual++;

		cell.v++;
	}

	GetDItem(pDialog, EL_RBTN_AUTOMATIC, &itype, (Handle *)&h, &rc);
	SetCtlValue(h, (short)(cAuto && !cManual));

	GetDItem(pDialog, EL_RBTN_MANUAL, &itype, (Handle *)&h, &rc);
	SetCtlValue(h, (short)(!cAuto && cManual));

	/*
	 * fill full source in popup menu below list
	 */

	//First delete all menu items
	while (CountMItems(pEL->hMenuSource))
		DelMenuItem(pEL->hMenuSource, 1);

	if (bSameSource && lpszSource)
	{
		strcpy(sz, lpLI->lpszDisplayName);

		if (lpLI->clenFileName)
		{
			if (lpLI->clenFileName < strlen(lpLI->lpszDisplayName)) {
				strcpy(szItem, sz + lpLI->clenFileName + 1);
				c2pstr(szItem);
				InsMenuItem(pEL->hMenuSource, (StringPtr)"\pfoo", 0);
				SetItem(pEL->hMenuSource, 1, (StringPtr)szItem);	// SetItem doesn't interpret metacharacters
  			}

			lpszSource = sz + lpLI->clenFileName;
			*lpszSource-- = 0;

			while (true)
			{
				while (lpszSource > sz && (*(lpszSource-1) != ':'))
					lpszSource--;
				strcpy(szItem , lpszSource);
				c2pstr(szItem);
  			   InsMenuItem(pEL->hMenuSource, (StringPtr)"\pfoo", 0);
				SetItem(pEL->hMenuSource, 1, (StringPtr)szItem);	// SetItem doesn't interpret metacharacters
				if (lpszSource == sz)
					break;
				*--lpszSource = 0;
			}
		}
		else
		{
			c2pstr(sz);
			AppendMenu(pEL->hMenuSource, (StringPtr)"\pfoo");
			SetItem(pEL->hMenuSource, CountMItems(pEL->hMenuSource), (StringPtr)sz);
		}
	}

	pEL->nPopupWidth = 0;
	for (iitem = 1; iitem <= CountMItems(pEL->hMenuSource); iitem++)
	{
		GetItem(pEL->hMenuSource, iitem, (StringPtr)szItem);
		if (StringWidth((StringPtr)szItem) + 26 > pEL->nPopupWidth)
			pEL->nPopupWidth = StringWidth((StringPtr)szItem) + 26;
	}

   GetDItem(pDialog, EL_UITEM_LINKSOURCE, &itype, (Handle *)&h, &rc);
	if (pEL->nPopupWidth > rc.right - rc.left)
		pEL->nPopupWidth = rc.right - rc.left;


	GetItem(pEL->hMenuSource, CountMItems(pEL->hMenuSource), (StringPtr)pEL->szLinkSource);
	p2cstr((unsigned char *)pEL->szLinkSource);

	GetDItem(pDialog, EL_UITEM_LINKSOURCE, &itype, (Handle *)&h, &rc);
	InsetRect(&rc, -2, -2);
	InvalRect(&rc);

	/* fill full link type name in static
	**	"type" text box
	*/
	if (!bSameType || !lpszType)
		lpszType = szNULL;

	strcpy(pEL->szLinkType, lpszType);

	GetDItem(pDialog, EL_UITEM_LINKTYPE, &itype, (Handle *)&h, &rc);
	InvalRect(&rc);
}




/*
 * Container_UpdateNow
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment LinksSeg
#else
#pragma code_seg("LinksSeg", "SWAPPABLE")
#endif
HRESULT Container_UpdateNow(DialogPtr pDialog, PEDITLINKS pEL)
{
	HRESULT					hErr;
	short					itype;
	Handle					h;
	Rect					rc;
	LPLINKINFO				lpLI;
	LPOLEUILINKCONTAINER	lpOleUILinkCntr = pEL->lpOEL->lpOleUILinkContainer;
	short					cch;
	Cell					cell;
	ListHandle				hListBox = pEL->hListLinks;
	Boolean					bUpdate = false;
    Str255                  StrTemp;

	OleDbgAssert(lpOleUILinkCntr);

	//REVIEW: Need to check if we actually have a selection first
	if (!pEL->fClose)
	{
		GetDItem(pDialog, EL_BTN_CLOSE, &itype, &h, &rc);
        GetIndString(StrTemp, DIDEditLinks, IDS_BTN_CLOSE);
		SetCTitle((ControlHandle)h, StrTemp);
		pEL->fClose = true;
	}

	SetPt(&cell, 0, 0);
	while (LGetSelect(true, &cell, hListBox))
	{
		cch = sizeof(LPLINKINFO);
		LGetCell(&lpLI, &cch, cell, hListBox);

		OLEDBG_BEGIN2("IOleUILinkContainer::UpdateLink called\r\n");
		hErr = lpOleUILinkCntr->lpVtbl->UpdateLink(
				lpOleUILinkCntr,
				lpLI->dwLink,
				true,
				false);
		OLEDBG_END2
		bUpdate = true;

		if (hErr != NOERROR) {
			OleDbgOutHResult("WARNING: IOleUILinkContainer::UpdateLink returned",hErr);
			break;
		}

		cell.v++;
	}

	if (bUpdate)
		RefreshLinkLB(hListBox, lpOleUILinkCntr);

	return hErr;
}




/*
 * Container_OpenSource
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment LinksSeg
#else
#pragma code_seg("LinksSeg", "SWAPPABLE")
#endif
HRESULT Container_OpenSource(DialogPtr pDialog, PEDITLINKS pEL)
{
	HRESULT					hErr;
	short					itype;
	Handle					h;
	Rect					rc;
	LPLINKINFO				lpLI;
	LPOLEUILINKCONTAINER	lpOleUILinkCntr = pEL->lpOEL->lpOleUILinkContainer;
	short					cch;
	Cell					cell;
	ListHandle				hListBox = pEL->hListLinks;
    Str255                  StrTemp;

	OleDbgAssert(lpOleUILinkCntr);

	//REVIEW: Need to check if we actually have a selection first
	if (!pEL->fClose)
	{
		GetDItem(pDialog, EL_BTN_CLOSE, &itype, &h, &rc);
        GetIndString(StrTemp, DIDEditLinks, IDS_BTN_CLOSE);
		SetCTitle((ControlHandle)h, StrTemp);
		pEL->fClose = true;
	}

	SetPt(&cell, 0, 0);
	if (!LGetSelect(true, &cell, hListBox))
		return NOERROR;

	cch = sizeof(LPLINKINFO);
	LGetCell(&lpLI, &cch, cell, hListBox);

	OLEDBG_BEGIN2("IOleUILinkContainer::OpenLinkSource called\r\n");
	hErr = lpOleUILinkCntr->lpVtbl->OpenLinkSource(
			lpOleUILinkCntr,
			lpLI->dwLink);
	OLEDBG_END2

	UpdateLinkLBItem(hListBox, cell.v, pEL, true);
	if (hErr != NOERROR)
		OleDbgOutHResult("WARNING: IOleUILinkContainer::OpenLinkSource returned",hErr);

	return hErr;
}




/*
 * Container_ChangeSource
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment LinksSeg
#else
#pragma code_seg("LinksSeg", "SWAPPABLE")
#endif
Boolean Container_ChangeSource(DialogPtr pDialog, PEDITLINKS pEL)
{
	unsigned int			uRet;
	short					itype;
	Handle					h;
	Rect					rc;
	LPLINKINFO				lpLI;
	short					cch;
	Cell					cell;
	ListHandle				hListBox = pEL->hListLinks;
	LPOLEUILINKCONTAINER	lpOleUILinkCntr = pEL->lpOEL->lpOleUILinkContainer;
	CHANGESOURCEHOOKDATA	cshData;
    Str255                  StrTemp;

	//REVIEW: Need to check if we actually have a selection first
	if (!pEL->fClose)
	{
		GetDItem(pDialog, EL_BTN_CLOSE, &itype, &h, &rc);
        GetIndString(StrTemp, DIDEditLinks, IDS_BTN_CLOSE);
		SetCTitle((ControlHandle)h, StrTemp);
		pEL->fClose = true;
	}

	cshData.lpEL = (LPEDITLINKS)pEL;
	cshData.lpszFrom = NULL;
	cshData.lpszTo = NULL;

	SetPt(&cell, 0, 0);
	if (LGetSelect(true, &cell, hListBox))
	{
		cch = sizeof(LPLINKINFO);
		LGetCell(&lpLI, &cch, cell, hListBox);

		cshData.lpLI = lpLI;
  		cshData.fValidLink = lpLI->fSourceAvailable;

		uRet = (unsigned int)ChangeSource(pDialog, &cshData);

		/* If Cancel is pressed in any ChangeSource dialog, stop
		**	the ChangeSource processing for all links.
		*/
		if (!uRet)
			return true;

		// first force the dialog to update so the update event
		// doesn't come through during the LRPC that can result
		// from this call.
		if (!EmptyRgn(((WindowPeek)pDialog)->updateRgn))
		{
			BeginUpdate(pDialog);
			UpdateDialog(pDialog, pDialog->visRgn);
			EndUpdate(pDialog);
		}

		UpdateLinkLBItem(hListBox, cell.v, pEL, true);

		if (cshData.lpszFrom && cshData.lpszTo)
		{
			ChangeAllLinks(hListBox, lpOleUILinkCntr, cshData.lpszFrom, cshData.lpszTo);
			OleStdFree(cshData.lpszFrom);
			OleStdFree(cshData.lpszTo);
		}
	}

	return true;
}




/*
 * Container_BreakLink
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment LinksSeg
#else
#pragma code_seg("LinksSeg", "SWAPPABLE")
#endif
HRESULT Container_BreakLink(DialogPtr pDialog, PEDITLINKS pEL)
{
	HRESULT					hErr;
	LPMONIKER				lpmk;
	short					itype;
	Handle					h;
	Rect					rc;
	LPLINKINFO				lpLI;
	short					cch;
	Cell					cell;
	LPOLEUILINKCONTAINER	lpOleUILinkCntr = pEL->lpOEL->lpOleUILinkContainer;
	ListHandle				hListBox = pEL->hListLinks;
	Boolean					bUpdate = false;
    Str255                  StrTemp;

	OleDbgAssert(lpOleUILinkCntr);

	lpmk = NULL;

	//REVIEW: Need to check if we actually have a selection first
	if (!pEL->fClose)
	{
		GetDItem(pDialog, EL_BTN_CLOSE, &itype, &h, &rc);
        GetIndString(StrTemp, DIDEditLinks, IDS_BTN_CLOSE);
		SetCTitle((ControlHandle)h, StrTemp);
		pEL->fClose = true;
	}

	SetPt(&cell, 0, 0);
	while (LGetSelect(true, &cell, hListBox))
	{
		cch = sizeof(LPLINKINFO);
		LGetCell(&lpLI, &cch, cell, hListBox);

		OLEDBG_BEGIN2("IOleUILinkContainer::CancelLink called\r\n");
		hErr = lpOleUILinkCntr->lpVtbl->CancelLink(
				lpOleUILinkCntr,
				lpLI->dwLink);
		OLEDBG_END2

		if (hErr != NOERROR)
		{
			OleDbgOutHResult("WARNING: IOleUILinkContainer::CancelLink returned",hErr);
			lpLI->fIsMarked = true;
			bUpdate = true;
		}
		else
		{
			// Delete links that we make null from listbox
			LDelRow(1, cell.v, hListBox);
			cell.v++;
		}
	}

	if (bUpdate)
		RefreshLinkLB(hListBox, lpOleUILinkCntr);

	return hErr;
}




/*
 * Container_AutomaticManual
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment LinksSeg
#else
#pragma code_seg("LinksSeg", "SWAPPABLE")
#endif
HRESULT Container_AutomaticManual(DialogPtr pDialog, Boolean fAutoMan, PEDITLINKS pEL)
{
	HRESULT					hErr = NOERROR;
	short					itype;
	Handle					h;
	Rect					rc;
	LPLINKINFO				lpLI;
	short					cch;
	Cell					cell;
	LPOLEUILINKCONTAINER	lpOleUILinkCntr = pEL->lpOEL->lpOleUILinkContainer;
	ListHandle				hListBox = pEL->hListLinks;
	Boolean					bUpdate = false;
    Str255                  StrTemp;

	OleDbgAssert(lpOleUILinkCntr);

	/* Change so looks at flag in structure.  Only update those that
	 * need to be updated.  Make sure to change flag if status changes.
	 */

	//REVIEW: Need to check if we actually have a selection first
	if (!pEL->fClose)
	{
		GetDItem(pDialog, EL_BTN_CLOSE, &itype, &h, &rc);
        GetIndString(StrTemp, DIDEditLinks, IDS_BTN_CLOSE);
		SetCTitle((ControlHandle)h, StrTemp);
		pEL->fClose = true;
	}

	SetPt(&cell, 0, 0);
	while (LGetSelect(true, &cell, hListBox))
	{
		cch = sizeof(LPLINKINFO);
		LGetCell(&lpLI, &cch, cell, hListBox);

		if (fAutoMan)  //If switching to AUTOMATIC
		{
			if (!lpLI->fIsAuto)   //Only change MANUAL links
			{
				OLEDBG_BEGIN2("IOleUILinkContainer::SetLinkUpdateOptions called\r\n");
				hErr=lpOleUILinkCntr->lpVtbl->SetLinkUpdateOptions(
						lpOleUILinkCntr,
						lpLI->dwLink,
						OLEUPDATE_ALWAYS);
				OLEDBG_END2

				lpLI->fIsAuto = true;
				lpLI->fIsMarked = true;
				bUpdate = true;
			}
		}
		else   //If switching to MANUAL
		{
			if (lpLI->fIsAuto)  //Only do AUTOMATIC Links
			{
				OLEDBG_BEGIN2("IOleUILinkContainer::SetLinkUpdateOptions called\r\n");
				hErr=lpOleUILinkCntr->lpVtbl->SetLinkUpdateOptions(
						lpOleUILinkCntr,
						lpLI->dwLink,
						OLEUPDATE_ONCALL);
				OLEDBG_END2

				lpLI->fIsAuto = false;
				lpLI->fIsMarked = true;
				bUpdate = true;
			}
		}
		
		if (hErr != NOERROR)
		{
			OleDbgOutHResult("WARNING: IOleUILinkContainer::SetLinkUpdateOptions returned",hErr);
			break;
		}

		cell.v++;
	}

	if (bUpdate)
		RefreshLinkLB(hListBox, lpOleUILinkCntr);

	return hErr;
}




/*
 * ChangeSource
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment LinksSeg
#else
#pragma code_seg("LinksSeg", "SWAPPABLE")
#endif
Boolean ChangeSource(DialogPtr pDialog, LPCHANGESOURCEHOOKDATA lpCshData)
{
#ifndef _MSC_VER
#pragma unused(pDialog)
#endif

   DialogTHndl	hDialogT;
	Rect		   rc;
	Point		   pt;
	short		   rgnActive[] = { 2, sfItemFileListUser, CS_EDIT_SOURCE };
#ifdef UIDLL
   short       hostResNum =  SetUpOLEUIResFile();
   void*       oldQD = SetLpqdFromA5();
#endif

	lpCshData->fFirstCall  = true;
	lpCshData->fEditActive = false;
	lpCshData->fTabKey     = false;
	lpCshData->Action      = NOTHING;

  if (!lpCshData->fValidLink)
	{
		lpCshData->fEditActive = true;
		rgnActive[1] = CS_EDIT_SOURCE;
		rgnActive[2] = sfItemFileListUser;
	}


	hDialogT = (DialogTHndl)GetResource('DLOG', (short)DIDChangeSource);
	if (ResError())
   {
#ifdef UIDLL
      ClearOLEUIResFile(hostResNum);
      RestoreLpqd(oldQD);
#endif
      return false;
   }
		

	rc = (**hDialogT).boundsRect;
	OffsetRect(&rc, (short)-rc.left, (short)-rc.top);

	pt.h = (qd.screenBits.bounds.right  - rc.right)/2;
	pt.v = (qd.screenBits.bounds.bottom - rc.bottom - 20 - GetMBarHeight())/3 + 20 + GetMBarHeight();

#ifndef __powerc
	CustomGetFile(NULL, -1, NULL, &lpCshData->sfReply, DIDChangeSource, pt,
				  (DlgHookYDProcPtr)ChangeSourceDialogProc,
				  (ModalFilterYDProcPtr)ChangeSourceModalProc,
				  rgnActive,
				  (ActivateYDProcPtr)ChangeSourceActivateProc,
				  lpCshData);
#else
	CustomGetFile(NULL, -1, NULL, &lpCshData->sfReply, DIDChangeSource, pt,
				  gChangeSourceDialogProc,
				  gChangeSourceModalProc,
				  rgnActive,
				  gChangeSourceActivateProc,
				  lpCshData);
#endif

#ifdef UIDLL
   ClearOLEUIResFile(hostResNum);
   RestoreLpqd(oldQD);
#endif
	return lpCshData->fHitOK;
}




/*
 * ChangeSourceDialogProc
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment LinksSeg
pascal short
#else
#pragma code_seg("LinksSeg", "SWAPPABLE")
short __pascal
#endif
ChangeSourceDialogProc(short iitem, DialogPtr pDialog, LPCHANGESOURCEHOOKDATA lpCshData)
{
	GrafPtr					gpSave;
	LPLINKINFO				lpLI;
	char					*psz;
	TEHandle				hText;
	LPOLEUILINKCONTAINER	lpOleUILinkCntr;
	char					szTmp[OLEUI_CCHPATHMAX];
	HRESULT					hErr;
	unsigned int			uRet;
	unsigned long			ulChEaten;
	CursHandle				hCursor = nil;
	short					itype;
	Handle					h;
	Rect					rc;
    Str255                  StrTemp;

	//Only continue if we're in the main dialog
	if (GetWRefCon(pDialog) != sfMainDialogRefCon)
		return iitem;

	GetPort(&gpSave);
	SetPort(pDialog);

	switch (iitem)
	{
		case sfHookFirstCall:
			//Sometimes sfHookFirstCall is sent after the dialog
			//has already been up for a while. Since calling the
			//initialization code more than once causes problems
			//we don't allow it.
			if (!lpCshData->fFirstCall)
				break;
			lpCshData->fFirstCall = false;

			lpLI = lpCshData->lpLI;

			lpCshData->fFlashOK        = true;
			lpCshData->bFileNameStored = true;
			lpCshData->bItemNameStored = (lpLI->clenFileName < strlen(lpLI->lpszDisplayName));
			lpCshData->nFileLength     = 0;
			lpCshData->szFileName[0]   = 0;
			lpCshData->szEdit[0]       = 0;

			if (lpLI->lpszDisplayName)
			{
				if (lpLI->clenFileName > 0)
				{
					for (psz = lpLI->lpszDisplayName + lpLI->clenFileName;
						 psz > lpLI->lpszDisplayName && *(psz - 1) != ':';
						 psz--, lpCshData->nFileLength++)
						;

					strcpy(lpCshData->szFileName, psz);
					lpCshData->szFileName[lpCshData->nFileLength] = 0;

					strcpy(lpCshData->szEdit, psz);
				}
				else
					strcpy(lpCshData->szEdit, lpLI->lpszDisplayName);
			}

			if (lpLI->lpszItemName)
				strcpy(lpCshData->szItemName, lpLI->lpszItemName);
			else
				lpCshData->szItemName[0] = 0;

			if (lpCshData->szFileName[0] != 0)
			{
				strcpy(szTmp, lpLI->lpszDisplayName);
				szTmp[lpLI->clenFileName] = 0;
				c2pstr(szTmp);
				if (FSMakeFSSpec(lpCshData->sfReply.sfFile.vRefNum,
								 lpCshData->sfReply.sfFile.parID,
								 (StringPtr)szTmp, &lpCshData->fssFile) == noErr)
				{
					lpCshData->Action = SET_SELECTION;
				}
		 	}

			//Convert to a Pascal string
			strcpy(szTmp, lpCshData->szEdit);
			c2pstr(szTmp);

			//Initialize the edit control
			GetDItem(pDialog, CS_EDIT_SOURCE, &itype, &h, &rc);
			SetIText(h, (StringPtr)szTmp);

			GetDItem(pDialog, CS_UITEM_OKOUTLINE, &itype, &h, &rc);
#ifndef __powerc
			SetDItem(pDialog, CS_UITEM_OKOUTLINE, itype, (Handle)CSUserItemProc, &rc);
#else
			SetDItem(pDialog, CS_UITEM_OKOUTLINE, itype, (Handle)gCSUserItemProc, &rc);
#endif

  		// Hide the help button if required
			if (!(lpCshData->lpEL->lpOEL->dwFlags & ELF_SHOWHELP))
				HideDItem(pDialog, CS_BTN_HELP);

			break;

		case sfHookNullEvent:
			switch (lpCshData->Action)
			{
				case SET_SELECTION:
					lpCshData->Action = NOTHING;

					lpCshData->sfReply.sfFile = lpCshData->fssFile;
					iitem = sfHookChangeSelection;

					break;

				case CHECK_SELECTION:
					lpCshData->Action = NOTHING;

					GetDItem(pDialog, CS_BTN_OK, &itype, &h, &rc);

					//If the current selection hasn't changed, continue
					if (!memcmp((void *)&lpCshData->sfReply.sfFile, (void *)&lpCshData->fssFile, sizeof(FSSpec)))
					{
                        GetIndString(StrTemp, DIDEditLinks, IDS_BTN_OK);
						SetCTitle((ControlHandle)h, StrTemp);
						break;
					}

					lpCshData->fssFile = lpCshData->sfReply.sfFile;

					//Is the current selection a file?
					lpCshData->fFileSelected = (lpCshData->sfReply.sfFile.name[0] &&
												!lpCshData->sfReply.sfIsVolume &&
												!lpCshData->sfReply.sfIsFolder);

					if (lpCshData->fFileSelected)
					{
						BlockMove(lpCshData->sfReply.sfFile.name,
								  (StringPtr)lpCshData->szFileName,
								  lpCshData->sfReply.sfFile.name[0]+1);

						p2cstr((unsigned char *)lpCshData->szFileName);
						lpCshData->nFileLength = strlen(lpCshData->szFileName);

						//Grab the filename and add the itemname to the end
						strcpy(lpCshData->szEdit, lpCshData->szFileName);
						
						if (lpCshData->lpLI->lpszItemName)
							strcat(lpCshData->szEdit, lpCshData->lpLI->lpszItemName);
                        GetIndString(StrTemp, DIDEditLinks, IDS_BTN_OK);
						SetCTitle((ControlHandle)h, StrTemp);
					}
					else
					{
						lpCshData->nFileLength = 0;
						if (lpCshData->lpLI->lpszItemName)
							strcpy(lpCshData->szEdit, lpCshData->lpLI->lpszItemName);
                        GetIndString(StrTemp, DIDEditLinks, IDS_BTN_OPEN);
						SetCTitle((ControlHandle)h, StrTemp);
					}

					//Convert to a Pascal string
					strcpy(szTmp, lpCshData->szEdit);
					c2pstr(szTmp);

					//Initialize the edit control
					GetDItem(pDialog, CS_EDIT_SOURCE, &itype, &h, &rc);
					SetIText(h, (StringPtr)szTmp);

					hText = ((DialogPeek)pDialog)->textH;
					TESetSelect(0, (lpCshData->nFileLength ? lpCshData->nFileLength : 32767), hText);

					break;
			}
			break;

		case sfItemOpenButton:

			//If the current selection is a folder and the list is active, ignore
			if (!lpCshData->fFileSelected && !lpCshData->fEditActive)
				break;

			if (lpCshData->fFlashOK)
				FlashButton(pDialog, CS_BTN_OK);

			hCursor = GetCursor(watchCursor);
			if (hCursor != nil);
				SetCursor(*hCursor);

			lpOleUILinkCntr = lpCshData->lpEL->lpOEL->lpOleUILinkContainer;

			GetDItem(pDialog, CS_EDIT_SOURCE, &itype, &h, &rc);
			GetIText(h, (StringPtr)lpCshData->szEdit);
			p2cstr((unsigned char *)lpCshData->szEdit);

			strncpy(szTmp, lpCshData->szEdit, lpCshData->nFileLength);
			szTmp[lpCshData->nFileLength] = 0;

			if (lpCshData->bFileNameStored && !strcmp(lpCshData->szFileName, szTmp))
			{
				if (lpCshData->nFileLength < strlen(lpCshData->szEdit))
				{
					strcpy(lpCshData->szItemName, lpCshData->szEdit + lpCshData->nFileLength);
					lpCshData->bItemNameStored = true;
				}
			}
			else if (lpCshData->bItemNameStored &&
					 !strcmp(lpCshData->szItemName, lpCshData->szEdit + lpCshData->nEditLength - strlen(lpCshData->szItemName)))
			{
				lpCshData->nFileLength = lpCshData->nEditLength - strlen(lpCshData->szItemName);
				strncpy(lpCshData->szFileName, lpCshData->szEdit, lpCshData->nFileLength);
				lpCshData->szFileName[lpCshData->nFileLength] = 0;
				lpCshData->bFileNameStored = true;
			}
			else
			{
				lpCshData->bItemNameStored = false;
				lpCshData->bFileNameStored = false;
			}

			Ole2UIPathNameFromDirID(lpCshData->fssFile.parID, lpCshData->fssFile.vRefNum, szTmp);
			strcat(szTmp, lpCshData->szEdit);

			if (lpCshData->bItemNameStored)
				lpCshData->nFileLength = strlen(szTmp) - strlen(lpCshData->szItemName);
			else
				lpCshData->nFileLength = strlen(szTmp);

			// Try to validate link source change
			hErr = lpOleUILinkCntr->lpVtbl->SetLinkSource(
					lpOleUILinkCntr,
					lpCshData->lpLI->dwLink,
					szTmp,
					lpCshData->nFileLength,
					&ulChEaten,
					true);

			// Link source change  not validated
			if (hErr != NOERROR) {

				uRet = OleUIPromptUser(IDD_INVALIDSOURCE);

				if (uRet == PU_BTN_YES)
				{
					/* User wants to correct invalid link. Set the edit
					**	control selection to the invalid part of the contents.
					*/
					hText = ((DialogPeek)pDialog)->textH;
					TESetSelect(ulChEaten - (strlen(szTmp) - strlen(lpCshData->szEdit)), 32767, hText);
					InvalRect(&(**hText).viewRect);
					iitem = sfHookSetActiveOffset + CS_EDIT_SOURCE;
					break;
				}
				else
				{
					/* User does not want to correct invalid link. So set
					**	the link source but don't validate the link.
					*/
					hErr = lpOleUILinkCntr->lpVtbl->SetLinkSource(
							lpOleUILinkCntr,
							lpCshData->lpLI->dwLink,
							szTmp,
							lpCshData->nFileLength,
							&ulChEaten,
							false);
					lpCshData->fValidLink = false;
				}
			}
			else
				lpCshData->fValidLink = true;	// Link source change validated

			strcpy(lpCshData->lpLI->lpszDisplayName, szTmp);
			lpCshData->lpLI->clenFileName = lpCshData->nFileLength;
			BreakString(lpCshData->lpLI);

			if (lpCshData->bItemNameStored && lpCshData->bFileNameStored)
			{
				DiffPrefix(lpCshData->lpLI->lpszDisplayName,
						   szTmp,
						   &lpCshData->lpszFrom,
						   &lpCshData->lpszTo);

				/* we keep the strings if there is a difference between the
				**	lpszFrom and lpszTo strings AND if the change is only
				**	in the file portion otherwise free them and other
				**	links won't be compared.
				*/
				if ((strcmp(lpCshData->lpszTo, lpCshData->lpszFrom) == 0) ||
					(strlen(lpCshData->lpszTo) > lpCshData->nFileLength))
				{
					if (lpCshData->lpszFrom)
					{
						OleStdFree(lpCshData->lpszFrom);
						lpCshData->lpszFrom = NULL;
					}
					if (lpCshData->lpszTo)
					{
						OleStdFree(lpCshData->lpszTo);
						lpCshData->lpszTo = NULL;
					}
				}
			}

			SetCursor(&qd.arrow);

			//Pretend like cancel was hit to ensure that the dialog goes away
			lpCshData->fHitOK = true;
			iitem = sfItemCancelButton;

			break;

		case sfItemCancelButton:
			lpCshData->fHitOK = false;
			break;
	}

	SetPort(gpSave);

	return iitem;
}




/*
 * ChangeSourceModalProc
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment LinksSeg
pascal Boolean
#else
#pragma code_seg("LinksSeg", "SWAPPABLE")
Boolean __pascal
#endif
ChangeSourceModalProc(DialogPtr pDialog, EventRecord *pEvent, short *pItem, LPCHANGESOURCEHOOKDATA lpCshData)
{

	GrafPtr		gpSave;
	Point		ptClick;
	char		chKey;
	short		iitem;
	short		itype;
	Handle		h;
	Rect		rc;

	//Only continue if we're in the main dialog
	if (GetWRefCon(pDialog) != sfMainDialogRefCon)
		return false;

	GetPort(&gpSave);
	SetPort(pDialog);

	switch (pEvent->what)
	{
		case mouseDown:
			ptClick = pEvent->where;
			GlobalToLocal(&ptClick);

			//Determine which item was hit
			iitem = FindDItem(pDialog, ptClick) + 1;

			if (iitem == CS_BTN_OK)
			{
				GetDItem(pDialog, CS_BTN_OK, &itype, &h, &rc);
				if (TrackControl((ControlHandle)h, ptClick, NULL))
				{
					lpCshData->fFlashOK = false;
					*pItem = sfItemOpenButton;

					if (!lpCshData->fFileSelected && !lpCshData->fEditActive)
						lpCshData->Action = CHECK_SELECTION;	//Check if the file selection has changed
				}
				SetPort(gpSave);
				return true;
			}

			//If an item was hit and it wasn't our edit control, check if the currently selected file has changed
			if (iitem != 0 && iitem != CS_EDIT_SOURCE)
				lpCshData->Action = CHECK_SELECTION;	//Check if the file selection has changed

			break;

		case keyDown:
		case autoKey:
			chKey = pEvent->message & charCodeMask;
			switch (chKey)
			{
				case ENTERKEY:
				case RETURNKEY:
					FlashButton(pDialog, CS_BTN_OK);
					lpCshData->fFlashOK = false;
					*pItem = sfItemOpenButton;

					if (!lpCshData->fFileSelected && !lpCshData->fEditActive)
						lpCshData->Action = CHECK_SELECTION;	//Check if the file selection has changed

					SetPort(gpSave);
					return true;

				default:
					if (!lpCshData->fEditActive)
					{
						//Was the tab key hit (edit control is about to be activated)?
						if ((pEvent->message & charCodeMask) == TABKEY)
							lpCshData->fTabKey = true;
						else
							lpCshData->Action = CHECK_SELECTION;	//Check if the file selection has changed
					}
					break;
			}
			break;
	}

	SetPort(gpSave);

	return false;
}




/*
 * ChangeSourceActivateProc
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment LinksSeg
pascal void
#else
#pragma code_seg("LinksSeg", "SWAPPABLE")
void __pascal
#endif
ChangeSourceActivateProc(DialogPtr pDialog, short iitem, Boolean fActivating, LPCHANGESOURCEHOOKDATA lpCshData)
{
	TEHandle	hText = ((DialogPeek)pDialog)->textH;
	char		szTmp[OLEUI_CCHPATHMAX];
	short		itype;
	Handle		h;
	Rect		rc;
    Str255      StrTemp;

	//Only continue if we're in the main dialog
	if (GetWRefCon(pDialog) != sfMainDialogRefCon)
		return;

	if (iitem == CS_EDIT_SOURCE)
	{
		GetDItem(pDialog, CS_BTN_OK, &itype, &h, &rc);

		if (fActivating)
		{
			//Was the tab key hit?
			if (lpCshData->fTabKey)
			{
				TESetSelect(0, (lpCshData->nFileLength ? lpCshData->nFileLength : 32767), hText);
				lpCshData->fTabKey = false;
			}

            GetIndString(StrTemp, DIDEditLinks, IDS_BTN_OK);
			SetCTitle((ControlHandle)h, StrTemp);

			lpCshData->fEditActive = true;
		}
		else
		{
			lpCshData->nEditLength = (**hText).teLength;
			if (lpCshData->nEditLength >= OLEUI_CCHPATHMAX)
				lpCshData->nEditLength = OLEUI_CCHPATHMAX - 1;

			BlockMove(*(**hText).hText, lpCshData->szEdit, lpCshData->nEditLength);
			lpCshData->szEdit[lpCshData->nEditLength] = 0;

			strncpy(szTmp, lpCshData->szEdit, lpCshData->nFileLength);
			szTmp[lpCshData->nFileLength] = 0;

			if (lpCshData->bFileNameStored && !strcmp(lpCshData->szFileName, szTmp))
			{
				strcpy(lpCshData->szItemName, lpCshData->szEdit + lpCshData->nFileLength);
				lpCshData->bItemNameStored = true;
			}
			else if (lpCshData->bItemNameStored &&
					 !strcmp(lpCshData->szItemName, lpCshData->szEdit + lpCshData->nEditLength - strlen(lpCshData->szItemName)))
			{
				lpCshData->nFileLength = lpCshData->nEditLength - strlen(lpCshData->szItemName);
				strncpy(lpCshData->szFileName, lpCshData->szEdit, lpCshData->nFileLength);
				lpCshData->szFileName[lpCshData->nFileLength] = 0;
				lpCshData->bFileNameStored = true;
			}
			else
			{
				lpCshData->bItemNameStored = false;
				lpCshData->bFileNameStored = false;
			}

			//If the filename has changed, deselect current file in list
			if (!lpCshData->bFileNameStored)
			{
				lpCshData->fssFile.name[0] = 0;
				lpCshData->Action = SET_SELECTION;	//Update the current file selection
			}

			//Is the current selection a folder or file?
			if (lpCshData->fFileSelected)
            {
                GetIndString(StrTemp, DIDEditLinks, IDS_BTN_OK);
        		SetCTitle((ControlHandle)h, StrTemp);
            }
			else
            {
			    GetIndString(StrTemp, DIDEditLinks, IDS_BTN_CANCEL);
            	SetCTitle((ControlHandle)h, StrTemp);
            }

			lpCshData->fEditActive = false;
		}
	}
}




/*
 * CSUserItemProc
 *
 * Purpose:
 *  This routine draws outline around the OK button
 *
 * Parameters:
 *  pWindow			WindowPtr to the dialog
 *  iitem			dialog item number
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment LinksSeg
pascal void
#else
#pragma code_seg("LinksSeg", "SWAPPABLE")
void __pascal
#endif
CSUserItemProc(DialogPtr pDialog, short iitem)
{
#ifndef _MSC_VER
#pragma unused(iitem)
#endif

	GrafPtr		gpSaved;
	PenState	psSaved;
	short		itype;
	Handle		h;
	Rect		rc;
	short		nOval;
#ifdef UIDLL
   void*    oldQD = SetLpqdFromA5();
#endif

	GetPort(&gpSaved);
	SetPort(pDialog);

	GetPenState(&psSaved);
	PenNormal();

	GetDItem(pDialog, CS_BTN_OK, &itype, &h, &rc);

	if ((**(ControlHandle)h).contrlHilite == 255)
		PenPat((ConstPatternParam)&qd.gray);

	GetDItem(pDialog, CS_UITEM_OKOUTLINE, &itype, &h, &rc);

	nOval = (rc.bottom - rc.top) / 2 + 2;

	PenSize(3, 3);
	FrameRoundRect(&rc, nOval, nOval);

	SetPenState(&psSaved);
	SetPort(gpSaved);
#ifdef UIDLL
   RestoreLpqd(oldQD);
#endif

}




/* DiffPrefix
 * ----------
 *
 *  Purpose:
 *      Compare (case-insensitive) two strings and return the prefixes of the
 *      the strings formed by removing the common suffix string from them.
 *      Integrity of tokens (directory name, filename and object names) are
 *      preserved. Note that the prefixes are converted to upper case
 *      characters.
 *
 *  Parameters:
 *      lpsz1           string 1
 *      lpsz2           string 2
 *      lplpszPrefix1   prefix of string 1
 *      lplpszPrefix2   prefix of string 2
 *
 *  Returns:
 *
 */

#ifndef _MSC_VER
#pragma segment LinksSeg
#else
#pragma code_seg("LinksSeg", "SWAPPABLE")
#endif
void DiffPrefix(char *lpsz1, char *lpsz2, char **lplpszPrefix1, char **lplpszPrefix2)
{
	char	*lpstr1;
	char	*lpstr2;

	*lplpszPrefix1 = NULL;
	*lplpszPrefix2 = NULL;
	*lplpszPrefix1 = OleStdMalloc(strlen(lpsz1)+1);
	if (!*lplpszPrefix1)
		return;

	*lplpszPrefix2 = OleStdMalloc(strlen(lpsz2)+1);
	if (!*lplpszPrefix2)
	{
		OleStdFree(*lplpszPrefix1);
		*lplpszPrefix1 = NULL;
		return;
	}

	strcpy(*lplpszPrefix1, lpsz1);
	strcpy(*lplpszPrefix2, lpsz2);

	lpstr1 = *lplpszPrefix1 + strlen(*lplpszPrefix1);
	lpstr2 = *lplpszPrefix2 + strlen(*lplpszPrefix2);

	while ((lpstr1 > *lplpszPrefix1) && (lpstr2 > *lplpszPrefix2))
	{
		lpstr1--;
		lpstr2--;
		if (*lpstr1 != *lpstr2)
		{
			lpstr1++;
			lpstr2++;
			break;
		}
	}

	for (; *lpstr1 && *lpstr1!=':' && *lpstr1!='!'; lpstr1++)
		;
	for (; *lpstr2 && *lpstr2!=':' && *lpstr2!='!'; lpstr2++)
		;

	*lpstr1 = 0;
	*lpstr2 = 0;
}




/*
 * EditLinksCleanup
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
#pragma segment LinksSeg
#else
#pragma code_seg("LinksSeg", "SWAPPABLE")
#endif
void EditLinksCleanup(DialogPtr pDialog)
{
	PEDITLINKS		pEL;

	pEL = (PEDITLINKS)GetWRefCon(pDialog);

	if (pEL)
	{
		if (pEL->hListLinks)
			LDispose(pEL->hListLinks);
		if (pEL->hMenuSource)
		{
			DeleteMenu(kLinkPathMenu);
			DisposeMenu(pEL->hMenuSource);
		}
		DisposePtr((Ptr)pEL);
	}

	DisposDialog(pDialog);
}
