Basic MDI Editor


The WRITEPAD sample is a basic MDI editor with a tool bar and status bar.

This sample is based on the BARMDI sample.

Module Map
----------

Dispatch - Message dispatching routines.
WinMain  - Calls initialization functions and processes the message loop.
BarMDI   - Window procedure for the main application window.
Init     - Performs application and instance-specific initialization.
About    - Defines a standard About dialog box.
Misc     - Defines the application-specific commands not related to
           a specific module.
MDIChild - Creates the MDI client window and the MDI Children. Also it
           contains the window procedure for the MDI children.
ToolBar  - Creates the tool bar and processes ToolTips notifications.
StatBar  - Creates and manages the status bar.
