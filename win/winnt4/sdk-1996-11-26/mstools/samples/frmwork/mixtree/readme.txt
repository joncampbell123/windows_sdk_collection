Using the TreeView control and Mixer Services


The MIXTREE sample demonstrates how to use a TreeView control and the mixer 
services. If a mixer device is present, then the TreeView control and text 
displays reflect actual components of the mixer device. If no mixer device is 
present, then simulation data will be used to generate the TreeView control 
and text displays.

This sample is based on the INITREE sample. It is a 32-bit only sample.

Module Map
----------

Dispatch - Message dispatching routines.
WinMain  - Calls initialization functions and processes
           the message loop.
MixTree  - Windows procedure for the main application window,
           providing message and command handlers.
Init     - Performs application and instance-specific initialization.
About    - Defines a standard About dialog box.
Misc     - Defines the application-specific commands
           not related to a specific module.
MixLine  - Enumerate mixer lines and controls, add them to the
           TreeView control, and modify its state.
MixInfo  - Obtain information about mixer lines and controls.
ItemInfo - Obtain and display information associated with TreeView items.
