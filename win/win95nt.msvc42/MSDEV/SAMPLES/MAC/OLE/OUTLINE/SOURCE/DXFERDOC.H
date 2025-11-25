/*****************************************************************************\
*                                                                             *
*    OleDataXferDoc.h                                                         *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#ifndef __DXFERDOC_H__
#define __DXFERDOC_H__

// OLDNAME: OleDataXferDocInterface.h
struct DocDropSourceImpl {
	IDropSourceVtbl*			lpVtbl;
	struct OleDocumentRec*		lpOleDoc;
	unsigned long				cRef;			// interface specific ref count
};

typedef struct DocDropSourceImpl DocDropSourceImpl, *DocDropSourceImplPtr;

void OleDataXferDocInitInterfaces(void);


#if defined(__PPCC__) || defined(__MWERKS__)
struct DocDataObjectImpl;
#endif


void DataXferDocIDataObjectInit(struct DocDataObjectImpl * pDocDataObjectImpl, struct OleDocumentRec* pOleDoc);

// OleDataXferDoc::IDataObject methods
STDMETHODIMP OleDataXferDocIntDataObjectQueryInterface(LPDATAOBJECT lpThis, REFIID riid, void* * lplpvObj);
STDMETHODIMP_(unsigned long) OleDataXferDocIntDataObjectAddRef(LPDATAOBJECT lpThis);
STDMETHODIMP_(unsigned long) OleDataXferDocIntDataObjectRelease (LPDATAOBJECT lpThis);
STDMETHODIMP OleDataXferDocIntDataObjectGetData(LPDATAOBJECT lpThis, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium);
STDMETHODIMP OleDataXferDocIntDataObjectGetDataHere(LPDATAOBJECT lpThis, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium);
STDMETHODIMP OleDataXferDocIntDataObjectQueryGetData(LPDATAOBJECT lpThis, LPFORMATETC lpFormatetc);
STDMETHODIMP OleDataXferDocIntDataObjectGetCanonicalFormatEtc(LPDATAOBJECT lpThis, LPFORMATETC lpformatetc, LPFORMATETC lpformatetcOut);
STDMETHODIMP OleDataXferDocIntDataObjectSetData(LPDATAOBJECT lpThis, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium, unsigned long fRelease);
STDMETHODIMP OleDataXferDocIntDataObjectEnumFormatEtc(LPDATAOBJECT lpThis, unsigned long dwDirection, LPENUMFORMATETC * lplpenumFormatEtc);
STDMETHODIMP OleDataXferDocIntDataObjectDAdvise(LPDATAOBJECT lpThis, FORMATETC * lpFormatetc, unsigned long advf, LPADVISESINK lpAdvSink, unsigned long * lpdwConnection);
STDMETHODIMP OleDataXferDocIntDataObjectDUnadvise(LPDATAOBJECT lpThis, unsigned long dwConnection);
STDMETHODIMP OleDataXferDocIntDataObjectEnumDAdvise(LPDATAOBJECT lpThis, LPENUMSTATDATA * lplpenumAdvise);

#if qOleDragDrop
void DataXferDocIDropSourceInit(DocDropSourceImplPtr pDocDropSourceImpl, struct OleDocumentRec* pOleDoc);

STDMETHODIMP OleDataXferDocIntDropSourceQueryInterface(LPDROPSOURCE lpThis, REFIID riid, void* * lplpvObj);
STDMETHODIMP_(unsigned long) OleDataXferDocIntDropSourceAddRef(LPDROPSOURCE lpThis);
STDMETHODIMP_(unsigned long) OleDataXferDocIntDropSourceRelease(LPDROPSOURCE lpThis);
STDMETHODIMP OleDataXferDocIntDropSourceQueryContinueDrag(LPDROPSOURCE lpThis, unsigned long fEscapePressed, unsigned long grfKeyState);
STDMETHODIMP OleDataXferDocIntDropSourceGiveFeedback(LPDROPSOURCE lpThis, unsigned long dwEffect);
#endif


// OLDNAME: OleDataXferDoc
STDMETHODIMP OleDataXferDocDataObjectGetData(struct OleDocumentRec* pOleDoc, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium);
STDMETHODIMP OleDataXferDocDataObjectGetDataHere(struct OleDocumentRec* pOleDoc, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium);
STDMETHODIMP OleDataXferDocDataObjectQueryGetData(struct OleDocumentRec* pOleDoc, LPFORMATETC lpFormatetc);
STDMETHODIMP OleDataXferDocDataObjectEnumFormatEtc( struct OleDocumentRec* pOleDoc, unsigned long dwDirection, LPENUMFORMATETC * lplpenumFormatEtc);
Handle OleDataXferDocGetObjectDescriptorData(struct OleDocumentRec* pOleDoc);

#endif	// __DXFERDOC_H__
