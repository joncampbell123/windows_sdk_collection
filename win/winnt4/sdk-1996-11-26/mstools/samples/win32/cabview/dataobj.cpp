//*******************************************************************************************
//
// Filename : DataObj.cpp
//	
//				Implementation file for CObjFormats and CCabObj
//
// Copyright (c) 1994 - 1996 Microsoft Corporation. All rights reserved
//
//*******************************************************************************************

#include "pch.h"

#include "thisdll.h"

#include "folder.h"
#include "dataobj.h"

#include "cabitms.h"

UINT CCabObj::s_uFileGroupDesc = 0;
UINT CCabObj::s_uFileContents = 0;


class CObjFormats : public IEnumFORMATETC
{
public:
	CObjFormats(UINT cfmt, const FORMATETC afmt[]);
	~CObjFormats();

    // *** IUnknown methods ***
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID * ppvObj);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // *** IEnumFORMATETC methods ***
    STDMETHODIMP Next(ULONG celt, FORMATETC *rgelt, ULONG *pceltFethed);
    STDMETHODIMP Skip(ULONG celt);
    STDMETHODIMP Reset();
    STDMETHODIMP Clone(IEnumFORMATETC ** ppenum);

private:
	CRefCount m_cRef;

	CRefDll m_cRefDll;

	UINT m_iFmt;
	UINT m_cFmt;
	FORMATETC *m_aFmt;
} ;


CObjFormats::CObjFormats(UINT cfmt, const FORMATETC afmt[])
{
	m_iFmt = 0;
	m_cFmt = cfmt;
	m_aFmt = new FORMATETC[cfmt];

	if (m_aFmt)
	{
		CopyMemory(m_aFmt, afmt, cfmt*sizeof(afmt[0]));
	}
}


CObjFormats::~CObjFormats()
{
	if (m_aFmt)
	{
		delete m_aFmt;
	}
}


// *** IUnknown methods ***
STDMETHODIMP CObjFormats::QueryInterface(
   REFIID riid, 
   LPVOID FAR* ppvObj)
{
	*ppvObj = NULL;

	if (!m_aFmt)
	{
		return(E_OUTOFMEMORY);
	}

	LPUNKNOWN pObj;
 
	if (riid == IID_IUnknown)
	{
		pObj = (IUnknown*)((IEnumFORMATETC*)this); 
	}
	else if (riid == IID_IEnumFORMATETC)
	{
		pObj = (IUnknown*)((IEnumFORMATETC*)this); 
	}
	else
	{
   		return(E_NOINTERFACE);
	}

	pObj->AddRef();
	*ppvObj = pObj;

	return(NOERROR);
}


STDMETHODIMP_(ULONG) CObjFormats::AddRef(void)
{
	return(m_cRef.AddRef());
}


STDMETHODIMP_(ULONG) CObjFormats::Release(void)
{
	if (!m_cRef.Release())
	{
   		delete this;
		return(0);
	}

	return(m_cRef.GetRef());
}


STDMETHODIMP CObjFormats::Next(ULONG celt, FORMATETC *rgelt, ULONG *pceltFethed)
{
	UINT cfetch;
	HRESULT hres = S_FALSE;	// assume less numbers

	if (m_iFmt < m_cFmt)
	{
		cfetch = m_cFmt - m_iFmt;
		if (cfetch >= celt)
		{
			cfetch = celt;
			hres = S_OK;
		}

		CopyMemory(rgelt, &m_aFmt[m_iFmt], cfetch * sizeof(FORMATETC));
		m_iFmt += cfetch;
	}
	else
	{
		cfetch = 0;
	}

	if (pceltFethed)
	{
		*pceltFethed = cfetch;
	}

	return hres;
}

STDMETHODIMP CObjFormats::Skip(ULONG celt)
{
	m_iFmt += celt;
	if (m_iFmt > m_cFmt)
	{
		m_iFmt = m_cFmt;
		return S_FALSE;
	}
	return S_OK;
}

STDMETHODIMP CObjFormats::Reset()
{
	m_iFmt = 0;
	return S_OK;
}

STDMETHODIMP CObjFormats::Clone(IEnumFORMATETC ** ppenum)
{
	return(E_NOTIMPL);
}

CCabObj::CCabObj(HWND hwndOwner, CCabFolder *pcf, LPCABITEM *apit, UINT cpit)
: m_lSel(8), m_lContents(NULL)
{
	m_pcfHere = pcf;
	pcf->AddRef();

	m_hwndOwner = hwndOwner;
	m_lSel.AddItems(apit, cpit);
}


CCabObj::~CCabObj()
{
	m_pcfHere->Release();
}


// *** IUnknown methods ***
STDMETHODIMP CCabObj::QueryInterface(
   REFIID riid, 
   LPVOID FAR* ppvObj)
{
	*ppvObj = NULL;

	if (m_lSel.GetState() == CCabItemList::State_OutOfMem)
	{
		return(E_OUTOFMEMORY);
	}

	LPUNKNOWN pObj;
 
	if (riid == IID_IUnknown)
	{
		pObj = (IUnknown*)((IDataObject*)this); 
	}
	else if (riid == IID_IDataObject)
	{
		pObj = (IUnknown*)((IDataObject*)this); 
	}
	else
	{
   		return(E_NOINTERFACE);
	}

	pObj->AddRef();
	*ppvObj = pObj;

	return(NOERROR);
}


STDMETHODIMP_(ULONG) CCabObj::AddRef(void)
{
	return(m_cRef.AddRef());
}


STDMETHODIMP_(ULONG) CCabObj::Release(void)
{
	if (!m_cRef.Release())
	{
   		delete this;
		return(0);
	}

	return(m_cRef.GetRef());
}


STDMETHODIMP CCabObj::GetData(FORMATETC *pfmt, STGMEDIUM *pmedium)
{
	if (!InitFileGroupDesc())
	{
		return(E_UNEXPECTED);
	}

	if (pfmt->cfFormat == s_uFileGroupDesc)
	{
		if (pfmt->ptd==NULL && (pfmt->dwAspect&DVASPECT_CONTENT) && pfmt->lindex==-1
			&& (pfmt->tymed&TYMED_HGLOBAL))
		{
			int cItems = m_lSel.GetCount();
			if (cItems < 1)
			{
				return(E_UNEXPECTED);
			}

			FILEGROUPDESCRIPTOR *pfgd = (FILEGROUPDESCRIPTOR *)GlobalAlloc(GMEM_FIXED,
				sizeof(FILEGROUPDESCRIPTOR) + (cItems-1)*sizeof(FILEDESCRIPTOR));
			if (!pfgd)
			{
				return(E_OUTOFMEMORY);
			}

			pfgd->cItems = cItems;
			for (--cItems; cItems>=0; --cItems)
			{
				LPCABITEM pItem = m_lSel[cItems];

				pfgd->fgd[cItems].dwFlags = FD_ATTRIBUTES|FD_WRITESTIME|FD_FILESIZE;
				pfgd->fgd[cItems].dwFileAttributes = pItem->uFileAttribs;
				DosDateTimeToFileTime(pItem->uFileDate, pItem->uFileTime,
					&pfgd->fgd[cItems].ftLastWriteTime);
				pfgd->fgd[cItems].nFileSizeHigh = 0;
				pfgd->fgd[cItems].nFileSizeLow  = pItem->dwFileSize;
				lstrcpyn(pfgd->fgd[cItems].cFileName, pItem->szName,
					sizeof(pfgd->fgd[cItems].cFileName));
			}

			pmedium->tymed = TYMED_HGLOBAL;
			pmedium->hGlobal = (HGLOBAL)pfgd;
			pmedium->pUnkForRelease = NULL;

			return(NOERROR);
		}

		return(E_INVALIDARG);
	}

	if (!InitFileContents())
	{
		return(E_UNEXPECTED);
	}

	if (pfmt->cfFormat == s_uFileContents)
	{
		if (pfmt->ptd==NULL && (pfmt->dwAspect&DVASPECT_CONTENT)
			&& (pfmt->tymed&TYMED_HGLOBAL))
		{
			int cItems = m_lSel.GetCount();
			if (pfmt->lindex >= cItems)
			{
				return(E_INVALIDARG);
			}

			HRESULT hRes = InitContents();
			if (FAILED(hRes))
			{
				return(hRes);
			}

			if (!m_lContents[pfmt->lindex])
			{
				return(E_OUTOFMEMORY);
			}

			// TODO: Maybe if we run out of memory I will just return an IStream for each
			// file and take the time hit to extract each one separately
			pmedium->tymed = TYMED_HGLOBAL;
			pmedium->hGlobal = m_lContents[pfmt->lindex];
			// We will delete these globals when we go away
			pmedium->pUnkForRelease = (IDataObject*)this;
			AddRef();

			return(NOERROR);
		}

		return(E_INVALIDARG);
	}

	return(E_NOTIMPL);
}


STDMETHODIMP CCabObj::GetDataHere(FORMATETC *pformatetc, STGMEDIUM *pmedium)
{
	return(E_NOTIMPL);
}


STDMETHODIMP CCabObj::QueryGetData(FORMATETC *pfmt)
{
	if (!InitFileGroupDesc())
	{
		return(E_UNEXPECTED);
	}

	if (pfmt->cfFormat == s_uFileGroupDesc)
	{
		if (pfmt->ptd==NULL && (pfmt->dwAspect&DVASPECT_CONTENT) && pfmt->lindex==-1
			&& (pfmt->tymed&TYMED_HGLOBAL))
		{
			return(S_OK);
		}

		return(E_INVALIDARG);
	}

	if (!InitFileContents())
	{
		return(E_UNEXPECTED);
	}

	if (pfmt->cfFormat == s_uFileContents)
	{
		if (pfmt->ptd==NULL && (pfmt->dwAspect&DVASPECT_CONTENT)
			&& (pfmt->tymed&TYMED_HGLOBAL))
		{
			return(S_OK);
		}

		return(E_INVALIDARG);
	}

	return(E_NOTIMPL);
}


STDMETHODIMP CCabObj::GetCanonicalFormatEtc(FORMATETC *pformatectIn, FORMATETC *pformatetcOut)
{
	return(E_NOTIMPL);
}


STDMETHODIMP CCabObj::SetData(FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease)
{
	return(E_NOTIMPL);
}


STDMETHODIMP CCabObj::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc)
{
	if (!InitFileGroupDesc() || !InitFileContents())
	{
		return(E_UNEXPECTED);
	}

    FORMATETC fmte[] = {
        {(USHORT)s_uFileContents,  NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL },
        {(USHORT)s_uFileGroupDesc, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL },
    };

	CObjFormats *pFmts = new CObjFormats(2, fmte);
	if (!pFmts)
	{
		return(E_OUTOFMEMORY);
	}

	pFmts->AddRef();
	HRESULT hRes = pFmts->QueryInterface(IID_IEnumFORMATETC, (LPVOID *)ppenumFormatEtc);
	pFmts->Release();

	return(hRes);
}


STDMETHODIMP CCabObj::DAdvise(FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSink,
	DWORD *pdwConnection)
{
	return(E_NOTIMPL);
}


STDMETHODIMP CCabObj::DUnadvise(DWORD dwConnection)
{
	return(E_NOTIMPL);
}


STDMETHODIMP CCabObj::EnumDAdvise(IEnumSTATDATA **ppenumAdvise)
{
	return(E_NOTIMPL);
}


BOOL CCabObj::InitFileGroupDesc()
{
	if (s_uFileGroupDesc)
	{
		return(TRUE);
	}

	s_uFileGroupDesc = RegisterClipboardFormat(CFSTR_FILEDESCRIPTOR);
	return(s_uFileGroupDesc != 0);
}


BOOL CCabObj::InitFileContents()
{
	if (s_uFileContents)
	{
		return(TRUE);
	}

	s_uFileContents = RegisterClipboardFormat(CFSTR_FILECONTENTS);
	return(s_uFileContents != 0);
}


HGLOBAL * CALLBACK CCabObj::ShouldExtract(LPCSTR pszFile, DWORD dwSize, UINT date,
		UINT time, UINT attribs, LPARAM lParam)
{
	CCabObj *pThis = (CCabObj*)lParam;

	int iItem = pThis->m_lSel.FindInList(pszFile, dwSize, date, time, attribs);
	if (iItem < 0)
	{
		return(EXTRACT_FALSE);
	}

	// Copy nothing for now
	return(&(pThis->m_lContents[iItem]));
}


HRESULT CCabObj::InitContents()
{
	if (m_lContents)
	{
		return(NOERROR);
	}

	int iCount = m_lSel.GetCount();
	if (iCount < 1)
	{
		return(E_UNEXPECTED);
	}

	m_lContents = (HGLOBAL *)GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT,
		sizeof(HGLOBAL)*m_lSel.GetCount());
	if (!m_lContents)
	{
		return(E_OUTOFMEMORY);
	}

	char szHere[MAX_PATH];
	if (!m_pcfHere->GetPath(szHere))
	{
		return(E_UNEXPECTED);
	}

	CCabExtract ceHere(szHere);

	ceHere.ExtractItems(m_hwndOwner, DIR_MEM, ShouldExtract, (LPARAM)this);

	return(TRUE);
}
