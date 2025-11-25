/*****************************************************************************\
*                                                                             *
*    OleInPlaceServerDocument.h                                               *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#ifndef __DOCSVR_H__
#define __DOCSVR_H__


#if qOleInPlace
// OLDNAME: OleOutlineInPlaceServer.h

void OleOutlineInPlaceServerDoUnknownMenuKey(struct ApplicationRec* pApp, EventRecord* pEvent);
void OleOutlineInPlaceServerDoUnknownMenuItem(struct ApplicationRec* pApp, EventRecord* pEvent, short menuID, short menuItem);
void OleOutlineInPlaceServerHideOthersCmd(struct ApplicationRec* pApp);
void OleOutlineInPlaceServerProcessEvent(struct ApplicationRec* pApp, EventRecord* pEvent);


// OLDNAME: OleInPlaceServerInterface.h
void OleInPlaceServerInitInterfaces(void);

struct OleInPlaceServerObjectImpl {
	IOleInPlaceObjectVtbl*		lpVtbl;
	struct OleDocumentRec*		lpOleDoc;
	unsigned long				cRef;
};

typedef struct OleInPlaceServerObjectImpl OleInPlaceServerObjectImpl, *OleInPlaceServerObjectImplPtr;

void OleInPlaceServerIObjectInit(OleInPlaceServerObjectImplPtr pOleInPlaceServerObjectImpl, struct OleDocumentRec* pOleDoc);

// OleInPlace::Object methods
STDMETHODIMP IOleInPlaceServerObjectQueryInterface(LPOLEINPLACEOBJECT lpThis, REFIID riid, void * * ppvObj);
STDMETHODIMP_(unsigned long) IOleInPlaceServerObjectAddRef(LPOLEINPLACEOBJECT lpThis);
STDMETHODIMP_(unsigned long) IOleInPlaceServerObjectRelease(LPOLEINPLACEOBJECT lpThis);
STDMETHODIMP IOleInPlaceServerObjectGetWindow(LPOLEINPLACEOBJECT lpThis, WindowPtr * lphwnd);
STDMETHODIMP IOleInPlaceServerObjectContextSensitiveHelp(LPOLEINPLACEOBJECT lpThis, unsigned long fEnterMode);
STDMETHODIMP IOleInPlaceServerObjectInPlaceDeactivate(LPOLEINPLACEOBJECT lpThis);
STDMETHODIMP IOleInPlaceServerObjectUIDeactivate(LPOLEINPLACEOBJECT lpThis);
STDMETHODIMP IOleInPlaceServerObjectSetObjectRects(LPOLEINPLACEOBJECT lpThis,
					LPCRECT lprcPosRect,
					RgnHandle clipRgn,
					RgnHandle frameRgn,
					RgnHandle cliRgn
					);
STDMETHODIMP IOleInPlaceServerObjectReactivateAndUndo(LPOLEINPLACEOBJECT lpThis);

struct OleInPlaceServerActiveObjectImpl {
	IOleInPlaceActiveObjectVtbl*	lpVtbl;
	struct OleDocumentRec*			lpOleDoc;
	unsigned long					cRef;
};

typedef struct OleInPlaceServerActiveObjectImpl OleInPlaceServerActiveObjectImpl, *OleInPlaceServerActiveObjectImplPtr;

void OleInPlaceServerIActiveObjectInit(OleInPlaceServerActiveObjectImplPtr pOleInPlaceServerActiveObjectImpl, struct OleDocumentRec* pOleDoc);

// OleInPlace::ActiveObject methods
STDMETHODIMP IOleInPlaceServerActiveObjectQueryInterface(LPOLEINPLACEACTIVEOBJECT lpThis, REFIID riid, void * * ppvObj);
STDMETHODIMP_(unsigned long) IOleInPlaceServerActiveObjectAddRef(LPOLEINPLACEACTIVEOBJECT lpThis);
STDMETHODIMP_(unsigned long) IOleInPlaceServerActiveObjectRelease(LPOLEINPLACEACTIVEOBJECT lpThis);
STDMETHODIMP IOleInPlaceServerActiveObjectGetWindow(LPOLEINPLACEACTIVEOBJECT lpThis, WindowPtr * lphwnd);
STDMETHODIMP IOleInPlaceServerActiveObjectContextSensitiveHelp(LPOLEINPLACEACTIVEOBJECT lpThis, unsigned long fEnterMode);
STDMETHODIMP IOleInPlaceServerActiveObjectTranslateAccelerator(LPOLEINPLACEACTIVEOBJECT lpThis, EventRecord * lpmsg);
STDMETHODIMP IOleInPlaceServerActiveObjectOnFrameWindowActivate(LPOLEINPLACEACTIVEOBJECT lpThis, unsigned long fActivate);
STDMETHODIMP IOleInPlaceServerActiveObjectOnDocWindowActivate(LPOLEINPLACEACTIVEOBJECT lpThis, unsigned long fActivate);
STDMETHODIMP IOleInPlaceServerActiveObjectResizeBorder(LPOLEINPLACEACTIVEOBJECT lpThis, LPCRECT lprectBorder, LPOLEINPLACEUIWINDOW lpUIWindow, unsigned long fFrameWindow);
STDMETHODIMP IOleInPlaceServerActiveObjectEnableModeless(LPOLEINPLACEACTIVEOBJECT lpThis, unsigned long fEnable);




// OLDNAME: OleInPlaceServerDocument.h
#define BEGIN_IP_OnIPActivate			0
#define BEGIN_IP_OnUIActivate			1
#define BEGIN_IP_OnDocActivate			2

#define END_IP_OnIPDeactivate			0
#define END_IP_OnUIDeactivate			1
#define END_IP_OnDocDeactivate			2
#define END_IP_Failure					3

struct OleInPlaceServerDocRec {
	Boolean								m_fIPActive;
	Boolean								m_fUIActive;
	Boolean								m_fUIVisible;

	Boolean								m_fKillInPlace;
	Boolean								m_fInvalidate;

	OleMBarHandle						m_hOleMBar;
	Handle								m_hSavedMBar;

	WindowPtr							m_pWindow;
	WindowPtr							m_pWindowParent;

	Point								m_ptLastSize;
	Point								m_ptExtent;

	LPOLEINPLACESITE					m_pSite;
	LPOLEINPLACEFRAME					m_pFrame;
	LPOLEINPLACEUIWINDOW				m_pUIWindow;

	OLEINPLACEFRAMEINFO					m_FrameInfo;

	RgnHandle							m_hPosRgn;
	RgnHandle							m_hClipRgn;
	RgnHandle							m_hFrameRgn;
	RgnHandle							m_hCliRgn;

	OleInPlaceServerObjectImpl			m_Object;
	OleInPlaceServerActiveObjectImpl	m_ActiveObject;
};

typedef struct OleInPlaceServerDocRec OleInPlaceServerDocRec, *OleInPlaceServerDocPtr;

void OleInPlaceServerDocInit(struct OleDocumentRec* pOleDoc);
void OleInPlaceServerDocDispose(struct OleDocumentRec* pOleDoc);

HRESULT OleInPlaceServerDocLocalQueryInterface(struct OleDocumentRec* pOleDoc, REFIID riid, void* * lplpvObj);

Boolean OleInPlaceServerDocIsInPlace(struct OleDocumentRec* pOleDoc);
Boolean OleInPlaceServerDocIsUIActive(struct OleDocumentRec* pOleDoc);
Boolean OleInPlaceServerDocIsUIVisible(struct OleDocumentRec* pOleDoc);

HRESULT OleInPlaceServerDocDoInPlaceEdit(struct OleDocumentRec* pOleDoc, long lVerb, EventRecord* lpmsg, LPOLECLIENTSITE lpActiveSite, WindowPtr hwndParent, LPCRECT lprcPosRect);

HRESULT OleInPlaceServerDocBeginInPlace(struct OleDocumentRec* pOleDoc);
HRESULT OleInPlaceServerDocEndInPlace(struct OleDocumentRec* pOleDoc, short wWhy);

HRESULT OleInPlaceServerDocIPActivate(struct OleDocumentRec* pOleDoc, LPCRECT lprcPosRect);
HRESULT OleInPlaceServerDocIPDeactivate(struct OleDocumentRec* pOleDoc, short wWhy);
HRESULT OleInPlaceServerDocUIActivate(struct OleDocumentRec* pOleDoc, short wWhy);
HRESULT OleInPlaceServerDocUIDeactivate(struct OleDocumentRec* pOleDoc, short wWhy);
void OleInPlaceServerDocShow(struct OleDocumentRec* pOleDoc, short wWhy);
void OleInPlaceServerDocHide(struct OleDocumentRec* pOleDoc, short wWhy);

HRESULT OleInPlaceServerDocPositionWindow(struct OleDocumentRec* pOleDoc, Boolean fGetPosition);
HRESULT OleInPlaceServerDocSetInPlaceWindow(struct OleDocumentRec* pOleDoc);
HRESULT OleInPlaceServerDocSetActiveObject(struct OleDocumentRec* pOleDoc);
HRESULT OleInPlaceServerDocNegotiateFrameSpace(struct OleDocumentRec* pOleDoc);

void OleInPlaceServerDocDoActivate(struct OleDocumentRec* pOleDoc, Boolean becomingActive);

void OleInPlaceServerDocDoKeyDown(struct OleDocumentRec* pOleDoc, EventRecord* pEvent);
void OleInPlaceServerDocUpdateMenus(struct OleDocumentRec* pOleDoc);
void OleInPlaceServerDocDoMenuKey(struct OleDocumentRec* pOleDoc, EventRecord* pEvent);
void OleInPlaceServerDocDoMenu(struct OleDocumentRec* pOleDoc, EventRecord* pEvent, short menuID, short menuItem);
void OleInPlaceServerDocHideOthersCmd(struct OleDocumentRec* pOleDoc);

HRESULT OleInPlaceServerDocEnableModeless(struct OleDocumentRec* pOleDoc, Boolean fEnable);
HRESULT OleInPlaceServerDocOnDocWindowActivate(struct OleDocumentRec* pOleDoc, Boolean fActivate);

Boolean OleInPlaceServerDocProcessEvent(struct OleDocumentRec* pOleDoc, EventRecord* pEvent);
Boolean OleInPlaceServerDocSetDefaultCursor(struct OleDocumentRec* pOleDoc);
void OleInPlaceServerDocDoIdle(struct OleDocumentRec* pOleDoc);

void OleInPlaceServerDocSetExtent(struct OleDocumentRec* pOleDoc, LPSIZEL lpsizel);
void OleInPlaceServerDocUpdateWindowPosition(struct OleDocumentRec* pOleDoc);
HRESULT OleInPlaceServerDocSetObjectRects(struct OleDocumentRec* pOleDoc,
					LPCRECT prcPosRect,
					RgnHandle clipRgn,
					RgnHandle frameRgn,
					RgnHandle cliRgn);

#endif


// OLDNAME: OleServerDocInterface.h
void OleServerDocInitInterfaces(void);

struct ServerDocOleObjectImpl {
	IOleObjectVtbl*					lpVtbl;
	struct OleDocumentRec*			lpOleDoc;
	unsigned long							cRef;			// interface specific ref count
};

typedef struct ServerDocOleObjectImpl ServerDocOleObjectImpl, *ServerDocOleObjectImplPtr;

void ServerDocIOleObjectInit(ServerDocOleObjectImplPtr pServerDocOleObjectImpl, struct OleDocumentRec* pOleDoc);

// ServerDoc::IOleObject methods
STDMETHODIMP IServerDocOleObjectQueryInterface(LPOLEOBJECT lpThis, REFIID riid, void* * lplpvObj);
STDMETHODIMP_(unsigned long) IServerDocOleObjectAddRef(LPOLEOBJECT lpThis);
STDMETHODIMP_(unsigned long) IServerDocOleObjectRelease(LPOLEOBJECT lpThis);
STDMETHODIMP IServerDocOleObjectSetClientSite(LPOLEOBJECT lpThis, LPOLECLIENTSITE lpclientSite);
STDMETHODIMP IServerDocOleObjectGetClientSite(LPOLEOBJECT lpThis, LPOLECLIENTSITE * lplpClientSite);
STDMETHODIMP IServerDocOleObjectSetHostNames(LPOLEOBJECT lpThis, const char* szContainerApp, const char* szContainerObj);
STDMETHODIMP IServerDocOleObjectClose(LPOLEOBJECT lpThis, unsigned long dwSaveOption);
STDMETHODIMP IServerDocOleObjectSetMoniker(LPOLEOBJECT lpThis, unsigned long dwWhichMoniker, LPMONIKER lpmk);
STDMETHODIMP IServerDocOleObjectGetMoniker(LPOLEOBJECT lpThis, unsigned long dwAssign, unsigned long dwWhichMoniker, LPMONIKER * lplpmk);
STDMETHODIMP IServerDocOleObjectInitFromData(LPOLEOBJECT lpThis, LPDATAOBJECT lpDataObject, unsigned long fCreation, unsigned long reserved);
STDMETHODIMP IServerDocOleObjectGetClipboardData(LPOLEOBJECT lpThis, unsigned long reserved, LPDATAOBJECT * lplpDataObject);
STDMETHODIMP IServerDocOleObjectDoVerb(LPOLEOBJECT lpThis, long lVerb, EventRecord* lpmsg, LPOLECLIENTSITE lpActiveSite, long lindex, WindowPtr hwndParent, LPCRECT lprcPosRect);
STDMETHODIMP IServerDocOleObjectEnumVerbs(LPOLEOBJECT lpThis, LPENUMOLEVERB * lplpenumOleVerb);
STDMETHODIMP IServerDocOleObjectUpdate(LPOLEOBJECT lpThis);
STDMETHODIMP IServerDocOleObjectIsUpToDate(LPOLEOBJECT lpThis);
STDMETHODIMP IServerDocOleObjectGetUserClassID(LPOLEOBJECT lpThis, LPCLSID lpclsid);
STDMETHODIMP IServerDocOleObjectGetUserType(LPOLEOBJECT lpThis, unsigned long dwFormOfType, char* * lpszUserType);
STDMETHODIMP IServerDocOleObjectSetExtent(LPOLEOBJECT lpThis, unsigned long dwDrawAspect, LPSIZEL lpsizel);
STDMETHODIMP IServerDocOleObjectGetExtent(LPOLEOBJECT lpThis, unsigned long dwDrawAspect, LPSIZEL lpsizel);
STDMETHODIMP IServerDocOleObjectAdvise(LPOLEOBJECT lpThis, LPADVISESINK lpAdvSink, unsigned long* lpdwConnection);
STDMETHODIMP IServerDocOleObjectUnadvise(LPOLEOBJECT lpThis, unsigned long dwConnection);
STDMETHODIMP IServerDocOleObjectEnumAdvise(LPOLEOBJECT lpThis, LPENUMSTATDATA * lplpenumAdvise);
STDMETHODIMP IServerDocOleObjectGetMiscStatus(LPOLEOBJECT lpThis, unsigned long dwAspect, unsigned long * lpdwStatus);
STDMETHODIMP IServerDocOleObjectSetColorScheme(LPOLEOBJECT lpThis, LPOLECOLORSCHEME lpLogpal);
STDMETHODIMP IServerDocOleObjectLockObject(LPOLEOBJECT lpThis, unsigned long fLock);

struct ServerDocPersistStorageImpl {
	IPersistStorageVtbl*			lpVtbl;
	struct OleDocumentRec*			lpOleDoc;
	unsigned long							cRef;			// interface specific ref count
};

typedef struct ServerDocPersistStorageImpl ServerDocPersistStorageImpl, *ServerDocPersistStorageImplPtr;

void ServerDocIPersistStorageInit(ServerDocPersistStorageImplPtr pServerDocPersistStorageImpl, struct OleDocumentRec* pOleDoc);

// ServerDoc::IPersistStorage methods
STDMETHODIMP IServerDocPersistStorageQueryInterface(LPPERSISTSTORAGE lpThis, REFIID riid, void* * lplpvObj);
STDMETHODIMP_(unsigned long) IServerDocPersistStorageAddRef(LPPERSISTSTORAGE lpThis);
STDMETHODIMP_(unsigned long) IServerDocPersistStorageRelease(LPPERSISTSTORAGE lpThis);
STDMETHODIMP IServerDocPersistStorageGetClassID(LPPERSISTSTORAGE lpThis, LPCLSID lpClassID);
STDMETHODIMP IServerDocPersistStorageIsDirty(LPPERSISTSTORAGE lpThis);
STDMETHODIMP IServerDocPersistStorageInitNew(LPPERSISTSTORAGE lpThis, LPSTORAGE lpStg);
STDMETHODIMP IServerDocPersistStorageLoad(LPPERSISTSTORAGE lpThis, LPSTORAGE lpStg);
STDMETHODIMP IServerDocPersistStorageSave(LPPERSISTSTORAGE lpThis, LPSTORAGE lpStg, unsigned long fSameAsLoad);
STDMETHODIMP IServerDocPersistStorageSaveCompleted(LPPERSISTSTORAGE lpThis, LPSTORAGE lpStgNew);
STDMETHODIMP IServerDocPersistStorageHandsOffStorage(LPPERSISTSTORAGE lpThis);

struct ServerDocStdMarshalInfoImpl {
	IStdMarshalInfoVtbl*			lpVtbl;
	struct OleDocumentRec*			lpOleDoc;
	unsigned long							cRef;			// interface specific ref count
};

typedef struct ServerDocStdMarshalInfoImpl ServerDocStdMarshalInfoImpl, *ServerDocStdMarshalInfoImplPtr;

void ServerDocIStdMarshalInfoInit(ServerDocStdMarshalInfoImplPtr pServerDocStdMarshalInfoImpl, struct OleDocumentRec* pOleDoc);

#if qOleTreatAs
// ServerDoc::IStdMarshalInfo methods
STDMETHODIMP IServerDocStdMarshalInfoQueryInterface(LPSTDMARSHALINFO lpThis, REFIID riid, void* * lplpvObj);
STDMETHODIMP_(unsigned long) IServerDocStdMarshalInfoAddRef(LPSTDMARSHALINFO lpThis);
STDMETHODIMP_(unsigned long) IServerDocStdMarshalInfoRelease(LPSTDMARSHALINFO lpThis);
STDMETHODIMP IServerDocStdMarshalInfoGetClassForHandler(LPSTDMARSHALINFO lpThis, unsigned long dwDestContext, void* pvDestContext, LPCLSID lpClassID);
#endif

// OLDNAME: OleServerDocument.h

// Maximum length of strings passed through IOleObject::SetHostNames
#define kMaxObjectNameLen		256
#define kMaxAppNameLen			32
#define kMaxContainerNameLen	32
#define kMaxMenuItemLen			33

struct OleServerDocRec {

	ServerDocOleObjectImpl		m_OleObject;

	ServerDocPersistStorageImpl	m_PersistStorage;

#if qOleTreatAs
	ServerDocStdMarshalInfoImpl m_StdMarshalInfo;
#endif // qOleTreatAs

	unsigned long				m_cPseudoObj;

	LPOLECLIENTSITE				m_lpOleClientSite;						// Client associated with the obj
	LPOLEADVISEHOLDER			m_lpOleAdviseHolder;					// helper obj to hold ole advises
	LPDATAADVISEHOLDER			m_lpDataAdviseHolder;					// helper obj to hold data advises
	Boolean						m_fEnableDraw;							// enable/disable notifications
	Boolean						m_fSendDataOnStop;						// did data ever change?
	STGMODE						m_dwStorageMode;						// indicates state during save
	Boolean						m_fSaveWithSameAsLoad;					// was IPS::Save called with fSameAsLoad==TRUE
	Boolean						m_fNoScribbleMode;						// was IPS::Save called
	LPMONIKER					m_lpSrcMonikerOfCopy;					// temp moniker for src of copy

	char						m_szObjectName[kMaxObjectNameLen];		

	char						m_szContainerApp[kMaxAppNameLen];
	char						m_szContainerObj[kMaxContainerNameLen];
	
	Boolean						m_DataChanged;
	Boolean						m_SizeChanged;
	
	Boolean						m_fSetFileMenu;
	unsigned char				m_sOldCloseMenuItem[kMaxMenuItemLen];	// ole close menu item string
	unsigned char				m_sOldSaveMenuItem[kMaxMenuItemLen];	// old save menu item string
	unsigned char				m_sOldSaveAsMenuItem[kMaxMenuItemLen];	// old save as menu item string
	unsigned char				m_sOldQuitMenuItem[kMaxMenuItemLen];	// old quit menu item string
	
#if qOleInPlace
	OleInPlaceServerDocRec		inplace;
#endif
};

typedef struct OleServerDocRec OleServerDocRec, *OleServerDocPtr;

void OleServerDocInit(struct OleDocumentRec* pOleDoc);
void OleServerDocDispose(struct OleDocumentRec* pOleDoc);

void OleServerDocShow(struct OleDocumentRec* pOleDoc);
void OleServerDocHide(struct OleDocumentRec* pOleDoc);
void OleServerDocDoActivate(struct OleDocumentRec* pOleDoc, Boolean becomingActive);

void OleServerDocDoSave(struct OleDocumentRec* pOleDoc);
void OleServerDocDoClose(struct OleDocumentRec* pOleDoc, Boolean askUserToSave, YNCResult defaultAnswer, Boolean quitting);

void OleServerDocDoIdle(struct OleDocumentRec* pOleDoc);

void OleServerDocSetDirty(struct OleDocumentRec* pOleDoc, Boolean fDirty);

void OleServerDocSetDataChanged(struct OleDocumentRec* pOleDoc, Boolean fDataChanged);
void OleServerDocSetSizeChanged(struct OleDocumentRec* pOleDoc, Boolean fSizeChanged);

HRESULT OleServerDocInitNewEmbedded(struct OleDocumentRec* pOleDoc);
void OleServerDocSendAdvise(struct OleDocumentRec* pOleDoc, OLE_NOTIFICATION advise, LPMONIKER lpmkDoc, unsigned long dwAdvf);
void OleServerDocPseudoObjLockDoc(struct OleDocumentRec* pOleDoc);
void OleServerDocPseudoObjUnlockDoc(struct OleDocumentRec* pOleDoc);

HRESULT OleServerDocLocalQueryInterface(struct OleDocumentRec* pOleDoc, REFIID riid, void* * lplpvObj);

HRESULT OleServerDocOleObjectDoVerb(struct OleDocumentRec* pOleDoc, long lVerb, EventRecord* lpmsg, LPOLECLIENTSITE lpActiveSite, long lindex, WindowPtr hwndParent, LPCRECT lprcPosRect);

HRESULT OleServerDocDataObjectGetData(struct OleDocumentRec* pOleDoc, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium);
HRESULT OleServerDocDataObjectQueryGetData(struct OleDocumentRec* pOleDoc, LPFORMATETC lpFormatetc);
HRESULT OleServerDocDataObjectAdvise(struct OleDocumentRec* pOleDoc, FORMATETC * lpFormatetc, unsigned long advf, LPADVISESINK lpAdvSink, unsigned long * lpdwConnection);
HRESULT OleServerDocDataObjectUnadvise(struct OleDocumentRec* pOleDoc, unsigned long dwConnection);

void OleServerDocRenamedUpdate(struct OleDocumentRec* pOleDoc, LPMONIKER lpmkDoc);
void OleServerDocPasteFormatFromData(
        struct OleDocumentRec*  pOleDoc,
        ResType              	cfFormat,
        LPDATAOBJECT            pSrcDataObj,
        Boolean                 fLink
);
void OleServerDocDoCopyRange(struct OleDocumentRec* pOleDoc);

// OLDNAME: OleOutlineServerDoc.h
struct OleOutlineServerDocRec {
	
#if qOleTreatAs
	CLSID							m_clsidTreatAs;		// clsid to pretend to be
	char*			    			m_pszTreatAsType;	// user type name to pretend to be
#endif	// qOleTreatAs

	short		__dummy__;		// make compiler happy
};

typedef struct OleOutlineServerDocRec OleOutlineServerDocRec, *OleOutlineServerDocPtr;

void OleOutlineServerDocInit(struct OleOutlineDocRec* pOleOutlineDoc);

HRESULT OleOutlineServerDocGetItemObject(struct OleOutlineDocRec* pOleOutlineDoc, char* pszItem, unsigned long SpeedNeeded, LPBINDCTX pbc, REFIID riid, void** ppvObject);
HRESULT OleOutlineServerDocIsItemRunning(struct OleOutlineDocRec* pOleOutlineDoc, char* pszItem);
LPMONIKER OleOutlineServerDocGetRangeRelMoniker(
        struct OleOutlineDocRec*        pOleOutlineDoc,
        void*					pRange,
        unsigned long           dwAssign
);
LPMONIKER OleOutlineServerDocGetRangeFullMoniker(
        struct OleOutlineDocRec*        pOleOutlineDoc,
        void*					pRange,
        unsigned long           dwAssign
);


#endif
