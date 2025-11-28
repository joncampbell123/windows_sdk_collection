//-----------------------------------------------------------------------------
// 
// Sample Name: Scrawl Sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  The Scrawl application demonstrates use of the mouse in nonexclusive mode in a 
  windowed application.

Path
====
  Source:     DXSDK\Samples\C#\DirectInput\Scrawl
  Executable: DXSDK\Samples\C#\DirectInput\Bin

User's Guide
============

  To scrawl, hold down the left button and move the mouse. Click the right 
  mouse button to invoke a pop-up menu. From the pop-up menu you can clear the 
  client window, set the mouse sensitivity, or close the application.

Programming Notes
=================
  The Scrawl application demonstrates many aspects of DirectInput programming, 
  including the following:
        Using the mouse in nonexclusive mode in a windowed application.

        Releasing the mouse when Windows needs to use it for menu access.

        Reacquiring the mouse when Windows no longer needs it.

        Reading buffered device data.

        Deferring screen updates till movement on both axes has been fully 
  processed.

        Event notifications of device activity.

        Scaling raw mouse coordinates before using them.

        Using relative axis mode.