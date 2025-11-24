/*
 * OLE2UI.C
 *
 * Contains initialization routines and miscellaneous API implementations for 
 * the OLE 2.0 User Interface Support Library.
 *
 * Copyright (c)1992 Microsoft Corporation, All Right Reserved
 */

#define STRICT  1

#include "ole2ui.h"
#include "common.h"
#include "utility.h"
#include "resimage.h"
#include "iconbox.h"
#include <commdlg.h>

#define WINDLL  1           // make far pointer version of stdargs.h
#include <stdarg.h>


OLEDBGDATA_MAIN("OLE2UI")

//The DLL instance handle shared amongst all dialogs.
HINSTANCE     ghInst;

//Registered messages for use with all the dialogs, registered in LibMain
UINT        uMsgHelp=0;
UINT        uMsgEndDialog=0;
UINT        uMsgBrowse=0;
UINT        uMsgChangeIcon=0;
UINT        uMsgFileOKString=0;
UINT        uMsgCloseBusyDlg=0;

//Clipboard formats used by PasteSpecial
UINT  cfObjectDescriptor;
UINT  cfLinkSrcDescriptor;
UINT  cfEmbedSource;
UINT  cfEmbeddedObject;
UINT  cfLinkSource;
UINT  cfOwnerLink;
UINT  cfFileName;

// local function prototypes
BOOL CALLBACK EXPORT PromptUserDlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK EXPORT UpdateLinksDlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);


// local definition
#define WM_U_UPDATELINK WM_USER


// local structure definition
typedef struct tagUPDATELINKS 
{
    LPOLEUILINKCONTAINER    lpOleUILinkCntr;    // pointer to Link Container
    UINT                    cLinks;             // total number of links
    UINT                    cUpdated;           // number of links updated
    DWORD                   dwLink;             // pointer to link
    BOOL                    fError;             // error flag
    LPSTR                   lpszTitle;          // title of the dialog box
} UPDATELINKS, *PUPDATELINKS, FAR *LPUPDATELINKS;



STDAPI_(BOOL) OleUIInitialize(HINSTANCE hInstance)
{
    HRSRC   hr;
    HGLOBAL hg;
    LPWORD lpdata;

    OleDbgOut1("OleUIInitialize called.\r\n");
    ghInst=hInstance;

    // Verify that we have the correct resources added to our application.
    if ((hr = FindResource(hInstance, "VERIFICATION", RT_RCDATA)) == NULL)
        goto ResourceLoadError;

    if ((hg = LoadResource(hInstance, hr)) == NULL)
        goto ResourceLoadError;

    if ((lpdata = (LPWORD)LockResource(hg)) == NULL)
        goto ResourceLockError;

    if ((WORD)*lpdata != (WORD)OLEUI_VERSION_MAGIC)
        goto ResourceReadError;

    // OK, resource versions match.  Contine on.
    UnlockResource(hg);
    FreeResource(hg);
    OleDbgOut1("OleUIInitialize: Resource magic number verified.\r\n");

    // Register messages we need for the dialogs.  If
    uMsgHelp      =RegisterWindowMessage(SZOLEUI_MSG_HELP);
    uMsgEndDialog =RegisterWindowMessage(SZOLEUI_MSG_ENDDIALOG);
    uMsgBrowse    =RegisterWindowMessage(SZOLEUI_MSG_BROWSE);
    uMsgChangeIcon=RegisterWindowMessage(SZOLEUI_MSG_CHANGEICON);
    uMsgFileOKString = RegisterWindowMessage(FILEOKSTRING);   
    uMsgCloseBusyDlg = RegisterWindowMessage(SZOLEUI_MSG_CLOSEBUSYDIALOG);

    // Register Clipboard Formats used by PasteSpecial dialog.
    cfObjectDescriptor = RegisterClipboardFormat(CF_OBJECTDESCRIPTOR);
    cfLinkSrcDescriptor= RegisterClipboardFormat(CF_LINKSRCDESCRIPTOR);
    cfEmbedSource      = RegisterClipboardFormat(CF_EMBEDSOURCE);
    cfEmbeddedObject   = RegisterClipboardFormat(CF_EMBEDDEDOBJECT);
    cfLinkSource       = RegisterClipboardFormat(CF_LINKSOURCE);
    cfOwnerLink        = RegisterClipboardFormat(CF_OWNERLINK);
    cfFileName         = RegisterClipboardFormat(CF_FILENAME);


    if (!FResultImageInitialize(hInstance))
        {
        OleDbgOut1("OleUIInitialize: FResultImageInitialize failed. Terminating.\r\n");
        return 0;
        }

    if (!FIconBoxInitialize(hInstance))
        {
        OleDbgOut1("OleUIInitialize: FIconBoxInitialize failed. Terminating.\r\n");
        return 0;
        }

    return TRUE;

ResourceLoadError:
    OleDbgOut1("OleUIInitialize: ERROR - Unable to find version verification resource.\r\n");
    return FALSE;

ResourceLockError:
    OleDbgOut1("OleUIInitialize: ERROR - Unable to lock version verification resource.\r\n");
    FreeResource(hg);
    return FALSE;

ResourceReadError:
    OleDbgOut1("OleUIInitialize: ERROR - Version verification values did not compare.\r\n");

    {char buf[255];
    wsprintf(buf, "resource read 0x%X, my value is 0x%X\n", (WORD)*lpdata, (WORD)OLEUI_VERSION_MAGIC);
    OutputDebugString(buf);
    }

    UnlockResource(hg);
    FreeResource(hg);
    return FALSE;
}


STDAPI_(BOOL) OleUIUnInitialize()
{
    IconBoxUninitialize();
    ResultImageUninitialize();

    return TRUE;
}



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
 *  lpszShortType   LPSTR with short type name (AuxName==2) corresponding
 *                  to the lpOleObj. if the string is NOT known, then NULL
 *                  may be passed. if NULL is passed, then 
 *                  IOleObject::GetUserType will be called to retrieve it.
 *                  if the caller has the string handy, then it is faster 
 *                  to pass it in.
 *
 *  hMenu           HMENU in which to make modifications.
 *
 *  uPos            Position of the menu item
 *
 *  idVerbMin       UINT ID value at which to start the verbs.
 *                      verb_0 = wIDMVerbMin + verb_0
 *                      verb_1 = wIDMVerbMin + verb_1
 *                      verb_2 = wIDMVerbMin + verb_2
 *                      etc.
 *
 *  bAddConvert     BOOL specifying whether or not to add a "Convert" item
 *                  to the bottom of the menu (with a separator).
 *
 *  idConvert       UINT ID value to use for the Convert menu item, if
 *                  bAddConvert is TRUE.
 *
 *  lphMenu         HMENU FAR * of the cascading verb menu if it's created.
 *                  If there is only one verb, this will be filled with NULL.
 *
 *
 * Return Value:
 *  BOOL            TRUE if lpObj was valid and we added at least one verb
 *                  to the menu.  FALSE if lpObj was NULL and we created
 *                  a disabled default menu item
 */

STDAPI_(BOOL) OleUIAddVerbMenu(LPOLEOBJECT lpOleObj,
                             LPSTR lpszShortType,
                             HMENU hMenu,
                             UINT uPos,
                             UINT uIDVerbMin,
                             BOOL bAddConvert,
                             UINT idConvert,
                             HMENU FAR *lphMenu)
{
    LPPERSISTSTORAGE    lpPS=NULL;
    LPENUMOLEVERB       lpEnumOleVerb = NULL;
    OLEVERB             oleverb;
    LPUNKNOWN           lpUnk;
    LPSTR               lpszShortTypeName = lpszShortType;
    LPSTR               lpszVerbName = NULL;
    HRESULT             hrErr;
    BOOL                fStatus;
    BOOL                fIsLink = FALSE;
    BOOL                fResult = FALSE;
    BOOL                fAddConvertItem = FALSE;
    LONG                lNextVerbExpected;
    int                 cVerbs = 0;
    UINT                uFlags = MF_BYPOSITION;
    static BOOL         fFirstTime = TRUE;
    static char         szBuffer[OLEUI_OBJECTMENUMAX];
    static char         szNoObjectCmd[OLEUI_OBJECTMENUMAX];
    static char         szObjectCmd1Verb[OLEUI_OBJECTMENUMAX];
    static char         szLinkCmd1Verb[OLEUI_OBJECTMENUMAX];
    static char         szObjectCmdNVerb[OLEUI_OBJECTMENUMAX];
    static char         szLinkCmdNVerb[OLEUI_OBJECTMENUMAX];
    static char         szUnknown[OLEUI_OBJECTMENUMAX];
    static char         szEdit[OLEUI_OBJECTMENUMAX];
    static char         szConvert[OLEUI_OBJECTMENUMAX];

    *lphMenu=NULL;

    // Set fAddConvertItem flag
    if (bAddConvert & (idConvert != 0))
       fAddConvertItem = TRUE;

    // only need to load the strings the 1st time
    if (fFirstTime) {
        if (0 == LoadString(ghInst, IDS_OLE2UIEDITNOOBJCMD, 
                 (LPSTR)szNoObjectCmd, OLEUI_OBJECTMENUMAX)) 
            return FALSE;
        if (0 == LoadString(ghInst, IDS_OLE2UIEDITLINKCMD_1VERB, 
                 (LPSTR)szLinkCmd1Verb, OLEUI_OBJECTMENUMAX))
            return FALSE;
        if (0 == LoadString(ghInst, IDS_OLE2UIEDITOBJECTCMD_1VERB, 
                 (LPSTR)szObjectCmd1Verb, OLEUI_OBJECTMENUMAX))
            return FALSE;

        if (0 == LoadString(ghInst, IDS_OLE2UIEDITLINKCMD_NVERB, 
                 (LPSTR)szLinkCmdNVerb, OLEUI_OBJECTMENUMAX))
            return FALSE;
        if (0 == LoadString(ghInst, IDS_OLE2UIEDITOBJECTCMD_NVERB, 
                 (LPSTR)szObjectCmdNVerb, OLEUI_OBJECTMENUMAX))
            return FALSE;

        if (0 == LoadString(ghInst, IDS_OLE2UIUNKNOWN, 
                 (LPSTR)szUnknown, OLEUI_OBJECTMENUMAX))
            return FALSE;

        if (0 == LoadString(ghInst, IDS_OLE2UIEDIT, 
                 (LPSTR)szEdit, OLEUI_OBJECTMENUMAX))
            return FALSE;

        if ( (0 == LoadString(ghInst, IDS_OLE2UICONVERT, 
                   (LPSTR)szConvert, OLEUI_OBJECTMENUMAX)) && fAddConvertItem)
            return FALSE;
        
    }

    // Delete whatever menu may happen to be here already.
    DeleteMenu(hMenu, uPos, uFlags);

    if (!lpOleObj)
        goto AVMError;

    if (! lpszShortTypeName) {
        // get the Short form of the user type name for the menu
        OLEDBG_BEGIN2("IOleObject::GetUserType called\r\n")
        hrErr = lpOleObj->lpVtbl->GetUserType(
                lpOleObj,
                USERCLASSTYPE_SHORT,
                (LPSTR FAR*)&lpszShortTypeName
        );
        OLEDBG_END2

        if (NOERROR != hrErr) {
            OleDbgOutHResult("IOleObject::GetUserType returned", hrErr);
        }
    }

    // check if the object is a link (it is a link if it support IOleLink)
    hrErr = lpOleObj->lpVtbl->QueryInterface(
            lpOleObj,
            &IID_IOleLink,
            (LPVOID FAR*)&lpUnk
    );
    if (NOERROR == hrErr) {
        fIsLink = TRUE;
        OleStdRelease(lpUnk);
    }

    // Get the verb enumerator from the OLE object
    OLEDBG_BEGIN2("IOleObject::EnumVerbs called\r\n")
    hrErr = lpOleObj->lpVtbl->EnumVerbs(
            lpOleObj,
            (LPENUMOLEVERB FAR*)&lpEnumOleVerb
    );
    OLEDBG_END2

    if (NOERROR != hrErr) {
        OleDbgOutHResult("IOleObject::EnumVerbs returned", hrErr);
    }

    if (!(*lphMenu = CreatePopupMenu()))
        goto AVMError;
    
    lNextVerbExpected = 0;  // we expect verbs to start at 0

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

        /* OLE2NOTE: negative verb numbers and verbs that do not
        **    indicate ONCONTAINERMENU should NOT be put on the verb menu
        */
        if (oleverb.lVerb < 0 || 
                ! (oleverb.grfAttribs & OLEVERBATTRIB_ONCONTAINERMENU)) {

            /* OLE2NOTE: we must still free the verb name string */
            if (oleverb.lpszVerbName)
                OleStdFreeString(oleverb.lpszVerbName, NULL);
            continue;           
        }

#if defined( OBSOLETE )
        /* OLE2NOTE: we only support consecutively numbered verbs
        **    starting with verb 0. we will stop adding verbs to the
        **    menu when the first gap in the verb numbers is
        **    encountered. 
        */
        if (oleverb.lVerb != lNextVerbExpected)
            break;              // STOP -- gap in verb numbers encountered

        lNextVerbExpected++;    // next verb should be consecutive
#endif

        // we must free the previous verb name string
        if (lpszVerbName)
            OleStdFreeString(lpszVerbName, NULL);

        lpszVerbName = oleverb.lpszVerbName;

        fStatus = InsertMenu(
                *lphMenu,
                (UINT)-1,
                MF_BYPOSITION | (UINT)oleverb.fuFlags,
                uIDVerbMin+(UINT)oleverb.lVerb,
                (LPSTR)lpszVerbName
        );
        if (! fStatus)
            goto AVMError;

        cVerbs++;
    }


   // Add the separator and "Convert" menu item.  Also add "Edit" item
   // if cVerbs = 0 so that we can just fall into the switch statement
   // below without any more Convert-specific code.

    if (fAddConvertItem) {
       
#if defined( OBSOLETE )
        if (0 == cVerbs) {
            fStatus = InsertMenu(*lphMenu,
                               (UINT)-1,
                               MF_BYPOSITION,
                               uIDVerbMin,
                               (LPCSTR)szEdit);
            if (! fStatus)
                goto AVMError;

            cVerbs++;
        }
#endif

        if (cVerbs > 0) {
            fStatus = InsertMenu(*lphMenu,
                        (UINT)-1,
                        MF_BYPOSITION | MF_SEPARATOR,
                        (UINT)0,
                        (LPCSTR)NULL);
            if (! fStatus)
                goto AVMError;
        }

        /* add convert menu */
        fStatus = InsertMenu(*lphMenu,
                    (UINT)-1,
                    MF_BYPOSITION,
                    idConvert,
                    (LPCSTR)szConvert);
        if (! fStatus)
            goto AVMError;

        cVerbs++;
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
        
        //No verbs.  Create a default using Edit as the verb.
        LPSTR       lpsz = (fIsLink ? szLinkCmd1Verb : szObjectCmd1Verb);

        if (*lpsz == '0') {
            wsprintf(szBuffer, lpsz+1, (LPSTR)szEdit,
                (lpszShortTypeName ? lpszShortTypeName : (LPSTR)"")
            );
        }
        else {
            wsprintf(szBuffer, lpsz+1,
                (lpszShortTypeName ? lpszShortTypeName : (LPSTR)""), 
                (LPSTR)szEdit
            );
        }
        
        DestroyMenu(*lphMenu);
        *lphMenu = NULL;
    
    }
    else if ((cVerbs == 1) && !fAddConvertItem) {
        //One verb without Convert, one item.
        LPSTR       lpsz = (fIsLink ? szLinkCmd1Verb : szObjectCmd1Verb);

        if (*lpsz == '0') {
            wsprintf(szBuffer, lpsz+1, lpszVerbName,
                (lpszShortTypeName ? lpszShortTypeName : (LPSTR)"")
            );
        }
        else {
            wsprintf(szBuffer, lpsz+1,
                (lpszShortTypeName ? lpszShortTypeName : (LPSTR)""), 
                lpszVerbName
            );
        }
        DestroyMenu(*lphMenu);
        *lphMenu=NULL;
    }
    else {

        //Multiple verbs or one verb with Convert, add the cascading menu
        wsprintf(
            szBuffer,
            (fIsLink ? (LPSTR)szLinkCmdNVerb:(LPSTR)szObjectCmdNVerb),
            (lpszShortTypeName ? lpszShortTypeName : (LPSTR)"")
        );
        uFlags |= MF_ENABLED | MF_POPUP;
        uIDVerbMin=(UINT)*lphMenu;
    }

    if (!InsertMenu(hMenu, uPos, uFlags, uIDVerbMin, szBuffer))

AVMError:
        {
            HMENU hmenuDummy = CreatePopupMenu();

            InsertMenu(hMenu, uPos, MF_GRAYED | MF_POPUP | uFlags,
                    (UINT)hmenuDummy, (LPSTR)szNoObjectCmd);
            fResult = FALSE;
            goto done;
        }

    fResult = TRUE;

done:
    if (lpszVerbName)
        OleStdFreeString(lpszVerbName, NULL);
    if (!lpszShortType && lpszShortTypeName)
        OleStdFreeString(lpszShortTypeName, NULL);
    if (lpEnumOleVerb)
        OleStdVerifyRelease(
                (LPUNKNOWN)lpEnumOleVerb, "IEnumOleVerb NOT released\r\n");
    return fResult;
}


/* PromptUserDlgProc
 * -----------------
 *
 *  Purpose:
 *      Dialog procedure used by OleUIPromptUser(). Returns when a button is 
 *      clicked in the dialog box and the button id is return.
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
BOOL CALLBACK EXPORT PromptUserDlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg) {
        case WM_INITDIALOG:
        {
            LPSTR lpszTitle;
            char szBuf[256];
            char szFormat[256];
            va_list argptr;
            
            argptr = (va_list)lParam;
            if (!lParam) {
                EndDialog(hDlg, -1);
                return FALSE;
            }
            
            lpszTitle = va_arg(argptr, LPSTR);
            SetWindowText(hDlg, lpszTitle);

            GetDlgItemText(hDlg, ID_PU_TEXT, (LPSTR)szFormat, 256);
            wvsprintf((LPSTR)szBuf, (LPSTR)szFormat, argptr);
            SetDlgItemText(hDlg, ID_PU_TEXT, (LPSTR)szBuf);
            return TRUE;
        }
        case WM_COMMAND:
            EndDialog(hDlg, wParam);
            return TRUE;
            
        default:
            return FALSE;
    }
}


/* OleUIPromptUser
 * ---------------
 *
 *  Purpose:
 *      Popup a dialog box with the specified template and returned the
 *      response (button id) from the user.
 *
 *  Parameters:
 *      nTemplate       resource number of the dialog
 *      hwndParent      parent of the dialog box
 *      ...             title of the dialog box followed by argument list
 *                      for the format string in the static control 
 *                      (ID_PU_TEXT) of the dialog box.
 *                      The caller has to make sure that the correct number
 *                      and type of argument are passed in.
 *
 *  Returns:
 *      button id selected by the user (template dependent)
 */
int EXPORT FAR CDECL OleUIPromptUser(int nTemplate, HWND hwndParent, ...)
{
    int         nRet;
    
    va_list     argptr;
    
    va_start(argptr, hwndParent);
    nRet = DialogBoxParam(ghInst, MAKEINTRESOURCE(nTemplate), hwndParent, 
            PromptUserDlgProc, (LPARAM)argptr);
    
    va_end(argptr);
    return nRet;
}



/* UpdateLinksDlgProc
 * ------------------
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
BOOL CALLBACK EXPORT UpdateLinksDlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    LPUPDATELINKS   lpUL;
    
    switch (iMsg) {
        case WM_INITDIALOG:
            lpUL = (LPUPDATELINKS)lParam;                       
            SetWindowText(hDlg, lpUL->lpszTitle);
            PostMessage(hDlg, WM_U_UPDATELINK, 0, lParam);
            return TRUE;
    
        case WM_COMMAND:    // Stop button 
            EndDialog(hDlg, 0);
            return TRUE;
            
        case WM_U_UPDATELINK:
        {
            HRESULT         hErr;
            int             nPercent;
            RECT            rc;
            char            szPercent[5];       // 0% to 100%
            HBRUSH          hbr;
            HDC             hDC;
            HWND            hwndMeter;
            MSG             msg;
            DWORD           dwUpdateOpt;

            lpUL = (LPUPDATELINKS)lParam;
            lpUL->dwLink = lpUL->lpOleUILinkCntr->lpVtbl->GetNextLink(
                    lpUL->lpOleUILinkCntr,
                    lpUL->dwLink
            );
            
            if (!lpUL->dwLink) {        // all links processed
                EndDialog(hDlg, 0);
                return TRUE;
            }
            
            hErr = lpUL->lpOleUILinkCntr->lpVtbl->GetLinkUpdateOptions(
                    lpUL->lpOleUILinkCntr,
                    lpUL->dwLink,
                    (LPDWORD)&dwUpdateOpt
            );
        
            if ((hErr == NOERROR) && (dwUpdateOpt == OLEUPDATE_ALWAYS)) {

                hErr = lpUL->lpOleUILinkCntr->lpVtbl->UpdateLink(
                        lpUL->lpOleUILinkCntr,
                        lpUL->dwLink,
                        FALSE,      // fMessage
                        FALSE       // ignored
                );
                lpUL->fError |= (hErr != NOERROR);
                lpUL->cUpdated++;

                // update percentage
                nPercent = lpUL->cUpdated * 100 / lpUL->cLinks;
                wsprintf((LPSTR)szPercent, "%d%%", nPercent);
                SetDlgItemText(hDlg, ID_PU_PERCENT, (LPSTR)szPercent);

                // update indicator
                hwndMeter = GetDlgItem(hDlg, ID_PU_METER);
                GetClientRect(hwndMeter, (LPRECT)&rc);
                rc.right = (rc.right - rc.left) * nPercent / 100 + rc.left;
                hbr = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
                if (hbr) {
                    hDC = GetDC(hwndMeter);
                    if (hDC) {
                        FillRect(hDC, (LPRECT)&rc, hbr);
                        ReleaseDC(hwndMeter, hDC);
                    }
                    DeleteObject(hbr);
                }
            }
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            
            PostMessage(hDlg, WM_U_UPDATELINK, 0, lParam);
            return TRUE;
        }
        
        default:
            return FALSE;
    }
}


/* OleUIUpdateLink
 * ---------------
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
STDAPI_(BOOL) OleUIUpdateLinks(LPOLEUILINKCONTAINER lpOleUILinkCntr, HWND hwndParent, LPSTR lpszTitle, int cLinks)
{
    LPUPDATELINKS lpUL = (LPUPDATELINKS)OleStdMalloc(sizeof(UPDATELINKS));
    BOOL          fError;

    OleDbgAssert(lpOleUILinkCntr && hwndParent && lpszTitle && (cLinks>0));
    OleDbgAssert(lpUL);
    
    lpUL->lpOleUILinkCntr = lpOleUILinkCntr;
    lpUL->cLinks           = cLinks;
    lpUL->cUpdated         = 0;
    lpUL->dwLink           = 0;
    lpUL->fError           = FALSE;
    lpUL->lpszTitle    = lpszTitle;

    DialogBoxParam(ghInst, MAKEINTRESOURCE(IDD_UPDATELINKS), 
            hwndParent, UpdateLinksDlgProc, (LPARAM)lpUL);

    fError = lpUL->fError;
    OleStdFree((LPVOID)lpUL);
    
    return !fError;
}
