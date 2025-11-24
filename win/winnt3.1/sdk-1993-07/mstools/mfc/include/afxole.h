// Microsoft Foundation Classes C++ library. 
// Copyright (C) 1992 Microsoft Corporation, 
// All rights reserved. 

// This source code is only intended as a supplement to the 
// Microsoft Foundation Classes Reference and Microsoft 
// QuickHelp documentation provided with the library. 
// See these sources for detailed information regarding the 
// Microsoft Foundation Classes product. 

 
#ifndef __AFXOLE_H__
#define __AFXOLE_H__

/////////////////////////////////////////////////////////////////////////////
// Classes declared in this file

	//CException
		class COleException;    // caught by client

// Client side of linking and embedding
	class COleClientItem;       // embedded ole object
	class COleClientDoc;        // a document containing COleClientItems

// Server (/provider) side of linking and embedding
	class COleServerItem;       // embedded ole object
	class COleServerDoc;        // server document
	class COleServer;           // server application

/////////////////////////////////////////////////////////////////////////////
// Make sure 'afxwin.h' is included first
#ifndef __AFXWIN_H__
#include "afxwin.h"
#endif

#define SERVERONLY  /* use smaller server version of OLEOBJECT VTable */
#include "ole.h"

#define OLE_MAXNAMESIZE     256         /* 255 real characters */

/////////////////////////////////////////////////////////////////////////////
// COleException - something going wrong

class COleException : public CException
{
	DECLARE_DYNAMIC(COleException)
public:
	OLESTATUS   m_status;
	COleException(OLESTATUS status);

	static  OLESTATUS   Process(CException*);   // helper
};

void    AfxThrowOleException(OLESTATUS status);

/////////////////////////////////////////////////////////////////////////////
// User interface helper functions

void AfxOleSetEditMenu(COleClientItem* pClient, CMenu* pMenu,
	UINT iMenuItem, UINT nIDVerbMin);
BOOL AfxOleInsertDialog(CString& name);
BOOL AfxOleLinksDialog(COleClientDoc* pDoc);

//////////////////////////////////////////////////////////////////////////////
// COleClientItem - Client view of an OLEOBJECT + OLECLIENT for callbacks

class COleClientItem : public CObject
{
// Implementation
protected:
	OLECLIENT   m_oleClient;        // must be first member variable
public: // in case you want direct access
	LPOLEOBJECT m_lpObject;
protected:
	OLESTATUS   m_lastStatus;
	COleClientDoc* m_pDocument;

	DECLARE_DYNAMIC(COleClientItem)

// Constructors
public:
	COleClientItem(COleClientDoc* pContainerDoc);
	~COleClientItem();

	// create from the clipboard
	BOOL    CreateFromClipboard(LPCSTR lpszItemName,
				OLEOPT_RENDER renderopt = olerender_draw,
				OLECLIPFORMAT cfFormat = 0);
	BOOL    CreateStaticFromClipboard(LPCSTR lpszItemName,
				OLEOPT_RENDER renderopt = olerender_draw,
				OLECLIPFORMAT cfFormat = 0);
	BOOL    CreateLinkFromClipboard(LPCSTR lpszItemName,
				OLEOPT_RENDER renderopt = olerender_draw,
				OLECLIPFORMAT cfFormat = 0);

	// create from a protocol name (Insert New Object dialog)
	BOOL    CreateNewObject(LPCSTR lpszClass, LPCSTR lpszItemName,
				OLEOPT_RENDER renderopt = olerender_draw,
				OLECLIPFORMAT cfFormat = 0);
	// special create for invisible
	BOOL    CreateInvisibleObject(LPCSTR lpszClass, LPCSTR lpszItemName,
				OLEOPT_RENDER renderopt = olerender_draw,
				OLECLIPFORMAT cfFormat = 0, BOOL bActivate = FALSE);

	// create a copy
	BOOL    CreateCloneFrom(COleClientItem* pObject, LPCSTR lpszItemName);

// General Attributes
	OLESTATUS GetLastStatus() const;
	UINT    GetType();  // OT_LINK, OT_EMBEDDED or OT_STATIC
	CString GetName();

	BOOL    GetSize(LPPOINT lpSize);    // return FALSE if BLANK
	BOOL    GetBounds(LPRECT lpBounds); // return FALSE if BLANK

	BOOL    IsOpen();                   // currently open on server side

	// Data access
	OLECLIPFORMAT EnumFormats(OLECLIPFORMAT nFormat) const;
	HANDLE  GetData(OLECLIPFORMAT nFormat, BOOL& bMustDelete);
	void    SetData(OLECLIPFORMAT nFormat, HANDLE hData);
	void    RequestData(OLECLIPFORMAT nFormat);

	// Other rare access information
	BOOL    IsEqual(COleClientItem* pObject);
	HANDLE  GetLinkFormatData();        // internal use
	COleClientDoc* GetDocument() const; // return container

	// global state - if anyone waiting for release => not normal operations
	static BOOL InWaitForRelease();

	// Helpers for checking clipboard data availability
	static BOOL CanPaste(OLEOPT_RENDER renderopt = olerender_draw,
				OLECLIPFORMAT cfFormat = 0);
	static BOOL CanPasteLink(OLEOPT_RENDER renderopt = olerender_draw,
				OLECLIPFORMAT cfFormat = 0);

// Attributes that apply to Linked Objects only

	// Link options are rarely changed (except through Links dialog)
	OLEOPT_UPDATE GetLinkUpdateOptions();
	void    SetLinkUpdateOptions(OLEOPT_UPDATE updateOpt);

// General Operations
	// Clean up
	void    Release();              // detach (close if needed)
	void    Delete();               // get rid of it then detach

	// Drawing
	BOOL    Draw(CDC *pDC, LPRECT lpBounds, LPRECT lpWBounds,
				CDC* pFormatDC);

	// Activation
	void    Activate(UINT nVerb, BOOL bShow = TRUE, BOOL bTakeFocus = TRUE,
				CWnd * pWndContainer = NULL, LPRECT lpBounds = NULL);

	// more advanced operations
	void    Rename(LPCSTR lpszNewname);    // call to rename item
	void    CopyToClipboard();
	void    SetTargetDevice(HANDLE hData);
			// handle to an OLETARGETDEVICE

// Operations that apply to Embedded Objects only
	void    SetHostNames(LPCSTR lpszHost, LPCSTR lpszHostObj);
	void    SetBounds(LPRECT lpRect);
	void    SetColorScheme(LPLOGPALETTE lpLogPalette);

// Operations that apply to Linked Objects only
	void    UpdateLink();               // make up-to-date
	void    CloseLink();                // close connection
										// can be used for embedded (rare)
	void    ReconnectLink();            // reactivate connection

	virtual BOOL FreezeLink(LPCSTR lpszFrozenName);
				// link -> embedded


// Overridables (notifications of OLECLIENT)
protected:
	// standard notifications from the server telling you about
	//    changes to the item

	// notifications you must implement
	virtual void OnChange(OLE_NOTIFICATION wNotification) = 0;
			// Change due to link update, document save or document close

	// notifications you can re-implement (not necessary)
	virtual void OnRenamed();           // document has been renamed
	virtual void OnRelease();

	// low level notification hook from the OLE DLLs
	virtual int ClientCallBack(OLE_NOTIFICATION wNotification);

// Overridables for User Interface and Error traps
protected:
	// deciding what are exceptions and what are ok for creation
	virtual BOOL CheckCreate(OLESTATUS status);
	virtual void CheckAsync(OLESTATUS status);
	virtual void CheckGeneral(OLESTATUS status);

	virtual void WaitForServer();
public:
	virtual BOOL ReportError(OLESTATUS status);

// Implementation
#ifdef _DEBUG
public:
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	static int cWaitForRelease;
public:
	virtual void Serialize(CArchive& ar);   // from CObject for storing only
	static COleClientItem* FromLp(LPOLECLIENT lpClient);

	friend struct _afxOleClientItemImplementation;
	friend class COleClientDoc;
};


//////////////////////////////////////////////////////////////////////////////
// COleClientDoc - registered client document

class COleClientDoc : public CObject
{
	DECLARE_DYNAMIC(COleClientDoc)

// Constructors and Destructors
public:
	COleClientDoc();
	~COleClientDoc();

	BOOL    Register(LPCSTR lpszClass, LPCSTR lpszDoc);
	void    Revoke();           // called by destructor

// Attributes
	BOOL    IsOpen() const;     // TRUE if successfully registered

// Operations (notify the global registry)
	void    NotifyRename(LPCSTR lpszNewName);
								// call this after document is renamed
	void    NotifyRevert();     // call this after document reverts to original
	void    NotifySaved();      // call this after document is saved

// Overridables you must implement for yourself

	// iteration
	POSITION GetStartPosition() const
				{ return NULL; }    // start of iteration
	virtual COleClientItem* GetNextItem(POSITION& rPosition,
		BOOL* pIsSelected) = 0;
			// return next item, update BOOL according to selection

// Overridables you do not have to implement
protected:
	virtual void CheckGeneral(OLESTATUS status) const;

// Implementation
public:
	LHCLIENTDOC m_lhClientDoc;

#ifdef _DEBUG
public:
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// COleServerItem - Server view of an OLEOBJECT + OLECLIENT


class COleServerItem : public CObject
{
protected:
	// NOTE: EVERYTHING in this class is protected - since everything
	//   in this class is designed for implementing an OLE server.
	// Requests will come from OLE Clients through non-C++ mechanisms,
	//   which will result in virtual functions in this class being
	//   called.

// Implementation
	OLEOBJECT   m_oleObject;        // must be first member variable
	LPOLECLIENT m_lpClient;
	DECLARE_DYNAMIC(COleServerItem)

// Constructors
	COleServerItem();
	~COleServerItem();

// Public Attributes
public:
	COleServerDoc* GetDocument() const; // return container

// Implementation Attributes
protected:
	COleServerDoc* m_pDocument;
	HPALETTE m_hPalette;
	CRect   m_rectBounds;       // HIMETRIC ! -  if IsRectNull => not set yet

	BOOL    IsConnected();  // TRUE if connected to client

// Operations for notifying client
public:
	int     NotifyClient(OLE_NOTIFICATION wNotification);
	void    NotifyChanged();        // call this after you change item

	void    BeginRevoke();          // revoke client connection

// Overridables you must implement for yourself
	// Raw data access
	virtual void Serialize(CArchive& ar) = 0; // for Native data

protected:
	// User interface
	virtual OLESTATUS OnShow(BOOL bTakeFocus) = 0;

	// Drawing for metafile format (return FALSE if not supported)
	virtual BOOL OnDraw(CMetaFileDC* pDC) = 0;
			// draw to boundaries set in m_rectBounds

// Overridables you may want to implement yourself
	virtual OLESTATUS OnRelease();
							// do extra cleanup
	virtual OLESTATUS OnExtraVerb(UINT nVerb);
							// do extra verbs - default is not implemented
	virtual OLESTATUS OnSetTargetDevice(LPOLETARGETDEVICE lpTargetDevice);
							// track target device changes - default ignores
	virtual OLESTATUS OnSetBounds(LPRECT lpRect);
							// track size changes - default updates m_rectBounds

	// standard get data as text
	virtual BOOL OnGetTextData(CString& rStringReturn);
							// default to not supported

	// more advanced implementation
	virtual OLESTATUS OnGetData(OLECLIPFORMAT nFormat, LPHANDLE lphReturn);

public: // may be called for direct clipboard access too
	virtual HANDLE GetMetafileData();
							// calls 'OnDraw(...)'
	virtual HANDLE GetNativeData();
							// calls 'Serialize(...)'

protected:
// Overridables you do not have to implement
	virtual LPVOID OnQueryProtocol(LPCSTR lpszProtocol) const;
							// default handles "StdFileEditing"
	virtual OLESTATUS OnSetColorScheme(LPLOGPALETTE lpLogPalette);
							// default updates m_hPalette
	virtual OLECLIPFORMAT OnEnumFormats(OLECLIPFORMAT nFormat) const;
							// default handles native + std. formats
	virtual OLESTATUS OnSetData(OLECLIPFORMAT nFormat, HANDLE hData);
							// default routes to GetNativeData
	virtual OLESTATUS OnDoVerb(UINT nVerb, BOOL bShow, BOOL bTakeFocus);
							// default routes to OnShow &/or OnExtraVerb

// Implementation
#ifdef _DEBUG
public:
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	static COleServerItem* FromLp(LPOLEOBJECT lpObject);
	friend struct _afxOleServerItemImplementation;
	friend class COleServerDoc;
	friend struct _afxOleServerDocImplementation;
};


//////////////////////////////////////////////////////////////////////////////
// COleServerDoc - registered server document containing COleServerItems

class COleServerDoc : public CObject
{
protected:
// Implementation
	OLESERVERDOC m_oleServerDoc;        // must be first member variable
	LHSERVERDOC m_lhServerDoc;          // registered handle
	BOOL    m_bWaiting;
	DECLARE_DYNAMIC(COleServerDoc)

// Constructors and Destructors
public:
	COleServerDoc();
	~COleServerDoc();

// Special construction routines if opened by user (or linked file)
	BOOL    Register(COleServer* pServer, LPCSTR lpszDoc);
							// call if opened by user (eg: File Open)
	void    Revoke();       // Revoke and wait to finish

// Attributes
	COleServer* m_pServer;
	HPALETTE m_hPalette;

	BOOL    IsOpen() const;     // TRUE if successfully registered

// Operations
	// changes to the entire document (automatically notifies clients)
	void    NotifyRename(LPCSTR lpszNewName);
	void    NotifyRevert();
	void    NotifySaved();

	// specific notifications for all clients
	void    NotifyAllClients(OLE_NOTIFICATION wNotification);
	void    NotifyClosed();         // call this after you close document
	void    NotifyChanged();        // call this after you change some
									//   global attibute like doc dimensions

protected:
// Overridables you must implement for yourself
	virtual COleServerItem* OnGetDocument() = 0;
				// return new item representing entire document
	virtual COleServerItem* OnGetItem(LPCSTR lpszItemName) = 0;
				// return new item for the named item

	// iteration
	POSITION GetStartPosition() const
				{ return NULL; }    // start of iteration
	virtual COleServerItem* GetNextItem(POSITION& rPosition) = 0;
				// return next item in iteration

// Overridables you may want to implement yourself
	virtual OLESTATUS OnSave();
	virtual OLESTATUS OnClose();
	virtual OLESTATUS OnExecute(LPVOID lpCommands);
	virtual OLESTATUS OnSetDocDimensions(LPRECT lpRect);

// Overridables you do not have to implement
	virtual OLESTATUS OnSetHostNames(LPCSTR lpszHost, LPCSTR lpszHostObj);
	virtual OLESTATUS OnSetColorScheme(LPLOGPALETTE lpLogPalette);
	virtual OLESTATUS OnRelease();

// Overridables for Error traps
protected:
	virtual void CheckAsync(OLESTATUS status);

// Implementation
protected:
	static COleServerDoc* FromLp(LPOLESERVERDOC lpServerDoc);
	void    AddItem(COleServerItem* pItem, LPOLECLIENT lpClient);

	// Used in implementation (opened by COleServer)
	OLESTATUS   BeginRevoke();  // Revoke but don't wait

#ifdef _DEBUG
public:
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	friend struct _afxOleServerDocImplementation;
	friend class COleServer;
	friend struct _afxOleServerImplementation;
};

//////////////////////////////////////////////////////////////////////////////
// COleServer - registered server application

class COleServer : public CObject
{
protected:
// Implementation
	OLESERVER m_oleServer;          // must be first member variable
	LHSERVER m_lhServer;            // registered handle
	DECLARE_DYNAMIC(COleServer)

// Constructors and Destructors
public:
	COleServer(BOOL bLaunchEmbedded);
	~COleServer();

	BOOL    Register(LPCSTR lpszClass, BOOL bMultiInstance);
	void    BeginRevoke();

// Attributes
	BOOL    IsOpen() const;         // TRUE if successfully registered
	BOOL    m_bLaunchEmbedded;

// Overridables you must implement for yourself
protected:
	// for those supporting embedding
	virtual COleServerDoc* OnCreateDoc(LPCSTR lpszClass,
				LPCSTR lpszDoc) = 0;
	virtual COleServerDoc* OnEditDoc(LPCSTR lpszClass,
				LPCSTR lpszDoc) = 0;

// Overridables you may want to implement yourself
	// for those supporting links
	virtual COleServerDoc* OnOpenDoc(LPCSTR lpszDoc);
	// for those supporting embedding from templates
	virtual COleServerDoc* OnCreateDocFromTemplate(LPCSTR lpszClass,
				LPCSTR lpszDoc, LPCSTR lpszTemplate);
	// for those supporting DDE execute commands
	virtual OLESTATUS OnExecute(LPVOID lpCommands);

// Overridables you do not have to implement
	virtual OLESTATUS OnExit();     // default to BeginRevoke
	virtual OLESTATUS OnRelease();  // default to cleanup

// Implementation support of managing # of open documents
protected:
	int     m_cOpenDocuments;
	void    AddDocument(COleServerDoc* pDoc, LHSERVERDOC lhServerDoc);
	void    RemoveDocument(COleServerDoc* pDoc);

// Implementation
#ifdef _DEBUG
public:
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	static COleServer * FromLp(LPOLESERVER lpServer);
	friend class COleServerDoc;

	friend struct _afxOleServerImplementation;
};

// Helper to register server in case of no .REG file loaded
BOOL AfxOleRegisterServerName(LPCSTR lpszClass, LPCSTR lpszLocalClassName);

//////////////////////////////////////////////////////////////////////////////
// Helper class for implementation

class CSharedFile : public CMemFile
{
	DECLARE_DYNAMIC(CSharedFile)

public:
// Constructors
	CSharedFile(UINT nAllocFlags = GMEM_DDESHARE, UINT nGrowBytes = 1024);
	~CSharedFile();

// Attributes
	HANDLE  Detach();
	void    SetHandle(HANDLE hGlobalMemory);

// Special implementation
protected:
	virtual BYTE FAR* Alloc(UINT nBytes);
	virtual BYTE FAR* Realloc(BYTE FAR* lpMem, UINT nBytes);
	virtual void Free(BYTE FAR* lpMem);

	UINT    m_nAllocFlags;
	HANDLE  m_hGlobalMemory;
};

//////////////////////////////////////////////////////////////////////////////
// Inlines

// for client classes
inline OLESTATUS COleClientItem::GetLastStatus() const
	{ return m_lastStatus; }
inline COleClientDoc* COleClientItem::GetDocument() const
	{ return m_pDocument; }
inline OLECLIPFORMAT COleClientItem::EnumFormats(OLECLIPFORMAT nFormat) const
	{ return ::OleEnumFormats(m_lpObject, nFormat); }
inline BOOL COleClientDoc::IsOpen() const
	{ return m_lhClientDoc != NULL; }

// for server classes
inline COleServerDoc* COleServerItem::GetDocument() const
	{ return m_pDocument; }
inline BOOL COleServerItem::IsConnected()
	{ return m_lpClient != NULL; }
inline void COleServerItem::NotifyChanged()
	{ NotifyClient(OLE_CHANGED); }

inline BOOL COleServerDoc::IsOpen() const
	{ return m_lhServerDoc != NULL; }
inline void COleServerDoc::NotifyChanged()
	{ NotifyAllClients(OLE_CHANGED); }
inline void COleServerDoc::NotifyClosed()
	{ NotifyAllClients(OLE_CLOSED); }

inline BOOL COleServer::IsOpen() const
	{ return m_lhServer != NULL; }

//////////////////////////////////////////////////////////////////////////////

#endif //__AFXOLE_H__
