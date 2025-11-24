MIXTREE - Based on IniTree

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Copyright (C) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.


PURPOSE:
    Demonstrate TreeView control and Mixer Services.   

USES:
    Based on IniTree

COMMENTS:
    This is a 32-bit only sample.  If a mixer device is present then the
    TreeView control and text displays will reflect actual components of
    the mixer device.  If no mixer device is present then simulation data
    will be used to generate the TreeView control and text displays.

MODULE MAP:
    Dispatch         - Message dispatching routines
    WinMain          - Calls initialization functions and processes 
                       the message loop
    MixTree          - Implements the windows procedure for the main 
                       application window, providing message and
                       command handlers.
    Init             - Performs application and instance specific 
                       initialization.
    About            - Defines a standard about dialog box.
    Misc             - Defines the applications specific commands 
                       not related to a specific module.    
    MixLine          - Enumerate mixer lines and controls, add them to the 
                       TreeView control, and modify its state.
    MixInfo          - Obtain information about mixer lines and controls.
    ItemInfo         - Obtain and display information associated with TreeView
                       items. 


