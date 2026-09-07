#include "hdrs.h"

#ifndef _PPCMAC
#include <initguid.h>
#else

#undef DEFINE_OLEGUID
#undef DEFINE_GUID
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    const GUID name = { l, w1, w2, b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 }
#define DEFINE_OLEGUID(name, l, w1, w2) \
    DEFINE_GUID(name, l, w1, w2, 0xC0,0,0,0,0,0,0,0x46)

#endif 
#include "mycoguid.h"


CApplication *	gApp;
char			gAppName[] = "Simple Container";

ASSERTDATA

	
CApplication::CApplication(void)
{		
	m_window			= nil;
	m_fDone				= false;
	m_fInForeground 	= true;
	m_pIMessageFilter   = nil;
	m_nWndCount			= 0;
}


CApplication::~CApplication(void)
{
}


void CApplication::InitApplication(void)
{
	int				loop;
	EventRecord		event;
	short			nMasters;

	InitToolbox();

	// Increase stack space by kExtraStackSpace bytes
	SetApplLimit(GetApplLimit() - kExtraStackSpace);
	MaxApplZone();

	// Allocate additional master pointer blocks
	nMasters = kExtraMasters;
	while (nMasters-- > 0)
		MoreMasters();

	// Do all of our menu initialization
	SetupMenus();

	// Wait for the app to come to the foreground
	for (loop = 0 ; loop < 3 ; loop++)
		EventAvail(everyEvent, &event);

	// Install our AppleEvent handlers
#ifdef __powerc
	AEInstallEventHandler(kCoreEventClass, kAEOpenApplication, NewAEEventHandlerProc(HandleOpenApplication), 0, false);
	AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments, NewAEEventHandlerProc(HandleOpenDocument), 0, false);
	AEInstallEventHandler(kCoreEventClass, kAEPrintDocuments, NewAEEventHandlerProc(HandlePrintDocument), 0, false);
	AEInstallEventHandler(kCoreEventClass, kAEQuitApplication, NewAEEventHandlerProc(HandleQuitApplication), 0, false);
#else
	AEInstallEventHandler(kCoreEventClass, kAEOpenApplication, (AEEventHandlerUPP)HandleOpenApplication, 0, false);
	AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments, (AEEventHandlerUPP)HandleOpenDocument, 0, false);
	AEInstallEventHandler(kCoreEventClass, kAEPrintDocuments, (AEEventHandlerUPP)HandlePrintDocument, 0, false);
	AEInstallEventHandler(kCoreEventClass, kAEQuitApplication, (AEEventHandlerUPP)HandleQuitApplication, 0, false);
#endif

	OleInitApplication();

	CreateNewDocument();
}


void CApplication::InitToolbox(void)
{
	InitGraf((Ptr)&qd.thePort);
	InitFonts();
	FlushEvents(everyEvent, 0);
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(NULL);
	InitCursor();
}
		
		
void CApplication::SetupMenus(void)
{		
	Handle		menuBar;
	MenuHandle	theMenu;

	menuBar = GetNewMBar(kMenuBar);
	if (!menuBar)
		FailAlert("\pCould not create menu bar.");

	SetMenuBar(menuBar);
	DisposeHandle(menuBar);
	
	theMenu = GetMenu(mFile);
	if (!theMenu)
		FailAlert("\pCould not retrieve menu resource.");

	InsertMenu(theMenu, -1);
	
	theMenu=GetMHandle(mApple);
	if (!theMenu)
		FailAlert("\pCould not retrieve menu resource.");
	
	theMenu=GetMenu(mObject);
	if (theMenu)
		InsertMenu(theMenu,-1);
		
	AddResMenu(GetMHandle(mApple), 'DRVR');

	DrawMenuBar();
}
	
		
void CApplication::DisposeApplication(void)
{
	OleCleanup();
}


void CApplication::OleInitApplication(void)
{
	HRESULT		hr;

#ifndef __powerc
	hr = InitOleManager(OLEMGR_BIND_NORMAL);
	FailScode("\pInitOleManager()", GetScode(hr));
#endif

	hr = OleInitialize(NULL);
	FailScode("\pOleInitialize()", GetScode(hr));

	OleMessageFilterRegister(this);
}


void CApplication::OleCleanup(void)
{
	if (m_pIMessageFilter)
		OleMessageFilterRevoke(gApp);

	OleUninitialize();

#ifndef __powerc
	UninitOleManager();
#endif
}


void CApplication::EventLoop(void)
{
	EventRecord		event;

	while (!m_fDone)
	{
		WaitNextEvent(everyEvent, &event, 0, NULL);
		HandleEvent(event);
	}
}	

			
void CApplication::AdjustMenus(void)
{
	MenuHandle	theMenu;

	theMenu = GetMenu(mEdit);
	EnDisItem(theMenu, iInsertObject, FrontWindow() != NULL);

	AddVerbs();

	theMenu = GetMenu(mFile);
	EnDisItem(theMenu, iClose, FrontWindow() != NULL);
}


void CApplication::HandleEvent(EventRecord event)
{	
	switch (event.what)
	{	
		case nullEvent:
			HandleNullEvent();
			break;	

		case mouseDown:
			HandleMouseDown(event);
			break;

		case mouseUp:
			HandleMouseUp();
			break;

		case keyDown:
		case autoKey:
			HandleKeyDown(event);
			break;
		
		case updateEvt:
			HandleUpdate(event);
			break;
			
		case diskEvt:
			HandleDiskEvent(event);
			break;
		
		case activateEvt:
			HandleActivate(event);
			break;
		
		case osEvt:
			HandleOSEvent(event);
			break;
		
		case kHighLevelEvent:
			AEProcessAppleEvent(&event);
			break;
	}
}


void CApplication::HandleNullEvent(void)
{
	Point			pt;
	WindowPtr		theWindow;
	CWindow *		theCWindow;

	if (m_fInForeground)
	{
		GetGlobalMouse(&pt);

		FindWindow(pt, &theWindow);
		if (theWindow)
		{
			theCWindow = (CWindow *)GetWRefCon(theWindow);
			ASSERT(theCWindow != NULL);
			theCWindow->AdjustCursor();
		}
		else
			SetCursor(&qd.arrow); 
	}
}


void CApplication::HandleMouseDown(EventRecord event)
{
	CWindow *	theCWindow;
	WindowPtr	theWindow;
	short		thePart;

	thePart = FindWindow(event.where, &theWindow);

	theCWindow = (CWindow *)GetWRefCon(theWindow);
	ASSERT(theCWindow != NULL);

	switch (thePart)
	{
		case inContent:
			if (theWindow != FrontWindow())
				SelectWindow(theWindow);
			else
				theCWindow->HandleContentClick();
			break;

		case inGoAway:
			if (TrackGoAway(theWindow, event.where))
				theCWindow->HandleClose();
			break;

		case inGrow:
			theCWindow->HandleGrow(event);
			break;

		case inZoomIn:
		case inZoomOut:
			if (TrackBox(theWindow, event.where, thePart))
				theCWindow->HandleZoom(thePart);
			break;

		case inDrag:
			DragWindow(theWindow, event.where, &(**GetGrayRgn()).rgnBBox);
			break;

		case inMenuBar:
			AdjustMenus();
			HandleMenuCommand(MenuSelect(event.where));
			break;

		case inSysWindow:
			SystemClick(&event, theWindow);
			break;
	}
}


void CApplication::HandleMouseUp(void)
{
	CWindow *	theCWindow;

	theCWindow = (CWindow *)GetWRefCon(FrontWindow());
	ASSERT(theCWindow != NULL);

	theCWindow->HandleMouseUp();
}


void CApplication::HandleKeyDown(EventRecord event)
{
	long		lResult;
	CWindow *	theCWindow;

	if (event.modifiers & cmdKey)
	{
		AdjustMenus();
		lResult = MenuKey((short) event.message & charCodeMask);
		if (HiWord(lResult))
		{
			InitCursor();
			HandleMenuCommand(lResult);
		}
		return;
	}

	if (FrontWindow())
	{
		theCWindow = (CWindow *)GetWRefCon(FrontWindow());
		ASSERT(theCWindow != NULL);

		theCWindow->HandleKeyDown(event);
	}
}


void CApplication::HandleActivate(EventRecord event)
{
	CWindow *	theCWindow;
	Boolean		fActivating = event.modifiers && activeFlag;

	theCWindow = (CWindow *)GetWRefCon((WindowPtr)event.message);
	ASSERT(theCWindow != NULL);

	theCWindow->HandleActivate(fActivating);
}


void CApplication::HandleUpdate(EventRecord event)
{
	CWindow *		theCWindow;

	theCWindow = (CWindow *)GetWRefCon((WindowPtr)event.message);
	ASSERT(theCWindow != NULL);

	theCWindow->HandleUpdate();
}


void CApplication::HandleDiskEvent(EventRecord event)
{
	Point	thePoint;

	if (HiWord(event.message) != noErr)
	{
		thePoint.h = 0x0050;
		thePoint.v = 0x0070;
		(void) DIBadMount(thePoint, event.message);		// Ignore the result
	}
}


void CApplication::HandleOSEvent(EventRecord event)
{
	if ((unsigned long)event.message >> 24 == suspendResumeMessage)
	{
		m_fInForeground = (Boolean)(event.message & resumeFlag);

#if 0
		if (m_document)
			m_document->HiliteWindow(m_fInForeground);
#endif
	}
}


void CApplication::HandleMenuCommand(long menuResult)
{
	short		menuID   = HiWord(menuResult),
				menuItem = LoWord(menuResult);
	Str255		daName;
	
	switch (menuID) {

		case mApple:		// Apple menu
			if (menuItem==1) {
					Alert(kAboutAlert,nil);
			}
			else {
				GetItem(GetMHandle(mApple), menuItem, daName);
				OpenDeskAcc(daName);
			}
			break;

		case mFile:		   // File menu
			switch (menuItem) {
			
				case iNew:
					CreateNewDocument();
					break;
				
				case iClose:
					m_window->HandleClose();
					break;
					
				case iQuit:					// Quit item
					gApp->m_fDone = true;
					break;
			}
			break;
			
		case mEdit:		   // File menu
			switch (menuItem) {
			
				case iInsertObject:
					m_window->ShowInsertObjectDlg();
					break;
			}
			break;
			
		case mObject:
			CallDoVerb(menuItem - 1); 
			break;
		}
		
		
	HiliteMenu(0);
}


void CApplication::CreateNewDocument(void)
{
	m_window = new CWindow();
	if (m_window == NULL)
		FailAlert("\pCould not instantiate CWindow object");

	m_window->InitWindow();

	m_window->m_pObjectSite = new CObjectSite;
	if (m_window->m_pObjectSite == NULL)
		FailAlert("\pCould not instantiate CObjectSite object");

	m_window->m_pObjectSite->InitObjectSite(m_window->m_pWindow);
	m_window->m_pObjectSite->AddRef();
}


void CApplication::AddVerbs(void)
{	
	LPENUMOLEVERB	lpEnumOleVerb;
	OLEVERB			oleverb;
	short			i,nCount;
	long			lNextVerbExpected;
	MenuHandle		mh;
	HRESULT			hr;
	LPOLEOBJECT		pObj;
	long			ref;
	
	lpEnumOleVerb=(LPENUMOLEVERB)'AAAA';	
	
	mh=GetMHandle(mObject);
	
	if (!mh)
	{
		WarningAlert("\pCould not retrieve submenu resource");
		return;
	}
	
	nCount=CountMItems(mh);
	
	for(i=nCount;i>0;i--)
		DelMenuItem(mh, i);
	
	MenuHandle	menu;
	
	menu = GetMenu(mEdit);
	
	if (!menu)
	{
		WarningAlert("\pCould not retrieve Edit menu resource");
		return;
	}
		
	if (!m_window || !m_window->m_pObjectSite->m_pObject)  // return after disabling menu if no obj
	{	
		
		DisableItem(menu, iVerbs);
		return;	
	}
	
	EnableItem(menu, iVerbs);
		
	pObj = 	m_window->m_pObjectSite->m_pObject;						
		
	lNextVerbExpected=0;
	
	hr=pObj->EnumVerbs(&lpEnumOleVerb);
			
	if (GetScode(hr)==OLEOBJ_E_NOVERBS) 
	{
		AppendMenu(mh, "\pPRIMARY");
		m_fHasVerbs = false;
		return;	
	}
	
	m_fHasVerbs = false;
	
	if (GetScode(hr)==OLE_S_USEREG)
	{
		FailScode("\pEnumVerbs()", GetScode(hr));
		return;
	}
		
	if (lpEnumOleVerb==NULL)
	{
		WarningAlert("\plpEnumOleVerb==NULL.");
		return;
	}
		
    while (true) 
    {         // forever
        hr = lpEnumOleVerb->Next(
                1,
                (LPOLEVERB)&oleverb,
                NULL
        );
        if (NOERROR != hr)
            break;              // DONE! no more verbs

        if (oleverb.lVerb < 0) 
            continue;           // skip negative verb numbers

        /* OLE2NOTE: we only support consecutively numbered verbs
        **    starting with verb 0. we will stop adding verbs to the
        **    menu when the first gap in the verb numbers is
        **    encountered. 
        */
        if (oleverb.lVerb != lNextVerbExpected)
            break;              // STOP -- gap in verb numbers encountered

        lNextVerbExpected++;    // next verb should be consecutive
		
		c2pstr(oleverb.lpszVerbName);
        AppendMenu(mh, (StringPtr)oleverb.lpszVerbName);
    }
    
    ref = lpEnumOleVerb->Release();
    ASSERT(ref == 0);
}


SCODE CApplication::CallDoVerb(long verb)
{
	Rect			rect;
	HRESULT			hr;
	SCODE			sc;
	CObjectSite *	pOS;
	
	
	ASSERT(gApp->m_window->m_pObjectSite->m_pObject)
	ASSERT(gApp->m_window->m_pObjectSite)
	
	pOS = gApp->m_window->m_pObjectSite;
	
	pOS->m_pObject->GetExtent(DVASPECT_CONTENT, &pOS->m_ObjSizeL);
    
	rect.left   = 0;
	rect.top    = 0;
	rect.right  = (short)pOS->m_ObjSizeL.cx;
	rect.bottom = (short)pOS->m_ObjSizeL.cy;

	hr = OleRun((LPUNKNOWN)pOS->m_pObject);
	FailScode("\pOleRun()", GetScode(hr));

	if (GetScode(hr) == NOERROR)
	{
		hr = pOS->m_pObject->DoVerb(verb,
					NULL,
					(LPOLECLIENTSITE)pOS->m_pOleClientSite,
					-1,
					gApp->m_window->m_pWindow,
					&rect);
		FailScode("\pDoVerb()", GetScode(hr));
	}

	sc = GetScode(hr);
	
	return sc;
}


void CApplication::BringToForeground(Boolean fWait)
{
	ProcessSerialNumber		currentProcess;
	ProcessSerialNumber		frontProcess;
	Boolean					fSame;
	EventRecord				theEvent;
	unsigned long			timeout = TickCount() + 60 * 10;
	
	GetCurrentProcess(&currentProcess);
	SetFrontProcess(&currentProcess);
	
	// spin until the this process is in front
	while (true && fWait)
	{
		GetFrontProcess(&frontProcess);
		SameProcess(&currentProcess, &frontProcess, &fSame);
		
		if (fSame)
			break;

		if (TickCount() > timeout) {
			ASSERTSZ(false, "timed out trying to switch to the foreground");
			return;
		}

		WaitNextEvent(0, &theEvent, 3, 0);
	}
}
