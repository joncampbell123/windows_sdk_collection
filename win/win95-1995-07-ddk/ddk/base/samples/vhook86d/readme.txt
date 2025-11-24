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

VHOOK86D.VXD

This is a simple VxD that will hook the V86 INT chain by using the
Hook_V86_Int_Chain service.  For more information on the
Hook_V86_Int_Chain service, please consult the Virtual Device
Guide of the DDK.

WARNING: This VxD, by default, will hook INT 2fh.
         This VxD may impair system performance.  It is intended
         only to be a demonstration.
