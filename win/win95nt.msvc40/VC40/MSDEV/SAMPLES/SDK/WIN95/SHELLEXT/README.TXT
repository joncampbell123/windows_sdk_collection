SHELLEXT.DLL

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Copyright (C) 1993-1995  Microsoft Corporation.  All Rights Reserved.

PURPOSE:
    Show how to implement the basic shell extensions.  These include
    context menu, property sheet, icon handler, and copy hook extensions.
    This sample does NOT implement a drag-drop handler, but the
    implementation is very similar to context menu handlers.


COMMENTS:
    This is a 32-bit only sample.  To use this sample, compile the DLL
    using VC++ 2.x and put a copy of SHELLEXT.DLL in the SYSTEM directory.
    Finally, double click on the SHELLEXT.REG file from within Explorer.

    Now, hit F5 to have the Explorer refresh itself, and the *.GAK
    files should have new icons.  Additionally, right clicking on a .GAK
    file will show you a modified context menu.  And last, but not least,
    the Properties... menu option will show a new tab for the .GAK icon
    color.  You can set the color here.
