// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.


// class CRectItem - main COleClientItem bounded by a rectangle

class CMainDoc;
class CMainView;

class CRectItem : public COleClientItem
{
	DECLARE_SERIAL(CRectItem)
	CRectItem();

public:
	CRectItem(COleDocument* pContainer);
	~CRectItem();

// Attributes
	CRect m_rect;           // where the item is on the window (.01 logical inches)
	CSize m_extent;         // last extent from server -- in device

	CMainDoc* GetDocument()
		{ return (CMainDoc*)COleClientItem::GetDocument(); }
	CMainView* GetActiveView()
		{ return (CMainView*)COleClientItem::GetActiveView(); }

// Operations
	void Dirty()
		{ GetDocument()->SetModifiedFlag(); }
	void Invalidate(CView* pNotThisView = NULL);
	BOOL UpdateExtent();
	void Move(CRect &rc);
	void ResetSize();
	void GetNativeClipboardData(COleDataSource *pDataSource);

	virtual void Serialize(CArchive& ar); // from CObject - public to call directly

// Overridables
protected:
	virtual void OnChange(OLE_NOTIFICATION wNotification, DWORD dwParam);
	virtual BOOL OnChangeItemPosition(const CRect& rectPos);
	virtual void OnActivate();
	virtual COleDataSource* OnGetClipboardData(BOOL bIncludeLink,
		LPPOINT lpOffset, LPSIZE lpSize);
	virtual void OnDeactivateUI(BOOL bUndoable);

public:
	virtual void OnGetItemPosition(CRect& rPosition);
};
