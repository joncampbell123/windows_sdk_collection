#ifndef __UTILS_H_
#define __UTILS_H_

extern WCHAR * const month[];

inline long WriteNamedParam(WCHAR* wszParamStart, LPCWSTR wszName, VARIANT* pVal);
WCHAR* BuildExecCommand(WCHAR* wszSQL, LPCWSTR wszStoredProc, LPCWSTR wszKeyField, VARIANT* pVal);

// This Variant wrapper is based on the CComVariant code
// from ATL, however it has been modified to support
// a fixed allocator, and has been modified to use OLEAUT32.DLL
// as little as possible.
class CFastVariant : public tagVARIANT
{
	DECLARE_FIXED_ALLOC(CFastVariant);
// Constructors
public:
	CFastVariant()
	{
		vt = VT_EMPTY;
	}
	~CFastVariant()
	{
		Clear();
	}

	CFastVariant(const VARIANT& varSrc)
	{
		vt = VT_EMPTY;
		InternalCopy(&varSrc);
	}

	CFastVariant(const CFastVariant& varSrc)
	{
		vt = VT_EMPTY;
		InternalCopy(&varSrc);
	}

	CFastVariant(BSTR bstrSrc)
	{
		vt = VT_EMPTY;
		*this = bstrSrc;
	}
	CFastVariant(LPCOLESTR lpszSrc)
	{
		vt = VT_EMPTY;
		*this = lpszSrc;
	}

#ifndef OLE2ANSI
	CFastVariant(LPCSTR lpszSrc)
	{
		vt = VT_EMPTY;
		*this = lpszSrc;
	}
#endif

	CFastVariant(bool bSrc)
	{
		vt = VT_BOOL;
#pragma warning(disable: 4310) // cast truncates constant value
		boolVal = bSrc ? VARIANT_TRUE : VARIANT_FALSE;
#pragma warning(default: 4310) // cast truncates constant value
	}

	CFastVariant(int nSrc)
	{
		vt = VT_I4;
		lVal = nSrc;
	}
	CFastVariant(BYTE nSrc)
	{
		vt = VT_UI1;
		bVal = nSrc;
	}
	CFastVariant(short nSrc)
	{
		vt = VT_I2;
		iVal = nSrc;
	}
	CFastVariant(long nSrc, VARTYPE vtSrc = VT_I4)
	{
		ATLASSERT(vtSrc == VT_I4 || vtSrc == VT_ERROR);
		vt = vtSrc;
		lVal = nSrc;
	}
	CFastVariant(float fltSrc)
	{
		vt = VT_R4;
		fltVal = fltSrc;
	}
	CFastVariant(double dblSrc)
	{
		vt = VT_R8;
		dblVal = dblSrc;
	}
	CFastVariant(CY cySrc)
	{
		vt = VT_CY;
		cyVal.Hi = cySrc.Hi;
		cyVal.Lo = cySrc.Lo;
	}
	CFastVariant(IDispatch* pSrc)
	{
		vt = VT_DISPATCH;
		pdispVal = pSrc;
		// Need to AddRef as VariantClear will Release
		if (pdispVal != NULL)
			pdispVal->AddRef();
	}
	CFastVariant(IUnknown* pSrc)
	{
		vt = VT_UNKNOWN;
		punkVal = pSrc;
		// Need to AddRef as VariantClear will Release
		if (punkVal != NULL)
			punkVal->AddRef();
	}

// Assignment Operators
public:
	CFastVariant& operator=(const CFastVariant& varSrc)
	{
		InternalCopy(&varSrc);
		return *this;
	}
	CFastVariant& operator=(const VARIANT& varSrc)
	{
		InternalCopy(&varSrc);
		return *this;
	}

	CFastVariant& operator=(BSTR bstrSrc)
	{
		InternalClear();
		vt = VT_BSTR;
		bstrVal = ::SysAllocString(bstrSrc);
		if (bstrVal == NULL && bstrSrc != NULL)
		{
			vt = VT_ERROR;
			scode = E_OUTOFMEMORY;
		}
		return *this;
	}

	CFastVariant& operator=(LPCOLESTR lpszSrc)
	{
		InternalClear();
		vt = VT_BSTR;
		bstrVal = ::SysAllocString(lpszSrc);

		if (bstrVal == NULL && lpszSrc != NULL)
		{
			vt = VT_ERROR;
			scode = E_OUTOFMEMORY;
		}
		return *this;
	}

	#ifndef OLE2ANSI
	CFastVariant& operator=(LPCSTR lpszSrc)
	{
		USES_CONVERSION;
		InternalClear();
		vt = VT_BSTR;
		bstrVal = ::SysAllocString(A2COLE(lpszSrc));

		if (bstrVal == NULL && lpszSrc != NULL)
		{
			vt = VT_ERROR;
			scode = E_OUTOFMEMORY;
		}
		return *this;
	}
	#endif

	CFastVariant& operator=(bool bSrc)
	{
		if (vt != VT_BOOL)
		{
			InternalClear();
			vt = VT_BOOL;
		}
	#pragma warning(disable: 4310) // cast truncates constant value
		boolVal = bSrc ? VARIANT_TRUE : VARIANT_FALSE;
	#pragma warning(default: 4310) // cast truncates constant value
		return *this;
	}

	CFastVariant& operator=(int nSrc)
	{
		if (vt != VT_I4)
		{
			InternalClear();
			vt = VT_I4;
		}
		lVal = nSrc;

		return *this;
	}

	CFastVariant& operator=(BYTE nSrc)
	{
		if (vt != VT_UI1)
		{
			InternalClear();
			vt = VT_UI1;
		}
		bVal = nSrc;
		return *this;
	}

	CFastVariant& operator=(short nSrc)
	{
		if (vt != VT_I2)
		{
			InternalClear();
			vt = VT_I2;
		}
		iVal = nSrc;
		return *this;
	}

	CFastVariant& operator=(long nSrc)
	{
		if (vt != VT_I4)
		{
			InternalClear();
			vt = VT_I4;
		}
		lVal = nSrc;
		return *this;
	}

	CFastVariant& operator=(float fltSrc)
	{
		if (vt != VT_R4)
		{
			InternalClear();
			vt = VT_R4;
		}
		fltVal = fltSrc;
		return *this;
	}

	CFastVariant& operator=(double dblSrc)
	{
		if (vt != VT_R8)
		{
			InternalClear();
			vt = VT_R8;
		}
		dblVal = dblSrc;
		return *this;
	}

	CFastVariant& operator=(CY cySrc)
	{
		if (vt != VT_CY)
		{
			InternalClear();
			vt = VT_CY;
		}
		cyVal.Hi = cySrc.Hi;
		cyVal.Lo = cySrc.Lo;
		return *this;
	}

	CFastVariant& operator=(IDispatch* pSrc)
	{
		InternalClear();
		vt = VT_DISPATCH;
		pdispVal = pSrc;
		// Need to AddRef as VariantClear will Release
		if (pdispVal != NULL)
			pdispVal->AddRef();
		return *this;
	}

	CFastVariant& operator=(IUnknown* pSrc)
	{
		InternalClear();
		vt = VT_UNKNOWN;
		punkVal = pSrc;

		// Need to AddRef as VariantClear will Release
		if (punkVal != NULL)
			punkVal->AddRef();
		return *this;
	}


// Comparison Operators
public:
	bool operator==(const VARIANT& varSrc) const
	{
		if (this == &varSrc)
			return true;

		// Variants not equal if types don't match
		if (vt != varSrc.vt)
			return false;

		// Check type specific values
		switch (vt)
		{
			case VT_EMPTY:
			case VT_NULL:
				return true;

			case VT_BOOL:
				return boolVal == varSrc.boolVal;

			case VT_UI1:
				return bVal == varSrc.bVal;

			case VT_I2:
				return iVal == varSrc.iVal;

			case VT_I4:
				return lVal == varSrc.lVal;

			case VT_R4:
				return fltVal == varSrc.fltVal;

			case VT_R8:
				return dblVal == varSrc.dblVal;

			case VT_BSTR:
				return (::SysStringByteLen(bstrVal) == ::SysStringByteLen(varSrc.bstrVal)) &&
						(::memcmp(bstrVal, varSrc.bstrVal, ::SysStringByteLen(bstrVal)) == 0);

			case VT_ERROR:
				return scode == varSrc.scode;

			case VT_DISPATCH:
				return pdispVal == varSrc.pdispVal;

			case VT_UNKNOWN:
				return punkVal == varSrc.punkVal;

			default:
				ATLASSERT(false);
				// fall through
		}

		return false;
	}
//	bool operator!=(const VARIANT& varSrc) const {return !operator==(varSrc);}
//	bool operator<(const VARIANT& varSrc) const {return VarCmp((VARIANT*)this, (VARIANT*)&varSrc, LOCALE_USER_DEFAULT)==VARCMP_LT;}
//	bool operator>(const VARIANT& varSrc) const {return VarCmp((VARIANT*)this, (VARIANT*)&varSrc, LOCALE_USER_DEFAULT)==VARCMP_GT;}

// Interim Fix for VC6 ATL
	bool operator!=(const VARIANT& varSrc) const {return VarCmp((VARIANT*)this, (VARIANT*)&varSrc, LOCALE_USER_DEFAULT, 0)!=VARCMP_EQ;}
	//bool operator==(const VARIANT& varSrc) const {return VarCmp((VARIANT*)this, (VARIANT*)&varSrc, LOCALE_USER_DEFAULT, 0)==VARCMP_EQ;}
	bool operator<(const VARIANT& varSrc) const {return VarCmp((VARIANT*)this, (VARIANT*)&varSrc, LOCALE_USER_DEFAULT, 0)==VARCMP_LT;}
	bool operator>(const VARIANT& varSrc) const {return VarCmp((VARIANT*)this, (VARIANT*)&varSrc, LOCALE_USER_DEFAULT, 0)==VARCMP_GT;}



// Operations
public:
	HRESULT Clear()
	{
		if (vt != VT_EMPTY)
			return ::VariantClear(this);
		else
			return S_OK;
	}

	HRESULT Copy(const VARIANT* pSrc)
	{
		return ::VariantCopy(this, const_cast<VARIANT*>(pSrc));
	}

	HRESULT Attach(VARIANT* pSrc)
	{
		// Clear out the variant
		HRESULT hr = Clear();
		if (!FAILED(hr))
		{
			// Copy the contents and give control to CFastVariant
			memcpy(this, pSrc, sizeof(VARIANT));
			pSrc->vt = VT_EMPTY;
			hr = S_OK;
		}
		return hr;
	}

	HRESULT Detach(VARIANT* pDest)
	{
		HRESULT hr = S_OK;
		if (pDest->vt != VT_EMPTY)
			hr = ::VariantClear(pDest);
		// Clear out the variant
		if (!FAILED(hr))
		{
			// Copy the contents and remove control from CFastVariant
			memcpy(pDest, this, sizeof(VARIANT));
			vt = VT_EMPTY;
			hr = S_OK;
		}
		return hr;
	}

	HRESULT ChangeType(VARTYPE vtNew, const VARIANT* pSrc = NULL)
	{
		VARIANT* pVar = const_cast<VARIANT*>(pSrc);
		// Convert in place if pSrc is NULL
		if (pVar == NULL)
			pVar = this;
		// Do nothing if doing in place convert and vts not different
		return ::VariantChangeType(this, pVar, 0, vtNew);
	}

	HRESULT WriteToStream(IStream* pStream);
	HRESULT ReadFromStream(IStream* pStream);

// Implementation
public:
	HRESULT InternalClear()
	{
		HRESULT hr = Clear();
		ATLASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
		{
			vt = VT_ERROR;
			scode = hr;
		}
		return hr;
	}

	void InternalCopy(const VARIANT* pSrc)
	{
		HRESULT hr = Copy(pSrc);
		if (FAILED(hr))
		{
			vt = VT_ERROR;
			scode = hr;
		}
	}
};

inline HRESULT CFastVariant::WriteToStream(IStream* pStream)
{
	HRESULT hr = pStream->Write(&vt, sizeof(VARTYPE), NULL);
	if (FAILED(hr))
		return hr;

	int cbWrite = 0;
	switch (vt)
	{
	case VT_UNKNOWN:
	case VT_DISPATCH:
		{
			CComPtr<IPersistStream> spStream;
			if (punkVal != NULL)
			{
				hr = punkVal->QueryInterface(IID_IPersistStream, (void**)&spStream);
				if (FAILED(hr))
					return hr;
			}
			if (spStream != NULL)
				return OleSaveToStream(spStream, pStream);
			else
				return WriteClassStm(pStream, CLSID_NULL);
		}
	case VT_UI1:
	case VT_I1:
		cbWrite = sizeof(BYTE);
		break;
	case VT_I2:
	case VT_UI2:
	case VT_BOOL:
		cbWrite = sizeof(short);
		break;
	case VT_I4:
	case VT_UI4:
	case VT_R4:
	case VT_INT:
	case VT_UINT:
	case VT_ERROR:
	case VT_NULL:
		cbWrite = sizeof(long);
		break;
	case VT_R8:
	case VT_CY:
	case VT_DATE:
		cbWrite = sizeof(double);
		break;
	default:
		break;
	}
	if (cbWrite != 0)
		return pStream->Write((void*) &bVal, cbWrite, NULL);

	CComBSTR bstrWrite;
	CFastVariant varBSTR;
	if (vt != VT_BSTR)
	{
		hr = VariantChangeType(&varBSTR, this, VARIANT_NOVALUEPROP, VT_BSTR);
		if (FAILED(hr))
			return hr;
		bstrWrite = varBSTR.bstrVal;
	}
	else
		bstrWrite = bstrVal;

	return bstrWrite.WriteToStream(pStream);
}

inline HRESULT CFastVariant::ReadFromStream(IStream* pStream)
{
	ATLASSERT(pStream != NULL);
	HRESULT hr = S_OK;
	if (vt != VT_EMPTY)
		hr = VariantClear(this);
	if (FAILED(hr))
		return hr;
	VARTYPE vtRead;
	hr = pStream->Read(&vtRead, sizeof(VARTYPE), NULL);
	if (hr == S_FALSE)
		hr = E_FAIL;
	if (FAILED(hr))
		return hr;

	vt = vtRead;
	int cbRead = 0;
	switch (vtRead)
	{
	case VT_UNKNOWN:
	case VT_DISPATCH:
		{
			punkVal = NULL;
			hr = OleLoadFromStream(pStream,
				(vtRead == VT_UNKNOWN) ? IID_IUnknown : IID_IDispatch,
				(void**)&punkVal);
			if (hr == REGDB_E_CLASSNOTREG)
				hr = S_OK;
			return S_OK;
		}
	case VT_UI1:
	case VT_I1:
		cbRead = sizeof(BYTE);
		break;
	case VT_I2:
	case VT_UI2:
	case VT_BOOL:
		cbRead = sizeof(short);
		break;
	case VT_I4:
	case VT_UI4:
	case VT_R4:
	case VT_INT:
	case VT_UINT:
	case VT_ERROR:
	case VT_NULL:
		cbRead = sizeof(long);
		break;
	case VT_R8:
	case VT_CY:
	case VT_DATE:
		cbRead = sizeof(double);
		break;
	default:
		break;
	}
	if (cbRead != 0)
	{
		hr = pStream->Read((void*) &bVal, cbRead, NULL);
		if (hr == S_FALSE)
			hr = E_FAIL;
		return hr;
	}
	CComBSTR bstrRead;

	hr = bstrRead.ReadFromStream(pStream);
	if (FAILED(hr))
		return hr;
	vt = VT_BSTR;
	bstrVal = bstrRead.Detach();
	if (vtRead != VT_BSTR)
		hr = ChangeType(vtRead);
	return hr;
}

#endif
