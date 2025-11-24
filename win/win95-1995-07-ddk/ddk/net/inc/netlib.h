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

/*
 * Function prototypes for the functions present in netlib32.
 */

#ifndef _INC_NETLIB
#define _INC_NETLIB

#ifndef RC_INVOKED
#pragma pack(1)         /* Assume byte packing throughout */
#endif /* !RC_INVOKED */

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

#ifdef DBCS
#define IS_LEAD_BYTE(c)     IsDBCSLeadByte(c)
#else
#define IS_LEAD_BYTE(c)     0
#endif

/*
 * BUGBUG - all the old netlib functions had cdecl on them.  why?
 */
#define NETLIB_FUNC	WINAPI

void NETLIB_FUNC initdbcs(void);		// ????
void NETLIB_FUNC init_strlib(void);

#ifdef DEBUG
void            cdecl nprintf(const char *, ...);
#endif

int sbufchkf(const char FAR *, unsigned short);

#if 0	/* strrpbrkf isn't used in our code */
LPSTR cdecl strrpbrkf(const char FAR *, const char FAR *);	// ????
#endif

LPSTR cdecl strtailf(const char FAR *, const char FAR *);		// ????
int NetPackString (char FAR* FAR*, char FAR*, char FAR* FAR*);

//====== Memory allocation functions =================================

// Alloc a chunk of memory, quickly, with no 64k limit on size of
// individual objects or total object size.
//
void * WINAPI MemAlloc(long cb);

// Realloc one of above.  If pb is NULL, then this function will do
// an alloc for you.
//
void * WINAPI MemReAlloc(void * pb, long cb);

// Free a chunk of memory alloced or realloced with above routines.
//
BOOL WINAPI MemFree(void * pb);

#ifdef DEBUG
// Data structure for debug instrumentation.
//
struct MemAllocInfo
{
	struct MemAllocInfo *pNext;
	UINT cAllocs;
	UINT cFrees;
	UINT cbAlloc;
	UINT cbMaxAlloc;
	UINT cbTotalAlloc;
};

VOID WINAPI MemRegisterWatcher(struct MemAllocInfo *pBuffer);
VOID WINAPI MemDeregisterWatcher(struct MemAllocInfo *pBuffer);
LPVOID WINAPI MemUpdateOff(VOID);
VOID WINAPI MemUpdateContinue(LPVOID pvContext);

UINT WINAPI MemGetAllocCount(void);
UINT WINAPI MemGetFreeCount(void);
#endif	/* DEBUG */

// I_IsBadStringPtrA()
//
// Private Win32 version of IsBadStringPtr that works properly, i.e.
// like the Win16 version, it returns TRUE if the string is not
// null-terminated.
BOOL WINAPI I_IsBadStringPtrA(LPCSTR lpsz, UINT ucchMax);

#ifndef USE_WIN32_ISBADSTRINGPTR
#define IsBadStringPtrA I_IsBadStringPtrA
#endif

/*
 * The following functions are DBCS-safe even in their C runtime versions,
 * so they don't need to exist in netlib itself.  In the debug build,
 * since the intrinsic CRT functions won't be intrinsic, we call the
 * Windows APIs instead.
 */

#ifdef DEBUG

#define strcpyf(d,s)	lstrcpy((d),(s))
#define strcatf(d,s)	lstrcat((d),(s))
#define strlenf(s)		lstrlen((s))
#define memcpyf(d,s,l)	CopyMemory((d),(s),(l))

#else

#define strcpyf(d,s)	strcpy((d),(s))
#define strcatf(d,s)	strcat((d),(s))
#define strlenf(s)		strlen((s))
#define memcpyf(d,s,l)	memcpy((d),(s),(l))

#endif

#pragma intrinsic(memcmp,memset)
#define memcmpf(d,s,l)	memcmp((d),(s),(l))
#define memsetf(s,c,l)	memset((s),(c),(l))
#define memmovef(d,s,l)	MoveMemory((d),(s),(l))

#define strcmpf(s1,s2)	lstrcmp(s1,s2)
#define stricmpf(s1,s2)	lstrcmpi(s1,s2)

/* In a DBCS build, AnsiUpper (aka CharUpper) will uppercase double-byte
 * Romanji characters, but IFSMGR and other DOS- and Unicode-based services
 * will not.  So in the DBCS build of our components, we must avoid calling
 * AnsiUpper and do our own thing.
 */
#ifdef DBCS
LPSTR NETLIB_FUNC struprf(LPSTR s);
#else
#define struprf(s)		AnsiUpper(s)
#endif

void  NETLIB_FUNC inidbcsf(void);
LPSTR NETLIB_FUNC strncpyf(LPSTR, LPCSTR, UINT);
int   NETLIB_FUNC strncmpf(LPCSTR, LPCSTR, UINT);
int   NETLIB_FUNC strnicmpf(LPCSTR, LPCSTR, UINT);
LPSTR NETLIB_FUNC strchrf(LPCSTR, UINT);
LPSTR NETLIB_FUNC strrchrf(LPCSTR, UINT);
UINT  NETLIB_FUNC strspnf(LPCSTR, LPCSTR);
UINT  NETLIB_FUNC strcspnf(LPCSTR, LPCSTR);

LPSTR NETLIB_FUNC strncatf(LPSTR, LPCSTR, UINT);
LPSTR NETLIB_FUNC strstrf(LPCSTR, LPCSTR);
LPSTR NETLIB_FUNC stristrf(LPCSTR, LPCSTR);

LPSTR NETLIB_FUNC strtokf(LPSTR, LPSTR);

#ifndef RC_INVOKED

#if 0	/* this definition is inaccurate and strpbrkf() isn't used in our code anyway */
#define strpbrkf(s,c)	((s) + strcspnf((s),(c)))
#endif

#pragma pack()
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _INC_NETLIB */
