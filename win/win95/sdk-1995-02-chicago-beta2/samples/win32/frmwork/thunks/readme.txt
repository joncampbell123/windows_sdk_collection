APP32 - Based on GENERIC

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Copyright (C) 1993-1995  Microsoft Corporation.  All Rights Reserved.


PURPOSE:
    The whole purpose of this sample is to provide a very simple
    sample demonstrating the use of the thunk compiler under 
    Windows 95.

USES:
    Use this sample as a starting point for building Windows 95
    thunking scripts for use with the thunk compiler.

COMMENTS:
    The 32-bit C/C++ compiler supports two new keywords for exporting
    functions from Win32 DLLs and importing them into applications:

    A DLL exports a function by putting the __declspec(dllexport)
    keyword in front of the function declaration and then using the
    -implib:DLLName.LIB linker command line argument.

    An application can then import the function by declaring it with the
    __declspec(dllimport) keyword and linking to the DLL's import library.

MODULE MAP:
  The following files implement the application (APP32.EXE):

    Dispatch- Message dispatching routines
    WinMain - Calls initialization functions and processes the message loop
    APP32   - Implements the windows procedure for the main application window
    Init    - Performs application and instance specific initialization
    About   - Defines a standard about dialog box.
    Misc    - Defines the applications specific commands not related to
                a specific module.

  The following files implement the DLL (DLL32.DLL):

    DLLMain - The DLL's entry point function
    Exports - Contains the DLL's exported functions and the call to the
              16-bit dll.


    MAKEFILE    - The application's makefile.  Also executes DLL's makefile.
    DLL32.MAK   - The DLL's makefile

