/*
 * BUSY.C
 *
 * Implements the OleUIBusy function which invokes the "Server Busy"
 * dialog.
 *
 * Copyright (c)1992 Microsoft Corporation, All Right Reserved
 */
 
#define STRICT  1
#include "ole2ui.h"
#include "common.h"
#include "utility.h"
#include "busy.h" 
#include <ctype.h> // for tolower() and toupper()

#ifndef WIN32
#include <toolhelp.h>
#endif


/*
 * OleUIBusy
 *
 * Purpose:
 *  Invokes the standard OLE "Server Busy" dialog box which
 *  notifies the user that the server application is not receiving
 *  messages.  The dialog then asks the user to either cancel
 *  the operation, switch to the task which is blocked, or continue
 *  waiting.
 *
 * Parameters:
 *  lpBZ            LPOLEUIBUSY pointing to the in-out structure
 *                  for this dialog.
 *
 * Return Value:
 *              OLEUI_BZERR_HTASKINVALID  : Error
 *              OLEUI_BZ_SWITCHTOSELECTED : Success, user selected "switch to"
 *              OLEUI_BZ_RETRYSELECTED    : Success, user selected "retry"
 *              OLEUI_CANCEL              : Success, user selected "cancel"
 */
 
STDAPI_(UINT) OleUIBusy(LPOLEUIBUSY lpOBZ)
    {
    UINT        uRet = 0;
    HGLOBAL     hMemDlg=NULL;

#if defined( NT_BUG )

    uRet=UStandardValidation((LPOLEUISTANDARD)lpOBZ, sizeof(OLEUIBUSY)
                             , &hMemDlg);
                             
    // Error out if the standard validation failed
    if (OLEUI_SUCCESS!=uRet)
        return uRet;

#if !defined( WIN32 )
    // Validate HTASK
    if (!IsTask(lpOBZ->hTask))
        uRet = OLEUI_BZERR_HTASKINVALID;
#endif
        
    // Error out if our secondary validation failed        
    if (OLEUI_ERR_STANDARDMIN <= uRet)
        {
        if (NULL!=hMemDlg)
            FreeResource(hMemDlg);

        return uRet;
        }

    // Invoke the dialog.
    uRet=UStandardInvocation(BusyDialogProc, (LPOLEUISTANDARD)lpOBZ,
                             hMemDlg, MAKEINTRESOURCE(IDD_BUSY));
#endif

    return uRet;
}


/*
 * BusyDialogProc
 *
 * Purpose:
 *  Implements the OLE Busy dialog as invoked through the OleUIBusy function.
 *
 * Parameters:
 *  Standard
 *
 * Return Value:
 *  Standard
 *
 */

BOOL CALLBACK EXPORT BusyDialogProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
    {
    LPBUSY         lpBZ;
    UINT           uRet = 0;   

    //Declare Win16/Win32 compatible WM_COMMAND parameters.
    COMMANDPARAMS(wID, wCode, hWndMsg);

    //This will fail under WM_INITDIALOG, where we allocate it.
    lpBZ=(LPBUSY)LpvStandardEntry(hDlg, iMsg, wParam, lParam, &uRet);

    //If the hook processed the message, we're done.
    if (0!=uRet)
        return (BOOL)uRet;

    //Process the temination message
    if (iMsg==uMsgEndDialog)
    {
        BusyCleanup(hDlg);
        StandardCleanup(lpBZ, hDlg);
        EndDialog(hDlg, wParam);
        return TRUE;
    }
    
    // Process our special "close" message.  If we get this message,
    // this means that the call got unblocked, so we need to
    // return OLEUI_BZ_CALLUNBLOCKED to our calling app.
    if (iMsg == uMsgCloseBusyDlg)
    {
        SendMessage(hDlg, uMsgEndDialog, OLEUI_BZ_CALLUNBLOCKED, 0L);
        return TRUE;
    }
        
    switch (iMsg)
        {
        case WM_INITDIALOG:
            FBusyInit(hDlg, wParam, lParam);
            return TRUE;

        case WM_COMMAND:
            switch (wID)
                {
                case IDBZ_SWITCHTO:
                
                    // If user selects "Switch To...", switch activation 
                    // directly to the window which is causing the problem.  
                    if (IsWindow(lpBZ->hWndBlocked))
                        MakeWindowActive(lpBZ->hWndBlocked);
                    else                        
                        StartTaskManager(); // Fail safe: Start Task Manager

                    // If this is the app not responding case, then we want
                    // to bring down the dialog when "SwitchTo" is selected.
                    // If the app is busy (RetryRejectedCall situation) then
                    // we do NOT want to bring down the dialog. this is 
                    // the OLE2.0 user model design.
                    if (lpBZ->dwFlags & BZ_NOTRESPONDINGDIALOG)
                        SendMessage(hDlg, uMsgEndDialog, OLEUI_BZ_SWITCHTOSELECTED, 0L);
                    break;

                case IDBZ_RETRY:
                    SendMessage(hDlg, uMsgEndDialog, OLEUI_BZ_RETRYSELECTED, 0L);
                    break;

                case IDCANCEL:
                    SendMessage(hDlg, uMsgEndDialog, OLEUI_CANCEL, 0L);
                    break;
                }
            break;
        }
    return FALSE;
    }


/*
 * FBusyInit
 *
 * Purpose:
 *  WM_INITIDIALOG handler for the Busy dialog box.
 *
 * Parameters:
 *  hDlg            HWND of the dialog
 *  wParam          WPARAM of the message
 *  lParam          LPARAM of the message
 *
 * Return Value:
 *  BOOL            Value to return for WM_INITDIALOG.
 */

BOOL FBusyInit(HWND hDlg, WPARAM wParam, LPARAM lParam)
    {
    LPBUSY           lpBZ;
    LPOLEUIBUSY      lpOBZ;
    HFONT            hFont;
    LPSTR            lpTaskName;
    LPSTR            lpWindowName;
    HICON            hIcon;  

    lpBZ=(LPBUSY)LpvStandardInit(hDlg, sizeof(OLEUIBUSY), TRUE, &hFont);

    // PvStandardInit sent a termination to us already.
    if (NULL==lpBZ)
        return FALSE;
    
    // Our original structure is in lParam    
    lpOBZ = (LPOLEUIBUSY)lParam;          
    
    // Copy it to our instance of the structure (in lpBZ)
    lpBZ->lpOBZ=lpOBZ;

    //Copy other information from lpOBZ that we might modify.
    lpBZ->dwFlags = lpOBZ->dwFlags;

    // Set default information    
    lpBZ->hWndBlocked = NULL;
    
    // Insert HWND of our dialog into the address pointed to by
    // lphWndDialog.  This can be used by the app who called
    // OleUIBusy to bring down the dialog with uMsgCloseBusyDialog
    if (lpOBZ->lphWndDialog && 
        !IsBadWritePtr((VOID FAR *)lpOBZ->lphWndDialog, sizeof(HWND)))
        {
        *lpOBZ->lphWndDialog = hDlg;
        }
    
    // Update text in text box -- 
    // GetTaskInfo will return two pointers, one to the task name
    // (file name) and one to the window name.  We need to call
    // IMallocMemFree on these when we're done with them.  We also
    // get the HWND which is blocked in this call
    //
    // In the case where this call fails, a default message should already
    // be present in the dialog template, so no action is needed
    
    if (GetTaskInfo(hDlg, lpOBZ->hTask, &lpTaskName, &lpWindowName, &lpBZ->hWndBlocked))
        {                   
        // Build string to present to user, place in IDBZ_MESSAGE1 control
        BuildBusyDialogString(hDlg, lpBZ->dwFlags, IDBZ_MESSAGE1, lpTaskName, lpWindowName);
        IMallocMemFree(lpTaskName);
        IMallocMemFree(lpWindowName);
        }
      
    // Update icon with the system "exclamation" icon
    hIcon = LoadIcon(NULL, IDI_EXCLAMATION);
    SendDlgItemMessage(hDlg, IDBZ_ICON, STM_SETICON, (WPARAM)hIcon, 0L);
    
    // Disable/Enable controls
    if ((lpBZ->dwFlags & BZ_DISABLECANCELBUTTON) ||
        (lpBZ->dwFlags & BZ_NOTRESPONDINGDIALOG))              // Disable cancel for "not responding" dialog
        EnableWindow(GetDlgItem(hDlg, IDCANCEL), FALSE);  

    if (lpBZ->dwFlags & BZ_DISABLESWITCHTOBUTTON)
        EnableWindow(GetDlgItem(hDlg, IDBZ_SWITCHTO), FALSE);
        
    if (lpBZ->dwFlags & BZ_DISABLERETRYBUTTON)
        EnableWindow(GetDlgItem(hDlg, IDBZ_RETRY), FALSE);

    // Call the hook with lCustData in lParam
    UStandardHook((LPVOID)lpBZ, hDlg, WM_INITDIALOG, wParam, lpOBZ->lCustData);

    // Update caption if lpszCaption was specified
    if (lpBZ->lpOBZ->lpszCaption && !IsBadReadPtr(lpBZ->lpOBZ->lpszCaption, 1)
          && lpBZ->lpOBZ->lpszCaption[0] != '\0')
        SetWindowText(hDlg, (LPSTR)lpBZ->lpOBZ->lpszCaption);

    return TRUE;
    }


/*
 * BuildBusyDialogString
 *
 * Purpose:
 *  Builds the string that will be displayed in the dialog from the
 *  task name and window name parameters.
 *
 * Parameters:
 *  hDlg            HWND of the dialog     
 *  dwFlags         DWORD containing flags passed into dialog
 *  iControl        Control ID to place the text string
 *  lpTaskName      LPSTR pointing to name of task (e.g. C:\TEST\TEST.EXE)
 *  lpWindowName    LPSTR for name of window
 *
 * Caveats:
 *  The caller of this function MUST de-allocate the lpTaskName and
 *  lpWindowName pointers itself with IMallocMemFree.
 *
 * Return Value:
 *  void
 */

void BuildBusyDialogString(HWND hDlg, DWORD dwFlags, int iControl, LPSTR lpTaskName, LPSTR lpWindowName)
{
    LPSTR       pszT, psz1, psz2, psz3;
    UINT        cch;
    LPSTR       pszDot, pszSlash;
    UINT        uiStringNum;
    
    /*
     * We need scratch memory for loading the stringtable string, 
     * the task name, and constructing the final string.  We therefore 
     * allocate three buffers as large as the maximum message 
     * length (512) plus the object type, guaranteeing that we have enough
     * in all cases.
     */
    cch=512;
    
    // Use OLE-supplied allocation
    if ((pszT = IMallocMemAlloc((DWORD)(3*cch))) == NULL)
        return;
             
    psz1=pszT;
    psz2=psz1+cch;
    psz3=psz2+cch;
    
    // Parse base name out of path name, use psz2 for the task
    // name to display
    
    _fstrcpy(psz2, lpTaskName);
    pszDot = _fstrrchr(psz2, '.');
    if (pszDot != NULL)
      *pszDot = '\0'; // Null terminate at the DOT

    pszSlash = _fstrrchr(psz2, '\\'); // Find last backslash in path
    if (pszSlash != NULL)
      psz2 = pszSlash + 1; // Nuke everything up to this point
    
#ifdef LOWERCASE_NAME      
    // Compile this with /DLOWERCASE_NAME if you want the lower-case
    // module name to be displayed in the dialog rather than the
    // all-caps name.
    {
    int i,l;
    
    // Now, lowercase all letters except first one
    l = _fstrlen(psz2);
    for(i=0;i<l;i++)
      psz2[i] = tolower(psz2[i]);
      
    psz2[0] = toupper(psz2[0]);  
    }
#endif
    
    // Check size of lpWindowName.  We can reasonably fit about 80 
    // characters into the text control, so truncate more than 80 chars
    if (_fstrlen(lpWindowName) > 80)
      lpWindowName[80] = '\0';
      
    // Load the format string out of stringtable, choose a different
    // string depending on what flags are passed in to the dialog
    if (dwFlags & BZ_NOTRESPONDINGDIALOG)
        uiStringNum = IDS_BZRESULTTEXTNOTRESPONDING;
    else
        uiStringNum = IDS_BZRESULTTEXTBUSY;

    if (LoadString(ghInst, uiStringNum, psz1, cch) == 0)
      return;

    // Build the string. The format string looks like this:
    // "This action cannot be completed because the '%s' application 
    // (%s) is [busy | not responding]. Choose \"Switch To\" to activate '%s' and 
    // correct the problem."
    
    wsprintf(psz3, psz1, (LPSTR)psz2, (LPSTR)lpWindowName, (LPSTR)psz2);
    SetDlgItemText(hDlg, iControl, (LPSTR)psz3);
    IMallocMemFree(pszT);
    
    return;
}



/*
 * BusyCleanup
 *
 * Purpose:
 *  Performs busy-specific cleanup before termination.
 *
 * Parameters:
 *  hDlg            HWND of the dialog box so we can access controls.
 *
 * Return Value:
 *  None
 */
void BusyCleanup(HWND hDlg)
{
   return;
}



/*
 * GetTaskInfo()
 *
 * Purpose:  Gets information about the specified task and places the
 * module name, window name and top-level HWND for the task in the specified 
 * pointers
 *
 * NOTE: The two string pointers allocated in this routine are
 * the responsibility of the CALLER to de-allocate.
 *
 * Parameters:
 *    hWnd             HWND who called this function
 *    htask            HTASK which we want to find out more info about
 *    lplpszTaskName   Location that the module name is returned
 *    lplpszWindowName Location where the window name is returned
 *
 */
    
BOOL GetTaskInfo(HWND hWnd, HTASK htask, LPSTR FAR* lplpszTaskName, LPSTR FAR*lplpszWindowName, HWND FAR*lphWnd)
{
    BOOL        fRet = FALSE;
#if !defined( WIN32 )
    TASKENTRY   te;
#endif
    HWND        hwndNext;
    LPSTR       lpszTN = NULL;
    LPSTR       lpszWN = NULL;
    HWND        hwndFind = NULL;
                   
    // Clear out return values in case of error                   
    *lplpszTaskName = NULL;
    *lplpszWindowName = NULL;
                   
#if !defined( WIN32 )
    te.dwSize = sizeof(TASKENTRY);                   
    if (TaskFindHandle(&te, htask))
#endif
        {
        // Now, enumerate top-level windows in system
        hwndNext = GetWindow(hWnd, GW_HWNDFIRST);
        while (hwndNext)
            {
            // See if we can find a non-owned top level window whose
            // hInstance matches the one we just got passed.  If we find one,
            // we can be fairly certain that this is the top-level window for
            // the task which is blocked.
            //
            // REVIEW:  Will this filter hold true for InProcServer DLL-created
            // windows?
            //
            if ((hwndNext != hWnd) &&
#if !defined( WIN32 )
                (GetWindowWord(hwndNext, GWW_HINSTANCE) == (WORD)te.hInst) &&
#else                     
                (GetWindowThreadProcessId(hwndNext,NULL) == htask) &&
#endif                     
				(IsWindowVisible(hwndNext)) &&
                !GetWindow(hwndNext, GW_OWNER))
                {
                // We found our window!  Alloc space for new strings
                if ((lpszTN = IMallocMemAlloc(OLEUI_CCHPATHMAX)) == NULL)
                    return TRUE;  // continue task window enumeration
                   
                if ((lpszWN = IMallocMemAlloc(OLEUI_CCHPATHMAX)) == NULL)
                    return TRUE;  // continue task window enumeration
                    
                // We found the window we were looking for, copy info to
                // local vars
                GetWindowText(hwndNext, lpszWN, OLEUI_CCHPATHMAX);
#if !defined( WIN32 )
                 lstrcpyn(lpszTN, te.szModule, OLEUI_CCHPATHMAX);
#else                     
                /* WIN32 NOTE: we are not able to get a module name
                **    given a thread process id on WIN32. the best we
                **    can do is use the window title as the module/app
                **    name.
                */
                 lstrcpyn(lpszTN, lpszWN, OLEUI_CCHPATHMAX);
#endif                     
                hwndFind = hwndNext;
                
                fRet = TRUE;
                goto OKDone;
                }
                
            hwndNext = GetWindow(hwndNext, GW_HWNDNEXT);
            }
        }

OKDone:

    // OK, everything was successful. Set string pointers to point to
    // our data.
    
    *lplpszTaskName = lpszTN;
    *lplpszWindowName = lpszWN;
    *lphWnd = hwndFind;
    
    return fRet;
}


/*
 * IMallocMemAlloc()
 *
 * Purpose: Allocates dwSize bytes of memory using the OLE-supplied
 * memory allocation.
 *
 * Parameters:
 *    dwSize           Size of memory requested
 *
 * Returns:
 *    void FAR *       Pointer to new memory, or NULL if failed
 *
 */
 
void FAR * IMallocMemAlloc(DWORD dwSize)
{
  LPMALLOC pIMalloc;
  LPSTR pszT;
  
  // Use OLE-supplied allocation
  if (CoGetMalloc(MEMCTX_TASK, &pIMalloc) != NOERROR)
     return NULL;

  pszT=(LPSTR)pIMalloc->lpVtbl->Alloc(pIMalloc, dwSize);
  pIMalloc->lpVtbl->Release(pIMalloc);
  return pszT;
}    


/*
 * IMallocMemFree()
 *
 * Purpose: Frees memory allocated with IMallocMemAlloc()
 *
 * Parameters:
 *    ptr              Pointer to memory to free
 *
 * Returns:
 *    SCODE            Return code from CoGetMalloc
 */
    
SCODE IMallocMemFree(void FAR *ptr)
{
  SCODE sc;
  LPMALLOC pIMalloc;
  
  sc = GetScode(CoGetMalloc(MEMCTX_TASK, &pIMalloc));
  if (SUCCEEDED(sc))
    {
    pIMalloc->lpVtbl->Free(pIMalloc, ptr);
    pIMalloc->lpVtbl->Release(pIMalloc);
    }
  return sc;
  
}


/*
 * StartTaskManager()
 *
 * Purpose: Starts Task Manager.  Used to bring up task manager to
 * assist in switching to a given blocked task.
 *
 */
 
StartTaskManager()
{
    WinExec("taskman.exe", SW_SHOW);
    return TRUE;
}    



/*
 * MakeWindowActive()
 *
 * Purpose: Makes specified window the active window.
 *
 */

void MakeWindowActive(HWND hWndSwitchTo)
{
    // Move the new window to the top of the Z-order
    SetWindowPos(hWndSwitchTo, HWND_TOP, 0, 0, 0, 0,
              SWP_NOSIZE | SWP_NOMOVE);

    // If it's iconic, we need to restore it.
    if (IsIconic(hWndSwitchTo))
        ShowWindow(hWndSwitchTo, SW_RESTORE);
}
