/////////////////////////////////////////////////////////////////////////////
// Special include for Win32s compatibility

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif

#ifndef __AFXSTATE_H__
	#include <afxstat_.h>   // for AFX_APP_STATE and AFX_THREAD_STATE
#endif

/////////////////////////////////////////////////////////////////////////////
// Inline function declarations

#ifdef _AFX_ENABLE_INLINES
#define _AFXCOLL_INLINE inline
#include <afxcoll.inl>
#endif

#undef AFX_DATA
#define AFX_DATA
#endif //!__AFXCOLL_H__

/////////////////////////////////////////////////////////////////////////////
