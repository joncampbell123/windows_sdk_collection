// This is a part of the Microsoft Foundation Classes C++ library. 
// Copyright (C) 1992 Microsoft Corporation 
// All rights reserved. 
//  
// This source code is only intended as a supplement to the 
// Microsoft Foundation Classes Reference and Microsoft 
// QuickHelp documentation provided with the library. 
// See these sources for detailed information regarding the 
// Microsoft Foundation Classes product. 


#include "afxwin.h"
#pragma hdrstop

#include "winhand_.h"

#ifdef AFX_CORE_SEG
#pragma code_seg(AFX_CORE_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// Standard exception processing

IMPLEMENT_DYNAMIC(CResourceException, CException)
static  CResourceException NEAR simpleResourceException; 
void AfxThrowResourceException()                            
	{ afxExceptionContext.Throw(&simpleResourceException, TRUE); }


/////////////////////////////////////////////////////////////////////////////
// Diagnostic Output
#ifdef _DEBUG
CDumpContext& operator<<(CDumpContext& dc, SIZE size)
{
	return dc << "(" << size.cx << " x " << size.cy << ")";
}

CDumpContext& operator<<(CDumpContext& dc, POINT point)
{
	return dc << "(" << point.x << ", " << point.y << ")";
}

CDumpContext& operator<<(CDumpContext& dc, const RECT& rect)
{
	return dc << "(L " << rect.left << ", T " << rect.top << ", R " <<
		rect.right << ", B " << rect.bottom << ")";
}
#endif //_DEBUG



/////////////////////////////////////////////////////////////////////////////
// CDC

IMPLEMENT_DYNAMIC(CDC, CObject)

// Map from HDC to CDC*
class NEAR CDCHandleMap : public CHandleMap
{
public:
	CObject* NewTempObject(HANDLE h)
				{
					// don't add in permanent
					CDC* p = new CDC();
					p->m_hDC = (HDC)h;       // set after constructed
					return p;
				}
	void DeleteTempObject(CObject* ob)
				{
					// destructor doesn't do anything
					ASSERT(ob->IsKindOf(RUNTIME_CLASS(CDC)));
					((CDC*)ob)->m_hDC = NULL;
					delete ob;
				}
};
static CDCHandleMap NEAR deviceMap;

#ifdef _DEBUG
void CDC::AssertValid() const
{
	CObject::AssertValid();
}

void CDC::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
	dc << "m_hDC = " << m_hDC;
}
#endif

CDC*
CDC::FromHandle(HDC hDC)
{
	return (CDC*)deviceMap.FromHandle(hDC);
}

void
CDC::DeleteTempMap()
{
	deviceMap.DeleteTemp();
}

BOOL
CDC::Attach(HDC hDC)
{
	ASSERT(m_hDC == NULL);      // only attach once, detach on destroy
	if (hDC == NULL)
		return FALSE;
	deviceMap.SetPermanent(m_hDC = hDC, this);
	return TRUE;
}

HDC CDC::Detach()
{
	HDC hDC;
	if ((hDC = m_hDC) != NULL)
		deviceMap.RemovePermanent(m_hDC);
	m_hDC = NULL;
	return hDC;
}

BOOL CDC::DeleteDC()
{
	if (m_hDC == NULL)
		return FALSE;
	 return ::DeleteDC(Detach());
}

CDC::~CDC()
{
	if (m_hDC != NULL)
		::DeleteDC(Detach());
}

/////////////////////////////////////////////////////////////////////////////
// Out-of-line routines

CGdiObject*
CDC::SelectGdiObject(HDC hDC, HANDLE h)
{
	return CGdiObject::FromHandle(::SelectObject(hDC, h));
}

CPalette*
CDC::SelectPalette(CPalette* pPalette, BOOL bForceBackground)
{
	return (CPalette*) CGdiObject::FromHandle(
		::SelectPalette(m_hDC, (HPALETTE)pPalette->m_hObject, bForceBackground));
}

#ifdef _NTWIN
// Win 3.0 compatible API for NT version.
int
CDC::StartDoc(LPCSTR pDocName)
{
	DOCINFO di;
	di.cbSize = sizeof(DOCINFO);
	di.lpszDocName = pDocName;
	di.lpszOutput = NULL;
	return StartDoc(&di);
}
#endif

/////////////////////////////////////////////////////////////////////////////
// Helper DCs

IMPLEMENT_DYNAMIC(CClientDC, CDC)
IMPLEMENT_DYNAMIC(CWindowDC, CDC)
IMPLEMENT_DYNAMIC(CPaintDC, CDC)
IMPLEMENT_DYNAMIC(CMetaFileDC, CDC)

#ifdef _DEBUG
void CClientDC::AssertValid() const
{
	CDC::AssertValid();
	ASSERT(IsWindow(m_hWnd));
}

void CClientDC::Dump(CDumpContext& dc) const
{
	CDC::Dump(dc);
	dc << " m_hWnd = " << (UINT)m_hWnd;
}
#endif

CClientDC::CClientDC(CWnd* pWnd)
{
	if (!Attach(::GetDC(m_hWnd = pWnd->GetSafeHwnd())))
		AfxThrowResourceException();
}

CClientDC::~CClientDC()
{
	ASSERT(m_hDC != NULL);
	::ReleaseDC(m_hWnd, Detach());
}


#ifdef _DEBUG
void CWindowDC::AssertValid() const
{
	CDC::AssertValid();
	ASSERT(::IsWindow(m_hWnd));
}

void CWindowDC::Dump(CDumpContext& dc) const
{
	CDC::Dump(dc);
	dc << " m_hWnd = " << (UINT)m_hWnd;
}
#endif

CWindowDC::CWindowDC(CWnd* pWnd)
{
	if (!Attach(::GetWindowDC(m_hWnd = pWnd->GetSafeHwnd())))
		AfxThrowResourceException();
}

CWindowDC::~CWindowDC()
{
	ASSERT(m_hDC != NULL);
	::ReleaseDC(m_hWnd, Detach());
}

#ifdef _DEBUG
void CPaintDC::AssertValid() const
{
	CDC::AssertValid();
	ASSERT(::IsWindow(m_hWnd));
}

void CPaintDC::Dump(CDumpContext& dc) const
{
	CDC::Dump(dc);
	dc << "\nm_hWnd = " << (UINT)m_hWnd;
	dc << "\nm_ps.hdc = " << (UINT)m_ps.hdc;
	dc << "\nm_ps.fErase = " << m_ps.fErase;
	dc << "\nm_ps.rcPaint = " << (CRect)m_ps.rcPaint;
}
#endif

CPaintDC::CPaintDC(CWnd* pWnd)
{
	ASSERT_VALID(pWnd);
	if (!Attach(::BeginPaint(m_hWnd = pWnd->m_hWnd, &m_ps)))
		AfxThrowResourceException();
}

CPaintDC::~CPaintDC()
{
	ASSERT(m_hDC != NULL);
	::EndPaint(m_hWnd, &m_ps);
	Detach();
}

/////////////////////////////////////////////////////////////////////////////
// CGdiObject

IMPLEMENT_DYNAMIC(CGdiObject, CObject)

// Map from H??? to CGdiObject*

class NEAR CGdiHandleMap : public CHandleMap
{
public:
	CObject* NewTempObject(HANDLE hObject)
				{
					// don't add in permanent
					CGdiObject* p = new CGdiObject;
					p->m_hObject = hObject;     // set after constructed
					return p;
				}
	void DeleteTempObject(CObject* ob)
				{
					ASSERT(ob->IsKindOf(RUNTIME_CLASS(CGdiObject)));
					((CGdiObject*)ob)->m_hObject = NULL;
									// clear before destructed
					delete ob;
				}
};
static CGdiHandleMap NEAR gdiMap;

#ifdef _DEBUG
void CGdiObject::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
	dc << " m_hObject = " << (UINT)m_hObject;
}
#endif

CGdiObject* 
CGdiObject::FromHandle(HANDLE h)
{
	return (CGdiObject*)gdiMap.FromHandle(h);
}

void
CGdiObject::DeleteTempMap()
{
	gdiMap.DeleteTemp();
}

BOOL
CGdiObject::Attach(HANDLE hObject)
{
	ASSERT(m_hObject == NULL);      // only attach once, detach on destroy
	if (hObject == NULL)
		return FALSE;
	gdiMap.SetPermanent(m_hObject = hObject, this);
	return TRUE;
}

HANDLE CGdiObject::Detach()
{
	HANDLE hObject;
	if ((hObject = m_hObject) != NULL)
	{
		gdiMap.RemovePermanent(m_hObject);
		// because of stock objects, it may be in temporary map too
		gdiMap.RemoveTemporary(m_hObject);
	}
	m_hObject = NULL;
	return hObject;
}

BOOL CGdiObject::DeleteObject()
{
	if (m_hObject == NULL)
		return FALSE;
	return ::DeleteObject(Detach());
}

CGdiObject::~CGdiObject()
{
	DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////
// Standard GDI objects

IMPLEMENT_DYNAMIC(CPen, CGdiObject)
IMPLEMENT_DYNAMIC(CBrush, CGdiObject)
IMPLEMENT_DYNAMIC(CFont, CGdiObject)
IMPLEMENT_DYNAMIC(CBitmap, CGdiObject)
IMPLEMENT_DYNAMIC(CPalette, CGdiObject)
IMPLEMENT_DYNAMIC(CRgn, CGdiObject)

/////////////////////////////////////////////////////////////////////////////
// Out-of-line constructors

////////////////////////////////////////////
// CPen

CPen::CPen(int nPenStyle, int nWidth, DWORD crColor)
{
	if (!Attach(::CreatePen(nPenStyle, nWidth, crColor)))
		AfxThrowResourceException();
}

////////////////////////////////////////////
// CBrush

CBrush::CBrush(DWORD crColor)
{
	if (!Attach(::CreateSolidBrush(crColor)))
		AfxThrowResourceException();
}

CBrush::CBrush(int nIndex, DWORD crColor)
{
	if (!Attach(::CreateHatchBrush(nIndex, crColor)))
		AfxThrowResourceException();
}

CBrush::CBrush(CBitmap* pBitmap)
{
	if (!Attach(::CreatePatternBrush((HBITMAP)pBitmap->m_hObject)))
		AfxThrowResourceException();
}

/////////////////////////////////////////////////////////////////////////////
