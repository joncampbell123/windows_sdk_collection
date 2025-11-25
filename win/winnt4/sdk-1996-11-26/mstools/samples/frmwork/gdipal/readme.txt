Palette Manager


The GDIPAL sample demonstrates the use of the Windows palette manager on 
palette devices. It allows the user to choose pen and brush colors from the 
system palette.

This sample is based on the GDIINPUT sample. It is a 32-bit only sample.

Module Map
----------

Dispatch - Message dispatching routines.
WinMain  - Calls initialization functions and processes the message loop.
GDIPal   - Window procedure for the main application window.
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
BrushDlg - Dialog box for choosing the brush style used for
           filling objects.
ColorDlg - Dialog box for choosing a color from the system palette
           when running GDIOut on a palette-based device.
PalCtrl  - Custom control for displaying and selecting colors from
           the current system palette.
Palette  - Palette manager-related routines such as handlers for
           palette messages.
InfoDlg  - Dialog box for displaying some basic color and palette
           information regarding the display device.
