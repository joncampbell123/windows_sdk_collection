Basic MDI Application


The BLANDMDI sample shows a basic Multiple Document Interface (MDI) 
application.

This sample is based on the GENERIC sample.

Module Map
----------

Dispatch - Message dispatching routines.
WinMain  - Calls initialization functions and processes the message loop.
BlandMDI - Window procedure for the main application window.
Init     - Performs application and instance-specific initialization.
About    - Defines a standard About dialog box.
Misc     - Defines the application-specific commands not related to
           a specific module.
MDIChild - Creates the MDI client window and the MDI Children.
           Contains the window procedure for the MDI children.
