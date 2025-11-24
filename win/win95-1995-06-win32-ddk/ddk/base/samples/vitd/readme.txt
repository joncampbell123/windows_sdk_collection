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

VITD.VXD

This VxD provides a simulation of a hardware interval timer for virtual
machines. To install it, build VITD.TXT, place the final VITD.VXD in
the Windows SYSTEM directory, and add a "device=VITD.VXD" line in the 
[386enh] section of the SYSTEM.INI and restart Windows.

The accompanying sample applications and header files provide documentation
on using the virtual interval timer.
