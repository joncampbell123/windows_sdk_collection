// Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation,
// All rights reserved.

// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __AFXWIN_H__
#define __AFXWIN_H__

/////////////////////////////////////////////////////////////////////////////
// Classes declared in this file

class CSize;
class CPoint;
class CRect;

//CObject
	// CException
		class CResourceException; // Win resource failure exception

	class CGdiObject;             // CDC drawing tool
		class CPen;               // a pen / HPEN wrapper
		class CBrush;             // a brush / HBRUSH wrapper
		class CFont;              // a font / HFONT wrapper
		class CBitmap;            // a bitmap / HBITMAP wrapper
		class CPalette;           // a palette / HPALLETE wrapper
		class CRgn;               // a region / HRGN wrapper

	class CDC;                    // a Display Context / HDC wrapper
		class CClientDC;          // CDC for client of window
		class CWindowDC;          // CDC for entire window
		class CPaintDC;           // embeddable BeginPaint struct helper

	class CMenu;                  // a menu / HMENU wrapper

	class CWnd;                   // a window / HWND wrapper
		class CDialog;            // a dialog

		// controls
		class CStatic;            // Static control
		class CButton;            // Button control
			class CBitmapButton;  // Bitmap button (self-draw)
		class CListBox;           // ListBox control
		class CComboBox;          // ComboBox control
		class CEdit;              // Edit control
		class CScrollBar;         // ScrollBar control

		// frame windows
		class CFrameWnd;          // standard SDI frame
			class CMDIFrameWnd;   // standard MDI frame
			class CMDIChildWnd;   // standard MDI child

	class CWinApp;                // simple application base class

/////////////////////////////////////////////////////////////////////////////
// Make sure 'afx.h' is included first

#ifdef STRICT
// The default for MFC is not STRICT, since C++ and MFC
// provide all of the same benefits (see TN012.TXT).  If
// you wish to use STRICT typechecking, then you must rebuild
// the library after removing the following #undef.
#undef STRICT
#endif

#define NO_STRICT 1

#ifndef __AFX_H__
#ifndef _WINDOWS
#define _WINDOWS
#endif
#include "afx.h"
#else
#ifndef _WINDOWS
	#error Please #define _WINDOWS before including afx.h
#endif
#endif

// we must include certain parts of Windows.h
#undef NOKERNEL
#undef NOGDI
#undef NOUSER
#undef NOSOUND
#undef NOCOMM
#undef NODRIVERS
#undef NOLOGERROR
#undef NOPROFILER
#undef NOMEMMGR
#undef NOLFILEIO
#undef NOOPENFILE
#undef NORESOURCE
#undef NOATOM
#undef NOLANGUAGE
#undef NOLSTRING
#undef NODBCS
#undef NOKEYBOARDINFO
#undef NOGDICAPMASKS
#undef NOCOLOR
#undef NOGDIOBJ
#undef NODRAWTEXT
#undef NOTEXTMETRIC
#undef NOSCALABLEFONT
#undef NOBITMAP
#undef NORASTEROPS
#undef NOMETAFILE
#undef NOSYSMETRICS
#undef NOSYSTEMPARAMSINFO
#undef NOMSG
#undef NOWINSTYLES
#undef NOWINOFFSETS
#undef NOSHOWWINDOW
#undef NODEFERWINDOWPOS
#undef NOVIRTUALKEYCODES
#undef NOKEYSTATES
#undef NOWH
#undef NOMENUS
#undef NOSCROLL
#undef NOCLIPBOARD
#undef NOICONS
#undef NOMB
#undef NOSYSCOMMANDS
#undef NOMDI
#undef NOCTLMGR
#undef NOWINMESSAGES

// The MFC library MUST be built with WINVER >= 0x030A (the default)
// even when Windows 3.0 is the target.  There are no compatability
// issues, rather this is done for source code maintainability.

// MFC applications may be built with WINVER == 0x300 (Win 3.0 only)
// or WINVER == 0x030A (Win 3.1/3.0)

#include "windows.h"

#ifndef WINVER
	#error Please include the correct WINDOWS.H (from \C700\INCLUDE)
#endif

#ifdef _NTWIN
// private header file for Windows NT 
#include "afxnt.h"
#endif //_NTWIN

#ifndef __AFXRES_H__
#include "afxres.h"     // standard resource IDs
#endif

#ifndef EXPORT
#define EXPORT __export
#endif
#ifdef _WINDLL
#ifndef _NTWIN
#define AFX_EXPORT	__loadds
#else
#define AFX_EXPORT APIENTRY
#endif
#else
#define AFX_EXPORT  EXPORT
#endif

// Type modifier for message handlers
#define afx_msg         /* intentional placeholder */

/////////////////////////////////////////////////////////////////////////////
// CSize - An extent, similar to Windows SIZE structure.

#ifndef _NTWIN
#if (WINVER < 0x030a)
typedef struct tagSIZE
{
	int cx;
	int cy;
} SIZE;
typedef SIZE*       PSIZE;
typedef SIZE NEAR* NPSIZE;
typedef SIZE FAR*  LPSIZE;
#endif	/* WINVER < 0x030a */
#endif

class CSize : public tagSIZE
{
public:

// Constructors
	CSize();
	CSize(int initCX, int initCY);
	CSize(SIZE initSize);
	CSize(POINT initPt);
	CSize(DWORD dwSize);

// Operations
	BOOL operator==(SIZE size) const;
	BOOL operator!=(SIZE size) const;
	void operator+=(SIZE size);
	void operator-=(SIZE size);

// Operators returning CSize values
	CSize operator+(SIZE size) const;
	CSize operator-(SIZE size) const;
};

/////////////////////////////////////////////////////////////////////////////
// CPoint - A 2-D point, similar to Windows POINT structure.

class CPoint : public tagPOINT
{
public:

// Constructors
	CPoint();
	CPoint(int initX, int initY);
	CPoint(POINT initPt);
	CPoint(SIZE initSize);
	CPoint(DWORD dwPoint);

// Operations
	void Offset(int xOffset, int yOffset);
	void Offset(POINT point);
	void Offset(SIZE size);
	BOOL operator==(POINT point) const;
	BOOL operator!=(POINT point) const;
	void operator+=(SIZE size);
	void operator-=(SIZE size);

// Operators returning CPoint values
	CPoint operator+(SIZE size) const;
	CPoint operator-(SIZE size) const;

// Operators returning CSize values
	CSize operator-(POINT point) const;
};

/////////////////////////////////////////////////////////////////////////////
// CRect - A 2-D rectangle, similar to Windows RECT structure.

class CRect : public tagRECT
{
public:

// Constructors
	CRect();
	CRect(int l, int t, int r, int b);
	CRect(const RECT& srcRect);
	CRect(LPRECT lpSrcRect);
	CRect(POINT point, SIZE size);

// Attributes (in addition to RECT members)
	int Width() const;
	int Height() const;
	CSize Size() const;
	CPoint& TopLeft();
	CPoint& BottomRight();

	// convert between CRect and LPRECT (no need for &)
	operator LPRECT();

	BOOL IsRectEmpty() const;
	BOOL IsRectNull() const;
	BOOL PtInRect(POINT point) const;

// Operations (from standard Windows)
	void SetRect(int x1, int y1, int x2, int y2);
	void SetRectEmpty();
	void CopyRect(LPRECT lpSrcRect);
	BOOL EqualRect(LPRECT lpRect) const;

	void InflateRect(int x, int y);
	void InflateRect(SIZE size);
	void OffsetRect(int x, int y);
	void OffsetRect(SIZE size);
	void OffsetRect(POINT point);

	// operations that fill '*this' with result
	int IntersectRect(LPRECT lpRect1, LPRECT lpRect2);
	int UnionRect(LPRECT lpRect1, LPRECT lpRect2);
#if (WINVER >= 0x030a)
	BOOL SubtractRect(LPRECT lpRectSrc1, LPRECT lpRectSrc2);
#endif /* WINVER >= 0x030a */

// Additional Operations
	void operator=(const RECT& srcRect);
	BOOL operator==(const RECT& rect) const;
	BOOL operator!=(const RECT& rect) const;
	void operator+=(POINT point);
	void operator-=(POINT point);
	void operator&=(const RECT& rect);
	void operator|=(const RECT& rect);

// Operators returning CRect values
	CRect operator+(POINT point) const;
	CRect operator-(POINT point) const;
	CRect operator&(const RECT& rect2) const;
	CRect operator|(const RECT& rect2) const;
};

#ifdef _DEBUG
// Diagnostic Output
CDumpContext& operator<<(CDumpContext& dc, SIZE size);
CDumpContext& operator<<(CDumpContext& dc, POINT point);
CDumpContext& operator<<(CDumpContext& dc, const RECT& rect);
#endif //_DEBUG

// Serialization
CArchive& operator<<(CArchive& ar, SIZE size);
CArchive& operator<<(CArchive& ar, POINT point);
CArchive& operator<<(CArchive& ar, const RECT& rect);
CArchive& operator>>(CArchive& ar, SIZE& size);
CArchive& operator>>(CArchive& ar, POINT& point);
CArchive& operator>>(CArchive& ar, RECT& rect);

/////////////////////////////////////////////////////////////////////////////
// Standard exception for resource failures

class CResourceException : public CException
{
	DECLARE_DYNAMIC(CResourceException)
public:
	CResourceException();
};

void AfxThrowResourceException();

/////////////////////////////////////////////////////////////////////////////
// CGdiObject abstract class for CDC SelectObject

class CGdiObject : public CObject
{
	DECLARE_DYNAMIC(CGdiObject)
public:

// Attributes
	HANDLE m_hObject;
	HANDLE GetSafeHandle() const;

	static CGdiObject* FromHandle(HANDLE hObject);
	static void DeleteTempMap();
	BOOL Attach(HANDLE hObject);
	HANDLE Detach();

// Constructors
	CGdiObject(); // must Create a derived class object
	virtual ~CGdiObject();
	BOOL DeleteObject();

// Operations
	int GetObject(int nCount, void FAR* lpObject) const;
	BOOL CreateStockObject(int nIndex);
	BOOL UnrealizeObject();

// Implementation
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif
};

/////////////////////////////////////////////////////////////////////////////
// CGdiObject subclasses (drawing tools)

class CPen : public CGdiObject
{
	DECLARE_DYNAMIC(CPen)

public:
	static CPen* FromHandle(HPEN hPen);

// Constructors
	CPen();
	CPen(int nPenStyle, int nWidth, DWORD crColor);
	BOOL CreatePen(int nPenStyle, int nWidth, DWORD crColor);
	BOOL CreatePenIndirect(LPLOGPEN lpLogPen);
};

class CBrush : public CGdiObject
{
	DECLARE_DYNAMIC(CBrush)

public:
	static CBrush* FromHandle(HBRUSH hBrush);

// Constructors
	CBrush();
	CBrush(DWORD crColor);             // CreateSolidBrush
	CBrush(int nIndex, DWORD crColor); // CreateHatchBrush
	CBrush(CBitmap* pBitmap);          // CreatePatternBrush

	BOOL CreateSolidBrush(DWORD crColor);
	BOOL CreateHatchBrush(int nIndex, DWORD crColor);
	BOOL CreateBrushIndirect(LPLOGBRUSH lpLogBrush);
	BOOL CreatePatternBrush(CBitmap* pBitmap);
	BOOL CreateDIBPatternBrush(GLOBALHANDLE hPackedDIB, UINT nUsage);
};

class CFont : public CGdiObject
{
	DECLARE_DYNAMIC(CFont)

public:
	static CFont* FromHandle(HFONT hFont);

// Constructors
	CFont();
	BOOL CreateFontIndirect(LPLOGFONT lpLogFont);
	BOOL CreateFont(int nHeight, int nWidth, int nEscapement,
			  int nOrientation, int nWeight, BYTE bItalic, BYTE bUnderline,
			  BYTE cStrikeOut, BYTE nCharSet, BYTE nOutPrecision,
			  BYTE nClipPrecision, BYTE nQuality, BYTE nPitchAndFamily,
			  LPCSTR lpFacename);
};


class CBitmap : public CGdiObject
{
	DECLARE_DYNAMIC(CBitmap)

public:
	static CBitmap* FromHandle(HBITMAP hBitmap);

// Constructors
	CBitmap();

	BOOL LoadBitmap(LPCSTR lpBitmapName);
	BOOL LoadBitmap(UINT nIDBitmap);
	BOOL LoadOEMBitmap(UINT nIDBitmap); // for OBM_/OCR_/OIC_
	BOOL CreateBitmap(int nWidth, int nHeight, BYTE nPlanes, BYTE nBitcount,
			const void FAR* lpBits);
	BOOL CreateBitmapIndirect(LPBITMAP lpBitmap);
	BOOL CreateCompatibleBitmap(CDC* pDC, int nWidth, int nHeight);
	BOOL CreateDiscardableBitmap(CDC* pDC, int nWidth, int nHeight);

// Operations
	DWORD SetBitmapBits(DWORD dwCount, const void FAR* lpBits);
	DWORD GetBitmapBits(DWORD dwCount, void FAR* lpBits) const;
	CSize SetBitmapDimension(int nWidth, int nHeight);
	CSize GetBitmapDimension() const;
};

class CPalette : public CGdiObject
{
	DECLARE_DYNAMIC(CPalette)

public:
	static CPalette* FromHandle(HPALETTE hPalette);

// Constructors
	CPalette();
	BOOL CreatePalette(LPLOGPALETTE lpLogPalette);

// Operations
	UINT GetPaletteEntries(UINT nStartIndex, UINT nNumEntries,
			  LPPALETTEENTRY lpPaletteColors) const;
	UINT SetPaletteEntries(UINT nStartIndex, UINT nNumEntries,
			  LPPALETTEENTRY lpPaletteColors);
	void AnimatePalette(UINT nStartIndex, UINT nNumEntries,
			  LPPALETTEENTRY lpPaletteColors);
	UINT GetNearestPaletteIndex(DWORD crColor) const;
	BOOL ResizePalette(UINT nNumEntries);
};

class CRgn : public CGdiObject
{
	DECLARE_DYNAMIC(CRgn)

public:
	static CRgn* FromHandle(HRGN hRgn);

// Constructors
	CRgn();
	BOOL CreateRectRgn(int x1, int y1, int x2, int y2);
	BOOL CreateRectRgnIndirect(LPRECT lpRect);
	BOOL CreateEllipticRgn(int x1, int y1, int x2, int y2);
	BOOL CreateEllipticRgnIndirect(LPRECT lpRect);
	BOOL CreatePolygonRgn(LPPOINT lpPoints, int nCount, int nMode);
	BOOL CreatePolyPolygonRgn(LPPOINT lpPoints, LPINT lpPolyCounts,
			int nCount, int nPolyFillMode);
	BOOL CreateRoundRectRgn(int x1, int y1, int x2, int y2,
			int x3, int y3);

// Operations
	void SetRectRgn(int x1, int y1, int x2, int y2);
	void SetRectRgn(LPRECT lpRect);
	int  CombineRgn(CRgn* pRgn1, CRgn* pRgn2, int nCombineMode);
	int  CopyRgn(CRgn* pRgnSrc);
	BOOL EqualRgn(CRgn* pRgn) const;
	int  OffsetRgn(int x, int y);
	int  OffsetRgn(POINT point);
	int  GetRgnBox(LPRECT lpRect) const;
	BOOL PtInRegion(int x, int y) const;
	BOOL PtInRegion(POINT point) const;
	BOOL RectInRegion(LPRECT lpRect) const;
};

/////////////////////////////////////////////////////////////////////////////
// The device context

class CDC : public CObject
{
	DECLARE_DYNAMIC(CDC)
public:

// Attributes
	HDC m_hDC;
	HDC GetSafeHdc() const;

	static CDC* FromHandle(HDC hDC);
	static void DeleteTempMap();
	BOOL Attach(HDC hDC);
	HDC  Detach();

// Constructors
	CDC();

	BOOL CreateDC(LPCSTR lpDriverName, LPCSTR lpDeviceName,
			LPCSTR lpOutput, const void FAR* lpInitData);
	BOOL CreateIC(LPCSTR lpDriverName, LPCSTR lpDeviceName,
			LPCSTR lpOutput, const void FAR* lpInitData);
	BOOL CreateCompatibleDC(CDC* pDC);

	BOOL DeleteDC();
	virtual ~CDC();

// Implementation support
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	static CGdiObject* SelectGdiObject(HDC hDC, HANDLE h);
public:

// Device-Context Functions
	CPoint GetDCOrg() const;
	int SaveDC() const;
	BOOL RestoreDC(int nSavedDC);
	int GetDeviceCaps(int nIndex) const;

// Drawing-Tool Functions
	CPoint GetBrushOrg() const;
	CPoint SetBrushOrg(int x, int y);
	CPoint SetBrushOrg(POINT point);
	int EnumObjects(int nObjectType,
					int (FAR PASCAL EXPORT* lpfn)(LPSTR, LPSTR),
					LPSTR lpData);

// type-safe selection helpers
	CGdiObject* SelectObject(CGdiObject* pObject);  // do not use for regions
	CGdiObject* SelectStockObject(int nIndex);
	CPen* SelectObject(CPen* pPen);
	CBrush* SelectObject(CBrush* pBrush);
	CFont* SelectObject(CFont* pFont);
	CBitmap* SelectObject(CBitmap* pBitmap);
	int SelectObject(CRgn* pRgn);       // special return for regions

// Color and Color Palette Functions
	DWORD GetNearestColor(DWORD crColor) const;
	CPalette* SelectPalette(CPalette* pPalette, BOOL bForceBackground);
	UINT RealizePalette();
	void UpdateColors();

// Drawing-Attribute Functions
	DWORD GetBkColor() const;
	DWORD SetBkColor(DWORD crColor);
	int GetBkMode() const;
	int SetBkMode(int nBkMode);
	int GetPolyFillMode() const;
	int SetPolyFillMode(int nPolyFillMode);
	int GetROP2() const;
	int SetROP2(int nDrawMode);
	int GetStretchBltMode() const;
	int SetStretchBltMode(int nStretchMode);
	DWORD GetTextColor() const;
	DWORD SetTextColor(DWORD crColor);

// Mapping Functions
	int GetMapMode() const;
	int SetMapMode(int nMapMode);
	// Viewport Origin
	CPoint GetViewportOrg() const;
	CPoint SetViewportOrg(int x, int y);
	CPoint SetViewportOrg(POINT point);
	CPoint OffsetViewportOrg(int nWidth, int nHeight);

	// Viewport Extent
	CSize GetViewportExt() const;
	CSize SetViewportExt(int x, int y);
	CSize SetViewportExt(SIZE size);
	CSize ScaleViewportExt(int xNum, int xDenom, int yNum, int yDenom);

	// Window Origin
	CPoint GetWindowOrg() const;
	CPoint SetWindowOrg(int x, int y);
	CPoint SetWindowOrg(POINT point);
	CPoint OffsetWindowOrg(int nWidth, int nHeight);

	// Window extent
	CSize GetWindowExt() const;
	CSize SetWindowExt(int x, int y);
	CSize SetWindowExt(SIZE size);
	CSize ScaleWindowExt(int xNum, int xDenom, int yNum, int yDenom);

// Coordinate Functions
	void DPtoLP(LPPOINT lpPoints, int nCount = 1) const;
	void DPtoLP(LPRECT lpRect) const;
	void LPtoDP(LPPOINT lpPoints, int nCount = 1) const;
	void LPtoDP(LPRECT lpRect) const;

// Region Functions
	BOOL FillRgn(CRgn* pRgn, CBrush* pBrush);
	BOOL FrameRgn(CRgn* pRgn, CBrush* pBrush, int nWidth, int nHeight);
	BOOL InvertRgn(CRgn* pRgn);
	BOOL PaintRgn(CRgn* pRgn);

// Clipping Functions
	int GetClipBox(LPRECT lpRect) const;
	int SelectClipRgn(CRgn* pRgn);
	int ExcludeClipRect(int x1, int y1, int x2, int y2);
	int ExcludeClipRect(LPRECT lpRect);
	int ExcludeUpdateRgn(CWnd* pWnd);
	int IntersectClipRect(int x1, int y1, int x2, int y2);
	int IntersectClipRect(LPRECT lpRect);
	int OffsetClipRgn(int x, int y);
	int OffsetClipRgn(SIZE size);
	BOOL PtVisible(int x, int y) const;
	BOOL PtVisible(POINT point) const;
	BOOL RectVisible(LPRECT lpRect) const;

// Line-Output Functions
	CPoint GetCurrentPosition() const;
	CPoint MoveTo(int x, int y);
	CPoint MoveTo(POINT point);
	BOOL LineTo(int x, int y);
	BOOL LineTo(POINT point);
	BOOL Arc(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
	BOOL Arc(LPRECT lpRect, POINT ptStart, POINT ptEnd);
	BOOL Polyline(LPPOINT lpPoints, int nCount);

// Simple Drawing Functions
	void FillRect(LPRECT lpRect, CBrush* pBrush);
	void FrameRect(LPRECT lpRect, CBrush* pBrush);
	void InvertRect(LPRECT lpRect);
	BOOL DrawIcon(int x, int y, HICON hIcon);
	BOOL DrawIcon(POINT point, HICON hIcon);

// Ellipse and Polygon Functions
	BOOL Chord(int x1, int y1, int x2, int y2, int x3, int y3,
			   int x4, int y4);
	BOOL Chord(LPRECT lpRect, POINT ptStart, POINT ptEnd);
	void DrawFocusRect(LPRECT lpRect);
	BOOL Ellipse(int x1, int y1, int x2, int y2);
	BOOL Ellipse(LPRECT lpRect);
	BOOL Pie(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
	BOOL Pie(LPRECT lpRect, POINT ptStart, POINT ptEnd);
	BOOL Polygon(LPPOINT lpPoints, int nCount);
	BOOL PolyPolygon(LPPOINT lpPoints, LPINT lpPolyCounts, int nCount);
	BOOL Rectangle(int x1, int y1, int x2, int y2);
	BOOL Rectangle(LPRECT lpRect);
	BOOL RoundRect(int x1, int y1, int x2, int y2, int x3, int y3);
	BOOL RoundRect(LPRECT lpRect, POINT point);

// Bitmap Function
	BOOL PatBlt(int x, int y, int nWidth, int nHeight, DWORD dwRop);
	BOOL BitBlt(int x, int y, int nWidth, int nHeight, CDC* pSrcDC,
				int xSrc, int ySrc, DWORD dwRop);
	BOOL StretchBlt(int x, int y, int nWidth, int nHeight, CDC* pSrcDC,
			int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, DWORD dwRop);
	DWORD GetPixel(int x, int y) const;
	DWORD GetPixel(POINT point) const;
	DWORD SetPixel(int x, int y, DWORD crColor);
	DWORD SetPixel(POINT point, DWORD crColor);
	BOOL FloodFill(int x, int y, DWORD crColor);
	BOOL ExtFloodFill(int x, int y, DWORD crColor, UINT nFillType);

// Text Functions
	BOOL TextOut(int x, int y, const CString& str);
	BOOL TextOut(int x, int y, LPCSTR lpString, int nCount);
	BOOL ExtTextOut(int x, int y, UINT nOptions, LPRECT lpRect,
			LPCSTR lpString, UINT nCount, LPINT lpDxWidths);
	CSize TabbedTextOut(int x, int y, LPCSTR lpString, int nCount,
			int nTabPositions, LPINT lpnTabStopPositions, int nTabOrigin);
	int DrawText(LPCSTR lpString, int nCount, LPRECT lpRect,
					UINT nFormat);
	CSize GetTextExtent(LPCSTR lpString, int nCount) const;
	CSize GetTabbedTextExtent(LPCSTR lpString, int nCount,
			int nTabPositions, LPINT lpnTabStopPositions) const;
	BOOL GrayString(CBrush* pBrush,
				BOOL (FAR PASCAL EXPORT* lpfnOutput)(HDC, DWORD, int),
					DWORD lpData, int nCount,
					int x, int y, int nWidth, int nHeight);
	UINT GetTextAlign() const;
	UINT SetTextAlign(UINT nFlags);
	int GetTextFace(int nCount, LPSTR lpFacename) const;
	BOOL GetTextMetrics(LPTEXTMETRIC lpMetrics) const;
	int SetTextJustification(int nBreakExtra, int nBreakCount);
	int GetTextCharacterExtra() const;
	int SetTextCharacterExtra(int nCharExtra);

// Font Functions
	BOOL GetCharWidth(UINT nFirstChar, UINT nLastChar, LPINT lpBuffer) const;
	DWORD SetMapperFlags(DWORD dwFlag);
	CSize GetAspectRatioFilter() const;

// Printer Escape Functions
	int Escape(int nEscape, int nCount, LPCSTR lpInData, void FAR* lpOutData);

	// Escape helpers
	int StartDoc(LPCSTR pDocName); 
	int StartPage();
	int EndPage();
	int SetAbortProc(BOOL (FAR PASCAL EXPORT* lpfn)(HDC, int));
	int AbortDoc();
	int EndDoc();

// Scrolling Functions
	BOOL ScrollDC(int dx, int dy, LPRECT lpRectScroll, LPRECT lpRectClip,
					CRgn* pRgnUpdate, LPRECT lpRectUpdate);

// MetaFile Functions
	BOOL PlayMetaFile(HANDLE hMF);

// Windows 3.1 Specific GDI functions
#if (WINVER >= 0x030a)
	BOOL QueryAbort() const;
	UINT SetBoundsRect(const RECT FAR* lpRectBounds, UINT flags);
	UINT GetBoundsRect(LPRECT lpRectBounds, UINT flags);
	int StartDoc(LPDOCINFO lpDocInfo);
	BOOL GetCharABCWidths(UINT nFirst, UINT nLast, LPABC lpabc) const;
	DWORD GetFontData(DWORD dwTable, DWORD dwOffset, LPVOID lpData, DWORD cbData) const;
	int GetKerningPairs(int nPairs, KERNINGPAIR FAR* lpkrnpair) const;
	UINT GetOutlineTextMetrics(UINT cbData, OUTLINETEXTMETRIC FAR* lpotm) const;
	DWORD GetGlyphOutline(UINT nChar, UINT nFormat, GLYPHMETRICS FAR* lpgm, 
		DWORD cbBuffer, void FAR* lpBuffer, const MAT2 FAR* lpmat2) const;
#endif

};

/////////////////////////////////////////////////////////////////////////////
// CDC Helpers

class CClientDC : public CDC
{
	DECLARE_DYNAMIC(CClientDC)

// Constructors
public:
	CClientDC(CWnd* pWnd);

// Attributes
protected:
	HWND m_hWnd;

// Implementation
public:
	virtual ~CClientDC();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

class CWindowDC : public CDC
{
	DECLARE_DYNAMIC(CWindowDC)

// Constructors
public:

	CWindowDC(CWnd* pWnd);

// Attributes
protected:
	HWND m_hWnd;

// Implementation
public:
	virtual ~CWindowDC();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

class CPaintDC : public CDC
{
	DECLARE_DYNAMIC(CPaintDC)

// Constructors
public:
	CPaintDC(CWnd* pWnd);   // BeginPaint

// Attributes
protected:
	HWND m_hWnd;
public:
	PAINTSTRUCT m_ps;       // actual paint struct !

// Implementation
	virtual ~CPaintDC();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

class CMetaFileDC : public CDC
{
	DECLARE_DYNAMIC(CMetaFileDC)

// Constructors
public:
	CMetaFileDC();
	BOOL Create(LPCSTR lpFilename = NULL);

	// Special close
	HANDLE  Close();

// Operations : selecting in a MetaFile DC returns a BOOL, not old object
	BOOL SelectObject(CGdiObject* pObject);
	BOOL SelectStockObject(int nIndex);
};


/////////////////////////////////////////////////////////////////////////////
// CMenu

class CMenu : public CObject
{
	DECLARE_DYNAMIC(CMenu)
public:

// Constructors
	CMenu();

	BOOL CreateMenu();
	BOOL CreatePopupMenu();
	BOOL LoadMenu(LPCSTR lpMenuName);
	BOOL LoadMenu(UINT nIDMenu);
	BOOL LoadMenuIndirect(const void FAR* lpMenuTemplate);
	BOOL DestroyMenu();

// Attributes
	HMENU m_hMenu;
	HMENU GetSafeHmenu() const;

	static CMenu* FromHandle(HMENU hMenu);
	static void DeleteTempMap();
	BOOL Attach(HMENU hMenu);
	HMENU Detach();

// CMenu Operations
	BOOL DeleteMenu(UINT nPosition, UINT nFlags);
	BOOL TrackPopupMenu(UINT nFlags, int x, int y,
						const CWnd* pWnd, const RECT FAR* lpRect = 0);

// CMenuItem Operations
	BOOL AppendMenu(UINT nFlags, UINT nIDNewItem = 0,
					LPCSTR lpNewItem = NULL);
	BOOL AppendMenu(UINT nFlags, UINT nIDNewItem, const CBitmap* pBmp);
	BOOL CheckMenuItem(UINT nIDCheckItem, UINT nCheck);
	BOOL EnableMenuItem(UINT nIDEnableItem, UINT nEnable);
	UINT GetMenuItemCount() const;
	UINT GetMenuItemID(int nPos) const;
	UINT GetMenuState(UINT nID, UINT nFlags) const;
	int GetMenuString(UINT nIDItem, LPSTR lpString, int nMaxCount,
					UINT nFlags) const;
	CMenu* GetSubMenu(int nPos) const;
	BOOL InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem = 0,
					LPCSTR lpNewItem = NULL);
	BOOL InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem,
					const CBitmap* pBmp);
	BOOL ModifyMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem = 0,
					LPCSTR lpNewItem = NULL);
	BOOL ModifyMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem,
					const CBitmap* pBmp);
	BOOL RemoveMenu(UINT nPosition, UINT nFlags);
	BOOL SetMenuItemBitmaps(UINT nPosition, UINT nFlags,
					const CBitmap* pBmpUnchecked, const CBitmap* pBmpChecked);

// Overridables (must override draw and measure for owner-draw menu items)
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	virtual ~CMenu();
};

/////////////////////////////////////////////////////////////////////////////
// Window wrapping message map

struct CMessageEntry;       // declared below after CWnd
struct NEAR CMessageMap
{
	CMessageMap*        pBaseMessageMap;
	CMessageEntry FAR*  lpEntries;
};

#define DECLARE_MESSAGE_MAP() \
private: \
	static CMessageEntry BASED_CODE _messageEntries[]; \
protected: \
	static CMessageMap messageMap; \
	virtual CMessageMap* GetMessageMap() const;

#define BEGIN_MESSAGE_MAP(theClass, baseClass) \
	CMessageMap* theClass::GetMessageMap() const \
		{ return &theClass::messageMap; } \
	CMessageMap theClass::messageMap = \
	{ &(baseClass::messageMap), \
		(CMessageEntry FAR*) &(theClass::_messageEntries) }; \
	CMessageEntry BASED_CODE theClass::_messageEntries[] = \
	{

#define END_MESSAGE_MAP() \
	{ 0, 0, AfxSig_end, (AFX_PMSG)0 } \
	};

// Message map signature values and macros in separate header
#include "afxmsg.h"

/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CWnd - a Microsoft Windows application window

class CWnd : public CObject
{
	DECLARE_DYNAMIC(CWnd)
protected:
	static const MSG* GetCurrentMessage();

// Attributes
public:
	HWND m_hWnd;

	HWND GetSafeHwnd() const;
	DWORD GetStyle() const;
	DWORD GetExStyle() const;

// Constructors and other creation
	CWnd();

	static CWnd* FromHandle(HWND hWnd);
	static CWnd* FromHandlePermanent(HWND hWnd);    // INTERNAL USE
	static void DeleteTempMap();
	BOOL Attach(HWND hWndNew);
	HWND Detach();
	BOOL SubclassWindow(HWND hWnd);
	BOOL SubclassDlgItem(UINT nID, CWnd* pParent);
			// for dynamic subclassing of windows control

protected: // This CreateEx() wraps CreateWindowEx
	BOOL CreateEx(DWORD dwExStyle, LPCSTR lpClassName,
		LPCSTR lpWindowName, DWORD dwStyle,
		int x, int y, int nWidth, int nHeight,
		HWND hwndParent, HMENU nIDorHMenu);

private:
	CWnd(HWND hWnd);    // just for special initialization

public:
	// for child windows...
	BOOL Create(LPCSTR lpClassName,
		LPCSTR lpWindowName, DWORD dwStyle,
		const RECT& rect,
		const CWnd* pParentWnd, UINT nID);

	virtual BOOL DestroyWindow();


// Message Functions
	LONG SendMessage(UINT message, UINT wParam = 0, LONG lParam = 0);
	BOOL PostMessage(UINT message, UINT wParam = 0, LONG lParam = 0);

// Window Text Functions
	void SetWindowText(LPCSTR lpString);
	int GetWindowText(LPSTR lpString, int nMaxCount) const;
	int GetWindowTextLength() const;
	void GetWindowText(CString& rString) const;
	void SetFont(CFont* pFont, BOOL bRedraw = TRUE);
	CFont* GetFont();

// CMenu Functions - non-Child windows only
	CMenu* GetMenu() const;
	BOOL SetMenu(CMenu* pMenu);
	void DrawMenuBar();
	CMenu* GetSystemMenu(BOOL bRevert) const;
	BOOL HiliteMenuItem(CMenu* pMenu, UINT nIDHiliteItem, UINT nHilite);

// Special attributes for Child windows only
	int GetDlgCtrlID() const;

// Window Size and Position Functions
	void CloseWindow();
	BOOL OpenIcon();
	BOOL IsIconic() const;
	BOOL IsZoomed() const;
	void MoveWindow(int x, int y, int nWidth, int nHeight,
				BOOL bRepaint = TRUE);
	void MoveWindow(LPRECT lpRect, BOOL bRepaint = TRUE);

	static const CWnd NEAR wndTop; // SetWindowPos's pWndInsertAfter
	static const CWnd NEAR wndBottom; // SetWindowPos's pWndInsertAfter
#if (WINVER >= 0x030a)
	static const CWnd NEAR wndTopMost; // SetWindowPos's pWndInsertAfter (3.1)
	static const CWnd NEAR wndNoTopMost; // SetWindowPos's pWndInsertAfter (3.1)
#endif /* WINVER >= 0x030a */ 

	BOOL SetWindowPos(const CWnd* pWndInsertAfter, int x, int y,
				int cx, int cy, UINT nFlags);
	UINT ArrangeIconicWindows();
	void BringWindowToTop();
	void GetWindowRect(LPRECT lpRect) const;
	void GetClientRect(LPRECT lpRect) const;

#if (WINVER >= 0x030a)
	BOOL GetWindowPlacement(WINDOWPLACEMENT FAR* lpwndpl) const;
	BOOL SetWindowPlacement(const WINDOWPLACEMENT FAR* lpwndpl);
#endif  /* WINVER >= 0x030a */

// Coordinate Mapping Functions
	void ClientToScreen(LPPOINT lpPoint) const;
	void ClientToScreen(LPRECT lpRect) const;
	void ScreenToClient(LPPOINT lpPoint) const;
	void ScreenToClient(LPRECT lpRect) const;
#if (WINVER >= 0x030a)
	void MapWindowPoints(CWnd* pwndTo, LPPOINT lpPoint, UINT nCount) const;
	void MapWindowPoints(CWnd* pwndTo, LPRECT lpRect) const;
#endif  /* WINVER >= 0x030a */

// Update/Painting Functions
	CDC* BeginPaint(LPPAINTSTRUCT lpPaint);
	void EndPaint(LPPAINTSTRUCT lpPaint);
	CDC* GetDC();
	CDC* GetWindowDC();
	int ReleaseDC(CDC* pDC);

	void UpdateWindow();
	void SetRedraw(BOOL bRedraw = TRUE);
	BOOL GetUpdateRect(LPRECT lpRect, BOOL bErase = FALSE);
	int GetUpdateRgn(CRgn* pRgn, BOOL bErase = FALSE);
	void Invalidate(BOOL bErase = TRUE);
	void InvalidateRect(LPRECT lpRect, BOOL bErase = TRUE);
	void InvalidateRgn(CRgn* pRgn, BOOL bErase = TRUE);
	void ValidateRect(LPRECT lpRect);
	void ValidateRgn(CRgn* pRgn);
	BOOL ShowWindow(int nCmdShow);
	BOOL IsWindowVisible() const;
	void ShowOwnedPopups(BOOL bShow = TRUE);

#if (WINVER >= 0x030a)
	CDC* GetDCEx(CRgn* prgnClip, DWORD flags);
	BOOL LockWindowUpdate();
	BOOL RedrawWindow(const RECT FAR* lpRectUpdate = NULL, 
		CRgn* prgnUpdate = NULL, 
		UINT flags = RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
	BOOL EnableScrollBar(int nSBFlags, UINT nArrowFlags = ESB_ENABLE_BOTH);
#endif  /* WINVER >= 0x030a */

// Timer Functions
	UINT SetTimer(int nIDEvent, UINT nElapse,
					UINT (FAR PASCAL EXPORT* lpfnTimer)(HWND, UINT, int, DWORD));
	BOOL KillTimer(int nIDEvent);

// Window State Functions
	BOOL IsWindowEnabled() const;
	BOOL EnableWindow(BOOL bEnable = TRUE);

	static CWnd* GetActiveWindow();
	CWnd* SetActiveWindow();

	static CWnd* GetCapture();
	CWnd* SetCapture();
	static CWnd* GetFocus();
	CWnd* SetFocus();

	CWnd* SetSysModalWindow();
	static CWnd* GetSysModalWindow();

	static CWnd* GetDesktopWindow();

// Dialog-Box Item Functions
// (NOTE: Dialog-Box Items are not necessarily in dialog boxes!)
	void CheckDlgButton(int nIDButton, UINT nCheck);
	void CheckRadioButton(int nIDFirstButton, int nIDLastButton,
					int nIDCheckButton);
	int GetCheckedRadioButton(int nIDFirstButton, int nIDLastButton);
	int DlgDirList(LPSTR lpPathSpec, int nIDListBox,
					int nIDStaticPath, UINT nFileType);
	int DlgDirListComboBox(LPSTR lpPathSpec, int nIDComboBox,
					int nIDStaticPath, UINT nFileType);
	BOOL DlgDirSelect(LPSTR lpString, int nIDListBox);
	BOOL DlgDirSelectComboBox(LPSTR lpString, int nIDComboBox);

	CWnd* GetDlgItem(int nID) const;
	UINT GetDlgItemInt(int nID, BOOL* lpTrans = NULL,
					BOOL bSigned = TRUE) const;
	int GetDlgItemText(int nID, LPSTR lpStr, int nMaxCount) const;

	CWnd* GetNextDlgGroupItem(CWnd* pWndCtl, BOOL bPrevious = FALSE) const;

	CWnd* GetNextDlgTabItem(CWnd* pWndCtl, BOOL bPrevious = FALSE) const;
	UINT IsDlgButtonChecked(int nIDButton) const;
	LONG SendDlgItemMessage(int nID, UINT message,
					UINT wParam = 0, LONG lParam = 0);
	void SetDlgItemInt(int nID, UINT nValue, BOOL bSigned = TRUE);
	void SetDlgItemText(int nID, LPCSTR lpString);

// Scrolling Functions
	int GetScrollPos(int nBar) const;
	void GetScrollRange(int nBar, LPINT lpMinPos, LPINT lpMaxPos) const;
	void ScrollWindow(int xAmount, int yAmount,
					const RECT FAR* lpRect = NULL,
					const RECT FAR* lpClipRect = NULL);
	int SetScrollPos(int nBar, int nPos, BOOL bRedraw = TRUE);
	void SetScrollRange(int nBar, int nMinPos, int nMaxPos,
			  BOOL bRedraw = TRUE);
	void ShowScrollBar(UINT nBar, BOOL bShow = TRUE);
#if (WINVER >= 0x030a)
	int ScrollWindowEx(int dx, int dy,
				const RECT FAR* lpRectScroll, const RECT FAR* lpRectClip,
				CRgn* prgnUpdate, LPRECT lpRectUpdate, UINT flags);
#endif  /* WINVER >= 0x030a */

// Window Access Functions
	CWnd* ChildWindowFromPoint(POINT point) const;
	static CWnd* FindWindow(LPCSTR lpClassName, LPCSTR lpWindowName);
	CWnd* GetNextWindow(UINT nFlag = GW_HWNDNEXT) const;
	CWnd* GetTopWindow() const;

	CWnd* GetWindow(UINT nCmd) const;
	CWnd* GetLastActivePopup() const;

	BOOL IsChild(CWnd* pWnd) const;
	CWnd* GetParent() const;
	CWnd* SetParent(CWnd* pWndNewParent);
	static CWnd* WindowFromPoint(POINT point);

// Alert Functions
	BOOL FlashWindow(BOOL bInvert);
	int MessageBox(LPCSTR lpText, LPCSTR lpCaption = NULL,
			  UINT nType = MB_OK);

// Clipboard Functions
	BOOL ChangeClipboardChain(HWND hWndNext);
	HWND SetClipboardViewer();
	BOOL OpenClipboard();
	static CWnd* GetClipboardOwner();
	static CWnd* GetClipboardViewer();
#if (WINVER >= 0x030a)
	static CWnd* GetOpenClipboardWindow();
#endif /* WINVER >= 0x030a */

// Caret Functions
	void CreateCaret(CBitmap* pBitmap);
	void CreateSolidCaret(int nWidth, int nHeight);
	void CreateGrayCaret(int nWidth, int nHeight);
	static CPoint GetCaretPos();
	static void SetCaretPos(POINT point);
	void HideCaret();
	void ShowCaret();

// Window-Management message handler member functions
protected:
	virtual BOOL OnCommand(UINT wParam, LONG lParam);

	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnActivateApp(BOOL bActive, HANDLE hTask);
	afx_msg void OnCancelMode();
	afx_msg void OnChildActivate();
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

#ifdef _NTWIN
	// special handler to fan in WM_CTLCOLOR - implementation
	afx_msg LRESULT OnNTCtlColor(WPARAM wParam, LPARAM lParam);
#endif
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnDestroy();
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnEndSession(BOOL bEnding);
	afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnGetMinMaxInfo(LPPOINT lpPoints);
	afx_msg void OnIconEraseBkgnd(CDC* pDC);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg LONG OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnPaint();
	afx_msg void OnParentNotify(UINT message, LONG lParam);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnQueryEndSession();
	afx_msg BOOL OnQueryNewPalette();
	afx_msg BOOL OnQueryOpen();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSize(UINT nType, int cx, int cy);
#if (WINVER >= 0x030a)
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
#endif /* WINVER >= 0x030a */

// Nonclient-Area message handler member functions
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcCalcSize(NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcDestroy();
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnNcMButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnNcMButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcMButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnNcPaint();
	afx_msg void OnNcRButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcRButtonUp(UINT nHitTest, CPoint point);

// System message handler member functions
#if (WINVER >= 0x030a)
	afx_msg void OnDropFiles(HANDLE hDropInfo);
	afx_msg void OnPaletteIsChanging(CWnd* pRealizeWnd);
#endif /* WINVER >= 0x030a */
	afx_msg void OnSysChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysCommand(UINT nID, LONG lParam);
	afx_msg void OnSysDeadChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnCompacting(UINT nCpuTime);
	afx_msg void OnDevModeChange(LPSTR lpDeviceName);
	afx_msg void OnFontChange();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg void OnSpoolerStatus(UINT nStatus, UINT nJobs);
	afx_msg void OnSysColorChange();
	afx_msg void OnTimeChange();
	afx_msg void OnWinIniChange(LPSTR lpSection);

// Input message handler member functions
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDeadChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnMouseActivate(CWnd* pFrameWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

// Initialization message handler member functions
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);

// Clipboard message handler member functions
	afx_msg void OnAskCbFormatName(UINT nMaxCount, LPSTR lpString);
	afx_msg void OnChangeCbChain(HWND hWndRemove, HWND hWndAfter);
	afx_msg void OnDestroyClipboard();
	afx_msg void OnDrawClipboard();
	afx_msg void OnHScrollClipboard(CWnd* pClipAppWnd, UINT nSBCode, UINT nPos);
	afx_msg void OnPaintClipboard(CWnd* pClipAppWnd, HANDLE hPaintStruct);
	afx_msg void OnRenderAllFormats();
	afx_msg void OnRenderFormat(UINT nFormat);
	afx_msg void OnSizeClipboard(CWnd* pClipAppWnd, HANDLE hRect);
	afx_msg void OnVScrollClipboard(CWnd* pClipAppWnd, UINT nSBCode, UINT nPos);

// Control message handler member functions
	afx_msg int OnCharToItem(UINT nChar, CListBox* pListBox, UINT nIndex);
	afx_msg int OnCompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	afx_msg void OnDeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
	afx_msg void OnDrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnMeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);

// MDI message handler member functions
	afx_msg void OnMDIActivate(BOOL bActivate, 
			CWnd* pActivateWnd, CWnd* pDeactivateWnd);

// Overridables and other helpers (for implementation of derived classes)
protected:
	// for deriving from a standard control
	virtual WNDPROC* GetSuperWndProcAddr();

	// for translating Windows messages in main message pump
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// for processing Windows messages
	virtual LONG WindowProc(UINT message, UINT wParam, LONG lParam);

	// for handling default processing
	LONG Default();
	virtual LONG DefWindowProc(UINT message, UINT wParam, LONG lParam);

	// for custom cleanup after WM_NCDESTROY
	virtual void PostNcDestroy();

// Implementation
public:
	virtual ~CWnd();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	friend void FAR PASCAL AFX_EXPORT _AfxSendMsgHook(int, UINT, LONG);
	friend LONG _AfxCallWndProc(CWnd*, HWND, UINT, UINT, LONG);

	friend class CWinApp;   // for PreTranslate access

	DECLARE_MESSAGE_MAP()
};

// helpers for registering your own WNDCLASSes
const char* AfxRegisterWndClass(UINT nClassStyle,
	HCURSOR hCursor = 0, HBRUSH hbrBackground = 0, HICON hIcon = 0);

LONG FAR PASCAL AFX_EXPORT AfxWndProc(HWND, UINT, UINT, LONG);

/////////////////////////////////////////////////////////////////////////////
// pointer to afx_msg member function

// #define AFX_MSG_CALL PASCAL

#ifdef _NTWIN
#define AFX_MSG_CALL /* assumes THISCALL */
#else
#define AFX_MSG_CALL PASCAL
#endif

typedef void (AFX_MSG_CALL CWnd::*AFX_PMSG)(void);
struct CMessageEntry
{
	UINT        nMessage;   // windows message or control notification code
	UINT        nID;        // control ID (or 0 for windows messages)
	UINT        nSig;       // signature type (action) or near pointer to extra
	AFX_PMSG    pfn;        // routine to call (or special value)
};

/////////////////////////////////////////////////////////////////////////////
// CDialog, CModalDialog


class CDialog : public CWnd
{
	DECLARE_DYNAMIC(CDialog)
public:

// Constructors (protected since you must subclass to implement a Dialog)
protected:
	// Modeless construct
	CDialog();
	~CDialog();

	BOOL Create(LPCSTR lpTemplateName, CWnd* pParentWnd = NULL);
	BOOL Create(UINT nIDTemplate, CWnd* pParentWnd = NULL);

	// Generic construct (for modal too)
	BOOL CreateIndirect(const void FAR* lpDialogTemplate,
		CWnd* pParentWnd = NULL);

// Attributes
public:
	void MapDialogRect(LPRECT lpRect) const;

protected:
	HBRUSH m_hBrushCtlBk;

// Operations
public:
	// message processing for modeless
	BOOL IsDialogMessage(LPMSG lpMsg);

	// support for passing on tab control - use 'PostMessage' if needed
	void NextDlgCtrl() const;
	void PrevDlgCtrl() const;
	void GotoDlgCtrl(CWnd* pWndCtrl);

	// default button access
	void SetDefID(UINT nID);
	DWORD GetDefID();

	// support for "new look" dialog boxes, set background color with this
	BOOL SetCtlBkColor(COLORREF clrCtlBk);

	// termination
	void EndDialog(int nResult);

// Overridables (special message map entries)
	virtual BOOL OnInitDialog();
	virtual void OnSetFont(CFont* pFont);

	// Default implementation sets colors to the "new look"
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
#endif

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual WNDPROC* GetSuperWndProcAddr();

	DECLARE_MESSAGE_MAP()
};

class CModalDialog : public CDialog
{
	DECLARE_DYNAMIC(CModalDialog)

// Constructors
public:
	CModalDialog(LPCSTR lpTemplateName, CWnd* pParentWnd = NULL);
	CModalDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL);

	// advanced usage - create indirect
	BOOL CreateIndirect(HANDLE hDialogTemplate);

// Operations
	virtual int DoModal();

// Overridables (automatic message map entries)
protected:
	virtual void OnOK();
	virtual void OnCancel();

// Implementation
#ifdef _DEBUG
public:
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// parameters for 'DoModal'
	LPCSTR m_lpDialogTemplate;      // name or MAKEINTRESOURCE
	HANDLE m_hDialogTemplate;       // Indirect if (lpDialogTemplate == NULL)
	CWnd* m_pParentWnd;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// Standard Windows controls

class CStatic : public CWnd
{
	DECLARE_DYNAMIC(CStatic)

// Constructors
public:
	CStatic();
	BOOL Create(LPCSTR lpText, DWORD dwStyle,
				const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff);

#if (WINVER >= 0x030a)
	HICON SetIcon(HICON hIcon);
	HICON GetIcon() const;
#endif /* WINVER >= 0x030a */
	

// Implementation
protected:
	virtual WNDPROC* GetSuperWndProcAddr();
};

class CButton : public CWnd
{
	DECLARE_DYNAMIC(CButton)

// Constructors
public:
	CButton();
	BOOL Create(LPCSTR lpCaption, DWORD dwStyle,
				const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes
	UINT GetState() const;
	void SetState(BOOL bHighlight);
	int GetCheck() const;
	void SetCheck(int nCheck);
	UINT GetButtonStyle() const;
	void SetButtonStyle(UINT nStyle, BOOL bRedraw = TRUE);

// Overridables (for owner draw only)
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

// Implementation
protected:
	virtual WNDPROC* GetSuperWndProcAddr();
};


class CListBox : public CWnd
{
	DECLARE_DYNAMIC(CListBox)

// Constructors
public:
	CListBox();
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes

	// for entire listbox
	int GetCount() const;
	int GetHorizontalExtent() const;
	void SetHorizontalExtent(int cxExtent);
	int GetTopIndex() const;
	int SetTopIndex(int nIndex);

	// for single-selection listboxes
	int GetCurSel() const;
	int SetCurSel(int nSelect);

	// for multiple-selection listboxes
	int GetSel(int nIndex) const;           // also works for single-selection
	int SetSel(int nIndex, BOOL bSelect = TRUE);
	int GetSelCount() const;
	int GetSelItems(int nMaxItems, LPINT rgIndex) const;

	// for listbox items
	DWORD GetItemData(int nIndex) const;
	int SetItemData(int nIndex, DWORD dwItemData);
	int GetItemRect(int nIndex, LPRECT lpRect) const;
	int GetText(int nIndex, LPSTR lpBuffer) const;
	int GetTextLen(int nIndex) const;
	void GetText(int nIndex, CString& rString) const;

	// Settable only attributes
	void SetColumnWidth(int cxWidth);
	BOOL SetTabStops(int nTabStops, LPINT rgTabStops);
	void SetTabStops();
	BOOL SetTabStops(int cxEachStop);

#if (WINVER >= 0x030a)
	int SetItemHeight(int nIndex, UINT cyItemHeight);
	int GetItemHeight(int nIndex) const;
	int FindStringExact(int nIndexStart, LPCSTR lpszFind) const;
	int GetCaretIndex() const;
	int SetCaretIndex(int nIndex, BOOL bScroll = TRUE);
	
#endif  /* WINVER >= 0x030a */

// Operations
	// manipulating listbox items
	int AddString(LPCSTR lpItem);
	int DeleteString(UINT nIndex);
	int InsertString(int nIndex, LPCSTR lpItem);
	void ResetContent();
	int Dir(UINT attr, LPCSTR lpWildCard);

	// selection helpers
	int FindString(int nStartAfter, LPCSTR lpItem) const;
	int SelectString(int nStartAfter, LPCSTR lpItem);
	int SelItemRange(BOOL bSelect, int nFirstItem, int nLastItem);

// Overridables (must override draw, measure and compare for owner draw)
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);

// Implementation
protected:
	virtual WNDPROC* GetSuperWndProcAddr();
};

class CComboBox : public CWnd
{
	DECLARE_DYNAMIC(CComboBox)

// Constructors
public:
	CComboBox();
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes
	// for entire combo box
	int GetCount() const;
	int GetCurSel() const;
	int SetCurSel(int nSelect);

	// for edit control
	DWORD GetEditSel() const;
	BOOL LimitText(int nMaxChars);
	BOOL SetEditSel(int nStartChar, int nEndChar);

	// for combobox item
	DWORD GetItemData(int nIndex) const;
	int SetItemData(int nIndex, DWORD dwItemData);
	int GetLBText(int nIndex, LPSTR lpText) const;
	int GetLBTextLen(int nIndex) const;
	void GetLBText(int nIndex, CString& rString) const;

#if (WINVER >= 0x030a)
	int SetItemHeight(int nIndex, UINT cyItemHeight);
	int GetItemHeight(int nIndex) const;
	int FindStringExact(int nIndexStart, LPCSTR lpszFind) const;
	int SetExtendedUI(BOOL bExtended = TRUE);
	BOOL GetExtendedUI() const;
	void GetDroppedControlRect(LPRECT lprect) const;
	BOOL GetDroppedState() const;
#endif  /* WINVER >= 0x030a */

// Operations
	// for drop-down combo boxes
	void ShowDropDown(BOOL bShowIt = TRUE);

	// manipulating listbox items
	int AddString(LPCSTR lpString);
	int DeleteString(UINT nIndex);
	int InsertString(int nIndex, LPCSTR lpString);
	void ResetContent();
	int Dir(UINT attr, LPCSTR lpWildCard);

	// selection helpers
	int FindString(int nStartAfter, LPCSTR lpString) const;
	int SelectString(int nStartAfter, LPCSTR lpString);

	// Clipboard operations
	void Clear();
	void Copy();
	void Cut();
	void Paste();

// Overridables (must override draw, measure and compare for owner draw)
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);

// Implementation
protected:
	virtual WNDPROC* GetSuperWndProcAddr();
};


class CEdit : public CWnd
{
	DECLARE_DYNAMIC(CEdit)

// Constructors
public:
	CEdit();
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes
	BOOL CanUndo() const;
	int GetLineCount() const;
	BOOL GetModify() const;
	void SetModify(BOOL bModified = TRUE);
	void GetRect(LPRECT lpRect) const;
	DWORD GetSel() const;
	void GetSel(int& nStartChar, int& nEndChar) const;
	HANDLE  GetHandle() const;
	void SetHandle(HANDLE hBuffer);

	// NOTE: first word in lpBuffer must contain the size of the buffer!
	int GetLine(int nIndex, LPSTR lpBuffer) const;
	int GetLine(int nIndex, LPSTR lpBuffer, int nMaxLength) const;

// Operations
	void EmptyUndoBuffer();
	BOOL FmtLines(BOOL bAddEOL);

	void LimitText(int nChars = 0);
	int LineFromChar(int nIndex = -1) const;
	int LineIndex(int nLine = -1) const;
	int LineLength(int nLine = -1) const;
	void LineScroll(int nLines, int nChars = 0);
	void ReplaceSel(LPCSTR lpNewText);
	void SetPasswordChar(char ch);
	void SetRect(LPRECT lpRect);
	void SetRectNP(LPRECT lpRect);
	void SetSel(DWORD dwSelection);
	void SetSel(int nStartChar, int nEndChar);
	BOOL SetTabStops(int nTabStops, LPINT rgTabStops);
	void SetTabStops();
	BOOL SetTabStops(int cxEachStop);

	// Clipboard operations
	BOOL Undo();
	void Clear();
	void Copy();
	void Cut();
	void Paste();

#if (WINVER >= 0x030a)
	BOOL SetReadOnly(BOOL bReadOnly = TRUE);
	int GetFirstVisibleLine() const;
	char GetPasswordChar() const;
#endif  /* WINVER >= 0x030a */

// Implementation
protected:
	virtual WNDPROC* GetSuperWndProcAddr();
};


class CScrollBar : public CWnd
{
	DECLARE_DYNAMIC(CScrollBar)

// Constructors
public:
	CScrollBar();
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes
	int GetScrollPos() const;
	int SetScrollPos(int nPos, BOOL bRedraw = TRUE);
	void GetScrollRange(LPINT lpMinPos, LPINT lpMaxPos) const;
	void SetScrollRange(int nMinPos, int nMaxPos, BOOL bRedraw = TRUE);
	void ShowScrollBar(BOOL bShow = TRUE);

#if (WINVER >= 0x030a)
	BOOL EnableScrollBar(UINT nArrowFlags = ESB_ENABLE_BOTH);
#endif  /* WINVER >= 0x030a */

// Implementation
protected:
	virtual WNDPROC* GetSuperWndProcAddr();
};

/////////////////////////////////////////////////////////////////////////////
// Extra Custom Controls

// CBitmapButton - push-button with 2 or 3 bitmap images
class CBitmapButton : public CButton
{
	DECLARE_DYNAMIC(CBitmapButton)

protected:
	// all bitmaps must be the same size
	CBitmap m_bitmap;           // normal image (REQUIRED)
	CBitmap m_bitmapSel;        // selected image (OPTIONAL)
	CBitmap m_bitmapFocus;      // focused but not selected (OPTIONAL)

public:
// Construction
	CBitmapButton();
	CBitmapButton(LPCSTR lpBitmapResource, LPCSTR lpBitmapResourceSel = NULL,
			LPCSTR lpBitmapResourceFocus = NULL);

	BOOL LoadBitmaps(LPCSTR lpBitmapResource, LPCSTR lpBitmapResourceSel = NULL,
			LPCSTR lpBitmapResourceFocus = NULL);
	BOOL AutoLoad(UINT nID, CWnd* pParent);

// Operations
	void SizeToContent();

// Implementation:
protected:
	virtual void    DrawItem(LPDRAWITEMSTRUCT lpDIS);
};

/////////////////////////////////////////////////////////////////////////////
// CFrameWnd

class CFrameWnd : public CWnd
{
	DECLARE_DYNAMIC(CFrameWnd)

protected:
// Protected attributes
	HACCEL m_hAccelTable;

public:
	static const CRect NEAR rectDefault;

// Constructors
	CFrameWnd();

	BOOL LoadAccelTable(LPCSTR lpAccelTableName);
	BOOL Create(LPCSTR lpClassName,
				LPCSTR lpWindowName,
				DWORD dwStyle = WS_OVERLAPPEDWINDOW,
				const RECT& rect = rectDefault,
				const CWnd* pParentWnd = NULL,      // != NULL for popups
				LPCSTR lpMenuName = NULL);

// Implementation
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	virtual ~CFrameWnd();

	virtual CFrameWnd* GetParentFrame();
	virtual CFrameWnd* GetChildFrame();

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();   // default to delete this.
};

/////////////////////////////////////////////////////////////////////////////
// MDI Support

class CMDIFrameWnd : public CFrameWnd
{
	DECLARE_DYNAMIC(CMDIFrameWnd)
public:

// Constructors
	CMDIFrameWnd();

	BOOL Create(LPCSTR lpClassName,
				LPCSTR lpWindowName,
				DWORD dwStyle,
				const RECT& rect,
				const CWnd* pParentWnd,
				LPCSTR lpMenuName);

// Overridables (automatic message map entries)
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

// Attributes
public:
	HWND m_hWndMDIClient;

	virtual BOOL CreateClient(LPCREATESTRUCT lpCreateStruct,
							  CMenu* pWindowMenu);

	void MDIActivate(CWnd* pWndActivate);
	CMDIChildWnd* MDIGetActive(BOOL* pbMaximized = NULL) const;
	void MDIIconArrange();
	void MDIMaximize(CWnd* pWnd);
	void MDINext();
	void MDIRestore(CWnd* pWnd);
	CMenu* MDISetMenu(CMenu* pFrameMenu, CMenu* pWindowMenu);

	void MDICascade();
	void MDITile();

#if (WINVER >= 0x030a)
	void MDITile(int nType);
	void MDICascade(int nType);
#endif /* WINVER >= 0x030a */

// Implementation
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#endif
	virtual CFrameWnd* GetChildFrame();
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LONG DefWindowProc(UINT nMsg, UINT wParam, LONG lParam);
	virtual BOOL OnCommand(UINT wParam, LONG lParam);
protected:
	DECLARE_MESSAGE_MAP()

	friend class CMDIChildWnd;
};


class CMDIChildWnd : public CFrameWnd
{
	DECLARE_DYNAMIC(CMDIChildWnd)
protected:
	CMDIFrameWnd* m_pMDIFrameWnd;           // our MDIFrame parent

// Constructors
public:
	CMDIChildWnd();

	BOOL Create(LPCSTR lpClassName,
				LPCSTR lpWindowName,
				DWORD dwStyle = 0,
				const RECT& rect = rectDefault,
				CMDIFrameWnd* pParentWnd = NULL);

// Operations
	void MDIDestroy();
	void MDIActivate();
	void MDIMaximize();
	void MDIRestore();

// Implementation
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	virtual BOOL DestroyWindow();
	virtual CFrameWnd* GetParentFrame();
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LONG DefWindowProc(UINT nMsg, UINT wParam, LONG lParam);

	friend class CMDIFrameWnd;
};

/////////////////////////////////////////////////////////////////////////////
// Global functions for access to the one and only CWinApp

extern "C"
{
// standard C variables if you wish to access them from C programs,
// use inline functions for C++ programs
extern CWinApp* afxCurrentWinApp;
extern HANDLE afxCurrentInstanceHandle;
extern HANDLE afxCurrentResourceHandle;
extern const char* afxCurrentAppName;
extern BOOL AfxWinInit(HINSTANCE, HINSTANCE, LPSTR, int);
extern void AfxWinTerm();
}

// Global Windows state data helper functions (inlines)
CWinApp* AfxGetApp();
HINSTANCE AfxGetInstanceHandle();
HINSTANCE AfxGetResourceHandle();
const char* AfxGetAppName();

/////////////////////////////////////////////////////////////////////////////
// CWinApp - the world's simplest Windows application

class CWinApp : public CObject
{
	DECLARE_DYNAMIC(CWinApp)
public:

// Constructor
	CWinApp(const char* pszAppName = NULL);
	void SetCurrentHandles();

// Attributes
	// Startup args (do not change)
	const char* m_pszAppName;       // from constructor
	HINSTANCE m_hInstance;
	HINSTANCE m_hPrevInstance;
	LPSTR m_lpCmdLine;
	int m_nCmdShow;

	// Running args
	CWnd* m_pMainWnd;       // main window (optional)

// Operations
	// Cursors
	HCURSOR LoadCursor(LPCSTR lpCursorName);
	HCURSOR LoadCursor(UINT nIDCursor);
	HCURSOR LoadStandardCursor(LPCSTR lpCursorName); // for IDC_ values
	HCURSOR LoadOEMCursor(UINT nIDCursor);          // for OCR_ values

	// Icons
	HICON LoadIcon(LPCSTR lpIconName);
	HICON LoadIcon(UINT nIDIcon);
	HICON LoadStandardIcon(LPCSTR lpIconName);    // for IDI_ values
	HICON LoadOEMIcon(UINT nIDIcon);             // for OIC_ values

	BOOL PumpMessage();

// Overridables
	// hooks for your initialization code
	virtual BOOL InitApplication();
	virtual BOOL InitInstance();

	virtual int Run();

	// called by standard 'Run' implementation
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle(LONG lCount); // return TRUE if more idle processing
	virtual int ExitInstance(); // return app exit code


// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	MSG  m_msgCur;

#ifdef _DEBUG
// Diagnostic trap for when going back to message pump is not permitted.
protected:
	int m_nDisablePumpCount;
public:
	void EnablePump(BOOL bEnable);
#endif

};

/////////////////////////////////////////////////////////////////////////////
// Extra diagnostic tracing options

#ifdef _WINDOWS
extern "C" { extern int afxTraceFlags; }
	// 1 => multi-app debugging
	// 2 => main message pump trace (includes DDE)
	// 4 => Windows message tracing
	// 8 => Windows command routing trace (set 4+8 for control notifications)
	// 16 (0x10) => special OLE callback trace
#endif // _WINDOWS

/////////////////////////////////////////////////////////////////////////////
// Inline function declarations

#include "afxwin.inl"


#endif //__AFXWIN_H__
