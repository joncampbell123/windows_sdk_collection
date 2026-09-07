/***
*malloc.h - declarations and definitions for memory allocation functions
*
*       Copyright (c) 1985-1995, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Contains the function declarations for memory allocation functions;
*       also defines manifest constants and types used by the heap routines.
*       [System V]
*
*       [Public]
*
****/

#ifndef _INC_MALLOC
#define _INC_MALLOC

#if !defined (_WIN32) && !defined (_MAC)
#error ERROR: Only Mac or Win32 targets supported!
#endif  /* !defined (_WIN32) && !defined (_MAC) */

#ifndef _CRTBLD
/* This version of the header files is NOT for user programs.
 * It is intended for use when building the C runtimes ONLY.
 * The version intended for public use will not have this message.
 */
#error ERROR: Use of C runtime library internal header file.
#endif  /* _CRTBLD */

#ifdef _MSC_VER
/*
 * Currently, all MS C compilers for Win32 platforms default to 8 byte
 * alignment.
 */
#pragma pack(push,8)
#endif  /* _MSC_VER */

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifndef _INTERNAL_IFSTRIP_
#include <cruntime.h>
#endif  /* _INTERNAL_IFSTRIP_ */

/* Define _CRTAPI1 (for compatibility with the NT SDK) */

#ifndef _CRTAPI1
#if _MSC_VER >= 800 && _M_IX86 >= 300
#define _CRTAPI1 __cdecl
#else  /* _MSC_VER >= 800 && _M_IX86 >= 300 */
#define _CRTAPI1
#endif  /* _MSC_VER >= 800 && _M_IX86 >= 300 */
#endif  /* _CRTAPI1 */


/* Define _CRTAPI2 (for compatibility with the NT SDK) */

#ifndef _CRTAPI2
#if _MSC_VER >= 800 && _M_IX86 >= 300
#define _CRTAPI2 __cdecl
#else  /* _MSC_VER >= 800 && _M_IX86 >= 300 */
#define _CRTAPI2
#endif  /* _MSC_VER >= 800 && _M_IX86 >= 300 */
#endif  /* _CRTAPI2 */


/* Define _CRTIMP */

#ifndef _CRTIMP
/* current definition */
#ifdef CRTDLL
#define _CRTIMP __declspec(dllexport)
#else  /* CRTDLL */
#ifdef _DLL
#define _CRTIMP __declspec(dllimport)
#else  /* _DLL */
#define _CRTIMP
#endif  /* _DLL */
#endif  /* CRTDLL */
#endif  /* _CRTIMP */


/* Define __cdecl for non-Microsoft compilers */

#if (!defined (_MSC_VER) && !defined (__cdecl))
#define __cdecl
#endif  /* (!defined (_MSC_VER) && !defined (__cdecl)) */


#ifndef _SIZE_T_DEFINED
typedef unsigned int size_t;
#define _SIZE_T_DEFINED
#endif  /* _SIZE_T_DEFINED */


/* Maximum heap request the heap manager will attempt */

#define _HEAP_MAXREQ    0xFFFFFFE0

/* Constants for _heapchk/_heapset/_heapwalk routines */

#define _HEAPEMPTY      (-1)
#define _HEAPOK         (-2)
#define _HEAPBADBEGIN   (-3)
#define _HEAPBADNODE    (-4)
#define _HEAPEND        (-5)
#define _HEAPBADPTR     (-6)
#define _FREEENTRY      0
#define _USEDENTRY      1

#ifndef _HEAPINFO_DEFINED
typedef struct _heapinfo {
        int * _pentry;
        size_t _size;
        int _useflag;
        } _HEAPINFO;
#define _HEAPINFO_DEFINED
#endif  /* _HEAPINFO_DEFINED */


/* External variable declarations */

#if defined (_DLL) && defined (_M_IX86)

#define _amblksiz   (*__p__amblksiz())
_CRTIMP unsigned int * __cdecl __p__amblksiz(void);

#else  /* defined (_DLL) && defined (_M_IX86) */

#ifndef DLL_FOR_WIN32S
extern unsigned int _amblksiz;
#endif  /* DLL_FOR_WIN32S */

#endif  /* defined (_DLL) && defined (_M_IX86) */



/* Function prototypes */

_CRTIMP void * __cdecl calloc(size_t, size_t);
_CRTIMP void   __cdecl free(void *);
_CRTIMP void * __cdecl malloc(size_t);
_CRTIMP void * __cdecl realloc(void *, size_t);
#if defined (_M_M68K) || defined (_M_MPPC)
_CRTIMP size_t __cdecl _stackavail(void);
#endif  /* defined (_M_M68K) || defined (_M_MPPC) */


void * __cdecl _alloca(size_t);
_CRTIMP void * __cdecl _expand(void *, size_t);
_CRTIMP int __cdecl _heapadd(void *, size_t);
_CRTIMP int __cdecl _heapchk(void);
_CRTIMP int __cdecl _heapmin(void);
_CRTIMP int __cdecl _heapset(unsigned int);
_CRTIMP int __cdecl _heapwalk(_HEAPINFO *);
_CRTIMP size_t __cdecl _heapused(size_t *, size_t *);
_CRTIMP size_t __cdecl _msize(void *);

#if !__STDC__
/* Non-ANSI names for compatibility */
#define alloca  _alloca
#endif  /* !__STDC__ */

#if defined (_M_MRX000) || defined (_M_PPC)
#pragma intrinsic(_alloca)
#endif  /* defined (_M_MRX000) || defined (_M_PPC) */




#ifdef __cplusplus
}
#endif  /* __cplusplus */

#ifdef DLL_FOR_WIN32S
#include <win32s.h>
#endif  /* DLL_FOR_WIN32S */

#ifdef _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif  /* _INC_MALLOC */
