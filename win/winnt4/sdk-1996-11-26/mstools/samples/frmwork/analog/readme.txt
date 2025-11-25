Demonstration of Common Controls


The ANALOG sample application demonstrates 4 of the common controls: status 
bar controls, progress bar controls, tracker (slider) controls, and up/down
(spin) controls in a copy machine panel theme.

To see the copier dialog, select Dialog from the main menu, then select 
Copier. The up/down control is used to select the number of copies to be 
made. The track bars are used for enlargement and shading. The status bar 
notifies the user of the selection made, the status of the copier, and the 
progress made when copying is started.

This sample is based on the DIALOG sample.

Module Map
----------

Dispatch - Message dispatching routines.
WinMain  - Calls initialization functions and processes the message loop.
Init     - Performs application and instance-specific initialization.
About    - Defines a standard About dialog box.
Misc     - Defines the application-specific commands not related to
           a specific module.
Analog   - Window procedure for the main application window.
Copier   - Creates and displays Copier dialog box which in turn demonstrates 
           some of the new Windows 95 common controls. The common 
           controls it demonstrates are up/down controls, track bar 
           controls, progress bar controls, and status bar controls.
