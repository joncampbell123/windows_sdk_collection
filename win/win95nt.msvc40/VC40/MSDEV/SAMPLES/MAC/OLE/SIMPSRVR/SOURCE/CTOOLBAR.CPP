#include "hdrs.h"


ASSERTDATA


CToolbar::CToolbar(void)
{
	m_pWindow   = NULL;
	m_PicHandle = NULL;
	m_Tool	    = NO_TOOL;
}


CToolbar::~CToolbar(void)
{
}


OSErr CToolbar::InitToolbar(void)
{
	OSErr	result;

	result = InitWindow(kToolbarWindow, true);
	if (result != noErr)
	{
		ASSERTSZ(false, "Could not create toolbar window.");
		return result;
	}

	m_PicHandle = (PicHandle)Get1Resource('PICT', kToolbarPict);
	ASSERTSZ(m_PicHandle != NULL, "Could not retrieve toolbar 'PICT' resource.");

	::HiliteWindow(m_pWindow, true);
	SelectWindow(m_pWindow);
	ShowWindow(m_pWindow);

	return noErr;
}


void CToolbar::AdjustCursor(void)
{
	SetCursor(&qd.arrow);
}


void CToolbar::DrawContents(void)
{
	if (m_PicHandle)
		DrawPicture(m_PicHandle, &m_pWindow->portRect);

	if (m_Tool != NO_TOOL)
		InvertRect(&m_updateRect);
}


void CToolbar::OnWindowResize(void)
{
}


void CToolbar::HandleContentClick(Point pt)
{
	short		tool;
	WindowPtr	save_port;
	Rect		updateRect;

	GetPort(&save_port);
	SetPort(m_pWindow);

	GlobalToLocal(&pt);

	tool = pt.v / TOOL_Y; 

	m_Tool = tool;

	updateRect.left   = 1;
	updateRect.top    = 1;
	updateRect.right  = TOOL_X - 1;
	updateRect.bottom = TOOL_Y;

	OffsetRect(&updateRect, 0, TOOL_Y * m_Tool);

	m_updateRect = updateRect;

	InvalRect(&m_pWindow->portRect);

	SetPort(save_port);
}
