Manipulating Menus


The MENU95 sample shows several ways of using and manipulating menus, 
including handling menu commands from menu bars & pop menus, inserting 
menus on the fly, modifying menus, and implementing owner draw.

NOTE: This sample demonstrates the new menu functions, but as direct 
translation of the 16-bit Windows menu functions. Therefore, this sample 
does not use these new functions efficiently. 

This sample is based on the INPUT sample. It is a 32-bit only sample.

Module Map
----------

Dispatch - Message dispatching routines.
WinMain  - Calls initialization functions and processes the message loop.
Menu     - Window procedure for the main application window.
Init     - Performs application and instance-specific initialization.
Menu     - Basic menu support: command handling, item checking,
           bitmap check-marks, and on the fly menu insertion.
OwnrDraw - Owner draw menu handling & on the fly menu modification.
Popup    - Implements a popup menu.
