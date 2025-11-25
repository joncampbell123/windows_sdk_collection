Demonstrates the CreateDIBSection Function


The GDIDIB sample demonstrates bitmap creation, editing, storage and retrieval.

This sample is based on the GDIPAL sample. It is a 32-bit only sample.

Module Map
----------

Dispatch - Message dispatching routines.
WinMain  - Calls initialization functions and processes the message loop.
GDIDIB   - Window procedure for the main application window.
Init     - Performs application and instance-specific initialization
About    - Defines a standard About dialog box.
Misc     - Defines the application-specific commands not related to
           a specific module.
ToolBar  - Creates the tool bar and processes ToolTips notifications.
StatBar  - Creates and manages the status bar.
Client   - Implements the window procedure for a child window
           that displays either an initialized bitmap or one loaded
           from disk file, and performs all drawing on the bitmap.
PenDlg   - Dialog box for choosing the pen style used for drawing.
BrushDlg - Dialog box for choosing the brush style used for filling objects.
ColorDlg - Dialog box for choosing a color from the system palette
           when running GDIDIB on a palette-based device.
PalCtrl  - Custom control for displaying and selecting colors from
           the current system palette.
Palette  - Palette manager-related routines such as handlers for
           palette messages.
InfoDlg  - Dialog box for displaying some basic color and palette
           information regarding the display device.
DIBUtil  - Contains various routines for manipulating DIBs.
FileDlg  - Implements dialogs for File Save, Save As and Close.
FileIO   - Implements code for reading and writing DIB files.
FileNew  - Implements code for creating a new DIB.
