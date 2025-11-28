/***
*sys/utime.h - definitions/declarations for utime()
*
*	Copyright (c) 1985-1994, Microsoft Corporation. All rights reserved.
*
*Purpose:
*	This file defines the structure used by the utime routine to set
*	new file access and modification times.  NOTE - MS-DOS
*	does not recognize access time, so this field will
*	always be ignored and the modification time field will be
*	used to set the new time.
*
****/

#ifndef _INC_UTIME
#define _INC_UTIME

#ifdef __cplusplus
extern "C" {
#endif


/* Define _CRTAPI1 (for compatibility with the NT SDK) */

#ifndef _CRTAPI1
#if	( (_MSC_VER >= 800) && (_M_IX86 >= 300) )
#define _CRTAPI1 __cdecl
#else
#define _CRTAPI1
#endif
#endif


/* Define _CRTAPI2 (for compatibility with the NT SDK) */

#ifndef _CRTAPI2
#if	( (_MSC_VER >= 800) && (_M_IX86 >= 300) )
#define _CRTAPI2 __cdecl
#else
#define _CRTAPI2
#endif
#endif


/* Define _CRTIMP */

#ifndef _CRTIMP
#ifdef	_NTSDK
/* definition compatible with NT SDK */
#define _CRTIMP
#else	/* ndef _NTSDK */
/* current definition */
#ifdef	_DLL
#define _CRTIMP __declspec(dllimport)
#else	/* ndef _DLL */
#define _CRTIMP
#endif	/* _DLL */
#endif	/* _NTSDK */
#endif	/* _CRTIMP */


/* Define __cdecl for non-Microsoft compilers */

#if	( !defined(_MSC_VER) && !defined(__cdecl) )
#define __cdecl
#endif

#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

#ifndef _TIME_T_DEFINED
typedef long time_t;
#define _TIME_T_DEFINED
#endif

/* define struct used by _utime() function */

#ifndef _UTIMBUF_DEFINED

struct _utimbuf {
	time_t actime;		/* access time */
	time_t modtime; 	/* modification time */
	};

#if	!__STDC__
/* Non-ANSI name for compatibility */
#ifdef	_NTSDK
#define utimbuf _utimbuf
#else	/* ndef _NTSDK */
struct utimbuf {
	time_t actime;		/* access time */
	time_t modtime; 	/* modification time */
	};
#endif	/* _NTSDK */
#endif

#define _UTIMBUF_DEFINED
#endif


/* Function Prototypes */

_CRTIMP int __cdecl _utime(char *, struct _utimbuf *);
_CRTIMP int __cdecl _futime(int, struct _utimbuf *);

/* Wide Function Prototypes */

_CRTIMP int __cdecl _wutime(wchar_t *, struct _utimbuf *);

#if	!__STDC__
/* Non-ANSI name for compatibility */
#ifdef	_NTSDK
#define utime _utime
#else	/* ndef _NTSDK */
_CRTIMP int __cdecl utime(char *, struct utimbuf *);
#endif	/* _NTSDK */
#endif

#ifdef __cplusplus
}
#endif

#endif	/* _INC_UTIME */
