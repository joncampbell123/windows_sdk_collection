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

Windows 95 VMOUSE and Mouse Minidrivers
Minidriver Sample Source and Instructions
-----------------------------------------
VMOUSE is the Win95 internal mouse driver.  It positions 
Windows' graphical pointer, and supports the standard Int33h 
mouse services for MS-DOS applications.  VMOUSE contains 
built in support for the following devices:

	Microsoft compatible Serial Mouse on COM1 - COM4
	PS/2 compatible Mouse (aka "Mouse port")
	Microsoft Bus Mouse
	Microsoft InPort Mouse
	Microsoft BallPoint Mouse, Serial and PS/2

In order to support additional pointing devices and extended 
software features for 3rd party products, VMOUSE provides 
services for "minidriver" VxDs.  Minidrivers can perform
any or all of the following functions:

1) Process mouse data received by VMOUSE from the devices
listed above, to add sensitivity, acceleration, etc.

2) Add support for devices not internal to VMOUSE.

3) Add support for extended Int33h services.


Writing a Minidriver
--------------------
Be sure to read the Mouse section of the documents
included in the Win95 DDK.

Use the sample serial mouse minidriver (SERMOU.VXD) 
in this kit to add support for your mouse devices, extended 
s/w features, and Int33h functions.  This minidriver can 
be built using the tools in the Win95 DDK.  

SERMOU.VXD writes directly to serial mice on COM1 - COM4.
This serial mouse code is provided only as an example of how
to input and process mouse data.  YOU SHOULD REMOVE THE
SERIAL INPUT CODE UNLESS YOU WANT YOUR MINIDRIVER TO DO
ITS OWN SERIAL MOUSE HANDLING!

If you are adding support for additional devices (e.g. 
PS/2, bus adapters, etc.) your  minidriver should detect
the mouse during the VxD's Device_Init time.  

If a mouse is detected, virtualize the IRQ and call VMOUSE 
via VMD_Set_Mouse_Data to inform it about the type of the 
mouse etc. This information is used by VMOUSE to reply to 
various Int33h functions.

When a mouse state change is detected by your IRQ handler, 
prepare a packet in the following form:

 AL = 0 0 b1 b3 b2 b4 0 0 where bx = 1 if button x down.
 ESI = Delta X movement (as read from the mouse, no translation)
 EDI = delta Y movement (as read from the mouse, no translation)

Then, call VMOUSE via VMD_Post_Pointer_Message.  VMOUSE will do 
scaling, conversion to pixels, etc., and call the appropriate 
application hooks.

If you want to provide special processing of mouse data, then hook
VMD_Manipulate_Pointer_Message by calling Hook_Device_Service. 
It will be called by VMD with the same parameters as above. The 
hook procedure returns with cooked parameters as follows:

 AL = x 0 b1 b3 b2 b4 0 0 where bx = 1 iff button x is down and
			  x = 1 if buttons have been mapped.
 ESI = delta X cooked
 EDI = delta Y cooked

The hook procedure can destroy all registers except EDX.

If you want to provide additional int 33h services, you need 
to call Hook_V86_Int_Chain. VMOUSE provides int 33h services till 
AX = 42.  If you need to know some of the data used by the services 
lower than 42, then let us know. We will design an interface which 
lets your minidriver access this data from VMD.

