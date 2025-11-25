/*****************************************************************************\
*                                                                             *
*    Name.h                                                                   *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#ifndef __POBJ_H__
#define __POBJ_H__

#include "Ole2.h"
#include "Types.h"
#include "Dialogs.h"
#include "Lists.h"
#include "Events.h"

// OLDNAME: Name.h

#define kMaxNameLen 30

#if defined(__MWERKS__)
struct LineRangeRec;
#if qOle
struct OleDocumentRec;
#endif
#endif

/*************************************************************************
** class NameRec
**    The class NameRec stores a particular named selection in the 
**    OUTLINEDOC document. The NAMETABLE class holds all of the names
**    defined in a particular OUTLINEDOC document. Each OUTLINENAME
**    object has a string as its key and a starting line index and an
**    ending line index for the named range.
*************************************************************************/

struct NameRec {
    char     		m_szText[kMaxNameLen+1];
    signed short   	m_nStartLine;  // must be signed for table update
    signed short   	m_nEndLine;    // functions to work
    
#if qOleServerApp
	struct PseudoObjRec* 	m_pPseudoObj; // ptr to pseudo object if allocated
#endif    
};

typedef struct NameRec NameRec, *NamePtr;

NamePtr NameCreate(char* szText, short iFrom, short iTo);
void NameDispose(NamePtr pName);
char* NameGetText(NamePtr pName);
void NameSetText(NamePtr pName, char* pszText);

#ifdef __PPCC__
struct LineRangeRec;
#endif

void NameSetSel(NamePtr pName, struct LineRangeRec* pLineRange, Boolean fRangeModified);
void NameGetSel(NamePtr pName, struct LineRangeRec* pLineRange);
HRESULT NameSaveToStg(NamePtr pName, struct LineRangeRec* pLineRange, LPSTREAM pNTStm, Boolean* pfNameSaved);
HRESULT NameLoadFromStg(NamePtr pName, LPSTREAM pNTStm);

// OLDNAME: NameTable.h
struct NameTableRec {
	DialogPtr				m_NameDialogPtr;
	struct DocumentRec*		m_pDoc;
	ListHandle				m_hListbox;
	short					m_Count;
};

typedef struct NameTableRec NameTableRec, *NameTablePtr;

void NameTableInit(NameTablePtr pNameTable, struct DocumentRec* pDoc);
void NameTableDispose(NameTablePtr pName);
void NameTableDoShowName(NameTablePtr pNameTable);
void NameTableDoGotoName(NameTablePtr pName);
void NameTableAddName(NameTablePtr pNameTable, struct NameRec* pName);
void NameTableDeleteName(NameTablePtr pNameTable, int nIndex);
void NameTableGotoName(NameTablePtr pNameTable, struct NameRec* pName);
#ifndef _MSC_VER
pascal Boolean NameTableDialogFilter(DialogPtr theDialog, EventRecord *theEvent, short *itemHit);
#else
Boolean __pascal NameTableDialogFilter(DialogPtr theDialog, EventRecord *theEvent, short *itemHit);
#endif
short NameTableGetNameIndex(NameTablePtr pNameTable, struct NameRec* pName);
struct NameRec* NameTableGetName(NameTablePtr pNameTable, short nIndex);
struct NameRec* NameTableFindName(NameTablePtr pNameTable, char* pszName);
struct NameRec* NameTableFindNamedRange(NameTablePtr pNameTable, struct LineRangeRec* pLineRange);
short NameTableGetCount(NameTablePtr pNameTable);
void NameTableClearAll(NameTablePtr pNameTable);
void NameTableAddLineUpdate(NameTablePtr pNameTable, short nAddIndex);
void NameTableDeleteLineUpdate(NameTablePtr pNameTable, short nDeleteIndex);

#if qOle
void NameTableSaveSelectionToStorage(NameTablePtr pNameTable, struct LineRangeRec* pSelection, LPSTORAGE lpDestStg);
void NameTableLoadFromStorage(NameTablePtr pNameTable, LPSTORAGE lpSrcStg);
#endif // qOle
struct NameTableHeaderRec {
	short		m_nNumNames;
};

typedef struct NameTableHeaderRec NameTableHeaderRec, *NameTableHeaderPtr;

#if qOleServerApp

// OLDNAME: OleName.h
void NameSendPendingAdvises(NamePtr pName);

#ifdef __PPCC__
struct PseudoObjRec;
struct OleDocumentRec;
#endif

struct PseudoObjRec* NameGetPseudoObj(
        NamePtr            		pName, 
        struct OleDocumentRec*  pOleDoc
);
void NameClosePseudoObj(NamePtr pName);

// OLDNAME: OleNameTable.h
void NameTableEditLineUpdate(
        NameTablePtr	pNameTable, 
        int             nEditIndex
);
void NameTableInformAllPseudoObjectsDocRenamed(
        NameTablePtr	pNameTable, 
        LPMONIKER       pmkDoc
);
void NameTableInformAllPseudoObjectsDocSaved(
        NameTablePtr		pNameTable, 
        LPMONIKER           pmkDoc
);
void NameTableSendPendingAdvises(NameTablePtr pNameTable);
struct PseudoObjRec* NameTableGetPseudoObj(
        NameTablePtr			pNameTable, 
        char*                   pszItem, 
        struct OleDocumentRec*	pOleDoc
);
void NameTableCloseAllPseudoObjs(NameTablePtr pNameTable);

// OLDNAME: OlePseudoObjInterface.h
void OlePseudoObjInitInterfaces(void);


struct PseudoObjUnknownImpl {
	IUnknownVtbl*				lpVtbl;
	struct PseudoObjRec*		lpPseudoObj;
	unsigned long				cRef;			// interface specific ref count
};

typedef struct PseudoObjUnknownImpl PseudoObjUnknownImpl, *PseudoObjUnknownImplPtr;

void PseudoObjIUnknownInit(PseudoObjUnknownImplPtr pPseudoObjUnknownImpl, struct PseudoObjRec* pPseudoObj);

// PseudoObj::IUnknown methods
STDMETHODIMP IPseudoObjUnknownQueryInterface(LPUNKNOWN lpThis, REFIID riid, void* * lplpvObj);
STDMETHODIMP_(unsigned long) IPseudoObjUnknownAddRef(LPUNKNOWN lpThis);
STDMETHODIMP_(unsigned long) IPseudoObjUnknownRelease(LPUNKNOWN lpThis);

struct PseudoObjOleObjectImpl {
	IOleObjectVtbl*				lpVtbl;
	struct PseudoObjRec*		lpPseudoObj;
	unsigned long				cRef;			// interface specific ref count
};

typedef struct PseudoObjOleObjectImpl PseudoObjOleObjectImpl, *PseudoObjOleObjectImplPtr;

void PseudoObjIOleObjectInit(PseudoObjOleObjectImplPtr pPseudoObjOleObjectImpl, struct PseudoObjRec* pPseudoObj);

// PseudoObj::IOleObject methods
STDMETHODIMP IPseudoObjOleObjectQueryInterface(LPOLEOBJECT lpThis, REFIID riid, void* * lplpvObj);
STDMETHODIMP_(unsigned long) IPseudoObjOleObjectAddRef(LPOLEOBJECT lpThis);
STDMETHODIMP_(unsigned long) IPseudoObjOleObjectRelease(LPOLEOBJECT lpThis);
STDMETHODIMP IPseudoObjOleObjectSetClientSite(LPOLEOBJECT lpThis, LPOLECLIENTSITE lpclientSite);
STDMETHODIMP IPseudoObjOleObjectGetClientSite(LPOLEOBJECT lpThis, LPOLECLIENTSITE * lplpClientSite);
STDMETHODIMP IPseudoObjOleObjectSetHostNames(LPOLEOBJECT lpThis, const char* szContainerApp, const char* szContainerObj);
STDMETHODIMP IPseudoObjOleObjectClose(LPOLEOBJECT lpThis, unsigned long dwSaveOption);
STDMETHODIMP IPseudoObjOleObjectSetMoniker(LPOLEOBJECT lpThis, unsigned long dwWhichMoniker, LPMONIKER lpmk);
STDMETHODIMP IPseudoObjOleObjectGetMoniker(LPOLEOBJECT lpThis, unsigned long dwAssign, unsigned long dwWhichMoniker, LPMONIKER * ppmk);
STDMETHODIMP IPseudoObjOleObjectInitFromData(LPOLEOBJECT lpThis, LPDATAOBJECT lpDataObject, unsigned long fCreation, unsigned long reserved);
STDMETHODIMP IPseudoObjOleObjectGetClipboardData(LPOLEOBJECT lpThis, unsigned long reserved, LPDATAOBJECT * lplpDataObject);
STDMETHODIMP IPseudoObjOleObjectDoVerb(LPOLEOBJECT lpThis, long lVerb, EventRecord* lpmsg, LPOLECLIENTSITE lpActiveSite, long lindex, WindowPtr hwndParent, LPCRECT lprcPosRect);
STDMETHODIMP IPseudoObjOleObjectEnumVerbs(LPOLEOBJECT lpThis, LPENUMOLEVERB * lplpenumOleVerb);
STDMETHODIMP IPseudoObjOleObjectUpdate(LPOLEOBJECT lpThis);
STDMETHODIMP IPseudoObjOleObjectIsUpToDate(LPOLEOBJECT lpThis);
STDMETHODIMP IPseudoObjOleObjectGetUserClassID(LPOLEOBJECT lpThis, LPCLSID lpclsid);
STDMETHODIMP IPseudoObjOleObjectGetUserType(LPOLEOBJECT lpThis, unsigned long dwFormOfType, char* * lpszUserType);
STDMETHODIMP IPseudoObjOleObjectSetExtent(LPOLEOBJECT lpThis, unsigned long dwDrawAspect, LPSIZEL lplgrc);
STDMETHODIMP IPseudoObjOleObjectGetExtent(LPOLEOBJECT lpThis, unsigned long dwDrawAspect, LPSIZEL lplgrc);
STDMETHODIMP IPseudoObjOleObjectAdvise(LPOLEOBJECT lpThis, LPADVISESINK lpAdvSink, unsigned long* lpdwConnection);
STDMETHODIMP IPseudoObjOleObjectUnadvise(LPOLEOBJECT lpThis, unsigned long dwConnection);
STDMETHODIMP IPseudoObjOleObjectEnumAdvise(LPOLEOBJECT lpThis, LPENUMSTATDATA * lplpenumAdvise);
STDMETHODIMP IPseudoObjOleObjectGetMiscStatus(LPOLEOBJECT lpThis, unsigned long dwAspect, unsigned long * lpdwStatus);
STDMETHODIMP IPseudoObjOleObjectSetColorScheme(LPOLEOBJECT lpThis, LPOLECOLORSCHEME lpLogpal);
STDMETHODIMP IPseudoObjOleObjectLockObject(LPOLEOBJECT lpThis, unsigned long fLock);

struct PseudoObjDataObjectImpl {
	IDataObjectVtbl*			lpVtbl;
	struct PseudoObjRec*		lpPseudoObj;
	unsigned long						cRef;			// interface specific ref count
};

typedef struct PseudoObjDataObjectImpl PseudoObjDataObjectImpl, *PseudoObjDataObjectImplPtr;

void PseudoObjIDataObjectInit(PseudoObjDataObjectImplPtr pPseudoObjDataObjectImpl, struct PseudoObjRec* pPseudoObj);

// PseudoObj::IDataObject methods
STDMETHODIMP IPseudoObjDataObjectQueryInterface(LPDATAOBJECT lpThis, REFIID riid, void* * lplpvObj);
STDMETHODIMP_(unsigned long) IPseudoObjDataObjectAddRef(LPDATAOBJECT lpThis);
STDMETHODIMP_(unsigned long) IPseudoObjDataObjectRelease (LPDATAOBJECT lpThis);
STDMETHODIMP IPseudoObjDataObjectGetData(LPDATAOBJECT lpThis, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium);
STDMETHODIMP IPseudoObjDataObjectGetDataHere(LPDATAOBJECT lpThis, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium);
STDMETHODIMP IPseudoObjDataObjectQueryGetData(LPDATAOBJECT lpThis, LPFORMATETC lpFormatetc);
STDMETHODIMP IPseudoObjDataObjectGetCanonicalFormatEtc(LPDATAOBJECT lpThis, LPFORMATETC lpformatetc, LPFORMATETC lpformatetcOut);
STDMETHODIMP IPseudoObjDataObjectSetData(LPDATAOBJECT lpThis, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium, unsigned long fRelease);
STDMETHODIMP IPseudoObjDataObjectEnumFormatEtc(LPDATAOBJECT lpThis, unsigned long dwDirection, LPENUMFORMATETC * lplpenumFormatEtc);
STDMETHODIMP IPseudoObjDataObjectAdvise(LPDATAOBJECT lpThis, FORMATETC * lpFormatetc, unsigned long advf, LPADVISESINK lpAdvSink, unsigned long * lpdwConnection);
STDMETHODIMP IPseudoObjDataObjectUnadvise(LPDATAOBJECT lpThis, unsigned long dwConnection);
STDMETHODIMP IPseudoObjDataObjectEnumAdvise(LPDATAOBJECT lpThis, LPENUMSTATDATA * lplpenumAdvise);


// OLDNAME: OlePseudoObj.h
#include "Const.h"

struct PseudoObjRec {
	unsigned long						m_cRef;					// total ref count for obj
	Boolean						m_fObjIsClosing;		// flag to guard recursive close

	void*						m_lpItem;
	struct OleDocumentRec*		m_lpOleDoc;
	LPOLEADVISEHOLDER			m_lpOleAdviseHolder;	// helper obj to hold ole advises
	LPDATAADVISEHOLDER			m_lpDataAdviseHolder;	// helper obj to hold data advises
	struct IEnumHolder*			m_lpEnumHolder;			// helper obj to hold enumerators

	Boolean						m_fDataChanged;			// data changed when draw disabled

	PseudoObjUnknownImpl		m_Unknown;

	PseudoObjOleObjectImpl		m_OleObject;

	PseudoObjDataObjectImpl		m_DataObject;
};

typedef struct PseudoObjRec PseudoObjRec, *PseudoObjPtr;

void PseudoObjInit(PseudoObjPtr pPseudoObj, void* pItem, struct OleDocumentRec* pOleDoc);
void PseudoObjDispose(PseudoObjPtr pPseudoObj);

unsigned long PseudoObjAddRef(PseudoObjPtr pPseudoObj);
unsigned long PseudoObjRelease(PseudoObjPtr pPseudoObj);
HRESULT PseudoObjQueryInterface(PseudoObjPtr pPseudoObj, REFIID riid, void* * lplpvObj);
Boolean PseudoObjClose(PseudoObjPtr pPseudoObj);
void PseudoObjGetExtent(PseudoObjPtr pPseudoObj, LPSIZEL lpsizel);
void PseudoObjSendAdvise(PseudoObjPtr pPseudoObj, OLE_NOTIFICATION wAdvise, LPMONIKER lpmkObj, ADVF dwAdvf);
LPMONIKER PseudoObjGetFullMoniker(PseudoObjPtr pPseudoObj, LPMONIKER pmkDoc);

#endif // qOleServerApp

#endif // __POBJ_H__
