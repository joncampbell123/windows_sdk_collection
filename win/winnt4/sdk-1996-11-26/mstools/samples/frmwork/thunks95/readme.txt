Thunk Compiler Demonstration


The THUNKS95 sample demonstrates how to use the Windows 95 thunk compiler. 
Use this sample as a starting point for building thunk scripts for use with 
the Windows 95 thunk compiler.

This sample is based on the GEN32 sample. This sample will only run on
Windows 95.

Module Map
----------

The following files implement the application (APP32.EXE):

    Dispatch - Message dispatching routines.
    WinMain  - Calls initialization functions and processes the message loop.
    APP32    - Window procedure for the main application window.
    Init     - Performs application and instance-specific initialization.
    About    - Defines a standard About dialog box.
    Misc     - Defines the application-specific commands not related to
               a specific module.
    MAKEFILE - The application makefile. Also executes the DLL makefile.

The following files implement the DLL (DLL32.DLL):

    DLLMain   - The DLL entry point function.
    Exports   - Contains the exported functions and the call to the 16-bit DLL.
    DLL32.MAK - The DLL makefile.
