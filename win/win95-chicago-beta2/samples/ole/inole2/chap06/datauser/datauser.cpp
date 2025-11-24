/*
 * DATAUSER.CPP
 * Data Object User Chapter 6
 *
 * Copyright (c)1993-1994 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#define INITGUIDS
#include "datauser.h"


//These are for displaying clipboard formats textually.
static TCHAR * rgszCF[13]={TEXT("Unknown"), TEXT("CF_TEXT")
                 , TEXT("CF_BITMAP"), TEXT("CF_METAFILEPICT")
                 , TEXT("CF_SYLK"), TEXT("CF_DIF"), TEXT("CF_TIFF")
                 , TEXT("CF_OEMTEXT"), TEXT("CF_DIB")
                 , TEXT("CF_PALETTE"), TEXT("CF_PENDATA")
                 , TEXT("CF_RIFF"), TEXT("CF_WAVE")};


static TCHAR szSuccess[]    =TEXT("succeeded");
static TCHAR szFailed[]     =TEXT("failed");
static TCHAR szExpected[]   =TEXT("expected");
static TCHAR szUnexpected[] =TEXT("unexpected!");


/*
 * WinMain
 *
 * Purpose:
 *  Main entry point of application.
 */

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hInstPrev
    , LPSTR pszCmdLine, int nCmdShow)
    {
    MSG         msg;
    PAPPVARS    pAV;

   #ifndef WIN32
    int         cMsg=96;

    while (!SetMessageQueue(cMsg) && (cMsg-=8));
   #endif

    pAV=new CAppVars(hInst, hInstPrev, nCmdShow);

    if (NULL==pAV)
        return -1;

    if (pAV->FInit())
        {
        while (GetMessage(&msg, NULL, 0,0 ))
            {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            }
        }

    delete pAV;
    return msg.wParam;
    }







/*
 * DataUserWndProc
 *
 * Purpose:
 *  Window class procedure.  Standard callback.
 */

LRESULT APIENTRY DataUserWndProc(HWND hWnd, UINT iMsg
    , WPARAM wParam, LPARAM lParam)
    {
    HRESULT         hr;
    PAPPVARS        pAV;
    HMENU           hMenu;
    FORMATETC       fe;
    WORD            wID;

    pAV=(PAPPVARS)GetWindowLong(hWnd, DATAUSERWL_STRUCTURE);

    switch (iMsg)
        {
        case WM_NCCREATE:
            pAV=(PAPPVARS)((LPCREATESTRUCT)lParam)->lpCreateParams;
            SetWindowLong(hWnd, DATAUSERWL_STRUCTURE, (LONG)pAV);
            return (DefWindowProc(hWnd, iMsg, wParam, lParam));

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_PAINT:
            pAV->Paint();
            break;

        case WM_COMMAND:
            SETDefFormatEtc(fe, 0, TYMED_HGLOBAL | TYMED_GDI
                | TYMED_MFPICT);

            hMenu=GetMenu(hWnd);
            wID=LOWORD(wParam);

            switch (wID)
                {
                case IDM_OBJECTUSEDLL:
                    if (!pAV->m_fEXE)
                        break;

                    pAV->m_fEXE=FALSE;
                    pAV->FReloadDataObjects(TRUE);
                    break;


                case IDM_OBJECTUSEEXE:
                    if (pAV->m_fEXE)
                        break;

                    pAV->m_fEXE=TRUE;
                    pAV->FReloadDataObjects(TRUE);
                    break;

                case IDM_OBJECTDATASIZESMALL:
                case IDM_OBJECTDATASIZEMEDIUM:
                case IDM_OBJECTDATASIZELARGE:
                    CheckMenuItem(hMenu
                        , IDM_OBJECTDATASIZESMALL,  MF_UNCHECKED);
                    CheckMenuItem(hMenu
                        , IDM_OBJECTDATASIZEMEDIUM, MF_UNCHECKED);
                    CheckMenuItem(hMenu
                        , IDM_OBJECTDATASIZELARGE,  MF_UNCHECKED);
                    CheckMenuItem(hMenu, wID,  MF_CHECKED);

                    //Kill old advise.
                    if (NULL!=pAV->m_pIDataObject
                        || 0!=pAV->m_dwConn)
                        {
                        pAV->m_pIDataObject->DUnadvise(pAV
                            ->m_dwConn);
                        }

                    if (IDM_OBJECTDATASIZELARGE==wID)
                        pAV->m_pIDataObject=pAV->m_pIDataLarge;
                    else if (IDM_OBJECTDATASIZEMEDIUM==wID)
                        pAV->m_pIDataObject=pAV->m_pIDataMedium;
                    else
                        pAV->m_pIDataObject=pAV->m_pIDataSmall;

                    //Setup new advise.
                    fe.cfFormat=pAV->m_cfAdvise;
                    pAV->m_pIDataObject->DAdvise(&fe, ADVF_NODATA
                        , pAV->m_pIAdviseSink, &pAV->m_dwConn);

                    break;

                case IDM_OBJECTQUERYGETDATA:
                    if (NULL==pAV->m_pIDataObject)
                        break;

                    fe.tymed=TYMED_HGLOBAL | TYMED_GDI
                         | TYMED_MFPICT;

                    pAV->TryQueryGetData(&fe, CF_TEXT, TRUE, 0);
                    pAV->TryQueryGetData(&fe, CF_BITMAP, TRUE, 1);
                    pAV->TryQueryGetData(&fe, CF_DIB, FALSE, 2);
                    pAV->TryQueryGetData(&fe, CF_METAFILEPICT
                        , TRUE, 3);
                    pAV->TryQueryGetData(&fe, CF_WAVE, FALSE, 4);
                    break;


                case IDM_OBJECTGETDATATEXT:
                case IDM_OBJECTGETDATABITMAP:
                case IDM_OBJECTGETDATAMETAFILEPICT:
                    if (NULL==pAV->m_pIDataObject)
                        break;

                    //Clean up whatever we currently have.
                    pAV->m_cf=0;
                    ReleaseStgMedium(&pAV->m_stm);

                    if (IDM_OBJECTGETDATATEXT==wID)
                        SETDefFormatEtc(fe, CF_TEXT, TYMED_HGLOBAL);

                    if (IDM_OBJECTGETDATABITMAP==wID)
                        SETDefFormatEtc(fe, CF_BITMAP, TYMED_GDI);

                    if (IDM_OBJECTGETDATAMETAFILEPICT==wID)
                        {
                        SETDefFormatEtc(fe, CF_METAFILEPICT
                            , TYMED_MFPICT);
                        }

                    hr=pAV->m_pIDataObject->GetData(&fe
                        , &(pAV->m_stm));

                    if (SUCCEEDED(hr))
                        pAV->m_cf=fe.cfFormat;

                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);
                    break;

                case IDM_OBJECTEXIT:
                    PostMessage(hWnd, WM_CLOSE, 0, 0L);
                    break;


                case IDM_ADVISETEXT:
                case IDM_ADVISEBITMAP:
                case IDM_ADVISEMETAFILEPICT:
                    if (NULL==pAV->m_pIDataObject)
                        break;

                    //Terminate the old connection
                    if (0!=pAV->m_dwConn)
                        {
                        pAV->m_pIDataObject->DUnadvise(pAV
                            ->m_dwConn);
                        }

                    CheckMenuItem(hMenu, pAV->m_cfAdvise
                        +IDM_ADVISEMIN, MF_UNCHECKED);
                    CheckMenuItem(hMenu, wID, MF_CHECKED);

                    //New format is wID-IDM_ADVISEMIN
                    pAV->m_cfAdvise=(UINT)(wID-IDM_ADVISEMIN);
                    fe.cfFormat=pAV->m_cfAdvise;
                    pAV->m_pIDataObject->DAdvise(&fe, ADVF_NODATA
                        , pAV->m_pIAdviseSink, &pAV->m_dwConn);

                    break;

                case IDM_ADVISEGETDATA:
                    pAV->m_fGetData=!pAV->m_fGetData;
                    CheckMenuItem(hMenu, wID, pAV->m_fGetData
                        ? MF_CHECKED : MF_UNCHECKED);
                    break;

                case IDM_ADVISEREPAINT:
                    pAV->m_fRepaint=!pAV->m_fRepaint;
                    CheckMenuItem(hMenu, wID, pAV->m_fRepaint
                        ? MF_CHECKED : MF_UNCHECKED);
                    break;

                default:
                    break;
                }
            break;

        default:
            return (DefWindowProc(hWnd, iMsg, wParam, lParam));
        }

    return 0L;
    }





/*
 * CAppVars::CAppVars
 * CAppVars::~CAppVars
 *
 * Constructor Parameters: (from WinMain)
 *  hInst           HINSTANCE of the application.
 *  hInstPrev       HINSTANCE of a previous instance.
 *  nCmdShow        UINT specifying how to show the app window.
 */

CAppVars::CAppVars(HINSTANCE hInst, HINSTANCE hInstPrev
    , UINT nCmdShow)
    {
    m_hInst       =hInst;
    m_hInstPrev   =hInstPrev;
    m_nCmdShow    =nCmdShow;

    m_hWnd        =NULL;
    m_fEXE        =FALSE;

    m_pIAdviseSink =NULL;
    m_dwConn       =0;
    m_cfAdvise     =0;
    m_fGetData     =FALSE;
    m_fRepaint     =FALSE;

    m_pIDataSmall =NULL;
    m_pIDataMedium=NULL;
    m_pIDataLarge =NULL;
    m_pIDataObject=NULL;

    m_cf=0;
    m_stm.tymed=TYMED_NULL;
    m_stm.lpszFileName=NULL;      //Initializes union to NULL
    m_stm.pUnkForRelease=NULL;

    m_fInitialized=FALSE;
    return;
    }


CAppVars::~CAppVars(void)
    {
    //This releases the data object interfaces and advises
    FReloadDataObjects(FALSE);

    ReleaseStgMedium(&m_stm);

    if (NULL!=m_pIAdviseSink)
        m_pIAdviseSink->Release();

    if (IsWindow(m_hWnd))
        DestroyWindow(m_hWnd);

    if (m_fInitialized)
        CoUninitialize();

    return;
    }







/*
 * CAppVars::FInit
 *
 * Purpose:
 *  Initializes an CAppVars object by registering window classes,
 *  creating the main window, and doing anything else prone to
 *  failure such as calling CoInitialize.  If this function fails
 *  the caller should insure that the destructor is called.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  BOOL            TRUE if successful, FALSE otherwise.
 */

BOOL CAppVars::FInit(void)
    {
    WNDCLASS    wc;
    DWORD       dwVer;
    BOOL        fRet;

    dwVer=CoBuildVersion();

    if (rmm!=HIWORD(dwVer))
        return FALSE;

    if (FAILED(CoInitialize(NULL)))
        return FALSE;

    m_fInitialized=TRUE;

    //Register our window classes.
    if (!m_hInstPrev)
        {
        wc.style          = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc    = DataUserWndProc;
        wc.cbClsExtra     = 0;
        wc.cbWndExtra     = CBWNDEXTRA;
        wc.hInstance      = m_hInst;
        wc.hIcon          = LoadIcon(m_hInst, TEXT("Icon"));
        wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszMenuName   = MAKEINTRESOURCE(IDR_MENU);
        wc.lpszClassName  = TEXT("DATAUSER");

        if (!RegisterClass(&wc))
            return FALSE;
        }

    //Create the main window.
    m_hWnd=CreateWindow(TEXT("DATAUSER"), TEXT("Data Object User")
        , WS_OVERLAPPEDWINDOW,35, 35, 350, 250, NULL
        , NULL, m_hInst, this);

    if (NULL==m_hWnd)
        return FALSE;

    ShowWindow(m_hWnd, m_nCmdShow);
    UpdateWindow(m_hWnd);

    m_pIAdviseSink=new CImpIAdviseSink(this);

    if (NULL==m_pIAdviseSink)
        return FALSE;

    m_pIAdviseSink->AddRef();

    CheckMenuItem(GetMenu(m_hWnd), IDM_OBJECTUSEDLL, MF_CHECKED);
    CheckMenuItem(GetMenu(m_hWnd), IDM_OBJECTDATASIZESMALL
        , MF_CHECKED);

    //Load the initial objects
    fRet=FReloadDataObjects(TRUE);
    m_pIDataObject=m_pIDataSmall;

    return fRet;
    }





/*
 * CAppVars::FReloadDataObjects
 *
 * Purpose:
 *  Releases the old data objects we're holding on to and reloads
 *  the new ones from either EXE or DLL depending on m_fEXE.
 *
 * Parameters:
 *  fReload         BOOL indicating if we are to recreate everything
 *                  or just release the old ones (so we can use this
 *                  from the destructor).
 *
 * Return Value:
 *  BOOL            TRUE if there are usable objects in us now.
 */

BOOL CAppVars::FReloadDataObjects(BOOL fReload)
    {
    HRESULT     hr1, hr2, hr3;
    DWORD       dwClsCtx;
    HCURSOR     hCur, hCurT;
    HMENU       hMenu;
    UINT        uTempD, uTempE;

    //Clean out any data we're holding
    m_cf=0;
    ReleaseStgMedium(&m_stm);

    //Turn off whatever data connection we have
    if (NULL!=m_pIDataObject && 0!=m_dwConn)
        m_pIDataObject->DUnadvise(m_dwConn);

    if (NULL!=m_pIDataLarge)
        m_pIDataLarge->Release();

    if (NULL!=m_pIDataMedium)
        m_pIDataMedium->Release();

    if (NULL!=m_pIDataSmall)
        m_pIDataSmall->Release();

    m_pIDataObject=NULL;
    CoFreeUnusedLibraries();

    //Exit if we just wanted to free.
    if (!fReload)
        return FALSE;


    hCur=LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT));
    hCurT=SetCursor(hCur);
    ShowCursor(TRUE);

    dwClsCtx=(m_fEXE) ? CLSCTX_LOCAL_SERVER : CLSCTX_INPROC_SERVER;

    hr1=CoCreateInstance(CLSID_DataObjectSmall, NULL, dwClsCtx
        , IID_IDataObject, (PPVOID)&m_pIDataSmall);

    hr2=CoCreateInstance(CLSID_DataObjectMedium, NULL, dwClsCtx
        , IID_IDataObject, (PPVOID)&m_pIDataMedium);

    hr3=CoCreateInstance(CLSID_DataObjectLarge, NULL, dwClsCtx
        , IID_IDataObject, (PPVOID)&m_pIDataLarge);

    ShowCursor(FALSE);
    SetCursor(hCurT);

    //If anything fails, recurse to clean up...
    if (FAILED(hr1) || FAILED(hr2) || FAILED(hr3))
        return FReloadDataObjects(FALSE);

    //Reset the state of the menus for Small, no advise, no options.
    hMenu=GetMenu(m_hWnd);
    CheckMenuItem(hMenu, IDM_OBJECTDATASIZESMALL,  MF_CHECKED);
    CheckMenuItem(hMenu, IDM_OBJECTDATASIZEMEDIUM, MF_UNCHECKED);
    CheckMenuItem(hMenu, IDM_OBJECTDATASIZELARGE,  MF_UNCHECKED);

    m_pIDataObject=m_pIDataSmall;
    CheckMenuItem(hMenu, m_cfAdvise+IDM_ADVISEMIN, MF_UNCHECKED);

    uTempE=m_fEXE  ? MF_CHECKED : MF_UNCHECKED;
    uTempD=!m_fEXE ? MF_CHECKED : MF_UNCHECKED;

    CheckMenuItem(hMenu, IDM_OBJECTUSEDLL, uTempD);
    CheckMenuItem(hMenu, IDM_OBJECTUSEEXE, uTempE);

    CheckMenuItem(hMenu, IDM_ADVISEGETDATA, MF_UNCHECKED);
    CheckMenuItem(hMenu, IDM_ADVISEREPAINT, MF_UNCHECKED);

    m_fGetData=FALSE;
    m_fRepaint=FALSE;

    //Cannot request data using async advises, so disable these.
    uTempE=m_fEXE  ? MF_DISABLED | MF_GRAYED : MF_ENABLED;
    EnableMenuItem(hMenu,  IDM_ADVISEGETDATA, uTempE);
    EnableMenuItem(hMenu, IDM_ADVISEREPAINT, uTempE);

    return TRUE;
    }



/*
 * CAppVars::TryQueryGetData
 *
 * Purpose:
 *  Centralized function call and output code for displaying results
 *  of various IDataObject::QueryGetData calls.
 *
 * Parameters:
 *  pFE             LPFORMATETC to test.
 *  cf              UINT specific clipboard format to stuff in pFE
 *                  before calling.  If zero, use whatever is
 *                  already in pFE.
 *  fExpect         BOOL indicating expected results
 *  y               UINT line on which to print results.
 *
 * Return Value:
 *  None
 */

void CAppVars::TryQueryGetData(LPFORMATETC pFE, UINT cf
    , BOOL fExpect, UINT y)
    {
    TCHAR       szTemp[80];
    LPTSTR      psz1;
    LPTSTR      psz2;
    UINT        cch;
    HRESULT     hr;
    HDC         hDC;

    if (0!=cf)
        pFE->cfFormat=cf;

    hr=m_pIDataObject->QueryGetData(pFE);
    psz1=(NOERROR==hr) ? szSuccess : szFailed;
    psz2=((NOERROR==hr)==fExpect) ? szExpected : szUnexpected;

    hDC=GetDC(m_hWnd);
    SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
    SetBkColor(hDC, GetSysColor(COLOR_WINDOW));

    if (CF_WAVE < cf || 0==cf)
        {
        cch=wsprintf(szTemp, TEXT("QueryGetData on %d %s (%s).")
            , cf, psz1, psz2);
        }
    else
        {
        cch=wsprintf(szTemp, TEXT("QueryGetData on %s %s (%s).")
            , (LPTSTR)rgszCF[cf], psz1, psz2);
        }

    //Don't overwrite other painted display.
    SetBkMode(hDC, TRANSPARENT);
    TextOut(hDC, 0, 16*y, szTemp, cch);

    ReleaseDC(m_hWnd, hDC);

    return;
    }






/*
 * CAppVars::Paint
 *
 * Purpose:
 *  Handles WM_PAINT for the main window by drawing whatever
 *  data we have sitting in the STGMEDIUM at this time.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  None
 */

void CAppVars::Paint(void)
    {
    PAINTSTRUCT     ps;
    HDC             hDC;
    HDC             hMemDC;
    LPMETAFILEPICT  pMF;
    LPTSTR          psz;
    RECT            rc;
    FORMATETC       fe;

    GetClientRect(m_hWnd, &rc);

    hDC=BeginPaint(m_hWnd, &ps);

    //May need to retrieve the data with EXE objects
    if (m_fEXE)
        {
        if (TYMED_NULL==m_stm.tymed && 0!=m_cf)
            {
            SETDefFormatEtc(fe, m_cf, TYMED_HGLOBAL
                | TYMED_MFPICT | TYMED_GDI);

            if (NULL!=m_pIDataObject)
                m_pIDataObject->GetData(&fe, &m_stm);
            }
        }

    switch (m_cf)
        {
        case CF_TEXT:
            psz=(LPTSTR)GlobalLock(m_stm.hGlobal);

            if (NULL==psz)
                break;

            SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
            SetBkColor(hDC, GetSysColor(COLOR_WINDOW));

            DrawText(hDC, psz, lstrlen(psz), &rc
                , DT_LEFT | DT_WORDBREAK);
            GlobalUnlock(m_stm.hGlobal);
            break;


        case CF_BITMAP:
            hMemDC=CreateCompatibleDC(hDC);

            if (NULL!=SelectObject(hMemDC, (HGDIOBJ)m_stm.hGlobal))
                {
                BitBlt(hDC, 0, 0, rc.right-rc.left, rc.bottom-rc.top
                    , hMemDC, 0, 0, SRCCOPY);
                }

            DeleteDC(hMemDC);
            break;


        case CF_METAFILEPICT:
            pMF=(LPMETAFILEPICT)GlobalLock(m_stm.hGlobal);

            if (NULL==pMF)
                break;

            SetMapMode(hDC, pMF->mm);
            SetWindowOrgEx(hDC, 0, 0, NULL);
            SetWindowExtEx(hDC, pMF->xExt, pMF->yExt, NULL);

            SetViewportExtEx(hDC, rc.right-rc.left
                , rc.bottom-rc.top, NULL);

            PlayMetaFile(hDC, pMF->hMF);
            GlobalUnlock(m_stm.hGlobal);
            break;


        default:
            break;
        }

    EndPaint(m_hWnd, &ps);
    return;
    }
