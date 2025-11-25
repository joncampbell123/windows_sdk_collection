Subclassing Controls


The SUBCLS sample demonstrates how to subclass controls. List box controls 
and edit controls do not notify the user for certain special keys. For 
example, the list box will not notify the parent if the user hits the enter 
key, the INSERT, or the DELETE key. To trap these keys and provide more 
functionality to the controls, one needs to subclass the control and add the 
functionality.

This sample is based on the GENERIC sample.

Module Map
----------

Dispatch - Message dispatching routines.
WinMain  - Calls initialization functions and processes the message loop.
Subcls   - Windows procedure for the main application window.
Init     - Performs application and instance-specific initialization.
About    - Defines a standard About dialog box.
Misc     - Defines the application-specific commands not related to
           a specific module.
Modal    - Implements the modal demo dialog box.
SubCls   - Has all the routines for subclassing the controls.
Dialog   - Defines the application-specific dialog and its commands.
