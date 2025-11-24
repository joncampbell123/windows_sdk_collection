PVIEW95 - Based on Gen32

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Copyright (C) 1993-1995  Microsoft Corporation.  All Rights Reserved.


PURPOSE:
    Show how to use to the 32-bit Toolhelp APIs to enumerate Win32 
    processes, threads, and modules in Windows 95.  Displays information 
    about each process and all threads within a process in listview 
    controls.  Finally, shows how to terminate processes.

    IMPORTANT:  This tool DOES NOT RUN ON Windows NT.  It will load, but
                immediately exits after posting a message box because it
                calls the new 32-bit Toolhelp API functions which are
                not implemented in Windows NT 3.1 and 3.5.

USES:
    PVIEW95 is a sample which enumerates processes and threads currently
    executing in the system.  While it has a functional use of allowing
    the user to kill processes and get information about them, it is 
    designed to be an example of how to write similar diagnostic and
    development tools.

    PROCTHRD.C contains the functions which use the new 32-bit ToolHelp
    functions.  These functions are exported by the Windows 95 KERNEL32.DLL,
    but not the Windows NT KERNEL32.DLL.  It also contains a function to
    abruptly terminate a Win32 process.

    LISTVIEW.C contains the functions which manipulate the process and
    thread list view controls.  List view controls were chosen as they
    facilitate adding multiple columns of data which may be sorted by
    clicking on a column header.  
    
INTERFACE:
    PVIEW95 has several non-obvious interface elements.  First, the top 
    list view control lists all active processes, while the bottom
    pane is used to list all threads from a selected process.  You must
    select a process from the list of processes before any data will be
    listed in the thread list.  This is because PVIEW95 only displays
    threads that belong to a single process.

    The column headers in both list view controls can be used for sorting 
    the displayed data.  Simply click on a column header and the data will
    be sorted.

    The toolbar uses Tooltips--leave the mouse cursor over a Toolbar button
    for a short time and a brief description of the button will appear.

    Be careful about terminating processes.  PVIEW95 will allow you to
    kill *any* Win32 process; killing some processes will cause a system
    crash.  PVIEW95 will ask for confirmation before killing any process,
    so you have a second chance before the process will be killed.

COMMENTS:
    PVIEW95 is a sample which enumerates processes and threads currently
    executing in the system.  While it has a functional use of allowing
    the user to kill processes and get information about them, it is 
    designed to be an example of how to write similar diagnostic and
    development tools.  As such, it is not supported as a tool.

    PVIEW95 has several limitations that shouldn't be present in a real
    tool whose purpose is similar.  A few limitations or features not
    implemented in PVIEW95 that a real tool could provide are:

    * Enumerate Win16 processes
    * Enumerate Win16 DLLs
    * Select and unload Win16 DLLs
    * Enumerate all modules within a Win32 process
    * Run on both Windows NT and Windows 95

    This is a 32-bit only sample.

MODULE MAP:
    Dispatch- Message dispatching routines
    WinMain - Calls initialization functions and processes the message loop
    PView95 - Implements the windows procedure for the main application window
    Init    - Performs application and instance specific initialization
    About   - Defines a standard about dialog box.
    Misc    - Defines the applications specific commands not related to
                a specific module.
    ProcThrd- Implements process, thread, module enumeration and
                manipulation functions
    Listview- Implements the process and thread listview controls
    Toolbar - Implements the Toolbar and Tooltips
