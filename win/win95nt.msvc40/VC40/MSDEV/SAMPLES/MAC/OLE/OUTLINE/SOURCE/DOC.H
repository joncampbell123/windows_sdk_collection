/*****************************************************************************\
*                                                                             *
*    Document.h                                                               *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#ifndef __DOC_H__
#define __DOC_H__

// OLDNAME: Document.h

#include "Window.h"
#include "Const.h"
#include "Line.h"
#if qOleDragDrop
	#include "DXferDoc.h"
#endif


#if qOleServerApp
	#include "DocSvr.h"
#elif qOleContainerApp
	#include "DocCntr.h"
#endif	

#ifdef __MWERKS__
#if qOle
struct OutlineDocRec;
struct OleDocumentRec;
struct DocImpl;
struct DocDataObjectImpl;
struct ContainerSiteImpl;
#endif
#endif 

#define DocumentVtblDef					\
	kDoKeyDownProcPtr;					\
	kScrollRectProcPtr;					\
	kSetFSSpecProcPtr;					\
	kIsEqualFSSpecProcPtr;				\
	kNeedsSaveProcPtr;					\
	kDoCloseProcPtr;					\
	kIsClosingProcPtr;					\
	kOpenFileProcPtr;					\
	kCloseFileProcPtr;					\
	kDoSaveProcPtr;						\
	kDoSaveAsProcPtr;					\
	kDoRevertProcPtr;					\
	kDoNewProcPtr;						\
	kDoOpenProcPtr;						\
	kWriteToFileProcPtr;				\
	kReadFromFileProcPtr;				\
	kSetDefaultCursorProcPtr;			\
	kPresentSaveDialogProcPtr;			\
	kCanCutProcPtr;						\
	kCanCopyProcPtr;					\
	kCanPasteProcPtr;					\
	kCanClearProcPtr;					\
	kCanSaveProcPtr;					\
	kCanSaveAsProcPtr;					\
	kCanRevertProcPtr;					\
	kDoCutProcPtr;						\
	kDoCopyProcPtr;						\
	kDoPasteProcPtr;					\
	kDoClearProcPtr;					\
	kIsDirtyProcPtr;					\
	kSetDirtyProcPtr;					\
	kSetBorderSpaceProcPtr;				\
	kEnableFrameToolsProcPtr;

#define kDoKeyDownProcPtr				void (*m_DoKeyDownProcPtr)(pThis, EventRecord* theEvent)
#define kNeedsSaveProcPtr				YNCResult (*m_NeedsSaveProcPtr)(pThis, Boolean askUserToSave, YNCResult defaultAnswer, Boolean quitting)
#define kDoCloseProcPtr					YNCResult (*m_DoCloseProcPtr)(pThis, Boolean askUserToSave, YNCResult defaultAnswer, Boolean quitting)
#define kIsClosingProcPtr				Boolean (*m_IsClosingProcPtr)(pThis)
#define kOpenFileProcPtr				void (*m_OpenFileProcPtr)(pThis, Boolean readOnly, Boolean newFile)
#define kCloseFileProcPtr				void (*m_CloseFileProcPtr)(pThis)
#define kDoSaveProcPtr					void (*m_DoSaveProcPtr)(pThis)
#define kDoSaveAsProcPtr				void (*m_DoSaveAsProcPtr)(pThis)
#define kDoRevertProcPtr				void (*m_DoRevertProcPtr)(pThis)
#define kDoOpenProcPtr					void (*m_DoOpenProcPtr)(pThis, FSSpecPtr theFile, Boolean readOnly)
#define kWriteToFileProcPtr				void (*m_WriteToFileProcPtr)(pThis)
#define kReadFromFileProcPtr			void (*m_ReadFromFileProcPtr)(pThis)
#define kSetDefaultCursorProcPtr		void (*m_SetDefaultCursorProcPtr)(pThis)
#define kPresentSaveDialogProcPtr		YNCResult (*m_PresentSaveDialogProcPtr)(pThis, Boolean quitting)
#define kScrollRectProcPtr				void (*m_ScrollRectProcPtr)(pThis, short distHoriz, short distVert)
#define kDoNewProcPtr					void (*m_DoNewProcPtr)(pThis)
#define kIsDirtyProcPtr					Boolean (*m_IsDirtyProcPtr)(pThis)
#define kSetDirtyProcPtr				void (*m_SetDirtyProcPtr)(pThis, Boolean fDirty)
#define kCanCutProcPtr					Boolean (*m_CanCutProcPtr)(pThis)
#define kCanCopyProcPtr					Boolean (*m_CanCopyProcPtr)(pThis)
#define kCanPasteProcPtr				Boolean (*m_CanPasteProcPtr)(pThis)
#define kCanClearProcPtr				Boolean (*m_CanClearProcPtr)(pThis)
#define kCanSaveProcPtr					Boolean (*m_CanSaveProcPtr)(pThis)
#define kCanSaveAsProcPtr				Boolean (*m_CanSaveAsProcPtr)(pThis)
#define kCanRevertProcPtr				Boolean (*m_CanRevertProcPtr)(pThis)
#define kDoCutProcPtr					void (*m_DoCutProcPtr)(pThis)
#define kDoCopyProcPtr					void (*m_DoCopyProcPtr)(pThis)
#define kDoPasteProcPtr					void (*m_DoPasteProcPtr)(pThis)
#define kDoClearProcPtr					void (*m_DoClearProcPtr)(pThis)
#define kSetFSSpecProcPtr				void (*m_SetFSSpecProcPtr)(pThis, FSSpecPtr pFile)
#define kIsEqualFSSpecProcPtr			Boolean (*m_IsEqualFSSpecProcPtr)(pThis, FSSpecPtr pFile)
#define kSetBorderSpaceProcPtr			void (*m_SetBorderSpaceProcPtr)(pThis, Rect* prectBorder)
#define kEnableFrameToolsProcPtr		void (*m_EnableFrameToolsProcPtr)(pThis, Boolean fEnable, Boolean fSetBorder)

#undef pThis
#define pThis struct DocumentRec* pDoc

struct DocumentVtblRec {
	WinVtblDef
	DocumentVtblDef
};

typedef struct DocumentVtblRec DocumentVtblRec, *DocumentVtblPtr;

DocumentVtblPtr DocGetVtbl(void);
void DocInitVtbl(void);
void DocDisposeVtbl(void);

#define DocumentDataDefInternal										\
	struct GopherRec*					m_GopherUpdateMenus;		\
	struct GopherRec*					m_GopherDoCmd;				\
	OleDocumentType						m_DocType;					\
	Boolean								m_fDataTransferDoc;			\
	FSSpec								m_File;						\
	short								m_FileRefNum;				\
	OSType								m_FileType;					\
	OSType								m_FileCreator;				\
	Boolean								m_ReadOnly;					\
	Boolean								m_Active;					\
	Boolean								m_fIsClosing;				\
	Boolean								m_Dirty;					\
	Boolean								m_fPositioned;				\
	BORDERWIDTHS						m_BorderWidths;				\
	Boolean								m_fAllocateSpace;			\
	Boolean								m_fShowFrameTools;

#define DocumentDataDefAndBase										\
	WinVtblPtr							baseVtbl;					\
	DocumentDataDefInternal

#define DocumentDataDef												\
	unsigned long						m_DocFiller;				\
	DocumentDataDefInternal

struct DocumentRec {
	DocumentVtblPtr						vtbl;
	WinDataDef
	DocumentDataDefAndBase
};

typedef struct DocumentRec DocumentRec, *DocumentPtr;

void DocInit(DocumentPtr pDoc, short resID, OSType fileType, Boolean fDataTransferDoc);
void DocDispose(DocumentPtr pDoc);

void DocDoActivate(DocumentPtr pDoc, Boolean becomingActive);
void DocHide(DocumentPtr pDoc);

void DocDoKeyDown(DocumentPtr pDoc, EventRecord* theEvent);
YNCResult DocNeedsSave(DocumentPtr pDoc, Boolean askUserToSave, YNCResult defaultAnswer, Boolean quitting);
YNCResult DocDoClose(DocumentPtr pDoc, Boolean askUserToSave, YNCResult defaultAnswer, Boolean quitting);
Boolean DocIsClosing(DocumentPtr pDoc);
void DocOpenFile(DocumentPtr pDoc, Boolean readOnly, Boolean newFile);
void DocCloseFile(DocumentPtr pDoc);
void DocDoSave(DocumentPtr pDoc);
void DocDoSaveAs(DocumentPtr pDoc);
void DocDoRevert(DocumentPtr pDoc);
void DocDoOpen(DocumentPtr pDoc, FSSpecPtr theFile, Boolean readOnly);
void DocWriteToFile(DocumentPtr pDoc);
void DocReadFromFile(DocumentPtr pDoc);
void DocSetDefaultCursor(DocumentPtr pDoc);
YNCResult DocPresentSaveDialog(DocumentPtr pDoc, Boolean quitting);
void DocDoCmd(DocumentPtr pDoc, long cmd);
void DocUpdateMenus(DocumentPtr pDoc);
void DocSetDirty(DocumentPtr pDoc, Boolean fDirty);
void DocScrollRect(DocumentPtr pDoc, short distHoriz, short distVert);
void DocGetFSSpec(DocumentPtr pDoc, FSSpecPtr pFile);
void DocSetFSSpec(DocumentPtr pDoc, FSSpecPtr pFile);
Boolean DocIsEqualFSSpec(DocumentPtr pDoc, FSSpecPtr pFile);
void DocDoNew(DocumentPtr pDoc);
Boolean DocIsDirty(DocumentPtr pDoc);
Boolean DocCanCopy(DocumentPtr pDoc);
Boolean DocCanCut(DocumentPtr pDoc);
Boolean DocCanPaste(DocumentPtr pDoc);
Boolean DocCanClear(DocumentPtr pDoc);
Boolean DocCanSave(DocumentPtr pDoc);
Boolean DocCanSaveAs(DocumentPtr pDoc);
Boolean DocCanRevert(DocumentPtr pDoc);
void DocDoCopy(DocumentPtr pDoc);
void DocDoCut(DocumentPtr pDoc);
void DocDoPaste(DocumentPtr pDoc);
void DocDoClear(DocumentPtr pDoc);

void DocSetBorderSpace(DocumentPtr pDoc, Rect* prectBorder);
void DocEnableFrameTools(DocumentPtr pDoc, Boolean fEnable, Boolean fSetBorder);

// OLDNAME OutlineDocument.h

#define kColumnHeadingsHeight	15
#define kRowHeadingsWidth		28

typedef DocumentVtblRec OutlineDocumentVtblRec, *OutlineDocumentVtblPtr;

OutlineDocumentVtblPtr OutlineDocGetVtbl(void);
void OutlineDocInitVtbl(void);
void OutlineDocDisposeVtbl(void);

struct OutlineDocRec {

	DocumentRec				superClass;

	DocumentVtblPtr		vtbl;

	struct GopherRec*		m_GopherUpdateMenus;
	struct GopherRec*		m_GopherDoCmd;

	struct NameTableRec*	m_NameTable;
	struct LineListRec*		m_LineList;
	
	short					m_nNextRangeNo;
	
	Boolean					m_fShowRowColumnHeadings;
	Boolean					m_fDrawColor;
};

typedef struct OutlineDocRec OutlineDocRec, *OutlineDocPtr;

void OutlineDocInit(OutlineDocPtr pOutlineDoc, short resID, OSType fileType, Boolean fDataTransferDoc);
void OutlineDocDispose(DocumentPtr pDoc);
void OutlineDocDoActivate(DocumentPtr pDoc, Boolean becomingActive);
void OutlineDocDoCmd(OutlineDocPtr pOutlineDoc, long cmd);
void OutlineDocUpdateMenus(OutlineDocPtr pOutlineDoc);
void OutlineDocDoUpdate(DocumentPtr pDoc);
void OutlineDocUpdateRowColumnHeadings(OutlineDocPtr pOutlineDoc);
void OutlineDocGetColumnHeadingRect(OutlineDocPtr pOutlineDoc, Rect* r);
void OutlineDocGetRowHeadingRect(OutlineDocPtr pOutlineDoc, Rect* r);
void OutlineDocDrawGrowIcon(OutlineDocPtr pOutlineDoc);
void OutlineDocScrollRect(DocumentPtr pDoc, short distHoriz, short distVert);
void OutlineDocDoOpen(DocumentPtr pDoc, FSSpecPtr theFile, Boolean readOnly);
long OutlineDocDoGrow(DocumentPtr pDoc, EventRecord* theEvent);
void OutlineDocDoZoom(DocumentPtr pDoc, short partCode);
void OutlineDocDoContent(DocumentPtr pDoc, EventRecord* theEvent);
void OutlineDocDoNewLine(OutlineDocPtr pOutlineDoc);
void OutlineDocIndentLine(OutlineDocPtr pOutlineDoc, Boolean indent);

void OutlineDocDoIdle(DocumentPtr pDoc);

Boolean OutlineDocCanCopy(DocumentPtr pDoc);
Boolean OutlineDocCanCut(DocumentPtr pDoc);
Boolean OutlineDocCanPaste(DocumentPtr pDoc);
Boolean OutlineDocCanClear(DocumentPtr pDoc);

void OutlineDocDoCopy(DocumentPtr pDoc);
void OutlineDocDoPaste(DocumentPtr pDoc);
void OutlineDocDoClear(DocumentPtr pDoc);

void OutlineDocShow(DocumentPtr pDoc);

void OutlineDocResize(OutlineDocPtr pOutlineDoc, Rect* rOldBounds, short wWidth, short wHeight);
void OutlineDocGetLineListBounds(OutlineDocPtr pOutlineDoc, Rect* rBounds);
void OutlineDocSetRowColumnHeadings(OutlineDocPtr pOutlineDoc, Boolean fRowColumnHeadings);

int OutlineDocPasteOutlineData(OutlineDocPtr pOutlineDoc, Handle hOutline, int nStartIndex);
int OutlineDocPasteTextData(OutlineDocPtr pOutlineDoc, Handle hText);

struct LineListRec* OutlineDocGetLineList(OutlineDocPtr pOutlineDoc);
struct NameTableRec* OutlineDocGetNameTable(OutlineDocPtr pOutlineDoc);

struct OutlineDocumentHeaderRec {
	char					m_szFormatName[32];
	short					m_narrAppVersionNo[2];
	short					m_fShowHeading;
	long					m_nNextRangeNo;
	long					m_nNextObjNo;
	long					m_reserved3;			// space reserved for future use
	long					m_reserved4;			// space reserved for future use
};

typedef struct OutlineDocumentHeaderRec OutlineDocumentHeaderRec, *OutlineDocumentHeaderPtr;

#if qOle

#ifdef OLD
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

// OLDNAME: OleDocumentInterface.h
// Flags to specify type of OLECREATE???FROMDATA call required
typedef enum tagOLECREATEFROMDATATYPE {
    OLECREATEFROMDATA_LINK    = 1,
    OLECREATEFROMDATA_OBJECT  = 2,
    OLECREATEFROMDATA_STATIC  = 3
} OLECREATEFROMDATATYPE;

void OleDocInitInterfaces(void);

typedef HRESULT (*OleDocGetClassIDProcPtr)(struct DocImpl* lpThis, CLSID* lpclsidReal, CLSID* lpclsidTreatAs);
typedef WindowPtr (*OleDocGetWindowProcPtr)(struct DocImpl* lpThis);
typedef void (*OleDocGetBoundsProcPtr)(struct DocImpl* lpThis, Rect* rBounds);
typedef void (*OleDocBringToFrontProcPtr)(struct DocImpl* lpThis);
typedef Handle (*OleDocGetMetafilePictDataProcPtr)(struct DocImpl* lpThis, void* pItem);
typedef Handle (*OleDocGetTextDataProcPtr)(struct DocImpl* lpThis, void* pItem);
typedef void (*OleDocGetExtentProcPtr)(struct DocImpl* lpThis, void* pItem, LPSIZEL lpsizel);
typedef Boolean (*OleDocIsDataTransferDocProcPtr)(struct DocImpl* lpThis);
typedef void (*OleDocDocLockAppProcPtr)(struct DocImpl* lpThis);
typedef void (*OleDocDocUnlockAppProcPtr)(struct DocImpl* lpThis);
typedef void (*OleDocGetFSSpecProcPtr)(struct DocImpl* lpThis, FSSpecPtr pFSSpec);
typedef OSType (*OleDocGetCreatorProcPtr)(struct DocImpl* lpThis);
typedef OSType (*OleDocGetTypeProcPtr)(struct DocImpl* lpThis);
typedef Boolean (*OleDocIsVisibleProcPtr)(struct DocImpl* lpThis);
typedef void (*OleDocShowProcPtr)(struct DocImpl* lpThis);
typedef void (*OleDocHideProcPtr)(struct DocImpl* lpThis);
typedef Boolean (*OleDocIsDirtyProcPtr)(struct DocImpl* lpThis);
typedef void (*OleDocSetDocTypeProcPtr)(struct DocImpl* lpThis, OleDocumentType docType);
typedef OleDocumentType (*OleDocGetDocTypeProcPtr)(struct DocImpl* lpThis);
typedef void (*OleDocSetWindowTitleProcPtr)(struct DocImpl* lpThis, StringPtr pTitle);
typedef YNCResult (*OleDocDoCloseProcPtr)(struct DocImpl* lpThis, Boolean askUserToSave, YNCResult defaultAnswer, Boolean quitting);
typedef Boolean (*OleDocIsClosingProcPtr)(struct DocImpl* lpThis);
typedef HRESULT (*OleDocLoadFromStgProcPtr)(struct DocImpl* lpThis, LPSTORAGE lpStg);
typedef HRESULT (*OleDocSaveToStgProcPtr)(struct DocImpl* lpThis, unsigned int uFormat, LPSTORAGE lpStg, Boolean fRemember);
typedef void (*OleDocSetDirtyProcPtr)(struct DocImpl* lpThis, Boolean fDirty);
typedef void (*OleDocSetFSSpecProcPtr)(struct DocImpl* lpThis, FSSpecPtr pSpec);
typedef void (*OleDocResizeProcPtr)(struct DocImpl* lpThis, Rect* rOldBounds, short wWidth, short wHeight);
typedef void (*OleDocPasteTextDataProcPtr)(struct DocImpl* lpThis, Handle hData);

typedef HRESULT (*OleDocGetItemObjectProcPtr)(struct DocImpl* lpThis, char* pszItem, unsigned long SpeedNeeded, LPBINDCTX pbc, REFIID riid, void* * ppvObject);
typedef HRESULT (*OleDocIsItemRunningProcPtr)(struct DocImpl* lpThis, char* pszItem);

#if qOleDragDrop
typedef void (*OleDocGetClientRectProcPtr)(struct DocImpl* lpThis, Rect* rClient);
typedef void (*OleDocScrollProcPtr)(struct DocImpl* lpThis, SCROLLDIR scrolldir);
#endif // qOleDragDrop

#if qOleContainerApp
typedef LPUNKNOWN (*OleDocGetObjectProcPtr)(struct DocImpl* lpThis, void* pItem, REFIID riid);
typedef void (*OleDocPasteOleObjectProcPtr)(struct DocImpl* lpThis, LPDATAOBJECT pSrcDataObj, unsigned long dwCreateType, ResType cfFormat, Boolean fDisplayAsIcon, PicHandle hMetaPict);
typedef void (*OleDocPasteOutlineDataProcPtr)(struct DocImpl* lpThis, LPSTORAGE pStorage);
typedef void (*OleDocInformAllOleObjectsDocRenamedProcPtr)(struct DocImpl* lpThis, LPMONIKER lpmkDoc);
typedef HRESULT (*OleDocGetItemStorageProcPtr)(struct DocImpl* lpThis, char* pszItem, void** lplpvStorage);

#if qOleInPlace
typedef void (*OleDocUpdateWindowPositionProcPtr)(struct DocImpl* lpThis);
typedef void (*OleDocDoActivateProcPtr)(struct DocImpl* lpThis, Boolean becomingActive);
typedef void (*OleDocDoIPDeactivateProcPtr)(struct DocImpl* lpThis);
typedef void (*OleDocDoUIDeactivateProcPtr)(struct DocImpl* lpThis);
#if qFrameTools
typedef Boolean (*OleDocFrameToolsEnabledProcPtr)(struct DocImpl* lpThis);
#endif
#endif

#endif

#if qOleServerApp
typedef char* (*OleDocGetItemNameProcPtr)(struct DocImpl* lpThis, void* pItem);
typedef void (*OleDocSelectItemProcPtr)(struct DocImpl* lpThis, void* pItem);
typedef void (*OleDocRemovePseudoObjFromItemProcPtr)(struct DocImpl* lpThis, void* pItem);
typedef void* (*OleDocGetCopiedRangeProcPtr)(struct DocImpl* lpThis);
typedef LPMONIKER (*OleDocGetRangeFullMonikerProcPtr)(struct DocImpl* lpThis, void* pRange, unsigned long dwAssign);
typedef LPMONIKER (*OleDocGetRangeRelMonikerProcPtr)(struct DocImpl* lpThis, void* pRange, unsigned long dwAssign);
typedef void (*OleDocInformAllPseudoObjectsDocRenamedProcPtr)(struct DocImpl* lpThis, LPMONIKER pmkDoc);
typedef void (*OleDocInformAllPseudoObjectsDocSavedProcPtr)(struct DocImpl* lpThis, LPMONIKER pmkDoc);
typedef void (*OleDocCloseAllPseudoObjsProcPtr)(struct DocImpl* lpThis);
typedef void (*OleDocPositionNewDocProcPtr)(struct DocImpl* lpThis);

#if qOleInPlace
typedef void (*OlePreInPlaceInsertMenusProcPtr)(struct DocImpl* lpThis);
typedef void (*OleGetInPaceBeforeMenuIDsProcPtr)(struct DocImpl* lpThis, short* beforeID1, short* beforeID3, short* beforeID5);
#if qFrameTools
typedef void (*OleEnableFrameToolsProcPtr)(struct DocImpl* lpThis, Boolean fEnable, Boolean fSetBorder);
#endif
#endif

#if qOleTreatAs
typedef void (*OleDocDoTreatAsProcPtr)(struct DocImpl* lpThis, LPSTORAGE pStg);
#endif // qOleTreatAs

#endif // qOleServerApp

#if qOleInPlace
typedef Boolean (*OleDocIsIPActiveProcPtr)(struct DocImpl* lpThis);
typedef Boolean (*OleDocIsUIActiveProcPtr)(struct DocImpl* lpThis);
typedef Boolean (*OleDocIsUIVisibleProcPtr)(struct DocImpl* lpThis);
#endif

struct IDocVtbl {

	OleDocGetClassIDProcPtr						m_GetClassIDProcPtr;
		
	OleDocGetWindowProcPtr						m_GetWindowProcPtr;
	OleDocGetBoundsProcPtr						m_GetBoundsProcPtr;
	OleDocBringToFrontProcPtr					m_BringToFrontProcPtr;

	OleDocGetExtentProcPtr						m_GetExtentProcPtr;

	OleDocGetMetafilePictDataProcPtr			m_GetMetafilePictDataProcPtr;
	OleDocGetTextDataProcPtr					m_GetTextDataProcPtr;
	
	OleDocIsDataTransferDocProcPtr				m_IsDataTransferDocProcPtr;
	OleDocIsDirtyProcPtr						m_IsDirtyProcPtr;
	OleDocIsVisibleProcPtr						m_IsVisibleProcPtr;
	
	OleDocSetDirtyProcPtr						m_SetDirtyProcPtr;
	
	OleDocSetDocTypeProcPtr						m_SetDocTypeProcPtr;
	OleDocGetDocTypeProcPtr						m_GetDocTypeProcPtr;
	
	OleDocDocLockAppProcPtr						m_DocLockAppProcPtr;
	OleDocDocUnlockAppProcPtr					m_DocUnlockAppProcPtr;
	
	OleDocGetFSSpecProcPtr						m_GetFSSpecProcPtr;
	
	OleDocGetCreatorProcPtr						m_GetCreatorProcPtr;
	OleDocGetTypeProcPtr						m_GetTypeProcPtr;
	
	OleDocShowProcPtr							m_ShowProcPtr;
	OleDocHideProcPtr							m_HideProcPtr;
	OleDocSetWindowTitleProcPtr					m_SetWindowTitleProcPtr;
	OleDocSetFSSpecProcPtr						m_SetFSSpecProcPtr;

	OleDocResizeProcPtr							m_ResizeProcPtr;
	
	OleDocDoCloseProcPtr						m_DoCloseProcPtr;
	OleDocIsClosingProcPtr						m_IsClosingProcPtr;
	
	OleDocLoadFromStgProcPtr					m_LoadFromStgProcPtr;
	OleDocSaveToStgProcPtr						m_SaveToStgProcPtr;
	
	OleDocPasteTextDataProcPtr					m_PasteTextDataProcPtr;
	
	OleDocGetItemObjectProcPtr					m_GetItemObjectProcPtr;
	OleDocIsItemRunningProcPtr					m_IsItemRunningProcPtr;

#if qOleDragDrop
	OleDocGetClientRectProcPtr					m_GetClientRectProcPtr;
	OleDocScrollProcPtr							m_ScrollProcPtr;
#endif // qOleDragDrop
		
#if qOleContainerApp
	OleDocGetItemStorageProcPtr					m_GetItemStorageProcPtr;
	OleDocGetObjectProcPtr						m_GetObjectInterfaceProcPtr;
	OleDocPasteOutlineDataProcPtr				m_PasteOutlineDataProcPtr;
	OleDocPasteOleObjectProcPtr					m_PasteOleObjectProcPtr;
	OleDocInformAllOleObjectsDocRenamedProcPtr	m_InformAllOleObjectsDocRenamedProcPtr;
	
#if qOleInPlace
	OleDocUpdateWindowPositionProcPtr			m_UpdateWindowPositionProcPtr;
	OleDocDoActivateProcPtr						m_DoActivateProcPtr;
	OleDocDoIPDeactivateProcPtr					m_DoIPDeactivateProcPtr;
	OleDocDoUIDeactivateProcPtr					m_DoUIDeactivateProcPtr;
#if qFrameTools
	OleDocFrameToolsEnabledProcPtr				m_FrameToolsEnabledProcPtr;
#endif // qFrameTools
#endif // qOleInPlace
#endif // qOleContainerApp

#if qOleServerApp
	OleDocGetItemNameProcPtr					m_GetItemNameProcPtr;
	OleDocSelectItemProcPtr						m_SelectItemProcPtr;
	OleDocRemovePseudoObjFromItemProcPtr		m_RemovePseudoObjFromItemProcPtr;
	OleDocGetCopiedRangeProcPtr					m_GetCopiedRangeProcPtr;
	OleDocGetRangeFullMonikerProcPtr			m_GetRangeFullMonikerProcPtr;
	OleDocGetRangeRelMonikerProcPtr				m_GetRangeRelMonikerProcPtr;
	OleDocInformAllPseudoObjectsDocRenamedProcPtr	m_InformAllPseudoObjectsDocRenamedProcPtr;
	OleDocInformAllPseudoObjectsDocSavedProcPtr		m_InformAllPseudoObjectsDocSavedProcPtr;
	OleDocCloseAllPseudoObjsProcPtr				m_CloseAllPseudoObjsProcPtr;
	OleDocPositionNewDocProcPtr					m_PositionNewDocProcPtr;

#if qOleInPlace
	OlePreInPlaceInsertMenusProcPtr				m_PreInPlaceInsertMenusProcPtr;
	OleGetInPaceBeforeMenuIDsProcPtr			m_GetInPlaceBeforeMenuIDsProcPtr;
#if qFrameTools
	OleEnableFrameToolsProcPtr					m_EnableFrameToolsProcPtr;
#endif
#endif

#if qOleTreatAs
	OleDocDoTreatAsProcPtr						m_DoTreatAsProcPtr;
#endif // qOleTreatAs

#endif // qOleServerApp

#if qOleInPlace
	OleDocIsIPActiveProcPtr						m_IsIPActiveProcPtr;
	OleDocIsUIActiveProcPtr						m_IsUIActiveProcPtr;
	OleDocIsUIVisibleProcPtr					m_IsUIVisibleProcPtr;
#endif

};

typedef struct IDocVtbl IDocVtbl;

struct DocImpl {
	IDocVtbl*					lpVtbl;
};

typedef struct DocImpl DocImpl, *DocImplPtr;

struct DocPersistFileImpl {
	IPersistFileVtbl*			lpVtbl;
	struct OleDocumentRec*		lpOleDoc;
	unsigned long				cRef;			// interface specific ref count
};

typedef struct DocPersistFileImpl DocPersistFileImpl, *DocPersistFileImplPtr;

void DocIPersistFileInit(DocPersistFileImplPtr pDocPersistFileImpl, struct OleDocumentRec* pOleDoc);

// OleDoc::IPersistFile methods
STDMETHODIMP OleDocIntPersistFileQueryInterface(LPPERSISTFILE lpThis, REFIID riid, void* * lplpvObj);
STDMETHODIMP_(unsigned long) OleDocIntPersistFileAddRef(LPPERSISTFILE lpThis);
STDMETHODIMP_(unsigned long) OleDocIntPersistFileRelease(LPPERSISTFILE lpThis);
STDMETHODIMP OleDocIntPersistFileGetClassID(LPPERSISTFILE lpThis, CLSID * lpclsid);
STDMETHODIMP OleDocIntPersistFileIsDirty(LPPERSISTFILE lpThis);
STDMETHODIMP OleDocIntPersistFileLoad(LPPERSISTFILE lpThis, const char* lpszFileName, unsigned long grfMode);
STDMETHODIMP OleDocIntPersistFileSave(LPPERSISTFILE lpThis, const char* lpszFileName, unsigned long fRemember);
STDMETHODIMP OleDocIntPersistFileSaveCompleted(LPPERSISTFILE lpThis, const char* lpszFileName);
STDMETHODIMP OleDocIntPersistFileGetCurFile(LPPERSISTFILE lpThis, char* * lplpszFileName);
STDMETHODIMP OleDocIntPersistFileLoadFsp(LPPERSISTFILE lpThis, const FSSpec *pSpec, unsigned long grfMode);
STDMETHODIMP OleDocIntPersistFileSaveFsp(LPPERSISTFILE lpThis, const FSSpec *pSpec, unsigned long fRemember);
STDMETHODIMP OleDocIntPersistFileSaveCompletedFSP(LPPERSISTFILE lpThis, const FSSpec *pSpec);
STDMETHODIMP OleDocIntPersistFileGetCurFsp(LPPERSISTFILE lpThis, FSSpec *pSpec);

struct DocOleItemContainerImpl {
	IOleItemContainerVtbl*		lpVtbl;
	struct OleDocumentRec*		lpOleDoc;
	unsigned long				cRef;			// interface specific ref count
};

typedef struct DocOleItemContainerImpl DocOleItemContainerImpl, *DocOleItemContainerImplPtr;

void DocIOleItemContainerInit(DocOleItemContainerImplPtr pDocOleItemContainerImpl, struct OleDocumentRec* pOleDoc);

// OleDoc::IOleItemContainer methods
STDMETHODIMP OleDocIntItemContainerQueryInterface(LPOLEITEMCONTAINER lpThis, REFIID riid, void* * lplpvObj);
STDMETHODIMP_(unsigned long) OleDocIntItemContainerAddRef(LPOLEITEMCONTAINER lpThis);
STDMETHODIMP_(unsigned long) OleDocIntItemContainerRelease(LPOLEITEMCONTAINER lpThis);
STDMETHODIMP OleDocIntItemContainerParseDisplayName(LPOLEITEMCONTAINER lpThis, LPBC lpbc, char* lpszDisplayName, unsigned long * lpchEaten, LPMONIKER * lplpmkOut);
STDMETHODIMP OleDocIntItemContainerEnumObjects(LPOLEITEMCONTAINER lpThis, unsigned long grfFlags, LPENUMUNKNOWN * lplpenumUnknown);
STDMETHODIMP OleDocIntItemContainerLockContainer(LPOLEITEMCONTAINER lpThis, unsigned long fLock);
STDMETHODIMP OleDocIntItemContainerGetObject(LPOLEITEMCONTAINER lpThis, char* pszItem, unsigned long SpeedNeeded, LPBINDCTX pbc, REFIID riid, void** ppvObject);
STDMETHODIMP OleDocIntItemContainerGetObjectStorage(LPOLEITEMCONTAINER lpThis, char* lpszItem, LPBINDCTX lpbc, REFIID riid, void* * lplpvStorage);
STDMETHODIMP OleDocIntItemContainerIsRunning(LPOLEITEMCONTAINER lpThis, char* pszItem);

struct DocDataObjectImpl {
	IDataObjectVtbl*			lpVtbl;
	struct OleDocumentRec*		lpOleDoc;
	unsigned long				cRef;			// interface specific ref count
};

typedef struct DocDataObjectImpl DocDataObjectImpl, *DocDataObjectImplPtr;

void DocIDataObjectInit(DocDataObjectImplPtr pDocDataObjectImpl, struct OleDocumentRec* pOleDoc);

// OleDoc::IDataObject methods
STDMETHODIMP OleDocIntDataObjectQueryInterface(LPDATAOBJECT lpThis, REFIID riid, void* * lplpvObj);
STDMETHODIMP_(unsigned long) OleDocIntDataObjectAddRef(LPDATAOBJECT lpThis);
STDMETHODIMP_(unsigned long) OleDocIntDataObjectRelease (LPDATAOBJECT lpThis);
STDMETHODIMP OleDocIntDataObjectGetData(LPDATAOBJECT lpThis, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium);
STDMETHODIMP OleDocIntDataObjectGetDataHere(LPDATAOBJECT lpThis, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium);
STDMETHODIMP OleDocIntDataObjectQueryGetData(LPDATAOBJECT lpThis, LPFORMATETC lpFormatetc);
STDMETHODIMP OleDocIntDataObjectGetCanonicalFormatEtc(LPDATAOBJECT lpThis, LPFORMATETC lpformatetc, LPFORMATETC lpformatetcOut);
STDMETHODIMP OleDocIntDataObjectSetData(LPDATAOBJECT lpThis, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium, unsigned long fRelease);
STDMETHODIMP OleDocIntDataObjectEnumFormatEtc(LPDATAOBJECT lpThis, unsigned long dwDirection, LPENUMFORMATETC * lplpenumFormatEtc);
STDMETHODIMP OleDocIntDataObjectAdvise(LPDATAOBJECT lpThis, FORMATETC * lpFormatetc, unsigned long advf, LPADVISESINK lpAdvSink, unsigned long * lpdwConnection);
STDMETHODIMP OleDocIntDataObjectUnadvise(LPDATAOBJECT lpThis, unsigned long dwConnection);
STDMETHODIMP OleDocIntDataObjectEnumAdvise(LPDATAOBJECT lpThis, LPENUMSTATDATA * lplpenumAdvise);

#if qOleDragDrop

struct DocDropTargetImpl {
	IDropTargetVtbl*			lpVtbl;
	struct OleDocumentRec*		lpOleDoc;
	unsigned long						cRef;			// interface specific ref count
};

typedef struct DocDropTargetImpl DocDropTargetImpl, *DocDropTargetImplPtr;

void DocIDropTargetInit(DocDropTargetImplPtr pDocDropTargetImpl, struct OleDocumentRec* pOleDoc);

STDMETHODIMP OleDocIntDropTargetQueryInterface(LPDROPTARGET lpThis, REFIID riid, void* * lplpvObj);
STDMETHODIMP_(unsigned long) OleDocIntDropTargetAddRef(LPDROPTARGET lpThis);
STDMETHODIMP_(unsigned long) OleDocIntDropTargetRelease (LPDROPTARGET lpThis);
STDMETHODIMP    OleDocIntDropTargetDragEnter(
    LPDROPTARGET            lpThis,
    LPDATAOBJECT            lpDataObj,
    unsigned long           grfKeyState,
    POINTL                  pointl,
    unsigned long*          lpdwEffect
);
STDMETHODIMP OleDocIntDropTargetDragOver(
    LPDROPTARGET            lpThis,
    unsigned long           grfKeyState,
    POINTL                  pointl,
    unsigned long*          lpdwEffect
);
STDMETHODIMP    OleDocIntDropTargetDragLeave(LPDROPTARGET lpThis);
STDMETHODIMP    OleDocIntDropTargetDrop(
    LPDROPTARGET            lpThis,
    LPDATAOBJECT            lpDataObj,
    unsigned long           grfKeyState,
    POINTL                  pointl,
    unsigned long*          lpdwEffect
);

#endif // qOleDragDrop

// OLDNAME: OleOutlineDocInterface.h
void OleOutlineDocInitInterfaces(void);

struct OutlineDocDocImpl {
	struct IDocVtbl*			lpVtbl;
	struct OleOutlineDocRec*	lpOleOutlineDoc;
};

typedef struct OutlineDocDocImpl OutlineDocDocImpl, *OutlineDocDocImplPtr;

void OleOutlineDocIDocInit(OutlineDocDocImplPtr pOutlineDocDocImpl, struct OleOutlineDocRec* pOleOutlineDoc);

struct OutlineDocUnknownImpl {
	IUnknownVtbl*					lpVtbl;
	struct OleOutlineDocRec*		lpOleOutlineDoc;
	unsigned long					cRef;			// interface specific ref count
};

typedef struct OutlineDocUnknownImpl OutlineDocUnknownImpl, *OutlineDocUnknownImplPtr;

void OleOutlineDocIUnknownInit(OutlineDocUnknownImplPtr pOutlineDocUnknownImpl, struct OleOutlineDocRec* pOleOutlineDoc);

// OleOutlineDoc::IUnknown methods
STDMETHODIMP IOleOutlineDocUnknownQueryInterface(LPUNKNOWN lpThis, REFIID riid, void* * lplpvObj);
STDMETHODIMP_(unsigned long) IOleOutlineDocUnknownAddRef(LPUNKNOWN lpThis);
STDMETHODIMP_(unsigned long) IOleOutlineDocUnknownRelease(LPUNKNOWN lpThis);

void OleOutlineDocIDocInit(OutlineDocDocImplPtr pIDoc, struct OleOutlineDocRec* pOleOutlineDoc);

HRESULT IOleOutlineDocGetClassID(struct DocImpl* lpThis, CLSID* lpclsidReal, CLSID* lpclsidTreatAs);
WindowPtr IOleOutlineDocGetWindow(struct DocImpl* lpThis);
void IOleOutlineDocGetBounds(struct DocImpl* lpThis, Rect* rBounds);
void IOleOutlineDocBringToFront(struct DocImpl* lpThis);
void IOleOutlineDocGetExtent(struct DocImpl* lpThis, void* pItem, LPSIZEL lpsizel);

Handle IOleOutlineDocGetMetafilePictData(struct DocImpl* lpThis, void* pItem);
Handle IOleOutlineDocGetTextData(struct DocImpl* lpThis, void* pItem);
Boolean IOleOutlineDocIsDataTransferDoc(struct DocImpl* lpThis);
Boolean IOleOutlineDocIsDirty(struct DocImpl* lpThis);
Boolean IOleOutlineDocIsVisibile(struct DocImpl* lpThis);
void IOleOutlineDocSetDirty(struct DocImpl* lpThis, Boolean fDirty);
void IOleOutlineDocDocLockApp(struct DocImpl* lpThis);
void IOleOutlineDocUnlockApp(struct DocImpl* lpThis);
void IOleOutlineDocGetFSSpec(struct DocImpl* lpThis, FSSpecPtr pFSSpec);
OSType IOleOutlineDocGetCreator(struct DocImpl* lpThis);
OSType IOleOutlineDocGetType(struct DocImpl* lpThis);
void IOleOutlineDocShow(struct DocImpl* lpThis);
void IOleOutlineDocHide(struct DocImpl* lpThis);
void IOleOutlineDocSetDocType(struct DocImpl* lpThis, OleDocumentType docType);
OleDocumentType IOleOutlineDocGetDocType(struct DocImpl* lpThis);
void IOleOutlineDocSetWindowTitle(struct DocImpl* lpThis, StringPtr pTitle);
void IOleOutlineDocSetFSSpec(struct DocImpl* lpThis, FSSpecPtr pSpec);
void IOleOutlineDocResize(struct DocImpl* lpThis, Rect* rOldBounds, short wWidth, short wHeight);
YNCResult IOleOutlineDocDoClose(struct DocImpl* lpThis, Boolean askUserToSave, YNCResult defaultAnswer, Boolean quitting);
Boolean IOleOutlineDocIsClosing(struct DocImpl* lpThis);
HRESULT IOleOutlineDocLoadFromStg(struct DocImpl* lpThis, LPSTORAGE lpStg);
HRESULT IOleOutlineDocSaveToStg(struct DocImpl* lpThis, unsigned int uFormat, LPSTORAGE lpStg, Boolean fRemember);
void IOleOutlineDocPasteTextData(struct DocImpl* lpThis, Handle hData);

HRESULT IOleOutlineDocGetItemObject(struct DocImpl* lpThis, char* pszItem, unsigned long SpeedNeeded, LPBINDCTX pbc, REFIID riid, void** ppvObject);
HRESULT IOleOutlineDocIsItemRunning(struct DocImpl* lpThis, char* pszItem);

#if qOleDragDrop
void IOleOutlineDocGetClientRect(struct DocImpl* lpThis, Rect* rClient);
void IOleOutlineDocScroll(struct DocImpl* lpThis, SCROLLDIR scrolldir);
#endif // qOleDragDrop

#if qOleContainerApp
HRESULT IOleOutlineDocGetItemStorage(struct DocImpl* lpThis, char* pszItem, void** lplpvStorage);
void IOleOutlineDocPasteOutlineData(struct DocImpl* lpThis, LPSTORAGE pStorage);
void IOleOutlineDocPasteOleObject(struct DocImpl* lpThis, LPDATAOBJECT pSrcDataObj, unsigned long dwCreateType, ResType cfFormat, Boolean fDisplayAsIcon, PicHandle hMetaPict);
LPUNKNOWN IOleOutlineDocGetObjectInterface(struct DocImpl*	lpThis, void* pItem, REFIID riid);
void IOleOutlineDocInformAllOleObjectsDocRenamed(struct DocImpl* lpThis, LPMONIKER pmkDoc);

#if qOleInPlace
void IOleOutlineDocUpdateWindowPosition(struct DocImpl* lpThis);
void IOleOutlineDocDoActivate(struct DocImpl* lpThis, Boolean beomingActive);
void IOleOutlineDocDoIPDeactivate(struct DocImpl* lpThis);
void IOleOutlineDocDoUIDeactivate(struct DocImpl* lpThis);
#if qFrameTools
Boolean IOleOutlineDocFrameToolsEnabled(struct DocImpl* lpThis);
#endif
#endif

#endif // qOleContainerApp

#if qOleServerApp
char* IOleOutlineDocGetItemName(struct DocImpl* lpThis, void* pItem);
void IOleOutlineDocSelectItem(struct DocImpl* lpThis, void* pItem);
void IOleOutlineDocRemovePseudoObjFromItem(struct DocImpl* lpThis, void* pItem);
void* IOleOutlineDocGetCopiedRange(struct DocImpl* lpThis);
LPMONIKER IOleOutlineDocGetRangeFullMoniker(struct DocImpl* lpThis, void* pRange, unsigned long dwAssign);
LPMONIKER IOleOutlineDocGetRangeRelMoniker(struct DocImpl* lpThis, void* pRange, unsigned long dwAssign);
void IOleOutlineDocInformAllPseudoObjectsDocRenamed(struct DocImpl* lpThis, LPMONIKER pmkDoc);
void IOleOutlineDocInformAllPseudoObjectsDocSaved(struct DocImpl* lpThis, LPMONIKER pmkDoc);
void IOleOutlineDocCloseAllPseudoObjs(struct DocImpl* lpThis);
void IOleOutlineDocPositionNewDoc(struct DocImpl* lpThis);

#if qOleInPlace
void IOleOutlineDocPreInPlaceInsertMenus(struct DocImpl* lpThis);
void IOleOutlineDocGetInPlaceBeforeMenuIDs(struct DocImpl* lpThis, short* beforeID1, short* beforeID3, short* beforeID5);
#if qFrameTools
void IOleOutlineDocEnableFrameTools(struct DocImpl* lpThis, Boolean fEnable, Boolean fSetBorder);
#endif
#endif

#if qOleTreatAs
void IOleOutlineDocDoTreatAs(struct DocImpl* lpThis, LPSTORAGE pStg);
#endif // qOleTreatAs

#endif // qOleServerApp

#if qOleInPlace
Boolean IOleOutlineDocIsIPActive(struct DocImpl* lpThis);
Boolean IOleOutlineDocIsUIActive(struct DocImpl* lpThis);
Boolean IOleOutlineDocIsUIVisible(struct DocImpl* lpThis);
#endif


typedef OutlineDocumentVtblRec OleOutlineDocVtblRec, *OleOutlineDocVtblPtr;

OleOutlineDocVtblPtr OleOutlineDocGetVtbl(void);
void OleOutlineDocInitVtbl(void);
void OleOutlineDocDisposeVtbl(void);
// added
struct OleDocumentRec {

	IUnknown*				m_pIUnknown;
	DocImpl*				m_pIDoc;
	
	LPSTORAGE				m_lpStorage;
	
	Boolean					m_fIsVisible;
	
	unsigned long			m_dwRegROT;				// key if doc registered as running
	LPMONIKER				m_lpFileMoniker;		// moniker if file-based/untitled doc
	struct OleDocumentRec*	m_pSrcDocOfCopy;
	Boolean					m_fLinkSourceAvail;
	
	DocPersistFileImpl		m_PersistFile;

	DocOleItemContainerImpl	m_OleItemContainer;

	DocDataObjectImpl		m_DataObject;
	
#if qOleDragDrop
	DocDropSourceImpl		m_DropSource;
	DocDropTargetImpl		m_DropTarget;
	
	Boolean					m_fRegDragDrop;
	Boolean 				m_fCanDropCopy;
	Boolean					m_fCanDropLink;
	
    unsigned long			m_TimeEnterScrollArea;	// time entering scroll region
    SCROLLDIR				m_LastScrollDir;		// current dir for drag scroll
    unsigned long			m_NextScrollTime;		// time for next scroll
	
#endif // qOleDragDrop	
	
#if qOleServerApp
	OleServerDocRec		server;
#elif qOleContainerApp
	OleContainerDocRec	container;	
#endif

};

typedef struct OleDocumentRec OleDocumentRec, *OleDocumentPtr;

// end add

struct OleOutlineDocRec {

	OutlineDocRec					superClass;
	
	DocumentVtblPtr				vtbl;

	struct GopherRec*				m_GopherUpdateMenus;
	struct GopherRec*				m_GopherDoCmd;

	unsigned long					m_cfSaveFormat;				// format used to save the doc
	unsigned long					m_cRef;						// total ref count for document

	Boolean							m_fLinkSourceEnabled;
	
	OutlineDocUnknownImpl			m_IUnknown;
	OutlineDocDocImpl				m_IDoc;
	
	OleDocumentRec					m_OleDoc;
	
#if qOleContainerApp
	OleOutlineContainerDocRec		container;
#elif qOleServerApp
	OleOutlineServerDocRec			server;
#endif
};

typedef struct OleOutlineDocRec OleOutlineDocRec, *OleOutlineDocPtr;

void OleOutlineDocInit(OleOutlineDocPtr pOleOutlineDoc, short resID, OSType fileType, Boolean fDataTransferDoc);
void OleOutlineDocDispose(DocumentPtr pDoc);
void OleOutlineDocFree(DocumentPtr pDoc);

void OleOutlineDocDoNew(DocumentPtr pDoc);
void OleOutlineDocOpenFile(DocumentPtr pDoc, Boolean readOnly, Boolean newFile);
void OleOutlineDocCloseFile(DocumentPtr pDoc);
void OleOutlineDocWriteToFile(DocumentPtr pDoc);
void OleOutlineDocReadFromFile(DocumentPtr pDoc);

void OleOutlineDocShow(DocumentPtr pDoc);
void OleOutlineDocHide(DocumentPtr pDoc);
void OleOutlineDocDoKeyDown(DocumentPtr pDoc, EventRecord* theEvent);

#if qOleServerApp && qOleInPlace
void OleOutlineDocSetDefaultCursor(DocumentPtr pDoc);
#endif

void OleOutlineDocDoCmd(OleOutlineDocPtr pOleOutlineDoc, long cmd);
void OleOutlineDocUpdateMenus(OleOutlineDocPtr pOleOutlineDoc);

void OleOutlineDocSetFSSpec(DocumentPtr pDoc, FSSpecPtr pFile);

void OleOutlineDocSetDirty(DocumentPtr pDoc, Boolean fDirty);

YNCResult OleOutlineDocNeedsSave(DocumentPtr pDoc, Boolean askUserToSave, YNCResult defaultAnswer, Boolean quitting);
YNCResult OleOutlineDocDoClose(DocumentPtr pDoc, Boolean askUserToSave, YNCResult defaultAnswer, Boolean quitting);
void OleOutlineDocDoSave(DocumentPtr pDoc);
void OleOutlineDocDoSaveAs(DocumentPtr pDoc);
void OleOutlineDocDoOpen(DocumentPtr pDoc, FSSpecPtr theFile, Boolean readOnly);
YNCResult OleOutlineDocPresentSaveDialog(DocumentPtr pDoc, Boolean quitting);

HRESULT OleOutlineDocQueryInterface(OleOutlineDocPtr pOleOutlineDoc, REFIID riid, void* * lplpvObj);
unsigned long OleOutlineDocAddRef(OleOutlineDocPtr pOleOutlineDoc);
unsigned long OleOutlineDocRelease(OleOutlineDocPtr pOleOutlineDoc);

HRESULT OleOutlineDocSaveSelectionToStorage(OleOutlineDocPtr pOleOutlineDoc, struct LineRangeRec* pSelection, unsigned int uFormat, LPSTORAGE lpDestStg, Boolean fRemember);
HRESULT OleOutlineDocLoadFromStorage(OleOutlineDocPtr pOleOutlineDoc, LPSTORAGE lpSrcStg);
LPMONIKER OleOutlineDocGetFullMoniker(OleOutlineDocPtr pOleOutlineDoc, unsigned long dwAssign);

Boolean OleOutlineDocCanPaste(DocumentPtr pDoc);
void OleOutlineDocDoCopy(DocumentPtr pDoc);
void OleOutlineDocDoClear(DocumentPtr pDoc);
void OleOutlineDocCopyToDoc(DocumentPtr pSrcDoc, DocumentPtr pDestDoc);
void OleOutlineDocDoPaste(DocumentPtr pDoc);
Boolean OleOutlineDocPasteFromData(OleOutlineDocPtr pOleOutlineDoc, LPDATAOBJECT pSrcDataObj, Boolean fLocalDataObj, Boolean fLink);
Boolean OleOutlineDocPasteFormatFromData(
        OleOutlineDocPtr    pOleOutlineDoc,
        ResType          cfFormat,
        LPDATAOBJECT        pSrcDataObj,
        unsigned long                fLocalDataObj,
        unsigned long                fLink,
        unsigned long                fDisplayAsIcon,
        Handle             hMetaPict
);
Boolean OleServerOutlineDocPasteFormatFromData(
        OleOutlineDocPtr        pOleOutlineDoc,
        ResType              cfFormat,
        LPDATAOBJECT            pSrcDataObj,
        Boolean                 fLocalDataObj,
        Boolean                 fLink
);

LPSTORAGE OleOutlineDocGetStorage(OleOutlineDocPtr pOleOutlineDoc);

unsigned int OleOutlineDocUIDialogHook(DialogPtr pDialog, EventRecord *pEvent, short *itemHit, long lCustData);

#if qOleDragDrop
void OleOutlineDocStartDrag(OleOutlineDocPtr pOleOutlineDoc, Point mPt);
unsigned long OleOutlineDocDoDragDrop (OleOutlineDocPtr pSrcOleOutlineDoc);
#endif

void OleOutlineDocDoIdle(DocumentPtr pDoc);
void OleOutlineDocDoActivate(DocumentPtr pDoc, Boolean becomingActive);
void OleOutlineDocDoContent(DocumentPtr pDoc, EventRecord* theEvent);

#if qOleServerApp && qOleTreatAs
void OleOutlineDocDoTreatAs(OleOutlineDocPtr pOleOutlineDoc, LPSTORAGE lpSrcStg);
#endif // qOleTreatAs

#if qOleInPlace
void OleOutlineDocDoDrag(DocumentPtr pDoc, EventRecord* theEvent, Boolean front);
long OleOutlineDocDoGrowWindow(DocumentPtr pDoc, EventRecord* theEvent);
void OleOutlineDocDoZoomWindow(DocumentPtr pDoc, short partCode);
void OleOutlineDocMoveWindow(DocumentPtr pDoc, short h, short v, Boolean front);
void OleOutlineDocScrollRect(DocumentPtr pDoc, short distHoriz, short distVert);
#endif


// OLDNAME: OleDocument.h




// removed



void OleDocInit(OleDocumentPtr pOleDoc, DocImplPtr pIDoc, IUnknown* pIUnknown);
void OleDocDispose(OleDocumentPtr pOleDoc);

void OleDocOpenFile(OleDocumentPtr pOleDoc, Boolean readOnly, Boolean newFile);
void OleDocCloseFile(OleDocumentPtr pOleDoc);
void OleDocWriteToFile(OleDocumentPtr pOleDoc);

void OleDocShow(OleDocumentPtr pOleDoc);
void OleDocHide(OleDocumentPtr pOleDoc);
void OleDocDoActivate(OleDocumentPtr pOleDoc, Boolean becomingActive);

void OleDocUpdateWindowPosition(OleDocumentPtr pOleDoc);

void OleDocDoSave(OleDocumentPtr pOleDoc);
void OleDocDoSaveAs(OleDocumentPtr pOleDoc);
void OleDocDoClose(OleDocumentPtr pOleDoc, Boolean askUserToSave, YNCResult defaultAnswer, Boolean quitting);
Boolean OleDocSetDefaultCursor(OleDocumentPtr pOleDoc);
void OleDocDoIdle(OleDocumentPtr pOleDoc);
void OleDocDoCopy(OleDocumentPtr pOleDoc);

#if qOleDragDrop
Boolean OleDocDoDragScroll(OleDocumentPtr pOleDoc, POINTL pointl);
Boolean OleDocStartDrag(
    OleDocumentPtr  pOleDoc,
	Point			mPt
);
Boolean OleDocQueryDrop (
    OleDocumentPtr  pOleDoc,
    unsigned long   grfKeyState,
    POINTL          pointl,
    Boolean         fDragScroll,
    unsigned long*  pEffect
);
unsigned long OleDocDoDragDrop(OleDocumentPtr pOleDoc);
#endif // qOleDragDrop

void OleDocSetDirty(OleDocumentPtr pOleDoc, Boolean fDirty);
void OleDocSetSizeChanged(OleDocumentPtr pOleDoc, Boolean fSizeChanged);

unsigned long OleDocAddRef(OleDocumentPtr pOleDoc);
unsigned long OleDocRelease(OleDocumentPtr pOleDoc);
HRESULT OleDocLock(OleDocumentPtr pOleDoc, Boolean fLock, Boolean fLastUnlockReleases);
HRESULT OleDocQueryInterface(OleDocumentPtr pOleDoc, REFIID riid, void* * lplpvObj);		// query the document for interfaces
HRESULT OleDocLocalQueryInterface(OleDocumentPtr pOleDoc, REFIID riid, void* * lplpvObj);	// query only OleDoc for interfaces

void OleDocRenamedUpdate(OleDocumentPtr pOleDoc, LPMONIKER lpmkDoc);

HRESULT OleDocDataObjectGetData(OleDocumentPtr pOleDoc, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium);
HRESULT OleDocDataObjectQueryGetData(OleDocumentPtr pOleDoc, LPFORMATETC lpFormatetc);
HRESULT OleDocDataObjectAdvise(OleDocumentPtr pOleDoc, FORMATETC * lpFormatetc, unsigned long advf, LPADVISESINK lpAdvSink, unsigned long * lpdwConnection);
HRESULT OleDocDataObjectUnadvise(OleDocumentPtr pOleDoc, unsigned long dwConnection);

LPSTORAGE OleDocGetStorage(OleDocumentPtr pOleDoc);
LPMONIKER OleDocGetFullMoniker(OleDocumentPtr pOleDoc, unsigned long dwAssign);
void OleDocSetFileMonikerFSSpec(OleDocumentPtr pOleDoc, FSSpecPtr pSpec);
HRESULT OleDocLoadFsp(OleDocumentPtr pOleDoc, FSSpecPtr pSpec, unsigned long grfMode);

void OleDocDoPaste(OleDocumentPtr pOleDoc);
HRESULT OleDocPasteFromData(
        OleDocumentPtr    pOleDoc,
        LPDATAOBJECT        pSrcDataObj,
        Boolean             fLink
);
void OleDocPasteFormatFromData(
        OleDocumentPtr      pOleDoc,
        ResType          	cfFormat,
        LPDATAOBJECT        pSrcDataObj,
        unsigned long       fLink,
        unsigned long       fDisplayAsIcon,
        PicHandle           hMetaPict
);
void OleDocQueryPasteFromData(
        OleDocumentPtr      pOleDoc,
        LPDATAOBJECT        pSrcDataObj,
        Boolean*			pCanCopy,
        Boolean*            pCanLink
);
void OleDocGetPasteStatus(
   		OleDocumentPtr 		pOleDoc,		// not used
    	Boolean*			pCanCopy,
    	Boolean*            pCanLink
);
void OleDocCopyToDoc(OleDocumentPtr pSrcOleDoc, OleDocumentPtr pDestOleDoc);
void OleDocDisableLinkSource(OleDocumentPtr pSrcOleDoc);

#if qOleContainerApp && qOleInPlace
void OleDocMoveWindow(OleDocumentPtr pOleDoc, short h, short v);
#endif

void OleDocEnableDialog(OleDocumentPtr pOleDoc);
void OleDocDisableDialog(OleDocumentPtr pOleDoc, Boolean resumeInPlace);

#endif // qOle
#endif
