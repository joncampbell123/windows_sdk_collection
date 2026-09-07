/*
 * OLE2UI.C
 *
 * Contains initialization routines and miscellaneous API implementations for
 * the OLE 2.0 User Interface Support Library.
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



#include <ole2.h>

#include "ole2ui.h"
#include "common.h"
#include <stdarg.h>
#include <stdio.h>
#ifndef _MSC_VER
#include <resources.h>
#else
#include <resource.h>
#endif


OLEDBGDATA

// local function prototypes
#ifndef _MSC_VER
static pascal Boolean PromptUserDialogProc(DialogPtr pDialog, EventRecord *pEvent, short *nItem);
#else
static Boolean __pascal PromptUserDialogProc(DialogPtr pDialog, EventRecord *pEvent, short *nItem);
#endif
static unsigned int PromptUserInit(DialogPtr pDialog, va_list argptr, POLEUISTANDARD pUI);
static void PromptUserCleanup(DialogPtr pDialog);

#ifndef _MSC_VER
static pascal Boolean UpdateLinksDialogProc(DialogPtr pDialog, EventRecord *pEvent, short *nItem);
#else
static Boolean __pascal UpdateLinksDialogProc(DialogPtr pDialog, EventRecord *pEvent, short *nItem);
#endif
static unsigned int UpdateLinksInit(DialogPtr pDialog, POLEUISTANDARD pOUI);
static void UpdateLinksCleanup(DialogPtr pDialog);

#ifdef __powerc

RoutineDescriptor gRDUpdateLinksDialogProc =
   BUILD_ROUTINE_DESCRIPTOR(uppModalFilterProcInfo, UpdateLinksDialogProc);
ModalFilterUPP gUpdateLinksDialogProc = &gRDUpdateLinksDialogProc;

RoutineDescriptor gRDPromptUserDialogProc =
   BUILD_ROUTINE_DESCRIPTOR(uppModalFilterProcInfo, PromptUserDialogProc);
static ModalFilterUPP gPromptUserDialogProc = &gRDPromptUserDialogProc;

#endif

#define DIDUpdateLinks			10006


// local structure definition
typedef struct tagUPDATELINKS
{
    LPOLEUILINKCONTAINER	lpOleUILinkCntr;    // pointer to Link Container
    unsigned int			cLinks;             // total number of links
    unsigned int			cUpdated;           // number of links updated
    unsigned long			dwLink;             // pointer to link
    Boolean					fError;             // error flag
    char*					lpszTitle;          // caption for dialog box
	POLEUISTANDARD			pUI;				// pointer to OLEUISTANDARD structure
} UPDATELINKS, *PUPDATELINKS, *LPUPDATELINKS;


/*
 * OleUIAddVerbMenu
 *
 * Purpose:
 *  Add the Verb menu for the specified object to the given menu.  If the
 *  object has one verb, we directly add the verb to the given menu.  If
 *  the object has multiple verbs we create a cascading sub-menu.
 *
 * Parameters:
 *  lpObj           LPOLEOBJECT pointing to the selected object.  If this
 *                  is NULL, then we create a default disabled menu item.
 *
 *  lpszShortType   char* with short type name (AuxName==2) corresponding
 *                  to the lpOleObj. if the string is NOT known, then NULL
 *                  may be passed. if NULL is passed, then
 *                  IOleObject::GetUserType will be called to retrieve it.
 *                  if the caller has the string handy, then it is faster
 *                  to pass it in.
 *
 *  hEditMenu       Edit menu handle in which to make modifications.
 *
 *	hVerbMenu		Verb menu handle in which to store the verbs
 *
 *	idVerbMenu		ID of the Verb menu
 *
 *  bAddConvert     Boolean specifying whether or not to add a "Convert" item
 *                  to the bottom of the menu (with a separator).
 *
 *  pIdConvert      location to store the position of Convert, if
 *                  bAddConvert is TRUE.
 *
 *
 * Return Value:
 *  Boolean            TRUE if lpObj was valid and we added at least one verb
 *                  to the menu.  FALSE if lpObj was NULL and we created
 *                  a disabled default menu item
 */

#ifndef _MSC_VER
#pragma segment Ole2UISeg
#else
#pragma code_seg("Ole2UISeg", "SWAPPABLE")
#endif
STDAPI_(Boolean) OleUIAddVerbMenu(
		LPOLEOBJECT 	lpOleObj,
		char* 			lpszShortType,
		MenuHandle 		hEditMenu,
		short			idEditMenu,
		MenuHandle		hVerbMenu,
		short			idVerbMenu,
		unsigned int	uVerbPos,
		Boolean 		bAddConvert,
		VerbMenuRec*	pVerbMenuRec
)
{
    LPENUMOLEVERB       lpEnumOleVerb = NULL;
    OLEVERB             oleverb;
    LPUNKNOWN           lpUnk;
    char*               lpszShortTypeName = lpszShortType;
    char*               lpszVerbName = NULL;
    HRESULT             hrErr;
    Boolean             fIsLink = false;
    Boolean             fResult = false;
    int                 cVerbs;
    short				ConvertMenuID = 0;
    short				ConvertMenuItem = 0;
    short				FirstVerbMenuID = 0;
    short				FirstVerbMenuItem = 0;

    static Boolean      fFirstTime = TRUE;
    static char         szBuffer[OLEUI_OBJECTMENUMAX];
    static char         szNoObjectCmd[OLEUI_OBJECTMENUMAX];
    static char         szObjectCmd1Verb[OLEUI_OBJECTMENUMAX];
    static char         szLinkCmd1Verb[OLEUI_OBJECTMENUMAX];
    static char         szObjectCmdNVerb[OLEUI_OBJECTMENUMAX];
    static char         szLinkCmdNVerb[OLEUI_OBJECTMENUMAX];
    static char         szUnknown[OLEUI_OBJECTMENUMAX];
    static char         szEdit[OLEUI_OBJECTMENUMAX];
    static char         szConvert[OLEUI_OBJECTMENUMAX];

#ifdef UIDLL
   short             hostResNum = 0;
#endif


    // only need to load the strings the 1st time
    if (fFirstTime) {

		Handle h;

#ifdef UIDLL
      hostResNum = SetUpOLEUIResFile();
#endif

    	h = GetResource('CSTR', IDS_OLE2UIEDITNOOBJCMD);
    	if (h == nil)
      {
#ifdef UIDLL
         ClearOLEUIResFile(hostResNum);
#endif
          return FALSE;
      }
    	
    	strcpy(szNoObjectCmd, *h);
    	
    	h = GetResource('CSTR', IDS_OLE2UIEDITLINKCMD_1VERB);
    	if (h == nil)
      {
#ifdef UIDLL
         ClearOLEUIResFile(hostResNum);
#endif
          return FALSE;
      }
    	strcpy(szLinkCmd1Verb, *h);
    	    	
    	h = GetResource('CSTR', IDS_OLE2UIEDITOBJECTCMD_1VERB);
    	if (h == nil)
      {
#ifdef UIDLL
         ClearOLEUIResFile(hostResNum);
#endif
          return FALSE;
      }
    	strcpy(szObjectCmd1Verb, *h);
    	
    	h = GetResource('CSTR', IDS_OLE2UIEDITLINKCMD_NVERB);
    	if (h == nil)
      {
#ifdef UIDLL
         ClearOLEUIResFile(hostResNum);
#endif
          return FALSE;
      }
    	strcpy(szLinkCmdNVerb, *h);
    	
    	h = GetResource('CSTR', IDS_OLE2UIEDITOBJECTCMD_NVERB);
    	if (h == nil)
      {
#ifdef UIDLL
         ClearOLEUIResFile(hostResNum);
#endif
          return FALSE;
      }
    	strcpy(szObjectCmdNVerb, *h);
    	
    	h = GetResource('CSTR', IDS_OLE2UIUNKNOWN);
    	if (h == nil)
      {
#ifdef UIDLL
         ClearOLEUIResFile(hostResNum);
#endif
          return FALSE;
      }
    	strcpy(szUnknown, *h);
    	
    	h = GetResource('CSTR', IDS_OLE2UIEDIT);
    	if (h == nil)
      {
#ifdef UIDLL
         ClearOLEUIResFile(hostResNum);
#endif
          return FALSE;
      }
    	strcpy(szEdit, *h);
    	
    	h = GetResource('CSTR', IDS_OLE2UICONVERT);
    	if (h == nil)
      {
#ifdef UIDLL
         ClearOLEUIResFile(hostResNum);
#endif
          return FALSE;
      }

#ifdef UIDLL
      ClearOLEUIResFile(hostResNum);
#endif

    	strcpy(szConvert, *h);
    	
        fFirstTime = FALSE;
    }

	if (pVerbMenuRec) {		// initialize
		pVerbMenuRec->ConvertMenuID 	= 0;
		pVerbMenuRec->ConvertMenuItem 	= 0;
		pVerbMenuRec->verbCount	 		= 0;
		
		if (pVerbMenuRec->verbRec) {
			OleStdFree(pVerbMenuRec->verbRec);
			pVerbMenuRec->verbRec = nil;
		}
	}
	
	// delete all items in the verb menu
	cVerbs = CountMItems(hVerbMenu);
	for (; cVerbs > 0; cVerbs--)
		DelMenuItem(hVerbMenu, 1);
		
	cVerbs = 0;
	
	sprintf(szBuffer, szNoObjectCmd);

    if (!lpOleObj)
        goto AVMError;

    if (! lpszShortTypeName) {
        // get the Short form of the user type name for the menu
        hrErr = lpOleObj->lpVtbl->GetUserType(
                lpOleObj,
                USERCLASSTYPE_SHORT,
                (char* *)&lpszShortTypeName
        );
        if (hrErr != NOERROR)
        	goto AVMError;
    }

    // check if the object is a link (it is a link if it support IOleLink)
    hrErr = lpOleObj->lpVtbl->QueryInterface(
            lpOleObj,
            &IID_IOleLink,
            (void* *)&lpUnk
    );
    if (NOERROR == hrErr) {
        fIsLink = TRUE;
        OleStdRelease(lpUnk);
    }

    // Get the verb enumerator from the OLE object
    hrErr = lpOleObj->lpVtbl->EnumVerbs(
            lpOleObj,
            (LPENUMOLEVERB *)&lpEnumOleVerb
    );

    // count the verb total
    while (lpEnumOleVerb != NULL) {         // forever
        hrErr = lpEnumOleVerb->lpVtbl->Next(
                lpEnumOleVerb,
                1,
                (LPOLEVERB)&oleverb,
                NULL
        );
        if (NOERROR != hrErr)
            break;              // DONE! no more verbs

        /* OLE2NOTE: only positive verb numbers and verbs that
        **    indicate ONCONTAINERMENU should be put on the verb menu
        */
        if (oleverb.lVerb >= 0 && (oleverb.grfAttribs & OLEVERBATTRIB_ONCONTAINERMENU))
	        cVerbs++;
    }

    // Reset the verb enumerator
	if (lpEnumOleVerb) {
	    hrErr = lpEnumOleVerb->lpVtbl->Reset(lpEnumOleVerb);
		if (hrErr != NOERROR)
			goto AVMError;
	    pVerbMenuRec->verbRec = OleStdMalloc(sizeof(VerbRec) * cVerbs);
	    if (!pVerbMenuRec->verbRec)
	    	goto AVMError;
	}
        	
    cVerbs = 0;
    // loop through all verbs
    while (lpEnumOleVerb != NULL) {         // forever
        hrErr = lpEnumOleVerb->lpVtbl->Next(
                lpEnumOleVerb,
                1,
                (LPOLEVERB)&oleverb,
                NULL
        );
        if (NOERROR != hrErr)
            break;              // DONE! no more verbs

        /* OLE2NOTE: only positive verb numbers and verbs that
        **    indicate ONCONTAINERMENU should be put on the verb menu
        */
        if (oleverb.lVerb >= 0 && (oleverb.grfAttribs & OLEVERBATTRIB_ONCONTAINERMENU)) {
			if (lpszVerbName)
				OleStdFreeString(lpszVerbName, NULL);
				
			lpszVerbName = oleverb.lpszVerbName;
			AppendMenu(hVerbMenu, (StringPtr)c2pstr(lpszVerbName));
			p2cstr((StringPtr)lpszVerbName);
	
			pVerbMenuRec->verbRec[cVerbs].verb 		= oleverb.lVerb;
			pVerbMenuRec->verbRec[cVerbs].menuID 	= idVerbMenu;
			pVerbMenuRec->verbRec[cVerbs].menuItem 	= cVerbs + 1;
			
			cVerbs++;
			EnableItem(hVerbMenu, (short)cVerbs);
		}
		else {
            /* OLE2NOTE: we must still free the verb name string */
            if (oleverb.lpszVerbName)
                OleStdFreeString(oleverb.lpszVerbName, NULL);
            continue;
        }
    }

	pVerbMenuRec->verbCount = cVerbs;
	
    // Add the separator and "Convert" menu item.
    if (bAddConvert) {

		if (lpszVerbName)
			OleStdFreeString(lpszVerbName, NULL);
			
		lpszVerbName = OleStdCopyString(szConvert, NULL);

        if (0 == cVerbs) {
            char* lpsz;

            // if object has no verbs, then use "Convert" as the obj's verb
            lpsz = lpszVerbName;

            // remove "..." from "Convert..." string; it will be added later
            if (lpsz) {
                while(*lpsz && *lpsz != '.')
                    lpsz++;
                *lpsz = '\0';
            }

            pVerbMenuRec->ConvertMenuID = idVerbMenu;
            pVerbMenuRec->ConvertMenuItem = ++cVerbs;
        }
        else {
			AppendMenu(hVerbMenu, "\p(-");

			pVerbMenuRec->ConvertMenuID = idVerbMenu;
			pVerbMenuRec->ConvertMenuItem = ++cVerbs + 1;	// skip the separator
        }

        /* add convert menu */
		AppendMenu(hVerbMenu, c2pstr(lpszVerbName));
		p2cstr((StringPtr)lpszVerbName);
		
		EnableItem(hVerbMenu, pVerbMenuRec->ConvertMenuItem);

    }


    /*
     * Build the appropriate menu based on the number of verbs found
     *
     * NOTE:  Localized verb menus may require a different format.
     *        to assist in localization of the single verb case, the
     *        szLinkCmd1Verb and szObjectCmd1Verb format strings start
     *        with either a '0' (note: NOT '\0'!) or a '1':
     *           leading '0' -- verb type
     *           leading '1' -- type verb
     */
    if (cVerbs == 0) {

        // there are NO verbs (not even Convert...). set the menu to be
        // "<short type> Object/Link" and gray it out.
        sprintf(
            szBuffer,
            (fIsLink ? (char*)szLinkCmdNVerb : (char*)szObjectCmdNVerb),
            (lpszShortTypeName ? lpszShortTypeName : (char*)"")
        );
    }
    else if (cVerbs == 1) {

        // One verb
        char* lpsz = (fIsLink ? szLinkCmd1Verb : szObjectCmd1Verb);

        if (*lpsz == '0') {
            sprintf(szBuffer, lpsz+1,
            	lpszVerbName,
                (lpszShortTypeName ? lpszShortTypeName : (char*)"")
            );
        }
        else {
            sprintf(szBuffer, lpsz+1,
                (lpszShortTypeName ? lpszShortTypeName : (char*)""),
                lpszVerbName
            );
        }

        // if only "verb" is "Convert..." then append the ellipses
        if (bAddConvert) {
            pVerbMenuRec->ConvertMenuID = idEditMenu;
            pVerbMenuRec->ConvertMenuItem = uVerbPos;
			strcat(szBuffer, "...");
		}
		else {
			pVerbMenuRec->verbRec[0].menuID 	= idEditMenu;
			pVerbMenuRec->verbRec[0].menuItem 	= uVerbPos;
		}

		fResult = true;
    }
    else {		// cVerbs > 1

        // Multiple verbs, add the cascading menu
        sprintf(
            szBuffer,
            (fIsLink ? (char*)szLinkCmdNVerb:(char*)szObjectCmdNVerb),
            (lpszShortTypeName ? lpszShortTypeName : (char*)"")
        );

		fResult = true;
    }

AVMError:

	SetItem(hEditMenu, (short)uVerbPos, (StringPtr)c2pstr(szBuffer));
	p2cstr((StringPtr)szBuffer);
	
	SetItemCmd(hEditMenu, (short)uVerbPos, (short)(cVerbs > 1 ? hMenuCmd : 0));
	SetItemMark(hEditMenu, (short)uVerbPos, (short)(cVerbs > 1 ? idVerbMenu : noMark));
	
	if (cVerbs)
        EnableItem(hEditMenu, (short)uVerbPos);
	else
        DisableItem(hEditMenu, (short)uVerbPos);

    if (lpszVerbName)
        OleStdFreeString(lpszVerbName, NULL);
    if (!lpszShortType && lpszShortTypeName)
        OleStdFreeString(lpszShortTypeName, NULL);
    if (lpEnumOleVerb)
        lpEnumOleVerb->lpVtbl->Release(lpEnumOleVerb);

    return fResult;
}

/* OleUIFindVerbConvert
 * --------------------
 *
 *  Purpose:
 *      Look up the verb menu record and for the menuID and menuItem and find the appropriate
 *		verb or convert
 *
 *  Parameters:
 *      pVerbMenuRec		pointer to the Verb Menu Record
 *      menuID				selected menu ID
 *      menuItem			selected menu Item
 *		pConvert			pointer to flag whether Convert is found
 *      pVerb				pointer to the verb found
 *
 *  Returns:
 *		true	if an OLE verb or Convert is found
 *		false	no matching
 *
 */

#ifndef _MSC_VER
#pragma segment Ole2UISeg
#else
#pragma code_seg("Ole2UISeg", "SWAPPABLE")
#endif
STDAPI_(Boolean) OleUIFindVerbConvert(
		VerbMenuRec* 	pVerbMenuRec,
		short 			menuID,
		short 			menuItem,
		Boolean*		pConvert,
		long* 			pVerb
)
{
	short count;
	
	if (!pVerbMenuRec || !pConvert || !pVerb)
		return false;
	
	if (pVerbMenuRec->ConvertMenuID == menuID && pVerbMenuRec->ConvertMenuItem == menuItem) {
		*pConvert = true;
		return true;
	}
	
	for (count = 0; count < pVerbMenuRec->verbCount; count++) {
		if (pVerbMenuRec->verbRec[count].menuID == menuID &&
			pVerbMenuRec->verbRec[count].menuItem == menuItem) {
			*pConvert = false;
			*pVerb = pVerbMenuRec->verbRec[count].verb;
			return true;
		}
	}
	
	return false;
}


/* OleUIPromptUser
 * ---------------
 *
 *  Purpose:
 *      Popup a dialog box with the specified template and returned the
 *      response (button id) from the user. PU_BTN_FIRST is assumed to be the
 *		default button when <Return> or <Enter> is pressed.
 *
 *  Parameters:
 *      nTemplate       resource number of the dialog
 *      ...             title of the dialog box followed by argument list
 *                      for the format string in the static control
 *                      (ID_PU_TEXT) of the dialog box.
 *                      The caller has to make sure that the correct number
 *                      and type of argument are passed in.
 *
 *  Returns:
 *      button id selected by the user (template dependent)
 *
 *  Comments:
 *      the following message dialog boxes are supported:
 *
 *      DID_LINKSOURCEUNAVAILABLE -- Link source is unavailable
 *          VARARG Parameters:
 *              None.
 *          Used for the following error codes:
 *              OLE_E_CANT_BINDTOSOURCE
 *              STG_E_PATHNOTFOUND
 *              (sc >= MK_E_FIRST) && (sc <= MK_E_LAST) -- any Moniker error
 *              any unknown error if object is a link
 *
 *      DID_SERVERNOTFOUND -- server registered but NOT found
 *          VARARG Parameters:
 *              char* lpszUserType -- user type name of object
 *          Used for the following error codes:
 *              CO_E_APPNOTFOUND
 *              CO_E_APPDIDNTREG
 *              any unknown error if object is an embedded object
 *
 *      DID_SERVERNOTREG -- server NOT registered
 *          VARARG Parameters:
 *              char* lpszUserType -- user type name of object
 *          Used for the following error codes:
 *              REGDB_E_CLASSNOTREG
 *              OLE_E_STATIC -- static object with no server registered
 *
 *      DID_LINKTYPECHANGED -- class of link source changed since last binding
 *          VARARG Parameters:
 *              char* lpszUserType -- user type name of ole link source
 *          Used for the following error codes:
 *              OLE_E_CLASSDIFF
 *
 *      DID_LINKTYPECHANGED -- class of link source changed since last binding
 *          VARARG Parameters:
 *              char* lpszUserType -- user type name of ole link source
 *          Used for the following error codes:
 *              OLE_E_CLASSDIFF
 *
 *      DID_OUTOFMEMORY -- out of memory
 *          VARARG Parameters:
 *              None.
 *          Used for the following error codes:
 *              E_OUTOFMEMORY
 *
 *		DID_INVALIDSOURCE -- invalid link source specified
 *          VARARG Parameters:
 *              None.
 *          Used in the links dialog when the user change source to an invalid link source
 */

#ifndef _MSC_VER
#pragma segment Ole2UISeg
#else
#pragma code_seg("Ole2UISeg", "SWAPPABLE")
#endif
#ifdef _MSC_VER
short _cdecl OleUIPromptUser(int nTemplate, ...)
#else
short OleUIPromptUser(int nTemplate, ...)
#endif
{
    va_list     	argptr;
    GrafPtr			gpSave;
	Point			pt;
    POLEUISTANDARD	pUI;
    DialogPtr		pDialog;
    short			itemHit;
	unsigned int	uRet;
	
    va_start(argptr, nTemplate);

	GetPort(&gpSave);
	
    pt.h = pt.v = 0;

    pUI = (POLEUISTANDARD)OleStdMalloc(sizeof(OLEUISTANDARD));

    pUI->cbStruct		= sizeof(OLEUISTANDARD);		//Structure Size
    pUI->dwFlags		= 0;			//IN-OUT:  Flags
    pUI->lpszCaption	= nil;			//Dialog caption bar contents
    pUI->ptPosition		= pt;				//Dialog position, {0,0} = center
    pUI->lpfnHook		= nil;				//Hook callback
    pUI->lCustData		= 0;				//Custom data to pass to hook
	
	//Validate pEL structure
	uRet = UStandardValidation(pUI, sizeof(OLEUISTANDARD));

	if (uRet != OLEUI_SUCCESS) {
		SetPort(gpSave);	
		return uRet;
	}

	pDialog = NULL;
	uRet = UStandardInvocation(pUI, &pDialog, nTemplate);

	uRet = PromptUserInit(pDialog, argptr, pUI);

	if (uRet != OLEUI_SUCCESS)
	{
		PromptUserCleanup(pDialog);
		SetPort(gpSave);
		return OLEUI_FALSE;
	}

   SetCursor(&qd.arrow);

	ShowWindow((WindowPtr)pDialog);

#ifdef __powerc
	ModalDialog(gPromptUserDialogProc, &itemHit);
#else

#ifdef UIDLL
   {
      short hostResNum = SetUpOLEUIResFile();
	   ModalDialog(PromptUserDialogProc, &itemHit);
      ClearOLEUIResFile(hostResNum);
   }
#else
   ModalDialog(PromptUserDialogProc, &itemHit);
#endif

#endif


    va_end(argptr);

	PromptUserCleanup(pDialog);

	/* map the item hit to the corresponding return value based on the dialog used */
	switch (nTemplate)
   {
		case IDD_INVALIDSOURCE:
			switch (itemHit)
         {
				case PU_BTN_FIRST:
					itemHit = PU_BTN_YES;
					break;
				case PU_BTN_FIRST + 1:
					itemHit = PU_BTN_NO;
					break;
			}
			break;
			
		case IDD_LINKSOURCEUNAVAILABLE:
			break;
			
		case IDD_SERVERNOTREG:
			break;
			
		case IDD_LINKTYPECHANGED:
			break;
			
		case IDD_SERVERNOTFOUND:
			break;
			
		case IDD_OUTOFMEMORY:
			break;
	}			

	SetPort(gpSave);
    return itemHit;
}


/* PromptUserDialogProc
 * --------------------
 *
 *  Purpose:
 *      Dialog procedure used by OleUIPromptUser(). Returns when a button is
 *      clicked in the dialog box and the button id is return. The button with ID = 2
 *		is assumed to be the default button for the dialog which has an outline drawn and
 *		is returned by <Return> or <Enter>.
 *
 *  Parameters:
 *
 *  Returns:
 *
 */

#ifndef _MSC_VER
#pragma segment Ole2UISeg
static pascal Boolean
#else
#pragma code_seg("Ole2UISeg", "SWAPPABLE")
static Boolean __pascal
#endif
PromptUserDialogProc(DialogPtr pDialog, EventRecord *pEvent, short *nItem)
{
    POLEUISTANDARD 	pUI = (POLEUISTANDARD)GetWRefCon(pDialog);
	GrafPtr			gpSave;
	Boolean			fHooked;
	short			iType;
	Handle			iHandle;
	Rect			iRect;
	char			chKey;
#ifdef UIDLL
   void*       oldQD = SetLpqdFromA5();
#endif
	
	GetPort(&gpSave);
	SetPort(pDialog);

	fHooked = FStandardHook(pUI, pDialog, pEvent, nItem, pUI->lCustData);
	if (fHooked)
	{
		SetPort(gpSave);
#ifdef UIDLL
      RestoreLpqd(oldQD);
#endif
		return true;
	}

	switch (pEvent->what) {
		case updateEvt:
			{
				WindowPtr		whichWindow;
	
				whichWindow = (WindowPtr)pEvent->message;
				if (whichWindow == pDialog)
				{
					PenState	penState;
					
					GetPenState(&penState);
				  	PenSize(3,3);
					
					GetDItem(pDialog, PU_BTN_FIRST, &iType, &iHandle, &iRect);
					InsetRect(&iRect, -4, -4);
				  	FrameRoundRect(&iRect, 16, 16);

					SetPenState(&penState);
				}
			}
			break;	

		case keyDown:
		case autoKey:
			chKey = pEvent->message & charCodeMask;
			switch (chKey)
			{
				case ENTERKEY:
				case RETURNKEY:
					FlashButton(pDialog, PU_BTN_FIRST);
					*nItem = PU_BTN_FIRST;
					SetPort(gpSave);
					return true;

				case ESCAPEKEY:
					break;
			}
			break;
    }

	SetPort(gpSave);
#ifdef UIDLL
      RestoreLpqd(oldQD);
#endif
    return false;
}

/* PromptUserInit
 * --------------
 *
 *  Purpose:
 *      Initialize the messaget text of the dialog based on the parameters passed in
 *
 *  Parameters:
 *      hDlg
 *      iMsg
 *      wParam
 *      lParam
 *
 *  Returns:
 *
 */

#ifndef _MSC_VER
#pragma segment Ole2UISeg
#else
#pragma code_seg("Ole2UISeg", "SWAPPABLE")
#endif
static unsigned int PromptUserInit(DialogPtr pDialog, va_list argptr, POLEUISTANDARD pUI)
{
	short			iType;
	Handle			iHandle;
	Rect			iRect;
	Str255			FormatStr;
	char			szBuf[256];

	GetDItem(pDialog, PU_TEXT_MESSAGE, &iType, &iHandle, &iRect);
	GetIText(iHandle, FormatStr);
	p2cstr(FormatStr);
	vsprintf(szBuf, (char*)FormatStr, argptr);
	c2pstr(szBuf);
	SetIText(iHandle, (StringPtr)szBuf);
	
	SetWRefCon(pDialog, (long)pUI);
	
	return OLEUI_SUCCESS;
}


/*
 * PromptUserCleanup
 *
 * Purpose:
 *  Handles any final cleanup necessary for PromptUser dialog.
 *
 * Parameters:
 *  pDialog			DialogPtr to the dialog box.
 *
 * Return Value:
 *  None
 */

#ifndef _MSC_VER
#pragma segment Ole2UISeg
#else
#pragma code_seg("Ole2UISeg", "SWAPPABLE")
#endif
static void PromptUserCleanup(DialogPtr pDialog)
{
	POLEUISTANDARD 	pUI;

	pUI = (POLEUISTANDARD)GetWRefCon(pDialog);

	if (pUI)
	{
		OleStdFree(pUI);
	}

	DisposDialog(pDialog);
}


/* OleUIUpdateLinks
 * ----------------
 *
 *  Purpose:
 *      Update all links in the Link Container and popup a dialog box which
 *      shows the progress of the updating.
 *      The process is stopped when the user press Stop button or when all
 *      links are processed.
 *
 *  Parameters:
 *      lpOleUILinkCntr         pointer to Link Container
 *      hwndParent              parent window of the dialog
 *      lpszTitle               title of the dialog box
 *      cLinks                  total number of links
 *
 *  Returns:
 *      TRUE                    all links updated successfully
 *      FALSE                   otherwise
 */

#ifndef _MSC_VER
#pragma segment Ole2UISeg
#else
#pragma code_seg("Ole2UISeg", "SWAPPABLE")
#endif
STDAPI_(Boolean) OleUIUpdateLinks(LPOLEUILINKCONTAINER lpOleUILinkCntr, int cLinks)
{
    PUPDATELINKS	pUL = nil;
    Boolean			fError;
    short			itemHit;
    POLEUISTANDARD	pUI;
    DialogPtr		pDialog;
	Point			pt;
	unsigned long	uRet;
	GrafPtr			gpSave;
	
	GetPort(&gpSave);
	
    OleDbgAssert(lpOleUILinkCntr && (cLinks>0));
    pt.h = pt.v = 0;

    pUI = (POLEUISTANDARD)OleStdMalloc(sizeof(OLEUISTANDARD));

    pUI->cbStruct		= sizeof(OLEUISTANDARD);		//Structure Size
    pUI->dwFlags		= 0;			//IN-OUT:  Flags
    pUI->lpszCaption	= nil;			//Dialog caption bar contents
    pUI->ptPosition		= pt;				//Dialog position, {0,0} = center
    pUI->lpfnHook		= nil;				//Hook callback
    pUI->lCustData		= 0;				//Custom data to pass to hook
	
	//Validate pEL structure
	uRet = UStandardValidation(pUI, sizeof(OLEUISTANDARD));

	if (uRet != OLEUI_SUCCESS) {
		SetPort(gpSave);	
		return uRet;
	}

	pDialog = NULL;
	uRet = UStandardInvocation(pUI, &pDialog, DIDUpdateLinks);

	uRet = UpdateLinksInit(pDialog, pUI);

	if (uRet != OLEUI_SUCCESS)
	{
		UpdateLinksCleanup(pDialog);
		SetPort(gpSave);
		return false;
	}

	pUL = (PUPDATELINKS)GetWRefCon(pDialog);

    pUL->lpOleUILinkCntr 	= lpOleUILinkCntr;
    pUL->cLinks           	= cLinks;
    pUL->cUpdated         	= 0;
    pUL->dwLink           	= 0;
    pUL->fError           	= FALSE;
    pUL->lpszTitle    		= nil;

	// count the number of links
	do {
		pUL->dwLink = pUL->lpOleUILinkCntr->lpVtbl->GetNextLink(pUL->lpOleUILinkCntr, pUL->dwLink);
		if (pUL->dwLink)
			pUL->cLinks++;
	} while (pUL->dwLink);

	ShowWindow((WindowPtr)pDialog);

#ifndef __powerc
	ModalDialog(UpdateLinksDialogProc, &itemHit);
#else

#ifdef UIDLL
   {
      short hostResNum = SetUpOLEUIResFile();
      ModalDialog( gUpdateLinksDialogProc, &itemHit);
      ClearOLEUIResFile(hostResNum);
   }
#else
	ModalDialog( gUpdateLinksDialogProc, &itemHit);
#endif

#endif

    fError = pUL->fError;

	UpdateLinksCleanup(pDialog);
	
	SetPort(gpSave);
    return !fError;
}

/* UpdateLinksDialogProc
 * ---------------------
 *
 *  Purpose:
 *      Dialog procedure used by OleUIUpdateLinks(). It will enumerate all
 *      all links in the container and updates all automatic links.
 *      Returns when the Stop Button is clicked in the dialog box or when all
 *      links are updated
 *
 *  Parameters:
 *      hDlg
 *      iMsg
 *      wParam
 *      lParam          pointer to the UPDATELINKS structure
 *
 *  Returns:
 *
 */

#ifndef _MSC_VER
#pragma segment Ole2UISeg
static pascal Boolean
#else
#pragma code_seg("Ole2UISeg", "SWAPPABLE")
static Boolean __pascal
#endif
UpdateLinksDialogProc(DialogPtr pDialog, EventRecord *pEvent, short *nItem)
{
    PUPDATELINKS	pUL = (LPUPDATELINKS)GetWRefCon(pDialog);
	GrafPtr			gpSave;
	Boolean			fHooked;
	short			iType;
	Handle			iHandle;
	Rect			iRect;
	char			chKey;
#ifdef UIDLL
   void*       oldQD = SetLpqdFromA5();
#endif
	
	GetPort(&gpSave);
	SetPort(pDialog);

	fHooked = FStandardHook((LPOLEUISTANDARD)pUL->pUI, pDialog, pEvent, nItem, pUL->pUI->lCustData);
	if (fHooked)
	{
		SetPort(gpSave);
#ifdef UIDLL
      RestoreLpqd(oldQD);
#endif
		return true;
	}

	switch (pEvent->what) {
		case updateEvt:
			{
				WindowPtr		whichWindow;
	
				whichWindow = (WindowPtr)pEvent->message;
				if (whichWindow == pDialog)
				{
					PenState	penState;
					
					GetPenState(&penState);
				  	PenSize(3,3);
					
					GetDItem(pDialog, UL_BTN_STOP, &iType, &iHandle, &iRect);
					InsetRect(&iRect, -4, -4);
				  	FrameRoundRect(&iRect, 16, 16);

					SetPenState(&penState);
					
					// draw outline of indicator
					GetDItem(pDialog, UL_UITEM_METER, &iType, &iHandle, &iRect);
					FrameRect(&iRect);
				}
			}
			break;	

		case nullEvent:
			{
				HRESULT         hErr;
				long			nPercent;
				char            szPercent[5];       // 0% to 100%
				unsigned long   dwUpdateOpt;
		
				pUL->dwLink = pUL->lpOleUILinkCntr->lpVtbl->GetNextLink(pUL->lpOleUILinkCntr, pUL->dwLink);
		
				if (!pUL->dwLink) {        // all links processed
					*nItem = UL_BTN_STOP;
					SetPort(gpSave);
#ifdef UIDLL
               RestoreLpqd(oldQD);
#endif
					return true;
				}
		
				hErr = pUL->lpOleUILinkCntr->lpVtbl->GetLinkUpdateOptions(pUL->lpOleUILinkCntr,	pUL->dwLink, &dwUpdateOpt);
		
				if ((hErr == NOERROR) && (dwUpdateOpt == OLEUPDATE_ALWAYS)) {
		
					hErr = pUL->lpOleUILinkCntr->lpVtbl->UpdateLink(
								pUL->lpOleUILinkCntr,
								pUL->dwLink,
								FALSE,      // fMessage
								FALSE       // ignored
					);
					pUL->fError |= (hErr != NOERROR);
					pUL->cUpdated++;
		
					nPercent = pUL->cUpdated * 100 / pUL->cLinks;
					if (nPercent <= 100) {  // do NOT advance % beyond 100%
						// update percentage
						sprintf((char*)szPercent, "%ld%%", nPercent);
						GetDItem(pDialog, UL_TEXT_PERCENT, &iType, &iHandle, &iRect);
						SetIText(iHandle, (StringPtr)c2pstr(szPercent));
							
						// update indicator
						GetDItem(pDialog, UL_UITEM_METER, &iType, &iHandle, &iRect);
						iRect.right = (iRect.right - iRect.left) * nPercent / 100 + iRect.left;
						InsetRect(&iRect, 1, 1);
						FillRect(&iRect, (ConstPatternParam)&qd.gray);
					}
				}
				
			}
			break;
			
		case keyDown:
		case autoKey:
			chKey = pEvent->message & charCodeMask;
			switch (chKey)
			{
				case ENTERKEY:
				case RETURNKEY:
				case ESCAPEKEY:
					FlashButton(pDialog, UL_BTN_STOP);
					*nItem = UL_BTN_STOP;
					SetPort(gpSave);
#ifdef UIDLL
               RestoreLpqd(oldQD);
#endif

					return true;
			}
			break;
    }

	SetPort(gpSave);
#ifdef UIDLL
      RestoreLpqd(oldQD);
#endif
	
    return false;
}


/*
 * UpdateLinksInit
 *
 * Purpose:
 *
 * Parameters:
 *
 * Return Value:
 */

#ifndef _MSC_VER
#pragma segment Ole2UISeg
#else
#pragma code_seg("Ole2UISeg", "SWAPPABLE")
#endif
static unsigned int UpdateLinksInit(DialogPtr pDialog, POLEUISTANDARD pUI)
{
	PUPDATELINKS	pUL;

	//1. Copy the structure at lpOEL
	pUL = (PUPDATELINKS)PvStandardInit(pDialog, sizeof(UPDATELINKS));

	//PvStandardInit failed
	if (pUL == NULL)
		return OLEUI_ERR_GLOBALMEMALLOC;

	//3. Save the original pointer.
	pUL->pUI = pUI;

	return OLEUI_SUCCESS;
}

/*
 * UpdateLinksCleanup
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
#pragma segment Ole2UISeg
#else
#pragma code_seg("Ole2UISeg", "SWAPPABLE")
#endif
static void UpdateLinksCleanup(DialogPtr pDialog)
{
	LPUPDATELINKS		pUL;

	pUL = (LPUPDATELINKS)GetWRefCon(pDialog);

	if (pUL)
	{
		if (pUL->pUI)
			OleStdFree(pUL->pUI);
			
		DisposePtr((Ptr)pUL);
	}

	DisposDialog(pDialog);
}
