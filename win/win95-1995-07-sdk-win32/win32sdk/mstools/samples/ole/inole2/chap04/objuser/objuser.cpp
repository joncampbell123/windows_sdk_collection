/*
 * OBJUSER.CPP
 * Koala Object User Chapter 4
 *
 * Copyright (c)1993-1995 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#define INITGUIDS
#define INC_OLE2
#include <windows.h>
#include <ole2.h>
#include <ole2ver.h>
#include "objuser.h"


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
 * ObjectUserWndProc
 *
 * Purpose:
 *  Window class procedure.  Standard callback.
 */

LRESULT APIENTRY ObjectUserWndProc(HWND hWnd, UINT iMsg
    , WPARAM wParam, LPARAM lParam)
    {
    HRESULT         hr;
    PAPPVARS        pAV;
    CLSID           clsID;
    LPCLASSFACTORY  pIClassFactory;
    DWORD           dwClsCtx;
    LPUNKNOWN       pUnk;

    pAV=(PAPPVARS)GetWindowLong(hWnd, OBJUSERWL_STRUCTURE);

    switch (iMsg)
        {
        case WM_NCCREATE:
            pAV=(PAPPVARS)(((LPCREATESTRUCT)lParam)->lpCreateParams);
            SetWindowLong(hWnd, OBJUSERWL_STRUCTURE, (LONG)pAV);
            return (DefWindowProc(hWnd, iMsg, wParam, lParam));

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam))
                {
                case IDM_OBJECTUSEDLL:
                    pAV->m_fEXE=FALSE;
                    CheckMenuItem(GetMenu(hWnd), IDM_OBJECTUSEDLL
                        , MF_CHECKED);
                    CheckMenuItem(GetMenu(hWnd), IDM_OBJECTUSEEXE
                        , MF_UNCHECKED);
                    break;


                case IDM_OBJECTUSEEXE:
                    pAV->m_fEXE=TRUE;
                    CheckMenuItem(GetMenu(hWnd), IDM_OBJECTUSEDLL
                        , MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hWnd), IDM_OBJECTUSEEXE
                        , MF_CHECKED);
                    break;


                case IDM_OBJECTCREATECOGCO:
                    if (NULL!=pAV->m_pIPersist)
                        {
                        pAV->m_pIPersist->Release();
                        pAV->m_pIPersist=NULL;
                        CoFreeUnusedLibraries();
                        }

                    dwClsCtx=(pAV->m_fEXE) ? CLSCTX_LOCAL_SERVER
                        : CLSCTX_INPROC_SERVER;

                    hr=CoGetClassObject(CLSID_Koala, dwClsCtx, NULL
                        , IID_IClassFactory
                        , (PPVOID)&pIClassFactory);


                    if (SUCCEEDED(hr))
                        {
                        //Create the Koala asking for IID_IPersist
                        hr=pIClassFactory->CreateInstance(NULL
                            , IID_IUnknown, (PPVOID)&pUnk);

                        //Release the class factory when done.
                        pIClassFactory->Release();

                        if (SUCCEEDED(hr))
                            {
                            pAV->Message(TEXT("Creation succeeded."));
                            pUnk->QueryInterface(IID_IPersist
                                , (PPVOID)&pAV->m_pIPersist);
                            pUnk->Release();
                            }
                        else
                            pAV->Message(TEXT("Creation failed."));
                        }
                    else
                        pAV->Message(TEXT("CoGetClassObject failed."));

                    break;


                case IDM_OBJECTCREATECOCI:
                    if (NULL!=pAV->m_pIPersist)
                        {
                        pAV->m_pIPersist->Release();
                        pAV->m_pIPersist=NULL;
                        CoFreeUnusedLibraries();
                        }

                    //Simpler creation:  use CoCreateInstance
                    dwClsCtx=(pAV->m_fEXE) ? CLSCTX_LOCAL_SERVER
                        : CLSCTX_INPROC_SERVER;

                    hr=CoCreateInstance(CLSID_Koala, NULL
                        , dwClsCtx, IID_IUnknown, (PPVOID)&pUnk);

                    if (SUCCEEDED(hr))
                        {
                        pAV->Message(TEXT("Creation succeeded."));
                        pUnk->QueryInterface(IID_IPersist
                            , (PPVOID)&pAV->m_pIPersist);
                        pUnk->Release();
                        }
                    else
                        pAV->Message(TEXT("Creation failed."));

                    break;


                case IDM_OBJECTRELEASE:
                    if (NULL==pAV->m_pIPersist)
                        {
                        pAV->Message(TEXT("There is no object."));
                        break;
                        }

                    pAV->m_pIPersist->Release();
                    pAV->m_pIPersist=NULL;
                    pAV->Message(TEXT("Object released."));

                    CoFreeUnusedLibraries();
                    break;


                case IDM_OBJECTGETCLASSID:
                    if (NULL==pAV->m_pIPersist)
                        {
                        pAV->Message(TEXT("There is no object."));
                        break;
                        }

                    hr=pAV->m_pIPersist->GetClassID(&clsID);

                    if (SUCCEEDED(hr))
                        {
                        LPTSTR      psz;
                        LPMALLOC    pIMalloc;

                        //String from CLSID uses task Malloc
#if defined(WIN32) && !defined(UNICODE)
                        LPOLESTR pszOLE;
                        StringFromCLSID(clsID, &pszOLE);
                        CoGetMalloc(MEMCTX_TASK, &pIMalloc);
                        psz = (LPSTR) pIMalloc->Alloc(wcslen(pszOLE)+1);
                        wcstombs(psz, pszOLE, wcslen(pszOLE)+1);
                        pIMalloc->Free(pszOLE);
                        pIMalloc->Release();
#else
                        StringFromCLSID(clsID, &psz);
#endif
                        pAV->Message(psz);

                        CoGetMalloc(MEMCTX_TASK, &pIMalloc);
                        pIMalloc->Free(psz);
                        pIMalloc->Release();
                        }
                    else
                        pAV->Message(TEXT("GetClassID call failed."));

                    break;

                case IDM_OBJECTEXIT:
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
 *
 */

CAppVars::CAppVars(HINSTANCE hInst, HINSTANCE hInstPrev
    , UINT nCmdShow)
    {
    m_hInst       =hInst;
    m_hInstPrev   =hInstPrev;
    m_nCmdShow    =nCmdShow;

    m_hWnd        =NULL;
    m_fEXE        =FALSE;

    m_pIPersist   =NULL;
    m_fInitialized=FALSE;
    return;
    }


CAppVars::~CAppVars(void)
    {
    if (NULL!=m_pIPersist)
        m_pIPersist->Release();

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

    dwVer=CoBuildVersion();

    if (rmm!=HIWORD(dwVer))
        return FALSE;

    if (FAILED(CoInitialize(NULL)))
        return FALSE;

    m_fInitialized=TRUE;

    if (!m_hInstPrev)
        {
        wc.style          = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc    = ObjectUserWndProc;
        wc.cbClsExtra     = 0;
        wc.cbWndExtra     = CBWNDEXTRA;
        wc.hInstance      = m_hInst;
        wc.hIcon          = LoadIcon(m_hInst, TEXT("Icon"));
        wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszMenuName   = MAKEINTRESOURCE(IDR_MENU);
        wc.lpszClassName  = TEXT("OBJUSER");

        if (!RegisterClass(&wc))
            return FALSE;
        }

    m_hWnd=CreateWindow(TEXT("OBJUSER")
        , TEXT("Component Object User"), WS_OVERLAPPEDWINDOW
        , 35, 35, 350, 250, NULL, NULL, m_hInst, this);

    if (NULL==m_hWnd)
        return FALSE;

    ShowWindow(m_hWnd, m_nCmdShow);
    UpdateWindow(m_hWnd);

    CheckMenuItem(GetMenu(m_hWnd), IDM_OBJECTUSEDLL, MF_CHECKED);
    CheckMenuItem(GetMenu(m_hWnd), IDM_OBJECTUSEEXE, MF_UNCHECKED);

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
    MessageBox(m_hWnd, psz, TEXT("Object User"), MB_OK);
    return;
    }
