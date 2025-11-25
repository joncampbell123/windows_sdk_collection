#ifndef _CAPPLICATION_H_
#define _CAPPLICATION_H_

class CDocument;
class CToolbar;
class CClassFactory;




#define kSimpleServerType					'SiSe'
#define kSimpleServerTypeStr				"SiSe"
#define kSimpleServerFullUserTypeName		"Simple OLE 2.0 Server Document"
#define kSimpleServerShortUserTypeName		"Simple Simple Document"
#define kSimpleServerAppName				"OLE 2.0 Simple Server"
#define kSimpleServerProgID					"SimpSrvr"
#define kSimpleServerCLSID					"{00000410-0000-0000-C000-000000000046}"
#define kSimpleServerDocumentFileType		'Smpl'
#define kSimpleServerDocumentFileTypeStr	"Smpl"


#define kExtraStackSpace	8096
#define	kExtraMasters		   2

#define	kMenuBar			 128

#define kAboutAlert			 129

#define	kCrosshairCursor	 128

#define	mObject				 100

#define mApple				 200
#define 	iAbout		 	   1

#define mFile			 	 201
#define 	iNew		 	   1
#define		iClose			   3
#define 	iQuit			   5

#define mEdit				 202
#define 	iUndo			   1
#define		iCut			   3
#define 	iCopy			   4
#define		iPaste			   5
#define 	iClear		       6
#define 	iSelectAll		   7
#define		iDelete			   9

#define 	MIN_WINDOW_X	 100
#define 	MIN_WINDOW_Y	 100


class CApplication : public IUnknown
{
public:

	unsigned long		m_cRef;
	unsigned long		m_cDoc;
	CDocument 			*m_document;
	CToolbar  			*m_toolbar;
	Boolean				m_fDone;
	Boolean				m_fInForeground;
	Boolean				m_fUserInControl;
	short				m_nWndCount;
	Cursor				m_crosshair;

	CClassFactory 		*m_ClassFactory;
	unsigned long		m_CoRegisterResult;
	LPMESSAGEFILTER		m_pIMessageFilter;

    // *** IUnknown methods ***
    virtual HRESULT STDMETHODCALLTYPE	QueryInterface(REFIID riid, void * * ppvObj);
    virtual unsigned long STDMETHODCALLTYPE		AddRef(void);
    virtual unsigned long STDMETHODCALLTYPE		Release(void);

	CApplication(void);
	~CApplication(void);

	void			InitApplication		(void);
	void			InitToolbox			(void);
	void 			SetupMenus			(void);

	void			DisposeApplication	(void);

	void			OleInitApplication	(void);
	void 			OleUpdateRegDB		(void);
	void			RegisterClassFactory(void);
	void			OleCleanup			(void);

	HRESULT			OleLockApplication	(Boolean fLock, Boolean fLastUnlockReleases);
	void			OleAppDocLock		(void);
	void			OleAppDocUnlock		(void);

	void 			EventLoop			(void);
	void 			AdjustMenus			(void);

	void 			HandleEvent			(EventRecord event);
	void			HandleMouseDown		(EventRecord event);
	void			HandleKeyDown		(EventRecord event);
	void			HandleActivate		(EventRecord event);
	void			HandleUpdate		(EventRecord event);
	void			HandleDiskEvent		(EventRecord event);
	void 			HandleOSEvent		(EventRecord event);
	void			HandleNullEvent		(void);
	void 			HandleMenuCommand	(long cmd);

	void			DoQuit				(void);
	Boolean			ReadyToQuit			(void);
	void 			CloseAllDocuments	(void);
	void			BringToForeground	(Boolean fWait);

	void			UpdateFrontDocument	(void);
	void			SelectTheWindow		(WindowPtr pWindow);

	void 			CreateNewDocument	(Boolean fShow);
};

extern CApplication *	gApp;
extern char				gAppName[];

#endif // _CAPPLICATION_H_