#ifndef _MSC_VER
#pragma once
#endif

#ifndef _WINDOWS_H_
#define _WINDOWS_H_

#define	kObjectWindow		128
#define kToolbarWindow		129

class 	CServerObject;
class	CShape;

class CWindow : public IUnknown
{
	public:

	unsigned long		m_cRef;
	WindowPtr 			m_pWindow;
	Boolean				m_fFloating;

	CWindow(void);
	~CWindow(void);

    // *** IUnknown methods ***
    virtual HRESULT STDMETHODCALLTYPE	QueryInterface(REFIID riid, void * * ppvObj);
    virtual unsigned long STDMETHODCALLTYPE		AddRef(void);
    virtual unsigned long STDMETHODCALLTYPE		Release(void);

	OSErr			InitWindow			(short resID, Boolean floating);

	virtual void 	HandleClose			(void);
	virtual void 	HandleKeyDown		(EventRecord event);
	virtual void 	HandleActivate		(Boolean activate);
	virtual void 	HandleUpdate		(void);
	virtual void 	HandleZoom			(short part);
	virtual void 	HandleGrow			(Point ptStart);
	virtual void	HandleDrag			(EventRecord event);

	virtual void	HiliteWindow		(Boolean hilite);

	virtual void 	AdjustCursor		(void) = 0;
	virtual	void	DrawContents		(void) = 0;

	virtual	void	OnWindowResize		(void) = 0;
	virtual void	HandleContentClick	(Point pt) = 0;
};

#endif // _WINDOWS_H_
