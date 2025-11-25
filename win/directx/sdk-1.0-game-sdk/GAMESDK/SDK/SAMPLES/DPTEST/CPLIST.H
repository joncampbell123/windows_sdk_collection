// cplist.h : header file
//

#include <dplay.h>
/////////////////////////////////////////////////////////////////////////////
// cplist dialog

class cplist : public CDialog
{
// Construction
public:
    cplist(CWnd* pParent = NULL);   // standard constructor
        DWORD m_dwSession;


// Dialog Data
    //{{AFX_DATA(cplist)
    enum { IDD = IDD_PLAYERS };
    CListBox    m_pList;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(cplist)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(cplist)
    virtual void OnCancel();
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};
