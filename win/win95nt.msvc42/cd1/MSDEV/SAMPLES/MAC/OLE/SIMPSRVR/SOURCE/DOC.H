#ifndef _CDOCUMENT_H_
#define _CDOCUMENT_H_

#include "CWindow.h"
#include "ole2.h"

enum OleDocumentType {
	kNewDocumentType,				// init from scratch (new doc)
	kEmbeddedDocumentType			// init from an IStorage* of an embedded obj
};

class 	CShape;
class	COleObject;
class	CDataObject;
class	CPersistStorage;

class CDocument : public CWindow
{
	public: 

	unsigned long		m_cRef;
	OleDocumentType		m_DocType;
	COleObject *		m_OleObject;
	CDataObject *		m_DataObject;
	CPersistStorage	*	m_PersistStorage;
	LPSTORAGE			m_lpStorage;
	LPSTREAM			m_lpStream;
	Boolean				m_fSaveWithSameAsLoad;
	Boolean				m_fNoScribbleMode;
	Boolean				m_fSendDataOnStop;
	Boolean				m_fIsClosing;
	LPOLECLIENTSITE		m_OleClientSite;
	LPOLEADVISEHOLDER	m_OleAdviseHolder;
	LPDATAADVISEHOLDER	m_DataAdviseHolder;
	SIZEL				m_sizel;
	Boolean				m_fVisible;
	CShape *			m_Shapes;

    // *** IUnknown methods ***
    virtual HRESULT STDMETHODCALLTYPE	QueryInterface(REFIID riid, void * * ppvObj);
    virtual unsigned long STDMETHODCALLTYPE		AddRef(void);
    virtual unsigned long STDMETHODCALLTYPE		Release(void);

	CDocument(void);
	~CDocument(void);

	OSErr			InitDocument		(void);

	void			OleSaveToStorage	(void);
	void			OleLoadFromStorage	(void);
	void			OleSendOnDataChange	(void);
	void			OleSendOnClose		(void);
	PicHandle		OleGetPict			(void);

	HRESULT			OleLockDocument		(Boolean fLock, Boolean fLastUnlockReleases);

	virtual void 	HandleClose			(void);

	void			ShowDocument		(void);
	void			HideDocument		(void);

	void			HiliteWindow		(Boolean hilite);

	void			ClipContents		(Boolean fClip);

	virtual void 	AdjustCursor		(void);
	virtual void	DrawContents		(void);

	virtual	void	OnWindowResize		(void);
	virtual void	HandleContentClick	(Point pt);

	void			DeleteLast			(void);
};

#endif // _CDOCUMENT_H_
