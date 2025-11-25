/*==========================================================================
 *
 *  Copyright (c) 1995 - 1997  Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       ddmm.cpp
 *  Content:    Routines for using DirectDraw on a multimonitor system
 *
 ***************************************************************************/

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

// DDRAW.H might not include these
#ifndef DDENUM_ATTACHEDSECONDARYDEVICES
#define DDENUM_ATTACHEDSECONDARYDEVICES     0x00000001L
#endif
#ifndef LPDDENUMCALLBACKEXA
    typedef BOOL (FAR PASCAL * LPDDENUMCALLBACKEXA)(GUID FAR *, LPSTR, LPSTR, LPVOID, HANDLE);	// wants HMONITOR
    typedef HRESULT (WINAPI * LPDIRECTDRAWENUMERATEEXA)( LPDDENUMCALLBACKEXA lpCallback, LPVOID lpContext, DWORD dwFlags);
#endif

typedef HRESULT (*PDRAWCREATE)(IID *,LPDIRECTDRAW *,LPUNKNOWN);
typedef HRESULT (*PDRAWENUM)(LPDDENUMCALLBACKA, LPVOID);

IDirectDraw * DirectDrawCreateFromDevice(LPSTR, PDRAWCREATE, PDRAWENUM);
IDirectDraw * DirectDrawCreateFromDeviceEx(LPSTR, PDRAWCREATE, LPDIRECTDRAWENUMERATEEXA);

#ifdef __cplusplus
}
#endif	/* __cplusplus */
