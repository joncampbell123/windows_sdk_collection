/*****************************************************************************\
*                                                                             *
*    Line.h                                                                   *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#ifndef __LINE_H__
#define __LINE_H__

#include "types.h"
#include "Const.h"
#include "Site.h"
#include "Lists.h"

		 
#define kTabWidth				57		// equivalent to 2cm (the tabwidth for the Windows version)

#ifdef _MSC_VER
typedef char LineType;
#define kUnkownLineType  0
#define kTextLineType  1
#define kContainerLineType  2
#else
enum LineType {
	kUnkownLineType,
	kTextLineType,
	kContainerLineType
};
#endif 

#ifndef _MSC_VER
typedef enum LineType LineType;
#endif

#if defined(__MWERKS__)
struct LineRec;
struct DocumentRec;
struct OleOutlineDocRec;
#if qOle
struct OleContainerLineRec;
struct LineListRec;
struct VerbMenuRec;
struct ContainerSiteImpl;
struct DocImpl;
#endif
#endif

typedef void (*LineDisposeProcPtr)(struct LineRec* pLine);
typedef void (*LineFreeProcPtr)(struct LineRec* pLine);
typedef void (*LineDrawProcPtr)(struct LineRec* pLine, Rect* r);
typedef void (*LineDrawSelectedProcPtr)(struct LineRec* pLine, Rect* r);
typedef void (*LineDrawFrameProcPtr)(struct LineRec* pLine, Rect* r);
typedef void (*LineFocusProcPtr)(struct LineRec* pLine, Rect* r);
typedef void (*LineSaveToStorageProcPtr)(struct LineRec* pLine, LPSTORAGE lpSrcStg, LPSTORAGE lpDestStg, LPSTREAM lpDestStm, Boolean fRemember);
typedef void (*LineIndentProcPtr)(struct LineRec* pLine, Boolean indent);
typedef char* (*LineGetTextProcPtr)(struct LineRec* pLine);
typedef void (*LineCopyToDocProcPtr)(struct LineRec* pLine, struct DocumentRec* pDoc);
typedef void (*LineUpdateViewProcPtr)(struct LineRec* pLine);
typedef void (*LineDoDoubleClickProcPtr)(struct LineRec* pLine);
typedef void (*LineSetExtentProcPtr)(struct LineRec* pLine, LPSIZEL pSizeL);

struct LineVtblRec {
	LineDisposeProcPtr				m_DisposeProcPtr;
	LineFreeProcPtr					m_FreeProcPtr;
	
	LineCopyToDocProcPtr			m_CopyToDocProcPtr;

	LineDrawProcPtr					m_DrawProcPtr;
	LineDrawSelectedProcPtr			m_DrawSelectedProcPtr;
	LineDrawFrameProcPtr			m_DrawFrameProcPtr;
	LineFocusProcPtr				m_FocusProcPtr;
	
	LineUpdateViewProcPtr			m_UpdateViewProcPtr;
	
	LineGetTextProcPtr				m_GetTextProcPtr;

	LineIndentProcPtr				m_LineIndentProcPtr;

	LineDoDoubleClickProcPtr		m_DoDoubleClickProcPtr;
	
	LineSetExtentProcPtr			m_SetExtentProcPtr;

#if qOle
		LineSaveToStorageProcPtr	m_SaveToStorageProcPtr;
#endif
};

typedef struct LineVtblRec LineVtblRec, *LineVtblPtr;

LineVtblPtr LineGetVtbl(void);
void LineInitVtbl(void);
void LineDisposeVtbl(void);

struct LineRec {

	LineVtblPtr			vtbl;

	LineType				m_LineType;
	
	struct LineListRec*		m_LineList;
	
	Boolean					m_fNeedsUpdateView;

	short					m_TabLevel;
	short					m_TabWidthInPoints;

	short					m_HeightInPoints;
	short					m_WidthInPoints;
};

typedef struct LineRec LineRec, *LinePtr;

void LineInit(LinePtr pLine, struct LineListRec* pLineList);
void LineDispose(LinePtr pLine);
void LineFree(LinePtr pLine);

#ifdef __PPCC__
struct DocumentRec;
#endif

void LineCopyToDoc(LinePtr pLine, struct DocumentRec* pDoc);

void LineDrawNow(LinePtr pLine);

void LineInval(LinePtr pLine);
void LineValid(LinePtr pLine);

void LineUpdateView(LinePtr pLine);

void LineDraw(LinePtr pLine, Rect* r);
void LineDrawSelected(LinePtr pLine, Rect* r);
void LineDrawFrame(LinePtr pLine, Rect* r);
void LineFocus(LinePtr pLine, Rect* r);

char* LineGetText(LinePtr pLine);

void LineIndent(LinePtr pLine, Boolean indent);
short LineGetTabLevel(LinePtr pLine);
void LineSetTabLevel(LinePtr pLine, short tabLevel);

void LineGetBounds(LinePtr pLine, Rect* r);
void LineSetBounds(LinePtr pLine, Rect* r);
WindowPtr LineGetWindow(LinePtr pLine);
LineType LineGetType(LinePtr pLine);

void LineGetExtent(LinePtr pLine, LPSIZEL lpsizel);
void LineSetExtent(LinePtr pLine, LPSIZEL lpsizel);

void LineDoDoubleClick(LinePtr pLine);

struct LineHeaderRec {
	LineType			m_LineType;
	unsigned short		m_nTabLevel;
	unsigned short		m_nTabWidthInHimetric;
	unsigned short		m_nWidthInHimetric;
	unsigned short		m_nHeightInHimetric;
	long				m_reserved;
};

typedef struct LineHeaderRec LineHeaderRec, *LineHeaderPtr;

// OLDNAME: LineList.h
					   
struct LineRangeRec {
	short	m_nStartLine;
	short	m_nEndLine;
};

typedef enum CursorPos {
	kCPContent,		// in content of the listbox
	// in the drag handle with one or more item selected
	kCPTopDragHandle,
	kCPBotDragHandle,
	
	// in the resize handle with one object selected
	kCPTopLeftHandle,
	kCPMidLeftHandle,
	kCPBotLeftHandle,
	kCPTopMidHandle,
	kCPBotMidHandle,
	kCPTopRightHandle,
	kCPMidRightHandle,
	kCPBotRightHandle,
	
	kCPElseWhere	// none of the above
	
} CursorPos;

typedef struct LineRangeRec LineRangeRec, *LineRangePtr;

struct LineListRec {

	struct DocumentRec*		m_pDoc;

	struct TableRec*		m_Lines;
	struct LineRangeRec		m_lrCopied;

	Boolean					m_fForceUpdate;
	
	short					m_OldScrollHPos;
	short					m_OldScrollVPos;
};

typedef struct LineListRec LineListRec, *LineListPtr;

void LineListInit(LineListPtr pLineList, WindowPtr theWindow, Rect* theRect, struct DocumentRec* pDoc);
void LineListDispose(LineListPtr pLineList);

Boolean LineListAddLine(LineListPtr pLineList, struct LineRec* pLine);
void LineListDeleteLine(LineListPtr pLineList, struct LineRec* pLine);
void LineListDeleteRange(LineListPtr pLineList, LineRangePtr pLineRange);
void LineListNewTextLine(LineListPtr pLineList, StringPtr s);

void LineListGetView(LineListPtr pLineList, Rect *r);
void LineListActivate(LineListPtr pLineList, Boolean becomingActive);
void LineListUpdate(LineListPtr pLineList, RgnHandle visRgn);
short LineListGetHorizOffset(LineListPtr pLineList);
short LineListGetVertOffset(LineListPtr pLineList);
void LineListUpdateView(LineListPtr pLineList);
void LineListSetRowHeight(LineListPtr pLineList, Point theCell, short newHeight);
void LineListUpdateTableLineExtent(LineListPtr pLineList, struct LineRec* pLine);
void LineListDrawLine(LineListPtr pLineList, struct LineRec* pLine);
void LineListInvalLine(LineListPtr pLineList, struct LineRec* pLine);
void LineListValidLine(LineListPtr pLineList, struct LineRec* pLine);
void LineListGetLineRect(LineListPtr pLineList, struct LineRec* pLine, Rect* r);
WindowPtr LineListGetWindow(LineListPtr pLineList);
Boolean LineListClick(LineListPtr pLineList, Point pt, short modifiers);
void LineListDoDoubleClick(LineListPtr pLineList);
void LineListTempSize(LineListPtr pLineList, short width, short height);
void LineListSize(LineListPtr pLineList, short width, short height);
Boolean LineListFindLineCell(LineListPtr pLineList, struct LineRec* pLine, Cell* theCell);
void LineListDrawCell(struct TableRec* theTable, Rect* r, void* p);
void LineListDrawSelectedCell(struct TableRec* theTable, Boolean selected, Rect* r, void* p);
short LineListGetCount(LineListPtr pLineList);
struct LineRec* LineListGetLine(LineListPtr pLineList, short index);
void LineListSetLine(LineListPtr pLineList, short index, struct LineRec* pLine);
void LineListGetExtent(LineListPtr pLineList, LineRangePtr pLineRange, LPSIZEL lpsizel);
void LineListGetExtentRect(LineListPtr pLineList, LineRangePtr pLineRange, Rect* r);
PicHandle LineListGetPICT(LineListPtr pLineList, LineRangePtr pLineRange, Rect* r);
Handle LineListGetText(LineListPtr pLineList, LineRangePtr pLineRange);
Boolean LineListIsSelection(LineListPtr pLineList);
Boolean LineListIsEmpty(LineListPtr pLineList);
void LineListSelectRange(LineListPtr pLineList, LineRangePtr plrSel);
void LineListIndentLines(LineListPtr pLineList, Boolean indent);
void LineListDoClear(LineListPtr pLineList);
void LineListCopyToDoc(LineListPtr pLineList, struct DocumentRec* pDoc);
void LineListShowLine(LineListPtr pLineList, struct LineRec* pLine);
void LineListShowIndexedLine(LineListPtr pLineList, int i);
void LineListSetRedraw(LineListPtr pLineList, Boolean redraw);

Boolean LineListLineIsVisible(LineListPtr pLineList, struct LineRec* pLine);
Boolean LineListIsLineSelected(LineListPtr pLineList, struct LineRec* pLine);
Boolean LineListIsVisible(LineListPtr pLineList);

int LineListGetFocusLineIndex(LineListPtr pLineList);
int LineListCopyRange(LineListPtr pSrcLL, LineRangePtr plrSel, LineListPtr pDestLL);
short LineListGetSelection(LineListPtr pLineList, LineRangePtr plrSel);
LineRangePtr LineListGetCopiedRange(LineListPtr pLineList);
void LineListSetCopiedRange(LineListPtr pLineList, LineRangePtr pLineRange);
Boolean LineListIsLineSelected(LineListPtr pLineList, struct LineRec* pLine);

#ifdef __PPCC__
struct DocumentRec;
#endif

struct DocumentRec*	LineListGetDoc(LineListPtr pLineList);
CursorPos LineListGiveCursorFeedback(LineListPtr pLineList, Point mPt);

#if qOleContainerApp

#ifdef __PPCC__
struct OleContainerLineRec;
#endif

void LineListDoResizeLine(LineListPtr pLineList, CursorPos cp, struct OleContainerLineRec* pContainerLine);
#endif

#if qOleDragDrop
void LineListScroll(LineListPtr pLineList, SCROLLDIR scrolldir);
#endif // qOleDragDrop

struct LineListHeaderRec {
	short		m_nNumLines;
	long		m_reserved1;						// space reserved for future use
	long		m_reserved2;						// space reserved for future use
};

typedef struct LineListHeaderRec LineListHeaderRec, *LineListHeaderPtr;

// OLDNAME: TextLine.h

#define kMaxTextLineLength		255

typedef LineVtblRec TextLineVtblRec, *TextLineVtblPtr;

TextLineVtblPtr TextLineGetVtbl(void);
void TextLineInitVtbl(void);
void TextLineDisposeVtbl(void);

struct TextLineRec {

	LineRec				superClass;

	LineVtblPtr		vtbl;


	short				m_TextFont;
	short				m_TextSize;
	short				m_TextFace;

	short				m_TextDescent;

	unsigned short		m_TextLineLength;
	char				m_Text[kMaxTextLineLength];
};

typedef struct TextLineRec TextLineRec, *TextLinePtr;

void TextLineInit(TextLinePtr pTextLine, struct LineListRec* pLineList);
void TextLineDraw(LinePtr pLine, Rect* r);
void TextLineSetText(TextLinePtr pTextLine, StringPtr s);
char* TextLineGetText(LinePtr pLine);
void TextLineUpdateView(LinePtr pLine);
void TextLineCopyToDoc(LinePtr pLine, struct DocumentRec* pDoc);

#if qOle
  
// OLDNAME: OleLine.h

void LineSaveToStorage(LinePtr pLine, LPSTORAGE lpSrcStg, LPSTORAGE lpDestStg, LPSTREAM lpDestStm, Boolean fRemember);

// OLDNAME: OleLineList.h

#ifdef __PPCC__
struct LineListRec;
struct LineRangeRec;
struct OleOutlineDocRec;
#endif


void LineListSaveSelectionToStorage(struct LineListRec* pLineList, struct LineRangeRec* pSelection, LPSTORAGE lpSrcStg, LPSTORAGE lpDestStg, LPSTREAM lpLineListStm, Boolean fRemember);
void LineListLoadFromStorage(struct LineListRec* pLineList, struct OleOutlineDocRec* pOleOutlineDoc, LPSTORAGE lpSrcStg, LPSTREAM lpLineListStm);

void LineListNewContainerLine(struct LineListRec* pLineList, struct OleOutlineDocRec* pOleOutlineDoc, unsigned long oleCreateType, const CLSID* clsid, FSSpecPtr pFSSpec, PicHandle hIcon, LPSTORAGE pStorage, char* pContainerStorageName);
void LineListNewContainerLineFromData(struct LineListRec* pLineList, struct OleOutlineDocRec* pOleOutlineDoc, LPDATAOBJECT pSrcDataObj, unsigned long oleCreateType, ResType cfFormat, Boolean fDisplayAsIcon, PicHandle hMetaPict, 
                                      char* pContainerStorageName);

#if qOleContainerApp
HRESULT LineListGetItemObject(struct LineListRec* pLineList, char* pszItem, unsigned long SpeedNeeded, REFIID riid, void* * ppvObject);
HRESULT LineListIsItemRunning(struct LineListRec* pLineList, char* pszItem);
HRESULT LineListGetItemStorage(struct LineListRec* pLineList, char* pszItem, void** lplpvStorage);

Boolean LineListDoClose(struct LineListRec* pLineList);

void LineListInformAllOleObjectsDocRenamed(struct LineListRec* pLineList, LPMONIKER pmkDoc);

struct OleContainerLineRec* LineListFindLineByStorageName(struct LineListRec* pLineList, char* StorageName);
unsigned long LineListGetNextLink(struct LineListRec* pLineList, unsigned long dwLink);

#ifdef __PPCC__
struct LineListRec;
struct VerbMenuRec;
#endif

void LineListUpdateEditMenu(struct LineListRec* pLineList, struct VerbMenuRec* pVerbMenuRec);
void LineListDoObjectVerb(struct LineListRec* pLineList, long verb);

#endif // qOleContainerApp

#if qOleContainerApp

// OLDNAME: OleContainerLineInterface.h
void OleContainerLineInitInterfaces(void);

struct ContainerLineSiteImpl {
	struct IContainerSiteVtbl*		lpVtbl;
	struct OleContainerLineRec*		lpOleContainerLine;
};

typedef struct ContainerLineSiteImpl ContainerLineSiteImpl, *ContainerLineSiteImplPtr;

void OleContainerLineIContainerSiteInit(ContainerLineSiteImplPtr pContainerLineSiteImpl, struct OleContainerLineRec* pOleContainerLine);

WindowPtr IOleContainerLineSiteGetWindow(struct ContainerSiteImpl* lpThis);
struct OleDocumentRec* IOleContainerLineSiteGetOleDocument(struct ContainerSiteImpl* lpThis);
OSType IOleContainerLineSiteGetAppCreator(struct ContainerSiteImpl* lpThis);
void IOleContainerLineSiteGetBounds(struct ContainerSiteImpl* lpThis, Rect* rBounds);
LPUNKNOWN IOleContainerLineSiteGetOleObject(struct ContainerSiteImpl* lpThis, REFIID riid);
unsigned long IOleContainerLineSiteGetDrawAspect(struct ContainerSiteImpl* lpThis);
void IOleContainerLineSiteSetExtent(struct ContainerSiteImpl* lpThis, LPSIZEL lpsizel);

#if qOleInPlace
void IOleContainerLineSiteGetClipRgn(struct ContainerSiteImpl* lpThis, RgnHandle clipRgn);
void IOleContainerLineSiteOnPosRectChange(struct ContainerSiteImpl* lpThis, Rect* rBounds);
LPOLEINPLACEFRAME IOleContainerLineSiteGetIPFrame(struct ContainerSiteImpl* lpThis);
LPOLEINPLACEUIWINDOW IOleContainerLineSiteGetIPUIWindow(struct ContainerSiteImpl* lpThis);
void IOleContainerLineSiteDrawNow(struct ContainerSiteImpl* lpThis);
#endif

struct ContainerLineUnknownImpl {
	IUnknownVtbl*					lpVtbl;
	struct OleContainerLineRec*		lpOleContainerLine;
	unsigned long					cRef;
};

typedef struct ContainerLineUnknownImpl ContainerLineUnknownImpl, *ContainerLineUnknownImplPtr;

void OleContainerLineIUnknownInit(ContainerLineUnknownImplPtr pContainerLineUnknownImpl, struct OleContainerLineRec* pOleContainerLine);

// OleContainerLine::IUnknown methods
STDMETHODIMP IOleContainerLineUnknownQueryInterface(LPUNKNOWN lpThis, REFIID riid, void* * lplpvObj);
STDMETHODIMP_(unsigned long) IOleContainerLineUnknownAddRef(LPUNKNOWN lpThis);
STDMETHODIMP_(unsigned long) IOleContainerLineUnknownRelease(LPUNKNOWN lpThis);

struct ContainerLineOleClientSiteImpl {
	IOleClientSiteVtbl*				lpVtbl;
	struct OleContainerLineRec*		lpOleContainerLine;
	unsigned long					cRef;
};

typedef struct ContainerLineOleClientSiteImpl ContainerLineOleClientSiteImpl, *ContainerLineOleClientSiteImplPtr;

void OleContainerLineIOleClientSiteInit(ContainerLineOleClientSiteImplPtr pContainerLineOleClientSiteImpl, struct OleContainerLineRec* pOleContainerLine);

// OleContainerLine::IOleClientSite methods
STDMETHODIMP IOleContainerLineOleClientSiteQueryInterface(LPOLECLIENTSITE lpThis, REFIID riid, void* * lplpvObj);
STDMETHODIMP_(unsigned long) IOleContainerLineOleClientSiteAddRef(LPOLECLIENTSITE lpThis);
STDMETHODIMP_(unsigned long) IOleContainerLineOleClientSiteRelease(LPOLECLIENTSITE lpThis);
STDMETHODIMP IOleContainerLineOleClientSiteSaveObject(LPOLECLIENTSITE lpThis);
STDMETHODIMP IOleContainerLineOleClientSiteGetMoniker(LPOLECLIENTSITE lpThis, unsigned long dwAssign, unsigned long dwWhichMoniker, LPMONIKER * ppmk);
STDMETHODIMP IOleContainerLineOleClientSiteGetContainer(LPOLECLIENTSITE lpThis, LPOLECONTAINER * ppContainer);
STDMETHODIMP IOleContainerLineOleClientSiteShowObject(LPOLECLIENTSITE lpThis);
STDMETHODIMP IOleContainerLineOleClientSiteOnShowWindow(LPOLECLIENTSITE lpThis, unsigned long fShow);
STDMETHODIMP IOleContainerLineOleClientSiteRequestNewObjectLayout(LPOLECLIENTSITE lpThis);

struct ContainerLineAdviseSink2Impl {
	IAdviseSink2Vtbl*				lpVtbl;
	struct OleContainerLineRec*		lpOleContainerLine;
	unsigned long					cRef;
};
typedef struct ContainerLineAdviseSink2Impl ContainerLineAdviseSink2Impl, *ContainerLineAdviseSink2ImplPtr;

void OleContainerLineIAdviseSink2Init(ContainerLineAdviseSink2ImplPtr pContainerLineAdviseSink2Impl, struct OleContainerLineRec* pOleContainerLine);

// OleContainerLine::IAdviseSink2 methods
STDMETHODIMP IOleContainerLineAdviseSink2QueryInterface(LPADVISESINK2 lpThis, REFIID riid, void* * lplpvObj);
STDMETHODIMP_(unsigned long) IOleContainerLineAdviseSink2AddRef(LPADVISESINK2 lpThis);
STDMETHODIMP_(unsigned long) IOleContainerLineAdviseSink2Release(LPADVISESINK2 lpThis);
STDMETHODIMP_(void) IOleContainerLineAdviseSink2OnDataChange(LPADVISESINK2 lpThis, FORMATETC * pFormatetc, STGMEDIUM * pStgmed);
STDMETHODIMP_(void) IOleContainerLineAdviseSink2OnViewChange(LPADVISESINK2 lpThis, unsigned long dwAspect, long lindex);
STDMETHODIMP_(void) IOleContainerLineAdviseSink2OnRename(LPADVISESINK2 lpThis, LPMONIKER pmk);
STDMETHODIMP_(void) IOleContainerLineAdviseSink2OnSave(LPADVISESINK2 lpThis);
STDMETHODIMP_(void) IOleContainerLineAdviseSink2OnClose(LPADVISESINK2 lpThis);
STDMETHODIMP_(void) IOleContainerLineAdviseSink2OnLinkSrcChange(LPADVISESINK2 lpThis, LPMONIKER pmk);

// OLDNAME: OleContainerLine.h

typedef LineVtblRec OleContainerLineVtblRec, *OleContainerLineVtblPtr;

OleContainerLineVtblPtr OleContainerLineGetVtbl(void);
void OleContainerLineInitVtbl(void);
void OleContainerLineDisposeVtbl(void);

struct OleContainerLineRec {

	LineRec							superClass;
	
	LineVtblPtr					vtbl;
	
	ContainerLineSiteImpl			m_ContainerSite;
	
	unsigned long					m_cRef;							// total ref count for the line
	
	Boolean							m_fWindowOpen;					// is object window open
	Boolean							m_fMonikerAssigned;				// has a moniker been assigned
	Boolean							m_fDoGetExtent;					// indicates extents may have changed
	Boolean							m_fDoSetExtent;				    // obj was resized when not running
                                         							//  IOO::SetExtent needed on next run
	unsigned long					m_DrawAspect;					// current display aspect for obj
																	//	(either DVASPECT_CONTENT or
																	//	DVASPECT_ICON)
	Boolean							m_fIsLink;						// is it a linked object?	
	Boolean							m_fLinkUnavailable;				// is the link unavailable?
	char*							m_pszShortType;					// short type name of OLE object needed
	
	char							m_sStorageName[CWCSTORAGENAME];	// name of pstg
	LPSTORAGE						m_pStorage;						// open pstg
	
	ContainerLineUnknownImpl		m_Unknown;
	
	ContainerLineOleClientSiteImpl	m_OleClientSite;
	
	ContainerLineAdviseSink2Impl	m_AdviseSink2;
	
	OleContainerSiteRec				site;
};

typedef struct OleContainerLineRec OleContainerLineRec, *OleContainerLinePtr;

void OleContainerLineInit(OleContainerLinePtr pOleContainerLine, struct LineListRec* pLineList);
void OleContainerLineDispose(LinePtr pLine);
void OleContainerLineFree(LinePtr pLine);

OleContainerLinePtr OleContainerLineCreateFromData(
		struct OleOutlineDocRec* 	pOleOutlineDoc,
		LPDATAOBJECT 		pSrcDataObj,
		unsigned long 		oleCreateType,
		ResType				cfFormat,
		Boolean 			fDisplayAsIcon,
		PicHandle 			hMetaPict,
		char* 				pContainerStorageName
);

void OleContainerLineSetExtent(LinePtr pLine, LPSIZEL pSizeL);
void OleContainerLineUpdateView(LinePtr pLine);
void OleContainerLineUpdateExtent(OleContainerLinePtr pOleContainerLine);

void OleContainerLineDraw(LinePtr pLine, Rect* r);
void OleContainerLineDrawSelected(LinePtr pLine, Rect* r);

void OleContainerLineDoDoubleClick(LinePtr pLine);

HRESULT OleContainerLineQueryInterface(OleContainerLinePtr pOleContainerLine, REFIID riid, void* * lplpvObj);
unsigned long OleContainerLineAddRef(OleContainerLinePtr pOleContainerLine);
unsigned long OleContainerLineRelease(OleContainerLinePtr pOleContainerLine);

void OleContainerLineLoadOleObject(OleContainerLinePtr pOleContainerLine);
Boolean OleContainerLineCloseOleObject(OleContainerLinePtr pOleContainerLine);
void OleContainerLineUnloadOleObject(OleContainerLinePtr pOleContainerLine, unsigned long dwSaveOption);

void OleContainerLineDoVerb(OleContainerLinePtr pOleContainerLine, long verb, Boolean fMessage, Boolean fAction);
void OleContainerLineRunOleObject(OleContainerLinePtr pOleContainerLine);

HRESULT OleContainerLineSaveOleObject(OleContainerLinePtr pOleContainerLine, LPSTORAGE pStorage, Boolean fSameAsLoad, Boolean fRemember, Boolean fForceUpdate);
LPUNKNOWN OleContainerLineGetOleObject(OleContainerLinePtr pOleContainerLine, REFIID riid);
LPMONIKER OleContainerLineGetRelMoniker(OleContainerLinePtr pOleContainerLine, OLEGETMONIKER dwAssign);
LPSTORAGE OleContainerLineGetStorage(OleContainerLinePtr pOleContainerLine);
const char* OleContainerLineGetStorageName(OleContainerLinePtr pOleContainerLine);
HRESULT OleContainerLineGetObject(OleContainerLinePtr pOleContainerLine, unsigned long SpeedNeeded, REFIID riid, void* * ppvObject);
HRESULT OleContainerLineIsRunning(OleContainerLinePtr pOleContainerLine);
void OleContainerLineInformOleObjectDocRenamed(OleContainerLinePtr pOleContainerLine, LPMONIKER pmkDoc);
void OleContainerLineCopyToDoc(LinePtr pLine, struct DocumentRec* pDoc);
char* OleContainerLineGetText(LinePtr pLine);
Boolean OleContainerLineIsOleLink(OleContainerLinePtr pContainerLine);
void OleContainerLineSetLinkUnavailable(OleContainerLinePtr pContainerLine, Boolean fUnavail);
Boolean OleContainerLineIsLinkUnavailable(OleContainerLinePtr pContainerLine);

void OleContainerLineSaveToStorage(LinePtr pLine, LPSTORAGE lpSrcStg, LPSTORAGE lpDestStg, LPSTREAM lpDestStm, Boolean fRemember);
void OleContainerLineLoadFromStorage(OleContainerLinePtr pOleContainerLine, LPSTORAGE lpSrcStg, LPSTREAM lpSrcStm);
void OleContainerLineDoConvert(OleContainerLinePtr pContainerLine, Boolean fServerNotRegistered);

short OleContainerLineFindHandle(OleContainerLinePtr pContainerLine, Point* pLocalPt);

// struct definition for persistant data storage of ContainerLine
struct ContainerLineRec {
	char			m_szStgName[CWCSTORAGENAME];			// stg name w/i cntr stg
	unsigned short	m_fMonikerAssigned;						// has a moniker been assigned to obj
	unsigned long	m_dwDrawAspect;							// current display aspect for obj
															// (either DVASPECT_CONTENT or
															// DVASPECT_ICON)
	SIZEL			m_sizeInHimetric;						// extents of obj in himetric units
	unsigned long	m_dwLinkType;				 			// is it a linked object?
					                                        //  0 -- NOT a link
                    					                    //  OLEUPDATE_ALWAYS (1) -- auto link
                    					                    //  OLEUPDATE_ONCALL (3) -- man. link
	unsigned short	m_fDoSetExtent;				            // obj was resized when not running
                                         					//  IOO::SetExtent needed on next run
};

typedef struct ContainerLineRec ContainerLineRec, *ContainerLinePtr;

#endif //qOleContainer

// OLDNAME: OleTextLine.h

void TextLineSaveToStorage(LinePtr pLine, LPSTORAGE lpSrcStg, LPSTORAGE lpDestStg, LPSTREAM lpDestStm, Boolean fRemeber);
void TextLineLoadFromStorage(TextLinePtr pTextLine, LPSTORAGE lpSrcStg, LPSTREAM lpSrcStm);


#endif // qOle

#endif
