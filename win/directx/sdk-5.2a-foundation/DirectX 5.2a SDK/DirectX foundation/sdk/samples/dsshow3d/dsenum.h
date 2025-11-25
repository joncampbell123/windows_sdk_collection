/*==========================================================================
 *
 *  Copyright (C) 1995-1997 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       dsenum.h        
 *  Content:    Direct Sound Enumeration defines
 *
 *
 ***************************************************************************/

#ifndef __DSENUM_INCLUDED__
#define __DSENUM_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

// Function declarations

BOOL DoDSoundEnumerate( LPGUID );

BOOL CALLBACK DSEnumProc( LPGUID, LPCTSTR, LPCTSTR, LPVOID );
BOOL CALLBACK DSEnumDlgProc( HWND, UINT, WPARAM, LPARAM );


#ifdef __cplusplus
};
#endif


#endif

