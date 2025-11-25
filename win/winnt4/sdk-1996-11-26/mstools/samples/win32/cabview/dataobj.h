//*******************************************************************************************
//
// Filename : DataObj.h
//	
//				Definition of CCabObj
//
// Copyright (c) 1994 - 1996 Microsoft Corporation. All rights reserved
//
//*******************************************************************************************

#ifndef _DATAOBJ_H_
#define _DATAOBJ_H_

class CCabObj : public IDataObject
{
public:
	CCabObj(HWND hwndOwner, CCabFolder *pcf, LPCABITEM *apit, UINT cpit);
	~CCabObj();

	STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppvObject);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
        
	STDMETHODIMP GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium);
	STDMETHODIMP GetDataHere(FORMATETC *pformatetc, STGMEDIUM *pmedium);
	STDMETHODIMP QueryGetData(FORMATETC *pformatetc);
	STDMETHODIMP GetCanonicalFormatEtc(FORMATETC *pformatectIn, FORMATETC *pformatetcOut);
	STDMETHODIMP SetData(FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease);
	STDMETHODIMP EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc);
	STDMETHODIMP DAdvise(FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSink,
		DWORD *pdwConnection);
	STDMETHODIMP DUnadvise(DWORD dwConnection);
	STDMETHODIMP EnumDAdvise(IEnumSTATDATA **ppenumAdvise);

private:
	BOOL InitFileGroupDesc();
	BOOL InitFileContents();

	static HGLOBAL * CALLBACK ShouldExtract(LPCSTR pszFile, DWORD dwSize,
		UINT date, UINT time, UINT attribs, LPARAM lParam);

	HRESULT InitContents();

private:
	static UINT s_uFileGroupDesc;
	static UINT s_uFileContents;

	CRefCount m_cRef;

	CRefDll m_cRefDll;

	CCabItemList m_lSel;
	HGLOBAL *m_lContents;

	CCabFolder *m_pcfHere;
	HWND m_hwndOwner;
} ;

#endif // _DATAOBJ_H_
