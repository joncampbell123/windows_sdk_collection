// dataobj.h : header file
//

#ifndef __DATAOBJ_H__
#define __DATAOBJ_H__

#include "iviewer.h"
/////////////////////////////////////////////////////////////////////////////
// CDataObjectViewer command target

class CDataObjectViewer : public CInterfaceViewer
{
	DECLARE_DYNCREATE(CDataObjectViewer)
protected:
	CDataObjectViewer();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataObjectViewer)
	//}}AFX_VIRTUAL
    virtual HRESULT OnView(HWND hwndParent, REFIID riid, LPUNKNOWN punk);

// Implementation
protected:
	virtual ~CDataObjectViewer();

	// Generated message map functions
	//{{AFX_MSG(CDataObjectViewer)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CDataObjectViewer)
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CDataObjectDlg dialog

class CDataObjectDlg : public CDialog
{
// Construction
public:
	CDataObjectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDataObjectDlg)
	enum { IDD = IDD_IDATAOBJDLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataObjectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDataObjectDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif // __DATAOBJ_H__

