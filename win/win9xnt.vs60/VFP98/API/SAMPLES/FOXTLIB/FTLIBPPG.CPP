// FoxtlibPpg.cpp : Implementation of the CFoxtlibPropPage property page class.

#include "stdafx.h"
#include "foxtlib.h"
#include "FoxtlibPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CFoxtlibPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CFoxtlibPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CFoxtlibPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CFoxtlibPropPage, "FOXTLIB.FoxtlibPropPage.1",
	0x22852eeb, 0xb01b, 0x11cf, 0xb8, 0x26, 0, 0xa0, 0xc9, 0x5, 0x5d, 0x9e)


/////////////////////////////////////////////////////////////////////////////
// CFoxtlibPropPage::CFoxtlibPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CFoxtlibPropPage

BOOL CFoxtlibPropPage::CFoxtlibPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_FOXTLIB_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CFoxtlibPropPage::CFoxtlibPropPage - Constructor

CFoxtlibPropPage::CFoxtlibPropPage() :
	COlePropertyPage(IDD, IDS_FOXTLIB_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CFoxtlibPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CFoxtlibPropPage::DoDataExchange - Moves data between page and properties

void CFoxtlibPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CFoxtlibPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CFoxtlibPropPage message handlers
