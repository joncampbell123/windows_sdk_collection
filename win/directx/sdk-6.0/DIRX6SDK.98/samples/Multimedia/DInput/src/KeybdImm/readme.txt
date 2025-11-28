//-----------------------------------------------------------------------------
// 
// Sample Name: KeybdImm Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  The KeybdImm program obtains and displays keyboard data.

Path
====
  Source: Mssdk\Samples\Multimedia\DInput\Src\KeybdImm

  Executable: Mssdk\Samples\Multimedia\DInput\Bin

User's Guide
============
  Hold down one or more keys and the index value of each key (see Keyboard 
  Device Constants) is shown.

Programming Notes
=================
  This sample illustrates how an application can use DirectInput to obtain 
  immediate keyboard data. Approximately 30 times per second the application 
  calls IDirectInputDevice::GetDeviceState and displays a string containing the 
  values of all the keys that are down.

