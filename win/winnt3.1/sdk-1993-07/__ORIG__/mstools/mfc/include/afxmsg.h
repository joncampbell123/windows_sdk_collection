// Microsoft Foundation Classes C++ library. 
// Copyright (C) 1992 Microsoft Corporation, 
// All rights reserved. 

// This source code is only intended as a supplement to the 
// Microsoft Foundation Classes Reference and Microsoft 
// QuickHelp documentation provided with the library. 
// See these sources for detailed information regarding the 
// Microsoft Foundation Classes product. 

// Do not include this file directly (included by AFXWIN.H)

/////////////////////////////////////////////////////////////////////////////

// Entries in a message map (a 'CMessageEntry') table can be of four formats
// 1) control notification message (i.e. in response to WM_COMMAND)
//      nNotifyCode, nControlID, signature type, parameterless member function
//      (eg: LBN_SELCHANGE, IDC_LISTBOX, AfxSig_vv, ... )
// 2) menu/accelerator notification message (i.e. special case of first format)
//      0, nID, signature type, parameterless member function
//      (eg: 0, IDM_FILESAVE, AfxSig_vv, ... )
// 3) constant windows message
//      nMessage, 0, signature type, member function
//      (eg: WM_PAINT, 0, ...)
// 4) variable windows message (using RegisterWindowMessage)
//      0xC000, 0, &nMessage, special member function

// The end of the message map is marked with a special value
//      0, 0, AfxSig_end, 0

/////////////////////////////////////////////////////////////////////////////

enum AfxSig
{
	AfxSig_end = 0,     // [marks end of message map]

	AfxSig_bD,      // BOOL (CDC *)
	AfxSig_bb,      // BOOL (BOOL)
	AfxSig_bWww,    // BOOL (CWnd*, UINT, UINT)
	AfxSig_hDWw,    // HBRUSH (CDC *, CWnd*, UINT)
	AfxSig_iwWw,    // int (UINT, CWnd*, UINT)
	AfxSig_iWww,    // int (CWnd*, UINT, UINT)
	AfxSig_is,      // int (LPSTR)
	AfxSig_lwl,     // LONG (UINT, LONG)
	AfxSig_lwwM,    // LONG (UINT, UINT, CMenu *)
	AfxSig_vv,      // void (void)
	AfxSig_vw,      // void (UINT)
	AfxSig_vww,     // void (UINT, UINT)
	AfxSig_vvii,    // void (int, int) // wParam is ignored
	AfxSig_vwww,    // void (UINT, UINT, UINT)
	AfxSig_vwii,    // void (UINT, int, int)
	AfxSig_vwl,     // void (UINT, LONG)
	AfxSig_vbWW,    // void (BOOL, CWnd*, CWnd*)
	AfxSig_vD,      // void (CDC *)
	AfxSig_vM,      // void (CMenu *)
	AfxSig_vMwb,    // void (CMenu *, UINT, BOOL)
	AfxSig_vW,      // void (CWnd*)
	AfxSig_vWww,    // void (CWnd*, UINT, UINT)
	AfxSig_vWh,     // void (CWnd*, HANDLE)
	AfxSig_vwW,     // void (UINT, CWnd*)
	AfxSig_vwWb,    // void (UINT, CWnd*, BOOL)
	AfxSig_vwwW,    // void (UINT, UINT, CWnd*)
	AfxSig_vs,      // void (LPSTR)
	AfxSig_wp,      // UINT (CPoint)
	AfxSig_wv,      // UINT (void)
	AfxSig_vPOS,    // void (WINDOWPOS FAR*)
	AfxSig_vCALC,   // void (NCCALCSIZE_PARAMS FAR*)
	AfxSig_bh,      // BOOL (HANDLE)

	// Other aliases (based on implementation)
#ifdef _NTWIN
	AfxSig_vwwh,				// void (UINT, UINT, HANDLE)
	AfxSig_vwp,					// void (UINT, CPoint)
#else
	AfxSig_vwwh = AfxSig_vwww,	// void (UINT, UINT, HANDLE)
	AfxSig_vwp = AfxSig_vwl,	// void (UINT, CPoint)
#endif
	AfxSig_bv = AfxSig_wv,      // BOOL (void)
	AfxSig_hv = AfxSig_wv,      // HANDLE (void)
	AfxSig_vb = AfxSig_vw,      // void (BOOL)
	AfxSig_vbh = AfxSig_vww,    // void (BOOL, HANDLE)
	AfxSig_vbw = AfxSig_vww,    // void (BOOL, UINT)
	AfxSig_vhh = AfxSig_vww, 	// void (HANDLE, HANDLE)
   AfxSig_vh  = AfxSig_vw		// void (HANDLE)
};

/////////////////////////////////////////////////////////////////////////////
// Message map tables for Windows messages

#define ON_WM_CREATE() \
	{ WM_CREATE, 0, AfxSig_is, \
		(AFX_PMSG)(int (AFX_MSG_CALL CWnd::*)(LPCREATESTRUCT))OnCreate },
#define ON_WM_DESTROY() \
	{ WM_DESTROY, 0, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))OnDestroy },
#define ON_WM_MOVE() \
	{ WM_MOVE, 0, AfxSig_vvii, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(int, int))OnMove },
#define ON_WM_SIZE() \
	{ WM_SIZE, 0, AfxSig_vwii, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, int, int))OnSize },
#define ON_WM_ACTIVATE() \
	{ WM_ACTIVATE, 0, AfxSig_vwWb, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, CWnd*, BOOL))OnActivate },
#define ON_WM_SETFOCUS() \
	{ WM_SETFOCUS, 0, AfxSig_vW, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(CWnd*))OnSetFocus },
#define ON_WM_KILLFOCUS() \
	{ WM_KILLFOCUS, 0, AfxSig_vW, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(CWnd*))OnKillFocus },
#define ON_WM_ENABLE() \
	{ WM_ENABLE, 0, AfxSig_vb, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(BOOL))OnEnable },
#define ON_WM_PAINT() \
	{ WM_PAINT, 0, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))OnPaint },
#define ON_WM_CLOSE() \
	{ WM_CLOSE, 0, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))OnClose },
#define ON_WM_QUERYENDSESSION() \
	{ WM_QUERYENDSESSION, 0, AfxSig_bv, \
		(AFX_PMSG)(BOOL (AFX_MSG_CALL CWnd::*)(void))OnQueryEndSession },
#define ON_WM_QUERYOPEN() \
	{ WM_QUERYOPEN, 0, AfxSig_bv, \
		(AFX_PMSG)(BOOL (AFX_MSG_CALL CWnd::*)(void))OnQueryOpen },
#define ON_WM_ERASEBKGND() \
	{ WM_ERASEBKGND, 0, AfxSig_bD, \
		(AFX_PMSG)(BOOL (AFX_MSG_CALL CWnd::*)(CDC *))OnEraseBkgnd },
#define ON_WM_SYSCOLORCHANGE() \
	{ WM_SYSCOLORCHANGE, 0, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))OnSysColorChange },
#define ON_WM_ENDSESSION() \
	{ WM_ENDSESSION, 0, AfxSig_vb, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(BOOL))OnEndSession },
#define ON_WM_SHOWWINDOW() \
	{ WM_SHOWWINDOW, 0, AfxSig_vbw, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(BOOL, UINT))OnShowWindow },
#define ON_WM_CTLCOLOR() \
	{ WM_CTLCOLOR, 0, AfxSig_hDWw, \
		(AFX_PMSG)(HBRUSH (AFX_MSG_CALL CWnd::*)(CDC *, CWnd*, UINT))OnCtlColor },
#define ON_WM_WININICHANGE() \
	{ WM_WININICHANGE, 0, AfxSig_vs, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(LPSTR))OnWinIniChange },
#define ON_WM_DEVMODECHANGE() \
	{ WM_DEVMODECHANGE, 0, AfxSig_vs, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(LPSTR))OnDevModeChange },
#define ON_WM_ACTIVATEAPP() \
	{ WM_ACTIVATEAPP, 0, AfxSig_vbh, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(BOOL, HANDLE))OnActivateApp },
#define ON_WM_FONTCHANGE() \
	{ WM_FONTCHANGE, 0, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))OnFontChange },
#define ON_WM_TIMECHANGE() \
	{ WM_TIMECHANGE, 0, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))OnTimeChange },
#define ON_WM_CANCELMODE() \
	{ WM_CANCELMODE, 0, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))OnCancelMode },
#define ON_WM_SETCURSOR() \
	{ WM_SETCURSOR, 0, AfxSig_bWww, \
		(AFX_PMSG)(BOOL (AFX_MSG_CALL CWnd::*)(CWnd*, UINT, UINT))OnSetCursor },
#define ON_WM_MOUSEACTIVATE() \
	{ WM_MOUSEACTIVATE, 0, AfxSig_iWww, \
		(AFX_PMSG)(int (AFX_MSG_CALL CWnd::*)(CWnd*, UINT, UINT))OnMouseActivate },
#define ON_WM_CHILDACTIVATE() \
	{ WM_CHILDACTIVATE, 0, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))OnChildActivate },
#define ON_WM_GETMINMAXINFO() \
	{ WM_GETMINMAXINFO, 0, AfxSig_vs, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(LPPOINT))OnGetMinMaxInfo },
#define ON_WM_ICONERASEBKGND() \
	{ WM_ICONERASEBKGND, 0, AfxSig_vD, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(CDC *))OnIconEraseBkgnd },
#define ON_WM_SPOOLERSTATUS() \
	{ WM_SPOOLERSTATUS, 0, AfxSig_vww, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, UINT))OnSpoolerStatus },
#define ON_WM_DRAWITEM() \
	{ WM_DRAWITEM, 0, AfxSig_vs, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(LPDRAWITEMSTRUCT))OnDrawItem },
#define ON_WM_MEASUREITEM() \
	{ WM_MEASUREITEM, 0, AfxSig_vs, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(LPMEASUREITEMSTRUCT))OnMeasureItem },
#define ON_WM_DELETEITEM() \
	{ WM_DELETEITEM, 0, AfxSig_vs, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(LPDELETEITEMSTRUCT))OnDeleteItem },
#define ON_WM_VKEYTOITEM() \
	{ WM_VKEYTOITEM, 0, AfxSig_iwWw, \
		(AFX_PMSG)(int (AFX_MSG_CALL CWnd::*)(UINT, CListBox*, UINT))OnVKeyToItem },
#define ON_WM_CHARTOITEM() \
	{ WM_CHARTOITEM, 0, AfxSig_iwWw, \
		(AFX_PMSG)(int (AFX_MSG_CALL CWnd::*)(UINT, CListBox*, UINT))OnCharToItem },
#define ON_WM_QUERYDRAGICON() \
	{ WM_QUERYDRAGICON, 0, AfxSig_hv, \
		(AFX_PMSG)(HANDLE (AFX_MSG_CALL CWnd::*)())OnQueryDragIcon },
#define ON_WM_COMPAREITEM() \
	{ WM_COMPAREITEM, 0, AfxSig_is, \
		(AFX_PMSG)(int (AFX_MSG_CALL CWnd::*)(LPCOMPAREITEMSTRUCT))OnCompareItem },
#define ON_WM_COMPACTING() \
	{ WM_COMPACTING, 0, AfxSig_vw, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT))OnCompacting },
#define ON_WM_NCCREATE() \
	{ WM_NCCREATE, 0, AfxSig_is, \
		(AFX_PMSG)(BOOL (AFX_MSG_CALL CWnd::*)(LPCREATESTRUCT))OnNcCreate },
#define ON_WM_NCDESTROY() \
	{ WM_NCDESTROY, 0, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))OnNcDestroy },
#define ON_WM_NCCALCSIZE() \
	{ WM_NCCALCSIZE, 0, AfxSig_vCALC, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(NCCALCSIZE_PARAMS FAR*))OnNcCalcSize },
#define ON_WM_NCHITTEST() \
	{ WM_NCHITTEST, 0, AfxSig_wp, \
		(AFX_PMSG)(UINT (AFX_MSG_CALL CWnd::*)(CPoint))OnNcHitTest },
#define ON_WM_NCPAINT() \
	{ WM_NCPAINT, 0, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))OnNcPaint },
#define ON_WM_NCACTIVATE() \
	{ WM_NCACTIVATE, 0, AfxSig_bb, \
		(AFX_PMSG)(BOOL (AFX_MSG_CALL CWnd::*)(BOOL))OnNcActivate },
#define ON_WM_GETDLGCODE() \
	{ WM_GETDLGCODE, 0, AfxSig_wv, \
		(AFX_PMSG)(UINT (AFX_MSG_CALL CWnd::*)(void))OnGetDlgCode },
#define ON_WM_NCMOUSEMOVE() \
	{ WM_NCMOUSEMOVE, 0, AfxSig_vwp, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, CPoint))OnNcMouseMove },
#define ON_WM_NCLBUTTONDOWN() \
	{ WM_NCLBUTTONDOWN, 0, AfxSig_vwp, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, CPoint))OnNcLButtonDown },
#define ON_WM_NCLBUTTONUP() \
	{ WM_NCLBUTTONUP, 0, AfxSig_vwp, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, CPoint))OnNcLButtonUp },
#define ON_WM_NCLBUTTONDBLCLK() \
	{ WM_NCLBUTTONDBLCLK, 0, AfxSig_vwp, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, CPoint))OnNcLButtonDblClk },
#define ON_WM_NCRBUTTONDOWN() \
	{ WM_NCRBUTTONDOWN, 0, AfxSig_vwp, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, CPoint))OnNcRButtonDown },
#define ON_WM_NCRBUTTONUP() \
	{ WM_NCRBUTTONUP, 0, AfxSig_vwp, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, CPoint))OnNcRButtonUp },
#define ON_WM_NCRBUTTONDBLCLK() \
	{ WM_NCRBUTTONDBLCLK, 0, AfxSig_vwp, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, CPoint))OnNcRButtonDblClk },
#define ON_WM_NCMBUTTONDOWN() \
	{ WM_NCMBUTTONDOWN, 0, AfxSig_vwp, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, CPoint))OnNcMButtonDown },
#define ON_WM_NCMBUTTONUP() \
	{ WM_NCMBUTTONUP, 0, AfxSig_vwp, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, CPoint))OnNcMButtonUp },
#define ON_WM_NCMBUTTONDBLCLK() \
	{ WM_NCMBUTTONDBLCLK, 0, AfxSig_vwp, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, CPoint))OnNcMButtonDblClk },
#define ON_WM_KEYDOWN() \
	{ WM_KEYDOWN, 0, AfxSig_vwww, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, UINT, UINT))OnKeyDown },
#define ON_WM_KEYUP() \
	{ WM_KEYUP, 0, AfxSig_vwww, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, UINT, UINT))OnKeyUp },
#define ON_WM_CHAR() \
	{ WM_CHAR, 0, AfxSig_vwww, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, UINT, UINT))OnChar },
#define ON_WM_DEADCHAR() \
	{ WM_DEADCHAR, 0, AfxSig_vwww, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, UINT, UINT))OnDeadChar },
#define ON_WM_SYSKEYDOWN() \
	{ WM_SYSKEYDOWN, 0, AfxSig_vwww, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, UINT, UINT))OnSysKeyDown },
#define ON_WM_SYSKEYUP() \
	{ WM_SYSKEYUP, 0, AfxSig_vwww, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, UINT, UINT))OnSysKeyUp },
#define ON_WM_SYSCHAR() \
	{ WM_SYSCHAR, 0, AfxSig_vwww, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, UINT, UINT))OnSysChar },
#define ON_WM_SYSDEADCHAR() \
	{ WM_SYSDEADCHAR, 0, AfxSig_vwww, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, UINT, UINT))OnSysDeadChar },
#define ON_WM_SYSCOMMAND() \
	{ WM_SYSCOMMAND, 0, AfxSig_vwl, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, LONG))OnSysCommand },
#define ON_WM_TIMER() \
	{ WM_TIMER, 0, AfxSig_vw, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT))OnTimer },
#define ON_WM_HSCROLL() \
	{ WM_HSCROLL, 0, AfxSig_vwwW, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, UINT, CScrollBar*))OnHScroll },
#define ON_WM_VSCROLL() \
	{ WM_VSCROLL, 0, AfxSig_vwwW, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, UINT, CScrollBar*))OnVScroll },
#define ON_WM_INITMENU() \
	{ WM_INITMENU, 0, AfxSig_vM, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(CMenu *))OnInitMenu },
#define ON_WM_INITMENUPOPUP() \
	{ WM_INITMENUPOPUP, 0, AfxSig_vMwb, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(CMenu *, UINT, BOOL))OnInitMenuPopup },
#ifdef _NTWIN
#define ON_WM_MENUSELECT() \
	{ WM_MENUSELECT, 0, AfxSig_vwwh, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, UINT, HMENU))OnMenuSelect },
#else
#define ON_WM_MENUSELECT() \
	{ WM_MENUSELECT, 0, AfxSig_vwww, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, UINT, HMENU))OnMenuSelect },
#endif
#define ON_WM_MENUCHAR() \
	{ WM_MENUCHAR, 0, AfxSig_lwwM, \
		(AFX_PMSG)(LONG (AFX_MSG_CALL CWnd::*)(UINT, UINT, CMenu *))OnMenuChar },
#define ON_WM_ENTERIDLE() \
	{ WM_ENTERIDLE, 0, AfxSig_vwW, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, CWnd*))OnEnterIdle },
#define ON_WM_MOUSEMOVE() \
	{ WM_MOUSEMOVE, 0, AfxSig_vwp, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, CPoint))OnMouseMove },
#define ON_WM_LBUTTONDOWN() \
	{ WM_LBUTTONDOWN, 0, AfxSig_vwp, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, CPoint))OnLButtonDown },
#define ON_WM_LBUTTONUP() \
	{ WM_LBUTTONUP, 0, AfxSig_vwp, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, CPoint))OnLButtonUp },
#define ON_WM_LBUTTONDBLCLK() \
	{ WM_LBUTTONDBLCLK, 0, AfxSig_vwp, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, CPoint))OnLButtonDblClk },
#define ON_WM_RBUTTONDOWN() \
	{ WM_RBUTTONDOWN, 0, AfxSig_vwp, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, CPoint))OnRButtonDown },
#define ON_WM_RBUTTONUP() \
	{ WM_RBUTTONUP, 0, AfxSig_vwp, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, CPoint))OnRButtonUp },
#define ON_WM_RBUTTONDBLCLK() \
	{ WM_RBUTTONDBLCLK, 0, AfxSig_vwp, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, CPoint))OnRButtonDblClk },
#define ON_WM_MBUTTONDOWN() \
	{ WM_MBUTTONDOWN, 0, AfxSig_vwp, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, CPoint))OnMButtonDown },
#define ON_WM_MBUTTONUP() \
	{ WM_MBUTTONUP, 0, AfxSig_vwp, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, CPoint))OnMButtonUp },
#define ON_WM_MBUTTONDBLCLK() \
	{ WM_MBUTTONDBLCLK, 0, AfxSig_vwp, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, CPoint))OnMButtonDblClk },
#define ON_WM_PARENTNOTIFY() \
	{ WM_PARENTNOTIFY, 0, AfxSig_vwl, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, LONG))OnParentNotify },
#define ON_WM_MDIACTIVATE() \
	{ WM_MDIACTIVATE, 0, AfxSig_vbWW, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(BOOL, CWnd*, CWnd*))OnMDIActivate },
#define ON_WM_RENDERFORMAT() \
	{ WM_RENDERFORMAT, 0, AfxSig_vw, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT))OnRenderFormat },
#define ON_WM_RENDERALLFORMATS() \
	{ WM_RENDERALLFORMATS, 0, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))OnRenderAllFormats },
#define ON_WM_DESTROYCLIPBOARD() \
	{ WM_DESTROYCLIPBOARD, 0, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))OnDestroyClipboard },
#define ON_WM_DRAWCLIPBOARD() \
	{ WM_DRAWCLIPBOARD, 0, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))OnDrawClipboard },
#define ON_WM_PAINTCLIPBOARD() \
	{ WM_PAINTCLIPBOARD, 0, AfxSig_vWh, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(CWnd*, HANDLE))OnPaintClipboard },
#define ON_WM_VSCROLLCLIPBOARD() \
	{ WM_VSCROLLCLIPBOARD, 0, AfxSig_vWww, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(CWnd*, UINT, UINT))OnVScrollClipboard },
#define ON_WM_SIZECLIPBOARD() \
	{ WM_SIZECLIPBOARD, 0, AfxSig_vWh, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(CWnd*, HANDLE))OnSizeClipboard },
#define ON_WM_ASKCBFORMATNAME() \
	{ WM_ASKCBFORMATNAME, 0, AfxSig_vwl, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(UINT, LPSTR))OnAskCbFormatName },
#define ON_WM_CHANGECBCHAIN() \
	{ WM_CHANGECBCHAIN, 0, AfxSig_vhh, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(HWND, HWND))OnChangeCbChain },
#define ON_WM_HSCROLLCLIPBOARD() \
	{ WM_HSCROLLCLIPBOARD, 0, AfxSig_vWww, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(CWnd*, UINT, UINT))OnHScrollClipboard },
#define ON_WM_QUERYNEWPALETTE() \
	{ WM_QUERYNEWPALETTE, 0, AfxSig_bv, \
		(AFX_PMSG)(BOOL (AFX_MSG_CALL CWnd::*)(void))OnQueryNewPalette },
#define ON_WM_PALETTECHANGED() \
	{ WM_PALETTECHANGED, 0, AfxSig_vW, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(CWnd*))OnPaletteChanged },

#if (WINVER >= 0x030a)
#define ON_WM_PALETTEISCHANGING() \
	{ WM_PALETTEISCHANGING, 0, AfxSig_vW, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(CWnd*))OnPaletteIsChanging },
#define ON_WM_DROPFILES() \
	{ WM_DROPFILES, 0, AfxSig_vh, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(HANDLE))OnDropFiles },
#define ON_WM_WINDOWPOSCHANGING() \
	{ WM_WINDOWPOSCHANGING, 0, AfxSig_vPOS, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(LPWINDOWPOS))OnWindowPosChanging },
#define ON_WM_WINDOWPOSCHANGED() \
	{ WM_WINDOWPOSCHANGED, 0, AfxSig_vPOS, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(LPWINDOWPOS))OnWindowPosChanged },
#endif /* WINVER >= 0x030a */

/////////////////////////////////////////////////////////////////////////////
// Message map tables for Control Notification messages

/* Edit Control Notification Codes */
#define ON_EN_SETFOCUS(id, memberFxn) \
	{ EN_SETFOCUS, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#define ON_EN_KILLFOCUS(id, memberFxn) \
	{ EN_KILLFOCUS, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#define ON_EN_CHANGE(id, memberFxn) \
	{ EN_CHANGE, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#define ON_EN_UPDATE(id, memberFxn) \
	{ EN_UPDATE, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#define ON_EN_ERRSPACE(id, memberFxn) \
	{ EN_ERRSPACE, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#define ON_EN_MAXTEXT(id, memberFxn) \
	{ EN_MAXTEXT, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#define ON_EN_HSCROLL(id, memberFxn) \
	{ EN_HSCROLL, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#define ON_EN_VSCROLL(id, memberFxn) \
	{ EN_VSCROLL, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },

/* User Button Notification Codes */
#define ON_BN_CLICKED(id, memberFxn) \
	{ BN_CLICKED, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#define ON_BN_PAINT(id, memberFxn) \
	{ BN_PAINT, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#define ON_BN_HILITE(id, memberFxn) \
	{ BN_HILITE, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#define ON_BN_UNHILITE(id, memberFxn) \
	{ BN_UNHILITE, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#define ON_BN_DISABLE(id, memberFxn) \
	{ BN_DISABLE, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#define ON_BN_DOUBLECLICKED(id, memberFxn) \
	{ BN_DOUBLECLICKED, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },

/* Listbox Notification Codes */
#define ON_LBN_ERRSPACE(id, memberFxn) \
	{ LBN_ERRSPACE, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#define ON_LBN_SELCHANGE(id, memberFxn) \
	{ LBN_SELCHANGE, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#define ON_LBN_DBLCLK(id, memberFxn) \
	{ LBN_DBLCLK, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#define ON_LBN_SETFOCUS(id, memberFxn) \
	{ LBN_SETFOCUS, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#define ON_LBN_KILLFOCUS(id, memberFxn) \
	{ LBN_KILLFOCUS, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },

/* Combo Box Notification Codes */
#define ON_CBN_ERRSPACE(id, memberFxn) \
	{ CBN_ERRSPACE, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#define ON_CBN_SELCHANGE(id, memberFxn) \
	{ CBN_SELCHANGE, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#define ON_CBN_DBLCLK(id, memberFxn) \
	{ CBN_DBLCLK, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#define ON_CBN_SETFOCUS(id, memberFxn) \
	{ CBN_SETFOCUS, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#define ON_CBN_KILLFOCUS(id, memberFxn) \
	{ CBN_KILLFOCUS, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#define ON_CBN_EDITCHANGE(id, memberFxn) \
	{ CBN_EDITCHANGE, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#define ON_CBN_EDITUPDATE(id, memberFxn) \
	{ CBN_EDITUPDATE, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#define ON_CBN_DROPDOWN(id, memberFxn) \
	{ CBN_DROPDOWN, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#if (WINVER >= 0x030a)
#define ON_CBN_CLOSEUP(id, memberFxn)  \
	{ CBN_CLOSEUP, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#define ON_CBN_SELENDOK(id, memberFxn)  \
	{ CBN_SELENDOK, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#define ON_CBN_SELENDCANCEL(id, memberFxn)  \
	{ CBN_SELENDCANCEL, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#endif  /* WINVER >= 0x030a */

/////////////////////////////////////////////////////////////////////////////
// User extensions for message map entries

// for Menu/Accelerator/Pushbutton commands
#define ON_COMMAND(id, memberFxn) \
	{ 0, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },

// for general controls
#define ON_CONTROL(wNotifyCode, id, memberFxn) \
	{ wNotifyCode, id, AfxSig_vv, \
		(AFX_PMSG)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },

// for Windows messages
#define ON_MESSAGE(message, memberFxn) \
	{ message, 0, AfxSig_lwl, \
		(AFX_PMSG)(LONG (AFX_MSG_CALL CWnd::*)(UINT, LONG))memberFxn },

// for Registered Windows messages
#define ON_REGISTERED_MESSAGE(nMessageVariable, memberFxn) \
	{ 0xC000, 0, (UINT)(UINT NEAR*)(&nMessageVariable), \
		/*implied 'AfxSig_lwl'*/ \
		(AFX_PMSG)(LONG (AFX_MSG_CALL CWnd::*)(UINT, LONG))memberFxn },

/////////////////////////////////////////////////////////////////////////////
