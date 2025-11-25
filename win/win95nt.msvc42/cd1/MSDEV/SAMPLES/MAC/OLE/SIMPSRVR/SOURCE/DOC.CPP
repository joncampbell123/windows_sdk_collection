#include "hdrs.h"


ASSERTDATA

#define 	WND_OFFSET		7


CDocument::CDocument(void)
{
	m_cRef	    	  		= 0;
	m_DocType				= kNewDocumentType;
	m_OleObject    	 		= NULL;
	m_DataObject   	 		= NULL;
	m_PersistStorage  		= NULL;
	m_lpStorage				= NULL;
	m_lpStream				= NULL;
	m_fSaveWithSameAsLoad 	= false;
	m_fNoScribbleMode	 	= false;
	m_fSendDataOnStop		= false;
	m_fIsClosing			= false;
	m_OleClientSite   		= NULL;
	m_OleAdviseHolder		= NULL;
	m_DataAdviseHolder		= NULL;
	m_fVisible              = false;
	m_Shapes                = NULL;
}


CDocument::~CDocument(void)
{
	gApp->OleAppDocUnlock();

	if (m_DataAdviseHolder)
		OleStdRelease((LPUNKNOWN)m_DataAdviseHolder);

	if (m_OleAdviseHolder)
		OleStdRelease((LPUNKNOWN)m_OleAdviseHolder);

	if (m_OleClientSite)
		OleStdRelease((LPUNKNOWN)m_OleClientSite);

	if (m_DataObject)
		delete m_DataObject;

	if (m_OleObject)
		delete m_OleObject;

	if (m_PersistStorage)
		delete m_PersistStorage;
}


OSErr CDocument::InitDocument(void)
{
	OSErr		result;
	char		strTitle[25];
	short		x, y;

	result = InitWindow(kObjectWindow, false);
	if (result != noErr)
		return result;

	if (++gApp->m_nWndCount == 1)
		strcpy(strTitle, "untitled");
	else
		sprintf(strTitle, "untitled %ld", gApp->m_nWndCount);
	c2pstr(strTitle);
	SetWTitle(m_pWindow, (StringPtr)strTitle);

	x = gApp->m_nWndCount * WND_OFFSET,
	y = x + 35;
	if (y > (qd.screenBits.bounds.bottom - 5)) // window tiling algorithm
	{
		x -= (WND_OFFSET * (gApp->m_nWndCount - 2));
		y -= (qd.screenBits.bounds.bottom - 35);
	}

	MoveWindow(m_pWindow, x, y, false);
	OnWindowResize();

	m_DataObject = new CDataObject(this);
	ASSERTSZ(m_DataObject != NULL, "Could not instantiate IDataObject.");

	m_OleObject = new COleObject(this);
	ASSERTSZ(m_OleObject != NULL, "Could not instantiate IOleObject.");

	m_PersistStorage = new CPersistStorage(this);
	ASSERTSZ(m_PersistStorage != NULL, "Could not instantiate IPersistStorage.");

	gApp->OleAppDocLock();

	gApp->SelectTheWindow(m_pWindow);

	return noErr;
}


HRESULT STDMETHODCALLTYPE CDocument::QueryInterface(REFIID riid, void * * ppvObj)
{
	*ppvObj = NULL;

	// Note: Use IsEqualIID to compare IIDs if you're using C

	if (riid == IID_IUnknown)
	{
		AddRef();
		*ppvObj = this;
		return ResultFromScode(S_OK);
	}

	if (riid == IID_IOleObject)
	{
		m_OleObject->AddRef();
		*ppvObj = m_OleObject;
		return ResultFromScode(S_OK);
	}

	if (riid == IID_IDataObject)
	{
		m_DataObject->AddRef();
		*ppvObj = m_DataObject;
		return ResultFromScode(S_OK);
	}

	if (riid == IID_IPersistStorage || riid == IID_IPersist)
	{
		m_PersistStorage->AddRef();
		*ppvObj = m_PersistStorage;
		return ResultFromScode(S_OK);
	}

	// request for unsupported interface

	return ResultFromScode(E_NOINTERFACE);
}


unsigned long STDMETHODCALLTYPE CDocument::AddRef(void)
{
	++m_cRef;

	DUMP_REFCOUNT("CDocument::AddRef",m_cRef);

	return m_cRef;
}


unsigned long STDMETHODCALLTYPE CDocument::Release(void)
{
	--m_cRef;
	ASSERT(m_cRef >= 0);

	DUMP_REFCOUNT("CDocument::Release",m_cRef);

	if (m_cRef == 0)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}


HRESULT CDocument::OleLockDocument(Boolean fLock, Boolean fLastUnlockReleases)
{
	HRESULT		hrErr;

	AddRef();			// artificial AddRef to make object stable

	hrErr = CoLockObjectExternal(this, fLock, fLastUnlockReleases);
	ASSERT(hrErr == NOERROR);

	Release();			// release artificial AddRef above;

	return hrErr;
}


void CDocument::OleSaveToStorage(void)
{
	HRESULT			hrErr;
	ULARGE_INTEGER	uli;
	LARGE_INTEGER	li;
	CShape * 		pShape;
	long			cShapes = 0;

	hrErr = WriteFmtUserTypeStg(m_lpStorage, kSimpleServerDocumentFileType, kSimpleServerFullUserTypeName);
	ASSERT(hrErr == NOERROR);

	uli.LowPart = 0;
	uli.HighPart = 0;
	m_lpStream->SetSize(uli);

	li.LowPart = 0;
	li.HighPart = 0;
	m_lpStream->Seek(li, STREAM_SEEK_SET, NULL);

	for (pShape = m_Shapes; pShape; pShape = pShape->m_Next)
		cShapes++;

	m_lpStream->Write(&cShapes, sizeof(cShapes), NULL);

	for (pShape = m_Shapes; pShape; pShape = pShape->m_Next)
		m_lpStream->Write(pShape, sizeof(*pShape), NULL);

	m_lpStream->Commit(STGC_DEFAULT);
}

void CDocument::OleLoadFromStorage(void)
{
	long		index,
				cShapes;
	CShape *	pShape;
	CShape *	pLast;

	m_lpStream->Read(&cShapes, sizeof(cShapes), NULL);

	ASSERT(m_Shapes == NULL);

	if (cShapes != 0)
	{
		m_Shapes = new CShape(NO_SHAPE);
		ASSERTSZ(m_Shapes != NULL, "Could not instantiate shape.");
	}

	if (m_Shapes)
		pShape = m_Shapes;

	for (index = 0; index < cShapes; index++)
	{
		m_lpStream->Read(pShape, sizeof(*pShape), NULL);
		pShape->m_Next = new CShape(NO_SHAPE);
		ASSERTSZ(pShape->m_Next != NULL, "Could not instantiate shape.");

		pLast = pShape;
		pShape = pShape->m_Next;

		if (!pShape)
			break;
	}

	if (pLast) pLast->m_Next = NULL;
	if (pShape) delete pShape;

	SetPort(m_pWindow);
	InvalRect(&m_pWindow->portRect);
}


void CDocument::OleSendOnDataChange(void)
{
	if (m_DataAdviseHolder)
		m_DataAdviseHolder->SendOnDataChange((LPDATAOBJECT)m_DataObject, 0, 0);
}


void CDocument::OleSendOnClose(void)
{
	if (m_OleAdviseHolder)
		m_OleAdviseHolder->SendOnClose();
}


PicHandle CDocument::OleGetPict(void)
{
	CShape *	pShape;
	PicHandle	hPict;

	hPict = OpenPicture(&m_pWindow->portRect);

	pShape = m_Shapes;

	while (pShape)
	{
		pShape->Draw();
		pShape = pShape->m_Next;
	}

	ClosePicture();

	return hPict;
}


void CDocument::HandleClose(void)
{
	AddRef();			// artificial addref

	ASSERTCOND(m_fIsClosing == false);
	m_fIsClosing = true;

	if (m_OleClientSite && (m_DocType == kEmbeddedDocumentType))
		m_OleClientSite->SaveObject();

	HideDocument();

	if (m_fSendDataOnStop && m_DataAdviseHolder)
	{
		OleSendOnDataChange();
		m_fSendDataOnStop = false;
	}

	if (m_OleAdviseHolder)
		OleSendOnClose();

	m_fIsClosing = false;

	Release();			// artificial release

	gApp->UpdateFrontDocument();
}


void CDocument::ShowDocument(void)
{
	if (m_fVisible)
		return;

	m_fVisible = true;

	OleLockDocument(true, false);

	// This is where we would RegisterDragDrop

	if (m_OleClientSite && (m_DocType == kEmbeddedDocumentType))
	{
		m_OleClientSite->ShowObject();
		m_OleClientSite->OnShowWindow(true);
	}

	ShowWindow(m_pWindow);
}


void CDocument::HideDocument(void)
{
	if (!m_fVisible)
		return;

	m_fVisible = false;

	HideWindow(m_pWindow);

	// This is where we would RevokeDragDrop

	OleLockDocument(false, m_fIsClosing);

	if (m_OleClientSite)
		m_OleClientSite->OnShowWindow(false);
}


void CDocument::ClipContents(Boolean fClip)
{
	Rect	rc;

	if (fClip)
	{
		rc = m_pWindow->portRect;
		rc.right  -= 15;
		rc.bottom -= 15;
	}
	else
	{
		rc.top = rc.left = -32767;
		rc.bottom = rc.right = 32767;
	}

	ClipRect(&rc);
}


void CDocument::HiliteWindow(Boolean hilite)
{
	//inherited::HiliteWindow(hilite);
	CWindow::HiliteWindow(hilite);

	SetPort(m_pWindow);
	DrawGrowIcon(m_pWindow);
}


void CDocument::AdjustCursor(void)
{
	if (gApp->m_toolbar->m_Tool == NO_TOOL)
		SetCursor(&qd.arrow);
	else
	{
		Point	pt;
		Rect	rc;

		SetPort(m_pWindow);
		GetMouse(&pt);

		rc = m_pWindow->portRect;
		rc.right  -= 15;
		rc.bottom -= 15;

		if (PtInRect(pt, &rc))
			SetCursor(&gApp->m_crosshair);
		else
			SetCursor(&qd.arrow);
	}
}


void CDocument::DrawContents(void)
{
	CShape *	pShape;

	EraseRect(&m_pWindow->portRect);
	DrawGrowIcon(m_pWindow);

	ClipContents(true);

	pShape = m_Shapes;
	if (pShape)
	{
		while (pShape->m_Next)
		{
			pShape->Draw();
			pShape = pShape->m_Next;
		}

		pShape->Draw();
	}

	ClipContents(false);
}


void CDocument::OnWindowResize(void)
{
	m_sizel.cx = m_pWindow->portRect.right;
	m_sizel.cy = m_pWindow->portRect.bottom;

	OleSendOnDataChange();
}


void CDocument::HandleContentClick(Point pt)
{
	CShape * 	pShape;
	CShape *	pNewShape;

	SetPort(m_pWindow);

	pShape = m_Shapes;

	if (!pShape)
	{ 
		pNewShape = new CShape(gApp->m_toolbar->m_Tool);
		ASSERTSZ(pNewShape != NULL, "Could not instantiate shape.");
		m_Shapes = pNewShape;
	}
	else
	{
		while (pShape->m_Next)
			pShape = pShape->m_Next;
		pNewShape = new CShape(gApp->m_toolbar->m_Tool);
		ASSERTSZ(pNewShape != NULL, "Could not instantiate shape.");
		pShape->m_Next = pNewShape;
	}

	ClipContents(true);

	GlobalToLocal(&pt);
	pNewShape->TrackShape(pt);

	ClipContents(false);

	OleSendOnDataChange();
}


void CDocument::DeleteLast(void)
{
	CShape *	pCur;
	CShape *	pLast;
	Boolean		fLast = false;
	Rect		r;

	pCur  = m_Shapes;
	pLast = pCur;

	if (m_Shapes->m_Next)
	{
		while (pCur->m_Next)
		{
			pLast = pCur;
			pCur  = pCur->m_Next;
		}

		pLast->m_Next = NULL;
	}
	else
		fLast = true;


	SetPort(m_pWindow);

	r = pCur->m_Bounds;

	switch(pCur->m_Type)
	{
		case LINE:
			PenPat(&qd.white);
			MoveTo(r.left, r.top);
			LineTo(r.right, r.bottom);
			PenNormal();
			break;

		case SQUARE:
			EraseRect(&r);
			break;

		case CIRCLE:
			EraseOval(&r);
			break;
	}

	if (pCur->m_Type == LINE)
	{
		Rect	newRect;

		newRect = r;

		newRect.right  = MAX(r.right, r.left);
		newRect.left   = MIN(r.right, r.left);
		newRect.top    = MIN(r.top, r.bottom);
		newRect.bottom = MAX(r.top, r.bottom);

		r = newRect;

		InsetRect(&r, -1, -1);
	}

	InvalRect(&r);

	delete pCur;

	if (fLast)
		m_Shapes = NULL;

	OleSendOnDataChange();
}
