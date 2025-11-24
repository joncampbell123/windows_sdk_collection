//---------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation 1995
//
//---------------------------------------------------------------------------

#include "priv.h"
#define INITGUID
#include <initguid.h>
#include "shesamp.h"

//
// Global variables
//
UINT g_cRefThisDll = 0;		// Reference count of this DLL.
UINT g_cfNetResource = 0;	// Clipboard format
HANDLE g_hmodThisDll = NULL;	// Handle to this DLL itself.

//
// Function prototypes
//
HRESULT CALLBACK ShellExtSample_CreateInstance(LPUNKNOWN, REFIID, LPVOID *);



//---------------------------------------------------------------------------
// LibMain
//---------------------------------------------------------------------------
BOOL APIENTRY LibMain(HANDLE hDll, DWORD dwReason, LPVOID lpReserved)
{
    switch(dwReason)
    {
    case DLL_PROCESS_ATTACH:
	g_hmodThisDll = hDll;
    	break;
    case DLL_PROCESS_DETACH:
	  break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_THREAD_ATTACH:
    default:
    	break;
    } // end switch()

    return TRUE;
}

//---------------------------------------------------------------------------
// DllCanUnloadNow
//---------------------------------------------------------------------------

STDAPI DllCanUnloadNow(void)
{
    return (g_cRefThisDll == 0) ? S_OK : S_FALSE;
}

//=========================================================================
// CDefClassFactory class ... From defclsf.c
//=========================================================================
typedef struct
{
    IClassFactory      cf;		
    UINT               cRef;		// Reference count
    LPFNCREATEINSTANCE lpfnCI;		// CreateInstance callback entry
    UINT *         pcRefDll;	// Reference count of the DLL
    const IID *    riidInst;		// Optional interface for instance
} CDefClassFactory;

extern CDefClassFactory * CDefClassFactory_Create(LPFNCREATEINSTANCE lpfnCI, UINT * pcRefDll, REFIID riidInst);

//---------------------------------------------------------------------------
//
// DllGetClassObject
//
//  This is the entry of this DLL, which all the In-Proc server DLLs should
// export. See the description of "DllGetClassObject" of OLE 2.0 reference
// manual for detail.
//
//---------------------------------------------------------------------------

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID * ppvOut)
{
    *ppvOut = NULL; // Assume Failure

    if (IsEqualIID(rclsid, &CLSID_ShellExtSample))
    {
        if (IsEqualIID(riid, &IID_IClassFactory)
    	   || IsEqualIID(riid, &IID_IUnknown))
        {
            CDefClassFactory * pacf = CDefClassFactory_Create(
    						 ShellExtSample_CreateInstance,
    						 &g_cRefThisDll,
    						 NULL);
    	    if (pacf)
    	    {
    		(IClassFactory *)*ppvOut = &pacf->cf;
    		return NOERROR;
    	    }
    	    return E_OUTOFMEMORY;
        }
        return E_NOINTERFACE;
    }
    else
    {
    	return CLASS_E_CLASSNOTAVAILABLE;
    }
}


//---------------------------------------------------------------------------
//
// CShellExtSample class
//
//---------------------------------------------------------------------------
typedef struct _CShellExtSample  // smx
{
	IExtractIcon		_exi;
	IPersistFile		_psf;
	IContextMenu		_ctm;
	IShellExtInit		_sxi;
	IShellPropSheetExt	_spx;
	int 			_cRef;		// reference count
	LPDATAOBJECT		_pdtobj;	// data object
	HKEY			_hkeyProgID;	// reg. database key to ProgID
	char			_szFile[MAX_PATH];
} CShellExtSample, * PSHELLEXTSAMPLE;

#define SMX_OFFSETOF(x)		((UINT)(&((PSHELLEXTSAMPLE)0)->x))
#define PVOID2PSMX(pv,offset)	((PSHELLEXTSAMPLE)(((LPBYTE)pv)-offset))

#define PEXI2PSMX(pexi)		PVOID2PSMX(pexi, SMX_OFFSETOF(_exi))
#define PPSF2PSMX(ppsf)		PVOID2PSMX(ppsf, SMX_OFFSETOF(_psf))
#define PCTM2PSMX(pctm)	        PVOID2PSMX(pctm, SMX_OFFSETOF(_ctm))
#define PSXI2PSMX(psxi)	        PVOID2PSMX(psxi, SMX_OFFSETOF(_sxi))
#define PSPX2PSMX(pspx)	        PVOID2PSMX(pspx, SMX_OFFSETOF(_spx))

//
// Vtable prototype
//
extern IExtractIconVtbl        c_ShellExtSample_EXIVtbl;
extern IPersistFileVtbl        c_ShellExtSample_PSFVtbl;
extern IContextMenuVtbl        c_ShellExtSample_CTMVtbl;
extern IShellExtInitVtbl       c_ShellExtSample_SXIVtbl;
extern IShellPropSheetExtVtbl  c_ShellExtSample_SPXVtbl;

//---------------------------------------------------------------------------
//
// ShellExtSample_CreateInstance
//
//  This function is called back from within IClassFactory::CreateInstance()
// of the default class factory object, which is created by CreateClassObject.
//
//---------------------------------------------------------------------------

HRESULT CALLBACK ShellExtSample_CreateInstance(LPUNKNOWN punkOuter, REFIID riid, LPVOID * ppvOut)
{
    HRESULT hres;
    PSHELLEXTSAMPLE psmx;

    *ppvOut = NULL;		// assume error

    //
    // Shell extentions typically does not support aggregation.
    //
    if (punkOuter)
	return CLASS_E_NOAGGREGATION;

    //
    // in C++:
    //	psmx = new CShellExtSample();
    //
    psmx = LocalAlloc(LPTR, sizeof(CShellExtSample));
    if (psmx)
    {
    	psmx->_exi.lpVtbl = &c_ShellExtSample_EXIVtbl;
    	psmx->_psf.lpVtbl = &c_ShellExtSample_PSFVtbl;
    	psmx->_ctm.lpVtbl = &c_ShellExtSample_CTMVtbl;
    	psmx->_sxi.lpVtbl = &c_ShellExtSample_SXIVtbl;
    	psmx->_spx.lpVtbl = &c_ShellExtSample_SPXVtbl;
    	psmx->_cRef = 1;
    	psmx->_pdtobj = NULL;
    	psmx->_hkeyProgID = NULL;

    	g_cRefThisDll++;

    	//
    	// in C++:
    	//  hres = psmx->QueryInterface(riid, ppvOut);
    	//  psmx->Release();
    	//
    	// Note that the Release member will free the object, if QueryInterface
    	// failed.
	//

    	if (IsEqualIID(riid, &IID_IPersistFile))
    	{
    	    hres = c_ShellExtSample_EXIVtbl.QueryInterface(&psmx->_exi, riid, ppvOut);
    	    c_ShellExtSample_EXIVtbl.Release(&psmx->_exi);
    	}
    	else if (IsEqualIID(riid, &IID_IShellExtInit))
    	{
    	    hres = c_ShellExtSample_CTMVtbl.QueryInterface(&psmx->_ctm, riid, ppvOut);
    	    c_ShellExtSample_CTMVtbl.Release(&psmx->_ctm);
    	}
    	else
    	{
    	    hres = E_NOINTERFACE;
    	}
    }
    else
    {
	hres = E_OUTOFMEMORY;
    }

    return hres;	// S_OK or E_NOINTERFACE
}

//---------------------------------------------------------------------------
//
//	Shell Extension Sample's IExtractIcon Interface
//
//---------------------------------------------------------------------------

STDMETHODIMP SHE_ExtractIcon_GetIconLocation(IExtractIcon * pexic,
		     UINT   uFlags,
		     LPSTR  szIconFile,
		     UINT   cchMax,
		     int  * piIndex,
		     UINT * pwFlags)
{
    PSHELLEXTSAMPLE this = PEXI2PSMX(pexic);
    if (this->_szFile[0])
    {
	GetPrivateProfileString("IconImage", "FileName", "shell32.dll",
				szIconFile, cchMax, this->_szFile);
	*piIndex = (int)GetPrivateProfileInt("IconImage", "Index", 0, this->_szFile);
    }
    else
    {
	lstrcpy(szIconFile, "shell32.dll");
	*piIndex = -10;
    }
    *pwFlags = 0;
    return NOERROR;
}

STDMETHODIMP SHE_ExtractIcon_ExtractIcon(IExtractIcon * pexic,
		       LPCSTR pszFile,
		       UINT   nIconIndex,
		       HICON  *phiconLarge,
		       HICON  *phiconSmall,
		       UINT   uIconSize)
{
    return S_FALSE;	// Force default extraction.
}

STDMETHODIMP_(UINT) SHE_ExtractIcon_AddRef(IExtractIcon * pexi)
{
    PSHELLEXTSAMPLE this = PEXI2PSMX(pexi);
    return ++this->_cRef;
}

STDMETHODIMP_(UINT) SHE_ExtractIcon_Release(IExtractIcon * pexi)
{
    PSHELLEXTSAMPLE this = PEXI2PSMX(pexi);
    if (InterlockedDecrement(&this->_cRef)) {
	return this->_cRef;
    }

    LocalFree((HLOCAL)this);
    g_cRefThisDll--;

    return 0;
}

STDMETHODIMP SHE_ExtractIcon_QueryInterface(IExtractIcon * pexi, REFIID riid, LPVOID * ppvOut)
{
    PSHELLEXTSAMPLE this = PEXI2PSMX(pexi);
    if (IsEqualIID(riid, &IID_IExtractIcon) || IsEqualIID(riid, &IID_IUnknown))
    {
	(IExtractIcon *)*ppvOut = pexi;
        this->_cRef++;
        return NOERROR;
    }
    else if (IsEqualIID(riid, &IID_IPersistFile))
    {
	(LPPERSISTFILE)*ppvOut = &this->_psf;
        this->_cRef++;
        return NOERROR;
    }
    else
    {
        *ppvOut = NULL;
        return E_NOINTERFACE;
    }
}


//---------------------------------------------------------------------------
//
//	Shell Extension Sample's IContextMenu Interface
//
//---------------------------------------------------------------------------

STDMETHODIMP SHE_ContextMenu_QueryContextMenu(LPCONTEXTMENU pctm,
                                               HMENU hmenu,
                                               UINT indexMenu,
                                               UINT idCmdFirst,
                                               UINT idCmdLast,
					       UINT uFlags)
{
    UINT idCmd = idCmdFirst;
    InsertMenu(hmenu, indexMenu++, MF_STRING|MF_BYPOSITION, idCmd++, "Check H&DROP (SHESamp)");
    InsertMenu(hmenu, indexMenu++, MF_STRING|MF_BYPOSITION, idCmd++, "Check H&NRES (SHESamp)");
    return MAKE_SCODE(SEVERITY_SUCCESS, FACILITY_NULL, (USHORT)2);
}

HRESULT SHE_ContextMenu_CheckForHDROP(PSHELLEXTSAMPLE this, HWND hwnd)
{
    FORMATETC fmte = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
    STGMEDIUM medium;
    HRESULT hres = this->_pdtobj->lpVtbl->GetData(this->_pdtobj, &fmte, &medium);

    if (SUCCEEDED(hres))
    {
	HDROP hdrop = medium.hGlobal;
	UINT cFiles = DragQueryFile(hdrop, (UINT)-1, NULL, 0);
	char szFile[MAX_PATH];
	char szBuf[MAX_PATH+64];

	DragQueryFile(hdrop, 0, szFile, sizeof(szFile));
	wsprintf(szBuf,
		 "%d files/directories in HDROP\n"
		 "The path to the first object is\n"
   		 "\t%s.",
		 cFiles,
		 szFile);

	MessageBox(hwnd, szBuf, "Shell Extension Sample", MB_OK);

	//
	// HACK: We are supposed to call ReleaseStgMedium. This is a temporary
	//  hack until OLE 2.01 for Chicago is released.
	//
	if (medium.pUnkForRelease)
	{
	    medium.pUnkForRelease->lpVtbl->Release(medium.pUnkForRelease);
	}
	else
	{
	    GlobalFree(medium.hGlobal);
	}
    }
    else
    {
	MessageBox(hwnd, "No file system object in the selection", "Shell Extension Sample", MB_OK);
    }

    return hres;
}

//
// Converts an offset to a string to a string pointer.
//
LPCSTR _Offset2Ptr(LPSTR pszBase, UINT offset, UINT * pcb)
{
    LPSTR pszRet;
    if (offset==0) {
	pszRet = NULL;
	*pcb = 0;
    } else {
	pszRet = pszBase + offset;
	*pcb = lstrlen(pszRet) + 1;
    }
    return pszRet;
}

//
// This is a helper routine which extracts a specified NETRESOURCE from hnres.
//
UINT SHE_ContextMenu_GetNetResource(HGLOBAL hnres, UINT iItem, LPNETRESOURCE pnresOut, UINT cbMax)
{
    LPNRESARRAY panr = GlobalLock(hnres);
    UINT iRet = 0;	// assume error
    if (hnres)
    {
	if (iItem==(UINT)-1)
	{
	    iRet = panr->cItems;
	}
	else if (iItem < panr->cItems)
	{
	    UINT cbProvider, cbRemoteName;
	    LPCSTR pszProvider = _Offset2Ptr((LPSTR)panr, (UINT)panr->nr[iItem].lpProvider, &cbProvider);
	    LPCSTR pszRemoteName = _Offset2Ptr((LPSTR)panr, (UINT)panr->nr[iItem].lpRemoteName, &cbRemoteName);
	    iRet = sizeof(NETRESOURCE) + cbProvider + cbRemoteName;
	    if (iRet <= cbMax)
	    {
		LPSTR psz = (LPSTR)(pnresOut+1);
		*pnresOut = panr->nr[iItem];
		if (pnresOut->lpProvider) {
		    pnresOut->lpProvider = psz;
		    lstrcpy(psz, pszProvider);
		    psz += cbProvider;
		}
		if (pnresOut->lpRemoteName) {
		    pnresOut->lpRemoteName = psz;
		    lstrcpy(psz, pszRemoteName);
		}
	    }
	}
	GlobalUnlock(hnres);
    }
    return iRet;
}

HRESULT SHE_ContextMenu_CheckForHNRES(PSHELLEXTSAMPLE this, HWND hwnd)
{
    FORMATETC fmte = {g_cfNetResource ? g_cfNetResource
		    : (g_cfNetResource = RegisterClipboardFormat(CFSTR_NETRESOURCES)),
        	NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM medium;
    HRESULT hres = this->_pdtobj->lpVtbl->GetData(this->_pdtobj, &fmte, &medium);
    if (SUCCEEDED(hres))
    {
	HGLOBAL hnres = medium.hGlobal;
	LPNETRESOURCE pnr = (LPNETRESOURCE)LocalAlloc(LPTR, 1024);
	if (pnr)
	{
	    char szBuf[512];
	    UINT cItems = SHE_ContextMenu_GetNetResource(hnres, (UINT)-1, NULL, 0);

	    // Get the NETRESOURCE of the first item
	    SHE_ContextMenu_GetNetResource(hnres, 0, pnr, 1024);

	    wsprintf(szBuf,
		     "%d network resource objects in HNRES\n"
		     "The attributes of the first object are\n"
		     "\tProvider      = %s\n"
		     "\tRemoteName    = %s\n"
		     "\tdwDisplayType = %x\n"
		     "\tdwType        = %x\n"
		     "\tdwUsage       = %x\n",
		     cItems,
		     pnr->lpProvider,
		     pnr->lpRemoteName ? pnr->lpRemoteName : "N/A",
		     pnr->dwDisplayType,
		     pnr->dwType,
		     pnr->dwUsage);

	    MessageBox(hwnd, szBuf, "Shell Extension Sample", MB_OK);

	    LocalFree(pnr);
	}
	else
	{
	    hres = E_OUTOFMEMORY;
	}

	//
	// HACK: We are supposed to call ReleaseStgMedium. This is a temporary
	//  hack until OLE 2.01 for Chicago is released.
	//
	if (medium.pUnkForRelease)
	{
	    medium.pUnkForRelease->lpVtbl->Release(medium.pUnkForRelease);
	}
	else
	{
	    GlobalFree(medium.hGlobal);
	}
    }
    else
    {
	MessageBox(hwnd, "No network objects in the selection", "Shell Extension Sample", MB_OK);
    }
    return hres;
}

STDMETHODIMP SHE_ContextMenu_InvokeCommand(LPCONTEXTMENU pctm,
				   LPCMINVOKECOMMANDINFO lpici)
{
    PSHELLEXTSAMPLE this = PCTM2PSMX(pctm);
    HRESULT hres = E_INVALIDARG;	// assume error
    //
    // No need to support string based command.
    //
    if (!HIWORD(lpici->lpVerb))
    {
	UINT idCmd = LOWORD(lpici->lpVerb);

	switch(idCmd)
	{
	case 0:
	    hres = SHE_ContextMenu_CheckForHDROP(this, lpici->hwnd);
	    break;

	case 1:
	    hres = SHE_ContextMenu_CheckForHNRES(this, lpici->hwnd);
	    break;
	}
    }
    return hres;
}

STDMETHODIMP SHE_ContextMenu_GetCommandString(LPCONTEXTMENU pctm,
    UINT idCmd, UINT uFlags, UINT *pwReserved, LPSTR pszName, UINT cchMax)
{
    if ( uFlags & GCS_HELPTEXT )
    {
	switch(idCmd)
	{
	case 0:
	    lstrcpyn(pszName, "Display Drag Drop Properties", cchMax);
	    break;

	case 1:
	    lstrcpyn(pszName, "Display Network Properties", cchMax);
	    break;
	}
     }
     else
	   return NOERROR;
}
STDMETHODIMP_(UINT) SHE_ContextMenu_AddRef(LPCONTEXTMENU pctm)
{
    PSHELLEXTSAMPLE this = PCTM2PSMX(pctm);
    return ++this->_cRef;
}

STDMETHODIMP_(UINT) SHE_ContextMenu_Release(LPCONTEXTMENU pctm)
{
    PSHELLEXTSAMPLE this = PCTM2PSMX(pctm);
    if (--this->_cRef) {
	return this->_cRef;
    }

    if (this->_pdtobj) {
	this->_pdtobj->lpVtbl->Release(this->_pdtobj);
    }

    if (this->_hkeyProgID) {
	RegCloseKey(this->_hkeyProgID);
    }

    LocalFree((HLOCAL)this);
    g_cRefThisDll--;

    return 0;
}

STDMETHODIMP SHE_ContextMenu_QueryInterface(LPCONTEXTMENU pctm, REFIID riid, LPVOID * ppvOut)
{
    PSHELLEXTSAMPLE this = PCTM2PSMX(pctm);
    if (IsEqualIID(riid, &IID_IContextMenu) || IsEqualIID(riid, &IID_IUnknown))
    {
        (LPCONTEXTMENU)*ppvOut=pctm;
        this->_cRef++;
        return NOERROR;
    }
    else if (IsEqualIID(riid, &IID_IShellExtInit))
    {
        (LPSHELLEXTINIT)*ppvOut=&this->_sxi;
        this->_cRef++;
        return NOERROR;
    }
    else
    {
        *ppvOut = NULL;
        return E_NOINTERFACE;
    }
}


//---------------------------------------------------------------------------
//
//	Shell Extension Sample's IShellPropSheetExt Interface
//
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//
// FSPage_InitDialog
//
//  This function is called when the dialog procedure receives the
// WM_INITDIALOG message. In this sample code, we simply fill the
// listbox with the list of fully qualified paths to the file and
// directories.
//
//---------------------------------------------------------------------------

void FSPage_InitDialog(HWND hDlg, LPPROPSHEETPAGE psp)
{
    char szFile[MAX_PATH];
    UINT iFile;
    HDROP hdrop = (HDROP)psp->lParam;

    for (iFile = 0; DragQueryFile(hdrop, iFile, szFile, sizeof(szFile)); iFile++)
	SendDlgItemMessage(hDlg, lst1, LB_ADDSTRING, 0, (LPARAM)szFile);
}


//---------------------------------------------------------------------------
//
// FSPage_DlgProc
//
//  The dialog procedure for the "FSPage" property sheet page.
//
//---------------------------------------------------------------------------

BOOL CALLBACK FSPage_DlgProc(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
    LPPROPSHEETPAGE psp = (LPPROPSHEETPAGE)GetWindowLong(hDlg, DWL_USER);

    switch (uMessage)
    {
    //
    //  When the shell creates a dialog box for a property sheet page,
    // it passes the pointer to the PROPSHEETPAGE data structure as
    // lParam. The dialog procedures of extensions typically store it
    // in the DWL_USER of the dialog box window.
    //
    case WM_INITDIALOG:
        SetWindowLong(hDlg, DWL_USER, lParam);
	psp = (LPPROPSHEETPAGE)lParam;
	FSPage_InitDialog(hDlg, psp);
        break;

    case WM_DESTROY:
        break;

    case WM_COMMAND:
        break;

    case WM_NOTIFY:
        switch (((NMHDR *)lParam)->code)
        {
        case PSN_SETACTIVE:
            break;

        case PSN_APPLY:
            break;

        default:
            break;
        }
        break;

    default:
        return(FALSE);
    }

    return(TRUE);
}

//
//
//
UINT CALLBACK FSPage_CallbackProc(HWND hwnd, UINT uMsg, LPPROPSHEETPAGE ppsp)
{
    switch (uMsg) {
    case PSPCB_RELEASE:
        GlobalFree((HDROP)ppsp->lParam);
        return TRUE;
    }
    return TRUE;
}


STDMETHODIMP SHE_PageExt_AddPages(LPSHELLPROPSHEETEXT pspx,
		              LPFNADDPROPSHEETPAGE lpfnAddPage,
			      LPARAM lParam)
{
    PSHELLEXTSAMPLE this = PSPX2PSMX(pspx);
    //
    // Call IDataObject::GetData asking for a CF_HDROP (i.e., HDROP).
    //
    FORMATETC fmte = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
    STGMEDIUM medium;
    HRESULT hres = this->_pdtobj->lpVtbl->GetData(this->_pdtobj, &fmte, &medium);

    if (SUCCEEDED(hres))
    {
	//
	//  We need to make a copy of hdrop, because we can't hang on
	// to this medium.
	//
	UINT cbDrop = GlobalSize(medium.hGlobal);
	HDROP hdrop = GlobalAlloc(GPTR, cbDrop);
	if (hdrop)
	{
	    PROPSHEETPAGE psp;
	    HPROPSHEETPAGE hpage;

	    hmemcpy((LPSTR)hdrop, GlobalLock(medium.hGlobal), cbDrop);
	    GlobalUnlock(medium.hGlobal);

	    //
	    //  Create a property sheet page object from a dialog box.
	    //
	    //  We store the hdrop (a copy of medium.hGlobal) in lParam,
	    // because it is the only instance data we need.
	    //
	    //  If the page needs more instance data, you can append
	    // arbitrary size of data at the end of this structure,
	    // and pass it to the CreatePropSheetPage. In such a case,
	    // the size of entire data structure (including page specific
	    // data) must be stored in the dwSize field.
	    //
	    psp.dwSize      = sizeof(psp);	// no extra data.
	    psp.dwFlags     = PSP_USEREFPARENT | PSP_USECALLBACK;
	    psp.hInstance   = g_hmodThisDll;
	    psp.pszTemplate = MAKEINTRESOURCE(DLG_FSPAGE);
	    psp.pfnDlgProc  = FSPage_DlgProc;
	    psp.pcRefParent = &g_cRefThisDll;
	    psp.pfnCallback = FSPage_CallbackProc;
	    psp.lParam      = (LPARAM)hdrop;

	    hpage = CreatePropertySheetPage(&psp);
	    if (hpage) {
		if (!lpfnAddPage(hpage, lParam))
		    DestroyPropertySheetPage(hpage);
	    }
	}

	//
	// HACK: We are supposed to call ReleaseStgMedium. This is a temporary
	//  hack until OLE 2.01 for Chicago is released.
	//
	if (medium.pUnkForRelease)
	{
	    medium.pUnkForRelease->lpVtbl->Release(medium.pUnkForRelease);
	}
	else
	{
	    GlobalFree(medium.hGlobal);
	}
    }

    return NOERROR;
}

STDMETHODIMP_(UINT) SHE_PageExt_AddRef(LPSHELLPROPSHEETEXT pspx)
{
    PSHELLEXTSAMPLE this = PSPX2PSMX(pspx);
    return ++this->_cRef;
}
STDMETHODIMP_(UINT) SHE_PageExt_Release(LPSHELLPROPSHEETEXT pspx)
{
    PSHELLEXTSAMPLE this = PSPX2PSMX(pspx);
    if (--this->_cRef) {
	return this->_cRef;
    }

    if (this->_pdtobj) {
	this->_pdtobj->lpVtbl->Release(this->_pdtobj);
    }

    if (this->_hkeyProgID) {
	RegCloseKey(this->_hkeyProgID);
    }

    LocalFree((HLOCAL)this);
    g_cRefThisDll--;

    return 0;
}

STDMETHODIMP SHE_PageExt_QueryInterface(LPSHELLPROPSHEETEXT pspx, REFIID riid, LPVOID * ppvOut)
{
    PSHELLEXTSAMPLE this = PSPX2PSMX(pspx);

    if (IsEqualIID(riid, &IID_IShellPropSheetExt) || IsEqualIID(riid, &IID_IUnknown))
    {
        (LPSHELLPROPSHEETEXT)*ppvOut=pspx;
        this->_cRef++;
        return NOERROR;
    }
    else if (IsEqualIID(riid, &IID_IShellExtInit))
    {
        (LPSHELLEXTINIT)*ppvOut=&this->_sxi;
        this->_cRef++;
        return NOERROR;
    }
    else
    {
        *ppvOut = NULL;
        return E_NOINTERFACE;
    }
}


//---------------------------------------------------------------------------
//
//	Shell Extension Sample's IPersistFile Interface
//
//---------------------------------------------------------------------------
STDMETHODIMP SHE_PersistFile_GetClassID(LPPERSISTFILE pPersistFile, LPCLSID lpClassID)
{
    return E_FAIL;
}

STDMETHODIMP SHE_PersistFile_IsDirty(LPPERSISTFILE pPersistFile)
{
    return E_FAIL;
}

STDMETHODIMP SHE_PersistFile_Load(LPPERSISTFILE pPersistFile, LPCOLESTR lpszFileName, DWORD grfMode)
{
    PSHELLEXTSAMPLE this = PPSF2PSMX(pPersistFile);
    WideCharToMultiByte(CP_ACP, WC_SEPCHARS, lpszFileName, -1, this->_szFile, sizeof(this->_szFile), NULL, NULL);
    return NOERROR;
}

STDMETHODIMP SHE_PersistFile_Save(LPPERSISTFILE pPersistFile, LPCOLESTR lpszFileName, BOOL fRemember)
{
    return E_FAIL;
}

STDMETHODIMP SHE_PersistFile_SaveCompleted(LPPERSISTFILE pPersistFile, LPCOLESTR lpszFileName)
{
    return E_FAIL;
}

STDMETHODIMP SHE_PersistFile_GetCurFile(LPPERSISTFILE pPersistFile, LPOLESTR * lplpszFileName)
{
    return E_FAIL;
}

STDMETHODIMP_(UINT) SHE_PersistFile_AddRef(LPPERSISTFILE ppsf)
{
    PSHELLEXTSAMPLE this = PPSF2PSMX(ppsf);
    return ++this->_cRef;
}

STDMETHODIMP_(UINT) SHE_PersistFile_Release(LPPERSISTFILE ppsf)
{
    PSHELLEXTSAMPLE this = PPSF2PSMX(ppsf);
    return SHE_ExtractIcon_Release(&this->_exi);
}

STDMETHODIMP SHE_PersistFile_QueryInterface(LPPERSISTFILE ppsf, REFIID riid, LPVOID * ppv)
{
    PSHELLEXTSAMPLE this = PPSF2PSMX(ppsf);
    return SHE_ExtractIcon_QueryInterface(&this->_exi, riid, ppv);
}




//---------------------------------------------------------------------------
//
//	Shell Extension Sample's IShellExtInit Interface
//
//---------------------------------------------------------------------------
STDMETHODIMP SHE_ShellExtInit_Initialize(LPSHELLEXTINIT psxi,
		LPCITEMIDLIST pidlFolder,
		LPDATAOBJECT pdtobj, HKEY hkeyProgID)
{
    PSHELLEXTSAMPLE this = PSXI2PSMX(psxi);
    // Initialize can be called more than once.
    if (this->_pdtobj) {
	this->_pdtobj->lpVtbl->Release(this->_pdtobj);
    }

    if (this->_hkeyProgID) {
	RegCloseKey(this->_hkeyProgID);
    }

    // Duplicate the pdtobj pointer
    if (pdtobj) {
	this->_pdtobj = pdtobj;
	pdtobj->lpVtbl->AddRef(pdtobj);
    }

    // Duplicate the handle
    if (hkeyProgID) {
	RegOpenKeyEx(hkeyProgID, NULL, 0L, MAXIMUM_ALLOWED, &this->_hkeyProgID);
    }

    return NOERROR;
}

STDMETHODIMP_(UINT) SHE_ShellExtInit_AddRef(LPSHELLEXTINIT psxi)
{
    PSHELLEXTSAMPLE this = PSXI2PSMX(psxi);
    return ++this->_cRef;
}

STDMETHODIMP_(UINT) SHE_ShellExtInit_Release(LPSHELLEXTINIT psxi)
{
    PSHELLEXTSAMPLE this = PSXI2PSMX(psxi);
    return SHE_ContextMenu_Release(&this->_ctm);
}

STDMETHODIMP SHE_ShellExtInit_QueryInterface(LPSHELLEXTINIT psxi, REFIID riid, LPVOID *ppv)
{
    PSHELLEXTSAMPLE this = PSXI2PSMX(psxi);

    if (IsEqualIID(riid, &IID_IContextMenu))
    {
        return SHE_ContextMenu_QueryInterface(&this->_ctm, riid, ppv);
    }
    else if (IsEqualIID(riid, &IID_IShellPropSheetExt))
    {
        return SHE_PageExt_QueryInterface(&this->_spx, riid, ppv);
    }
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
}



//---------------------------------------------------------------------------
// CShellExtSample class : Vtables
//---------------------------------------------------------------------------

#pragma data_seg(".text")
IShellPropSheetExtVtbl c_ShellExtSample_SPXVtbl = {
	SHE_PageExt_QueryInterface,
	SHE_PageExt_AddRef,
	SHE_PageExt_Release,
	SHE_PageExt_AddPages
};

IExtractIconVtbl c_ShellExtSample_EXIVtbl = {
	SHE_ExtractIcon_QueryInterface,
	SHE_ExtractIcon_AddRef,
	SHE_ExtractIcon_Release,
	SHE_ExtractIcon_GetIconLocation,
	SHE_ExtractIcon_ExtractIcon,
};

IPersistFileVtbl c_ShellExtSample_PSFVtbl = {
	SHE_PersistFile_QueryInterface,
	SHE_PersistFile_AddRef,
	SHE_PersistFile_Release,
	SHE_PersistFile_GetClassID,
	SHE_PersistFile_IsDirty,
	SHE_PersistFile_Load,
	SHE_PersistFile_Save,
	SHE_PersistFile_SaveCompleted,
	SHE_PersistFile_GetCurFile
};

IContextMenuVtbl c_ShellExtSample_CTMVtbl = {
	SHE_ContextMenu_QueryInterface,
	SHE_ContextMenu_AddRef,
	SHE_ContextMenu_Release,
	SHE_ContextMenu_QueryContextMenu,
	SHE_ContextMenu_InvokeCommand,
	SHE_ContextMenu_GetCommandString,
};

IShellExtInitVtbl c_ShellExtSample_SXIVtbl = {
	SHE_ShellExtInit_QueryInterface,
	SHE_ShellExtInit_AddRef,
	SHE_ShellExtInit_Release,
	SHE_ShellExtInit_Initialize
};
#pragma data_seg()
