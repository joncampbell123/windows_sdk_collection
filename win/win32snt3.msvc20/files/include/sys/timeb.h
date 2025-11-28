/***
*sys/timeb.h - definition/declarations for _ftime()
*
*	Copyright (c) 1985-1994, Microsoft Corporation. All rights reserved.
*
*Purpose:
*	This file define the _ftime() function and the types it uses.
*	[System V]
*
****/

#ifndef _INC_TIMEB
#define _INC_TIMEB

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


#ifndef _TIME_T_DEFINED
typedef long time_t;
#define _TIME_T_DEFINED
#endif


/* Structure returned by _ftime system call */

#ifndef _TIMEB_DEFINED
struct _timeb {
	time_t time;
	unsigned short millitm;
	short timezone;
	short dstflag;
	};

#if	!__STDC__

/* Non-ANSI name for compatibility */

#ifdef	_NTSDK
/* definition compatible with NT SDK */
#define timeb	_timeb
#else	/* ndef _NTSDK */
/* current definition */
struct timeb {
	time_t time;
	unsigned short millitm;
	short timezone;
	short dstflag;
	};
#endif	/* _NTSDK */

#endif

#define _TIMEB_DEFINED
#endif


/* Function prototypes */

_CRTIMP void __cdecl _ftime(struct _timeb *);

#if	!__STDC__

/* Non-ANSI name for compatibility */

#ifdef	_NTSDK
/* definition compatible with NT SDK */
#define ftime	_ftime
#else	/* ndef _NTSDK */
/* current declaration */
_CRTIMP void __cdecl ftime(struct timeb *);
#endif	/* _NTSDK */

#endif


#ifdef __cplusplus
}
#endif

#endif	/* _INC_TIMEB */
