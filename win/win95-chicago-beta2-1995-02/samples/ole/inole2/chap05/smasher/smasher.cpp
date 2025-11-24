/*
 * SMASHER.CPP
 *
 * File Manager extension DLL to demonstrate compound file
 * defragmentation.
 *
 * Copyright (c)1993-1994 Microsoft Corporation, All Rights Reserved.
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 *
 */


#define INC_OLE2
#include <windows.h>
#include <ole2.h>
#ifdef WIN32
#include <wfext.h>              //Win32 comes with this.
#else
#include "wfext16.h"            //16-bit version with this sample
#endif
#include <book1632.h>
#include "smasher.h"


HINSTANCE   g_hInst;
BOOL        fInitialized;


//Toolbar to place on Windows for Workgroups File Manager
EXT_BUTTON btns[1]={{IDM_SMASH, IDS_SMASHHELP+1, 0}};


/*
 * LibMain[32]
 *
 * Purpose:
 *  Entry point for Windows DLL either in Win16 or Win32
 */

#ifdef WIN32

BOOL WINAPI DllMain(HINSTANCE hInstance, ULONG ulReason
    , LPVOID pvReserved)
    {
    if (DLL_PROCESS_ATTACH==ulReason)
        g_hInst=hInstance;

    return TRUE;
    }

#else

int PASCAL LibMain(HINSTANCE hInstance, WORD wDataSeg
    , WORD cbHeapSize, LPSTR lpCmdLine)
    {

    if (0!=cbHeapSize)
        UnlockData(0);

    g_hInst=hInstance;
    return (int)hInstance;
    }

#endif





/*
 * FMExtensionProc
 *
 * Purpose:
 *  File Manager Extension callback function, receives messages from
 *  file manager when extension toolbar buttons and commands are
 *  invoked.
 *
 * Parameters:
 *  hWnd            HWND of File Manager.
 *  iMsg            UINT message identifier
 *  lParam          LONG extra information.
 *
 * Return Value:
 *  HMENU
 */

HMENU WINAPI FMExtensionProc(HWND hWnd, UINT iMsg, LONG lParam)
    {
    HMENU               hMenu=NULL;
    LPFMS_LOAD          pLoad;
    LPFMS_TOOLBARLOAD   pTool;
    LPFMS_HELPSTRING    pHelp;

    switch (iMsg)
        {
        case FMEVENT_LOAD:
            pLoad=(LPFMS_LOAD)lParam;
            pLoad->dwSize=sizeof(FMS_LOAD);

            /*
             * If the host did CoInitialize, this has no effect.
             * Otherwise it sets up the standard IMalloc for us.
             */
            if (FAILED(CoInitialize(NULL)))
                return NULL;

            fInitialized=TRUE;

            //Assign the popup menu name for extension
            LoadString(g_hInst, IDS_SMASH, pLoad->szMenuName
                , sizeof(pLoad->szMenuName));

            //Load the popup menu
            pLoad->hMenu=LoadMenu(g_hInst
                , MAKEINTRESOURCE(IDR_MENU));

            return pLoad->hMenu;


        case FMEVENT_UNLOAD:
            if (fInitialized)
                CoUninitialize();
            break;


        case FMEVENT_TOOLBARLOAD:
            /*
             * File Manager loaded our toolbar extension, so fill
             * the TOOLBARLOAD structure with information about our
             * buttons.  This is only for Windows for Workgroups.
             */

            pTool=(LPFMS_TOOLBARLOAD)lParam;
            pTool->lpButtons= (LPEXT_BUTTON)&btns;
            pTool->cButtons = 1;
            pTool->cBitmaps = 1;
            pTool->idBitmap = IDR_BITMAP;
            break;


        case FMEVENT_HELPSTRING:
            //File Manager is requesting a status-line help string.
            pHelp=(LPFMS_HELPSTRING)lParam;

            LoadString(g_hInst, IDS_SMASHHELP+pHelp->idCommand
                , pHelp->szHelp, sizeof(pHelp->szHelp));

            break;


        case IDM_SMASH:
            SmashSelectedFiles(hWnd);
            break;
        }

    return hMenu;
    }






/*
 * SmashSelectedFiles
 *
 * Purpose:
 *  Retrieves the list of selected files from File Manager and
 *  attempts to compress each one.
 *
 * Parameters:
 *  hWnd            HWND of the File Manager message processing window
 *                  that we send messages to in order to retrieve the
 *                  count of selected files and their filenames.
 *
 * Return Value:
 *  BOOL            TRUE if the function was successful, FALSE otherwise.
 */

BOOL SmashSelectedFiles(HWND hWnd)
    {
    FMS_GETFILESEL  fms;
    UINT            cFiles;
    UINT            i;
    LPTSTR          pszErr;
    HRESULT         hr;
    STATSTG         st;
    OFSTRUCT        of;
    LPMALLOC        pIMalloc;
    LPSTORAGE       pIStorageOld;
    LPSTORAGE       pIStorageNew;
    HCURSOR         hCur;
   #if defined(WIN32) && !defined(UNICODE)
    CHAR            szTemp[CCHPATHMAX];
   #endif

    /*
     * Retrieve information from File Manager about the selected
     * files and allocate memory for the paths and filenames.
     */

    //Get the number of selected items.
    cFiles=(UINT)SendMessage(hWnd, FM_GETSELCOUNT, 0, 0L);

    //Nothing to do, so quit.
    if (0==cFiles)
        return TRUE;

    //Get error string memory
    hr=CoGetMalloc(MEMCTX_TASK, &pIMalloc);

    if (FAILED(hr))
        return FALSE;

    hCur=SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT)));
    pszErr=(LPTSTR)pIMalloc->Alloc(1024);

    /*
     * Enumerate selected files and directories with the
     * FM_GETFILESEL message.  For each file, check if it's
     * a Compound File (StgIsStorageFile) and if not, skip it.
     *
     * If it is a compound file, create a temp file and CopyTo
     * from old to new. If this works, we reopen the old file
     * in overwrite mode and CopyTo back into it.
     */

    for (i = 0; i < cFiles; i++)
        {
        SendMessage(hWnd, FM_GETFILESEL, i, (LONG)&fms);

        OLECHAR pwcsName[MAX_PATH];
#if defined(WIN32) && !defined(UNICODE)
        mbstowcs(pwcsName, fms.szName, MAX_PATH);
#else
        pwcsName = fms.szName;
#endif
        //Skip non-storages.
        if (NOERROR!=StgIsStorageFile(pwcsName))
            {
            wsprintf(pszErr, SZERRNOTACOMPOUNDFILE
                , (LPTSTR)fms.szName);
            MessageBox(hWnd, pszErr, SZSMASHER, MB_OK
                | MB_ICONHAND);
            continue;
            }

        /*
         * Create a temporary file.  We don't use DELETEONRELEASE
         * in case we have to save it when copying over the old
         * file fails.
         */
        hr=StgCreateDocfile(NULL, STGM_CREATE | STGM_READWRITE
            | STGM_DIRECT | STGM_SHARE_EXCLUSIVE, 0, &pIStorageNew);

        if (FAILED(hr))
            {
            MessageBox(hWnd, SZERRTEMPFILE, SZSMASHER, MB_OK
                | MB_ICONHAND);
            continue;
            }

        //Open the existing file as read-only
        hr=StgOpenStorage(pwcsName, NULL, STGM_DIRECT | STGM_READ
            | STGM_SHARE_DENY_WRITE, NULL, 0, &pIStorageOld);

        if (FAILED(hr))
            {
            pIStorageNew->Release();
            wsprintf(pszErr, SZERROPENFAILED, (LPTSTR)fms.szName);
            MessageBox(hWnd, pszErr, SZSMASHER, MB_OK
                | MB_ICONHAND);
            continue;
            }

        /*
         * Compress with CopyTo.  Since the temp is opened in
         * direct mode, changes are immediate.
         */
        hr=pIStorageOld->CopyTo(NULL, NULL, NULL, pIStorageNew);
        pIStorageOld->Release();

        if (FAILED(hr))
            {
            pIStorageNew->Release();
            MessageBox(hWnd, SZERRTEMPFILECOPY, SZSMASHER, MB_OK
                | MB_ICONHAND);
            continue;
            }

        //Temp file contains defragmented copy, try copying back.
        hr=StgOpenStorage(pwcsName, NULL, STGM_DIRECT
            | STGM_CREATE | STGM_WRITE | STGM_SHARE_EXCLUSIVE
            , NULL, 0, &pIStorageOld);

        if (FAILED(hr))
            {
            pIStorageNew->Stat(&st, 0);
            pIStorageNew->Release();

            wsprintf(pszErr, SZERRTEMPHASFILE, (LPTSTR)st.pwcsName);
            pIMalloc->Free(st.pwcsName);

            MessageBox(hWnd, pszErr, SZSMASHER, MB_OK
                | MB_ICONHAND);
            continue;
            }

        //Copy over the old one.
        pIStorageNew->CopyTo(NULL, NULL, NULL, pIStorageOld);
        pIStorageOld->Release();

        //Delete the temporary file.
        pIStorageNew->Stat(&st, 0);
        pIStorageNew->Release();

       #if defined(WIN32) && !defined(UNICODE)
        UNICODETOANSI(st.pwcsName, szTemp, CCHPATHMAX);
        OpenFile(szTemp, &of, OF_DELETE);
       #else
        OpenFile(st.pwcsName, &of, OF_DELETE);
       #endif
        pIMalloc->Free(st.pwcsName);
        }

    pIMalloc->Free(pszErr);
    pIMalloc->Release();

    SetCursor(hCur);
    return TRUE;
    }
