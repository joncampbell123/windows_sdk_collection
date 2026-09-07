/*****************************************************************************\
*                                                                             *
*    OleInPlaceContainerApp.h                                                 *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#ifndef __APPCNTR_H__
#define __APPCNTR_H__

#ifdef __PPCC__
struct OleApplicationRec;
#endif


#if qOleInPlace

// OLDNAME: OleInPlaceContainerAppInt.h
void OleInPlaceContainerAppInitInterfaces(void);

struct OleInPlaceContainerFrameImpl {
	IOleInPlaceFrameVtbl*		lpVtbl;
	struct OleApplicationRec*	lpOleApp;
	unsigned long				cRef;
};

typedef struct OleInPlaceContainerFrameImpl OleInPlaceContainerFrameImpl, *OleInPlaceContainerFrameImplPtr;

void OleInPlaceContainerIFrameInit(OleInPlaceContainerFrameImplPtr pOleInPlaceContainerFrameImpl, struct OleApplicationRec* pOleApp);

// OleInPlace::Frame methods
STDMETHODIMP IOleInPlaceContainerAppFrameQueryInterface(LPOLEINPLACEFRAME lpThis, REFIID riid, void * * ppvObj);
STDMETHODIMP_(unsigned long) IOleInPlaceContainerAppFrameAddRef(LPOLEINPLACEFRAME lpThis);
STDMETHODIMP_(unsigned long) IOleInPlaceContainerAppFrameRelease(LPOLEINPLACEFRAME lpThis);
STDMETHODIMP IOleInPlaceContainerAppFrameGetWindow(LPOLEINPLACEFRAME lpThis, WindowPtr * lphwnd);
STDMETHODIMP IOleInPlaceContainerAppFrameContextSensitiveHelp(LPOLEINPLACEFRAME lpThis, unsigned long fEnterMode);
STDMETHODIMP IOleInPlaceContainerAppFrameGetBorder(LPOLEINPLACEFRAME lpThis, Rect * lprectBorder);
STDMETHODIMP IOleInPlaceContainerAppFrameRequestBorderSpace(LPOLEINPLACEFRAME lpThis, LPCBORDERWIDTHS lpborderwidths);
STDMETHODIMP IOleInPlaceContainerAppFrameSetBorderSpace(LPOLEINPLACEFRAME lpThis, LPCBORDERWIDTHS lpborderwidths);
STDMETHODIMP IOleInPlaceContainerAppFrameSetActiveObject(LPOLEINPLACEFRAME lpThis, LPOLEINPLACEACTIVEOBJECT lpActiveObject, const char * lpszObjName);
STDMETHODIMP IOleInPlaceContainerAppFrameInsertMenus(LPOLEINPLACEFRAME lpThis, OleMBarHandle hOleMBar);
STDMETHODIMP IOleInPlaceContainerAppFrameAdjustMenus(LPOLEINPLACEFRAME lpThis, OleMBarHandle hOleMBar);
STDMETHODIMP IOleInPlaceContainerAppFrameRemoveMenus(LPOLEINPLACEFRAME lpThis, OleMBarHandle hOleMBar);
STDMETHODIMP IOleInPlaceContainerAppFrameSetStatusText(LPOLEINPLACEFRAME lpThis, const char * lpszStatusText);
STDMETHODIMP IOleInPlaceContainerAppFrameEnableModeless(LPOLEINPLACEFRAME lpThis, unsigned long fEnable);
STDMETHODIMP IOleInPlaceContainerAppFrameTranslateAccelerator(LPOLEINPLACEFRAME lpThis, EventRecord * lpmsg, long ID);


// OLDNAME: OleInPlaceContainerApp.h

struct OleInPlaceContainerAppRec {
	Boolean								m_fIgnoreResumeEvent;		// inplace window is activating
	Boolean								m_fIgnoreSuspendEvent;		// inplace window is deactivating

	OleInPlaceContainerFrameImpl		m_Frame;
	
	LPOLEINPLACEACTIVEOBJECT			m_pActiveObject;
};

typedef struct OleInPlaceContainerAppRec OleInPlaceContainerAppRec, *OleInPlaceContainerAppPtr;

void OleInPlaceContainerAppInit(struct OleApplicationRec* pOleApp);
void OleInPlaceContainerAppDispose(struct OleApplicationRec* pOleApp);

HRESULT OleInPlaceContainerAppLocalQueryInterface(struct OleApplicationRec* pOleApp, REFIID riid, void* * lplpvObj);
LPOLEINPLACEFRAME OleInPlaceContainerAppGetIPFrame(struct OleApplicationRec* pOleApp);
HRESULT OleInPlaceContainerAppInsertMenus(struct OleApplicationRec* pOleApp, OleMBarHandle hOleMBar);
HRESULT OleInPlaceContainerAppAdjustMenus(struct OleApplicationRec* pOleApp, OleMBarHandle hOleMBar);
HRESULT OleInPlaceContainerAppRemoveMenus(struct OleApplicationRec* pOleApp, OleMBarHandle hOleMBar);
HRESULT OleInPlaceContainerAppTranslateAccelerator(struct OleApplicationRec* pOleApp, EventRecord* pEvent, long mResult);
HRESULT OleInPlaceContainerAppGetBorder(struct OleApplicationRec* pOleApp, Rect * lprectBorder);
HRESULT OleInPlaceContainerAppSetBorderSpace(struct OleApplicationRec* pOleApp, LPCBORDERWIDTHS lpborderwidths);
HRESULT OleInPlaceContainerAppSetActiveObject(struct OleApplicationRec* pOleApp, LPOLEINPLACEACTIVEOBJECT pActiveObject);
void OleInPlaceContainerAppDoNew(struct OleApplicationRec* pOleApp);
Boolean OleInPlaceContainerAppDoSuspend(struct OleApplicationRec* pOleApp);
Boolean OleInPlaceContainerAppDoResume(struct OleApplicationRec* pOleApp);
#ifndef _MSC_VER
pascal OSErr OleInPlaceContainerRemoteEvent(AppleEvent* theAppleEvent, AppleEvent* reply, struct OleApplicationRec* pOleApp);
#else
OSErr __pascal OleInPlaceContainerRemoteEvent(AppleEvent* theAppleEvent, AppleEvent* reply, struct OleApplicationRec* pOleApp);
#endif


#endif  // qOleInPlace

// OLDNAME: OleContainerApp.h

#define kMaxNumOfMTS		10

struct OleContainerAppRec {
	FORMATETC						m_arrSingleObjGetFmts[kMaxNumOfMTS];	// no. formats avail when single obj copied
	short							m_nSingleObjGetFmts;
                                        // array of FormatEtc's available via
                                        // IDataObject::GetData when a single
                                        // OLE object is copied.

#if qOleInPlace
	OleInPlaceContainerAppRec		inplace;
#endif
};

typedef struct OleContainerAppRec OleContainerAppRec, *OleContainerAppPtr;

void OleContainerAppInit(struct OleApplicationRec* pOleApp);
void OleContainerAppDispose(struct OleApplicationRec* pOleApp);
void OleContainerAppAddSingleObjGetFormat(struct OleApplicationRec* pOleApp, LPFORMATETC pFormatetc);

HRESULT OleContainerAppLocalQueryInterface(struct OleApplicationRec* pOleApp, REFIID riid, void* * lplpvObj);


// OLDNAME: OleOutlineContainer.h

void OleOutlineContainerInit(struct OleOutlineAppRec* pOleOutlineApp, OSType creator);


#endif // __OLECONTAINERAPP_H__
