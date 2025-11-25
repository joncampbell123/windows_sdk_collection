//*******************************************************************************************
//
// Filename : Folder.h
//	
//				Definitions of CCabFolder and CCabItemList
//
// Copyright (c) 1994 - 1996 Microsoft Corporation. All rights reserved
//
//*******************************************************************************************

#ifndef _CABFOLD_H_
#define _CABFOLD_H_

enum _CV_COLS
{
	CV_COL_NAME = 0,
	CV_COL_SIZE,
	CV_COL_TYPE,
	CV_COL_MODIFIED,
	CV_COL_MAX,
} ;

typedef struct _CABITEM
{
	WORD wSize;
	DWORD dwFileSize;
	USHORT uFileDate;
	USHORT uFileTime;
	USHORT uFileAttribs;
	char szName[1];
} CABITEM, *LPCABITEM;

class CCabItemList
{
public:
	CCabItemList(UINT uStep) {m_uStep=uStep;}
	CCabItemList() {CCabItemList(8);}
	~CCabItemList() {CleanList();}

	enum
	{
		State_UnInit,
		State_Init,
		State_OutOfMem,
	};

	UINT GetState();

	LPCABITEM operator[](UINT nIndex)
	{
		return((LPCABITEM)DPA_GetPtr(m_dpaList, nIndex));
	}
	UINT GetCount() {return(GetState()==State_Init ? DPA_GetPtrCount(m_dpaList) : 0);}

	BOOL InitList();

	BOOL AddItems(LPCABITEM *apit, UINT cpit);
	BOOL AddItem(LPCSTR pszName, DWORD dwFileSize,
		UINT uFileDate, UINT uFileTime, UINT uFileAttribs);

	int FindInList(LPCSTR pszName, DWORD dwFileSize,
		UINT uFileDate, UINT uFileTime, UINT uFileAttribs);
	BOOL IsInList(LPCSTR pszName, DWORD dwFileSize,
		UINT uFileDate, UINT uFileTime, UINT uFileAttribs)
	{
		return(FindInList(pszName, dwFileSize, uFileDate, uFileTime, uFileAttribs) >= 0);
	}


private:
	BOOL StoreItem(LPITEMIDLIST pidl);
	void CleanList();

private:
	UINT m_uStep;
	HDPA m_dpaList;
} ;

class CCabFolder : public IPersistFolder, public IShellFolder
{
public:
	CCabFolder() : m_pidlHere(0), m_lItems(1024/sizeof(LPVOID)) {}
	~CCabFolder()
	{
		if (m_pidlHere)
		{
			ILFree(m_pidlHere);
		}
	}

	// *** IUnknown methods ***
	STDMETHODIMP QueryInterface(
		REFIID riid, 
		LPVOID FAR* ppvObj);
	STDMETHODIMP_(ULONG) AddRef(void);
	STDMETHODIMP_(ULONG) Release(void);

	// *** IParseDisplayName method ***
	STDMETHODIMP ParseDisplayName(
		HWND hwndOwner,
		LPBC pbc, 
		LPOLESTR lpszDisplayName,
		ULONG FAR* pchEaten, 
		LPITEMIDLIST * ppidl,
		ULONG *pdwAttributes);

	// *** IOleContainer methods ***
	STDMETHODIMP EnumObjects(
		HWND hwndOwner, 
		DWORD grfFlags,
		LPENUMIDLIST * ppenumIDList);

	// *** IShellFolder methods ***
	STDMETHODIMP BindToObject(
		LPCITEMIDLIST pidl, 
		LPBC pbc,
		REFIID riid, 
		LPVOID FAR* ppvObj);
	STDMETHODIMP BindToStorage(
		LPCITEMIDLIST pidl, 
		LPBC pbc,
		REFIID riid, 
		LPVOID FAR* ppvObj);
	STDMETHODIMP CompareIDs(
		LPARAM lParam, 
		LPCITEMIDLIST pidl1,
		LPCITEMIDLIST pidl2);
	STDMETHODIMP CreateViewObject(
		HWND hwndOwner, 
		REFIID riid,
		LPVOID FAR* ppvObj);
	STDMETHODIMP GetAttributesOf(
		UINT cidl, 
		LPCITEMIDLIST FAR* apidl,
		ULONG FAR* rgfInOut);
	STDMETHODIMP GetUIObjectOf(
		HWND hwndOwner, 
		UINT cidl, 
		LPCITEMIDLIST FAR* apidl, 
		REFIID riid, 
		UINT FAR* prgfInOut, 
		LPVOID FAR* ppvObj);
	STDMETHODIMP GetDisplayNameOf(
		LPCITEMIDLIST pidl, 
		DWORD dwReserved, 
		LPSTRRET lpName);
	STDMETHODIMP SetNameOf(
		HWND hwndOwner, 
		LPCITEMIDLIST pidl,
		LPCOLESTR lpszName, 
		DWORD dwReserved,
		LPITEMIDLIST FAR* ppidlOut);

	// *** IPersist methods ***
	STDMETHODIMP GetClassID(
		LPCLSID lpClassID);
	// *** IPersistFolder methods ***
	STDMETHODIMP Initialize(
		LPCITEMIDLIST pidl);

public:
	static LPITEMIDLIST CreateIDList(LPCSTR pszName, DWORD dwFileSize,
		UINT uFileDate, UINT uFileTime, UINT uFileAttribs);
	static void GetNameOf(LPCABITEM pit, LPSTRRET lpName);
	static void GetTypeOf(LPCABITEM pit, LPSTRRET lpName);

	BOOL GetPath(LPSTR szPath);

private:
	static void CALLBACK EnumToList(LPCSTR pszFile, DWORD dwSize, UINT date,
		UINT time, UINT attribs, LPARAM lParam);

	HRESULT InitItems();

private:
	CRefDll m_cRefDll;

	CRefCount m_cRef;

	LPITEMIDLIST m_pidlHere;		// maintains the current pidl

	CCabItemList m_lItems;

	friend class CEnumCabObjs;
} ;

#endif // _CABFOLD_H_
