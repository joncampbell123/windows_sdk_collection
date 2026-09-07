#ifndef __DOCCNTR_H__
#define __DOCCNTR_H__


#if qOleInPlace

// OLDNAME: OleInPlaceContainerDocInt.h

void OleInPlaceContainerDocInitInterfaces(void);

struct OleInPlaceContainerUIWindowImpl {
	IOleInPlaceUIWindowVtbl*	lpVtbl;
	struct OleDocumentRec*		lpOleDoc;
	unsigned long				cRef;
};

typedef struct OleInPlaceContainerUIWindowImpl OleInPlaceContainerUIWindowImpl, *OleInPlaceContainerUIWindowImplPtr;

void OleInPlaceContainerIUIWindowInit(OleInPlaceContainerUIWindowImplPtr pOleInPlaceContainerUIWindowImpl, struct OleDocumentRec* pOleDoc);

// OleInPlace::UIWindow methods
STDMETHODIMP IOleInPlaceContainerDocUIWindowQueryInterface(LPOLEINPLACEUIWINDOW lpThis, REFIID riid, void * * ppvObj);
STDMETHODIMP_(unsigned long) IOleInPlaceContainerDocUIWindowAddRef(LPOLEINPLACEUIWINDOW lpThis);
STDMETHODIMP_(unsigned long) IOleInPlaceContainerDocUIWindowRelease(LPOLEINPLACEUIWINDOW lpThis);
STDMETHODIMP IOleInPlaceContainerDocUIWindowGetWindow(LPOLEINPLACEUIWINDOW lpThis, WindowPtr * lphwnd);
STDMETHODIMP IOleInPlaceContainerDocUIWindowContextSensitiveHelp(LPOLEINPLACEUIWINDOW lpThis, unsigned long fEnterMode);
STDMETHODIMP IOleInPlaceContainerDocUIWindowGetBorder(LPOLEINPLACEUIWINDOW lpThis, Rect * lprectBorder);
STDMETHODIMP IOleInPlaceContainerDocUIWindowRequestBorderSpace(LPOLEINPLACEUIWINDOW lpThis, LPCBORDERWIDTHS lpborderwidths);
STDMETHODIMP IOleInPlaceContainerDocUIWindowSetBorderSpace(LPOLEINPLACEUIWINDOW lpThis, LPCBORDERWIDTHS lpborderwidths);
STDMETHODIMP IOleInPlaceContainerDocUIWindowSetActiveObject(LPOLEINPLACEUIWINDOW lpThis, LPOLEINPLACEACTIVEOBJECT lpActiveObject, const char * lpszObjName);

// OLDNAME: OleInPlaceContainerDoc.h
struct OleInPlaceContainerDocRec {

	Boolean									m_fIgnoreActivateEvent;
	Boolean									m_fIgnoreOnUIVisible;

	ProcessSerialNumber						m_ServerPSN;

	OleInPlaceContainerUIWindowImpl			m_UIWindow;
	
	LPOLEINPLACEACTIVEOBJECT				m_pActiveObject;
};

typedef struct OleInPlaceContainerDocRec OleInPlaceContainerDocRec, *OleInPlaceContainerDocPtr;

void OleInPlaceContainerDocInit(struct OleDocumentRec* pOleDoc);
void OleInPlaceContainerDocDispose(struct OleDocumentRec* pOleDoc);

HRESULT OleInPlaceContainerDocLocalQueryInterface(struct OleDocumentRec* pOleDoc, REFIID riid, void* * lplpvObj);

Boolean OleInPlaceContainerDocIsUIActive(struct OleDocumentRec* pOleDoc);
Boolean OleInPlaceContainerDocIsUIVisible(struct OleDocumentRec* pOleDoc);

void OleInPlaceContainerDocIPDeactivate(struct OleDocumentRec* pOleDoc);
void OleInPlaceContainerDocUIDeactivate(struct OleDocumentRec* pOleDoc);

LPOLEINPLACEUIWINDOW OleInPlaceContainerDocGetIPUIWindow(struct OleDocumentRec* pOleDoc);
HRESULT OleInPlaceContainerDocSetBorderSpace(struct OleDocumentRec* pOleDoc, LPCBORDERWIDTHS lpborderwidths);
HRESULT OleInPlaceContainerDocSetActiveObject(struct OleDocumentRec* pOleDoc, LPOLEINPLACEACTIVEOBJECT pActiveObject, const char* pObjectName);

void OleInPlaceContainerDocDoActivate(struct OleDocumentRec* pOleDoc, Boolean becomingActive);
Boolean OleInPlaceContainerDocNeedsActivateEvent(struct OleDocumentRec* pOleDoc);
void OleInPlaceContainerDocUpdateWindowPosition(struct OleDocumentRec* pOleDoc);

void OleInPlaceContainerDocSetIdleCursor(struct OleDocumentRec* pOleDoc, CursPtr pCursor);

// OLNAME: OleOutlineInPlaceContainer
long OleOutlineInPlaceContainerMenuItemToCmd(struct ApplicationRec* pApp, short menuID, short menuItem);
void OleOutlineInPlaceContainerSetIdleCursor(struct ApplicationRec* pApp, CursPtr pCursor);

#endif // qOleInPlace

// OLDNAME: OleOutlineContainerDocInt.h
#define kMaxPathLen		256

void OleOutlineContainerDocInitInterfaces(void);

struct OutlineDocOleUILinkContainerImpl {
	IOleUILinkContainerVtbl*		lpVtbl;
	struct OleOutlineDocRec*		lpOleOutlineDoc;
	unsigned long					cRef;			// interface specific ref count
};

typedef struct OutlineDocOleUILinkContainerImpl OutlineDocOleUILinkContainerImpl, *OutlineDocOleUILinkContainerImplPtr;

void OleOutlineDocIOleUILinkContainerInit(OutlineDocOleUILinkContainerImplPtr pOutlineDocOleUILinkContImpl, struct OleOutlineDocRec* pOleOutlineDoc);

STDMETHODIMP IOleOutlineDocLinkContQueryInterface(
        LPOLEUILINKCONTAINER    lpThis,
        REFIID             		riid,
        void**             		lplpvObj
);
STDMETHODIMP_(unsigned long) IOleOutlineDocLinkContAddRef(LPOLEUILINKCONTAINER lpThis);
STDMETHODIMP_(unsigned long) IOleOutlineDocLinkContRelease(LPOLEUILINKCONTAINER lpThis);
STDMETHODIMP_(unsigned long) IOleOutlineDocLinkContGetNextLink(
        LPOLEUILINKCONTAINER    lpThis,
        unsigned long           dwLink
);
STDMETHODIMP IOleOutlineDocLinkContSetLinkUpdateOptions(
        LPOLEUILINKCONTAINER    lpThis,
        unsigned long           dwLink,
        unsigned long           dwUpdateOpt
);
STDMETHODIMP IOleOutlineDocLinkContGetLinkUpdateOptions(
        LPOLEUILINKCONTAINER    	lpThis,
        unsigned long               dwLink,
        unsigned long*              lpdwUpdateOpt
);
STDMETHODIMP IOleOutlineDocLinkContSetLinkSource(
        LPOLEUILINKCONTAINER	lpThis,
		unsigned long			dwLink,
        char*					lpszDisplayName,
		unsigned long			lenFileName,		
        unsigned long*			lpchEaten,
		Boolean					fValidateSource
);
STDMETHODIMP IOleOutlineDocLinkContGetLinkSource(
        LPOLEUILINKCONTAINER    lpThis,
        unsigned long       	dwLink,
        char**              	lplpszDisplayName,
        unsigned long*      	lplenFileName,
        char**              	lplpszFullLinkType,
        char**              	lplpszShortLinkType,
        Boolean*            	lpfSourceAvailable,
        Boolean*            	lpfIsSelected
);
STDMETHODIMP IOleOutlineDocLinkContOpenLinkSource(
        LPOLEUILINKCONTAINER    lpThis,
        unsigned long           dwLink
);
STDMETHODIMP IOleOutlineDocLinkContUpdateLink(
        LPOLEUILINKCONTAINER    lpThis,
        unsigned long           dwLink,
		Boolean					fErrorMessage,
		Boolean					fErrorAction		// ignore if fErrorMessage
													//		is FALSE
);
STDMETHODIMP IOleOutlineDocLinkContCancelLink(
        LPOLEUILINKCONTAINER    lpThis,
        unsigned long           dwLink
);

// OLDNAME: OleOutlineContainerDoc.h
struct OleOutlineContainerDocRec {
	VerbMenuRec							m_VerbMenuRec;	// menu ID for Convert in the Object Verb menu
	
	unsigned long						m_nNextObjNo;

	LPSTORAGE							m_pStorage;

	OutlineDocOleUILinkContainerImpl	m_IOleUILinkContainer;
};

typedef struct OleOutlineContainerDocRec OleOutlineContainerDocRec, *OleOutlineContainerDocPtr;

void OleOutlineContainerDocInit(struct OleOutlineDocRec* pOleOutlineDoc);
void OleOutlineContainerDocDispose(struct OleOutlineDocRec* pOleOutlineDoc);

void OleOutlineContainerDocDoNew(struct OleOutlineDocRec* pOleOutlineDoc);
YNCResult OleOutlineContainerDocDoClose(struct OleOutlineDocRec* pOleOutlineDoc, Boolean askUserToSave, YNCResult defaultAnswer, Boolean quitting);

void OleOutlineContainerDocInsertDlg(struct OleOutlineDocRec* pOleOutlineDoc);
void OleOutlineContainerDocEditLinksDlg(struct OleOutlineDocRec* pOleOutlineDoc);
void OleOutlineContainerDocPasteSpecialDlg(struct OleOutlineDocRec* pOleOutlineDoc);
void OleOutlineContainerDocUpdateLinksDlg(struct OleOutlineDocRec* pOleOutlineDoc);
void OleOutlineContainerDocConvert(
		struct OleOutlineDocRec* 	pOleOutlineDoc,
		Boolean				fServerNotRegistered
);
void OleOutlineContainerDocNewStorageName(struct OleOutlineDocRec* pOleOutlineDoc, char* pStorageName);
Boolean OleOutlineContainerDocStorageExists(struct OleOutlineDocRec* pOleOutlineDoc, char* pStorageName);
HRESULT OleOutlineContainerDocPasteOutlineData(
        struct OleOutlineDocRec*		pOleOutlineDoc,
        LPSTORAGE               pStorage
);
void OleOutlineContainerDocPasteOleObject(
        struct OleOutlineDocRec*	pOleOutlineDoc,
        LPDATAOBJECT            	pSrcDataObj,
		unsigned long				dwCreateType,
		ResType						cfFormat,
        Boolean                 	fDisplayAsIcon,
        PicHandle                 	hMetaPict
);

HRESULT OleOutlineContainerDocGetItemObject(struct OleOutlineDocRec* pOleOutlineDoc, char* pszItem, unsigned long SpeedNeeded, LPBINDCTX pbc, REFIID riid, void** ppvObject);
HRESULT OleOutlineContainerDocIsItemRunning(struct OleOutlineDocRec* pOleOutlineDoc, char* pszItem);
HRESULT OleOutlineContainerDocGetItemStorage(struct OleOutlineDocRec* pOleOutlineDoc, char* pszItem, void** lplpvStorage);
void OleOutlineContainerDocInformAllOleObjectsDocRenamed(struct OleOutlineDocRec* pOleOutlineDoc, LPMONIKER pmkDoc);
unsigned long OleOutlineContainerDocGetNextLink(struct OleOutlineDocRec* pOleOutlineDoc, unsigned long dwLink);
void OleOutlineContainerDocUnloadAllOleObjectsOfClass(
        struct OleOutlineDocRec* 	pOleOutlineDoc,
        REFCLSID            		rClsid,
        unsigned long				dwSaveOption
);

#if qOleInPlace
void OleOutlineContainerDocUpdateWindowPosition(struct OleOutlineDocRec* pOleOutlineDoc);
void OleOutlineContainerDocScrollRect(struct OleOutlineDocRec* pOleOutlineDoc, short distHoriz, short distVert);
Boolean OleOutlineContainerDocIsIPActive(struct OleOutlineDocRec* pOleOutlineDoc);
Boolean OleOutlineContainerDocIsUIActive(struct OleOutlineDocRec* pOleOutlineDoc);
Boolean OleOutlineContainerDocIsUIVisible(struct OleOutlineDocRec* pOleOutlineDoc);
void OleOutlineContainerDocDoIPDeactivate(struct OleOutlineDocRec* pOleOutlineDoc);
void OleOutlineContainerDocDoUIDeactivate(struct OleOutlineDocRec* pOleOutlineDoc);
#if qFrameTools
Boolean OleOutlineContainerDocFrameToolsEnabled(struct OleOutlineDocRec* pOleOutlineDoc);
#endif
#endif

// OLDNAME: OleContainerDoc.h
struct OleContainerDocRec {
	unsigned long				m_AspectOleObjCopied;		// Draw aspect of single embedding copied
	CLSID						m_clsidOleObjCopied;		// CLSID of single embedding copied
	void*						m_pCopiedEmbedding;			// pointer to copied item
	void*						m_pCopiedEmbeddingSource;	// pointer to source of copied item
	
#if qOleInPlace
	OleInPlaceContainerDocRec	inplace;
#endif
};

typedef struct OleContainerDocRec OleContainerDocRec, *OleContainerDocPtr;

void OleContainerDocInit(struct OleDocumentRec* pOleDoc);
void OleContainerDocDispose(struct OleDocumentRec* pOleDoc);
void OleContainerDocDoCopySingleEmbedding(
		struct OleDocumentRec*		pOleDoc,
		LPOLEOBJECT			pSrcOleObj,
		unsigned long		DrawAspect,
		void*				pCopiedItem,
		void*				pCopiedItemSource
);
void OleContainerDocPasteLink(struct OleDocumentRec* pOleDoc);
void OleContainerDocPasteFormatFromData(
        struct OleDocumentRec*	pOleDoc,
        ResType              	cfFormat,
        LPDATAOBJECT            pSrcDataObj,
        Boolean                 fLink,
        Boolean                 fDisplayAsIcon,
        PicHandle               hMetaPict
);
void OleContainerDocRenamedUpdate(struct OleDocumentRec* pOleDoc, LPMONIKER lpmkDoc);

HRESULT OleContainerDocLocalQueryInterface(struct OleDocumentRec* pOleDoc, REFIID riid, void* * lplpvObj);
void OleContainerDocDoActivate(struct OleDocumentRec* pOleDoc, Boolean becomingActive);


#endif // __DOCCNTR_H__

