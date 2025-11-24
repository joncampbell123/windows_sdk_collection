// This is a part of the Microsoft Foundation Classes C++ library. 
// Copyright (C) 1992 Microsoft Corporation 
// All rights reserved. 
//  
// This source code is only intended as a supplement to the 
// Microsoft Foundation Classes Reference and Microsoft 
// QuickHelp documentation provided with the library. 
// See these sources for detailed information regarding the 
// Microsoft Foundation Classes product. 

#include "afx.h"
#pragma hdrstop

#ifdef AFX_CORE_SEG
#pragma code_seg(AFX_CORE_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#if defined(_WINDOWS) && defined(_DOSWIN)
extern "C" void far pascal Throw(const int FAR*, int);
#define longjmp ::Throw
#else
extern "C" void __cdecl longjmp(jmp_buf, int);
#endif

/////////////////////////////////////////////////////////////////////////////
// single threaded, assume 1 global exception context

CExceptionContext  NEAR afxExceptionContext;

IMPLEMENT_DYNAMIC(CException, CObject)      // abstract class

/////////////////////////////////////////////////////////////////////////////
// class CExceptionContext (thread global state)

void 
CExceptionContext::Throw(CException* pNewException)
{
	// default to not shared
	Throw(pNewException, FALSE);
}

void 
CExceptionContext::ThrowLast()
{
	// default to not shared, use the old one
	ASSERT(m_pCurrent != NULL);

	Throw(m_pCurrent, FALSE);
}


void 
CExceptionContext::Throw(CException* pNewException, BOOL bShared)
{
	ASSERT(pNewException != NULL);
	TRACE("Warning: Throwing an Exception of type %s\n",
		pNewException->GetRuntimeClass()->m_pszClassName);

	if (m_pCurrent != pNewException)
	{
		// throwing a new exception (otherwise keep old shared state)
		if (m_pCurrent != NULL && m_bDeleteWhenDone)
			delete m_pCurrent;
		m_pCurrent = pNewException;
		m_bDeleteWhenDone = !bShared;
	}

	// walk the handlers
	if (m_pLinkTop == NULL)
	{
		// uncaught exception, terminate
		TRACE("Error: Un-caught Exception (%s)\n",
			pNewException->GetRuntimeClass()->m_pszClassName);
		AfxTerminate();
	}

	CExceptionLink* pReceiver = m_pLinkTop;
	m_pLinkTop = m_pLinkTop->m_pLinkPrev;
	pReceiver->m_pLinkPrev = NULL;
	longjmp(pReceiver->m_jumpBuf, 1);
}

void 
CExceptionContext::Cleanup()
{
	if (m_bDeleteWhenDone)
		delete m_pCurrent;
	m_pCurrent = NULL;
}


CExceptionLink::~CExceptionLink()
{
	if (afxExceptionContext.m_pLinkTop == this)
		afxExceptionContext.m_pLinkTop = m_pLinkPrev;
	else if (m_pLinkPrev != NULL)
		AfxTerminate(); 
}

/////////////////////////////////////////////////////////////////////////////
// Support for new exception APIs

static AFX_TERM_PROC pfnTerminate = AfxAbort;

void CDECL AfxTerminate()
{
	TRACE("AfxTerminate called\n");
	(*pfnTerminate)();
}

AFX_TERM_PROC AfxSetTerminate(AFX_TERM_PROC pfnNew)
{
	AFX_TERM_PROC pfnOld = pfnTerminate;
	pfnTerminate = pfnNew;
	return pfnOld;
}

/////////////////////////////////////////////////////////////////////////////
// Standard exceptions

IMPLEMENT_DYNAMIC(CMemoryException, CException)
static  CMemoryException NEAR simpleMemoryException; 
void PASCAL AfxThrowMemoryException()                           
	{ afxExceptionContext.Throw(&simpleMemoryException, TRUE); }

IMPLEMENT_DYNAMIC(CNotSupportedException, CException)
static  CNotSupportedException NEAR simpleNotSupportedException; 
void PASCAL AfxThrowNotSupportedException()                         
	{ afxExceptionContext.Throw(&simpleNotSupportedException, TRUE); }

/////////////////////////////////////////////////////////////////////////////
