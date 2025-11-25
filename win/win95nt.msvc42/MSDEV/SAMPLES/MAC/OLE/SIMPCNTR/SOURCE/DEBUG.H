#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>

#ifdef _MSC_VER
#include <string.h>
#else
#include <strings.h>
#endif

#undef ASSERT
#undef ASSERTCOND
#undef ASSERTSZ
#undef ASSERTDATA

#ifdef DEBUG

	extern "C" PASCAL_CALL(void) OutputDebugString(const char *);

	#define ASSERT(x)			{ if (!(x)) { sprintf(gDebugStr, "%s(%ld): %s", _szAssertFile, __LINE__, #x); DebugStr(c2pstr(gDebugStr)); } }
	#define ASSERTCOND(x)		{ if (!(x)) { sprintf(gDebugStr, "%s(%ld): %s", _szAssertFile, __LINE__, #x); DebugStr(c2pstr(gDebugStr)); } }
	#define ASSERTSZ(x, y)		{ if (!(x)) { sprintf(gDebugStr, "%s(%ld): %s", _szAssertFile, __LINE__, #y); DebugStr(c2pstr(gDebugStr)); } }
	#define ASSERTDATA			static char _szAssertFile[] = __FILE__;

	#define DEBUGNUM(n)  		{ NumToString(n,(StringPtr)gDebugStr); DebugStr((StringPtr)gDebugStr); }
	#define	ENTER_INTERFACE(n)	{ sprintf(gDebugStr, "%s\r", n); OutputDebugString(gDebugStr); }
	#define	DUMP_REFCOUNT(n,x)	{ sprintf(gDebugStr, "%s (m_cRef == %ld)\r",n, x); OutputDebugString(gDebugStr); }

	extern char gDebugStr[255];

#else

	#define ASSERT(x)
	#define ASSERT(x)
	#define ASSERTCOND(x)
	#define ASSERTSZ(x, y)
	#define ASSERTDATA

	#define DEBUGNUM(n)
	#define ENTER_INTERFACE(n)
	#define DUMP_REFCOUNT(n,x)

#endif // DEBUG

#endif // _DEBUG_H_
