Generic Windows Application with a Toolbar and Status Bar


The BARSDI sample demonstrates the use of  status bar and tool bar controls 
in their simplest forms. These controls are attached to the main window 
(typically) of an application to give users a visual indication of menu items 
and other relevant pieces of information.

This sample is based on the GEN32 sample. It is a 32-bit only sample.

Module Map
----------

Dispatch - Message dispatching routines.
WinMain  - Calls initialization functions and processes the message loop.
BarSDI   - Window procedure for the main application window.
Init     - Performs application and instance-specific initialization.
About    - Defines a standard About dialog box.
Misc     - Defines the application-specific commands not related to
           a specific module.
ToolBar  - Creates the tool bar and processes ToolTips notifications.
StatBar  - Creates and manages the status bar.
