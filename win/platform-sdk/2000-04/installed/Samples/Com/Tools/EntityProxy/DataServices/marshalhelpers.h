#include <comdef.h>

#pragma warning( disable : 4290 )

// This code is derived from the CComVariant implementation
// of WriteToStream
inline unsigned long GetVariantSize(VARIANT& var)
{
	VARTYPE vt = var.vt;
	unsigned long sz = sizeof(VARTYPE);
	HRESULT hr = 0;

	switch (vt)
	{
	case VT_UNKNOWN:
	case VT_DISPATCH:
		{
			{
				IUnknown* punkVal = var.punkVal;		
				CComPtr<IPersistStream> spStream;
				if (punkVal != NULL)
				{
					hr = punkVal->QueryInterface(IID_IPersistStream, (void**)&spStream);
					if (FAILED(hr))
						return sz;
					ULARGE_INTEGER pcbSize;
					hr = spStream->GetSizeMax(&pcbSize);
					if (FAILED(hr))
						return sz;
					sz += pcbSize.LowPart;
					sz += 256;
				}
			}
		}
	case VT_UI1:
	case VT_I1:
		sz  +=  sizeof(BYTE);
		break;
	case VT_I2:
	case VT_UI2:
	case VT_BOOL:
		sz  +=  sizeof(short);
		break;
	case VT_I4:
	case VT_UI4:
	case VT_R4:
	case VT_INT:
	case VT_UINT:
	case VT_ERROR:
	case VT_NULL:
		sz  +=  sizeof(long);
		break;
	case VT_R8:
	case VT_CY:
	case VT_DATE:
		sz  +=  sizeof(double);
		break;
	case VT_BSTR:
		sz += ((SysStringLen(var.bstrVal) * sizeof(OLECHAR))+6);
		break;
	case VT_VARIANT:
		sz += sizeof(VARIANT);
		sz += GetVariantSize(*(var.pvarVal));
	default:
		break;
	}
	return sz;
}

// IStream / C++ stream operator support functions

template <class _T_> IStream& operator<<( IStream& stream, const _T_& data ) throw(_com_error)
{
	_com_util::CheckError(stream.Write( &data, sizeof(data), 0));
	return stream;
}

template <class _T_> IStream& operator>>( IStream& stream, _T_& data ) throw(_com_error)
{
	_com_util::CheckError(stream.Read( &data, sizeof(data), 0));
	return stream;
}

inline IStream& operator<<( IStream& stream, CFastVariant& data ) throw(_com_error)
{
	_com_util::CheckError(data.WriteToStream( &stream ));
	return stream;
}

inline IStream& operator>>( IStream& stream, CFastVariant& data ) throw(_com_error)
{
	_com_util::CheckError(data.ReadFromStream( &stream ));
	return stream;
}

inline IStream& operator<<( IStream& stream, VARIANT data ) throw(_com_error)
{
	CFastVariant v;
	v.Attach(&data);
	
	stream<<v;

	v.Detach(&data);

	return stream;
}

inline IStream& operator>>( IStream& stream, VARIANT& data ) throw(_com_error)
{
	CFastVariant v;

	stream>>v;

	v.Detach(&data);
	return stream;
}

inline IStream& operator<<( IStream& stream, const char* data ) throw(_com_error)
{
	long length = strlen(data) + 1;

	stream<<length;
	_com_util::CheckError(stream.Write( data, length*sizeof(*data), 0));

	return stream;
}

inline IStream& operator>>( IStream& stream, char* data ) throw(_com_error)
{
	long length;

	stream>>length;
	_com_util::CheckError(stream.Read( data, length*sizeof(*data), 0));
	return stream;
}

inline IStream& operator<<( IStream& stream, const wchar_t *data ) throw(_com_error)
{
	long length = wcslen(data) + 1;

	stream << length;
	_com_util::CheckError(stream.Write( data, length*sizeof(*data), 0));

	return stream;
}

inline IStream& operator>>( IStream& stream, wchar_t *data ) throw(_com_error)
{
	long length;

	stream>>length;
	_com_util::CheckError(stream.Read( data, length * sizeof(*data), 0));
	return stream;
}

inline IStream& operator>>( IStream& stream, CString &data ) throw(_com_error)
{
	long length;
	LPWSTR wszData;

	stream>>length;

	wszData = data.GetBuffer(length+1);

	_com_util::CheckError(stream.Read( (void*)wszData, length * sizeof(*wszData), 0));

	data.ReleaseBuffer();
	return stream;
}

inline IStream& operator<<( IStream& stream, const _bstr_t& data ) throw(_com_error)
{
	CComBSTR bstr;

	bstr.Attach(data);

	_com_util::CheckError(bstr.WriteToStream(&stream));
	bstr.Detach();

	return stream;
}

inline IStream& operator>>( IStream& stream, _bstr_t& data ) throw(_com_error)
{
	CComBSTR bstr;

	_com_util::CheckError(bstr.ReadFromStream(&stream));

	_bstr_t  bs(bstr.Detach(),false);	// use ref counting to prevent copy

	data = bs;

	return stream;
}

inline IStream& operator>>( IStream& stream, CComBSTR& data ) throw(_com_error)
{
	_com_util::CheckError(data.ReadFromStream(&stream));
	return stream;
}

inline IStream& operator<<( IStream& stream, CComBSTR& data ) throw(_com_error)
{
	_com_util::CheckError(data.WriteToStream(&stream));
	return stream;
}

#pragma warning( default : 4290 )
