/*****************************************************************************\
*                                                                             *
*    Layers.h                                                                 *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#ifndef __LAYERS__
#define __LAYERS__


#ifdef		__cplusplus
extern "C" {
#endif


#ifdef _MSC_VER
#include <macos\Windows.h>
#else
#include <Windows.h>
#endif

#if defined(_MSC_VER) || defined(__powerc)

OSErr __pascal		NewLayer(WindowPtr* layerRef, Boolean visible, Boolean neverActive, WindowPtr behind, long refCon);
WindowPtr __pascal	GetLayer(void);
OSErr __pascal		SetLayer(WindowPtr layer);
WindowPtr __pascal	SwapLayer(WindowPtr layer);

#else

pascal OSErr		NewLayer(WindowPtr* layerRef, Boolean visible, Boolean neverActive, WindowPtr behind, long refCon) = { 0x7001, 0xa829 };
pascal WindowPtr	GetLayer(void) = { 0x7003, 0xa829 };
pascal OSErr		SetLayer(WindowPtr layer) = { 0x7004, 0xa829 };
pascal WindowPtr	SwapLayer(WindowPtr layer) = { 0x7005, 0xa829 };

#endif


#ifdef		__cplusplus
}
#endif


#endif
