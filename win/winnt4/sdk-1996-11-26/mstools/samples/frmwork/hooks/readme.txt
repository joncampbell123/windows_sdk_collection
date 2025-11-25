Simplified Spy Utility


The HOOKS sample works like Spy++. It shows how to implement hooks.

This sample is based on the GEN32, DLLSKEL, BARSDI, and INITREE samples. 
It is a 32-bit only sample.

Module Map
----------

Dispatch - Message dispatching routines.
WinMain  - Calls initialization functions and processes the message loop.
Init     - Performs application and instance-specific initialization.
About    - Defines a standard About dialog box.
Misc     - Defines the application-specific commands not related to
           a specific module.
TreeView - Shows how a TreeView is implemented.
Header   - Shows how a header is implemented.
ToolBar  - Implements the tool bar for the sample
StatBar  - Implements the Status bar.
hooks    - Houses the main routines of the module.
propsht  - Implements property sheets.
split.c  - Has code to maintain and split the main window.
Browse   - Implements the browse dialog.
List     - Implements the list box containing all messages.
DllMain  - Main module for the DLLSKEL.DLL.
Exports  - Contains all exported functions of the DLL.
SysHook  - Installs system-wide hooks.
Syshook2 - Misc. functions used by SysHook.
