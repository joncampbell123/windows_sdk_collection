#ifndef _MSC_VER
#pragma once
#endif

#ifndef _WINDOWS_H_
#define _WINDOWS_H_

#define	kObjectWindow		128

#include "site.h"

class CWindow
{
	public:
	
	WindowPtr 			m_pWindow;
	CObjectSite			*m_pObjectSite;
	COleClientSite		*m_pOleClientSite;
	long				m_Click;
		
	CWindow(void);
	~CWindow(void);
	
	OSErr			InitWindow			(void);

	void 			HandleClose			(void);
	void 			HandleKeyDown		(EventRecord event);
	void 			HandleMouseUp		(void);
	void 			HandleActivate		(Boolean fActivating);
	void 			HandleUpdate		(void);
	void 			HandleZoom			(short part);
	void 			HandleGrow			(EventRecord event);

	void			ClipContents		(Boolean fClip);

	void 			AdjustCursor		(void);
	void 			DrawContents		(void);
	
	void			HandleContentClick	(void);

	void			ShowInsertObjectDlg	(void);
};

#endif // _WINDOWS_H_
