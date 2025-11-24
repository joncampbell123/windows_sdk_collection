/*****************************************************************/
/**                 Microsoft Windows 95                        **/
/**        Copyright (C) Microsoft Corp., 1991-1995             **/
/*****************************************************************/ 

#include "mspp.h"

//
// Global variables
//
HANDLE hInstance;

/////////////////////////////////////////////////////////////////////////////
//  OnProcessAttach
//
//  Initialization to be performed at DLL_PROCESS_ATTACH time. Critical
//  section and local heap setup.  Returns TRUE if successful, FALSE 
//  otherwise.
//
/////////////////////////////////////////////////////////////////////////////
BOOL OnProcessAttach( HANDLE hInst) {
          
  DBGMSG(DBG_LEV_VERBOSE,("MSPP32.OnProcessAttach called\n"));

  PPInitCritical();
            
  hInstance = hInst;

  return PPInitHeap();
}

/////////////////////////////////////////////////////////////////////////////
//  OnProcessDetach
//
//  Housekeeping to be performed at DLL_PROCESS_DETACH time. Delete
//  critical sections and check for memory leaks.
//
//
/////////////////////////////////////////////////////////////////////////////
void OnProcessDetach()
{

 DBGMSG(DBG_LEV_VERBOSE,("MSPP32.OnProcessDetach called\n"));

 PPDeleteCritical();

 PPMemLeakCheck();

}

BOOL _stdcall DllEntryPoint(int hInstDll, DWORD fdwReason, LPVOID reserved)
{

  DBGMSG(DBG_LEV_VERBOSE,("MSPP32.DllEntryPoint called\n"));

//
// Run our normal DLL startup stuff.
//
  if (fdwReason == DLL_PROCESS_ATTACH) {
    return OnProcessAttach((HANDLE) hInstDll);
  }
  else if (fdwReason == DLL_PROCESS_DETACH) {
    OnProcessDetach();
  }

  return TRUE;
}


