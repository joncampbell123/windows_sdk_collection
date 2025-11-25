#ifndef _CDATAOBJECT_H_
#define _CDATAOBJECT_H_

class CDocument;

class CDataObject : public IDataObject  
{
	public:

	unsigned long			m_cRef;
	CDocument*				m_pDocument;

	CDataObject(CDocument * pDoc);
	~CDataObject(void);

    // *** IUnknown methods ***
    virtual HRESULT STDMETHODCALLTYPE	QueryInterface(REFIID riid, void * * ppvObj);
    virtual unsigned long STDMETHODCALLTYPE		AddRef(void);
    virtual unsigned long STDMETHODCALLTYPE		Release(void);

    // *** IDataObject methods ***
    virtual HRESULT STDMETHODCALLTYPE	GetData(LPFORMATETC pformatetcIn,
										LPSTGMEDIUM pmedium );
    virtual HRESULT STDMETHODCALLTYPE	GetDataHere(LPFORMATETC pformatetc,
										LPSTGMEDIUM pmedium );
    virtual HRESULT STDMETHODCALLTYPE	QueryGetData(LPFORMATETC pformatetc );
    virtual HRESULT STDMETHODCALLTYPE	GetCanonicalFormatEtc(LPFORMATETC pformatetc,
										LPFORMATETC pformatetcOut);
    virtual HRESULT STDMETHODCALLTYPE	SetData(LPFORMATETC pformatetc, STGMEDIUM  * pmedium,
										unsigned long fRelease);
    virtual HRESULT STDMETHODCALLTYPE	EnumFormatEtc(unsigned long dwDirection,
										LPLPENUMFORMATETC ppenumFormatEtc);

    virtual HRESULT STDMETHODCALLTYPE	DAdvise(FORMATETC * pFormatetc, unsigned long advf, 
										LPADVISESINK pAdvSink, unsigned long * pdwConnection);
    virtual HRESULT STDMETHODCALLTYPE	DUnadvise(unsigned long dwConnection);
    virtual HRESULT STDMETHODCALLTYPE	EnumDAdvise(LPLPENUMSTATDATA ppenumAdvise);
};

#endif // _CDATAOBJECT_H_