//-----------------------------------------------------------------------------
// 
// Sample Name: MDIWindow Sample
// 
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  MDIWindow demonstrates DirectDraw in MDI windows.

Path
====
   Source:     DXSDK\Samples\VB.NET\DirectDraw\MDIWindow
   Executable: DXSDK\Samples\VB.NET\DirectDraw\Bin

User's Guide
============
  Use the OpenWindow command to open a new window.

Programming Notes
=================
You can use DirectDraw to blit to multiple windows created by an application running at 
the normal cooperative level. To do this, create a single DirectDraw object with a primary surface. 
Then, create a Clipper object and assign it to your primary surface by setting the Clipper property.
To draw only the client area of a window, set the clipper to that window’s client area by setting the 
WindowHandle property before drawing to the primary surface. Whenever you need to draw to another window’s 
client area, set the WindowHandle property again with the new target window handle.
 

Creating multiple DirectDraw objects that draw to each others’ primary surface is not recommended. The technique 
described above provides an efficient and reliable way to draw to multiple client areas with a single DirectDraw object.

 

 
