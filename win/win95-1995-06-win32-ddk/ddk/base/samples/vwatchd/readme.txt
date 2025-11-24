*****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
*****************************************************************************

VWATCHD.VXD

VwatchD is a Virtual Device that demonstrates the basic structure of
VxD. TEST.EXE contains the DOS application that calls the VwatchD
V86 API entry.

To Install VwatchD.VXD, copy the file VwatchD.VXD to Windows\system 
directory. Specify "device=VwatchD.VXD" in system.ini [386enh]
section. 

A debug terminal needs to be setup to see the debugging output
by VwatchD. Also DOS386.EXE debug version should be running. 
