// Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation,
// All rights reserved.

// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __AFXWIN_INL__
#define __AFXWIN_INL__

#ifdef _DEBUG
extern char BASED_CODE _afxSzAfxWinInl[]; // defined in dumpcont.cpp
#undef THIS_FILE
#define THIS_FILE _afxSzAfxWinInl
#endif

// Global helper functions
inline CWinApp* AfxGetApp()
	{ return afxCurrentWinApp; }
inline HINSTANCE AfxGetInstanceHandle()
	{ ASSERT(afxCurrentInstanceHandle != NULL);
		return (HINSTANCE)afxCurrentInstanceHandle; }
inline HINSTANCE AfxGetResourceHandle()
	{ ASSERT(afxCurrentResourceHandle != NULL);
		return (HINSTANCE)afxCurrentResourceHandle; }
inline const char* AfxGetAppName()
	{ ASSERT(afxCurrentAppName != NULL); return afxCurrentAppName; }

// CSize inline functions
inline CSize::CSize()
	{ /* random filled */ }
inline CSize::CSize(int initCX, int initCY)
	{ cx = initCX; cy = initCY; }
inline CSize::CSize(SIZE initSize)
	{ *(SIZE*)this = initSize; }
inline CSize::CSize(POINT initPt)
	{ *(POINT*)this = initPt; }
inline CSize::CSize(DWORD dwSize)
#ifdef _NTWIN
	{ cx = (short)LOWORD(dwSize); cy = (short)HIWORD(dwSize); }
#else
	{ *(DWORD*)this = dwSize; }
#endif
inline BOOL CSize::operator==(SIZE size) const
	{ return (cx == size.cx && cy == size.cy); }
inline BOOL CSize::operator!=(SIZE size) const
	{ return (cx != size.cx || cy != size.cy); }
inline void CSize::operator+=(SIZE size)
	{ cx += size.cx; cy += size.cy; }
inline void CSize::operator-=(SIZE size)
	{ cx -= size.cx; cy -= size.cy; }
inline CSize CSize::operator+(SIZE size) const
	{ return CSize(cx + size.cx, cy + size.cy); }
inline CSize CSize::operator-(SIZE size) const
	{ return CSize(cx - size.cx, cy - size.cy); }

// CPoint inline functions
inline CPoint::CPoint()
	{ /* random filled */ }
inline CPoint::CPoint(int initX, int initY)
	{ x = initX; y = initY; }
inline CPoint::CPoint(POINT initPt)
	{ *(POINT*)this = initPt; }
inline CPoint::CPoint(SIZE initSize)
	{ *(SIZE*)this = initSize; }
inline CPoint::CPoint(DWORD dwPoint)
#ifdef _NTWIN
	{ x = (short)LOWORD(dwPoint); y = (short)HIWORD(dwPoint); }
#else
	{ *(DWORD*)this = dwPoint; }
#endif
inline void CPoint::Offset(int xOffset, int yOffset)
	{ x += xOffset; y += yOffset; }
inline void CPoint::Offset(POINT point)
	{ x += point.x; y += point.y; }
inline void CPoint::Offset(SIZE size)
	{ x += size.cx; y += size.cy; }
inline BOOL CPoint::operator==(POINT point) const
	{ return (x == point.x && y == point.y); }
inline BOOL CPoint::operator!=(POINT point) const
	{ return (x != point.x || y != point.y); }
inline void CPoint::operator+=(SIZE size)
	{ x += size.cx; y += size.cy; }
inline void CPoint::operator-=(SIZE size)
	{ x -= size.cx; y -= size.cy; }
inline CPoint CPoint::operator+(SIZE size) const
	{ return CPoint(x + size.cx, y + size.cy); }
inline CPoint CPoint::operator-(SIZE size) const
	{ return CPoint(x - size.cx, y - size.cy); }
inline CSize CPoint::operator-(POINT point) const
	{ return CSize(x - point.x, y - point.y); }


// CRect inline functions
inline CRect::CRect()
	{ /* random filled */ }
inline CRect::CRect(int l, int t, int r, int b)
	{ left = l; top = t; right = r; bottom = b; }
inline CRect::CRect(const RECT& srcRect)
	{ ::CopyRect((LPRECT)this, (LPRECT)&srcRect); }
inline CRect::CRect(LPRECT lpSrcRect)
	{ ::CopyRect((LPRECT)this, lpSrcRect); }
inline CRect::CRect(POINT point, SIZE size)
	{ left = point.x; top = point.y; 
		right = point.x + size.cx; bottom = point.y + size.cy; }
inline int CRect::Width() const
	{ return right - left; }
inline int CRect::Height() const
	{ return bottom - top; }
inline CSize CRect::Size() const
	{ return CSize(right - left, bottom - top); }
inline CPoint& CRect::TopLeft()
	{ return *((CPoint*)this); }
inline CPoint& CRect::BottomRight()
	{ return *((CPoint*)this+1); }
inline CRect::operator LPRECT()
	{ return (LPRECT)this; }
inline BOOL CRect::IsRectEmpty() const
	{ return ::IsRectEmpty((LPRECT)this); }
inline BOOL CRect::IsRectNull() const
	{ return (left == 0 && right == 0 && top == 0 && bottom == 0); }
inline BOOL CRect::PtInRect(POINT point) const
	{ return ::PtInRect((LPRECT)this, point); }
inline void CRect::SetRect(int x1, int y1, int x2, int y2)
	{ ::SetRect((LPRECT)this, x1, y1, x2, y2); }
inline void CRect::SetRectEmpty()
	{ ::SetRectEmpty((LPRECT)this); }
inline void CRect::CopyRect(LPRECT lpSrcRect)
	{ ::CopyRect((LPRECT)this, lpSrcRect); }
inline BOOL CRect::EqualRect(LPRECT lpRect) const
	{ return ::EqualRect((LPRECT)this, lpRect); }
inline void CRect::InflateRect(int x, int y)
	{ ::InflateRect((LPRECT)this, x, y); }
inline void CRect::InflateRect(SIZE size)
	{ ::InflateRect((LPRECT)this, size.cx, size.cy); }
inline void CRect::OffsetRect(int x, int y)
	{ ::OffsetRect((LPRECT)this, x, y); }
inline void CRect::OffsetRect(POINT point)
	{ ::OffsetRect((LPRECT)this, point.x, point.y); }
inline void CRect::OffsetRect(SIZE size)
	{ ::OffsetRect((LPRECT)this, size.cx, size.cy); }
inline int CRect::IntersectRect(LPRECT lpRect1, LPRECT lpRect2)
	{ return ::IntersectRect((LPRECT)this, lpRect1, lpRect2);}
inline int CRect::UnionRect(LPRECT lpRect1, LPRECT lpRect2)
	{ return ::UnionRect((LPRECT)this, lpRect1, lpRect2); }
inline void CRect::operator=(const RECT& srcRect)
	{ ::CopyRect((LPRECT)this, (LPRECT)&srcRect); }
inline BOOL CRect::operator==(const RECT& rect) const
	{ return ::EqualRect((LPRECT)this, (LPRECT)&rect); }
inline BOOL CRect::operator!=(const RECT& rect) const
	{ return !::EqualRect((LPRECT)this, (LPRECT)&rect); }
inline void CRect::operator+=(POINT point)
	{ ::OffsetRect((LPRECT)this, point.x, point.y); }
inline void CRect::operator-=(POINT point)
	{ ::OffsetRect((LPRECT)this, -point.x, -point.y); }
inline void CRect::operator&=(const RECT& rect)
	{ ::IntersectRect((LPRECT)this, this, (LPRECT)&rect); }
inline void CRect::operator|=(const RECT& rect)
	{ ::UnionRect((LPRECT)this, this, (LPRECT)&rect); }
inline CRect CRect::operator+(POINT pt) const
	{ CRect rect(*this); ::OffsetRect(&rect, pt.x, pt.y); return rect; }
inline CRect CRect::operator-(POINT pt) const
	{ CRect rect(*this); ::OffsetRect(&rect, -pt.x, -pt.y); return rect; }
inline CRect CRect::operator&(const RECT& rect2) const
	{ CRect rect; ::IntersectRect(&rect, (LPRECT)this, (LPRECT)&rect2);
	  return rect; }
inline CRect CRect::operator|(const RECT& rect2) const
	{ CRect rect; ::UnionRect(&rect, (LPRECT)this, (LPRECT)&rect2);
	  return rect; }
#if (WINVER >= 0x030a)
inline BOOL CRect::SubtractRect(LPRECT lpRectSrc1, LPRECT lpRectSrc2)
	{ return ::SubtractRect((LPRECT)this, lpRectSrc1, lpRectSrc2); }
#endif /* WINVER >= 0x030a */

inline CArchive& operator<<(CArchive& ar, SIZE size)
	{ ar.Write(&size, sizeof(SIZE));
	return ar; }
inline CArchive& operator<<(CArchive& ar, POINT point)
	{ ar.Write(&point, sizeof(POINT));
	return ar; }
inline CArchive& operator<<(CArchive& ar, const RECT& rect)
	{ ar.Write(&rect, sizeof(RECT));
	return ar; }
inline CArchive& operator>>(CArchive& ar, SIZE& size)
	{ ar.Read(&size, sizeof(SIZE));
	return ar; }
inline CArchive& operator>>(CArchive& ar, POINT& point)
	{ ar.Read(&point, sizeof(POINT));
	return ar; }
inline CArchive& operator>>(CArchive& ar, RECT& rect)
	{ ar.Read(&rect, sizeof(RECT));
	return ar; }


// CResourceException inline functions
inline CResourceException::CResourceException() 
	{ }

// CGdiObject inline functions
inline HANDLE   CGdiObject::GetSafeHandle() const
	{ return this == NULL ? NULL : m_hObject; }
inline CGdiObject::CGdiObject() 
	{ m_hObject = NULL; }
inline int CGdiObject::GetObject(int nCount, void FAR* lpObject) const
	{ return ::GetObject(m_hObject, nCount, lpObject); }
inline BOOL CGdiObject::CreateStockObject(int nIndex)
	{ return Attach(::GetStockObject(nIndex)); }
inline BOOL CGdiObject::UnrealizeObject()
#ifdef _NTWIN
	{ return TRUE; }
#else
	{ return ::UnrealizeObject(m_hObject); }
#endif

// CPen inline functions
inline CPen* CPen::FromHandle(HPEN hPen)
	{ return (CPen*) CGdiObject::FromHandle(hPen); }
inline CPen::CPen() 
	{ }
inline BOOL CPen::CreatePen(int nPenStyle, int nWidth, DWORD crColor)
	{ return Attach(::CreatePen(nPenStyle, nWidth, crColor)); }
inline BOOL CPen::CreatePenIndirect(LPLOGPEN lpLogPen)
	{ return Attach(::CreatePenIndirect(lpLogPen)); }

// CBrush inline functions
inline CBrush* CBrush::FromHandle(HBRUSH hBrush)
	{ return (CBrush*) CGdiObject::FromHandle(hBrush); }
inline CBrush::CBrush() 
	{ }
inline BOOL CBrush::CreateSolidBrush(DWORD crColor)
	{ return Attach(::CreateSolidBrush(crColor)); }
inline BOOL CBrush::CreateHatchBrush(int nIndex, DWORD crColor)
	{ return Attach(::CreateHatchBrush(nIndex, crColor)); }
inline BOOL CBrush::CreateBrushIndirect(LPLOGBRUSH lpLogBrush)
	{ return Attach(::CreateBrushIndirect(lpLogBrush)); }
inline BOOL CBrush::CreateDIBPatternBrush(GLOBALHANDLE hPackedDIB, UINT nUsage)
	{ return Attach(::CreateDIBPatternBrush(hPackedDIB, nUsage)); }
inline BOOL CBrush::CreatePatternBrush(CBitmap* pBitmap)
	{ return Attach(::CreatePatternBrush((HBITMAP)pBitmap->m_hObject)); }

// CFont inline functions
inline CFont* CFont::FromHandle(HFONT hFont)
	{ return (CFont*) CGdiObject::FromHandle(hFont); }
inline CFont::CFont() 
	{ }
inline BOOL CFont::CreateFontIndirect(LPLOGFONT lpLogFont)
	{ return Attach(::CreateFontIndirect(lpLogFont)); }
inline BOOL CFont::CreateFont(int nHeight, int nWidth, int nEscapement,
		int nOrientation, int nWeight, BYTE bItalic, BYTE bUnderline,
		BYTE cStrikeOut, BYTE nCharSet, BYTE nOutPrecision,
		BYTE nClipPrecision, BYTE nQuality, BYTE nPitchAndFamily,
		LPCSTR lpFacename)
	{ return Attach(::CreateFont(nHeight, nWidth, nEscapement,
		nOrientation, nWeight, bItalic, bUnderline, cStrikeOut,
		nCharSet, nOutPrecision, nClipPrecision, nQuality,
		nPitchAndFamily, lpFacename)); }

// CBitmap inline functions
inline CBitmap* CBitmap::FromHandle(HBITMAP hBitmap)
	{ return (CBitmap*) CGdiObject::FromHandle(hBitmap); }
inline CBitmap::CBitmap() 
	{ }
inline BOOL CBitmap::CreateBitmap(int nWidth, int nHeight, BYTE nPlanes,
	 BYTE nBitcount, const void FAR* lpBits)
	{ return Attach(::CreateBitmap(nWidth, nHeight, nPlanes, nBitcount, lpBits)); }
inline BOOL CBitmap::CreateBitmapIndirect(LPBITMAP lpBitmap)
	{ return Attach(::CreateBitmapIndirect(lpBitmap)); }

inline DWORD CBitmap::SetBitmapBits(DWORD dwCount, const void FAR* lpBits)
	{ return ::SetBitmapBits((HBITMAP)m_hObject, dwCount, (const BYTE*)lpBits); }
inline DWORD CBitmap::GetBitmapBits(DWORD dwCount, void FAR* lpBits) const
	{ return ::GetBitmapBits((HBITMAP)m_hObject, dwCount, lpBits); }
inline CSize CBitmap::SetBitmapDimension(int nWidth, int nHeight)
#ifdef _NTWIN
	{ SIZE size; 
		VERIFY(::SetBitmapDimensionEx((HBITMAP)m_hObject, nWidth, nHeight, &size)); 
		return size; }
#else
	{ return ::SetBitmapDimension((HBITMAP)m_hObject, nWidth, nHeight); }
#endif

inline CSize CBitmap::GetBitmapDimension() const
#ifdef _NTWIN
	{ SIZE size; 
		VERIFY(::GetBitmapDimensionEx((HBITMAP)m_hObject, &size)); 
		return size; }
#else
	{ return ::GetBitmapDimension((HBITMAP)m_hObject); }
#endif
inline BOOL CBitmap::LoadBitmap(LPCSTR lpBitmapName)
	{ return Attach(::LoadBitmap(AfxGetResourceHandle(), lpBitmapName));}
inline BOOL CBitmap::LoadBitmap(UINT nIDBitmap)
	{ return Attach(::LoadBitmap(AfxGetResourceHandle(),
	MAKEINTRESOURCE(nIDBitmap))); }
inline BOOL CBitmap::LoadOEMBitmap(UINT nIDBitmap)
	{ return Attach(::LoadBitmap(NULL, MAKEINTRESOURCE(nIDBitmap))); }
inline BOOL CBitmap::CreateCompatibleBitmap(CDC* pDC, int nWidth, int nHeight)
	{ return Attach(::CreateCompatibleBitmap(pDC->m_hDC, nWidth, nHeight)); }
inline BOOL CBitmap::CreateDiscardableBitmap(CDC* pDC, int nWidth, int nHeight)
	{ return Attach(::CreateDiscardableBitmap(pDC->m_hDC, nWidth, nHeight)); }

// CPalette inline functions
inline CPalette* CPalette::FromHandle(HPALETTE hPalette)
	{ return (CPalette*) CGdiObject::FromHandle(hPalette); }
inline CPalette::CPalette() 
	{ }
inline BOOL CPalette::CreatePalette(LPLOGPALETTE lpLogPalette)
	{ return Attach(::CreatePalette(lpLogPalette)); }
inline UINT CPalette::GetPaletteEntries(UINT nStartIndex, UINT nNumEntries,
		LPPALETTEENTRY lpPaletteColors) const
	{ return ::GetPaletteEntries((HPALETTE)m_hObject, nStartIndex,
		nNumEntries, lpPaletteColors); }
inline UINT CPalette::SetPaletteEntries(UINT nStartIndex, UINT nNumEntries,
		LPPALETTEENTRY lpPaletteColors)
	{ return ::SetPaletteEntries((HPALETTE)m_hObject, nStartIndex,
		nNumEntries, lpPaletteColors); }
inline void CPalette::AnimatePalette(UINT nStartIndex, UINT nNumEntries,
		LPPALETTEENTRY lpPaletteColors)
	{ ::AnimatePalette((HPALETTE)m_hObject, nStartIndex, nNumEntries,
		lpPaletteColors); }
inline UINT CPalette::GetNearestPaletteIndex(DWORD crColor) const
	{ return ::GetNearestPaletteIndex((HPALETTE)m_hObject, crColor); }
inline BOOL CPalette::ResizePalette(UINT nNumEntries)
	{ return ::ResizePalette((HPALETTE)m_hObject, nNumEntries); }


// CRgn inline functions
inline CRgn* CRgn::FromHandle(HRGN hRgn)
	{ return (CRgn*) CGdiObject::FromHandle(hRgn); }
inline CRgn::CRgn() 
	{ }
inline BOOL CRgn::CreateRectRgn(int x1, int y1, int x2, int y2)
	{ return Attach(::CreateRectRgn(x1, y1, x2, y2)); }
inline BOOL CRgn::CreateRectRgnIndirect(LPRECT lpRect)
	{ return Attach(::CreateRectRgnIndirect(lpRect)); }
inline BOOL CRgn::CreateEllipticRgn(int x1, int y1, int x2, int y2)
	{ return Attach(::CreateEllipticRgn(x1, y1, x2, y2)); }
inline BOOL CRgn::CreateEllipticRgnIndirect(LPRECT lpRect)
	{ return Attach(::CreateEllipticRgnIndirect(lpRect)); }
inline BOOL CRgn::CreatePolygonRgn(LPPOINT lpPoints, int nCount, int nMode)
	{ return Attach(::CreatePolygonRgn(lpPoints, nCount, nMode)); }
inline BOOL CRgn::CreatePolyPolygonRgn(LPPOINT lpPoints, LPINT lpPolyCounts, int nCount, int nPolyFillMode)
	{ return Attach(::CreatePolyPolygonRgn(lpPoints, lpPolyCounts, nCount, nPolyFillMode)); }
inline BOOL CRgn::CreateRoundRectRgn(int x1, int y1, int x2, int y2, int x3, int y3)
	{ return Attach(::CreateRoundRectRgn(x1, y1, x2, y2, x3, y3)); }
inline void CRgn::SetRectRgn(int x1, int y1, int x2, int y2)
	{ ::SetRectRgn((HRGN)m_hObject, x1, y1, x2, y2); }
inline void CRgn::SetRectRgn(LPRECT lpRect)
	{ ::SetRectRgn((HRGN)m_hObject, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom); }
inline int CRgn::CombineRgn(CRgn* pRgn1, CRgn* pRgn2, int nCombineMode)
	{ return ::CombineRgn((HRGN)m_hObject, (HRGN)pRgn1->m_hObject, 
		(HRGN)pRgn2->GetSafeHandle(), nCombineMode); }
inline int CRgn::CopyRgn(CRgn* pRgnSrc)
	{ return ::CombineRgn((HRGN)m_hObject, (HRGN)pRgnSrc->m_hObject, NULL, RGN_COPY); }
inline BOOL CRgn::EqualRgn(CRgn* pRgn) const
	{ return ::EqualRgn((HRGN)m_hObject, (HRGN)pRgn->m_hObject); }
inline int CRgn::OffsetRgn(int x, int y)
	{ return ::OffsetRgn((HRGN)m_hObject, x, y); }
inline int CRgn::OffsetRgn(POINT point)
	{ return ::OffsetRgn((HRGN)m_hObject, point.x, point.y); }
inline int CRgn::GetRgnBox(LPRECT lpRect) const
	{ return ::GetRgnBox((HRGN)m_hObject, lpRect); }
inline BOOL CRgn::PtInRegion(int x, int y) const
	{ return ::PtInRegion((HRGN)m_hObject, x, y); }
inline BOOL CRgn::PtInRegion(POINT point) const
	{ return ::PtInRegion((HRGN)m_hObject, point.x, point.y); }
inline BOOL CRgn::RectInRegion(LPRECT lpRect) const
	{ return ::RectInRegion((HRGN)m_hObject, lpRect); }

// CDC inline functions
inline HDC CDC::GetSafeHdc() const
	{ return this == NULL ? NULL : m_hDC; }
inline CDC::CDC() 
	{ m_hDC = NULL; }
#ifdef _NTWIN
inline BOOL CDC::CreateDC(LPCSTR lpDriverName,
	LPCSTR lpDeviceName, LPCSTR lpOutput,
	const void FAR* lpInitData)
	{ return Attach(::CreateDC(lpDriverName,
		lpDeviceName, lpOutput, (const DEVMODE*)lpInitData)); }
inline BOOL CDC::CreateIC(LPCSTR lpDriverName,
	LPCSTR lpDeviceName, LPCSTR lpOutput,
	const void FAR* lpInitData)
	{ return Attach(::CreateIC(lpDriverName,
		lpDeviceName, lpOutput, (const DEVMODE*)lpInitData)); }
#else
inline BOOL CDC::CreateIC(LPCSTR lpDriverName,
	LPCSTR lpDeviceName, LPCSTR lpOutput,
	const void FAR* lpInitData)
	{ return Attach(::CreateIC(lpDriverName,
		lpDeviceName, lpOutput, lpInitData)); }
inline BOOL CDC::CreateDC(LPCSTR lpDriverName,
	LPCSTR lpDeviceName, LPCSTR lpOutput,
	const void FAR* lpInitData)
	{ return Attach(::CreateDC(lpDriverName,
		lpDeviceName, lpOutput, lpInitData)); }
#endif
inline BOOL CDC::CreateCompatibleDC(CDC* pDC)
	{ return Attach(::CreateCompatibleDC(pDC->GetSafeHdc())); }
inline int CDC::ExcludeUpdateRgn(CWnd* pWnd)
	{ return ::ExcludeUpdateRgn(m_hDC, pWnd->m_hWnd); }
inline CPoint CDC::GetDCOrg() const
#ifdef _NTWIN
	{ POINT point;
		VERIFY(::GetDCOrgEx(m_hDC, &point));
		return point; }
#else
	{ return ::GetDCOrg(m_hDC); }
#endif

inline int CDC::SaveDC() const
	{ return ::SaveDC(m_hDC); }
inline BOOL CDC::RestoreDC(int nSavedDC)
	{ return ::RestoreDC(m_hDC, nSavedDC); }
inline int CDC::GetDeviceCaps(int nIndex) const
	{ return ::GetDeviceCaps(m_hDC, nIndex); }

inline CPoint CDC::GetBrushOrg() const
#ifdef _NTWIN
	{ POINT point;
		VERIFY(::GetBrushOrgEx(m_hDC, &point));
		return point; }
#else
	{ return ::GetBrushOrg(m_hDC); }
#endif

inline CPoint CDC::SetBrushOrg(int x, int y)
#ifdef _NTWIN
	{ POINT point;
		VERIFY(::SetBrushOrgEx(m_hDC, x, y, &point));
		return point; }
#else
	{ return ::SetBrushOrg(m_hDC, x, y); }
#endif

inline CPoint CDC::SetBrushOrg(POINT point)
#ifdef _NTWIN
	{ VERIFY(::SetBrushOrgEx(m_hDC, point.x, point.y, &point));
		return point; }
#else
	{ return ::SetBrushOrg(m_hDC, point.x, point.y); }
#endif

inline int CDC::EnumObjects(int nObjectType, 
			int (FAR PASCAL EXPORT* lpfn)(LPSTR, LPSTR), LPSTR lpData)
#ifdef STRICT
	{ return ::EnumObjects(m_hDC, nObjectType, (GOBJENUMPROC)lpfn, (LPARAM)lpData); }
#else
	{ return ::EnumObjects(m_hDC, nObjectType, (GOBJENUMPROC)lpfn, lpData); }
#endif
inline CGdiObject* CDC::SelectObject(CGdiObject* pObject)
	{ return SelectGdiObject(m_hDC, pObject->m_hObject); }
inline CGdiObject* CDC::SelectStockObject(int nIndex)
	{ ASSERT(::GetStockObject(nIndex) != NULL);
		return SelectGdiObject(m_hDC, ::GetStockObject(nIndex)); }
inline CPen* CDC::SelectObject(CPen* pPen)
	{ return (CPen*) SelectGdiObject(m_hDC, pPen->m_hObject); }
inline CBrush* CDC::SelectObject(CBrush* pBrush)
	{ return (CBrush*) SelectGdiObject(m_hDC, pBrush->m_hObject); }
inline CFont* CDC::SelectObject(CFont* pFont)
	{ return (CFont*) SelectGdiObject(m_hDC, pFont->m_hObject); }
inline CBitmap* CDC::SelectObject(CBitmap* pBitmap)
	{ return (CBitmap*) SelectGdiObject(m_hDC, pBitmap->m_hObject);}
inline int CDC::SelectObject(CRgn* pRgn)
	{ return (int)::SelectObject(m_hDC, pRgn->m_hObject); }
inline DWORD CDC::GetNearestColor(DWORD crColor) const
	{ return ::GetNearestColor(m_hDC, crColor); }
inline UINT CDC::RealizePalette()
	{ return ::RealizePalette(m_hDC); }
inline void CDC::UpdateColors()
	{ ::UpdateColors(m_hDC); }
inline DWORD CDC::GetBkColor() const
	{ return ::GetBkColor(m_hDC); }
inline DWORD CDC::SetBkColor(DWORD crColor)
	{ return ::SetBkColor(m_hDC, crColor); }
inline int CDC::GetBkMode() const
	{ return ::GetBkMode(m_hDC); }
inline int CDC::SetBkMode(int nBkMode)
	{ return ::SetBkMode(m_hDC, nBkMode); }
inline int CDC::GetPolyFillMode() const
	{ return ::GetPolyFillMode(m_hDC); }
inline int CDC::SetPolyFillMode(int nPolyFillMode)
	{ return ::SetPolyFillMode(m_hDC, nPolyFillMode); }
inline int CDC::GetROP2() const
	{ return ::GetROP2(m_hDC); }
inline int CDC::SetROP2(int nDrawMode)
	{ return ::SetROP2(m_hDC, nDrawMode); }
inline int CDC::GetStretchBltMode() const
	{ return ::GetStretchBltMode(m_hDC); }
inline int CDC::SetStretchBltMode(int nStretchMode)
	{ return ::SetStretchBltMode(m_hDC, nStretchMode); }
inline DWORD CDC::GetTextColor() const
	{ return ::GetTextColor(m_hDC); }
inline DWORD CDC::SetTextColor(DWORD crColor)
	{ return ::SetTextColor(m_hDC, crColor); }
inline int CDC::GetMapMode() const
	{ return ::GetMapMode(m_hDC); }
inline int CDC::SetMapMode(int nMapMode)
	{ return ::SetMapMode(m_hDC, nMapMode); }
inline CPoint CDC::GetViewportOrg() const
#ifdef _NTWIN
	{ POINT point;
		VERIFY(::GetViewportOrgEx(m_hDC, &point));
		return point; }
#else
	{ return ::GetViewportOrg(m_hDC); }
#endif
inline CPoint CDC::SetViewportOrg(int x, int y)
#ifdef _NTWIN
	{ POINT point;
		VERIFY(::SetViewportOrgEx(m_hDC, x, y, &point));
		return point; }
#else
	{ return ::SetViewportOrg(m_hDC, x, y); }
#endif
inline CPoint CDC::SetViewportOrg(POINT point)
#ifdef _NTWIN
	{ VERIFY(::SetViewportOrgEx(m_hDC, point.x, point.y, &point));
		return point; }
#else
	{ return ::SetViewportOrg(m_hDC, point.x, point.y); }
#endif
inline CPoint CDC::OffsetViewportOrg(int nWidth, int nHeight)
#ifdef _NTWIN
	{ POINT point;
		VERIFY(::OffsetViewportOrgEx(m_hDC, nWidth, nHeight, &point));
		return point; }
#else
	{ return ::OffsetViewportOrg(m_hDC, nWidth, nHeight); }
#endif
inline CSize CDC::GetViewportExt() const
#ifdef _NTWIN
	{ SIZE size;
		VERIFY(::GetViewportExtEx(m_hDC, &size));
		return size; }
#else
	{ return ::GetViewportExt(m_hDC); }
#endif
inline CSize CDC::SetViewportExt(int x, int y)
#ifdef _NTWIN
	{ SIZE size;
		VERIFY(::SetViewportExtEx(m_hDC, x, y, &size));
		return size; }
#else
	{ return ::SetViewportExt(m_hDC, x, y); }
#endif
inline CSize CDC::SetViewportExt(SIZE size)
#ifdef _NTWIN
	{  VERIFY(::SetViewportExtEx(m_hDC, size.cx, size.cy, &size));
		return size; }
#else
	{ return ::SetViewportExt(m_hDC, size.cx, size.cy); }
#endif
inline CSize CDC::ScaleViewportExt(int xNum, int xDenom, int yNum, int yDenom)
#ifdef _NTWIN
	{ SIZE size;
		VERIFY(::ScaleViewportExtEx(m_hDC, xNum, xDenom, yNum, yDenom, &size));
		return size; }
#else
	{ return ::ScaleViewportExt(m_hDC, xNum, xDenom, yNum, yDenom);}
#endif
inline CPoint CDC::GetWindowOrg() const
#ifdef _NTWIN
	{ POINT point;
		VERIFY(::GetWindowOrgEx(m_hDC, &point));
		return point; }
#else
	{ return ::GetWindowOrg(m_hDC); }
#endif
inline CPoint CDC::SetWindowOrg(int x, int y)
#ifdef _NTWIN
	{ POINT point;
		VERIFY(::SetWindowOrgEx(m_hDC, x, y, &point));
		return point; }
#else
	{ return ::SetWindowOrg(m_hDC, x, y); }
#endif
inline CPoint CDC::SetWindowOrg(POINT point)
#ifdef _NTWIN
	{ VERIFY(::SetWindowOrgEx(m_hDC, point.x, point.y, &point));
		return point; }
#else
	{ return ::SetWindowOrg(m_hDC, point.x, point.y); }
#endif
inline CPoint CDC::OffsetWindowOrg(int nWidth, int nHeight)
#ifdef _NTWIN
	{ POINT point;
		VERIFY(::OffsetWindowOrgEx(m_hDC, nWidth, nHeight, &point));
		return point; }
#else
	{ return ::OffsetWindowOrg(m_hDC, nWidth, nHeight); }
#endif
inline CSize CDC::SetWindowExt(SIZE size)
#ifdef _NTWIN
	{ VERIFY(::SetWindowExtEx(m_hDC, size.cx, size.cy, &size));
		return size; }
#else
	{ return ::SetWindowExt(m_hDC, size.cx, size.cy); }
#endif
inline CSize CDC::ScaleWindowExt(int xNum, int xDenom, int yNum, int yDenom)
#ifdef _NTWIN
	{ SIZE size;
		VERIFY(::ScaleWindowExtEx(m_hDC, xNum, xDenom, yNum, yDenom, &size));
		return size; }
#else
	{ return ::ScaleWindowExt(m_hDC, xNum, xDenom, yNum, yDenom);}
#endif

inline void CDC::DPtoLP(LPPOINT lpPoints, int nCount /* = 1 */) const
	{ VERIFY(::DPtoLP(m_hDC, lpPoints, nCount)); }
inline void CDC::DPtoLP(LPRECT lpRect) const
	{ VERIFY(::DPtoLP(m_hDC, (LPPOINT)lpRect, 2)); }
inline void CDC::LPtoDP(LPPOINT lpPoints, int nCount /* = 1 */) const
	{ VERIFY(::LPtoDP(m_hDC, lpPoints, nCount)); }
inline void CDC::LPtoDP(LPRECT lpRect) const
	{ VERIFY(::LPtoDP(m_hDC, (LPPOINT)lpRect, 2)); }
inline BOOL CDC::FillRgn(CRgn* pRgn, CBrush* pBrush)
	{ return ::FillRgn(m_hDC, (HRGN)pRgn->m_hObject, (HBRUSH)pBrush->m_hObject); }
inline BOOL CDC::FrameRgn(CRgn* pRgn, CBrush* pBrush, int nWidth, int nHeight)
	{ return ::FrameRgn(m_hDC, (HRGN)pRgn->m_hObject, (HBRUSH)pBrush->m_hObject,
		nWidth, nHeight); }
inline BOOL CDC::InvertRgn(CRgn* pRgn)
	{ return ::InvertRgn(m_hDC, (HRGN)pRgn->m_hObject); }
inline BOOL CDC::PaintRgn(CRgn* pRgn)
	{ return ::PaintRgn(m_hDC, (HRGN)pRgn->m_hObject); }
inline int CDC::GetClipBox(LPRECT lpRect) const
	{ return ::GetClipBox(m_hDC, lpRect); }
inline int CDC::SelectClipRgn(CRgn* pRgn)
	{ return ::SelectClipRgn(m_hDC, (HRGN)pRgn->GetSafeHandle()); }
inline int CDC::ExcludeClipRect(int x1, int y1, int x2, int y2)
	{ return ::ExcludeClipRect(m_hDC, x1, y1, x2, y2); }
inline int CDC::ExcludeClipRect(LPRECT lpRect)
	{ return ::ExcludeClipRect(m_hDC, lpRect->left, lpRect->top,
		lpRect->right, lpRect->bottom); }
inline int CDC::IntersectClipRect(int x1, int y1, int x2, int y2)
	{ return ::IntersectClipRect(m_hDC, x1, y1, x2, y2); }
inline int CDC::IntersectClipRect(LPRECT lpRect)
	{ return ::IntersectClipRect(m_hDC, lpRect->left, lpRect->top,
		lpRect->right, lpRect->bottom); }
inline int CDC::OffsetClipRgn(int x, int y)
	{ return ::OffsetClipRgn(m_hDC, x, y); }
inline int CDC::OffsetClipRgn(SIZE size)
	{ return ::OffsetClipRgn(m_hDC, size.cx, size.cy); }
inline BOOL CDC::PtVisible(int x, int y) const
	{ return ::PtVisible(m_hDC, x, y); }
inline BOOL CDC::PtVisible(POINT point) const
	{ return ::PtVisible(m_hDC, point.x, point.y); }
inline BOOL CDC::RectVisible(LPRECT lpRect) const
	{ return ::RectVisible(m_hDC, lpRect); }
inline CPoint CDC::GetCurrentPosition() const
#ifdef _NTWIN
	{ POINT point;
		VERIFY(::GetCurrentPositionEx(m_hDC, &point));
		return point; }
#else
	{ return ::GetCurrentPosition(m_hDC); }
#endif
inline CPoint CDC::MoveTo(int x, int y)
#ifdef _NTWIN
	{ POINT point;
		VERIFY(::MoveToEx(m_hDC, x, y, &point));
		return point; }
#else
	{ return ::MoveTo(m_hDC, x, y); }
#endif
inline CPoint CDC::MoveTo(POINT point)
#ifdef _NTWIN
	{ VERIFY(::MoveToEx(m_hDC, point.x, point.y, &point));
		return point; }
#else
	{ return ::MoveTo(m_hDC, point.x, point.y); }
#endif
inline BOOL CDC::LineTo(int x, int y)
	{ return ::LineTo(m_hDC, x, y); }
inline BOOL CDC::LineTo(POINT point)
	{ return ::LineTo(m_hDC, point.x, point.y); }
inline BOOL CDC::Arc(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
	{ return ::Arc(m_hDC, x1, y1, x2, y2, x3, y3, x4, y4); }
inline BOOL CDC::Arc(LPRECT lpRect, POINT ptStart, POINT ptEnd)
	{ return ::Arc(m_hDC, lpRect->left, lpRect->top,
		lpRect->right, lpRect->bottom, ptStart.x, ptStart.y,
		ptEnd.x, ptEnd.y); }
inline BOOL CDC::Polyline(LPPOINT lpPoints, int nCount)
	{ return ::Polyline(m_hDC, lpPoints, nCount); }
inline void CDC::FillRect(LPRECT lpRect, CBrush* pBrush)
	{ ::FillRect(m_hDC, lpRect, (HBRUSH)pBrush->m_hObject); }
inline void CDC::FrameRect(LPRECT lpRect, CBrush* pBrush)
	{ ::FrameRect(m_hDC, lpRect, (HBRUSH)pBrush->m_hObject); }
inline void CDC::InvertRect(LPRECT lpRect)
	{ ::InvertRect(m_hDC, lpRect); }
inline BOOL CDC::DrawIcon(int x, int y, HICON hIcon)
	{ return ::DrawIcon(m_hDC, x, y, hIcon); }
inline BOOL CDC::DrawIcon(POINT point, HICON hIcon)
	{ return ::DrawIcon(m_hDC, point.x, point.y, hIcon); }
inline BOOL CDC::Chord(int x1, int y1, int x2, int y2, int x3, int y3,
	int x4, int y4)
	{ return ::Chord(m_hDC, x1, y1, x2, y2, x3, y3, x4, y4); }
inline BOOL CDC::Chord(LPRECT lpRect, POINT ptStart, POINT ptEnd)
	{ return ::Chord(m_hDC, lpRect->left, lpRect->top,
		lpRect->right, lpRect->bottom, ptStart.x, ptStart.y,
		ptEnd.x, ptEnd.y); }
inline void CDC::DrawFocusRect(LPRECT lpRect)
	{ ::DrawFocusRect(m_hDC, lpRect); }
inline BOOL CDC::Ellipse(int x1, int y1, int x2, int y2)
	{ return ::Ellipse(m_hDC, x1, y1, x2, y2); }
inline BOOL CDC::Ellipse(LPRECT lpRect)
	{ return ::Ellipse(m_hDC, lpRect->left, lpRect->top,
		lpRect->right, lpRect->bottom); }
inline BOOL CDC::Pie(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
	{ return ::Pie(m_hDC, x1, y1, x2, y2, x3, y3, x4, y4); }
inline BOOL CDC::Pie(LPRECT lpRect, POINT ptStart, POINT ptEnd)
	{ return ::Pie(m_hDC, lpRect->left, lpRect->top,
		lpRect->right, lpRect->bottom, ptStart.x, ptStart.y,
		ptEnd.x, ptEnd.y); }
inline BOOL CDC::Polygon(LPPOINT lpPoints, int nCount)
	{ return ::Polygon(m_hDC, lpPoints, nCount); }
inline BOOL CDC::PolyPolygon(LPPOINT lpPoints, LPINT lpPolyCounts, int nCount)
	{ return ::PolyPolygon(m_hDC, lpPoints, lpPolyCounts, nCount); }
inline BOOL CDC::Rectangle(int x1, int y1, int x2, int y2)
	{ return ::Rectangle(m_hDC, x1, y1, x2, y2); }
inline BOOL CDC::Rectangle(LPRECT lpRect)
	{ return ::Rectangle(m_hDC, lpRect->left, lpRect->top,
		lpRect->right, lpRect->bottom); }
inline BOOL CDC::RoundRect(int x1, int y1, int x2, int y2, int x3, int y3)
	{ return ::RoundRect(m_hDC, x1, y1, x2, y2, x3, y3); }
inline BOOL CDC::RoundRect(LPRECT lpRect, POINT point)
	{ return ::RoundRect(m_hDC, lpRect->left, lpRect->top,
		lpRect->right, lpRect->bottom, point.x, point.y); }
inline BOOL CDC::PatBlt(int x, int y, int nWidth, int nHeight, DWORD dwRop)
	{ return ::PatBlt(m_hDC, x, y, nWidth, nHeight, dwRop); }
inline BOOL CDC::BitBlt(int x, int y, int nWidth, int nHeight, CDC* pSrcDC,
	int xSrc, int ySrc, DWORD dwRop)
	{ return ::BitBlt(m_hDC, x, y, nWidth, nHeight,
		pSrcDC->GetSafeHdc(), xSrc, ySrc, dwRop); }
inline BOOL CDC::StretchBlt(int x, int y, int nWidth, int nHeight, CDC* pSrcDC,
	int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, DWORD dwRop)
	{ return ::StretchBlt(m_hDC, x, y, nWidth, nHeight,
		pSrcDC->GetSafeHdc(), xSrc, ySrc, nSrcWidth, nSrcHeight,
		dwRop); }
inline DWORD CDC::GetPixel(int x, int y) const
	{ return ::GetPixel(m_hDC, x, y); }
inline DWORD CDC::GetPixel(POINT point) const
	{ return ::GetPixel(m_hDC, point.x, point.y); }
inline DWORD CDC::SetPixel(int x, int y, DWORD crColor)
	{ return ::SetPixel(m_hDC, x, y, crColor); }
inline DWORD CDC::SetPixel(POINT point, DWORD crColor)
	{ return ::SetPixel(m_hDC, point.x, point.y, crColor); }
inline BOOL CDC::FloodFill(int x, int y, DWORD crColor)
	{ return ::FloodFill(m_hDC, x, y, crColor); }
inline BOOL CDC::ExtFloodFill(int x, int y, DWORD crColor, UINT nFillType)
	{ return ::ExtFloodFill(m_hDC, x, y, crColor, nFillType); }
inline BOOL CDC::TextOut(int x, int y, const CString& str)
	{ return ::TextOut(m_hDC, x, y, (const char*)str, str.GetLength()); }
inline BOOL CDC::TextOut(int x, int y, LPCSTR lpString, int nCount)
	{ return ::TextOut(m_hDC, x, y, lpString, nCount); }
inline BOOL CDC::ExtTextOut(int x, int y, UINT nOptions, LPRECT lpRect,
	LPCSTR lpString, UINT nCount, LPINT lpDxWidths)
	{ return ::ExtTextOut(m_hDC, x, y, nOptions, lpRect,
		lpString, nCount, lpDxWidths); }
inline CSize CDC::TabbedTextOut(int x, int y, LPCSTR lpString, int nCount,
	int nTabPositions, LPINT lpnTabStopPositions, int nTabOrigin)
	{ return ::TabbedTextOut(m_hDC, x, y, (LPSTR)lpString, nCount,
		nTabPositions, lpnTabStopPositions, nTabOrigin); }
inline int CDC::DrawText(LPCSTR lpString, int nCount, LPRECT lpRect,
		UINT nFormat)
	{ return ::DrawText(m_hDC, lpString, nCount, lpRect, nFormat); }
inline CSize CDC::GetTextExtent(LPCSTR lpString, int nCount) const
#ifdef _NTWIN
	{ SIZE size;
		VERIFY(::GetTextExtentPoint(m_hDC, lpString, nCount, &size));
		return size; }
#else
	{ return ::GetTextExtent(m_hDC, lpString, nCount); }
#endif
inline CSize CDC::GetTabbedTextExtent(LPCSTR lpString, int nCount,
	int nTabPositions, LPINT lpnTabStopPositions) const
	{ return ::GetTabbedTextExtent(m_hDC, lpString, nCount,
		nTabPositions, lpnTabStopPositions); }
inline BOOL CDC::GrayString(CBrush* pBrush,
	BOOL (FAR PASCAL EXPORT* lpfnOutput)(HDC, DWORD, int),
		DWORD lpData, int nCount,
		int x, int y, int nWidth, int nHeight)
	{ return ::GrayString(m_hDC, (HBRUSH)pBrush->m_hObject,
		(GRAYSTRINGPROC)lpfnOutput, lpData, nCount, x, y, nWidth, nHeight); }
inline UINT CDC::GetTextAlign() const
	{ return ::GetTextAlign(m_hDC); }
inline UINT CDC::SetTextAlign(UINT nFlags)
	{ return ::SetTextAlign(m_hDC, nFlags); }
inline int CDC::GetTextFace(int nCount, LPSTR lpFacename) const
	{ return ::GetTextFace(m_hDC, nCount, lpFacename); }
inline BOOL CDC::GetTextMetrics(LPTEXTMETRIC lpMetrics) const
	{ return ::GetTextMetrics(m_hDC, lpMetrics); }
inline int CDC::SetTextJustification(int nBreakExtra, int nBreakCount)
	{ return ::SetTextJustification(m_hDC, nBreakExtra,
		nBreakCount); }
inline int CDC::GetTextCharacterExtra() const
	{ return ::GetTextCharacterExtra(m_hDC); }
inline int CDC::SetTextCharacterExtra(int nCharExtra)
	{ return ::SetTextCharacterExtra(m_hDC, nCharExtra); }
inline BOOL CDC::GetCharWidth(UINT nFirstChar, UINT nLastChar, LPINT lpBuffer) const
	{ return ::GetCharWidth(m_hDC, nFirstChar, nLastChar,
		lpBuffer); }
inline DWORD CDC::SetMapperFlags(DWORD dwFlag)
	{ return ::SetMapperFlags(m_hDC, dwFlag); }
inline CSize CDC::GetAspectRatioFilter() const
#ifdef _NTWIN
	{ SIZE size;
		VERIFY(::GetAspectRatioFilterEx(m_hDC, &size));
		return size; }
#else
	{ return ::GetAspectRatioFilter(m_hDC); }
#endif
inline BOOL CDC::ScrollDC(int dx, int dy, LPRECT lpRectScroll, LPRECT lpRectClip,
		CRgn* pRgnUpdate, LPRECT lpRectUpdate)
	{ return ::ScrollDC(m_hDC, dx, dy, lpRectScroll,
		lpRectClip, (HRGN)pRgnUpdate->GetSafeHandle(), lpRectUpdate); }
inline BOOL CDC::PlayMetaFile(HANDLE hMF)
	{ return ::PlayMetaFile(m_hDC, (HMETAFILE)hMF); }
inline CSize CDC::GetWindowExt() const
#ifdef _NTWIN
	{ SIZE size;
		VERIFY(::GetWindowExtEx(m_hDC, &size));
		return size; }
#else
	{ return ::GetWindowExt(m_hDC); }
#endif
inline CSize CDC::SetWindowExt(int x, int y)
#ifdef _NTWIN
	{ SIZE size;
		VERIFY(::SetWindowExtEx(m_hDC, x, y, &size));
		return size; }
#else
	{ return ::SetWindowExt(m_hDC, x, y); }
#endif

// Printer Escape Functions
inline int CDC::Escape(int nEscape, int nCount, LPCSTR lpInData, void FAR* lpOutData)
	{ return ::Escape(m_hDC, nEscape, nCount, lpInData, lpOutData);}

#ifndef _NTWIN
inline int CDC::StartDoc(LPCSTR pDocName)
	{ return ::Escape(m_hDC, STARTDOC, _fstrlen(pDocName),
		pDocName, NULL);}
#endif

#if (WINVER >= 0x030a)
#ifndef _NTWIN
inline BOOL CDC::QueryAbort() const
	{ return ::QueryAbort(m_hDC, 0); }
#else
inline BOOL CDC::QueryAbort() const
	{ return TRUE; }	// NOTE: Win32 API does not support QueryAbort.
#endif
inline int CDC::StartDoc(LPDOCINFO lpDocInfo)
	{ return ::StartDoc(m_hDC, lpDocInfo);}
#endif

#if (WINVER >= 0x030a) || defined(_NTWIN)
inline int CDC::StartPage()
	{ return ::StartPage(m_hDC); }
inline int CDC::EndPage()
	{ return ::EndPage(m_hDC);}
inline int CDC::SetAbortProc(BOOL (FAR PASCAL EXPORT* lpfn)(HDC, int))
	{ return ::SetAbortProc(m_hDC, (ABORTPROC)lpfn);}
inline int CDC::AbortDoc()
	{ return ::AbortDoc(m_hDC);}
inline int CDC::EndDoc()
	{ return ::EndDoc(m_hDC);}
#else
inline int CDC::StartPage()
	{ return 0; /* not implemented */ }
inline int CDC::EndPage()
	{ return ::Escape(m_hDC, NEWFRAME, 0, NULL, NULL);}
inline int CDC::SetAbortProc(BOOL (FAR PASCAL EXPORT* lpfn)(HDC, int))
	{ return ::Escape(m_hDC, SETABORTPROC, 0, (LPCSTR)lpfn, NULL);}
inline int CDC::AbortDoc()
	{ return ::Escape(m_hDC, ABORTDOC, 0, NULL, NULL);}
inline int CDC::EndDoc()
	{ return ::Escape(m_hDC, ENDDOC, 0, NULL, NULL);}
#endif

// CDC 3.1 Specific functions
#if (WINVER >= 0x030a)
inline UINT CDC::SetBoundsRect(const RECT FAR* lpRectBounds, UINT flags)
	{ return ::SetBoundsRect(m_hDC, lpRectBounds, flags); }
inline UINT CDC::GetBoundsRect(LPRECT lpRectBounds, UINT flags)
	{ return ::GetBoundsRect(m_hDC, lpRectBounds, flags); }
inline UINT CDC::GetOutlineTextMetrics(UINT cbData, OUTLINETEXTMETRIC FAR* lpotm) const
	{ return ::GetOutlineTextMetrics(m_hDC, cbData, lpotm); }
inline BOOL CDC::GetCharABCWidths(UINT nFirst, UINT nLast, LPABC lpabc) const
	{ return ::GetCharABCWidths(m_hDC, nFirst, nLast, lpabc); }
inline DWORD CDC::GetFontData(DWORD dwTable, DWORD dwOffset, LPVOID lpData, 
	DWORD cbData) const
	{ return ::GetFontData(m_hDC, dwTable, dwOffset, lpData, cbData); }
inline int CDC::GetKerningPairs(int nPairs, KERNINGPAIR FAR* lpkrnpair) const
	{ return ::GetKerningPairs(m_hDC, nPairs, lpkrnpair); }
inline DWORD CDC::GetGlyphOutline(UINT nChar, UINT nFormat, GLYPHMETRICS FAR* lpgm, 
		DWORD cbBuffer, void FAR* lpBuffer, const MAT2 FAR* lpmat2) const
	{ return ::GetGlyphOutline(m_hDC, nChar, nFormat, 
			lpgm, cbBuffer, lpBuffer, lpmat2); }
#endif /* WINVER >= 0x030a */

// CMetaFileDC inline functions
inline CMetaFileDC::CMetaFileDC() 
	{ }
inline BOOL CMetaFileDC::Create(LPCSTR lpFilename /* = NULL */)
	{ return Attach(::CreateMetaFile(lpFilename)); }
inline HANDLE   CMetaFileDC::Close()
	{ return ::CloseMetaFile(Detach()); }
inline BOOL CMetaFileDC::SelectObject(CGdiObject* pObject)
	{ return (BOOL)::SelectObject(m_hDC, pObject->m_hObject); }
inline BOOL CMetaFileDC::SelectStockObject(int nIndex)
	{ ASSERT(::GetStockObject(nIndex) != NULL);
		return (BOOL)::SelectObject(m_hDC, ::GetStockObject(nIndex)); }


// CMenu inline functions
inline CMenu::CMenu() 
	{ m_hMenu = NULL; }
inline BOOL CMenu::CreateMenu()
	{ return Attach(::CreateMenu()); }
inline BOOL CMenu::CreatePopupMenu()
	{ return Attach(::CreatePopupMenu()); }
inline HMENU CMenu::GetSafeHmenu() const
	{ return this == NULL ? NULL : m_hMenu; }
inline BOOL CMenu::DeleteMenu(UINT nPosition, UINT nFlags)
	{ return ::DeleteMenu(m_hMenu, nPosition, nFlags); }
inline BOOL CMenu::AppendMenu(UINT nFlags, UINT nIDNewItem /* = 0 */, LPCSTR lpNewItem /* = NULL */)
	{ return ::AppendMenu(m_hMenu, nFlags, nIDNewItem,  lpNewItem); }
inline BOOL CMenu::AppendMenu(UINT nFlags, UINT nIDNewItem, const CBitmap* pBmp)
	{ return ::AppendMenu(m_hMenu, nFlags | MF_BITMAP, nIDNewItem,
#ifndef _NTWIN
		MAKEINTRESOURCE(pBmp->m_hObject)); }
#else
		(LPCSTR)pBmp->m_hObject); }
#endif
inline BOOL CMenu::CheckMenuItem(UINT nIDCheckItem, UINT nCheck)
	{ return ::CheckMenuItem(m_hMenu, nIDCheckItem, nCheck); }
inline BOOL CMenu::EnableMenuItem(UINT nIDEnableItem, UINT nEnable)
	{ return ::EnableMenuItem(m_hMenu, nIDEnableItem, nEnable); }
inline UINT CMenu::GetMenuItemCount() const
	{ return ::GetMenuItemCount(m_hMenu); }
inline UINT CMenu::GetMenuItemID(int nPos) const
	{ return ::GetMenuItemID(m_hMenu, nPos); }
inline UINT CMenu::GetMenuState(UINT nID, UINT nFlags) const
	{ return ::GetMenuState(m_hMenu, nID, nFlags); }
inline int CMenu::GetMenuString(UINT nIDItem, LPSTR lpString, int nMaxCount,    UINT nFlags) const
	{ return ::GetMenuString(m_hMenu, nIDItem, lpString, nMaxCount, nFlags); }
inline CMenu* CMenu::GetSubMenu(int nPos) const
	{ return CMenu::FromHandle(::GetSubMenu(m_hMenu, nPos)); }
inline BOOL CMenu::InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem /* = 0 */,
		LPCSTR lpNewItem /* = NULL */)
	{ return ::InsertMenu(m_hMenu, nPosition, nFlags, nIDNewItem, lpNewItem); }
inline BOOL CMenu::InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem, const CBitmap* pBmp)
	{ return ::InsertMenu(m_hMenu, nPosition, nFlags | MF_BITMAP, nIDNewItem,
#ifndef _NTWIN
		MAKEINTRESOURCE(pBmp->m_hObject)); }
#else
		(LPCSTR)pBmp->m_hObject); }
#endif
inline BOOL CMenu::ModifyMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem /* = 0 */, LPCSTR lpNewItem /* = NULL */)
	{ return ::ModifyMenu(m_hMenu, nPosition, nFlags, nIDNewItem, lpNewItem); }
inline BOOL CMenu::ModifyMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem, const CBitmap* pBmp)
	{ return ::ModifyMenu(m_hMenu, nPosition, nFlags | MF_BITMAP, nIDNewItem,
#ifndef _NTWIN
		MAKEINTRESOURCE(pBmp->m_hObject)); }
#else
		(LPCSTR)pBmp->m_hObject); }
#endif
inline BOOL CMenu::RemoveMenu(UINT nPosition, UINT nFlags)
	{ return ::RemoveMenu(m_hMenu, nPosition, nFlags); }
inline BOOL CMenu::SetMenuItemBitmaps(UINT nPosition, UINT nFlags,
		const CBitmap* pBmpUnchecked, const CBitmap* pBmpChecked)
	{ return ::SetMenuItemBitmaps(m_hMenu, nPosition, nFlags,
		(HBITMAP)pBmpUnchecked->GetSafeHandle(),
		(HBITMAP)pBmpChecked->GetSafeHandle()); }
inline BOOL CMenu::LoadMenu(LPCSTR lpMenuName)
	{ return Attach(::LoadMenu(AfxGetResourceHandle(), lpMenuName)); }
inline BOOL CMenu::LoadMenu(UINT nIDMenu)
	{ return Attach(::LoadMenu(AfxGetResourceHandle(),
			MAKEINTRESOURCE(nIDMenu))); }
inline BOOL CMenu::LoadMenuIndirect(const void FAR* lpMenuTemplate)
	{ return Attach(::LoadMenuIndirect(lpMenuTemplate)); }
inline BOOL CMenu::TrackPopupMenu(UINT nFlags, int x, int y,
					const CWnd* pWnd, const RECT FAR* lpRect)
	{ return ::TrackPopupMenu(m_hMenu, nFlags, x, y, 0, pWnd->m_hWnd,
			lpRect); }

// CWnd 
inline HWND CWnd::GetSafeHwnd() const
	{ return this == NULL ? NULL : m_hWnd; }
inline CWnd::CWnd() 
	{ m_hWnd = NULL; }
inline CWnd::CWnd(HWND hWnd)
	{ m_hWnd = hWnd; }
inline DWORD CWnd::GetStyle() const
	{ return (DWORD)GetWindowLong(m_hWnd, GWL_STYLE); }
inline DWORD CWnd::GetExStyle() const
	{ return (DWORD)GetWindowLong(m_hWnd, GWL_EXSTYLE); }
inline LONG CWnd::SendMessage(UINT message, UINT wParam, LONG lParam)
	{ return ::SendMessage(m_hWnd, message, wParam, lParam); }
inline BOOL CWnd::PostMessage(UINT message, UINT wParam, LONG lParam)
	{ return ::PostMessage(m_hWnd, message, wParam, lParam); }
inline void CWnd::SetWindowText(LPCSTR lpString)
	{ ::SetWindowText(m_hWnd, lpString); }
inline int CWnd::GetWindowText(LPSTR lpString, int nMaxCount) const
	{ return ::GetWindowText(m_hWnd, lpString, nMaxCount); }
inline int CWnd::GetWindowTextLength() const
	{ return ::GetWindowTextLength(m_hWnd); }
inline void CWnd::GetWindowText(CString& rString) const
	{ int nLen = GetWindowTextLength();
		GetWindowText(rString.GetBufferSetLength(nLen), nLen+1); }
inline void CWnd::SetFont(CFont* pFont, BOOL bRedraw /* = TRUE */)
	{ SendMessage(WM_SETFONT, (UINT)pFont->GetSafeHandle(), bRedraw); }
inline CFont* CWnd::GetFont()
	{ return CFont::FromHandle((HFONT)SendMessage(WM_GETFONT)); }
inline CMenu* CWnd::GetMenu() const
	{ return CMenu::FromHandle(::GetMenu(m_hWnd)); }
inline BOOL CWnd::SetMenu(CMenu* pMenu)
	{ return ::SetMenu(m_hWnd, pMenu->GetSafeHmenu()); }
inline void CWnd::DrawMenuBar()
	{ ::DrawMenuBar(m_hWnd); }
inline CMenu* CWnd::GetSystemMenu(BOOL bRevert) const
	{ return CMenu::FromHandle(::GetSystemMenu(m_hWnd, bRevert)); }
inline BOOL CWnd::HiliteMenuItem(CMenu* pMenu, UINT nIDHiliteItem, UINT nHilite)
	{ return ::HiliteMenuItem(m_hWnd, pMenu->m_hMenu, nIDHiliteItem, nHilite); }
inline int CWnd::GetDlgCtrlID() const
	{ return ::GetDlgCtrlID(m_hWnd); }
inline void CWnd::CloseWindow()
	{ ::CloseWindow(m_hWnd); }
inline BOOL CWnd::OpenIcon()
	{ return ::OpenIcon(m_hWnd); }
inline BOOL CWnd::IsIconic() const
	{ return ::IsIconic(m_hWnd); }
inline BOOL CWnd::IsZoomed() const
	{ return ::IsZoomed(m_hWnd); }
inline void CWnd::MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint /* = TRUE */)
	{ ::MoveWindow(m_hWnd, x, y, nWidth, nHeight, bRepaint); }
inline void CWnd::MoveWindow(LPRECT lpRect, BOOL bRepaint /* = TRUE */)
	{ ::MoveWindow(m_hWnd, lpRect->left, lpRect->top, lpRect->right - lpRect->left,
			lpRect->bottom - lpRect->top, bRepaint); }
inline BOOL CWnd::SetWindowPos(const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags)
#if (WINVER >= 0x030a) || defined(_NTWIN)
	{ return ::SetWindowPos(m_hWnd, pWndInsertAfter->GetSafeHwnd(),
		x, y, cx, cy, nFlags); }
#else
	{ ::SetWindowPos(m_hWnd, pWndInsertAfter->GetSafeHwnd(),
		x, y, cx, cy, nFlags); return TRUE; }
#endif /* WINVER >= 0x030a */
inline UINT CWnd::ArrangeIconicWindows()
	{ return ::ArrangeIconicWindows(m_hWnd); }
inline void CWnd::BringWindowToTop()
	{ ::BringWindowToTop(m_hWnd); }
inline void CWnd::GetWindowRect(LPRECT lpRect) const
	{ ::GetWindowRect(m_hWnd, lpRect); }
inline void CWnd::GetClientRect(LPRECT lpRect) const
	{ ::GetClientRect(m_hWnd, lpRect); }
#if (WINVER >= 0x030a)
inline BOOL CWnd::GetWindowPlacement(WINDOWPLACEMENT FAR* lpwndpl) const
	{ return ::GetWindowPlacement(m_hWnd, lpwndpl); }
inline BOOL CWnd::SetWindowPlacement(const WINDOWPLACEMENT FAR* lpwndpl)
	{ return ::SetWindowPlacement(m_hWnd, lpwndpl); }
inline void CWnd::MapWindowPoints(CWnd* pwndTo, LPPOINT lpPoint, UINT nCount) const
	{ ::MapWindowPoints(m_hWnd, pwndTo->GetSafeHwnd(), lpPoint, nCount); }
inline void CWnd::MapWindowPoints(CWnd* pwndTo, LPRECT lpRect) const
	{ ::MapWindowPoints(m_hWnd, pwndTo->GetSafeHwnd(), (LPPOINT)lpRect, 2); }
#endif /* WINVER >= 0x030a */
inline void CWnd::ClientToScreen(LPPOINT lpPoint) const
	{ ::ClientToScreen(m_hWnd, lpPoint); }
inline void CWnd::ClientToScreen(LPRECT lpRect) const
	{ ::ClientToScreen(m_hWnd, (LPPOINT)lpRect);
		::ClientToScreen(m_hWnd, ((LPPOINT)lpRect)+1); }
inline void CWnd::ScreenToClient(LPPOINT lpPoint) const
	{ ::ScreenToClient(m_hWnd, lpPoint); }
inline void CWnd::ScreenToClient(LPRECT lpRect) const
	{ ::ScreenToClient(m_hWnd, (LPPOINT)lpRect);
		::ScreenToClient(m_hWnd, ((LPPOINT)lpRect)+1); }
inline CDC* CWnd::BeginPaint(LPPAINTSTRUCT lpPaint)
	{ return CDC::FromHandle(::BeginPaint(m_hWnd, lpPaint)); }
inline void CWnd::EndPaint(LPPAINTSTRUCT lpPaint)
	{ ::EndPaint(m_hWnd, lpPaint); }
inline CDC* CWnd::GetDC()
	{ return CDC::FromHandle(::GetDC(m_hWnd)); }
#if (WINVER >= 0x030a)
inline CDC* CWnd::GetDCEx(CRgn* prgnClip, DWORD flags)
	{ return CDC::FromHandle(::GetDCEx(m_hWnd, (HRGN)prgnClip->GetSafeHandle(), flags)); }
#endif /* WINVER >= 0x030a */
inline CDC* CWnd::GetWindowDC()
	{ return CDC::FromHandle(::GetWindowDC(m_hWnd)); }
inline int CWnd::ReleaseDC(CDC* pDC)
	{ return ::ReleaseDC(m_hWnd, pDC->m_hDC); }
inline void CWnd::UpdateWindow()
	{ ::UpdateWindow(m_hWnd); }
inline void CWnd::SetRedraw(BOOL bRedraw /* = TRUE */)
	{ ::SendMessage(m_hWnd, WM_SETREDRAW, bRedraw, 0); }
inline BOOL CWnd::GetUpdateRect(LPRECT lpRect, BOOL bErase /* = FALSE */)
	{ return ::GetUpdateRect(m_hWnd, lpRect, bErase); }
inline int CWnd::GetUpdateRgn(CRgn* pRgn, BOOL bErase /* = FALSE */)
	{ return ::GetUpdateRgn(m_hWnd, (HRGN)pRgn->m_hObject, bErase); }
inline void CWnd::Invalidate(BOOL bErase /* = TRUE  */)
	{ ::InvalidateRect(m_hWnd, (LPRECT)NULL, bErase); }
inline void CWnd::InvalidateRect(LPRECT lpRect, BOOL bErase /* = TRUE  */)
	{ ::InvalidateRect(m_hWnd, lpRect, bErase); }
inline void CWnd::InvalidateRgn(CRgn* pRgn, BOOL bErase /* = TRUE  */)
	{ ::InvalidateRgn(m_hWnd, (HRGN)pRgn->GetSafeHandle(), bErase); }
inline void CWnd::ValidateRect(LPRECT lpRect)
	{ ::ValidateRect(m_hWnd, lpRect); }
inline void CWnd::ValidateRgn(CRgn* pRgn)
	{ ::ValidateRgn(m_hWnd, (HRGN)pRgn->GetSafeHandle()); }
inline BOOL CWnd::ShowWindow(int nCmdShow)
	{ return ::ShowWindow(m_hWnd, nCmdShow); }
inline BOOL CWnd::IsWindowVisible() const
	{ return ::IsWindowVisible(m_hWnd); }
inline void CWnd::ShowOwnedPopups(BOOL bShow /* = TRUE */)
	{ ::ShowOwnedPopups(m_hWnd, bShow); }
#if (WINVER >= 0x030a)
inline BOOL CWnd::LockWindowUpdate()
	{ return ::LockWindowUpdate(m_hWnd); }
inline BOOL CWnd::RedrawWindow(const RECT FAR* lpRectUpdate, 
			CRgn* prgnUpdate, 
			UINT flags /* = RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE */)
	{ return ::RedrawWindow(m_hWnd, lpRectUpdate, (HRGN)prgnUpdate->GetSafeHandle(), flags); }
inline BOOL CWnd::EnableScrollBar(int nBar, 
		UINT nArrowFlags /* = ESB_ENABLE_BOTH */)
	{ return (BOOL)::EnableScrollBar(m_hWnd, nBar, nArrowFlags); }
#endif /* WINVER >= 0x030a */
inline UINT CWnd::SetTimer(int nIDEvent, UINT nElapse,
		UINT (FAR PASCAL EXPORT* lpfnTimer)(HWND, UINT, int, DWORD))
	{ return ::SetTimer(m_hWnd, nIDEvent, nElapse,
		(TIMERPROC)lpfnTimer); }
inline BOOL CWnd::KillTimer(int nIDEvent)
	{ return ::KillTimer(m_hWnd, nIDEvent); }
inline BOOL CWnd::IsWindowEnabled() const
	{ return ::IsWindowEnabled(m_hWnd); }
inline BOOL CWnd::EnableWindow(BOOL bEnable /* = TRUE */)
	{ return ::EnableWindow(m_hWnd, bEnable); }
inline CWnd* CWnd::GetActiveWindow()
	{ return CWnd::FromHandle(::GetActiveWindow()); }
inline CWnd* CWnd::SetActiveWindow()
	{ return CWnd::FromHandle(::SetActiveWindow(m_hWnd)); }
inline CWnd* CWnd::GetCapture()
	{ return CWnd::FromHandle(::GetCapture()); }
inline CWnd* CWnd::SetCapture()
	{ return CWnd::FromHandle(::SetCapture(m_hWnd)); }
inline CWnd* CWnd::GetFocus()
	{ return CWnd::FromHandle(::GetFocus()); }
inline CWnd* CWnd::SetFocus()
	{ return CWnd::FromHandle(::SetFocus(m_hWnd)); }
inline CWnd* CWnd::SetSysModalWindow()
	{ return CWnd::FromHandle(::SetSysModalWindow(m_hWnd)); }
inline CWnd* CWnd::GetSysModalWindow()
	{ return CWnd::FromHandle(::GetSysModalWindow()); }
inline CWnd* CWnd::GetDesktopWindow()
	{ return CWnd::FromHandle(::GetDesktopWindow()); }
inline void CWnd::CheckDlgButton(int nIDButton, UINT nCheck)
	{ ::CheckDlgButton(m_hWnd, nIDButton, nCheck); }
inline void CWnd::CheckRadioButton(int nIDFirstButton, int nIDLastButton,
		int nIDCheckButton)
	{ ::CheckRadioButton(m_hWnd, nIDFirstButton, nIDLastButton, nIDCheckButton); }
inline int CWnd::DlgDirList(LPSTR lpPathSpec, int nIDListBox,
		int nIDStaticPath, UINT nFileType)
	{ return ::DlgDirList(m_hWnd, lpPathSpec, nIDListBox,
			nIDStaticPath, nFileType); }
inline int CWnd::DlgDirListComboBox(LPSTR lpPathSpec, int nIDComboBox,
		int nIDStaticPath, UINT nFileType)
	{ return ::DlgDirListComboBox(m_hWnd, lpPathSpec,
			nIDComboBox, nIDStaticPath, nFileType); }
inline BOOL CWnd::DlgDirSelect(LPSTR lpString, int nIDListBox)
#ifdef _NTWIN
	{ return ::DlgDirSelectEx(m_hWnd, lpString, _MAX_PATH, nIDListBox); }
#else
	{ return ::DlgDirSelect(m_hWnd, lpString, nIDListBox); }
#endif
inline BOOL CWnd::DlgDirSelectComboBox(LPSTR lpString, int nIDComboBox)
#ifdef _NTWIN
	{ return ::DlgDirSelectComboBoxEx(m_hWnd, lpString, _MAX_PATH, nIDComboBox);}
#else
	{ return ::DlgDirSelectComboBox(m_hWnd, lpString, nIDComboBox);}
#endif
inline CWnd* CWnd::GetDlgItem(int nID) const
	{ return CWnd::FromHandle(::GetDlgItem(m_hWnd, nID)); }
inline UINT CWnd::GetDlgItemInt(int nID, BOOL* lpTrans /* = NULL */,
		BOOL bSigned /* = TRUE */) const
	{ return ::GetDlgItemInt(m_hWnd, nID, lpTrans, bSigned);}
inline int CWnd::GetDlgItemText(int nID, LPSTR lpStr, int nMaxCount) const
	{ return ::GetDlgItemText(m_hWnd, nID, lpStr, nMaxCount);}
inline CWnd* CWnd::GetNextDlgGroupItem(CWnd* pWndCtl, BOOL bPrevious /* = FALSE */) const
	{ return CWnd::FromHandle(::GetNextDlgGroupItem(m_hWnd,
			pWndCtl->m_hWnd, bPrevious)); }
inline CWnd* CWnd::GetNextDlgTabItem(CWnd* pWndCtl, BOOL bPrevious /* = FALSE */) const
	{ return CWnd::FromHandle(::GetNextDlgTabItem(m_hWnd,
			pWndCtl->m_hWnd, bPrevious)); }
inline UINT CWnd::IsDlgButtonChecked(int nIDButton) const
	{ return ::IsDlgButtonChecked(m_hWnd, nIDButton); }
inline LONG CWnd::SendDlgItemMessage(int nID, UINT message, UINT wParam /* = 0 */, LONG lParam /* = 0 */)
	{ return ::SendDlgItemMessage(m_hWnd, nID, message, wParam, lParam); }
inline void CWnd::SetDlgItemInt(int nID, UINT nValue, BOOL bSigned /* = TRUE */)
	{ ::SetDlgItemInt(m_hWnd, nID, nValue, bSigned); }
inline void CWnd::SetDlgItemText(int nID, LPCSTR lpString)
	{ ::SetDlgItemText(m_hWnd, nID, lpString); }
inline int CWnd::GetScrollPos(int nBar) const
	{ return ::GetScrollPos(m_hWnd, nBar); }
inline void CWnd::GetScrollRange(int nBar, LPINT lpMinPos, LPINT lpMaxPos) const
	{ ::GetScrollRange(m_hWnd, nBar, lpMinPos, lpMaxPos); }
inline void CWnd::ScrollWindow(int xAmount, int yAmount,
		const RECT FAR* lpRect /* = NULL */,
		const RECT FAR* lpClipRect /* = NULL */)
	{::ScrollWindow(m_hWnd, xAmount, yAmount, lpRect, lpClipRect); }
#if (WINVER >= 0x030a)
inline int CWnd::ScrollWindowEx(int dx, int dy,
				const RECT FAR* lpScrollRect, const RECT FAR* lpClipRect,
				CRgn* prgnUpdate, LPRECT lpUpdateRect, UINT flags)
	{ return ::ScrollWindowEx(m_hWnd, dx, dy, lpScrollRect, lpClipRect,
			(HRGN)prgnUpdate->GetSafeHandle(), lpUpdateRect, flags); }
#endif /* WINVER >= 0x030a */
inline int CWnd::SetScrollPos(int nBar, int nPos, BOOL bRedraw /* = TRUE */)
	{ return ::SetScrollPos(m_hWnd, nBar, nPos, bRedraw); }
inline void CWnd::SetScrollRange(int nBar, int nMinPos, int nMaxPos,
	BOOL bRedraw /* = TRUE */)
	{ ::SetScrollRange(m_hWnd, nBar, nMinPos, nMaxPos, bRedraw); }
inline void CWnd::ShowScrollBar(UINT nBar, BOOL bShow /* = TRUE */)
	{ ::ShowScrollBar(m_hWnd, nBar, bShow); }
inline CWnd* CWnd::ChildWindowFromPoint(POINT point) const
	{ return CWnd::FromHandle(::ChildWindowFromPoint(m_hWnd, point)); }
inline CWnd* CWnd::FindWindow(LPCSTR lpClassName, LPCSTR lpWindowName)
	{ return CWnd::FromHandle(
			::FindWindow(lpClassName, lpWindowName)); }
inline CWnd* CWnd::GetNextWindow(UINT nFlag /* = GW_HWNDNEXT */) const
	{ return CWnd::FromHandle(::GetNextWindow(m_hWnd, nFlag)); }
inline CWnd* CWnd::GetTopWindow() const
	{ return CWnd::FromHandle(::GetTopWindow(m_hWnd)); }
inline CWnd* CWnd::GetWindow(UINT nCmd) const
	{ return CWnd::FromHandle(::GetWindow(m_hWnd, nCmd)); }
inline CWnd* CWnd::GetLastActivePopup() const
	{ return CWnd::FromHandle(::GetLastActivePopup(m_hWnd)); }
inline BOOL CWnd::IsChild(CWnd* pWnd) const
	{ return ::IsChild(m_hWnd, pWnd->GetSafeHwnd()); }
inline CWnd* CWnd::GetParent() const
	{ return CWnd::FromHandle(::GetParent(m_hWnd)); }
inline CWnd* CWnd::SetParent(CWnd* pWndNewParent)
	{ return CWnd::FromHandle(::SetParent(m_hWnd,
			pWndNewParent->GetSafeHwnd())); }
inline CWnd* CWnd::WindowFromPoint(POINT point)
	{ return CWnd::FromHandle(::WindowFromPoint(point)); }
inline BOOL CWnd::FlashWindow(BOOL bInvert)
	{ return ::FlashWindow(m_hWnd, bInvert); }
inline int CWnd::MessageBox(LPCSTR lpText, LPCSTR lpCaption /* = NULL */,
	UINT nType /* = MB_OK */)
	{ return ::MessageBox(GetSafeHwnd(), lpText, lpCaption,nType); }
inline BOOL CWnd::ChangeClipboardChain(HWND hWndNext)
	{ return ::ChangeClipboardChain(m_hWnd, hWndNext); }
inline HWND CWnd::SetClipboardViewer()
	{ return ::SetClipboardViewer(m_hWnd); }
inline BOOL CWnd::OpenClipboard()
	{ return ::OpenClipboard(m_hWnd); }
#if (WINVER >= 0x030a)
inline CWnd* CWnd::GetOpenClipboardWindow()
	{ return CWnd::FromHandle(::GetOpenClipboardWindow()); }
#endif /* WINVER >= 0x030a */
inline CWnd* CWnd::GetClipboardOwner()
	{ return CWnd::FromHandle(::GetClipboardOwner()); }
inline CWnd* CWnd::GetClipboardViewer()
	{ return CWnd::FromHandle(::GetClipboardViewer()); }
inline void CWnd::CreateCaret(CBitmap* pBitmap)
	{ ::CreateCaret(m_hWnd, (HBITMAP)pBitmap->GetSafeHandle(), 0, 0); }
inline void CWnd::CreateSolidCaret(int nWidth, int nHeight)
	{ ::CreateCaret(m_hWnd, (HBITMAP)0, nWidth, nHeight); }
inline void CWnd::CreateGrayCaret(int nWidth, int nHeight)
	{ ::CreateCaret(m_hWnd, (HBITMAP)1, nWidth, nHeight); }
inline CPoint CWnd::GetCaretPos()
	{ CPoint point; ::GetCaretPos((LPPOINT)&point); return point; }
inline void CWnd::SetCaretPos(POINT point)
	{ ::SetCaretPos(point.x, point.y); }
inline void CWnd::HideCaret()
	{ ::HideCaret(m_hWnd); }
inline void CWnd::ShowCaret()
	{ ::ShowCaret(m_hWnd); }
inline afx_msg void CWnd::OnActivate(UINT, CWnd*, BOOL)
	{ Default(); }
inline afx_msg void CWnd::OnActivateApp(BOOL, HANDLE)
	{ Default(); }
inline afx_msg void CWnd::OnCancelMode()
	{ Default(); }
inline afx_msg void CWnd::OnChildActivate()
	{ Default(); }
inline afx_msg void CWnd::OnClose()
	{ Default(); }
inline afx_msg int CWnd::OnCreate(LPCREATESTRUCT)
	{ return (int)Default(); }
inline afx_msg HBRUSH CWnd::OnCtlColor(CDC*, CWnd*, UINT)
	{ return (HBRUSH)Default(); }
inline afx_msg void CWnd::OnEnable(BOOL)
	{ Default(); }
inline afx_msg void CWnd::OnEndSession(BOOL)
	{ Default(); }
inline afx_msg void CWnd::OnEnterIdle(UINT, CWnd*)
	{ Default(); }
inline afx_msg BOOL CWnd::OnEraseBkgnd(CDC*)
	{ return (BOOL)Default(); }
inline afx_msg void CWnd::OnGetMinMaxInfo(LPPOINT)
	{ Default(); }
inline afx_msg void CWnd::OnIconEraseBkgnd(CDC*)
	{ Default(); }
inline afx_msg void CWnd::OnKillFocus(CWnd*)
	{ Default(); }
inline afx_msg LONG CWnd::OnMenuChar(UINT, UINT,    CMenu*)
	{ return Default(); }
inline afx_msg void CWnd::OnMenuSelect(UINT, UINT,  HMENU)
	{ Default(); }
inline afx_msg void CWnd::OnMove(int, int)
	{ Default(); }
inline afx_msg void CWnd::OnPaint()
	{ Default(); }
inline afx_msg void CWnd::OnParentNotify(UINT, LONG)
	{ Default(); }
inline afx_msg HCURSOR CWnd::OnQueryDragIcon()
	{ return (HCURSOR)Default(); }
inline afx_msg BOOL CWnd::OnQueryEndSession()
	{ return (BOOL)Default(); }
inline afx_msg BOOL CWnd::OnQueryNewPalette()
	{ return (BOOL)Default(); }
inline afx_msg BOOL CWnd::OnQueryOpen()
	{ return (BOOL)Default(); }
inline afx_msg void CWnd::OnSetFocus(CWnd*)
	{ Default(); }
inline afx_msg void CWnd::OnShowWindow(BOOL, UINT)
	{ Default(); }
inline afx_msg void CWnd::OnSize(UINT, int, int)
	{ Default(); }

#if (WINVER >= 0x030a)
inline afx_msg void CWnd::OnWindowPosChanging(WINDOWPOS FAR*)
	{ Default(); }
inline afx_msg void CWnd::OnWindowPosChanged(WINDOWPOS FAR*)
	{ Default(); }
inline afx_msg void CWnd::OnDropFiles(HANDLE)
	{ Default(); }
inline afx_msg void CWnd::OnPaletteIsChanging(CWnd*)
	{ Default(); }
#endif /* WINVER >= 0x030a */

inline afx_msg BOOL CWnd::OnNcActivate(BOOL)
	{ return (BOOL)Default(); }
inline afx_msg void CWnd::OnNcCalcSize(NCCALCSIZE_PARAMS FAR*)
	{ Default(); }
inline afx_msg BOOL CWnd::OnNcCreate(LPCREATESTRUCT)
	{ return (BOOL)Default(); }
inline afx_msg UINT CWnd::OnNcHitTest(CPoint)
	{ return (UINT)Default(); }
inline afx_msg void CWnd::OnNcLButtonDblClk(UINT, CPoint)
	{ Default(); }
inline afx_msg void CWnd::OnNcLButtonDown(UINT, CPoint)
	{ Default(); }
inline afx_msg void CWnd::OnNcLButtonUp(UINT, CPoint)
	{ Default(); }
inline afx_msg void CWnd::OnNcMButtonDblClk(UINT, CPoint)
	{ Default(); }
inline afx_msg void CWnd::OnNcMButtonDown(UINT, CPoint)
	{ Default(); }
inline afx_msg void CWnd::OnNcMButtonUp(UINT, CPoint)
	{ Default(); }
inline afx_msg void CWnd::OnNcMouseMove(UINT, CPoint)
	{ Default(); }
inline afx_msg void CWnd::OnNcPaint()
	{ Default(); }
inline afx_msg void CWnd::OnNcRButtonDblClk(UINT, CPoint)
	{ Default(); }
inline afx_msg void CWnd::OnNcRButtonDown(UINT, CPoint)
	{ Default(); }
inline afx_msg void CWnd::OnNcRButtonUp(UINT, CPoint)
	{ Default(); }
inline afx_msg void CWnd::OnSysChar(UINT, UINT, UINT)
	{ Default(); }
inline afx_msg void CWnd::OnSysCommand(UINT, LONG)
	{ Default(); }
inline afx_msg void CWnd::OnSysDeadChar(UINT, UINT, UINT)
	{ Default(); }
inline afx_msg void CWnd::OnSysKeyDown(UINT, UINT, UINT)
	{ Default(); }
inline afx_msg void CWnd::OnSysKeyUp(UINT, UINT, UINT)
	{ Default(); }
inline afx_msg void CWnd::OnCompacting(UINT)
	{ Default(); }
inline afx_msg void CWnd::OnDevModeChange(LPSTR)
	{ Default(); }
inline afx_msg void CWnd::OnFontChange()
	{ Default(); }
inline afx_msg void CWnd::OnPaletteChanged(CWnd*)
	{ Default(); }
inline afx_msg void CWnd::OnSpoolerStatus(UINT, UINT)
	{ Default(); }
inline afx_msg void CWnd::OnSysColorChange()
	{ Default(); }
inline afx_msg void CWnd::OnTimeChange()
	{ Default(); }
inline afx_msg void CWnd::OnWinIniChange(LPSTR)
	{ Default(); }
inline afx_msg void CWnd::OnChar(UINT, UINT, UINT)
	{ Default(); }
inline afx_msg void CWnd::OnDeadChar(UINT, UINT, UINT)
	{ Default(); }
inline afx_msg void CWnd::OnHScroll(UINT, UINT, CScrollBar*)
	{ Default(); }
inline afx_msg void CWnd::OnKeyDown(UINT, UINT, UINT)
	{ Default(); }
inline afx_msg void CWnd::OnKeyUp(UINT, UINT, UINT)
	{ Default(); }
inline afx_msg void CWnd::OnLButtonDblClk(UINT, CPoint)
	{ Default(); }
inline afx_msg void CWnd::OnLButtonDown(UINT, CPoint)
	{ Default(); }
inline afx_msg void CWnd::OnLButtonUp(UINT, CPoint)
	{ Default(); }
inline afx_msg void CWnd::OnMButtonDblClk(UINT, CPoint)
	{ Default(); }
inline afx_msg void CWnd::OnMButtonDown(UINT, CPoint)
	{ Default(); }
inline afx_msg void CWnd::OnMButtonUp(UINT, CPoint)
	{ Default(); }
inline afx_msg int CWnd::OnMouseActivate(CWnd*, UINT, UINT)
	{ return (int)Default(); }
inline afx_msg void CWnd::OnMouseMove(UINT, CPoint)
	{ Default(); }
inline afx_msg void CWnd::OnRButtonDblClk(UINT, CPoint)
	{ Default(); }
inline afx_msg void CWnd::OnRButtonDown(UINT, CPoint)
	{ Default(); }
inline afx_msg void CWnd::OnRButtonUp(UINT, CPoint)
	{ Default(); }
inline afx_msg BOOL CWnd::OnSetCursor(CWnd*, UINT, UINT)
	{ return (BOOL)Default(); }
inline afx_msg void CWnd::OnTimer(UINT)
	{ Default(); }
inline afx_msg void CWnd::OnVScroll(UINT, UINT, CScrollBar*)
	{ Default(); }
inline afx_msg void CWnd::OnInitMenu(CMenu*)
	{ Default(); }
inline afx_msg void CWnd::OnInitMenuPopup(CMenu*, UINT, BOOL)
	{ Default(); }
inline afx_msg void CWnd::OnAskCbFormatName(UINT, LPSTR)
	{ Default(); }
inline afx_msg void CWnd::OnChangeCbChain(HWND, HWND)
	{ Default(); }
inline afx_msg void CWnd::OnDestroyClipboard()
	{ Default(); }
inline afx_msg void CWnd::OnDrawClipboard()
	{ Default(); }
inline afx_msg void CWnd::OnHScrollClipboard(CWnd*, UINT, UINT)
	{ Default(); }
inline afx_msg void CWnd::OnPaintClipboard(CWnd*, HANDLE)
	{ Default(); }
inline afx_msg void CWnd::OnRenderAllFormats()
	{ Default(); }
inline afx_msg void CWnd::OnRenderFormat(UINT)
	{ Default(); }
inline afx_msg void CWnd::OnSizeClipboard(CWnd*, HANDLE)
	{ Default(); }
inline afx_msg void CWnd::OnVScrollClipboard(CWnd*, UINT, UINT)
	{ Default(); }
inline afx_msg int CWnd::OnCharToItem(UINT, CListBox*, UINT)
	{ return (int)Default(); }
inline afx_msg UINT CWnd::OnGetDlgCode()
	{ return (UINT)Default(); }
inline afx_msg int CWnd::OnVKeyToItem(UINT, CListBox*,  UINT)
	{ return (int)Default(); }
inline afx_msg void CWnd::OnMDIActivate(BOOL, CWnd*, CWnd*)
	{ Default(); }

// CDialog inline functions
inline BOOL CDialog::Create(UINT nIDTemplate, CWnd* pParentWnd /* = NULL */)
	{ return Create(MAKEINTRESOURCE(nIDTemplate), pParentWnd); }
inline void CDialog::MapDialogRect(LPRECT lpRect) const
	{ ::MapDialogRect(m_hWnd, lpRect); }
inline BOOL CDialog::IsDialogMessage(LPMSG lpMsg)
	{ return ::IsDialogMessage(m_hWnd, lpMsg); }
inline void CDialog::NextDlgCtrl() const
	{ ::SendMessage(m_hWnd, WM_NEXTDLGCTL, 0, 0); }
inline void CDialog::PrevDlgCtrl() const
	{ ::SendMessage(m_hWnd, WM_NEXTDLGCTL, 1, 0); }
inline void CDialog::GotoDlgCtrl(CWnd* pWndCtrl)
	{ ::SendMessage(m_hWnd, WM_NEXTDLGCTL, (UINT)pWndCtrl->m_hWnd, 1L); }
inline void CDialog::SetDefID(UINT nID)
	{ ::SendMessage(m_hWnd, DM_SETDEFID, nID, 0); }
inline DWORD CDialog::GetDefID()
	{ return ::SendMessage(m_hWnd, DM_GETDEFID, 0, 0); }
inline void CDialog::EndDialog(int nResult)
	{ ::EndDialog(m_hWnd, nResult); }

// Window control inline functions
inline CStatic::CStatic() 
	{ }
inline CButton::CButton() 
	{ }
#if (WINVER >= 0x030a)
inline HICON CStatic::SetIcon(HICON hIcon)
	{ return (HICON)::SendMessage(m_hWnd, STM_SETICON, (WPARAM)hIcon, 0L); }
inline HICON CStatic::GetIcon() const
	{ return (HICON)::SendMessage(m_hWnd, STM_GETICON, 0, 0L); }
#endif /* WINVER >= 0x030a */
	
inline UINT CButton::GetState() const
	{ return (UINT)::SendMessage(m_hWnd, BM_GETSTATE, 0, 0); }
inline void CButton::SetState(BOOL bHighlight)
	{ ::SendMessage(m_hWnd, BM_SETSTATE, bHighlight, 0); }
inline int CButton::GetCheck() const
	{ return (BOOL)::SendMessage(m_hWnd, BM_GETCHECK, 0, 0); }
inline void CButton::SetCheck(int nCheck)
	{ ::SendMessage(m_hWnd, BM_SETCHECK, nCheck, 0); }
inline UINT CButton::GetButtonStyle() const
	{ return (UINT)GetWindowLong(m_hWnd, GWL_STYLE) & 0xff; }
inline void CButton::SetButtonStyle(UINT nStyle, BOOL bRedraw /* = TRUE */)
	{ ::SendMessage(m_hWnd, BM_SETSTYLE, nStyle, (LONG)bRedraw); }
inline CListBox::CListBox() 
	{ }
inline int CListBox::GetCount() const
	{ return (int)::SendMessage(m_hWnd, LB_GETCOUNT, 0, 0); }
inline int CListBox::GetCurSel() const
	{ return (int)::SendMessage(m_hWnd, LB_GETCURSEL, 0, 0); }
inline int CListBox::SetCurSel(int nSelect)
	{ return (int)::SendMessage(m_hWnd, LB_SETCURSEL, nSelect, 0); }
inline int CListBox::GetHorizontalExtent() const
	{ return (int)::SendMessage(m_hWnd, LB_GETHORIZONTALEXTENT,
		0, 0); }
inline void CListBox::SetHorizontalExtent(int cxExtent)
	{ ::SendMessage(m_hWnd, LB_SETHORIZONTALEXTENT, cxExtent, 0); }
inline int CListBox::GetSelCount() const
	{ return (int)::SendMessage(m_hWnd, LB_GETSELCOUNT, 0, 0); }
inline int CListBox::GetSelItems(int nMaxItems, LPINT rgIndex) const
	{ return (int)::SendMessage(m_hWnd, LB_GETSELITEMS, nMaxItems, (LONG)rgIndex); }
inline int CListBox::GetTopIndex() const
	{ return (int)::SendMessage(m_hWnd, LB_GETTOPINDEX, 0, 0); }
inline int CListBox::SetTopIndex(int nIndex)
	{ return (int)::SendMessage(m_hWnd, LB_SETTOPINDEX, nIndex, 0);}
inline DWORD CListBox::GetItemData(int nIndex) const
	{ return ::SendMessage(m_hWnd, LB_GETITEMDATA, nIndex, 0); }
inline int CListBox::SetItemData(int nIndex, DWORD dwItemData)
	{ return (int)::SendMessage(m_hWnd, LB_SETITEMDATA, nIndex, (LONG)dwItemData); }
inline int CListBox::GetItemRect(int nIndex, LPRECT lpRect) const
	{ return (int)::SendMessage(m_hWnd, LB_GETITEMRECT, nIndex, (LONG)lpRect); }
inline int CListBox::GetSel(int nIndex) const
	{ return (int)::SendMessage(m_hWnd, LB_GETSEL, nIndex, 0); }
inline int CListBox::SetSel(int nIndex, BOOL bSelect)
	{ return (int)::SendMessage(m_hWnd, LB_SETSEL, bSelect, nIndex); }
inline int CListBox::GetText(int nIndex, LPSTR lpBuffer) const
	{ return (int)::SendMessage(m_hWnd, LB_GETTEXT, nIndex, (LONG)lpBuffer); }
inline int CListBox::GetTextLen(int nIndex) const
	{ return (int)::SendMessage(m_hWnd, LB_GETTEXTLEN, nIndex, 0); }
inline void CListBox::GetText(int nIndex, CString& rString) const
	{ GetText(nIndex, rString.GetBufferSetLength(GetTextLen(nIndex))); }
inline void CListBox::SetColumnWidth(int cxWidth)
	{ ::SendMessage(m_hWnd, LB_SETCOLUMNWIDTH, cxWidth, 0); }
inline BOOL CListBox::SetTabStops(int nTabStops, LPINT rgTabStops)
	{ return (BOOL)::SendMessage(m_hWnd, LB_SETTABSTOPS, nTabStops, (LONG)rgTabStops); }
inline void CListBox::SetTabStops()
	{ VERIFY(::SendMessage(m_hWnd, LB_SETTABSTOPS, 0, 0)); }
inline BOOL CListBox::SetTabStops(int cxEachStop)
	{ return (BOOL)::SendMessage(m_hWnd, LB_SETTABSTOPS, 1, (LONG)(LPINT)&cxEachStop); }
#if (WINVER >= 0x030a)
inline int CListBox::SetItemHeight(int nIndex, UINT cyItemHeight)
	{ return (int)::SendMessage(m_hWnd, LB_SETITEMHEIGHT, nIndex, MAKELONG(cyItemHeight, 0)); }
inline int CListBox::GetItemHeight(int nIndex) const
	{ return (int)::SendMessage(m_hWnd, LB_GETITEMHEIGHT, nIndex, 0L); }
inline int CListBox::FindStringExact(int nIndexStart, LPCSTR lpszFind) const
	{ return (int)::SendMessage(m_hWnd, LB_FINDSTRINGEXACT, nIndexStart, (LONG)lpszFind); }
inline int CListBox::GetCaretIndex() const
	{ return (int)::SendMessage(m_hWnd, LB_GETCARETINDEX, 0, 0L); }
inline int CListBox::SetCaretIndex(int nIndex, BOOL bScroll /* = TRUE */)
	{ return (int)::SendMessage(m_hWnd, LB_SETCARETINDEX, nIndex, MAKELONG(bScroll, 0)); }
#endif  /* WINVER >= 0x030a */
inline int CListBox::AddString(LPCSTR lpItem)
	{ return (int)::SendMessage(m_hWnd, LB_ADDSTRING, 0, (LONG)lpItem); }
inline int CListBox::DeleteString(UINT nIndex)
	{ return (int)::SendMessage(m_hWnd, LB_DELETESTRING, nIndex, 0); }
inline int CListBox::InsertString(int nIndex, LPCSTR lpItem)
	{ return (int)::SendMessage(m_hWnd, LB_INSERTSTRING, nIndex, (LONG)lpItem); }
inline void CListBox::ResetContent()
	{ ::SendMessage(m_hWnd, LB_RESETCONTENT, 0, 0); }
inline int CListBox::Dir(UINT attr, LPCSTR lpWildCard)
	{ return (int)::SendMessage(m_hWnd, LB_DIR, attr, (LONG)lpWildCard); }
inline int CListBox::FindString(int nStartAfter, LPCSTR lpItem) const
	{ return (int)::SendMessage(m_hWnd, LB_FINDSTRING,
		nStartAfter, (LONG)lpItem); }
inline int CListBox::SelectString(int nStartAfter, LPCSTR lpItem)
	{ return (int)::SendMessage(m_hWnd, LB_SELECTSTRING,
		nStartAfter, (LONG)lpItem); }
inline int CListBox::SelItemRange(BOOL bSelect, int nFirstItem, int nLastItem)
	{ return (int)::SendMessage(m_hWnd, LB_SELITEMRANGE, bSelect,
		MAKELONG(nFirstItem, nLastItem)); }
inline CComboBox::CComboBox() 
	{ }
inline int CComboBox::GetCount() const
	{ return (int)::SendMessage(m_hWnd, CB_GETCOUNT, 0, 0); }
inline int CComboBox::GetCurSel() const
	{ return (int)::SendMessage(m_hWnd, CB_GETCURSEL, 0, 0); }
inline int CComboBox::SetCurSel(int nSelect)
	{ return (int)::SendMessage(m_hWnd, CB_SETCURSEL, nSelect, 0); }
inline DWORD CComboBox::GetEditSel() const
	{ return ::SendMessage(m_hWnd, CB_GETEDITSEL, 0, 0); }
inline BOOL CComboBox::LimitText(int nMaxChars)
	{ return (BOOL)::SendMessage(m_hWnd, CB_LIMITTEXT, nMaxChars, 0); }
inline BOOL CComboBox::SetEditSel(int nStartChar, int nEndChar)
	{ return (BOOL)::SendMessage(m_hWnd, CB_SETEDITSEL, 0, MAKELONG(nStartChar, nEndChar)); }
inline DWORD CComboBox::GetItemData(int nIndex) const
	{ return ::SendMessage(m_hWnd, CB_GETITEMDATA, nIndex, 0); }
inline int CComboBox::SetItemData(int nIndex, DWORD dwItemData)
	{ return (int)::SendMessage(m_hWnd, CB_SETITEMDATA, nIndex, (LONG)dwItemData); }
inline int CComboBox::GetLBText(int nIndex, LPSTR lpText) const
	{ return (int)::SendMessage(m_hWnd, CB_GETLBTEXT, nIndex, (LONG)lpText); }
inline int CComboBox::GetLBTextLen(int nIndex) const
	{ return (int)::SendMessage(m_hWnd, CB_GETLBTEXTLEN, nIndex, 0); }
inline void CComboBox::GetLBText(int nIndex, CString& rString) const
	{ GetLBText(nIndex, rString.GetBufferSetLength(GetLBTextLen(nIndex))); }
inline void CComboBox::ShowDropDown(BOOL bShowIt /* = TRUE */)
	{ ::SendMessage(m_hWnd, CB_SHOWDROPDOWN, bShowIt, 0); }
inline int CComboBox::AddString(LPCSTR lpString)
	{ return (int)::SendMessage(m_hWnd, CB_ADDSTRING, 0, (LONG)lpString); }
inline int CComboBox::DeleteString(UINT nIndex)
	{ return (int)::SendMessage(m_hWnd, CB_DELETESTRING, nIndex, 0);}
inline int CComboBox::InsertString(int nIndex, LPCSTR lpString)
	{ return (int)::SendMessage(m_hWnd, CB_INSERTSTRING, nIndex, (LONG)lpString); }
inline void CComboBox::ResetContent()
	{ ::SendMessage(m_hWnd, CB_RESETCONTENT, 0, 0); }
inline int CComboBox::Dir(UINT attr, LPCSTR lpWildCard)
	{ return (int)::SendMessage(m_hWnd, CB_DIR, attr, (LONG)lpWildCard); }
inline int CComboBox::FindString(int nStartAfter, LPCSTR lpString) const
	{ return (int)::SendMessage(m_hWnd, CB_FINDSTRING, nStartAfter,
		(LONG)lpString); }
inline int CComboBox::SelectString(int nStartAfter, LPCSTR lpString)
	{ return (int)::SendMessage(m_hWnd, CB_SELECTSTRING,
		nStartAfter, (LONG)lpString); }
inline void CComboBox::Clear()
	{ ::SendMessage(m_hWnd, WM_CLEAR, 0, 0); }
inline void CComboBox::Copy()
	{ ::SendMessage(m_hWnd, WM_COPY, 0, 0); }
inline void CComboBox::Cut()
	{ ::SendMessage(m_hWnd, WM_CUT, 0, 0); }
inline void CComboBox::Paste()
	{ ::SendMessage(m_hWnd, WM_PASTE, 0, 0); }
#if (WINVER >= 0x030a)
inline int CComboBox::SetItemHeight(int nIndex, UINT cyItemHeight)
	{ return (int)::SendMessage(m_hWnd, CB_SETITEMHEIGHT, nIndex, MAKELONG(cyItemHeight, 0)); }
inline int CComboBox::GetItemHeight(int nIndex) const
	{ return (int)::SendMessage(m_hWnd, CB_GETITEMHEIGHT, nIndex, 0L); }
inline int CComboBox::FindStringExact(int nIndexStart, LPCSTR lpszFind) const
	{ return (int)::SendMessage(m_hWnd, CB_FINDSTRINGEXACT, nIndexStart, (LONG)lpszFind); }
inline int CComboBox::SetExtendedUI(BOOL bExtended /* = TRUE */ )
	{ return (int)::SendMessage(m_hWnd, CB_SETEXTENDEDUI, bExtended, 0L); }
inline BOOL CComboBox::GetExtendedUI() const
	{ return (BOOL)::SendMessage(m_hWnd, CB_GETEXTENDEDUI, 0, 0L); }
inline void CComboBox::GetDroppedControlRect(LPRECT lprect) const
	{ ::SendMessage(m_hWnd, CB_GETDROPPEDCONTROLRECT, 0, (DWORD)lprect); }
inline BOOL CComboBox::GetDroppedState() const
	{ return (BOOL)::SendMessage(m_hWnd, CB_GETDROPPEDSTATE, 0, 0L); }
#endif  /* WINVER >= 0x030a */

inline CEdit::CEdit()
	{ }
inline BOOL CEdit::CanUndo() const
	{ return (BOOL)::SendMessage(m_hWnd, EM_CANUNDO, 0, 0); }
inline int CEdit::GetLineCount() const
	{ return (int)::SendMessage(m_hWnd, EM_GETLINECOUNT, 0, 0); }
inline BOOL CEdit::GetModify() const
	{ return (BOOL)::SendMessage(m_hWnd, EM_GETMODIFY, 0, 0); }
inline void CEdit::SetModify(BOOL bModified /* = TRUE */)
	{ ::SendMessage(m_hWnd, EM_SETMODIFY, bModified, 0); }
inline void CEdit::GetRect(LPRECT lpRect) const
	{ ::SendMessage(m_hWnd, EM_GETRECT, 0, (LONG)lpRect); }
inline DWORD CEdit::GetSel() const
	{ return ::SendMessage(m_hWnd, EM_GETSEL, 0, 0); }
inline void CEdit::GetSel(int& nStartChar, int& nEndChar) const
#ifdef _NTWIN
	{ ::SendMessage(m_hWnd, EM_GETSEL, (WPARAM)&nStartChar,(LPARAM)&nEndChar); }
#else
	{ 
		DWORD dwSel = (DWORD)::SendMessage(m_hWnd, EM_GETSEL, 0, 0); 
		nStartChar = (int)LOWORD(dwSel); 
		nEndChar = (int)HIWORD(dwSel); 
	}
#endif
inline HANDLE   CEdit::GetHandle() const
	{ return (HANDLE)::SendMessage(m_hWnd, EM_GETHANDLE, 0, 0); }
inline void CEdit::SetHandle(HANDLE hBuffer)
	{ ::SendMessage(m_hWnd, EM_SETHANDLE, (UINT)hBuffer, 0); }
inline int CEdit::GetLine(int nIndex, LPSTR lpBuffer) const
	{ return (int)::SendMessage(m_hWnd, EM_GETLINE, nIndex, (LONG)lpBuffer); }
inline int CEdit::GetLine(int nIndex, LPSTR lpBuffer, int nMaxLength) const
	{
		*(LPINT)lpBuffer = nMaxLength;
		return (int)::SendMessage(m_hWnd, EM_GETLINE, nIndex,
			(LONG)lpBuffer);
	}
inline void CEdit::EmptyUndoBuffer()
	{ ::SendMessage(m_hWnd, EM_EMPTYUNDOBUFFER, 0, 0); }
inline BOOL CEdit::FmtLines(BOOL bAddEOL)
	{ return (BOOL)::SendMessage(m_hWnd, EM_FMTLINES, bAddEOL, 0); }
inline void CEdit::LimitText(int nChars /* = 0 */)
	{ ::SendMessage(m_hWnd, EM_LIMITTEXT, nChars, 0); }
inline int CEdit::LineFromChar(int nIndex /* = -1 */) const
	{ return (int)::SendMessage(m_hWnd, EM_LINEFROMCHAR, nIndex, 0); }
inline int CEdit::LineIndex(int nLine /* = -1 */) const
	{ return (int)::SendMessage(m_hWnd, EM_LINEINDEX, nLine, 0); }
inline int CEdit::LineLength(int nLine /* = -1 */) const
	{ return (int)::SendMessage(m_hWnd, EM_LINELENGTH, nLine, 0); }
inline void CEdit::LineScroll(int nLines, int nChars /* = 0 */)
#ifdef _NTWIN
	{ ::SendMessage(m_hWnd, EM_LINESCROLL, nChars, nLines); }
#else
	{ ::SendMessage(m_hWnd, EM_LINESCROLL, 0, MAKELONG(nLines, nChars)); }
#endif
inline void CEdit::ReplaceSel(LPCSTR lpNewText)
	{ ::SendMessage(m_hWnd, EM_REPLACESEL, 0, (LONG)lpNewText); }
inline void CEdit::SetPasswordChar(char ch /* = '\0' */)
	{ ::SendMessage(m_hWnd, EM_SETPASSWORDCHAR, ch, 0); }
inline void CEdit::SetRect(LPRECT lpRect)
	{ ::SendMessage(m_hWnd, EM_SETRECT, 0, (LONG)lpRect); }
inline void CEdit::SetRectNP(LPRECT lpRect)
	{ ::SendMessage(m_hWnd, EM_SETRECTNP, 0, (LONG)lpRect); }
inline void CEdit::SetSel(DWORD dwSelection)
#ifdef _NTWIN
	{ ::SendMessage(m_hWnd, EM_SETSEL, LOWORD(dwSelection), HIWORD(dwSelection)); }
#else
	{ ::SendMessage(m_hWnd, EM_SETSEL, 0, (LONG)dwSelection); }
#endif
inline void CEdit::SetSel(int nStartChar, int nEndChar)
#ifdef _NTWIN
	{ ::SendMessage(m_hWnd, EM_SETSEL, nStartChar, nEndChar); }
#else
	{ ::SendMessage(m_hWnd, EM_SETSEL, 0, MAKELONG(nStartChar, nEndChar)); }
#endif
inline BOOL CEdit::SetTabStops(int nTabStops, LPINT rgTabStops)
	{ return (BOOL)::SendMessage(m_hWnd, EM_SETTABSTOPS, nTabStops,
		(LONG)rgTabStops); }
inline void CEdit::SetTabStops()
	{ VERIFY(::SendMessage(m_hWnd, EM_SETTABSTOPS, 0, 0)); }
inline BOOL CEdit::SetTabStops(int cxEachStop)
	{ return (BOOL)::SendMessage(m_hWnd, EM_SETTABSTOPS,
		1, (LONG)(LPINT)&cxEachStop); }
inline BOOL CEdit::Undo()
	{ return (BOOL)::SendMessage(m_hWnd, EM_UNDO, 0, 0); }
inline void CEdit::Clear()
	{ ::SendMessage(m_hWnd, WM_CLEAR, 0, 0); }
inline void CEdit::Copy()
	{ ::SendMessage(m_hWnd, WM_COPY, 0, 0); }
inline void CEdit::Cut()
	{ ::SendMessage(m_hWnd, WM_CUT, 0, 0); }
inline void CEdit::Paste()
	{ ::SendMessage(m_hWnd, WM_PASTE, 0, 0); }
#if (WINVER >= 0x030a)
inline BOOL CEdit::SetReadOnly(BOOL bReadOnly /* = TRUE */ )
	{ return (BOOL)::SendMessage(m_hWnd, EM_SETREADONLY, bReadOnly, 0L); }
inline int CEdit::GetFirstVisibleLine() const
	{ return (int)::SendMessage(m_hWnd, EM_GETFIRSTVISIBLELINE, 0, 0L); }
inline char CEdit::GetPasswordChar() const
	{ return (char)::SendMessage(m_hWnd, EM_GETPASSWORDCHAR, 0, 0L); }
#endif  /* WINVER >= 0x030a */

inline CScrollBar::CScrollBar() 
	{ }
inline int CScrollBar::GetScrollPos() const
	{ return ::GetScrollPos(m_hWnd, SB_CTL); }
inline int CScrollBar::SetScrollPos(int nPos, BOOL bRedraw /* = TRUE */)
	{ return ::SetScrollPos(m_hWnd, SB_CTL, nPos, bRedraw); }
inline void CScrollBar::GetScrollRange(LPINT lpMinPos, LPINT lpMaxPos) const
	{ ::GetScrollRange(m_hWnd, SB_CTL, lpMinPos, lpMaxPos); }
inline void CScrollBar::SetScrollRange(int nMinPos, int nMaxPos, BOOL bRedraw /* = TRUE */)
	{ ::SetScrollRange(m_hWnd, SB_CTL, nMinPos, nMaxPos, bRedraw); }
inline void CScrollBar::ShowScrollBar(BOOL bShow /* = TRUE */)
	{ ::ShowScrollBar(m_hWnd, SB_CTL, bShow); }
#if (WINVER >= 0x030a)
inline BOOL CScrollBar::EnableScrollBar(UINT nArrowFlags /* = ESB_ENABLE_BOTH */ )
	{ return ::EnableScrollBar(m_hWnd, SB_CTL, nArrowFlags); }
#endif  /* WINVER >= 0x030a */

inline CBitmapButton::CBitmapButton()
	{ }
inline CBitmapButton::CBitmapButton(LPCSTR lpBitmapResource,
	LPCSTR lpBitmapResourceSel /* = NULL */,
	LPCSTR lpBitmapResourceFocus /* = NULL */)
	{ VERIFY(LoadBitmaps(lpBitmapResource, lpBitmapResourceSel,
		lpBitmapResourceFocus)); }
	

// MDI inline functions
inline void CMDIFrameWnd::MDIActivate(CWnd* pWndActivate)
	{ ::SendMessage(m_hWndMDIClient, WM_MDIACTIVATE,
		(UINT)pWndActivate->m_hWnd, 0); }
inline CMDIChildWnd* CMDIFrameWnd::MDIGetActive(BOOL* pbMaximized /* = NULL */) const
#ifdef _NTWIN
	{ HWND hWnd = (HWND)::SendMessage(m_hWndMDIClient, WM_MDIGETACTIVE, 0, 0);
		if (pbMaximized != NULL)
			*pbMaximized = !!(::GetWindowLong(hWnd, GWL_STYLE) & WS_MAXIMIZE);
		return (CMDIChildWnd*)CWnd::FromHandle(hWnd); }
#else
	{ LONG l;
		l = ::SendMessage(m_hWndMDIClient, WM_MDIGETACTIVE, 0, 0);
		if (pbMaximized != NULL)
			*pbMaximized = HIWORD(l);
		return (CMDIChildWnd*)CWnd::FromHandle((HWND)LOWORD(l)); }
#endif
inline void CMDIFrameWnd::MDIIconArrange()
	{ ::SendMessage(m_hWndMDIClient, WM_MDIICONARRANGE, 0, 0); }
inline void CMDIFrameWnd::MDIMaximize(CWnd* pWnd)
	{ ::SendMessage(m_hWndMDIClient, WM_MDIMAXIMIZE, (UINT)pWnd->m_hWnd, 0); }
inline void CMDIFrameWnd::MDINext()
	{ ::SendMessage(m_hWndMDIClient, WM_MDINEXT, 0, 0); }
inline void CMDIFrameWnd::MDIRestore(CWnd* pWnd)
	{ ::SendMessage(m_hWndMDIClient, WM_MDIRESTORE, (UINT)pWnd->m_hWnd, 0); }
inline CMenu* CMDIFrameWnd::MDISetMenu(CMenu* pFrameMenu, CMenu* pWindowMenu)
#ifdef _NTWIN
	{ return CMenu::FromHandle((HMENU)::SendMessage(
		m_hWndMDIClient, WM_MDISETMENU,
		(WPARAM)pFrameMenu->GetSafeHmenu(),
		(LPARAM)pWindowMenu->GetSafeHmenu())); }
#else
	{ return CMenu::FromHandle((HMENU)::SendMessage(
		m_hWndMDIClient, WM_MDISETMENU, 0,
		MAKELONG(pFrameMenu->GetSafeHmenu(),
		pWindowMenu->GetSafeHmenu()))); }
#endif
inline void CMDIFrameWnd::MDITile()
	{ ::SendMessage(m_hWndMDIClient, WM_MDITILE, 0, 0); }
inline void CMDIFrameWnd::MDICascade()
	{ ::SendMessage(m_hWndMDIClient, WM_MDICASCADE, 0, 0); }

#if (WINVER >= 0x030a)
inline void CMDIFrameWnd::MDICascade(int nType)
	{ ::SendMessage(m_hWndMDIClient, WM_MDICASCADE, nType, 0); }
inline void CMDIFrameWnd::MDITile(int nType)
	{ ::SendMessage(m_hWndMDIClient, WM_MDITILE, nType, 0); }
#endif /* WINVER >= 0x030a */

inline CMDIChildWnd::CMDIChildWnd() 
	{ }
inline void CMDIChildWnd::MDIDestroy()
	{ GetParent()->SendMessage(WM_MDIDESTROY, (UINT)m_hWnd); }
inline void CMDIChildWnd::MDIActivate()
	{ GetParent()->SendMessage(WM_MDIACTIVATE, (UINT)m_hWnd); }
inline void CMDIChildWnd::MDIMaximize()
	{ GetParent()->SendMessage(WM_MDIMAXIMIZE, (UINT)m_hWnd); }
inline void CMDIChildWnd::MDIRestore()
	{ GetParent()->SendMessage(WM_MDIRESTORE, (UINT)m_hWnd); }

// CWinApp inline functions
inline HCURSOR CWinApp::LoadCursor(LPCSTR lpCursorName)
	{ return ::LoadCursor(AfxGetResourceHandle(), lpCursorName); }
inline HCURSOR CWinApp::LoadCursor(UINT nIDCursor)
	{ return ::LoadCursor(AfxGetResourceHandle(),
		MAKEINTRESOURCE(nIDCursor)); }
inline HCURSOR CWinApp::LoadStandardCursor(LPCSTR lpCursorName)
	{ return ::LoadCursor(NULL, lpCursorName); }
inline HCURSOR CWinApp::LoadOEMCursor(UINT nIDCursor)
	{ return ::LoadCursor(NULL, MAKEINTRESOURCE(nIDCursor)); }
inline HICON    CWinApp::LoadIcon(LPCSTR lpIconName)
	{ return ::LoadIcon(AfxGetResourceHandle(), lpIconName); }
inline HICON    CWinApp::LoadIcon(UINT nIDIcon)
	{ return ::LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(nIDIcon)); }
inline HICON    CWinApp::LoadStandardIcon(LPCSTR lpIconName)
	{ return ::LoadIcon(NULL, lpIconName); }
inline HICON    CWinApp::LoadOEMIcon(UINT nIDIcon)
	{ return ::LoadIcon(NULL, MAKEINTRESOURCE(nIDIcon)); }

#undef THIS_FILE
#define THIS_FILE __FILE__
#endif //__AFXWIN_INL__
