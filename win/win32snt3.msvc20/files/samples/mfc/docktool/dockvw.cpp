// dockvw.cpp : implementation of the CDockView class
//

#include "stdafx.h"
#include "docktool.h"

#include "dockdoc.h"
#include "dockvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDockView

IMPLEMENT_DYNCREATE(CDockView, CView)

BEGIN_MESSAGE_MAP(CDockView, CView)
	//{{AFX_MSG_MAP(CDockView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDockView construction/destruction

CDockView::CDockView()
{
	// TODO: add construction code here

}

CDockView::~CDockView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CDockView drawing

void CDockView::OnDraw(CDC* pDC)
{
	CDockDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDC;    // unused right now

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CDockView diagnostics

#ifdef _DEBUG
void CDockView::AssertValid() const
{
	CView::AssertValid();
}

void CDockView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDockDoc* CDockView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDockDoc)));
	return (CDockDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDockView message handlers
