//////////////////////////////////////////////////////////////////////////
//	AVDEV.H								//
//									//
//	Device specific include file for AVCAPT.DRV.			//
//									//
//////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1994 - 1995 Microsoft Corporation.	All Rights Reserved.
//
//---------------------------------------------------------------------------

#define MAX_SIZE_X_NTSC		640	// These sizes give square pixels
#define MAX_SIZE_Y_NTSC		480
#define MAX_SIZE_X_PAL		640
#define MAX_SIZE_Y_PAL		512

#define DEFAULT_SIZE_X		160
#define DEFAULT_SIZE_Y		120

#define MAX_COLOR_VALUE		0x3f    // Sets scroll range

#define HW_MAX_HUE		MAX_COLOR_VALUE
#define HW_MAX_SAT		MAX_COLOR_VALUE
#define HW_MAX_BRIGHTNESS	MAX_COLOR_VALUE
#define HW_MAX_CONTRAST		MAX_COLOR_VALUE
#define HW_MAX_RED		MAX_COLOR_VALUE
#define HW_MAX_GREEN		MAX_COLOR_VALUE
#define HW_MAX_BLUE		MAX_COLOR_VALUE
#define HW_MAX_ZOOM		8

#define HW_DEFAULT_HUE		(MAX_COLOR_VALUE/2)
#define HW_DEFAULT_IOBASE	0x2AD6

#define HW_DEFAULT_SAT		(MAX_COLOR_VALUE/2)
#define HW_DEFAULT_CONTRAST	(MAX_COLOR_VALUE/2)
#define HW_DEFAULT_BRIGHTNESS	(MAX_COLOR_VALUE)
#define HW_DEFAULT_RED		(MAX_COLOR_VALUE/2)
#define HW_DEFAULT_GREEN	(MAX_COLOR_VALUE/2)
#define HW_DEFAULT_BLUE		(MAX_COLOR_VALUE/2)
#define HW_DEFAULT_INPUT	0
#define HW_DEFAULT_STANDARD	0
#define HW_DEFAULT_ZOOM		4

#define HW_SOURCE0		0
#define HW_SOURCE1		1
#define HW_SOURCE2		2
