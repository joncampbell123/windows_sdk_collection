//-----------------------------------------------------------------------------
// 
// Sample Name: DS3DView Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  The DS3DView program demonstrates the integration of 3-D graphics and 3-D 
  sound, using Direct3D and DirectSound.

Path
====
  Source: \Mssdk\Samples\Multimedia\DSound\Src\DS3DView

  Executable: \Mssdk\Samples\Multimedia\DSound\Bin

User's Guide
============
  To display an object, choose Add 3D Visual from the File menu. More than one 
  object can be added. You can find .x files containing the definitions of 3-D 
  objects in the Media directories for the D3DIM and D3DRM samples.

  Attach a sound to an object by clicking on the object, selecting Attach Sound 
  from the Sound menu, and then loading a wave file. You can attach one sound 
  to each object. To select a sound after it is loaded, click on the object to 
  which the sound is attached.

  After you select a sound, you can hear it by choosing Play Sound Once or Play 
  Sound Looping from the Sound menu. To hear some of the effects of 
  DirectSound, try selecting Orbit Selected Object or Bullet Selected Object 
  from the Motion menu. You can vary the speed of the object by changing the 
  value in the dialog box that appears.

  You can experiment with the 3-D properties of a sound by selecting it and 
  then choosing Selected Sound Properties from the Sound menu. Choosing Global 
  Properties allows you to modify parameters of the listener object.

Programming Notes
=================
  In addition to 3-D retained mode techniques, this program illustrates how to 
  load wave files, attach them to objects in a 3-D environment, and change the 
  parameters of 3-D sound buffers.

