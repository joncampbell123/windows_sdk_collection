Basic Editor


The EDITSDI sample shows a generic windows application with basic editor 
functionality. This sample is similar to Notepad in that it uses an edit 
control as the input control for storing the text.

This sample is based on the DIALOG sample.

Module Map
----------

Dispatch - Message dispatching routines.
WinMain  - Calls initialization functions and processes the message loop.
Editsdi  - Window procedure for the main application window.
Init     - Performs application and instance-specific initialization.
About    - Defines a standard About dialog box.
Misc     - Defines the application-specific commands not related to
           a specific module.
Filedlg  - Shows basic use of Open and Save As common dialogs.
Finddlg  - Shows the use of the find and replace common dialogs.
Optdlg   - Shows the use of the fonts and colors common dialogs.
Printdlg - Shows the use of the Print and Print Setup common dialogs.
File     - File manipulation functions.
Print    - Application-specific printing commands.
Search   - Find and replace dialog support.
