/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

//
// comm.h: Declares data, defines and struct types for common code
//            module.
//
//

#ifndef __COMM_H__
#define __COMM_H__


#ifdef DEBUG
#define INLINE
#define DEBUG_CODE(x)   x
#else
#define INLINE          __inline
#define DEBUG_CODE(x)   
#endif

// Return the number of elements in an array
//
#define ARRAYSIZE(a)    (sizeof(a)/sizeof(a[0]))

// Copy chunk of memory
//
#define BltByte(pdest, psrc, cb)    hmemcpy((LPVOID)pdest, (LPVOID)psrc, cb)

// General flag macros
//
#define SetFlag(obj, f)             do {obj |= (f);} while (0)
#define ToggleFlag(obj, f)          do {obj ^= (f);} while (0)
#define ClearFlag(obj, f)           do {obj &= ~(f);} while (0)
#define IsFlagSet(obj, f)           (BOOL)(((obj) & (f)) == (f))  
#define IsFlagClear(obj, f)         (BOOL)(((obj) & (f)) != (f))  

// Model independent, language-independent (DBCS aware) macros
#define IsSzEqual(sz1, sz2)         (BOOL)(lstrcmpi(sz1, sz2) == 0)
#define IsCaseSzEqual(sz1, sz2)     (BOOL)(lstrcmp(sz1, sz2) == 0)

#ifndef DBCS
#define AnsiNext(x)         ((x)+1)
#define AnsiPrev(y,x)       ((x)-1)
#define IsDBCSLeadByte(x)   ((x), FALSE)
#endif


typedef struct tagPORTPAIR
    {
    char szPortName[LINE_LEN];
    char szFriendlyName[LINE_LEN];
    } PORTPAIR, FAR * LPPORTPAIR;

// This structure is a table mapping portnames to friendly names
typedef struct tagPORTMAP
    {
    LPPORTPAIR  rgports;    // Alloc
    int         cports;
    } PORTMAP, FAR * LPPORTMAP;

BOOL    PUBLIC PortMap_Create(LPPORTMAP FAR * ppmap);
LPCSTR  PUBLIC PortMap_GetFriendly(LPPORTMAP pmap, LPCSTR pszPortName);
LPCSTR  PUBLIC PortMap_GetPortName(LPPORTMAP pmap, LPCSTR pszFriendlyName);
void    PUBLIC PortMap_Free(LPPORTMAP pmap);

typedef BOOL (WINAPI FAR * ENUMDEVICEPROC)(HKEY hkeyDevice, HKEY hkeyDriver, LPARAM lParam);

#define FindDevice(pszClass, pszFriendly, phkeyDev, phkeyDrv)   \
        FindDeviceByString(pszClass, c_szFriendlyName, pszFriendly, phkeyDev, phkeyDrv)

BOOL    PUBLIC EnumerateDevices(LPCSTR pszClass, ENUMDEVICEPROC pfnDevice, LPARAM lParam);
BOOL    PUBLIC FindDeviceByString(LPCSTR pszClass, LPCSTR pszValueName, LPCSTR pszValue, HKEY FAR * phKeyDev, HKEY FAR * phKeyDrv);

BOOL    PUBLIC MyLocalReAlloc(LPVOID FAR * ppv, int cbOld, int cbNew);

LPSTR   PUBLIC SzFromIDS (UINT ids, LPSTR pszBuf, int cbBuf);

#endif // __COMM_H__

