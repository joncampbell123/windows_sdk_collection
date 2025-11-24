/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

#include "cplx.h"
#include "rc.h"

//
// Initialize GUIDs (should be done only and at-least once per DLL/EXE)
//
#pragma data_seg(".text")
#define INITGUID
#include <initguid.h>
#include <coguid.h>
#include <shlguid.h>
DEFINE_GUID( CLSID_CplExtSample, 0x4753B0A0L, 0x79BB, 0x101B, 0xAE, 0x4B, 0x00, 0xAA, 0x00, 0x37, 0x3B, 0x20 );
#pragma data_seg()


//
// Global variables
//
UINT g_cRefThisDll = 0;		// Reference count of this DLL.
HANDLE g_hmodThisDll = NULL;	// Handle to this DLL itself.


//
// Function prototypes
//
HRESULT CALLBACK CplExtSample_CreateInstance(LPUNKNOWN, REFIID, LPVOID FAR*);

BOOL CALLBACK FooDlgProc(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam);


//---------------------------------------------------------------------------
// LibMain
//---------------------------------------------------------------------------

BOOL APIENTRY
LibMain(HANDLE hDll, DWORD dwReason, LPVOID lpReserved)
{
    if( dwReason == DLL_PROCESS_ATTACH )
        g_hmodThisDll = hDll;

    return TRUE;
}


//---------------------------------------------------------------------------
// DllCanUnloadNow
//---------------------------------------------------------------------------

STDAPI
DllCanUnloadNow( void )
{
    return ResultFromScode( g_cRefThisDll? S_FALSE : S_OK );
}


//=========================================================================
// CDefClassFactory class ... From defclsf.c
//=========================================================================
typedef struct
{
    IClassFactory      cf;
    UINT               cRef;		// Reference count
    LPFNCREATEINSTANCE lpfnCI;		// CreateInstance callback entry
    UINT FAR *         pcRefDll;	// Reference count of the DLL
    const IID FAR *    riidInst;		// Optional interface for instance
} CDefClassFactory;

extern CDefClassFactory * NEAR PASCAL
    CDefClassFactory_Create( LPFNCREATEINSTANCE lpfnCI,
                                UINT FAR *pcRefDll, REFIID riidInst );


//---------------------------------------------------------------------------
//
// DllGetClassObject
//
//  This is the entry of this DLL, which all the In-Proc server DLLs should
// export.  See the description of "DllGetClassObject" of OLE 2.0 reference
// manual for detail.
//
//---------------------------------------------------------------------------

STDAPI
DllGetClassObject( REFCLSID rclsid, REFIID riid, LPVOID FAR *ppvOut )
{
    *ppvOut = NULL; // Assume Failure

    if( IsEqualCLSID( rclsid, &CLSID_CplExtSample ) )
    {
        if( IsEqualIID( riid, &IID_IClassFactory )
	        || IsEqualIID( riid, &IID_IUnknown ) )
        {
            CDefClassFactory * pacf =
                CDefClassFactory_Create( CplExtSample_CreateInstance,
                &g_cRefThisDll, NULL );

	    if( pacf )
	    {
	        (IClassFactory FAR *)*ppvOut = &pacf->cf;
	        return NOERROR;
	    }

	    return ResultFromScode( E_OUTOFMEMORY );
        }

        return ResultFromScode( E_NOINTERFACE );
    }
    else
        return ResultFromScode( CLASS_E_CLASSNOTAVAILABLE );
}


//---------------------------------------------------------------------------
//
// CShellExtSample class
//
//---------------------------------------------------------------------------
typedef struct _CplExtSample
{
	IShellPropSheetExt	_spx;
	int 			_cRef;
	HKEY			_hkeyProgID;
} CplExtSample;

#define SMX_OFFSETOF(x) 	((UINT)(&((CplExtSample *)0)->x))
#define PVOID2PSMX(pv,offset)	((CplExtSample *)(((LPBYTE)pv)-offset))

#define PSPX2PSMX(pspx)	        PVOID2PSMX(pspx, SMX_OFFSETOF(_spx))

//
// Vtable prototype
//
extern IShellPropSheetExtVtbl      c_CplExtSample_SPXVtbl;

//---------------------------------------------------------------------------
//
// ShellExtSample_CreateInstance
//
//  This function is called back from within IClassFactory::CreateInstance()
// of the default class factory object, which is created by CreateClassObject.
//
//---------------------------------------------------------------------------

HRESULT CALLBACK CplExtSample_CreateInstance(LPUNKNOWN punkOuter,
				        REFIID riid, LPVOID FAR* ppvOut)
{
    HRESULT hres;
    CplExtSample *psmx;
    //
    // Shell extentions typically do not need to support aggregation.
    //
    if (punkOuter) {
	return ResultFromScode(CLASS_E_NOAGGREGATION);
    }

    //
    // in C++:
    //	psmx = new CCplExtSample();
    //
    psmx = LocalAlloc( LPTR, sizeof( CplExtSample ) );
    if (!psmx) {
	return ResultFromScode(E_OUTOFMEMORY);
    }
    psmx->_spx.lpVtbl = &c_CplExtSample_SPXVtbl;
    psmx->_cRef = 1;
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
    hres = E_NOINTERFACE;

    if (IsEqualIID(riid, &IID_IShellPropSheetExt)) {
        hres = c_CplExtSample_SPXVtbl.QueryInterface(&psmx->_spx, riid, ppvOut);
        c_CplExtSample_SPXVtbl.Release(&psmx->_spx);
    }
    
    return hres;	// S_OK or E_NOINTERFACE
}



//---------------------------------------------------------------------------
//
//	Shell Extension Sample's IShellPropSheetExt Interface
//
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//
// FooDlgProc
//
//  The dialog procedure for the "FooPage" property sheet page.
//
//---------------------------------------------------------------------------

BOOL CALLBACK FooDlgProc(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam)
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

        // put your init here
        break;

    case WM_DESTROY:
        // put your cleanup here
        break;

    case WM_COMMAND:
        // pretend the user dorked with a setting
        // tell the property manager we have outstanding changes
        // this will enable the "Apply Now" button...
        SendMessage( GetParent( hDlg ), PSM_CHANGED, (WPARAM)hDlg, 0L );
        break;

    case WM_NOTIFY:
        switch (((NMHDR FAR *)lParam)->code)
        {
        case PSN_APPLY:
            // this is akin to an ok handler
            // this notify is only sent if we've sent a PSM_CHANGED to
            // our parent since the last apply or reset...
            // DO NOT call EndDialog here! (just apply the new status)
            break;

        case PSN_RESET:
            // this is akin to a cancel handler
            // this notify is only sent if we've sent a PSM_CHANGED to
            // our parent since the last apply or reset...
            // DO NOT call EndDialog here! (just forget the interim status)
            break;

        case PSN_SETACTIVE:
        case PSN_KILLACTIVE:
            //  You might care about these if you do something piggish on your
            // page.  Ideally, you would only get really piggish when your page
            // is the active one.
            break;

        default:
            break;
        }
        break;

    default:
        return FALSE;
    }

    return(TRUE);
}


STDMETHODIMP SHE_PageExt_AddPages(LPSHELLPROPSHEETEXT pspx,
		              LPFNADDPROPSHEETPAGE lpfnAddPage,
			      LPARAM lParam)
{
    PROPSHEETPAGE psp;
    HPROPSHEETPAGE hpage;

    psp.dwSize = sizeof( psp );
    psp.dwFlags = PSP_DEFAULT;
    psp.hInstance = g_hmodThisDll;
    psp.pszTemplate = MAKEINTRESOURCE( DLG_FOO );
    psp.pfnDlgProc = FooDlgProc;
    psp.lParam = 0;

    if( ( hpage = CreatePropertySheetPage( &psp ) ) == NULL )
        return ResultFromScode( E_OUTOFMEMORY );

    if( !lpfnAddPage( hpage, lParam ) )
    {
        DestroyPropertySheetPage( hpage );
        return ResultFromScode( E_FAIL );
    }

    return NOERROR;
}


STDMETHODIMP SHE_PageExt_ReplacePage( LPSHELLPROPSHEETEXT pspx, UINT uPageID,
		              LPFNADDPROPSHEETPAGE lpfnAddPage,
			      LPARAM lParam )
{
    return NOERROR;
}

STDMETHODIMP_(UINT) SHE_PageExt_AddRef(LPSHELLPROPSHEETEXT pspx)
{
    CplExtSample *this = PSPX2PSMX(pspx);
    return ++this->_cRef;
}

STDMETHODIMP_(UINT) SHE_PageExt_Release(LPSHELLPROPSHEETEXT pspx)
{
    CplExtSample *this = PSPX2PSMX(pspx);

    if (--this->_cRef) {
        return this->_cRef;
    }
	
    if (this->_hkeyProgID)
    {
        RegCloseKey(this->_hkeyProgID);
    }

    LocalFree((HLOCAL)this);
    g_cRefThisDll--;
	
    return 0;
}

STDMETHODIMP SHE_PageExt_QueryInterface(LPSHELLPROPSHEETEXT pspx, REFIID riid, LPVOID FAR* ppvOut)
{
    CplExtSample *this = PSPX2PSMX(pspx);

    if (IsEqualIID(riid, &IID_IShellPropSheetExt) || IsEqualIID(riid, &IID_IUnknown))
    {
        (LPSHELLPROPSHEETEXT)*ppvOut=pspx;
        this->_cRef++;
        return NOERROR;
    }

    *ppvOut=NULL;
    return ResultFromScode(E_NOINTERFACE);
}


//---------------------------------------------------------------------------
// CShellExtSample class : Vtables
//---------------------------------------------------------------------------

#pragma data_seg(".text")
IShellPropSheetExtVtbl c_CplExtSample_SPXVtbl = {
	SHE_PageExt_QueryInterface,
	SHE_PageExt_AddRef,
	SHE_PageExt_Release,
	SHE_PageExt_AddPages,
	SHE_PageExt_ReplacePage
};
#pragma data_seg()
