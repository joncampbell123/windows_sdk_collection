// view.h
//

#ifndef _VIEW_H_
#define _VIEW_H_

class CDropTargetView : public CView
{
public:
    CDropTargetView();
    DECLARE_DYNAMIC(CDropTargetView)

// Attributes
public:
    COle2ViewDoc* GetDocument();
    CListBox      m_lb ;

#if _MFC_VER > 0x0210
    COleDropTarget  m_dropTarget ;

// Overridables
public:
    virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject,
        DWORD dwKeyState, CPoint point);
    virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject,
        DWORD dwKeyState, CPoint point);
    virtual void OnDragLeave();
    virtual BOOL OnDrop(COleDataObject* pDataObject,
        DROPEFFECT dropEffect, CPoint point);
#endif
// Generated message map functions
protected:
    //{{AFX_MSG(CDropTargetView)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in vw.cpp
inline COle2ViewDoc* CDropTargetView::GetDocument()
   { return (COle2ViewDoc*) m_pDocument; }
#endif

#endif // _VIEW_H_
