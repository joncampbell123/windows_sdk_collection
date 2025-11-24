/*
 * INIT.C
 * StatStrip Test Version 1.00
 *
 * Initialization code for the StaStrip demonstration
 *
 * Copyright (c)1993-1994 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#include <windows.h>
#include <stastrip.h>
#include "stdemo.h"



/*
 * AppPAllocate
 *
 * Purpose:
 *  Initializes application data structures, registers window
 *  classes, and creates the main window.
 *
 * Parameters:
 *  pfSuccess       PINT in which a flag indicates the outcome of
 *                  the function.
 *
 *  Other parameters are from WinMain
 *
 * Return Value:
 *  PAPPVARS        If NULL returned then AppPAllocate could not
 *                  allocate memory.  If a non-NULL pointer is
 *                  returned with *pfSuccess, then call AppPFree
 *                  immediately.  If you get a non-NULL pointer
 *                  and *pfSuccess==TRUE then the function succeeded.
 */

PAPPVARS AppPAllocate(PINT pfSuccess, HINSTANCE hInst
    , HINSTANCE hInstPrev, LPSTR pszCmdLine, int nCmdShow)
    {
    PAPPVARS    pAV;
    WNDCLASS    wc;
    RECT        rc;

    if (NULL==pfSuccess)
        return NULL;

    *pfSuccess=FALSE;

    //Allocate the structure
    pAV=(PAPPVARS)(void *)LocalAlloc(LPTR, CBAPPVARS);

    if (NULL==pAV)
        return NULL;

    //Initialize WinMain parameter holders.
    pAV->hInst     =hInst;
    pAV->hInstPrev =hInstPrev;
    pAV->pszCmdLine=pszCmdLine;
    pAV->nCmdShow  =nCmdShow;

    //Register our window classes.
    if (!hInstPrev)
        {
        wc.style          = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc    = StatDemoWndProc;
        wc.cbClsExtra     = 0;
        wc.cbWndExtra     = sizeof(PAPPVARS);
        wc.hInstance      = hInst;
        wc.hIcon          = LoadIcon(hInst, TEXT("Icon"));
        wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground  = (HBRUSH)(COLOR_APPWORKSPACE + 1);
        wc.lpszMenuName   = MAKEINTRESOURCE(IDR_MENU);
        wc.lpszClassName  = TEXT("STDEMO");

        if (!RegisterClass(&wc))
            return pAV;
        }

    //Create the main window.
    pAV->hWnd=CreateWindow(TEXT("STDEMO")
        , TEXT("StatStrip Demonstration")
        , WS_MINIMIZEBOX | WS_OVERLAPPEDWINDOW, 35, 35, 450, 300
        , NULL, NULL, hInst, pAV);

    if (NULL==pAV->hWnd)
        return pAV;

    //Create the StatStrip window
    GetClientRect(pAV->hWnd, &rc);
    pAV->hWndST=CreateWindow(CLASS_STATSTRIP, TEXT("Ready")
        , WS_CHILD | WS_VISIBLE
        , rc.left, rc.top, rc.right-rc.left, CYSTATSTRIP, pAV->hWnd
        , (HMENU)ID_STATSTRIP, hInst, 0L);

    if (NULL==pAV->hWndST)
        return pAV;

    //Initialize all the messages in the StatStrip
    StatStripMessageMap(pAV->hWndST, pAV->hWnd, hInst
        , IDR_STATMESSAGEMAP, IDS_STATMESSAGEMIN, IDS_STATMESSAGEMAX
        , CCHMESSAGEMAX, ID_MENUMESSAGE, ID_MENUMESSAGE
        , ID_MESSAGEREADY, ID_MESSAGEEMPTY, ID_MENUSYS);

    ShowWindow(pAV->hWnd, nCmdShow);
    UpdateWindow(pAV->hWnd);

    *pfSuccess=TRUE;
    return pAV;
    }





/*
 * AppPFree
 *
 * Purpose:
 *  Reverses all initialization done by AppPAllocate, cleaning up
 *  any allocations including the application structure itself.
 *
 * Parameters:
 *  pAV             PAPPVARS to the application structure
 *
 * Return Value:
 *  PAPPVARS        NULL if successful, pAV if not, meaning we
 *                  couldn't free some allocation.
 */

PAPPVARS AppPFree(PAPPVARS pAV)
    {
    if (NULL==pAV)
        return NULL;

    if (NULL!=pAV->hFont)
        DeleteObject(pAV->hFont);

    if (IsWindow(pAV->hWndST))
        DestroyWindow(pAV->hWndST);

    if (IsWindow(pAV->hWnd))
        DestroyWindow(pAV->hWnd);

    return (PAPPVARS)(void *)LocalFree((HLOCAL)(UINT)(LONG)pAV);
    }
