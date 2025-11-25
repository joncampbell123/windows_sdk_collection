Using a TreeView and an ImageList


The INITREE sample shows how to create and fill a TreeView control with a 
hierarchical list of items (in this example, the INI files in the Windows 
directory are used). It also shows how to use the ImageList in conjunction 
with the TreeView control. Show how to use the FindFirstFile and 
FindNextFile functions to search a directory.

This sample is based on the GEN32 sample. It is a 32-bit only sample.

Module Map
----------

Dispatch         - Message dispatching routines.
WinMain          - Calls initialization functions and processes the message
                   loop.
Generic          - Window procedure for the main application window.
Init             - Performs application and instance-specific initialization
About            - Defines a standard About dialog box.
Misc             - Defines the application-specific commands 
                   not related to a specific module.
INI_FillTreeView - Entry point function to fill the TreeView control.
FillIniKeys      - Builds a list of sections for each INI file.
FillKeyItems     - Builds a list of entries for each section.
CreateImageList  - Creates an Image List.
CreateTreeView   - Creates a TreeView control.
