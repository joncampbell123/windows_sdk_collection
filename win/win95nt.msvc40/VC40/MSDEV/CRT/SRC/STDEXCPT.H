/***
*stdexcpt.h - User include file for standard exception classes
*
*       Copyright (c) 1994-1995, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file presents an interface to the standard exception classes,
*       as specified by the ANSI X3J16/ISO SC22/WG21 Working Paper for
*       Draft C++, May 1994.
*
*       [Public]
*
****/

#ifndef _INC_STDEXCPT
#define _INC_STDEXCPT

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

#ifndef _SIZE_T_DEFINED
typedef unsigned int size_t;
#define _SIZE_T_DEFINED
#endif  /* _SIZE_T_DEFINED */


//
// Standard exception class heirarchy (ref. 1/94 WP 17.3.2.1, as ammended 3/94).
//
// exception (formerly xmsg)
//   logic
//     domain
//   runtime
//     range
//     alloc
//       xalloc
//
// Updated as per May'94 Working Paper

typedef const char *__exString;

class _CRTIMP exception
{
public:
    exception();
    exception(const __exString&);
    exception(const exception&);
    exception& operator= (const exception&);
    virtual ~exception();
    virtual __exString what() const;
private:
    __exString _m_what;
    int _m_doFree;
};

#ifdef __RTTI_OLDNAMES
typedef exception xmsg;        // A synonym for folks using older standard
#endif  /* __RTTI_OLDNAMES */

//
//  logic_error
//
class _CRTIMP logic_error: public exception
{
public:
    logic_error (const __exString& _what_arg) : exception(_what_arg) {}
};

#endif  /* _INC_STDEXCPT */

