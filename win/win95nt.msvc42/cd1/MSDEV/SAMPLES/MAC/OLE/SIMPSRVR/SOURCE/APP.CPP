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
DEFINE_OLEGUID(CLSID_SimpSrvr, 0x00000410L, 0, 0);

#include "mycoguid.h"

CApplication *	gApp;
char			gAppName[] = "Simple Server";

ASSERTDATA


CApplication::CApplication(void)
{
	m_cRef              = 0;
	m_cDoc				= 0;
	m_document			= NULL;
	m_toolbar			= NULL;
	m_fDone				= false;
	m_fInForeground 	= true;
	m_fUserInControl    = true;
	m_nWndCount			= 0;
	m_ClassFactory      = NULL;
	m_CoRegisterResult  = 0;
	m_pIMessageFilter   = NULL;
}


CApplication::~CApplication(void)
{
}


HRESULT STDMETHODCALLTYPE CApplication::QueryInterface(REFIID riid, void * * ppv)
{
	*ppv = NULL;

	ENTER_INTERFACE("CApplication::QueryInterface");

	// Note: Use IsEqualIID to compare IIDs if you're using C

	if (riid == IID_IUnknown)
	{
		AddRef();
		*ppv = this;
		return ResultFromScode(S_OK);
	}

	return ResultFromScode(E_NOINTERFACE);
}


unsigned long STDMETHODCALLTYPE CApplication::AddRef(void)
{
	++m_cRef;

	DUMP_REFCOUNT("CApplication::AddRef", m_cRef);

	return m_cRef;
}


unsigned long STDMETHODCALLTYPE CApplication::Release(void)
{
	--m_cRef;
	ASSERT(m_cRef >= 0);

	DUMP_REFCOUNT("CApplication::Release",m_cRef);

	if (m_cRef == 0)
	{
		// We don't want to delete the Application object
		// until we've exited our event loop.
	}

	return m_cRef;
}


void CApplication::InitApplication(void)
{
	int				loop;
	EventRecord		event;
	short			nMasters;
	CursHandle		ch;
	OSErr			err;

	InitToolbox();

	// Increase stack space by kExtraStackSpace bytes
	SetApplLimit(GetApplLimit() - kExtraStackSpace);
	MaxApplZone();

	// Allocate additional master pointer blocks
	nMasters = kExtraMasters;
	while (nMasters-- > 0)
		MoreMasters();

	// Setup the crosshair cursor
	ch = GetCursor(kCrosshairCursor);
	if (ch == NULL)
		FailAlert("\pCould not retrieve cursor resource.");
	m_crosshair = **ch;
	ReleaseResource((Handle)ch);

	// Do all of our menu initialization
	SetupMenus();

	// Wait for the app to come to the foreground
	for (loop = 0 ; loop < 3 ; loop++)
		EventAvail(everyEvent, &event);

	// Install our AppleEvent handlers
#ifdef __powerc
	AEInstallEventHandler(kCoreEventClass, kAEOpenApplication, NewAEEventHandlerProc(HandleOpenApplication), (long)gApp, false);
	AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments, NewAEEventHandlerProc(HandleOpenDocument), 0, false);
	AEInstallEventHandler(kCoreEventClass, kAEPrintDocuments, NewAEEventHandlerProc(HandlePrintDocument), 0, false);
	AEInstallEventHandler(kCoreEventClass, kAEQuitApplication, NewAEEventHandlerProc(HandleQuitApplication), 0, false);
	AEInstallEventHandler(typeDDE, typeEMBD, NewAEEventHandlerProc(HandleOleEmbedding), (long)gApp, false);
#else
	AEInstallEventHandler(kCoreEventClass, kAEOpenApplication, (AEEventHandlerUPP)HandleOpenApplication, (long)gApp, false);
	AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments, (AEEventHandlerUPP)HandleOpenDocument, 0, false);
	AEInstallEventHandler(kCoreEventClass, kAEPrintDocuments, (AEEventHandlerUPP)HandlePrintDocument, 0, false);
	AEInstallEventHandler(kCoreEventClass, kAEQuitApplication, (AEEventHandlerUPP)HandleQuitApplication, 0, false);
	AEInstallEventHandler(typeDDE, typeEMBD, (AEEventHandlerUPP)HandleOleEmbedding, (long)gApp, false);
#endif

	m_toolbar = new CToolbar;
	ASSERTSZ(m_toolbar != NULL, "Could not instantiate toolbar.");

	err = m_toolbar->InitToolbar();
	FailOSErr("\pCToolBar::InitToolbar()", err);

	OleInitApplication();
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
	ASSERT(m_cDoc == 0);
	ASSERT(m_cRef == 0);

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

	OleUpdateRegDB();

	AddRef();		// artifical AddRef

	RegisterClassFactory();

	OleMessageFilterRegister(this);

	ASSERTCOND(m_fUserInControl == true);
	OleLockApplication(true, false);

	Release();		// artifical release
}


void CApplication::OleUpdateRegDB(void)
{
	// Ole 1.0 Reg Database
	{
		OLEREG_HKEY 	hKey;
		char			buf[256];
		long			cb = sizeof(buf);

		hKey = OleregOpenRegistration();

		if (OleregGetValue(kSimpleServerProgID, OLEREGSVR_HUMAN_READABLE, 0, buf, &cb) != OLEREG_OK)
		{
			OleregSetValue(kSimpleServerProgID, OLEREGSVR_HUMAN_READABLE, REG_IGNORE, kSimpleServerFullUserTypeName, strlen(kSimpleServerFullUserTypeName));
			OleregSetValue(kSimpleServerProgID, OLEREGSVR_SIGNATURE, REG_IGNORE, kSimpleServerTypeStr, 4);
			OleregSetValue(kSimpleServerProgID, OLEREGSVR_VERB, REG_REPLACE, "Edit", 4);
			OleregSetValue(kSimpleServerProgID, OLEREGSVR_VERB, REG_INSERT, "Play", 4);
		}

		OleregCloseRegistration(hKey);
	}

	// Ole 2.0 Reg Database
	{
		HKEY	hKey;

		if (ResultFromScode(RegOpenKey(HKEY_CLASSES_ROOT, kSimpleServerTypeStr, &hKey)) == S_OK)
			RegCloseKey(hKey);
		else
		{

#if  !defined(applec)

		WriteToOleReg("CLSID\\" kSimpleServerCLSID, kSimpleServerFullUserTypeName);
		WriteToOleReg("CLSID\\" kSimpleServerCLSID "\\Insertable", "");
		WriteToOleReg("CLSID\\" kSimpleServerCLSID "\\LocalServer", kSimpleServerTypeStr);
		WriteToOleReg("CLSID\\"kSimpleServerCLSID"\\ProgID", kSimpleServerProgID);
		WriteToOleReg("CLSID\\"kSimpleServerCLSID"\\AuxUserType\\2", kSimpleServerShortUserTypeName);
		WriteToOleReg("CLSID\\"kSimpleServerCLSID"\\AuxUserType\\3", kSimpleServerAppName);
		WriteToOleReg("CLSID\\"kSimpleServerCLSID"\\InprocHandler", "OLE2:Def$DefFSet");
		WriteToOleReg("CLSID\\"kSimpleServerCLSID"\\DataFormats\\GetSet\\0", "PICT,1,32,1");
		WriteToOleReg("CLSID\\"kSimpleServerCLSID"\\MiscStatus", "528");
		WriteToOleReg(kSimpleServerTypeStr, kSimpleServerCLSID);
		WriteToOleReg(kSimpleServerProgID"\\CLSID", kSimpleServerCLSID);
		WriteToOleReg(kSimpleServerProgID"\\Insertable","");

#else	
			// latest vers of MPW c++ cannot compile the code above.
			// If your compiler is able to digest the syntax above, GOOD,
			// turn the code on.  You NEED it.
			
		{
		char Key[256];

			sprintf(Key,"%s%s%s","CLSID\\",kSimpleServerCLSID,"");
			WriteToOleReg(Key, kSimpleServerFullUserTypeName);
			sprintf(Key,"%s%s%s","CLSID\\",kSimpleServerCLSID,"\\Insertable");
			WriteToOleReg(Key, "");

			sprintf(Key,"%s%s%s","CLSID\\",kSimpleServerCLSID,"\\LocalServer");
			WriteToOleReg(Key, kSimpleServerTypeStr);

			sprintf(Key,"%s%s%s","CLSID\\",kSimpleServerCLSID,"\\ProgID");
			WriteToOleReg(Key, kSimpleServerProgID);

			sprintf(Key,"%s%s%s","CLSID\\",kSimpleServerCLSID,"\\AuxUserType\\2");
			WriteToOleReg(Key, kSimpleServerShortUserTypeName);

			sprintf(Key,"%s%s%s","CLSID\\",kSimpleServerCLSID,"\\AuxUserType\\3");
			WriteToOleReg(Key, kSimpleServerAppName);

			sprintf(Key,"%s%s%s","CLSID\\",kSimpleServerCLSID,"\\InprocHandler");
			WriteToOleReg(Key, "OLE2:Def$DefFSet");

			sprintf(Key,"%s%s%s","CLSID\\",kSimpleServerCLSID,"\\DataFormats\\GetSet\\0");
			WriteToOleReg(Key, "PICT,1,32,1");

			sprintf(Key,"%s%s%s","CLSID\\",kSimpleServerCLSID,"\\MiscStatus");
			WriteToOleReg(Key, "528");

			sprintf(Key,"%s%s%s",kSimpleServerTypeStr,"","");
			WriteToOleReg(Key, kSimpleServerCLSID);


			sprintf(Key,"%s%s%s",kSimpleServerProgID,"\\CLSID","");
			WriteToOleReg(Key, kSimpleServerCLSID);

			sprintf(Key,"%s%s%s",kSimpleServerProgID,"\\Insertable","");
			WriteToOleReg(Key,"");

		}

#endif
		}
	}
}


void CApplication::RegisterClassFactory(void)
{
	HRESULT		hrErr;

	m_ClassFactory = new CClassFactory;
	if (!m_ClassFactory)
		FailAlert("\pCould not instantiate IClassFactory");

	hrErr = CoRegisterClassObject(CLSID_SimpSrvr,
								  m_ClassFactory,
								  CLSCTX_LOCAL_SERVER,
                          REGCLS_MULTI_SEPARATE,
                          &m_CoRegisterResult);
	FailScode("\pCoRegisterClassObject()", GetScode(hrErr));
}


void CApplication::OleCleanup(void)
{
	HRESULT		hr;

	if (m_ClassFactory)
	{
		hr = CoRevokeClassObject(m_CoRegisterResult);
		FailScode("\pCoRevokeClassObject", GetScode(hr));
	}

	if (m_pIMessageFilter)
		OleMessageFilterRevoke(gApp);

	OleUninitialize();
#ifndef __powerc
	UninitOleManager();
#endif
}


HRESULT CApplication::OleLockApplication(Boolean fLock, Boolean fLastUnlockReleases)
{
	HRESULT		hrErr;

	AddRef();			// artificial AddRef to make object stable

	hrErr = CoLockObjectExternal(this, fLock, fLastUnlockReleases);
	ASSERT(hrErr == NOERROR);

	Release();			// release artificial AddRef above;

	return hrErr;
}


void CApplication::OleAppDocLock(void)
{
	m_cDoc++;

	OleLockApplication(true, false);
}


void CApplication::OleAppDocUnlock(void)
{
	ASSERTCOND(m_cDoc > 0);

	m_cDoc--;

	OleLockApplication(false, true);
}


void CApplication::EventLoop(void)
{
	EventRecord		event;

	while (!ReadyToQuit())
	{
		WaitNextEvent(everyEvent, &event, 0, NULL);
		HandleEvent(event);
	}
}


void CApplication::AdjustMenus(void)
{
	MenuHandle	theMenu;

	theMenu = GetMenu(mFile);
	EnDisItem(theMenu, iClose, (m_document != NULL));

	theMenu = GetMenu(mEdit);
	DisableItem(theMenu, -1);

	if (m_document)
		EnDisItem(theMenu, iDelete, (m_document->m_Shapes != NULL));
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
			if (theCWindow->m_fFloating || theCWindow == (CWindow *)m_document)
				theCWindow->HandleContentClick(event.where);
			else
				SelectTheWindow(theWindow);
			break;

		case inGoAway:
			if (TrackGoAway(theWindow, event.where))
				theCWindow->HandleClose();
			break;

		case inGrow:
			theCWindow->HandleGrow(event.where);
			break;

		case inZoomIn:
		case inZoomOut:
			if (TrackBox(theWindow, event.where, thePart))
				theCWindow->HandleZoom(thePart);
			break;

		case inDrag:
			theCWindow->HandleDrag(event);
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


void CApplication::HandleKeyDown(EventRecord event)
{
	long	lResult;

	if (event.modifiers & cmdKey)
	{
		AdjustMenus();
		lResult = MenuKey((short) event.message & charCodeMask);
		if (HiWord(lResult))
		{
			InitCursor();
			HandleMenuCommand(lResult);
		}
	}
	else
		if (m_document)
			m_document->HandleKeyDown(event);
}


void CApplication::HandleActivate(EventRecord event)
{
	CDocument *		theCDocument;
	Boolean			fActivating = event.modifiers && activeFlag;

	theCDocument = (CDocument *)GetWRefCon((WindowPtr)event.message);
	ASSERT(theCDocument != NULL);

	theCDocument->HandleActivate(fActivating);
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

		ShowHide(m_toolbar->m_pWindow, m_fInForeground);

		if (m_document)
			m_document->HiliteWindow(m_fInForeground);
	}
}


void CApplication::HandleMenuCommand(long menuResult)
{
	short		menuID   = HiWord(menuResult),
				menuItem = LoWord(menuResult);
	Str255		daName;

	switch (menuID) {

		case mApple:		// Apple menu
			if (menuItem==1)
				Alert(kAboutAlert, NULL);
			else
			{
				GetItem(GetMHandle(mApple), menuItem, daName);
				OpenDeskAcc(daName);
			}
			break;

		case mFile:		   // File menu
			switch (menuItem)
			{
				case iNew:
					if (!m_fUserInControl)
					{
						m_fUserInControl = true;
						OleLockApplication(true, false);
					}
					CreateNewDocument(true);
					break;

				case iClose:
					m_document->HandleClose();
					break;

				case iQuit:
					DoQuit();
					break;
			}
			break;

		case mEdit:		   // Edit menu
			switch (menuItem)
			{
				case (iDelete):
					m_document->DeleteLast();
					break;
			}
			break;

		}

	HiliteMenu(0);
}


void CApplication::DoQuit(void)
{
	CloseAllDocuments();

	AddRef();		// artifical addref

	if (m_fUserInControl)
		OleLockApplication(false, true);

	Release();		// artifical release
}


Boolean CApplication::ReadyToQuit(void)
{
	return (m_cRef == 0);
}


void CApplication::CloseAllDocuments(void)
{
	while (m_document)
		m_document->HandleClose();
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

		if (TickCount() > timeout)
		{
			ASSERTSZ(false, "timed out trying to switch to the foreground");
			return;
		}

		WaitNextEvent(0, &theEvent, 3, 0);
	}
}


void CApplication::UpdateFrontDocument(void)
{
	WindowPtr	pWindow;
	CDocument *	theCDocument;

	m_document = NULL;

	if (m_toolbar)
		pWindow = (WindowPtr)((WindowPeek)m_toolbar->m_pWindow)->nextWindow;
	else
		pWindow = FrontWindow();

	if (pWindow)
	{
		theCDocument = (CDocument *)GetWRefCon(pWindow);
		ASSERT(theCDocument != NULL);
		if (m_document != theCDocument)
			SelectTheWindow(pWindow);
	}
}


void CApplication::SelectTheWindow(WindowPtr pWindow)
{
	CDocument *	theCDocument;

	theCDocument = (CDocument *)GetWRefCon(pWindow);
	ASSERT(theCDocument != NULL);

	if (theCDocument->m_fFloating)
		return;

	if (m_document && m_document != theCDocument)
	{
		LMSetCurDeactive((WindowRef)m_document->m_pWindow);
		m_document->HiliteWindow(false);
	}

	if (theCDocument != m_document)
		SendBehind(pWindow, m_toolbar->m_pWindow);

	if (!((WindowPeek)pWindow)->hilited)
	{
		LMSetCurActivate((WindowRef)theCDocument->m_pWindow);
		theCDocument->HiliteWindow(true);
	}

	m_document = theCDocument;
}


void CApplication::CreateNewDocument(Boolean fShow)
{
	CDocument *	theCDocument;

	theCDocument =  new CDocument;

	if (theCDocument == NULL)
		FailAlert("\pCould not instantiate CDocument object");

	theCDocument->InitDocument();

	if (fShow)
		theCDocument->ShowDocument();
}
