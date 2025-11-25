/*
 * PATRON.CPP
 * Patron Chapter 12
 *
 * WinMain which is all we need for the basic application.
 *
 * Copyright (c)1993-1996 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#define INITGUIDS
#include "patron.h"

extern "C"
{
OLEDBGDATA_MAIN("PATRON")
}



/*
 * WinMain
 *
 * Purpose:
 *  Main entry point of application.  Should register the app class
 *  if a previous instance has not done so and do any other one-time
 *  initializations.
 */

int PASCAL WinMain (HINSTANCE hInst, HINSTANCE hPrev
    , LPSTR pszCmdLine, int nCmdShow)
    {
    PCPatronFrame   pFR;
    FRAMEINIT       fi;
    WPARAM          wRet=0;

   #ifndef WIN32
    int             cMsg=96;

    while (!SetMessageQueue(cMsg) && (cMsg-=8));
   #endif

    //Attempt to allocate and initialize the application
    pFR=new CPatronFrame(hInst, hPrev, pszCmdLine, nCmdShow);

    if (NULL==pFR)
        return -1;

    fi.idsMin=IDS_FRAMEMIN;
    fi.idsMax=IDS_FRAMEMAX;
    fi.idsStatMin=IDS_STATMESSAGEMIN;
    fi.idsStatMax=IDS_STATMESSAGEMAX;
    fi.idStatMenuMin=ID_MENUFILE;
    fi.idStatMenuMax=ID_MENUHELP;
    fi.iPosWindowMenu=WINDOW_MENU;
    fi.cMenus=CMENUS;

    fi.x=CW_USEDEFAULT;
    fi.y=CW_USEDEFAULT;
    fi.cx=CW_USEDEFAULT;
    fi.cy=CW_USEDEFAULT;

    //If we can initialize pFR, start chugging messages
    if (pFR->FInit(&fi))
        wRet=pFR->MessageLoop();

    delete pFR;
    return wRet;
    }




/*
 * CPatronFrame::CPatronFrame
 * CPatronFrame::~CPatronFrame
 *
 * Constructor Parameters:
 *  hInst           HINSTANCE from WinMain
 *  hInstPrev       HINSTANCE from WinMain
 *  pszCmdLine      LPSTR from WinMain
 *  nCmdShow        int from WInMain
 */

CPatronFrame::CPatronFrame(HINSTANCE hInst, HINSTANCE hInstPrev
    , LPSTR pszCmdLine, int nCmdShow)
    : CFrame(hInst, hInstPrev, pszCmdLine, nCmdShow)
    {
    m_fInitialized=FALSE;
    m_fOleStdInit=FALSE;
    m_pIClassDataTran=NULL;
    return;
    }


CPatronFrame::~CPatronFrame(void)
    {
    if (NULL!=m_pIClassDataTran)
        {
        m_pIClassDataTran->LockServer(FALSE);
        m_pIClassDataTran->Release();
        }

    OleFlushClipboard();

    if (m_fOleStdInit)
        OleStdUninitialize();

    if (m_fInitialized)
        OleUninitialize();
    return;
    }




/*
 * CPatronFrame::FInit
 *
 * Purpose:
 *  Call OleInitialize then calling down into the base class
 *  initialization.
 *
 * Parameters:
 *  pFI             PFRAMEINIT containing initialization
 *                  parameters.
 *
 * Return Value:
 *  BOOL            TRUE if initialization succeeded,
 *                  FALSE otherwise.
 */

BOOL CPatronFrame::FInit(PFRAMEINIT pFI)
    {
    DWORD       dwVer;
    HRESULT     hr;

    dwVer=OleBuildVersion();

    if (rmm!=HIWORD(dwVer))
        return FALSE;

    if (FAILED(OleInitialize(NULL)))
        return FALSE;

    m_fInitialized=TRUE;

    if (!OleStdInitialize(m_hInst))
        return FALSE;

    m_fOleStdInit=TRUE;

    //Lock the data transfer object factory as an optimization.
    hr=CoGetClassObject(CLSID_DataTransferObject
        , CLSCTX_INPROC_SERVER, NULL, IID_IClassFactory
        , (PPVOID)&m_pIClassDataTran);

    if (SUCCEEDED(hr))
        m_pIClassDataTran->LockServer(TRUE);

    return CFrame::FInit(pFI);
    }





/*
 * CPatronFrame::CreateCClient
 *
 * Purpose:
 *  Constructs a new client specific to the application.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  PCClient        Pointer to the new client object.
 */

PCClient CPatronFrame::CreateCClient(void)
    {
    return (PCClient)(new CPatronClient(m_hInst));
    }






/*
 * CPatronFrame::FRegisterAllClasses
 *
 * Purpose:
 *  Registers all classes used in this application.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  BOOL            TRUE if registration succeeded, FALSE otherwise.
 */

BOOL CPatronFrame::FRegisterAllClasses(void)
    {
    WNDCLASS        wc;

    //First let the standard frame do its thing
    if (!CFrame::FRegisterAllClasses())
        return FALSE;

    //We need double-clicks now and for object activation later.
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.hInstance     = m_hInst;
    wc.cbClsExtra    = 0;
    wc.lpfnWndProc   = PagesWndProc;
    wc.cbWndExtra    = CBPAGESWNDEXTRA;
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = SZCLASSPAGES;

    if (!RegisterClass(&wc))
        return FALSE;

    return TRUE;
    }





/*
 * CPatronFrame::OnCommand
 *
 * Purpose:
 *  WM_COMMAND handler for the Patron frame window that processes extra
 *  File menu items as well as the Page menu.
 *
 * Parameters:
 *  hWnd            HWND of the frame window.
 *  wParam          WPARAM of the message.
 *  lParam          LPARAM of the message.
 *
 * Return Value:
 *  LRESULT         Return value for the message.
 */

LRESULT CPatronFrame::OnCommand(HWND hWnd, WPARAM wParam
    , LPARAM lParam)
    {
    PCPatronDoc     pDoc;

    COMMANDPARAMS(wID, wCode, hWndMsg);

    /*
     * Don't bother with anything during first initialization,
     * skipping many GizmoBar notifications.
     */
    if (m_fInit)
        return 0L;

    pDoc=(PCPatronDoc)m_pCL->ActiveDocument();

    if (NULL!=pDoc && (IDM_VERBMIN <= wID) && (IDM_VERBMAX >= wID))
        {
        pDoc->ActivateObject(wID-IDM_VERBMIN);
        return 0L;
        }

    switch (wID)
        {
        case IDM_FILEPRINT:
            pDoc->Print(m_hWnd);
            return 0L;

        case IDM_FILEPRINTERSETUP:
            pDoc->PrinterSetup(m_hWnd, FALSE);
            return 0L;

        case IDM_EDITPASTESPECIAL:
            pDoc->FPasteSpecial(m_hWnd);
            return 0L;

        case IDM_EDITDELETEOBJECT:
            pDoc->Delete();
            return 0L;

        case IDM_EDITINSERTOBJECT:
            pDoc->FInsertObject(m_hWnd);
            return 0L;

        //CHAPTER12MOD
        case IDM_EDITLINKS:
            pDoc->FEditLinks(m_hWnd);
            return 0L;
        //End CHAPTER12MOD

        case IDM_PAGENEWPAGE:
            pDoc->NewPage();
            break;

        case IDM_PAGEDELETEPAGE:
            pDoc->DeletePage();
            break;

        case IDM_PAGENEXTPAGE:
            pDoc->NextPage();
            break;

        case IDM_PAGEPREVIOUSPAGE:
            pDoc->PreviousPage();
            break;

        case IDM_PAGEFIRSTPAGE:
            pDoc->FirstPage();
            break;

        case IDM_PAGELASTPAGE:
            pDoc->LastPage();
            break;

        //CHAPTER12MOD
        case IDM_PAGESHOWOBJECTS:
            {
            BOOL    fTemp;

            //First get the current state, then toggle it.
            fTemp=pDoc->FShowOrQueryObjectTypes(TRUE, FALSE);
            pDoc->FShowOrQueryObjectTypes(FALSE, !fTemp);
            }
            break;
        //End CHAPTER12MOD

        default:
           return CFrame::OnCommand(hWnd, wParam, lParam);
        }

    return 0L;
    }








/*
 * CPatronFrame::CreateGizmos
 *
 * Purpose:
 *  Procedure to create all the necessary gizmobar buttons.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  UINT            Number of gizmos added to the bar.
 */

UINT CPatronFrame::CreateGizmos(void)
    {
    UINT            iLast;
    UINT            uState=GIZMO_NORMAL;
    UINT            utCmd =GIZMOTYPE_BUTTONCOMMAND;

    //Insert the standard ones.
    iLast=CFrame::CreateGizmos();

    //Remove Undo:  we don't use it.
    m_pGB->Remove(IDM_EDITUNDO);

    /*
     * Insert Print File Import in the 5th position and account
     * for it in iLast.
     */
    m_pGB->Add(utCmd, 4, IDM_FILEPRINT, m_dxB, m_dyB
        , NULL, NULL, 6, uState);

    iLast++;

    m_pGB->Add(GIZMOTYPE_SEPARATOR, iLast++, 0, 6, m_dyB
        , NULL, NULL, 0, uState);

    //Add New Page, and Delete Page
    m_pGB->Add(utCmd, iLast++, IDM_PAGENEWPAGE, m_dxB, m_dyB
        , NULL, m_hBmp, 2, uState);
    m_pGB->Add(utCmd, iLast++, IDM_PAGEDELETEPAGE, m_dxB, m_dyB
        , NULL, m_hBmp, 3, uState);

    m_pGB->Add(GIZMOTYPE_SEPARATOR, iLast++, 0, 6, m_dyB
        , NULL, NULL, 0, uState);

    //First, Prev, Next, Last pages.
    m_pGB->Add(utCmd, iLast++, IDM_PAGEFIRSTPAGE, m_dxB, m_dyB
        , NULL, m_hBmp, 4, uState);
    m_pGB->Add(utCmd, iLast++, IDM_PAGEPREVIOUSPAGE, m_dxB, m_dyB
        , NULL, m_hBmp, 5, uState);
    m_pGB->Add(utCmd, iLast++, IDM_PAGENEXTPAGE, m_dxB, m_dyB
        , NULL, m_hBmp, 6, uState);
    m_pGB->Add(utCmd, iLast++, IDM_PAGELASTPAGE, m_dxB, m_dyB
        , NULL, m_hBmp, 7, uState);

    return iLast;
    }







/*
 * CPatronFrame::UpdateMenus
 *
 * Purpose:
 *  Handles the WM_INITMENU message for the frame window.  Depending
 *  on the existence of an active window, menu items are selectively
 *  enabled and disabled.
 *
 * Parameters:
 *  hMenu           HMENU of the menu to intialize
 *  iMenu           UINT position of the menu.
 *
 * Return Value:
 *  None
 */

void CPatronFrame::UpdateMenus(HMENU hMenu, UINT iMenu)
    {
    PCPatronDoc     pDoc;
    BOOL            fOK=FALSE;
    BOOL            fCallDefault=TRUE;
    UINT            uTemp;
    UINT            uTempE;
    UINT            uTempD;

    pDoc=(PCPatronDoc)m_pCL->ActiveDocument();

    uTempE=MF_ENABLED | MF_BYCOMMAND;
    uTempD=MF_DISABLED | MF_GRAYED | MF_BYCOMMAND;
    uTemp=((NULL!=pDoc) ? uTempE : uTempD);

    if (m_phMenu[0]==hMenu)
        {
        EnableMenuItem(hMenu, IDM_FILEPRINT, uTemp);

        if (NULL!=pDoc)
            fOK=pDoc->FQueryPrinterSetup();

        EnableMenuItem(hMenu, IDM_FILEPRINTERSETUP
            , (fOK) ? uTempE : uTempD);
        }

    if (m_phMenu[1]==hMenu)
        {
        if (NULL!=pDoc)
            fOK=pDoc->FQueryPaste();

        EnableMenuItem(hMenu, IDM_EDITPASTE
            , (fOK) ? uTempE : uTempD);
        EnableMenuItem(hMenu, IDM_EDITPASTESPECIAL
            , (fOK) ? uTempE : uTempD);

        //Cut, Copy, Delete depends on there being a selection.
        if (NULL!=pDoc)
            fOK=pDoc->FQueryObjectSelected(hMenu);
        else
            fOK=FALSE;

        EnableMenuItem(hMenu, IDM_EDITCUT, (fOK) ? uTempE : uTempD);
        EnableMenuItem(hMenu, IDM_EDITCOPY
            , (fOK) ? uTempE : uTempD);
        EnableMenuItem(hMenu, IDM_EDITDELETEOBJECT
            , (fOK) ? uTempE : uTempD);

        EnableMenuItem(hMenu, IDM_EDITINSERTOBJECT, uTemp);

        //CHAPTER12MOD
        if (NULL!=pDoc)
            fOK=pDoc->FQueryEnableEditLinks();
        else
            fOK=FALSE;

        EnableMenuItem(hMenu, IDM_EDITLINKS
            , (fOK) ? uTempE : uTempD);
        //End CHAPTER12MOD

        //We did the whole menu...
        fCallDefault=FALSE;
        }

    //Page menu
    if (m_phMenu[2]==hMenu)
        {
        EnableMenuItem(hMenu, IDM_PAGENEWPAGE,      uTemp);
        EnableMenuItem(hMenu, IDM_PAGEDELETEPAGE,   uTemp);
        EnableMenuItem(hMenu, IDM_PAGENEXTPAGE,     uTemp);
        EnableMenuItem(hMenu, IDM_PAGEPREVIOUSPAGE, uTemp);
        EnableMenuItem(hMenu, IDM_PAGEFIRSTPAGE,    uTemp);
        EnableMenuItem(hMenu, IDM_PAGELASTPAGE,     uTemp);

        //CHAPTER12MOD
        //Check the Show Objects command or not.
        if (NULL!=pDoc)
            fOK=pDoc->FShowOrQueryObjectTypes(TRUE, FALSE);
        else
            fOK=FALSE;

        CheckMenuItem(hMenu, IDM_PAGESHOWOBJECTS, MF_BYCOMMAND
            | ((fOK) ? MF_CHECKED : MF_UNCHECKED));
        EnableMenuItem(hMenu, IDM_PAGESHOWOBJECTS, uTemp);
        //End CHAPTER12MOD
        }

    if (fCallDefault)
        CFrame::UpdateMenus(hMenu, iMenu);

    return;
    }






/*
 * CPatronFrame::UpdateGizmos
 *
 * Purpose:
 *  Enables and disables gizmos depending on whether we have
 *  a document or not.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  None
 */

void CPatronFrame::UpdateGizmos(void)
    {
    PCDocument  pDoc;
    BOOL        fEnable;

    //Let the default hack on its gizmos.
    CFrame::UpdateGizmos();

    pDoc=m_pCL->ActiveDocument();
    fEnable=(NULL!=pDoc);

    //No document, disable just about everything
    m_pGB->Enable(IDM_FILEPRINT,        fEnable);
    m_pGB->Enable(IDM_FILEPRINTERSETUP, fEnable);

    m_pGB->Enable(IDM_PAGENEWPAGE,      fEnable);
    m_pGB->Enable(IDM_PAGEDELETEPAGE,   fEnable);
    m_pGB->Enable(IDM_PAGEFIRSTPAGE,    fEnable);
    m_pGB->Enable(IDM_PAGEPREVIOUSPAGE, fEnable);
    m_pGB->Enable(IDM_PAGENEXTPAGE,     fEnable);
    m_pGB->Enable(IDM_PAGELASTPAGE,     fEnable);

    return;
    }



/*
 * CPatronFrame::FMessageHook
 *
 * Purpose:
 *  Override of CFrame::FMessageHook so we can specifically trap
 *  WM_MENUSELECT messages for the Object verb menu to provide some
 *  meaningful information on the status strip.
 *
 * Parameters:
 *  <WndProc Parameters>
 *  pLRes           LRESULT * in which to store the return value
 *                  for the message.
 *
 * Return Value:
 *  BOOL            TRUE to prevent further processing,
 *                  FALSE otherwise.
 */

BOOL CPatronFrame::FMessageHook(HWND hWnd, UINT iMsg, WPARAM wParam
    , LPARAM lParam, LRESULT *pLRes)
    {
    BOOL        fRet=FALSE;

    *pLRes=0;

    MENUSELECTPARAMS(wItem, wMenuFlags, hMenu);

    //If this is the wrong message, nothing to do.
    if (WM_MENUSELECT!=iMsg)
        return FALSE;

    //This happens when there's no menu selection.
    if (-1==wMenuFlags)
        return FALSE;

    if (MF_POPUP & wMenuFlags)
        {
        /*
         * If this is the cascade verb menu itself, display the same
         * message.  m_phMenu[1] contains the current edit menu
         * handle.
         */
        fRet=((HMENU)wItem==GetSubMenu(m_phMenu[1],MENUPOS_OBJECT));
        }
    else
        {
        /*
         * If the ID is in the verb range, use
         * IDS_ITEMMESSAGEEDITOBJECT message
         */
        fRet=(IDM_VERBMIN <= wItem && IDM_VERBMAX >= wItem);
        }

    if (fRet)
        m_pSS->MessageDisplay(IDM_EDITOBJECT);

    return fRet;
    }
