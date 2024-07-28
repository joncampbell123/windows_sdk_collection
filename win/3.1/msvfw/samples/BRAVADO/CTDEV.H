/****************************************************************************
 *
 *   ctdev.h
 * 
 *   Device specific include file 
 *
 *   Microsoft Video for Windows Sample Capture Driver
 *   Chips & Technologies 9001 based frame grabbers.
 *
 ***************************************************************************/
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/


#define MAX_SIZE_X_NTSC       640       // These sizes give square pixels
#define MAX_SIZE_Y_NTSC       480
#define MAX_SIZE_X_PAL        640
#define MAX_SIZE_Y_PAL        512

#define DEFAULT_SIZE_X        160
#define DEFAULT_SIZE_Y        120

#define MAX_COLOR_VALUE       0x3f    // Sets scroll range

#define CT_MAX_HUE            MAX_COLOR_VALUE
#define CT_MAX_SAT            MAX_COLOR_VALUE
#define CT_MAX_BRIGHTNESS     MAX_COLOR_VALUE
#define CT_MAX_CONTRAST       MAX_COLOR_VALUE
#define CT_MAX_RED            MAX_COLOR_VALUE
#define CT_MAX_GREEN          MAX_COLOR_VALUE
#define CT_MAX_BLUE           MAX_COLOR_VALUE
#define CT_MAX_ZOOM           8

#ifdef _BRAVADO
#define CT_DEFAULT_HUE        0
#define CT_DEFAULT_IOBASE     0x224
#endif

#ifdef _VBLASTER
#define CT_DEFAULT_HUE        (MAX_COLOR_VALUE/2)
#define CT_DEFAULT_IOBASE     0x2AD6
#endif

#define CT_DEFAULT_SAT        (MAX_COLOR_VALUE/2)
#define CT_DEFAULT_CONTRAST   (MAX_COLOR_VALUE/2)
#define CT_DEFAULT_BRIGHTNESS (MAX_COLOR_VALUE)
#define CT_DEFAULT_RED        (MAX_COLOR_VALUE/2)
#define CT_DEFAULT_GREEN      (MAX_COLOR_VALUE/2)
#define CT_DEFAULT_BLUE       (MAX_COLOR_VALUE/2)
#define CT_DEFAULT_INPUT      0
#define CT_DEFAULT_STANDARD   0
#define CT_DEFAULT_ZOOM       4

#define CT_SOURCE0      0
#define CT_SOURCE1      1
#define CT_SOURCE2      2


