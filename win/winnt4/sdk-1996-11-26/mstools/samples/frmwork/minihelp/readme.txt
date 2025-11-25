Using the Windows Help Engine


The MINIHELP sample shows how to call the Windows help engine from an 
application. It also shows how to build a help file (.HLP). To build the .HLP 
file, use the MAKEFILE provided.

This sample is based on the GENERIC sample.

Module Map
----------

Dispatch - Message dispatching routines.
WinMain  - Calls initialization functions and processes the message loop.
MiniHelp - Windows procedure for the main application window.
Init     - Performs application and instance-specific initialization.
About    - Defines a standard About dialog box.
Misc     - Defines the application-specific commands not related to
           a specific module.
