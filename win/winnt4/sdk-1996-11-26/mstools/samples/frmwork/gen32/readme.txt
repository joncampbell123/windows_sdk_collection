Generic Win32-based Application


The GEN32 sample is a generic Win32-based application with a menu and an 
About box, but without any real functionality.

Module Map
----------

Dispatch - Message dispatching routines.
WinMain  - Calls initialization functions and processes the message loop.
Generic  - Window procedure for the main application window.
Init     - Performs application and instance-specific initialization.
About    - Defines a standard About dialog box.
Misc     - Defines the application-specific commands not related to
           a specific module.
