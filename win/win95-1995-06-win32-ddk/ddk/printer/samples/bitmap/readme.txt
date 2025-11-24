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

The BITMAP.DRV takes an application's "print out" and stores it in 
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

The BITMAP.DRV is very simplistic (no extensive error checking, keeps
the whole "print out" in a global allocated buffer before writing to 
disk).  The BITMAP.DRV is intended to be a starting point for 
fax/bitmap drivers that want to use UNIDRV.DLL.  The BITMAP.DRV is 
installed as a normal printer driver would be.

The source in BITMAP.C demonstrates the use of UNIDRV's Dump() callback
function. Basically, UNIDRV will write all of its printer escape 
sequences (specified as escape 0 in RC file) to the nul file.  The
BITMAP.DRV doesn't want these escape sequences to be written to the DIB
file.  So, the BITMAP.DRV intercepts the raster output from UNIDRV 
through the use of fnDump() in BITMAP.C.  BITMAP.DRV takes care of
DWORD aligning the scan lines and opening, writing to, and closing the
DIB file.

The BITMAP.DRV creates the DIB file at STARTDOC time and will write
the image to the file at ENDDOC time and then close the file.

NOTE: You may have to add the BITMAPFILEHEADER declaration to your
GDIDEFS.INC (or some other include file) if the compiler doesn't
recognize it.
