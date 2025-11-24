/*
 * DLLFUNCS.C
 *
 * Contains entry and exit points for the DLL implementation
 * of the OLE 2.0 User Interface Support Library.           
 *
 * This file is not needed if we are linking the static library
 * version of this library.
 *
 * Copyright (c)1992 Microsoft Corporation, All Right Reserved
 */
 
#define STRICT  1
#include "ole2ui.h"
#include "common.h"

OLEDBGDATA


/*
 * LibMain
 *
 * Purpose:
 *  DLL-specific entry point called from LibEntry.  Initializes
 *  the DLL's heap and registers the GizmoBar GizmoBar.
 *
 * Parameters:
 *  hInst           HINSTANCE instance of the DLL.
 *  wDataSeg        WORD segment selector of the DLL's data segment.
 *  wHeapSize       WORD byte count of the heap.
 *  lpCmdLine       LPSTR to command line used to start the module.
 *
 * Return Value:
 *  HANDLE          Instance handle of the DLL.
 *
 */

#ifdef WIN32

extern  BOOL WINAPI _CRT_INIT(HINSTANCE hInst, DWORD dwReason, LPVOID lpReserved);
extern  _cexit(void);
int CALLBACK WEP(int);

extern  BOOL __stdcall LibMain
(                       
    HINSTANCE hInst,                    
    ULONG Reason,                    
    PCONTEXT Context                 
)
{
    UNREFERENCED_PARAMETER(Context);
    if (Reason == DLL_PROCESS_DETACH) {

        _cexit(); // BUGBUG32  replace with call to CRT_INIT when it's fixed

        WEP(0);
        return TRUE;
    }
    else if (Reason != DLL_PROCESS_ATTACH)
        return TRUE;

    if (!_CRT_INIT(hInst,Reason,NULL))
         return FALSE;

    OleUIInitialize(hInst);

	 return TRUE;

}

#else

int FAR PASCAL LibMain(HINSTANCE hInst, WORD wDataSeg
                          , WORD cbHeapSize, LPSTR lpCmdLine)
    {
    OleDbgOut2("LibMain: OLE2UI.DLL loaded\r\n");
    
// TODO:
//  Once we get the DLL and LIB versions of this library working, we need
//  to take this call to OleUIInitialize (and OleUIUninitialize below) out
//  because it is the app's responsibility to call these    
    
    OleUIInitialize(hInst);

    //All done...
    if (0!=cbHeapSize)
        UnlockData(0);

    return (int)hInst;
    }

#endif

/*
 * WEP
 *
 * Purpose:
 *  Required DLL Exit function.
 *
 * Parameters:
 *  bSystemExit     BOOL indicating if the system is being shut
 *                  down or the DLL has just been unloaded.
 *
 * Return Value:
 *  void
 *
 */
int CALLBACK EXPORT WEP(int bSystemExit)
{
    OleUIUnInitialize();
    return 0;
}





