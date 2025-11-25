Enhanced Metafiles


The GDIMETA sample demonstrates creating, loading, saving, and editing 
enhanced metafiles

This sample is based on the GDIPAL sample. It is a 32-bit only sample.

Module Map
----------

Dispatch - Message dispatching routines.
WinMain  - Calls initialization functions and processes the message loop.
GDIMeta  - Window procedure for the main application window.
Init     - Performs application and instance-specific initialization.
About    - Defines a standard About dialog box.
Misc     - Defines the application-specific commands not related to
           a specific module.
ToolBar  - Creates the tool bar and processes ToolTips notifications.
StatBar  - Creates and manages the status bar.
Client   - Implements the window procedure for a child window
           that covers the entire client area of the main window except
           for the tool bar and status bar. Performs all drawing.
PenDlg   - Dialog box for choosing the pen style used for drawing.
BrushDlg - Dialog box for choosing the brush style used for filling objects.
