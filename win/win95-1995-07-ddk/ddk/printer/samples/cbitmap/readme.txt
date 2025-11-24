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

This driver creates a 24 bit color DIBs of the print job.

UNIDRV.DLL uses the DIBENG.DLL to render the image in 24 bit color.

The CBITMAP.DRV takes an application's "print out" and stores it in 
device-independent format (DIB).  The structure of a DIB is as 
follows:

===============================
=  BITMAPFILEHEADER           =
=                             =
=  BITMAPINFOHEADER           =
=                             =
=  DIB's color table (if one) =
=                             =
=  DIB's image                =
===============================

The CBITMAP.DRV is very simplistic.  The CBITMAP.DRV is intended to be a 
starting point for color fax/bitmap drivers that want to use UNIDRV.DLL.  
The CBITMAP.DRV is installed as a normal printer driver would be.

The source in CBITMAP.C demonstrates the use of UNIDRV's Dump() callback
function. Basically, UNIDRV will write all of its printer escape 
sequences (specified as escape 0 in RC file) to the nul file.  The
CBITMAP.DRV doesn't want these escape sequences to be written to the DIB
file.  So, the CBITMAP.DRV intercepts the raster output from UNIDRV 
through the use of fnDump() in CBITMAP.C.

NOTE: You may have to add the BITMAPFILEHEADER declaration to your
GDIDEFS.INC (or some other include file) if the compiler doesn't
recognize it.
