                   Microsoft DirectX 2.0 SDK and Direct 3D SDK

                                DXSDKWAT.ZIP

                      Source and Makefile Modifications
                       for use with Watcom C/C++ 10.6

                          (c) 1996 by Sybase, Inc.

DISCLAIMER: This product is provided by Sybase, Inc. on an "as-is" basis.
You may use and distribute these files in accordance with the Microsoft
DirectX SDK license, provided that you indemnify and hold both
Microsoft Corp. and Sybase, Inc. harmless against any express or implied
liabilities, including loss and damage, arising from the use of this software.

This software has been provided by Sybase, Inc. (formerly Watcom International
Corp.) for modifying the DirectX 2 SDK.


Directions for use:

After installing the Microsoft DirectX SDK, copy the DXSDKWAT.ZIP file to the
C:\DXSDK\SDK directory (assuming that you've installed the DirectX SDK in
C:\DXSDK), and unzip it with the commands:

c:
cd \dxsdk\sdk
pkunzip -d dxsdkwat.zip

When it prompts for overwriting a file, press 'a' to overwrite all of them.
This will overwrite some existing files, and add Watcom makefiles, which
should allow the developer to compile this SDK with either Microsoft Visual
C++ or Watcom C/C++ (10.6 or later).  

If you then cd to the samples directory, and type:

makeallw

This will automatically compile all samples with Watcom C/C++.

Known Problems

FOXBEAR -- This program encounters a runtime error with Watcom C/C++ 10.6,
which causes the program to exit when the routine that superimposes the
graphical resolution is called.

IKLOWNS -- This program includes some inline assembly code that is not
recognized by Watcom C/C++ (in cgbitbuf.cpp).  There is currently no makefile
available for this example.

PALETTE -- During execution, the diagnostic text is not displayed in the
window.

ROCKEM -- Splash screen comes up, but it doesn't get any farther.

TUNNEL -- During execution, the program runs fine for a second, but suddenly
stops and displays small bits of tunnel graphic on the display.

TRIANGLE -- The triangle graphic does not move.

A fix for these problems will be made available through Watcom technical
support at a later date.

If you would like a document which outlines the list of changes that were
made to the SDK, contact Watcom C/C++ Technical Support at (519) 884-0702.

