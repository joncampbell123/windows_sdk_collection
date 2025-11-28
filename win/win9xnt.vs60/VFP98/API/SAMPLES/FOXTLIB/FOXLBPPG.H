// FoxtlibPpg.h : Declaration of the CFoxtlibPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CFoxtlibPropPage : See FoxtlibPpg.cpp.cpp for implementation.

class CFoxtlibPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CFoxtlibPropPage)
	DECLARE_OLECREATE_EX(CFoxtlibPropPage)

// Constructor
public:
	CFoxtlibPropPage();

// Dialog Data
	//{{AFX_DATA(CFoxtlibPropPage)
	enum { IDD = IDD_PROPPAGE_FOXTLIB };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CFoxtlibPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
