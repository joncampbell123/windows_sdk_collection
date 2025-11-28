//-----------------------------------------------------------------------------
// 
// Sample Name: MouseExc Sample
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  The MouseExc program demonstrates how to initialize and get immediate data 
  from a DirectInput device.

Path
====
  Source: Mssdk\Samples\Multimedia\DInput\Src\MouseExc

  Executable: Mssdk\Samples\Multimedia\DInput\Bin

User's Guide
============
  Move the mouse around and observe how the change in coordinates is displayed. 
  Hold down a mouse button and its number is shown. Note that the system cursor 
  is not present.

Programming Notes
=================
  This sample illustrates how an application can use DirectInput to obtain 
  relative mouse data in exclusive foreground mode. Approximately 30 times per 
  second the program displays the change in mouse coordinates since the last 
  call to IDirectInputDevice::GetDeviceState. 

