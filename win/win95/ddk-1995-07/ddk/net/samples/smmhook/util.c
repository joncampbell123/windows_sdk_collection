//THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
//ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
//THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//
// Copyright  1993-1995  Microsoft Corporation.  All Rights Reserved.
//
//      MODULE:         util.c
//
//      PURPOSE:        Common utilities
//
//	PLATFORMS:	Windows 95
//
//      FUNCTIONS:
//              InitACBList() - initializes the session control block list
//              DeInitACBList() - cleans up the session control block list
//              FindACBFromConn() - searches or allocates a session control block
//              CleanupACB() - removes the session control block
//              EnumCloseThreadWindow() - closes each window for the SMM thread
//              CloseThreadWindows() - enumerates the SMM thread window
//
//	SPECIAL INSTRUCTIONS: N/A
//

#include "smmhook.h" // includes common header files and global declarations

BOOL             gfSessInit = FALSE;
LPACB_HEADER     ghACB;
CRITICAL_SECTION ghSemACBhdr;

//
//
//   FUNCTION: InitACBList()
//
//   PURPOSE: intializes the session control block list
//
//

VOID NEAR PASCAL InitACBList()
{
  // If it is already initialized, do nothing
  //
  if (!gfSessInit)
  {
    ghACB = NULL;
    InitializeCriticalSection(&ghSemACBhdr);
    gfSessInit = TRUE;
  };
}

//
//
//   FUNCTION: DeInitACBList()
//
//   PURPOSE: cleans up the session control block list
//
//

VOID NEAR PASCAL DeInitACBList()
{
  // If it is not initialized, do nothing
  //
  if (gfSessInit)
  {
    ghACB = NULL;
    DeleteCriticalSection(&ghSemACBhdr);
    gfSessInit = FALSE;
  };
}

//
//
//   FUNCTION: FindACBFromConn(HANDLE, DWORD)
//
//   PURPOSE: searches or allocates a session control block for the Dial-Up
//              Networking connection
//
//   COMMENTS:
//	
//      This function searches a control block for the specified connection
//      handle first. If it is not found and the dwsize parameter is not zero,
//      it allocates a control block for the connection.
//

LPACB_HEADER NEAR PASCAL FindACBFromConn(HANDLE hConn, DWORD dwSize)
{
  LPACB_HEADER  lpACB;

  // Do we have the structure available?
  //
  ENTERCRITICALSECTION(ghSemACBhdr);
  if (ghACB != NULL)
  {
    // get myself a long pointer
    //
    lpACB = ghACB;

    // scan linked list to find matching lpAECB
    //
    while (lpACB->hConn != hConn && lpACB->pnext != NULL)
      lpACB = lpACB->pnext;

    // Have we found the node?
    //
    if (lpACB->hConn != hConn)
    {
      // No, if we are looking for an existing one, then mark not found
      //
      if (dwSize == 0)
      {
        lpACB = NULL;
      }
      else
      {
        // otherwise create a new node at end of list
        //
        LPACB_HEADER lpTemp;

        if (!(lpTemp = (LPACB_HEADER)LocalAlloc(LPTR, dwSize)))
        {
          lpACB = NULL;
        }
        else
        {
          lpACB->pnext = (LPACB_HEADER) lpTemp; // make link
          lpACB        = lpACB->pnext;  // jump to new node
          lpACB->pnext = NULL;          // init next for new node
          lpACB->hConn = hConn;
        };
      };
    };
  }
  else
  {
    lpACB = NULL;

    // make sure caller is interested in a new aecb
    //
    if (dwSize != 0)
    {
      // Allocate global AECB header from global heap
      //
      if ((ghACB = (LPACB_HEADER)LocalAlloc(LPTR, dwSize)) != NULL)
      {
        lpACB        = ghACB;
        lpACB->pnext = NULL;
        lpACB->hConn = hConn;
      };
    };
  };
  LEAVECRITICALSECTION (ghSemACBhdr);

  return(lpACB);

}

//
//
//   FUNCTION: CleanupAECB (LPAECB)
//
//   PURPOSE: deallocates the specified control block
//
//

void NEAR PASCAL CleanupACB (LPACB_HEADER lpACB)
{
  ENTERCRITICALSECTION(ghSemACBhdr);
  if (lpACB == ghACB)
  {
    LocalFree (ghACB);
    ghACB = NULL;
  }
  else
  {
    LPACB_HEADER lpacbNeighbor = ghACB;

    // scan list to find neighbor pointing to lpAECB
    //
    while (lpacbNeighbor->pnext != lpACB &&
           lpacbNeighbor->pnext != NULL)
      lpacbNeighbor = lpacbNeighbor->pnext;

    // did we find it?
    //
    if (lpacbNeighbor->pnext == lpACB)
    {
      lpacbNeighbor->pnext = lpACB->pnext;
      LocalFree ((HANDLE)lpACB);
    }
  }
  LEAVECRITICALSECTION(ghSemACBhdr);

}

//
//
//   FUNCTION: EnumCloseThreadWindow(HWND, LPARAM)
//
//   PURPOSE: Closes each window found for the thread
//
//
static BOOL CALLBACK EnumCloseThreadWindow(HWND hwnd, LPARAM lParam)
{
  PostMessage(hwnd, WM_CLOSE, 0, 0);
  return TRUE;
}

//
//
//   FUNCTION: CloseThreadWindows(DWORD)
//
//   PURPOSE: Enumerates windows for the specified thread
//
//
BOOL NEAR PASCAL CloseThreadWindows(DWORD tid)
{
  return (EnumThreadWindows(tid, EnumCloseThreadWindow, 0));
}
