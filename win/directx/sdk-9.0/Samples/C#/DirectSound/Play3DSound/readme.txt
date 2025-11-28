//-----------------------------------------------------------------------------
// 
// Sample Name: Play3DSound Sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  The Play3DSound sample shows how to create a 3-D sound buffer and 
  manipulate its properties. It is similar to the 3DAudio sample but does not 
  use an audiopath.

Path
====
  Source:     DXSDK\Samples\C#\DirectSound\Play3DSound
  Executable: DXSDK\Samples\C#\DirectSound\Bin

User's Guide
============
  Click Segment File and load a wave file. Play the segment. The position of 
  the sound source is shown as a red dot on the graph, where the x-axis is 
  from left to right and the z-axis is from bottom to top. Change the range
  of movement on the two axes by using the sliders. 

  The listener is located at the center of the graph, and has its default 
  orientation, looking along the positive z-axis; that is, toward the top of 
  the screen. The sound source moves to the listener's left and right and to 
  the listener's front and rear, but does not move above and below the listener.

  The sliders in the center of the window control the properties of the 
  listener; that is, the global sound properties. If you click Defer 
  Settings, changes are not applied until you click Apply Settings. 

Programming Notes
=================
  For a simpler example of how to setup a DirectSound buffer without a 
  3D positioning, see the PlaySound sample. 
