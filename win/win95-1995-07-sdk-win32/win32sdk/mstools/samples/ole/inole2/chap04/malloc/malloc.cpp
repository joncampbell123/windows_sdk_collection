/*
 * MALLOC.CPP
 * IMalloc Demonstration Chapter 4
 *
 * Copyright (c)1993-1995 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#include "malloc.h"


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

   #ifdef WIN16
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
 * MallocWndProc
 *
 * Purpose:
 *  Standard window class procedure.
 */

LRESULT APIENTRY MallocWndProc(HWND hWnd, UINT iMsg
    , WPARAM wParam, LPARAM lParam)
    {
    PAPPVARS        pAV;
    ULONG           cb;
    UINT            i;
    BOOL            fResult;

    pAV=(PAPPVARS)GetWindowLong(hWnd, MALLOCWL_STRUCTURE);

    switch (iMsg)
        {
        case WM_NCCREATE:
            pAV=(PAPPVARS)(((LPCREATESTRUCT)lParam)->lpCreateParams);
            SetWindowLong(hWnd, MALLOCWL_STRUCTURE, (LONG)pAV);
            return (DefWindowProc(hWnd, iMsg, wParam, lParam));

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam))
                {
                case IDM_IMALLOCCOGETMALLOCTASKS:
                    pAV->SwitchAllocator(TASK_STANDARD);
                    break;


               #ifndef WIN32
                //Shared allocator not available on Win32.
                case IDM_IMALLOCCOGETMALLOCSHARED:
                    pAV->SwitchAllocator(SHARED_STANDARD);
                    break;
               #endif //WIN32


                case IDM_IMALLOCRELEASE:
                    if (NULL==pAV->m_pIMalloc)
                        {
                        pAV->Message(TEXT("Select an allocator."));
                        break;
                        }

                    pAV->FreeAllocations(TRUE);
                    pAV->Message(TEXT("Release complete."));
                    break;


                case IDM_IMALLOCALLOC:
                    if (NULL==pAV->m_pIMalloc)
                        {
                        pAV->Message(TEXT("Select an allocator."));
                        break;
                        }

                    fResult=pAV->DoAllocations(FALSE);

                    pAV->Message(fResult
                        ? TEXT("Alloc succeeded.")
                        : TEXT("Alloc failed."));

                    break;


                case IDM_IMALLOCFREE:
                    if (NULL==pAV->m_pIMalloc)
                        {
                        pAV->Message(TEXT("Select an allocator."));
                        break;
                        }

                    if (!pAV->m_fAllocated)
                        {
                        pAV->Message(TEXT("Choose Alloc first."));
                        break;
                        }

                    pAV->FreeAllocations(FALSE);
                    pAV->Message(TEXT("Free complete."));
                    break;


                case IDM_IMALLOCREALLOC:
                    if (NULL==pAV->m_pIMalloc)
                        {
                        pAV->Message(TEXT("Select an allocator."));
                        break;
                        }

                    fResult=pAV->DoAllocations(TRUE);

                    pAV->Message(fResult
                        ? TEXT("Realloc succeeded.")
                        : TEXT("Realloc failed."));

                    break;


                case IDM_IMALLOCGETSIZE:
                    if (NULL==pAV->m_pIMalloc)
                        {
                        pAV->Message(TEXT("Select an allocator."));
                        break;
                        }

                    if (!pAV->m_fAllocated)
                        {
                        pAV->Message(TEXT("Choose Alloc first."));
                        break;
                        }

                    fResult=TRUE;

                    for (i=0; i < CALLOCS; i++)
                        {
                        cb=pAV->m_pIMalloc->GetSize(pAV->m_rgpv[i]);

                        /*
                         * We test that the size is *at least*
                         * what we wanted.
                         */
                        fResult &= (pAV->m_rgcb[i] <= cb);
                        }

                    pAV->Message(fResult
                        ? TEXT("Sizes matched.")
                        : TEXT("Sizes mismatch."));

                    break;


                case IDM_IMALLOCDIDALLOC:
                    if (NULL==pAV->m_pIMalloc)
                        {
                        pAV->Message(TEXT("Select an allocator."));
                        break;
                        }

                    if (!pAV->m_fAllocated)
                        {
                        pAV->Message(TEXT("Choose Alloc first."));
                        break;
                        }

                    /*
                     * DidAlloc may return -1 if it does not know
                     * whether or not it actually allocated
                     * something.  In that case we just blindly
                     * & in a -1 with no affect.
                     */

                    fResult=(BOOL)-1;

                    for (i=0; i < CALLOCS; i++)
                        {
                        fResult &= pAV->m_pIMalloc
                            ->DidAlloc(pAV->m_rgpv[i]);
                        }

                    if (0==fResult)
                        pAV->Message(TEXT("DidAlloc is FALSE"));

                    if (1==fResult)
                        pAV->Message(TEXT("DidAlloc is TRUE"));

                    if (-1==fResult)
                        pAV->Message(TEXT("DidAlloc is unknown."));

                    break;


                case IDM_IMALLOCHEAPMINIMIZE:
                    if (NULL==pAV->m_pIMalloc)
                        {
                        pAV->Message(TEXT("Select an allocator."));
                        break;
                        }

                    pAV->m_pIMalloc->HeapMinimize();
                    pAV->Message(TEXT("HeapMinimize finished."));
                    break;


                case IDM_IMALLOCEXIT:
                    PostMessage(hWnd, WM_CLOSE, 0, 0L);
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
    UINT        i;
    ULONG       cb;

    m_hInst       =hInst;
    m_hInstPrev   =hInstPrev;
    m_nCmdShow    =nCmdShow;

    m_hWnd        =NULL;
    m_pIMalloc    =NULL;
    m_fInitialized=FALSE;
    m_fAllocated  =FALSE;

    //100 is arbitrary.  IMalloc can handle larger.
    cb=100;

    for (i=0; i < CALLOCS; i++)
        {
        m_rgcb[i]=cb;
        m_rgpv[i]=NULL;

        cb*=2;
        }

    return;
    }



CAppVars::~CAppVars(void)
    {
    FreeAllocations(TRUE);

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

    //Make sure COMPOBJ.DLL is the right version
    dwVer=CoBuildVersion();

    if (rmm!=HIWORD(dwVer))
        return FALSE;

    if (!m_hInstPrev)
        {
        wc.style          = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc    = MallocWndProc;
        wc.cbClsExtra     = 0;
        wc.cbWndExtra     = CBWNDEXTRA;
        wc.hInstance      = m_hInst;
        wc.hIcon          = LoadIcon(m_hInst, TEXT("Icon"));
        wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszMenuName   = MAKEINTRESOURCE(IDR_MENU);
        wc.lpszClassName  = TEXT("MALLOC");

        if (!RegisterClass(&wc))
            return FALSE;
        }

    m_hWnd=CreateWindow(TEXT("MALLOC"), TEXT("IMalloc Object Demo")
        , WS_OVERLAPPEDWINDOW, 35, 35, 350, 250, NULL, NULL
        , m_hInst, this);

    if (NULL==m_hWnd)
        return FALSE;

    ShowWindow(m_hWnd, m_nCmdShow);
    UpdateWindow(m_hWnd);

    return TRUE;
    }




/*
 * CAppVars::Message
 *
 * Purpose:
 *  Displays a message using a message box.  This is just to
 *  centralize the call to simpify other code.
 *
 * Parameters:
 *  psz             LPTSTR to the string to display.
 *
 * Return Value:
 *  None
 */

void inline CAppVars::Message(LPTSTR psz)
    {
    MessageBox(m_hWnd, psz, TEXT("Malloc"), MB_OK);
    return;
    }





/*
 * CAppVars::SwitchAllocator
 *
 * Purpose:
 *  Reinitializes COM with a different allocator if switching
 *  between the task allocators.
 *
 * Parameters:
 *  iMalloc         ALLOCTYPE identifying the new one to use.
 *
 * Return Value:
 *  None--state is kept in CAppVars.
 */

void CAppVars::SwitchAllocator(ALLOCTYPE iMalloc)
    {
    HRESULT     hr;
    TCHAR      *pszType=TEXT("task");
    TCHAR       szTemp[80];

    FreeAllocations(TRUE);

    //This releases previous IMalloc
    if (m_fInitialized)
        CoUninitialize();

    hr=CoInitialize(NULL);

    if (FAILED(hr))
        return;

    m_fInitialized=TRUE;

    if (SHARED_STANDARD!=iMalloc)
        hr=CoGetMalloc(MEMCTX_TASK, &m_pIMalloc);
    else
        {
       #ifndef WIN32
        //Shared allocator only on Win16.
        hr=CoGetMalloc(MEMCTX_SHARED, &m_pIMalloc);
        pszType=TEXT("shared");
       #else
        return;
       #endif
        }

    wsprintf(szTemp, TEXT("CoGetMalloc(%s) %s"), pszType
        , SUCCEEDED(hr) ? TEXT("succeeded") : TEXT("failed"));

    Message(szTemp);
    return;
    }





/*
 * CAppVars::DoAllocations
 *
 * Purpose:
 *  Centralized place to clean up allocations made on
 *  the current IMalloc.
 *
 * Parameters:
 *  fRelease        BOOL indicating if we're to
 *                  IMalloc::Release as well.
 *
 * Return Value:
 *  BOOL            TRUE if all allocations succeeded.
 */

BOOL CAppVars::DoAllocations(BOOL fRealloc)
    {
    UINT        i;
    ULONG       iByte;
    BOOL        fResult=TRUE;
    ULONG       cb;
    LPVOID      pv;

    if (!fRealloc)
        FreeAllocations(FALSE);

    for (i=0; i < CALLOCS; i++)
        {
        //cb is set in the code below for later initialization
        if (fRealloc)
            {
            m_rgcb[i]+=128;
            cb=m_rgcb[i];

            //Old memory is not freed if Realloc fails
            pv=m_pIMalloc->Realloc(m_rgpv[i], cb);
            }
        else
            {
            cb=m_rgcb[i];
            pv=m_pIMalloc->Alloc(cb);
            }

        m_rgpv[i]=pv;

        //Fill the memory with letters.
        if (NULL!=pv)
            {
            LPBYTE  pb=(LPBYTE)pv;

            for (iByte=0; iByte < cb; iByte++)
                *pb++=('a'+i);
            }

        fResult &= (NULL!=pv);
        }

    m_fAllocated=fResult;

    //Clean up whatever we might have allocated
    if (!fResult)
        FreeAllocations(FALSE);

    return fResult;
    }







/*
 * CAppVars::FreeAllocations
 *
 * Purpose:
 *  Centralized place to clean up allocations made on
 *  the current IMalloc.
 *
 * Parameters:
 *  fRelease        BOOL indicating if we're to
 *                  IMalloc::Release as well.
 *
 * Return Value:
 *  None
 */

void CAppVars::FreeAllocations(BOOL fRelease)
    {
    UINT    i;

    if (NULL==m_pIMalloc)
        return;

    if (m_fAllocated)
        {
        for (i=0; i < CALLOCS; i++)
            {
            if (NULL!=m_rgpv[i])
                m_pIMalloc->Free(m_rgpv[i]);

            m_rgpv[i]=NULL;
            }

        m_fAllocated=FALSE;
        }

    if (fRelease)
        {
        m_pIMalloc->Release();
        m_pIMalloc=NULL;
        }

    return;
    }
