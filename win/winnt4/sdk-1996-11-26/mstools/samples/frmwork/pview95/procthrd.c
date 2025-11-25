// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993 - 1996  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE:   procthrd.c
//
//  PURPOSE:  Implements process and thread enumeration/manipulation
//            functions.
//
//  FUNCTIONS:
//    InitToolhelp32        - Dynamically resolves addresses of 32-bit
//                            Toolhelp functions
//    RefreshThreadList     - Enumerates and displays active threads
//    RefreshProcessList    - Enumerates and displays active processes
//    GetProcessModule      - Gets information about a specific module in
//                            a specified process
//    KillProcess           - Kills the specified process
//    GetModuleNameFromExe  - Returns a Win16 app/DLL's module name from
//                            its executable file.
//
//
//  COMMENTS:
//

#include <windows.h>
#include <tlhelp32.h>              // Required for ToolHelp32 functions
#include <commctrl.h>
#include "procthrd.h"
#include "listview.h"
#include "globals.h"


//-------------------------------------------------------------------------
// Type definitions for functions pointers used to call 32-bit Toolhelp
// functions.
typedef BOOL (WINAPI *MODULEWALK)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
typedef BOOL (WINAPI *THREADWALK)(HANDLE hSnapshot, LPTHREADENTRY32 lpte);
typedef BOOL (WINAPI *PROCESSWALK)(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
typedef HANDLE (WINAPI *CREATESNAPSHOT)(DWORD dwFlags, DWORD th32ProcessID);


//-------------------------------------------------------------------------
// File-scope globals.  These pointers are declared because we have to
// dynamically link to these funtions.  They are exported only by the
// Windows 95 KERNEL32.DLL.  Explicitly linking to them will make this
// application unloadable on Windows NT and will produce an ugly system
// dialog that isn't nice for end-users to see.

static CREATESNAPSHOT pCreateToolhelp32Snapshot = NULL;
static MODULEWALK  pModule32First  = NULL;
static MODULEWALK  pModule32Next   = NULL;
static PROCESSWALK pProcess32First = NULL;
static PROCESSWALK pProcess32Next  = NULL;
static THREADWALK  pThread32First  = NULL;
static THREADWALK  pThread32Next   = NULL;


//-------------------------------------------------------------------------
// Local function prototypes.

BOOL GetProcessModule (DWORD dwPID, DWORD dwModuleID, LPMODULEENTRY32 lpMe32,
                       DWORD cbMe32);
BOOL GetModuleNameFromExe (LPCSTR szFileName, LPSTR szModuleName, WORD cbLen);


//
//  FUNCTION: InitToolhelp32(void)
//
//  PURPOSE:  Initializes pointers to the 32-bit Toolhelp APIs that are
//            exported only by the Windows 95 KERNEL32.DLL.
//
//  PARAMETERS:
//    None.
//
//  RETURN VALUE:
//    TRUE if addresses of all needed 32-bit Toolhelp functions were
//    retrieved.
//    FALSE if we couldn't get the address of even one needed 32-bit
//    Toolhelp function
//
//  COMMENTS:
//    This function is needed to initialize the function pointers to the
//    32-bit Toolhelp functions because at this time, only the Windows 95
//    KERNEL32.DLL exports them.  If we did not dynamically resolve the
//    addresses of these functions, this application would not even load
//    on anything but Windows 95.
//
//    In the case of this sample, this function is called in InitApplication
//    which will prevent this application from even running if it can't
//    access the necessary functions.
//

BOOL InitToolhelp32 (void)
{
    BOOL   bRet  = FALSE;
    HANDLE hKernel = NULL;


    // Obtain a module handle to KERNEL so that we can get the addresses of
    // the 32-bit Toolhelp functions we need.

    hKernel = GetModuleHandle("KERNEL32.DLL");

    if (hKernel)
    {
        pCreateToolhelp32Snapshot =
          (CREATESNAPSHOT)GetProcAddress(hKernel, "CreateToolhelp32Snapshot");

        pModule32First  = (MODULEWALK)GetProcAddress(hKernel,
                                                      "Module32First");
        pModule32Next   = (MODULEWALK)GetProcAddress(hKernel,
                                                     "Module32Next");

        pProcess32First = (PROCESSWALK)GetProcAddress(hKernel,
                                                      "Process32First");
        pProcess32Next  = (PROCESSWALK)GetProcAddress(hKernel,
                                                      "Process32Next");

        pThread32First  = (THREADWALK)GetProcAddress(hKernel,
                                                     "Thread32First");
        pThread32Next   = (THREADWALK)GetProcAddress(hKernel,
                                                     "Thread32Next");

        // All of our addresses must be non-NULL in order for us to be
        // successful.  If even one of these addresses is NULL, then we
        // must fail because we won't be able to walk one of the lists
        // we need to.
        bRet =  pModule32First && pModule32Next  && pProcess32First &&
                pProcess32Next && pThread32First && pThread32Next &&
                pCreateToolhelp32Snapshot;
    }
    else
        bRet = FALSE;  // Couldn't even get a module handle to KERNEL.

    return bRet;
}


//
//  FUNCTION: RefreshThreadList(HWND, DWORD)
//
//  PURPOSE:  Enumerates and displays the list of threads owned by a
//            specified process.
//
//  PARAMETERS:
//    hListView    - Handle of the listview that lists thread information
//    dwOwnerPID   - ID of process whose threads we will list
//
//  RETURN VALUE:
//    TRUE if the threads were successfully enumerated and listed
//     FALSE if the threads could not be enumerated or listed
//
//  COMMENTS:
//

BOOL RefreshThreadList (HWND hListView, DWORD dwOwnerPID)
{
    HANDLE        hThreadSnap = NULL;
    BOOL          bRet        = FALSE;
    THREADENTRY32 te32        = {0};

    // Take a snapshot of all threads currently in the system.
    hThreadSnap = pCreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == (HANDLE)-1)
        return (FALSE);

    // Clear the current contents of the thread list view (which are now old).
    ListView_DeleteAllItems(g_hwndThread);

    // Size of the THREADENTRY32 structure must be filled out before use.
    te32.dwSize = sizeof(THREADENTRY32);

    // Walk thread snapshot to find all threads of the process we want.
    // If the thread belongs to the process we want, add its information
    // to the display list.
    if (pThread32First(hThreadSnap, &te32))
    {
        do
        {
            if (te32.th32OwnerProcessID == dwOwnerPID)
            {
                TINFO ti;

                ti.tid        = te32.th32ThreadID;
                ti.pidOwner   = te32.th32OwnerProcessID;
                ti.tpDeltaPri = te32.tpDeltaPri;
                ti.tpBasePri  = te32.tpBasePri;

                AddThreadItem(hListView, ti);
            }
        }
        while (pThread32Next(hThreadSnap, &te32));
        bRet = TRUE;
    }
    else
        bRet = FALSE;          // Couldn't walk the list of threads.

    // Don't forget to clean up the snapshot object...
    CloseHandle (hThreadSnap);

    return (bRet);
}


//
//  FUNCTION: RefreshProcessList(HWND)
//
//  PURPOSE:  Enumerates and displays the list of processes.
//
//  PARAMETERS:
//    hListView   - Handle of the listview that lists process information
//
//  RETURN VALUE:
//    TRUE if the processes were successfully enumerated and listed
//     FALSE if the processes could not be enumerated or listed
//
//  COMMENTS:
//     Refreshes the list of processes by making a process snapshot and
//    putting info about each process into the listview control specifed by
//    hListView.

BOOL RefreshProcessList (HWND hListView)
{
    HANDLE         hProcessSnap = NULL;
    BOOL           bRet         = FALSE;
    PROCESSENTRY32 pe32         = {0};

    // Take a snapshot of all processes currently in the system.
    hProcessSnap = pCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == (HANDLE)-1)
        return (FALSE);

    // Clear the current contents of the process list view (which are now old).
    ListView_DeleteAllItems(g_hwndProcess);

    // Size of the PROCESSENTRY32 structure must be filled out before use.
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Walk the snapshot of processes and for each process, get information
    // to display.
    if (pProcess32First(hProcessSnap, &pe32))
    {
        BOOL          bGotModule = FALSE;
        MODULEENTRY32 me32       = {0};
        PINFO         pi         = {0};

        do
        {
            bGotModule = GetProcessModule(pe32.th32ProcessID,
                                          pe32.th32ModuleID,
                                          &me32,
                                          sizeof(MODULEENTRY32));
            if (bGotModule)
            {
                HANDLE hProcess;

                // Get actual priority class
                hProcess = OpenProcess (PROCESS_ALL_ACCESS,
                                        FALSE,
                                        pe32.th32ProcessID);
                pi.dwPriorityClass = GetPriorityClass (hProcess);
                CloseHandle (hProcess);

                // Get process's base priority value
                pi.pcPriClassBase = pe32.pcPriClassBase;
                pi.pid            = pe32.th32ProcessID;
                pi.cntThreads     = pe32.cntThreads;

                lstrcpy(pi.szFullPath, pe32.szExeFile);

                // Test to see if the app is a Win16 or Win32 app.  If the
                // file name returned in the PROCESSENTRY32 and MODULEENTRY32
                // structures are equal, then we have a Win32 app, otherwise,
                // we have a Win16 app.

                if (!lstrcmpi (pe32.szExeFile, me32.szExePath))
                {
                    // Win32 app, use MODULENETRY32 module name
                    lstrcpy(pi.szModName, me32.szModule);
                    pi.uAppType = EXETYPE_32BIT;
                }
                else
                {
                    // Win16 app, get module name out of EXE header of file
                    if (!GetModuleNameFromExe (pe32.szExeFile, pi.szModName,
                                               sizeof(pi.szModName)))
                    {
                        // If we can't get the module name for some reason, at
                        // least put something in the module name.
                        lstrcpy(pi.szModName, me32.szModule);
                    }
                    pi.uAppType = EXETYPE_16BIT;
                }
                AddProcessItem(hListView, pi);
            }
        }
        while (pProcess32Next(hProcessSnap, &pe32));
        bRet = TRUE;
    }
    else
        bRet = FALSE;    // Couldn't walk the list of processes.

    // Don't forget to clean up the snapshot object...
    CloseHandle (hProcessSnap);
    return (bRet);
}


//
//  FUNCTION: GetProcessModule(DWORD, DWORD, LPMODULEENTRY32, DWORD)
//
//  PURPOSE:  Given a Process ID and module ID, return its module information.
//
//  PARAMETERS:
//    dwPID      - ID of process that owns the module we want information
//                 about.
//    dwModuleID - ToolHelp32 ID of the module within the process
//    lpMe32     - Structure to return data about the module we want
//    cbMe32     - Size of the buffer pointed to by lpMe32--to make sure we
//                 don't copy too much data into lpMe32.
//
//  RETURN VALUE:
//    TRUE if it returns information about the specifed module.
//    FALSE if it could not enumerate the modules in the process, or the
//          module is not found in the process.
//
//  COMMENTS:
//

BOOL GetProcessModule (DWORD           dwPID,
                       DWORD           dwModuleID,
                       LPMODULEENTRY32 lpMe32,
                       DWORD           cbMe32)
{
    BOOL          bRet        = FALSE;
    BOOL          bFound      = FALSE;
    HANDLE        hModuleSnap = NULL;
    MODULEENTRY32 me32        = {0};

    // Take a snapshot of all modules in the specified process.
    hModuleSnap = pCreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
    if (hModuleSnap == (HANDLE)-1)
        return (FALSE);

    // Size of the MODULEENTRY32 structure must be initialized before use!
    me32.dwSize = sizeof(MODULEENTRY32);

    // Walk the module list of the process and find the module we are
    // interested in.  Then, copy the information to the buffer pointed to
    // by lpMe32 so that we can return it to the caller.
    if (pModule32First(hModuleSnap, &me32))
    {
        do
        {
            if (me32.th32ModuleID == dwModuleID)
            {
                CopyMemory (lpMe32, &me32, cbMe32);
                bFound = TRUE;
            }
        }
        while (!bFound && pModule32Next(hModuleSnap, &me32));

        bRet = bFound;   // If this sets bRet to FALSE, then dwModuleID
                         // no longer exsists in the specified process.
    }
    else
        bRet = FALSE;    // Couldn't walk module list.

    // Don't forget to clean up the snapshot object...
    CloseHandle (hModuleSnap);

    return (bRet);
}


//
//  FUNCTION: KillProcess(DWORD)
//
//  PURPOSE:  To kills a specified process
//
//  PARAMETERS:
//    dwPID      - Process ID of the process to kill
//
//  RETURN VALUE:
//    TRUE if the specified process was killed
//    FALSE if it wasn't killed.
//
//  COMMENTS:
//    KillProcess does not return until the process is terminated.
//

BOOL KillProcess (DWORD dwPID)
{
    BOOL   bRet     = FALSE;
    HANDLE hProcess = NULL;

    // Open the process to obtain a handle to it.  If we get a handle,
    // try to terminate the process.  Wait until the process is really
     // dead before returning.
    hProcess = OpenProcess (PROCESS_TERMINATE, FALSE, dwPID);
    if (hProcess != NULL)
    {
        bRet = TerminateProcess (hProcess, 0xFFFFFFFF);

        // Don't want to wait if the terminate fails.  If the
        // process is still running and we can't terminate it,
        // then waiting for it would cause this process to stop
        // execution.
         if (bRet)
            WaitForSingleObject(hProcess, 10000);

        CloseHandle (hProcess);
    }
    else
        bRet = FALSE;

    return (bRet);
}


//
//  FUNCTION: GetModuleNameFromExe(LPCSTR, LPSTR, WORD)
//
//  PURPOSE:  Retrieves the module name of a Win16 app or DLL from its
//            excutable file.
//
//  PARAMETERS:
//    szFileName   - Executable file (.EXE or .DLL) from which to retrieve
//                   module name
//    szModuleName - Points to buffer that receives the module name
//    cbLen        - Specifies maximum length of szModuleName including NULL
//
//  RETURN VALUE:
//    TRUE if the module name was succesfully copied into szModuleName.
//    FALSE if it wasn't killed.
//
//  COMMENTS:
//    Works for Win16 New Executable files only.
//

BOOL GetModuleNameFromExe (LPCSTR szFileName, LPSTR szModuleName, WORD cbLen)
{
    BOOL              bResult      = FALSE;
    HANDLE            hFile        = NULL;
    HANDLE            hFileMapping = NULL;
    PIMAGE_OS2_HEADER pNEHdr       = NULL;
    PIMAGE_DOS_HEADER pDosExeHdr   = NULL;

    // Open the file as read-only.  (This file may be opened already by the
    // system if it is an application or DLL that is currently loaded.)
    // Create a read-only file mapping and map a read-only view of the file.
    // If we can't open the file for some reason, then return FALSE.

    hFile = CreateFile(szFileName,
                       GENERIC_READ,
                       FILE_SHARE_READ|FILE_SHARE_WRITE,
                       NULL,
                       OPEN_EXISTING,
                       0,
                       NULL);

    if (hFile == INVALID_HANDLE_VALUE)
        return FALSE;

    hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (hFileMapping == NULL)
    {
        CloseHandle(hFile);
        return FALSE;
    }

    pDosExeHdr = (PIMAGE_DOS_HEADER)MapViewOfFile (hFileMapping,
                                                   FILE_MAP_READ,
                                                   0,
                                                   0,
                                                   0);
    if (!pDosExeHdr)
    {
        CloseHandle(hFileMapping);
        CloseHandle(hFile);
        return FALSE;
    }

    __try
    {
        // Go to the beginning of the NE header.
        pNEHdr =
           (PIMAGE_OS2_HEADER)((LPSTR)pDosExeHdr + pDosExeHdr -> e_lfanew);

        // Check to make sure that the file has DOS and NE EXE headers
        if (pDosExeHdr -> e_magic == IMAGE_DOS_SIGNATURE
            && pNEHdr -> ne_magic == IMAGE_OS2_SIGNATURE)
        {
            lstrcpyn (szModuleName, (LPSTR)pNEHdr + pNEHdr -> ne_restab +1,
                      min((BYTE)*((LPSTR)pNEHdr + pNEHdr -> ne_restab) + 1,
                          cbLen));
            bResult = TRUE;
        }
        else
            bResult = FALSE;

    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        // If an access violation occurs in the try block, we know the file
        // is not a NE file because it is too small to have a NE header, or
        // the offset of the NE header isn't close to being correct.
        bResult = FALSE;
    }

    // Clean up file mapping, all views of file mapping, and close the
    // file.
    UnmapViewOfFile(pDosExeHdr);
    CloseHandle(hFileMapping);
    CloseHandle(hFile);

    return bResult;
}

