#ifndef STF_LITE
#include <windows.h>
#ifdef WIN32
#include <malloc.h>
#endif
#include "setupapi.h"
#include "msregdb.h"

HANDLE   hinstDLL = NULL;

#ifdef WIN16
typedef LONG (APIENTRY *LPFNRCLK)(LONG);
typedef LONG (APIENTRY *LPFNRCRK)(LONG, LPSTR, LPSTR);
typedef LONG (APIENTRY *LPFNRDLK)(LONG, LPSTR);
typedef LONG (APIENTRY *LPFNROPK)(LONG, LPSTR, LPSTR);
typedef LONG (APIENTRY *LPFNRQRV)(LONG, LPSTR, LPSTR, LPSTR);
typedef LONG (APIENTRY *LPFNRSTV)(LONG, LPSTR, LONG, LPSTR, LONG);

LPFNRCLK lpfnRegCloseKey;
LPFNRCRK lpfnRegCreateKey;
LPFNRDLK lpfnRegDeleteKey;
LPFNROPK lpfnRegOpenKey;
LPFNRQRV lpfnRegQueryValue;
LPFNRSTV lpfnRegSetValue;
#endif

#ifdef WIN32
typedef LONG (APIENTRY *LPFNRCLK)(HKEY);
typedef LONG (APIENTRY *LPFNRCRK)(HKEY, LPCTSTR, PHKEY);
typedef LONG (APIENTRY *LPFNRDLK)(HKEY, LPCTSTR);
typedef LONG (APIENTRY *LPFNROPK)(HKEY, LPCTSTR, PHKEY);
typedef LONG (APIENTRY *LPFNRQRV)(HKEY, LPCTSTR, LPTSTR, PLONG);
typedef LONG (APIENTRY *LPFNRSTV)(HKEY, LPCTSTR, DWORD, LPTSTR, DWORD);
typedef LONG (APIENTRY *LPFNRCRKe)(HKEY, LPCTSTR, DWORD, LPTSTR, DWORD,
				REGSAM, LPSECURITY_ATTRIBUTES, PHKEY, LPDWORD);
typedef LONG (APIENTRY *LPFNROPKe)(HKEY, LPCTSTR, DWORD, REGSAM, PHKEY);
typedef LONG (APIENTRY *LPFNRQRVe)(HKEY, LPTSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
typedef LONG (APIENTRY *LPFNRSTVe)(HKEY, LPCTSTR, DWORD, DWORD, LPBYTE, DWORD);
typedef LONG (APIENTRY *LPFNREK)(HKEY, DWORD, LPTSTR, LPDWORD);
typedef LONG (APIENTRY *LPFNRQIK)(HKEY, LPTSTR, LPDWORD, LPDWORD, LPDWORD,
		LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, PFILETIME);

LPFNRCLK lpfnRegCloseKey;
LPFNRCRK lpfnRegCreateKey;
LPFNRDLK lpfnRegDeleteKey;
LPFNROPK lpfnRegOpenKey;
LPFNRQRV lpfnRegQueryValue;
LPFNRSTV lpfnRegSetValue;
LPFNRCRKe lpfnRegCreateKeyEx;
LPFNROPKe lpfnRegOpenKeyEx;
LPFNRQRVe lpfnRegQueryValueEx;
LPFNRSTVe lpfnRegSetValueEx;
LPFNREK lpfnRegEnumKey;
LPFNRQIK lpfnRegQueryInfoKey;
#endif

// **************************************************************************
void CreateRegKey(LPSTR szKey)
{
	HKEY hKey;
	INT i;

	if ((*lpfnRegCreateKey)(HKEY_CLASSES_ROOT, szKey, &hKey)
			> ERROR_SUCCESS)
		{
		i = EercErrorHandler(GRC_API_FAILED, 1, "CreateRegKey", NULL, NULL);
#ifdef DEBUG
		StfApiErr(saeFail, "CreateRegKey", szKey);
#endif // DEBUG
		SetupError(STFERR);
		}

	if ((*lpfnRegCloseKey)(hKey) > ERROR_SUCCESS)
		{
		i = EercErrorHandler(GRC_API_FAILED, 1, "CreateRegKey", NULL, NULL);
#ifdef DEBUG
		StfApiErr(saeFail, "CreateRegKey", szKey);
#endif // DEBUG
		SetupError(STFERR);
		}
}


// **************************************************************************
void CreateRegKeyValue(LPSTR szKey, LPSTR szValue)
{
	if ((*lpfnRegSetValue)(HKEY_CLASSES_ROOT, (LPCTSTR)szKey, REG_SZ, szValue,
			lstrlen(szKey)) > ERROR_SUCCESS)
		{
		EercErrorHandler(GRC_API_FAILED, 1, "CreateRegKeyValue", NULL, NULL);
#ifdef DEBUG
		StfApiErr(saeFail, "CreateRegKeyValue",SzCat2Str((LPSTR)szKey,", ",(LPSTR)szValue));
#endif // DEBUG
		SetupError(STFERR);
		}
}


// **************************************************************************
INT DoesRegKeyExist(LPSTR szKey)
{
	HKEY hKey;

	if ((*lpfnRegOpenKey)(HKEY_CLASSES_ROOT, szKey, &hKey)
			!= ERROR_SUCCESS)
		return(0);

	(*lpfnRegCloseKey)(hKey);

	return(1);
}


// **************************************************************************
void SetRegKeyValue(LPSTR szKey, LPSTR szValue)
{
	if ((*lpfnRegSetValue)(HKEY_CLASSES_ROOT, szKey, REG_SZ,  szValue,
			lstrlen(szKey)) > ERROR_SUCCESS)
		{
		EercErrorHandler(GRC_API_FAILED, 1, "SetRegKeyValue", NULL, NULL);
#ifdef DEBUG
		StfApiErr(saeFail, "SetRegKeyValue", SzCat2Str((LPSTR)szKey, ", ", (LPSTR)szValue));
#endif // DEBUG
		SetupError(STFERR);
		}
}


// **************************************************************************
LPSTR GetRegKeyValue(LPSTR szKey, LPSTR szBuf, INT cbBuf)
{
	LONG cb = cbBuf;

	if (szBuf != NULL && cbBuf > 0)
		*szBuf = '\0';

	if (!DoesRegKeyExist(szKey))
		return(szBuf);

	if ((*lpfnRegQueryValue)(HKEY_CLASSES_ROOT, szKey, szBuf, &cb)
			!= ERROR_SUCCESS)
		{
		EercErrorHandler(GRC_API_FAILED, 1, "SetRegKeyValue", NULL, NULL);
#ifdef DEBUG
		StfApiErr(saeFail, "GetRegKeyValue", szKey);
#endif // DEBUG
		SetupError(STFERR);
		}

	if (cb > cbBuf)
		{
		DoMsgBox("Buffer Overflow", "MS-Setup Error", MB_ICONHAND+MB_OK);
		SetupError(STFERR);
		}
	*(szBuf + cb) = '\0';

	return(szBuf);
}


// **************************************************************************
void DeleteRegKey(LPSTR szKey)
{
#ifdef WIN16
    (*lpfnRegDeleteKey)(HKEY_CLASSES_ROOT, szKey);
#elif defined(WIN32)
    if (DoesRegKeyExistEx(HKEY_CLASSES_ROOT, szKey))
	{
	//
	// The Win32 "RegDeleteKey()" function will not delete a key
	// that has existing subkeys (unlike the Win16 version).
	// DeleteAllSubkeys is a recursive function that removes a
	// complete "tree" of registry keys.
	//
	if (!DeleteAllSubKeys(HKEY_CLASSES_ROOT, (LPCTSTR)szKey))
	    {
	    EercErrorHandler(GRC_API_FAILED, 1, "DeleteAllSubKeys", NULL, NULL);
#ifdef DEBUG
	    StfApiErr(saeFail, "DeleteAllSubKeys", (LPSTR)szKey);
#endif // DEBUG
	    SetupError(STFERR);
	    }
	}
#endif
}



#ifdef WIN32

//
// All the functions that follow are "extended" functions for
// Win32.  They are not limited to the HKEY_CLASSES_ROOT key
// as the original Setup Toolkit functions are.
//

BOOL DeleteAllSubKeys(HKEY hKey, LPCTSTR szKey)
{
    HKEY hKey2;
    TCHAR ClassStr[128];
    LPTSTR szClass = ClassStr;
    LPTSTR szSubKey;
    LPTSTR szFullSubKey;
    LONG lResult;
    DWORD cchClass, dwNumSubKeys, cchMaxSubKey, cchMaxClass,
		cValues, cchMaxValName, cbMaxValData, cbSecurityDescriptor;
    DWORD i, cchName;
    FILETIME ftLastWrite;

    if ((lResult = (*lpfnRegOpenKeyEx)(hKey, szKey,
				0, KEY_READ, &hKey2)) > ERROR_SUCCESS)
	{
	EercErrorHandler(GRC_API_FAILED, 1, "DeleteAllSubKeys:open", NULL, NULL);
#ifdef DEBUG
	StfApiErr(saeFail, "DeleteAllSubKeys:open", (LPSTR)szKey);
#endif // DEBUG
	return(FALSE);
	}

    //
    // First, see how many subkeys this key has.
    //
    cchClass=128;
    if ((lResult = (*lpfnRegQueryInfoKey)(hKey2, szClass, &cchClass, NULL, &dwNumSubKeys, 
		&cchMaxSubKey, &cchMaxClass, &cValues, &cchMaxValName,
		&cbMaxValData, &cbSecurityDescriptor, &ftLastWrite)) != ERROR_SUCCESS)
	{
	(*lpfnRegCloseKey)(hKey2);
	return(FALSE);
	}

    //
    // if this key has no subkeys at all, we can remove
    // it immediately and finish.
    //
    if (dwNumSubKeys == 0)
	{
	(*lpfnRegCloseKey)(hKey2);
	lResult = (*lpfnRegDeleteKey)(hKey, szKey);
	}
    else
	{
	//
	// This key has subkeys; we'll need to enumerate them
	// and "zap" each one in turn.
	//
	// RegQueryInfoKey() has indicated what the maximum length
	// of a subkey name will be for this particular key
	// in "cchMaxSubKey", so we can allocate just what we
	// need to hold the names.
	//
	szSubKey = (LPTSTR)malloc((cchMaxSubKey+1) * sizeof(TCHAR));
	szFullSubKey = (LPTSTR)malloc((cchMaxSubKey + lstrlen(szKey) + 2)*sizeof(TCHAR));

	for (i = dwNumSubKeys; i > 0; i--)
	    {
	    cchName = cchMaxSubKey;
	    if ((lResult = (*lpfnRegEnumKey)(hKey2, i - 1,
			szSubKey, &cchName)) == ERROR_SUCCESS)
		{
		//
		// Construct the full hierarchical key name, and
		// nuke it.
		// (RegEnumKey just returns the name of the subkey)
		//
		wsprintf(szFullSubKey, "%s\\%s", szKey, szSubKey);
		if (!DeleteAllSubKeys(hKey, szFullSubKey))
		    {
		    (*lpfnRegCloseKey)(hKey2);
		    free(szSubKey);
		    free(szFullSubKey);
		    return(FALSE);
		    }
		}
	    else
		{
		if (lResult != ERROR_NO_MORE_ITEMS)
		    {
		    // Usually, you only see NO_MORE_ITEMS
		    // when you're counting up from 0 and
		    // gone past the end.
		    // Some other tragedy has befallen us.
		    //
		    EercErrorHandler(GRC_API_FAILED, 1, "DeleteAllSubKeys:enum", NULL, NULL);
#ifdef DEBUG
		    StfApiErr(saeFail, "DeleteAllSubKeys:Enum", (LPSTR)szKey);
#endif // DEBUG
		    (*lpfnRegCloseKey)(hKey2);
		    free(szSubKey);
		    free(szFullSubKey);
		    return(FALSE);
		    }
		}
	    }
	//
	// Now that the count of subkeys is down to 0, we
	// can delete ourselves.
	//
	(*lpfnRegCloseKey)(hKey2);
	lResult = (*lpfnRegDeleteKey)(hKey, szKey);
        free(szSubKey);
        free(szFullSubKey);
	}
    return(TRUE);
}

// **************************************************************************
void CreateRegKeyEx(HKEY hKey, LPCSTR szKey)
{
	HKEY hKey2;
	INT i;

	if ((*lpfnRegCreateKey)(hKey, szKey, &hKey2)
			> ERROR_SUCCESS)
		{
		i = EercErrorHandler(GRC_API_FAILED, 1, "CreateRegKeyEx", NULL, NULL);
#ifdef DEBUG
		StfApiErr(saeFail, "CreateRegKeyEx", (LPSTR)szKey);
#endif // DEBUG
		SetupError(STFERR);
		}

	if ((*lpfnRegCloseKey)(hKey2) > ERROR_SUCCESS)
		{
		i = EercErrorHandler(GRC_API_FAILED, 1, "CreateRegKeyEx", NULL, NULL);
#ifdef DEBUG
		StfApiErr(saeFail, "CreateRegKeyEx", (LPSTR)szKey);
#endif // DEBUG
		SetupError(STFERR);
		}
}


// **************************************************************************
void CreateRegKeyValueEx(HKEY hKey, LPCTSTR szKey,
			 LPTSTR szValueName, DWORD dwType,
			 LPBYTE lpValueData, DWORD dwDataLen)
{
	HKEY hKey2;

        if ((*lpfnRegOpenKeyEx)(hKey, szKey, 0, KEY_WRITE, &hKey2) > ERROR_SUCCESS)
		{
		EercErrorHandler(GRC_API_FAILED, 1, "CreateRegKeyValueEx:open", NULL, NULL);
#ifdef DEBUG
		StfApiErr(saeFail, "CreateRegKeyValueEx:open", SzCat2Str((LPSTR)szKey, ", ", (LPSTR)szValueName));
#endif // DEBUG
		SetupError(STFERR);
		}

	if ((*lpfnRegSetValueEx)(hKey2, szValueName, 0, dwType, lpValueData,
			dwDataLen) > ERROR_SUCCESS)
		{
		EercErrorHandler(GRC_API_FAILED, 1, "CreateRegKeyValueEx", NULL, NULL);
#ifdef DEBUG
		StfApiErr(saeFail, "CreateRegKeyValueEx", SzCat2Str((LPSTR)szKey, ", ", (LPSTR)szValueName));
#endif // DEBUG
		SetupError(STFERR);
		}
}


// **************************************************************************
INT DoesRegKeyExistEx(HKEY hKey, LPCSTR szKey)
{
	HKEY hKey2;

	if ((*lpfnRegOpenKey)(hKey, szKey, &hKey2)
			!= ERROR_SUCCESS)
		return(0);

	(*lpfnRegCloseKey)(hKey2);

	return(1);
}


// **************************************************************************
void SetRegKeyValueEx(HKEY hKey, LPCTSTR szKey,
			LPTSTR szValueName, DWORD dwType,
			LPBYTE lpValueData, DWORD dwDataLen)
{
	HKEY hKey2;

        if ((*lpfnRegOpenKeyEx)(hKey, szKey, 0, KEY_WRITE, &hKey2) > ERROR_SUCCESS)
		{
		EercErrorHandler(GRC_API_FAILED, 1, "SetRegKeyValueEx:open", NULL, NULL);
#ifdef DEBUG
		StfApiErr(saeFail, "SetRegKeyValueEx:open", SzCat2Str((LPSTR)szKey, ", ", (LPSTR)szValueName));
#endif // DEBUG
		SetupError(STFERR);
		}

	if ((*lpfnRegSetValueEx)(hKey2, szValueName, 0, dwType, lpValueData,
			dwDataLen) > ERROR_SUCCESS)
		{
		EercErrorHandler(GRC_API_FAILED, 1, "SetRegKeyValueEx", NULL, NULL);
#ifdef DEBUG
		StfApiErr(saeFail, "SetRegKeyValueEx", SzCat2Str((LPSTR)szKey, ", ", (LPSTR)szValueName));
#endif // DEBUG
		SetupError(STFERR);
		}
}


// **************************************************************************
LONG GetRegKeyValueEx(HKEY hKey, LPCTSTR szKey, LPTSTR szValueName, LPDWORD lpdwType, LPBYTE lpBuf, DWORD cbBuf)
{
	HKEY	hKey2;
	DWORD	cb = cbBuf;
        LONG	lRetVal;

	if (!DoesRegKeyExistEx(hKey, szKey))
		return(ERROR_BADKEY);

        if ((*lpfnRegOpenKeyEx)(hKey, szKey, 0, KEY_READ, &hKey2) > ERROR_SUCCESS)
		{
		EercErrorHandler(GRC_API_FAILED, 1, "GetRegKeyValueEx", NULL, NULL);
#ifdef DEBUG
		StfApiErr(saeFail, "GetRegKeyValueEx", SzCat2Str((LPSTR)szKey, ", ", (LPSTR)szValueName));
#endif // DEBUG
		SetupError(STFERR);
		}

	if ((*lpfnRegQueryValueEx)(hKey2, (LPTSTR)szValueName, NULL,
			 lpdwType, lpBuf, &cb)
			!= ERROR_SUCCESS)
		{
		EercErrorHandler(GRC_API_FAILED, 1, "SetRegKeyValueEx", NULL, NULL);
#ifdef DEBUG
		StfApiErr(saeFail, "GetRegKeyValueEx", (LPSTR)szKey);
#endif // DEBUG
		SetupError(STFERR);
		}

	if (cb > cbBuf)
		{
		DoMsgBox("Buffer Overflow", "MS-Setup Error", MB_ICONHAND+MB_OK);
		SetupError(STFERR);
		lRetVal = ERROR_MORE_DATA;
		}
	else
		lRetVal = ERROR_SUCCESS;

	return(lRetVal);
}


// **************************************************************************
void DeleteRegKeyEx(HKEY hKey, LPCTSTR szKey)
{
    if (DoesRegKeyExistEx(hKey, szKey))
	{
	//
	// DeleteAllSubkeys is a recursive function that removes a
	// key tree from the bottom up.  The Win32 "RegDeleteKey()"
	// function will not delete a key that has existing subkeys.
	//
	if (!DeleteAllSubKeys(hKey, szKey))
	    {
	    EercErrorHandler(GRC_API_FAILED, 1, "DeleteAllSubKeys", NULL, NULL);
#ifdef DEBUG
	    StfApiErr(saeFail, "DeleteAllSubKeys", (LPSTR)szKey);
#endif // DEBUG
	    SetupError(STFERR);
	    }
	}
}

#endif

// **************************************************************************
BOOL FInitRegDb(void)
{
#ifdef WIN16
	if ((hinstDLL = LoadLibrary("SHELL.DLL")) == NULL)
		return(FALSE);

	if (hinstDLL < 32)
		{
		hinstDLL = NULL;
		return(FALSE);
		}
#elif defined(WIN32)
	if ((hinstDLL = LoadLibrary("ADVAPI32.DLL")) == NULL)
		return(FALSE);
#endif

	lpfnRegCloseKey =   (LPFNRCLK)GetProcAddress(hinstDLL, "RegCloseKey");
#ifdef UNICODE
	lpfnRegCreateKey =  (LPFNRCRK)GetProcAddress(hinstDLL, "RegCreateKeyW");
	lpfnRegDeleteKey =  (LPFNRDLK)GetProcAddress(hinstDLL, "RegDeleteKeyW");
	lpfnRegOpenKey =    (LPFNROPK)GetProcAddress(hinstDLL, "RegOpenKeyW");
	lpfnRegQueryValue = (LPFNRQRV)GetProcAddress(hinstDLL, "RegQueryValueW");
	lpfnRegSetValue =   (LPFNRSTV)GetProcAddress(hinstDLL, "RegSetValueW");
#else
	lpfnRegCreateKey =  (LPFNRCRK)GetProcAddress(hinstDLL, "RegCreateKeyA");
	lpfnRegDeleteKey =  (LPFNRDLK)GetProcAddress(hinstDLL, "RegDeleteKeyA");
	lpfnRegOpenKey =    (LPFNROPK)GetProcAddress(hinstDLL, "RegOpenKeyA");
	lpfnRegQueryValue = (LPFNRQRV)GetProcAddress(hinstDLL, "RegQueryValueA");
	lpfnRegSetValue =   (LPFNRSTV)GetProcAddress(hinstDLL, "RegSetValueA");
#endif

	if (lpfnRegCloseKey == NULL
			|| lpfnRegCreateKey == NULL
			|| lpfnRegDeleteKey == NULL
			|| lpfnRegOpenKey == NULL
			|| lpfnRegQueryValue == NULL
			|| lpfnRegSetValue == NULL)
		{
		FreeLibrary(hinstDLL);

		hinstDLL = NULL;
		return(FALSE);
		}

#ifdef WIN32
#ifdef UNICODE
	lpfnRegCreateKeyEx = (LPFNRCRKe)GetProcAddress(hinstDLL, "RegCreateKeyExW");
	lpfnRegOpenKeyEx = (LPFNROPKe)GetProcAddress(hinstDLL, "RegOpenKeyExW");
	lpfnRegQueryValueEx = (LPFNRQRVe)GetProcAddress(hinstDLL, "RegQueryValueExW");
	lpfnRegSetValueEx = (LPFNRSTVe)GetProcAddress(hinstDLL, "RegSetValueExW");
	lpfnRegEnumKey = (LPFNREK)GetProcAddress(hinstDLL, "RegEnumKeyW");
	lpfnRegQueryInfoKey = (LPFNRQIK)GetProcAddress(hinstDLL, "RegQueryInfoKeyW");
#else
	lpfnRegCreateKeyEx = (LPFNRCRKe)GetProcAddress(hinstDLL, "RegCreateKeyExA");
	lpfnRegOpenKeyEx = (LPFNROPKe)GetProcAddress(hinstDLL, "RegOpenKeyExA");
	lpfnRegQueryValueEx = (LPFNRQRVe)GetProcAddress(hinstDLL, "RegQueryValueExA");
	lpfnRegSetValueEx = (LPFNRSTVe)GetProcAddress(hinstDLL, "RegSetValueExA");
	lpfnRegEnumKey = (LPFNREK)GetProcAddress(hinstDLL, "RegEnumKeyA");
	lpfnRegQueryInfoKey = (LPFNRQIK)GetProcAddress(hinstDLL, "RegQueryInfoKeyA");
#endif // UNICODE
#endif // WIN32

	return(TRUE);
}


// **************************************************************************
VOID TerminateRegDb(void)
{
	if (hinstDLL != NULL)
		{
		FreeLibrary(hinstDLL);

		hinstDLL = NULL;
		}
}


#endif  /* !STF_LITE */
