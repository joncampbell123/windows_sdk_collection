//*******************************************************************************************
//
// Filename : folder.cpp
//	
//				CAB Files Shell Extension
//
// Copyright (c) 1994 - 1996 Microsoft Corporation. All rights reserved
//
//*******************************************************************************************


#include "pch.h"

#include "thisdll.h"
#include "thisguid.h"

#include "SFView.H"
#include "folder.h"
#include "enum.h"
#include "view.h"
#include "icon.h"
#include "menu.h"
#include "dataobj.h"

#include "cabitms.h"

// *** IUnknown methods ***
STDMETHODIMP CCabFolder::QueryInterface(
   REFIID riid, 
   LPVOID FAR* ppvObj)
{
	*ppvObj = NULL;

	LPVOID pObj;
 
	if (riid == IID_IUnknown)
	{
		pObj = (IUnknown*)((IShellFolder*)this); 
		// The (IShellFolder*) ^^^ up there is to disambiguate :) the reference
	}
	else if (riid == IID_IShellFolder)
	{
		pObj = (IShellFolder*)this;
	}
	else if (riid == IID_IPersistFolder)
	{
		pObj = (IPersistFolder*)this;
	}
	else
	{
   		return(E_NOINTERFACE);
	}

	((LPUNKNOWN)pObj)->AddRef();
	*ppvObj = pObj;

	return(NOERROR);
}


STDMETHODIMP_(ULONG) CCabFolder::AddRef(void)
{
	return(m_cRef.AddRef());
}


STDMETHODIMP_(ULONG) CCabFolder::Release(void)
{
	if (!m_cRef.Release())
	{
   		delete this;
		return(0);
	}

	return(m_cRef.GetRef());
}

// *** IParseDisplayName method ***
STDMETHODIMP CCabFolder::ParseDisplayName(
   HWND hwndOwner,
   LPBC pbc, 
   LPOLESTR lpszDisplayName,
   ULONG FAR* pchEaten, 
   LPITEMIDLIST * ppidl,
   ULONG *pdwAttributes)
{
	return(E_NOTIMPL);
}


// *** IOleContainer methods ***
//**********************************************************************
//
// CCabFolder::EnumObjects
//
// Purpose:
//
//      Creates an item enumeration object 
//      (an IEnumIDList interface) that can be used to 
//      enumerate the contents of a folder.
//
// Parameters:
//
//       HWND hwndOwner       -    handle to the owner window
//       DWORD grFlags        -    flags about which items to include
//       LPENUMIDLIST *ppenumIDList - address that receives IEnumIDList
//                                    interface pointer 
//
//
// Comments:
//
//
//********************************************************************

STDMETHODIMP CCabFolder::EnumObjects(
   HWND hwndOwner, 
   DWORD grfFlags,
   LPENUMIDLIST * ppenumIDList) // LPENUMUNKNOWN FAR* ppenumUnknown)
{
	CEnumCabObjs *pce = new CEnumCabObjs(this, grfFlags);
	if (!pce)
	{
		return(E_OUTOFMEMORY);
	}

	pce->AddRef();
	HRESULT hRes = pce->QueryInterface(IID_IEnumIDList, (LPVOID*)ppenumIDList);
	pce->Release();

	return(hRes);
}

// *** IShellFolder methods ***
// subfolders not implemented
STDMETHODIMP CCabFolder::BindToObject(
   LPCITEMIDLIST pidl, 
   LPBC pbc,
   REFIID riid, 
   LPVOID FAR* ppvObj)
{
	return(E_NOTIMPL);
}


STDMETHODIMP CCabFolder::BindToStorage(
   LPCITEMIDLIST pidl, 
   LPBC pbc,
   REFIID riid, 
   LPVOID FAR* ppvObj)
{
	return(E_NOTIMPL);
}


//**********************************************************************
//
// CCabFolder::CompareIDs
//
// Purpose:
//
//      Determines the relative ordering of two file
//      objects or folders, given their item identifier lists
//
// Parameters:
//
//      LPARAM lParam         -    type of comparison
//      LPCITEMIDLIST pidl1   -    address to ITEMIDLIST
//      LPCITEMIDLIST pidl2   -    address to ITEMIDLIST
//
//
// Comments:
//
//
//********************************************************************

STDMETHODIMP CCabFolder::CompareIDs(
   LPARAM lParam, 
   LPCITEMIDLIST pidl1,
   LPCITEMIDLIST pidl2)
{
	LPCABITEM pit1 = (LPCABITEM)pidl1;
	LPCABITEM pit2 = (LPCABITEM)pidl2;

	short nCmp = 0;

	switch (lParam)
	{
	case CV_COL_NAME:
		break;

	case CV_COL_SIZE:
		if (pit1->dwFileSize < pit2->dwFileSize)
		{
			nCmp = -1;
		}
		else if (pit1->dwFileSize > pit2->dwFileSize)
		{
			nCmp = 1;
		}
		break;

	case CV_COL_TYPE:
	{
		STRRET srName1, srName2;

		GetTypeOf(pit1, &srName1);
		GetTypeOf(pit2, &srName2);

		nCmp = (SHORT)lstrcmp(srName1.cStr, srName2.cStr);
		break;
	}

	case CV_COL_MODIFIED:
		FILETIME ft1, ft2;

		DosDateTimeToFileTime(pit1->uFileDate, pit1->uFileTime, &ft1);
		DosDateTimeToFileTime(pit2->uFileDate, pit2->uFileTime, &ft2);

		nCmp = (SHORT)CompareFileTime(&ft1, &ft2);
		break;

	default:
		break;
	}

	if (nCmp != 0)
	{
		return(ResultFromShort(nCmp));
	}

	return(ResultFromShort(lstrcmpi(pit1->szName, pit2->szName)));
}


//**********************************************************************
//
// CCabFolder::CreateViewObject
//
// Purpose:
//
//      IShellbrowser calls this to create a ShellView  
//      object
//
// Parameters:
//
//      HWND   hwndOwner     -
//   
//      REFIID riid          -  interface ID
//
//      LPVOID * ppvObj      -  pointer to the Shellview object
//
// Return Value:
//
//      NOERROR
//      E_OUTOFMEMORY
//      E_NOINTERFACE
//
//
// Comments:
//
//      ShellBrowser interface calls this to request the ShellFolder
//      to create a ShellView object
//
//********************************************************************

STDMETHODIMP CCabFolder::CreateViewObject(
   HWND hwndOwner, 
   REFIID riid,
   LPVOID FAR* ppvObj)
{
	IUnknown *pObj = NULL;

	if (riid == IID_IShellView)
	{
		
		// Create a call back for the ShellView

		IShellFolderViewCallback *pcb;
		HRESULT hRes = CabView_CreateCallback(&pcb);

		hRes = CreateShellFolderView(this, pcb, (LPSHELLVIEW FAR*)&pObj);
		if (pcb)
		{
			// The ShellFolderView should have AddRef'ed if it needed it.
			pcb->Release();
		}

		if (FAILED(hRes))
		{
			return(hRes);
		}
	}
	else
	{
		return(E_NOINTERFACE);
	}

	if (!pObj)
	{
		return(E_OUTOFMEMORY);
	}

	// The ref count is already 1
	HRESULT hRes = pObj->QueryInterface(riid, ppvObj);
	pObj->Release();

	return(NOERROR);
}


// **************************************************************************************
//
// CCabFolder::GetAttributesOf
//
// Purpose
//
//			Retrieves attributes of one of more file objects
//
// Parameters:
//			
//    UINT cidl                -    number of file objects
//    LPCITEMIDLIST  *apidl    -    pointer to array of ITEMIDLIST
//    ULONG *rgfInOut          -    array of values that specify file object
//                                  attributes
//
//
// Return Value:
//    
//     NOERROR
//
//	Comments
//
// ***************************************************************************************

STDMETHODIMP CCabFolder::GetAttributesOf(
   UINT cidl, 
   LPCITEMIDLIST FAR* apidl,
   ULONG FAR* rgfInOut)
{
	*rgfInOut &= SFGAO_CANCOPY;

	return(NOERROR);
}

// **************************************************************************************
//
// CCabFolder::GetUIObjectOf
//
// Purpose
//
//			Returns an interface that can be used to carry out actions on 
//          the specified file objects or folders
//
// Parameters:
//        
//        HWND hwndOwner        -    handle of the Owner window
//
//        UINT cidl             -    Number of file objects
//        
//        LPCITEMIDLIST *apidl  -    array of file object pidls
//
//        REFIID                -    Identifier of interface to return
//        			
//        UINT * prgfInOut      -    reserved
//
//        LPVOID *ppvObj        -    address that receives interface pointer
//
// Return Value:
//        
//         E_INVALIDARG
//         E_NOINTERFACE
//         E_OUTOFMEMORY
//
//	Comments
// ***************************************************************************************

STDMETHODIMP CCabFolder::GetUIObjectOf(
   HWND hwndOwner, 
   UINT cidl, 
   LPCITEMIDLIST FAR* apidl, 
   REFIID riid, 
   UINT FAR* prgfInOut, 
   LPVOID FAR* ppvObj)
{
	LPUNKNOWN pObj = NULL;

	if (riid == IID_IExtractIcon)
	{
		if (cidl != 1)
		{
			return(E_INVALIDARG);
		}

		LPCABITEM pci = (LPCABITEM)*apidl;
		pObj = (LPUNKNOWN)(IExtractIcon *)(new CCabItemIcon(pci->szName));
	}
	else if (riid == IID_IContextMenu)
	{
		if (cidl < 1)
		{
			return(E_INVALIDARG);
		}

		pObj = (LPUNKNOWN)(IContextMenu *)(new CCabItemMenu(hwndOwner, this,
			(LPCABITEM *)apidl, cidl));
	}
	else if (riid == IID_IDataObject)
	{
		if (cidl < 1)
		{
			return(E_INVALIDARG);
		}

		pObj = (LPUNKNOWN)(IDataObject *)(new CCabObj(hwndOwner, this,
			(LPCABITEM *)apidl, cidl));
	}
	else
	{
		return(E_NOINTERFACE);
	}

	if (!pObj)
	{
		return(E_OUTOFMEMORY);
	}

	pObj->AddRef();
	HRESULT hRes = pObj->QueryInterface(riid, ppvObj);
	pObj->Release();

	return(hRes);
}

//*****************************************************************************
//
// CCabFolder::GetDisplayNameOf
//
// Purpose:
//        Retrieves the display name for the specified file object or 
//        subfolder.
//
//
// Parameters:
//
//        LPCITEMIDLIST    pidl    -    pidl of the file object
//        DWORD  dwReserved        -    Value of the type of display name to 
//                                      return
//        LPSTRRET  lpName         -    address holding the name returned        
//
//
// Comments:
//
//*****************************************************************************


STDMETHODIMP CCabFolder::GetDisplayNameOf(
   LPCITEMIDLIST pidl, 
   DWORD dwReserved, 
   LPSTRRET lpName)
{
	LPCABITEM pit = (LPCABITEM)pidl;

	GetNameOf(pit, lpName);

	return(NOERROR);
}


STDMETHODIMP CCabFolder::SetNameOf(
   HWND hwndOwner, 
   LPCITEMIDLIST pidl,
   LPCOLESTR lpszName, 
   DWORD dwReserved,
   LPITEMIDLIST FAR* ppidlOut)
{
	return(E_NOTIMPL);
}

  // *** IPersist methods ***
//**********************************************************************
//
// CCabFolder::GetClassID
//
// Purpose:
//
//      Return the class id
//
// Parameters:
//
//      LPCLSID lpClassID       -   pointer to the ClassID member
//
// Return Value:
//
//      NOERROR
//
//
// Comments:
//
//      This routine returns the Class ID for the DLL
//
//********************************************************************


STDMETHODIMP CCabFolder::GetClassID(
   LPCLSID lpClassID)
{
	*lpClassID = CLSID_ThisDll;
	return NOERROR;
}


// *** IPersistFolder methods ***
//**********************************************************************
//
// CCabFolder::Initialize folder
//
// Purpose:
//
//      Explorer calls this while initializing the ShellFolder 
//      object
//
// Parameters:
//
//      LPCITEMIDLIST pidl		-   pidl passed by IShellBrowser
//
// Return Value:
//
//      S_OK
//
//
// Comments:
//
//      This routine is called by Explorer during initialization
//
//********************************************************************


STDMETHODIMP CCabFolder::Initialize(
   LPCITEMIDLIST pidl)
{
	if (m_pidlHere)
	{
		ILFree(m_pidlHere);
	}

	// Clone the pidl passed by the explorer

	m_pidlHere = ILClone(pidl);
	if (!m_pidlHere)
	{
		return(E_OUTOFMEMORY);
	}

	return(S_OK);
}

//*****************************************************************************
//
// CCabFolder::CreateIDList
//
// Purpose:
//
//    Creates an item identifier list for the objects in the namespace
//
//
//*****************************************************************************

LPITEMIDLIST CCabFolder::CreateIDList(LPCSTR pszName, DWORD dwFileSize,
	UINT uFileDate, UINT uFileTime, UINT uFileAttribs)
{
	// We'll assume no name is longer than MAX_PATH
	// Note the terminating NULL is already in the sizeof(CABITEM)
	BYTE bBuf[sizeof(CABITEM) + MAX_PATH + sizeof(WORD)];
	LPCABITEM pci = (LPCABITEM)bBuf;

	UINT uNameLen = lstrlen(pszName);
	if (uNameLen >= MAX_PATH)
	{
		uNameLen = MAX_PATH;
	}

	pci->wSize = (WORD)(sizeof(CABITEM) + uNameLen);
	pci->dwFileSize = dwFileSize;
	pci->uFileDate = (USHORT)uFileDate;
	pci->uFileTime = (USHORT)uFileTime;
	pci->uFileAttribs = (USHORT)uFileAttribs;
	lstrcpyn(pci->szName, pszName, uNameLen+1);

	// Terminate the IDList
	*(WORD *)(((LPSTR)pci)+pci->wSize) = 0;

	return(ILClone((LPCITEMIDLIST)pci));
}

//*****************************************************************************
//
// CCabFolder::GetPath
//
// Purpose:
//    
//        Get the Path for the current pidl
//
// Parameters:
//   
//        LPSTR szPath        -    return pointer for path string
//
// Comments:
//
//*****************************************************************************

BOOL CCabFolder::GetPath(LPSTR szPath)
{
	if (!m_pidlHere || !SHGetPathFromIDList(m_pidlHere, szPath))
	{
		*szPath = '\0';
		return(FALSE);
	}

	return(TRUE);
}


void CCabFolder::GetNameOf(LPCABITEM pit, LPSTRRET lpName)
{
	lpName->uType = STRRET_OFFSET;
	lpName->uOffset = FIELDOFFSET(CABITEM, szName);

	SHFILEINFO sfi;

	if (SHGetFileInfo(pit->szName, 0, &sfi, sizeof(sfi),
		SHGFI_USEFILEATTRIBUTES | SHGFI_DISPLAYNAME)
		&& lstrcmp(sfi.szDisplayName, pit->szName) != 0)
	{
		lpName->uType = STRRET_CSTR;
		lstrcpy(lpName->cStr, sfi.szDisplayName);
	}
}


void CCabFolder::GetTypeOf(LPCABITEM pit, LPSTRRET lpName)
{
	lpName->uType = STRRET_CSTR;
	lpName->cStr[0] = '\0';

	SHFILEINFO sfi;

	if (SHGetFileInfo(pit->szName, 0, &sfi, sizeof(sfi),
		SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME))
	{
		lstrcpy(lpName->cStr, sfi.szTypeName);
	}
}
//*****************************************************************************
//
// CCabFolder::EnumToList
//
// Purpose:
//
//       This notify callback is called by the FDI routines. It adds the
//       file object from the cab file to the list.  
//
// Parameters:
//
//
//
// Comments:
//
//*****************************************************************************


void CALLBACK CCabFolder::EnumToList(LPCSTR pszFile, DWORD dwSize, UINT date,
		UINT time, UINT attribs, LPARAM lParam)
{
	CCabFolder *pThis = (CCabFolder *)lParam;

	pThis->m_lItems.AddItem(pszFile, dwSize, date, time, attribs);
}


HRESULT CCabFolder::InitItems()
{
	switch (m_lItems.GetState())
	{
	case CCabItemList::State_Init:
		return(NOERROR);

	case CCabItemList::State_OutOfMem:
		return(E_OUTOFMEMORY);

	case CCabItemList::State_UnInit:
	default:
		break;
	}

	// Force the list to initialize
	m_lItems.InitList();

	char szHere[MAX_PATH];

	// the m_pidl has been set to current dir
	// get the path to the current directory
	if (!GetPath(szHere))
	{
		return(E_UNEXPECTED);
	}

	CCabItems ciHere(szHere);

	if (!ciHere.EnumItems(EnumToList, (LPARAM)this))
	{
		return(E_UNEXPECTED);
	}

	return(NOERROR);
}

//*******************************************************************************************
//
// CreateInstance
//
// Purpose:
//
//		Create a CCabFolder object and returns it
//
// Parameters:
//		REFIID riid		-	a reference to the interface that is
//							being queried	
//	
//		LPVOID *ppvObj	-	an out parameter to return a pointer to
//							interface being queried
//
//*******************************************************************************************

HRESULT CreateInstance(REFIID riid, LPVOID *ppvObj)
{
	IUnknown *pObj = NULL;
	*ppvObj = NULL;

	if(riid == IID_IPersistFolder)
	{
		pObj = (IUnknown *)(IPersistFolder *)(new CCabFolder);
	}
	else if(riid == IID_IShellFolder)
	{
		pObj = (IUnknown *)(IShellFolder *)(new CCabFolder);
	}
	else
	{
		return(E_NOINTERFACE);
	}

	if (!pObj)
	{
		return(E_OUTOFMEMORY);
	}

	pObj->AddRef();
	HRESULT hRes = pObj->QueryInterface(riid, ppvObj);
	pObj->Release();

	return(hRes);
}


UINT CCabItemList::GetState()
{
	if (m_uStep == 0)
	{
		if (m_dpaList)
		{
			return(State_Init);
		}

		return(State_OutOfMem);
	}

	return(State_UnInit);
}


BOOL CCabItemList::StoreItem(LPITEMIDLIST pidl)
{
	if (pidl)
	{
		if (InitList() && DPA_InsertPtr(m_dpaList, 0x7fff, (LPSTR)pidl)>=0)
		{
			return(TRUE);
		}

		ILFree(pidl);
	}

	CleanList();
	return(FALSE);
}


BOOL CCabItemList::AddItems(LPCABITEM *apit, UINT cpit)
{
	for (UINT i=0; i<cpit; ++i)
	{
		if (!StoreItem(ILClone((LPCITEMIDLIST)apit[i])))
		{
			return(FALSE);
		}
	}

	return(TRUE);
}


BOOL CCabItemList::AddItem(LPCSTR pszName, DWORD dwFileSize,
	UINT uFileDate, UINT uFileTime, UINT uFileAttribs)
{
	return(StoreItem(CCabFolder::CreateIDList(pszName, dwFileSize, uFileDate, uFileTime,
		uFileAttribs)));
}


int CCabItemList::FindInList(LPCSTR pszName, DWORD dwFileSize,
	UINT uFileDate, UINT uFileTime, UINT uFileAttribs)
{
	// TODO: Linear search for now; binary later
	for (int i=DPA_GetPtrCount(m_dpaList)-1; i>=0; --i)
	{
		if (lstrcmpi(pszName, (*this)[i]->szName) == 0)
		{
			break;
		}
	}

	return(i);
}


BOOL CCabItemList::InitList()
{
	switch (GetState())
	{
	case State_Init:
		return(TRUE);

	case State_OutOfMem:
		return(FALSE);

	case State_UnInit:
	default:
		m_dpaList = DPA_Create(m_uStep);
		m_uStep = 0;

		return(InitList());
	}
}


void CCabItemList::CleanList()
{
	if (m_uStep != 0)
	{
		m_dpaList = NULL;
		m_uStep = 0;
		return;
	}

	if (!m_dpaList)
	{
		return;
	}

	for (int i=DPA_GetPtrCount(m_dpaList)-1; i>=0; --i)
	{
		ILFree((LPITEMIDLIST)((*this)[i]));
	}

	DPA_Destroy(m_dpaList);
	m_dpaList = NULL;
}
