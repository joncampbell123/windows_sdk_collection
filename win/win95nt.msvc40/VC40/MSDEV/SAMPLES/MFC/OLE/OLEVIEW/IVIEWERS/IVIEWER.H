// iviewer.h : header file
//

#ifndef __IVIEWER_H__
#define __IVIEWER_H__
/////////////////////////////////////////////////////////////////////////////
// CInterfaceViewer command target

class CInterfaceViewer : public CCmdTarget
{
public:
	DECLARE_DYNAMIC(CInterfaceViewer)
	CInterfaceViewer();           // protected constructor used by dynamic creation
	virtual ~CInterfaceViewer();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInterfaceViewer)
	//}}AFX_VIRTUAL
    virtual HRESULT OnView(HWND hwndParent, REFIID riid, LPUNKNOWN punk);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInterfaceViewer)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

    // Implemented interfaces
    //
public:
	DECLARE_MESSAGE_MAP()

	// IInterfaceViewer
    BEGIN_INTERFACE_PART(InterfaceViewer, IInterfaceViewer)
        STDMETHOD(View)(HWND hwndParent, REFIID riid, LPUNKNOWN punk);
    END_INTERFACE_PART(InterfaceViewer)    
    DECLARE_INTERFACE_MAP()

};

#endif // __IVIEWER_H__
