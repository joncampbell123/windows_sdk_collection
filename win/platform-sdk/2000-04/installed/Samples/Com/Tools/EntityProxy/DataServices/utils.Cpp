#include "stdafx.h"

static WCHAR * const month[] = {L"January ", L"February ", L"March ", L"April ", L"May ", L"June ", L"July ", L"August ", L"September ", L"October ", L"November ", L"December "};

long WriteNamedParam(WCHAR* wszParamStart, LPCWSTR wszName, VARIANT* pVal)
{
	long length;

	if (pVal->vt == VT_BSTR)
	{
		int slen = SysStringLen(pVal->bstrVal);
		
		if (slen > 0)
			length = WriteChars(wszParamStart, wszName, (LPCWSTR)pVal->bstrVal, slen, true);
		else
			length = WriteChars(wszParamStart, wszName, L"\'\'", 0, false);
	}
	else if (pVal->vt == VT_I2)
	{
		length = WriteChars(wszParamStart, wszName, (short)pVal->iVal);
	}
	else if (pVal->vt == VT_DATE)
	{
		SYSTEMTIME systime;
		VariantTimeToSystemTime(pVal->date, &systime);
		length = WriteChars(wszParamStart, wszName, (LPCWSTR)month[systime.wMonth-1], (short)systime.wDay, (short)systime.wYear);
	}
	else if (pVal->vt == VT_NULL)
	{
		length = WriteChars(wszParamStart, wszName, L"NULL", 0, false);
	}
	else if (pVal->vt == VT_I4)
	{
		length = WriteChars(wszParamStart, wszName, (long)pVal->lVal);
	}
	else if (pVal->vt == VT_BOOL)
	{
		long bVal;
		if (pVal->boolVal == VARIANT_TRUE)
			bVal = 1;
		else
			bVal = 0;
		length = WriteChars(wszParamStart, wszName, (long)bVal);
	}
	else if (pVal->vt == VT_EMPTY)
	{
		length = WriteChars(wszParamStart, wszName, (long)0);
	}
	else if (pVal->vt == VT_UI1)
	{
		length = WriteChars(wszParamStart, wszName, (BYTE)pVal->bVal);
	}
	else if (pVal->vt == VT_DECIMAL)
	{
		CFastVariant data;
		VariantChangeType(&data, pVal, 0, VT_BSTR);
		length = WriteChars(wszParamStart, wszName, (LPCWSTR)data.bstrVal, SysStringLen(data.bstrVal), false);
	}
	else if (pVal->vt == VT_CY)
	{
		CFastVariant data;
		VariantChangeType(&data, pVal, 0, VT_BSTR);
		length = WriteChars(wszParamStart, wszName, (LPCWSTR)data.bstrVal, SysStringLen(data.bstrVal), false);
	}
	else if (pVal->vt == VT_R4)
	{
		CFastVariant data;
		VariantChangeType(&data, pVal, 0, VT_BSTR);
		length = WriteChars(wszParamStart, wszName, (LPCWSTR)data.bstrVal, SysStringLen(data.bstrVal), false);
	}
	else if (pVal->vt == VT_R8)
	{
		CFastVariant data;
		VariantChangeType(&data, pVal, 0, VT_BSTR);
		length = WriteChars(wszParamStart, wszName, (LPCWSTR)data.bstrVal, SysStringLen(data.bstrVal), false);
	}
	else
	{
		CFastVariant data;
		VariantChangeType(&data, pVal, 0, VT_BSTR);
		length = WriteChars(wszParamStart, wszName, (LPCWSTR)data.bstrVal, SysStringLen(data.bstrVal), false);
	}
	return length;
}

WCHAR* BuildExecCommand(WCHAR* wszSQLStart, LPCWSTR wszStoredProc, LPCWSTR wszKeyField, VARIANT* pVal)
{
	WCHAR *wszSQL = wszSQLStart;

	if (wszStoredProc[0] != L'\0')
	{
		wszSQL += WriteChars(wszSQL, L"EXEC ");
		wszSQL += WriteChars(wszSQL, wszStoredProc);
		wszSQL[0] = L' ';
		wszSQL[1] = L'@';
		wszSQL += 2;
		wszSQL += WriteNamedParam(wszSQL, wszKeyField, pVal);
	}
	// WriteChars as used in WriteNamedParam automatically adds a comma and a space
	// since the most used case requires it.
	// We don't need it here, so just insert a terminator.
	wszSQL[-2] = L'\0';
	return wszSQLStart;
}

IMPLEMENT_FIXED_ALLOC(CFastVariant, ENTITY_PER_BANK);


