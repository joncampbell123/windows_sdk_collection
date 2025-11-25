//*******************************************************************************************
//
// Filename : SFView.h
//	
//				Definition of IShellFolderViewCallback
//
// Copyright (c) 1994 - 1996 Microsoft Corporation. All rights reserved
//
//*******************************************************************************************


// 1C503180-E9F5-11CE-AE65-08002B2E1262
DEFINE_GUID(IID_IShellFolderViewCallback, 0x1C503180L, 0xE9F5, 0x11CE, 0xAE, 0x65, 0x08, 0x00, 0x2B, 0x2E, 0x12, 0x62);

#ifndef _SFView_H_
#define _SFView_H_

#undef  INTERFACE
#define INTERFACE IShellFolderViewCallback

DECLARE_INTERFACE_(IShellFolderViewCallback, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG, AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG, Release) (THIS) PURE;

	// *** IShellFolderViewCallback methods ***
	STDMETHOD(Message)(THIS_ UINT uMsg, WPARAM wParam, LPARAM lParam) PURE;
};

struct SFVCB_GETDETAILSOF_DATA
{
	// In:
	LPCITEMIDLIST pidl;	// NULL if column header

	// Out:
	int fmt;			// A LVCFMT_* value if pidl == NULL
	UINT cChar;			// Default width in chars if pidl == NULL
	LPARAM lParamSort;	// The lParam for IShellFolder::CompareIDs if pidl == NULL
	STRRET str;			// The string to display
} ;

#define HANDLE_SFVCB_MSG(message, fn)    \
    case (message): return HANDLE_##message((wParam), (lParam), (fn))

#define SFVCB_GETDETAILSOF 1
// wParam is the index of the column
// lParam is a pointer to SFVCB_GETDETAILSOF_DATA
// return is an error code if the column is out of range
// HRESULT OnGetDetailsOf(UINT uColumn, SFVCB_GETDETAILSOF_DATA *pData)
#define HANDLE_SFVCB_GETDETAILSOF(_wp, _lp, _fn) ((_fn)((UINT)(_wp), \
	(SFVCB_GETDETAILSOF_DATA*)(_lp)))

HRESULT CreateShellFolderView(LPSHELLFOLDER psf,
	IShellFolderViewCallback *psfvcb, LPSHELLVIEW * ppsv);

#define SFVID_FIRST (FCIDM_SHVIEWFIRST + 0x2000)
#define SFVID_LAST  (FCIDM_SHVIEWFIRST + 0x3000)

#define SFVID_MENU_ARRANGE (SFVID_FIRST + 0x000)

#endif // _SFView_H_
