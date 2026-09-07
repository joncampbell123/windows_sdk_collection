// Static.cpp : Implementation of CEntity
#include "stdafx.h"
#include "Data.h"
#include "Entity.h"

/////////////////////////////////////////////////////////////////////////////
// CEntity functions that need exceptions turned on

STDMETHODIMP CEntity::put_Recordset(_Recordset *newVal)
{
	HRESULT hr = S_OK;
	try
	{
		_RecordsetPtr spRecordset = newVal;
		FieldsPtr spFields = spRecordset->Fields;
		long nRSCount = spFields->Count;
		long iCol;

		if (spRecordset->GetadoEOF() != VARIANT_TRUE)
		{
			hr = Initialize(nRSCount);
			if (SUCCEEDED(hr))
			{
				for (iCol = 0; iCol < nRSCount && SUCCEEDED(hr); iCol++)
				{ 
					hr = Insert(iCol + 1, spFields->Item[iCol]->Name, &spFields->Item[iCol]->Value);
				}
			}
		}
	}
	catch (_com_error e)
	{
		hr = e.Error();
	}
	return hr;
}


