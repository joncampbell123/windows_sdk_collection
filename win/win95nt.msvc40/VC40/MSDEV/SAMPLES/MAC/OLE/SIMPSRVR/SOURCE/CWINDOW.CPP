#include "hdrs.h"

#define 	WND_OFFSET		7
#define		TopLeft(r)		(*(Point *)&(r).top)
#define		HiWrd(aLong)	(((aLong) >> 16) & 0xFFFF)
#define		LoWrd(aLong)	((aLong) & 0xFFFF)

ASSERTDATA


CWindow::CWindow(void)
{
	m_cRef      = 0;
	m_pWindow   = NULL;
	m_fFloating = false;
}


CWindow::~CWindow(void)
{
	if (m_pWindow)
		DisposeWindow(m_pWindow);
}


HRESULT STDMETHODCALLTYPE CWindow::QueryInterface(REFIID riid, void * * ppv)
{
	*ppv = NULL;

	ENTER_INTERFACE("CWindow::QueryInterface");

	// Note: Use IsEqualIID to compare IIDs if you're using C

	if (riid == IID_IUnknown)
	{
		AddRef();
		*ppv = this;
		return ResultFromScode(S_OK);
	}

	return ResultFromScode(E_NOINTERFACE);
}


unsigned long STDMETHODCALLTYPE CWindow::AddRef(void)
{
	++m_cRef;

	DUMP_REFCOUNT("CWindow::AddRef", m_cRef);

	return m_cRef;
}


unsigned long STDMETHODCALLTYPE CWindow::Release(void)
{
	--m_cRef;
	ASSERT(m_cRef >= 0);

	DUMP_REFCOUNT("CWindow::Release",m_cRef);

	if (m_cRef == 0)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}


OSErr CWindow::InitWindow(short resID, Boolean floating)
{
	if (floating)
		m_pWindow = GetNewWindow(resID, NULL, (WindowPtr)-1);
	else
		m_pWindow = GetNewWindow(resID, NULL, (gApp->m_toolbar ? gApp->m_toolbar->m_pWindow : NULL));

	if (m_pWindow == NULL)
		return memFullErr;

	SetWRefCon(m_pWindow, (long)this);
	m_fFloating = floating;

	return noErr;
}


void CWindow::HandleClose(void)
{
}


void CWindow::HandleKeyDown(EventRecord event)
{
}


void CWindow::HandleActivate(Boolean fActivating)
{
	AdjustCursor();
}


void CWindow::HandleUpdate(void)
{
	WindowPtr	save_port;

	ASSERT(m_pWindow);

	GetPort(&save_port);
	SetPort(m_pWindow);

	BeginUpdate(m_pWindow);
	DrawContents();
	EndUpdate(m_pWindow);

	SetPort(save_port);
}


void CWindow::HandleZoom(short part)
{
	GrafPtr 	savePort;

	GetPort(&savePort);
	SetPort(m_pWindow);

	EraseRect(&m_pWindow->portRect);

	ZoomWindow(m_pWindow, part, false);
	gApp->SelectTheWindow(m_pWindow);

	OnWindowResize();
}


void CWindow::HandleGrow(Point ptStart)
{
	long		growResult;
	Rect		r1,
				r2;

	SetPort(m_pWindow);

	r2 = m_pWindow->portRect;

	SetRect(&r1, MIN_WINDOW_X, MIN_WINDOW_Y, 32767, 32767);

	growResult = GrowWindow(m_pWindow, ptStart, &r1);
	if (growResult != 0) 
		SizeWindow(m_pWindow, LoWord(growResult), HiWord(growResult), true);

	r1 = m_pWindow->portRect;
	r1.top = MIN(r1.bottom, r2.bottom) - 15;

	InvalRect(&r1);

	r1 = m_pWindow->portRect;
	r1.left = MIN(r1.right, r2.right) - 15;

	InvalRect(&r1);

	OnWindowResize();
}


void CWindow::HandleDrag(EventRecord event)
{
	GrafPtr		deskPort;
	RgnHandle	oldClip,
				dragRgn;
	Point		startPt;
	long		offset;
	short		xoff, yoff;

	if (m_fFloating)
	{
		DragWindow(m_pWindow, event.where, &(**GetGrayRgn()).rgnBBox);
		return;
	}

	if (!(event.modifiers & cmdKey))
		gApp->SelectTheWindow(m_pWindow);

	GetWMgrPort(&deskPort);
	SetPort(deskPort);

	oldClip = NewRgn();
	CopyRgn(deskPort->clipRgn, oldClip);

	CopyRgn(GetGrayRgn(), deskPort->clipRgn);
	ClipAbove((WindowRef)m_pWindow);

	dragRgn = NewRgn();
	CopyRgn(((WindowPeek)m_pWindow)->strucRgn, dragRgn);

	startPt = event.where;
	offset = DragGrayRgn(dragRgn, startPt, &(**GetGrayRgn()).rgnBBox, &(**GetGrayRgn()).rgnBBox, noConstraint, NULL);

	DisposeRgn(dragRgn);

	CopyRgn(oldClip, deskPort->clipRgn);
	DisposeRgn(oldClip);

	SetPort(m_pWindow);

	if (offset != 0x80008000)
	{
		startPt = TopLeft((**((WindowPeek)m_pWindow)->contRgn).rgnBBox);
		xoff = (short) LoWrd(offset);
		yoff = (short) HiWrd(offset);
		if (startPt.v + yoff > GetMBarHeight() + 5)
			MoveWindow(m_pWindow, startPt.h + xoff, startPt.v + yoff, false);
	}
}


void CWindow::HiliteWindow(Boolean hilite)
{
	::HiliteWindow(m_pWindow, hilite);
}
