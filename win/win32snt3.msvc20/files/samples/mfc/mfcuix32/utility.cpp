/*
 * UTILITY.C
 *
 * Utility routines for functions inside OLE2UI.DLL
 *
 *  General:
 *  ----------------------
 *  HourGlassOn             Displays the hourglass
 *  HourGlassOff            Hides the hourglass
 *
 *  Misc Tools:
 *  ----------------------
 *  Browse                  Displays the "File..." or "Browse..." dialog.
 *  ReplaceCharWithNull     Used to form filter strings for Browse.
 *  ErrorWithFile           Creates an error message with embedded filename
 *  OpenFileError           Give error message for OpenFile error return
 *  ChopText                Chop a file path to fit within a specified width
 *  DoesFileExist           Checks if file is valid
 *
 *  Registration Database:
 *  ----------------------
 *  HIconFromClass          Extracts the first icon in a class's server path
 *  FServerFromClass        Retrieves the server path for a class name (fast)
 *  UClassFromDescription   Finds the classname given a description (slow)
 *  UDescriptionFromClass   Retrieves the description for a class name (fast)
 *  FGetVerb                Retrieves a specific verb for a class (fast)
 *
 *
 * Copyright (c)1992 Microsoft Corporation, All Right Reserved
 */

#include "precomp.h"
#include <stdlib.h>
#include <commdlg.h>
#include <memory.h>
#include <cderr.h>
#include "common.h"
#include "utility.h"

OLEDBGDATA

/*
 * HourGlassOn
 *
 * Purpose:
 *  Shows the hourglass cursor returning the last cursor in use.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  HCURSOR         Cursor in use prior to showing the hourglass.
 */

HCURSOR WINAPI HourGlassOn(void)
{
	HCURSOR     hCur;

	hCur=SetCursor(LoadCursor(NULL, IDC_WAIT));
	ShowCursor(TRUE);

	return hCur;
}


/*
 * HourGlassOff
 *
 * Purpose:
 *  Turns off the hourglass restoring it to a previous cursor.
 *
 * Parameters:
 *  hCur            HCURSOR as returned from HourGlassOn
 *
 * Return Value:
 *  None
 */

void WINAPI HourGlassOff(HCURSOR hCur)
{
	ShowCursor(FALSE);
	SetCursor(hCur);
	return;
}


/*
 * Browse
 *
 * Purpose:
 *  Displays the standard GetOpenFileName dialog with the title of
 *  "Browse."  The types listed in this dialog are controlled through
 *  iFilterString.  If it's zero, then the types are filled with "*.*"
 *  Otherwise that string is loaded from resources and used.
 *
 * Parameters:
 *  hWndOwner       HWND owning the dialog
 *  lpszFile        LPSTR specifying the initial file and the buffer in
 *                  which to return the selected file.  If there is no
 *                  initial file the first character of this string should
 *                  be NULL.
 *  lpszInitialDir  LPSTR specifying the initial directory.  If none is to
 *                  set (ie, the cwd should be used), then this parameter
 *                  should be NULL.
 *  cchFile         UINT length of pszFile
 *  iFilterString   UINT index into the stringtable for the filter string.
 *  dwOfnFlags      DWORD flags to OR with OFN_HIDEREADONLY
 *
 * Return Value:
 *  BOOL            TRUE if the user selected a file and pressed OK.
 *                  FALSE otherwise, such as on pressing Cancel.
 */

BOOL WINAPI Browse(HWND hWndOwner, LPTSTR lpszFile, LPTSTR lpszInitialDir, UINT cchFile, UINT iFilterString, DWORD dwOfnFlags)
{
	UINT    cch;
	TCHAR   szFilters[256];
	TCHAR   szDlgTitle[128];  // that should be big enough

	if (NULL==lpszFile || 0==cchFile)
		return FALSE;

	/*
	 * Exact contents of the filter combobox is TBD.  One idea
	 * is to take all the extensions in the RegDB and place them in here
	 * with the descriptive class name associate with them.  This has the
	 * extra step of finding all extensions of the same class handler and
	 * building one extension string for all of them.  Can get messy quick.
	 * UI demo has only *.* which we do for now.
	 */

	if (0 != iFilterString)
	{
		cch = LoadString(_g_hOleStdResInst, iFilterString, (LPTSTR)szFilters,
			sizeof(szFilters)/sizeof(TCHAR));
	}
	else
	{
		szFilters[0] = 0;
		cch = 1;
	}

	if (0==cch)
		return FALSE;

	ReplaceCharWithNull(szFilters, szFilters[cch-1]);

	//Prior string must also be initialized, if there is one.
	OPENFILENAME ofn;
	memset((LPOPENFILENAME)&ofn, 0, sizeof(ofn));
	ofn.lStructSize =sizeof(ofn);
	ofn.hwndOwner   =hWndOwner;
	ofn.lpstrFile   =lpszFile;
	ofn.nMaxFile    =cchFile;
	ofn.lpstrFilter =(LPTSTR)szFilters;
	ofn.nFilterIndex=1;
	if (LoadString(_g_hOleStdResInst, IDS_BROWSE, (LPTSTR)szDlgTitle, sizeof(szDlgTitle)/sizeof(TCHAR)))
		ofn.lpstrTitle  =(LPTSTR)szDlgTitle;
	ofn.hInstance = _g_hOleStdInst;
	ofn.lpTemplateName = MAKEINTRESOURCE(IDD_FILEOPEN);
	if (NULL != lpszInitialDir)
		ofn.lpstrInitialDir = lpszInitialDir;

	ofn.Flags= OFN_HIDEREADONLY | OFN_ENABLETEMPLATE | (dwOfnFlags) ;

	//On success, copy the chosen filename to the static display
	BOOL bResult = GetOpenFileName((LPOPENFILENAME)&ofn);
	return bResult;
}

/*
 * ReplaceCharWithNull
 *
 * Purpose:
 *  Walks a null-terminated string and replaces a given character
 *  with a zero.  Used to turn a single string for file open/save
 *  filters into the appropriate filter string as required by the
 *  common dialog API.
 *
 * Parameters:
 *  psz             LPTSTR to the string to process.
 *  ch              int character to replace.
 *
 * Return Value:
 *  int             Number of characters replaced.  -1 if psz is NULL.
 */

int WINAPI ReplaceCharWithNull(LPTSTR psz, int ch)
{
	int cChanged = 0;

	if (psz == NULL)
		return -1;

	while ('\0' != *psz)
	{
		if (ch == *psz)
		{
			*psz++ = '\0';
			cChanged++;
			continue;
		}
		psz = CharNext(psz);
	}
	return cChanged;
}

/*
 * ErrorWithFile
 *
 * Purpose:
 *  Displays a message box built from a stringtable string containing
 *  one %s as a placeholder for a filename and from a string of the
 *  filename to place there.
 *
 * Parameters:
 *  hWnd            HWND owning the message box.  The caption of this
 *                  window is the caption of the message box.
 *  hInst           HINSTANCE from which to draw the idsErr string.
 *  idsErr          UINT identifier of a stringtable string containing
 *                  the error message with a %s.
 *  lpszFile        LPSTR to the filename to include in the message.
 *  uFlags          UINT flags to pass to MessageBox, like MB_OK.
 *
 * Return Value:
 *  int             Return value from MessageBox.
 */

int WINAPI ErrorWithFile(HWND hWnd, HINSTANCE hInst, UINT idsErr,
	LPTSTR pszFile, UINT uFlags)
{
	int             iRet=0;
	HANDLE          hMem;
	const UINT      cb = (2*MAX_PATH);
	LPTSTR          psz1, psz2, psz3;

	if (NULL==hInst || NULL==pszFile)
		return iRet;

	//Allocate three 2*MAX_PATH byte work buffers
	hMem=GlobalAlloc(GHND, (DWORD)(3*cb)*sizeof(TCHAR));

	if (NULL==hMem)
		return iRet;

	psz1 = (LPTSTR)GlobalLock(hMem);
	psz2 = psz1+cb;
	psz3 = psz2+cb;

	if (0!=LoadString(hInst, idsErr, psz1, cb))
	{
		wsprintf(psz2, psz1, pszFile);

		//Steal the caption of the dialog
		GetWindowText(hWnd, psz3, cb);
		iRet=MessageBox(hWnd, psz2, psz3, uFlags);
	}

	GlobalUnlock(hMem);
	GlobalFree(hMem);
	return iRet;
}


/*
 * HIconFromClass
 *
 * Purpose:
 *  Given an object class name, finds an associated executable in the
 *  registration database and extracts the first icon from that
 *  executable.  If none is available or the class has no associated
 *  executable, this function returns NULL.
 *
 * Parameters:
 *  pszClass        LPSTR giving the object class to look up.
 *
 * Return Value:
 *  HICON           Handle to the extracted icon if there is a module
 *                  associated to pszClass.  NULL on failure to either
 *                  find the executable or extract and icon.
 */

HICON WINAPI HIconFromClass(LPTSTR pszClass)
{
	HICON   hIcon;
	TCHAR   szEXE[MAX_PATH];
	UINT    Index;
	CLSID   clsid;

	if (NULL==pszClass)
		return NULL;

	CLSIDFromString(pszClass, &clsid);

	if (!FIconFileFromClass(clsid, szEXE, MAX_PATH_SIZE, &Index))
		return NULL;

	hIcon = ExtractIcon(_g_hOleStdInst, szEXE, Index);
	return hIcon;
}

/*
 * FServerFromClass
 *
 * Purpose:
 *  Looks up the classname in the registration database and retrieves
 *  the name undet protocol\StdFileEditing\server.
 *
 * Parameters:
 *  pszClass        LPSTR to the classname to look up.
 *  pszEXE          LPSTR at which to store the server name
 *  cch             UINT size of pszEXE
 *
 * Return Value:
 *  BOOL            TRUE if one or more characters were loaded into pszEXE.
 *                  FALSE otherwise.
 */

BOOL WINAPI FServerFromClass(LPTSTR pszClass, LPTSTR pszEXE, UINT cch)
{

	DWORD       dw;
	LONG        lRet;
	HKEY        hKey;

	if (NULL==pszClass || NULL==pszEXE || 0==cch)
		return FALSE;

	/*
	 * We have to go walking in the registration database under the
	 * classname, so we first open the classname key and then check
	 * under "\\LocalServer" to get the .EXE.
	 */

	//Open up the class key
	lRet=RegOpenKey(HKEY_CLASSES_ROOT, pszClass, &hKey);

	if ((LONG)ERROR_SUCCESS!=lRet)
		return FALSE;

	//Get the executable path.
	dw=(DWORD)cch;
	lRet = RegQueryValue(hKey, TEXT("LocalServer"), pszEXE, (LONG*)&dw);

	RegCloseKey(hKey);

	return ((ERROR_SUCCESS == lRet) && (dw > 0));
}


/*
 * UClassFromDescription
 *
 * Purpose:
 *  Looks up the actual OLE class name in the registration database
 *  for the given descriptive name chosen from a listbox.
 *
 * Parameters:
 *  psz             LPSTR to the descriptive name.
 *  pszClass        LPSTR in which to store the class name.
 *  cb              UINT maximum length of pszClass.
 *
 * Return Value:
 *  UINT            Number of characters copied to pszClass.  0 on failure.
 */

UINT WINAPI UClassFromDescription(LPTSTR psz, LPTSTR pszClass, UINT cb)
	{
	DWORD           dw;
	HKEY            hKey;
	TCHAR           szClass[OLEUI_CCHKEYMAX];
	LONG            lRet;
	UINT            i;

	//Open up the root key.
	lRet=RegOpenKey(HKEY_CLASSES_ROOT, NULL, &hKey);

	if ((LONG)ERROR_SUCCESS!=lRet)
		return 0;

	i=0;
	lRet=RegEnumKey(hKey, i++, szClass, OLEUI_CCHKEYMAX_SIZE);

	//Walk the available keys
	while ((LONG)ERROR_SUCCESS==lRet)
	{
		dw=(DWORD)cb;
		lRet = RegQueryValue(hKey, szClass, pszClass, (LONG*)&dw);

		//Check if the description matches the one just enumerated
		if ((LONG)ERROR_SUCCESS==lRet)
		{
			if (!lstrcmp(pszClass, psz))
				break;
		}

		//Continue with the next key.
		lRet=RegEnumKey(hKey, i++, szClass, OLEUI_CCHKEYMAX_SIZE);
	}

	//If we found it, copy to the return buffer
	if ((LONG)ERROR_SUCCESS==lRet)
		lstrcpy(pszClass, szClass);
	else
		dw=0L;

	RegCloseKey(hKey);
	return (UINT)dw;
}


/*
 * UDescriptionFromClass
 *
 * Purpose:
 *  Looks up the actual OLE descriptive name name in the registration
 *  database for the given class name.
 *
 * Parameters:
 *  pszClass        LPSTR to the class name.
 *  psz             LPSTR in which to store the descriptive name.
 *  cb              UINT maximum length of psz.
 *
 * Return Value:
 *  UINT            Number of characters copied to pszClass.  0 on failure.
 */

UINT WINAPI UDescriptionFromClass(LPTSTR pszClass, LPTSTR psz, UINT cb)
{
	DWORD           dw;
	HKEY            hKey;
	LONG            lRet;

	if (NULL==pszClass || NULL==psz)
		return 0;

	//Open up the root key.
	lRet=RegOpenKey(HKEY_CLASSES_ROOT, NULL, &hKey);

	if ((LONG)ERROR_SUCCESS!=lRet)
		return 0;

	//Get the descriptive name using the class name.
	dw = (DWORD)cb;
	lRet = RegQueryValue(hKey, pszClass, psz, (LONG*)&dw);

	RegCloseKey(hKey);

	psz+=lstrlen(psz)+1;
	*psz=0;

	if ((LONG)ERROR_SUCCESS!=lRet)
		return 0;

	return (UINT)dw;
}


// returns width of line of text. this is a support routine for ChopText
static LONG GetTextWSize(HDC hDC, LPTSTR lpsz)
{
	SIZE size;

	if (GetTextExtentPoint(hDC, lpsz, lstrlen(lpsz), (LPSIZE)&size))
		return size.cx;
	else
		return 0;
}

static void WINAPI Abbreviate(HDC hdc, int nWidth, LPTSTR lpch, int nMaxChars)
{
	/* string is too long to fit; chop it */
	/* set up new prefix & determine remaining space in control */
	LPTSTR lpszFileName = NULL;
	LPTSTR lpszCur = _tcsinc(_tcsinc(lpch));
	lpszCur = _tcschr(lpszCur, TEXT('\\'));
		// algorithm will insert \... so allocate extra 4
	LPTSTR lpszNew = new TCHAR[lstrlen(lpch)+5];
	if (lpszNew == NULL)
		return;

	if (lpszCur != NULL)  // at least one backslash
	{
		*lpszNew = (TCHAR)0;
		*lpszCur = (TCHAR)0;
		lstrcpy(lpszNew, lpch);
		*lpszCur = TEXT('\\');
		// lpszNew now contains c: or \\servername
		lstrcat(lpszNew, TEXT("\\..."));
		// lpszNew now contains c:\... or \\servername\...
		LPTSTR lpszEnd = lpszNew;
		while (*lpszEnd != (TCHAR)0)
			lpszEnd = _tcsinc(lpszEnd);
		// lpszEnd is now at the end of c:\... or \\servername\...

		// move down directories until it fits or no more directories
		while (lpszCur != NULL)
		{
			*lpszEnd = (TCHAR)0;
			lstrcat(lpszEnd, lpszCur);
			if (GetTextWSize(hdc, lpszNew) <= nWidth &&
				lstrlen(lpszNew) < nMaxChars)
			{
				lstrcpy(lpch, lpszNew);
				delete [] lpszNew;
				return;
			}
			lpszCur = _tcsinc(lpszCur);  // advance past backslash
			lpszCur = _tcschr(lpszCur, TEXT('\\'));
		}

		// try just ...filename and then shortening filename
		lpszFileName = _tcsrchr(lpch, TEXT('\\'));
	}
	else
		lpszFileName = lpch;
	while (*lpszFileName != (TCHAR)0)
	{
		lstrcpy(lpszNew, TEXT("..."));
		lstrcat(lpszNew, lpszFileName);
		if (GetTextWSize(hdc, lpszNew) <= nWidth &&
			lstrlen(lpszNew) < nMaxChars)
		{
			lstrcpy(lpch, lpszNew);
			delete [] lpszNew;
			return;
		}
		lpszFileName = _tcsinc(lpszFileName);
	}
	// not even a single character fit
	*lpch = (TCHAR)0;
	return;
}

/*
 * ChopText
 *
 * Purpose:
 *  Parse a string (pathname) and convert it to be within a specified
 *  length by chopping the least significant part
 *
 * Parameters:
 *  hWnd            window handle in which the string resides
 *  nWidth          max width of string in pixels
 *                  use width of hWnd if zero
 *  lpch            pointer to beginning of the string
 *  nMaxChars       maximum allowable number of characters (0 ignore)
 *
 * Return Value:
 *  pointer to the modified string
 */
LPTSTR WINAPI ChopText(HWND hWnd, int nWidth, LPTSTR lpch, int nMaxChars)
{
	HDC     hdc;
	HFONT   hfont;
	HFONT   hfontOld = NULL;
	RECT    rc;

	if (!hWnd || !lpch)
		return NULL;

	if (nMaxChars == 0)
		nMaxChars = 32768; // big number

	/* Get length of static field. */
	if (!nWidth)
	{
		GetClientRect(hWnd, (LPRECT)&rc);
		nWidth = rc.right - rc.left;
	}

	/* Set up DC appropriately for the static control */
	hdc = CreateIC(TEXT("DISPLAY"), NULL, NULL, NULL);
	hfont = (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0L);

	if (NULL != hfont)   // WM_GETFONT returns NULL if window uses system font
		hfontOld = (HFONT)SelectObject(hdc, hfont);

	/* check horizontal extent of string */
	if (GetTextWSize(hdc, lpch) > nWidth || lstrlen(lpch) >= nMaxChars)
		Abbreviate(hdc, nWidth, lpch, nMaxChars);

	if (NULL != hfont)
		SelectObject(hdc, hfontOld);
	DeleteDC(hdc);

	return lpch;
}

/*
 * OpenFileError
 *
 * Purpose:
 *  display message for error returned from OpenFile
 *
 * Parameters:
 *  hDlg            HWND of the dialog.
 *  nErrCode        UINT error code returned in OFSTRUCT passed to OpenFile
 *  lpszFile        LPSTR file name passed to OpenFile
 *
 * Return Value:
 *  None
 */
void WINAPI OpenFileError(HWND hDlg, UINT nErrCode, LPTSTR lpszFile)
{
	switch (nErrCode)
	{
		case 0x0005:    // Access denied
			ErrorWithFile(hDlg, _g_hOleStdResInst, IDS_CIFILEACCESS, lpszFile, MB_OK);
			break;

		case 0x0020:    // Sharing violation
			ErrorWithFile(hDlg, _g_hOleStdResInst, IDS_CIFILESHARE, lpszFile, MB_OK);
			break;

		case 0x0002:    // File not found
		case 0x0003:    // Path not found
			ErrorWithFile(hDlg, _g_hOleStdResInst, IDS_CIINVALIDFILE, lpszFile, MB_OK);
			break;

		default:
			ErrorWithFile(hDlg, _g_hOleStdResInst, IDS_CIFILEOPENFAIL, lpszFile, MB_OK);
			break;
	}
}

/*
 * DoesFileExist
 *
 * Purpose:
 *  Determines if a file path exists
 *
 * Parameters:
 *  lpszFile        LPTSTR - file name
 *
 * Return Value:
 *  BOOL            TRUE if file exists, else FALSE.
 *
 */
BOOL WINAPI DoesFileExist(LPTSTR lpszFile)
{
	int         i;
	static const TCHAR *arrIllegalNames[] = {
		TEXT("LPT1"),
		TEXT("LPT2"),
		TEXT("LPT3"),
		TEXT("COM1"),
		TEXT("COM2"),
		TEXT("COM3"),
		TEXT("COM4"),
		TEXT("CON"),
		TEXT("AUX"),
		TEXT("PRN")
	};
	DWORD dwAttrs;

	// Check is the name is an illegal name (eg. the name of a device)
	for (i=0; i < (sizeof(arrIllegalNames)/sizeof(arrIllegalNames[0])); i++)
	{
		if (lstrcmpi(lpszFile, arrIllegalNames[i])==0)
			return FALSE;
	}

	dwAttrs = GetFileAttributes(lpszFile);
	if ((dwAttrs & ~(FILE_ATTRIBUTE_ARCHIVE|FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_READONLY)) == 0)
		return TRUE;
	// look in path for file
	TCHAR ch;
	LPTSTR lpszFilePart;
	return SearchPath(NULL, lpszFile, NULL, 1, &ch, &lpszFilePart) != 0;
}

/*
 * FormatStrings
 *
 * Purpose:
 *  Simple message formatting API compatible w/ different languages
 *
 * Note:
 *  Shamelessly stolen/modified from MFC source code
 *
 */

void WINAPI FormatStrings(LPTSTR lpszDest, LPCTSTR lpszFormat,
	LPCTSTR* rglpsz, int nString)
{
	LPCTSTR pchSrc = lpszFormat;
	while (*pchSrc != '\0')
	{
		if (pchSrc[0] == '%' && (pchSrc[1] >= '1' && pchSrc[1] <= '9'))
		{
			int i = pchSrc[1] - '1';
			pchSrc += 2;
			if (i >= nString)
				*lpszDest++ = '?';
			else if (rglpsz[i] != NULL)
			{
				lstrcpy(lpszDest, rglpsz[i]);
				lpszDest += lstrlen(lpszDest);
			}
		}
		else
		{
#ifndef _UNICODE
			if (IsDBCSLeadByte(*pchSrc))
				*lpszDest++ = *pchSrc++; // copy first of 2 bytes
#endif
			*lpszDest++ = *pchSrc++;
		}
	}
	*lpszDest = '\0';
}

void WINAPI FormatString1(LPTSTR lpszDest, LPCTSTR lpszFormat, LPCTSTR lpsz1)
{
	FormatStrings(lpszDest, lpszFormat, &lpsz1, 1);
}

void WINAPI FormatString2(LPTSTR lpszDest, LPCTSTR lpszFormat, LPCTSTR lpsz1,
	LPCTSTR lpsz2)
{
	LPCTSTR rglpsz[2];
	rglpsz[0] = lpsz1;
	rglpsz[1] = lpsz2;
	FormatStrings(lpszDest, lpszFormat, rglpsz, 2);
}

// Replacement for stdlib atol,
// which didn't work and doesn't take far pointers.
// Must be tolerant of leading spaces.
//
//
LONG WINAPI Atol(LPTSTR lpsz)
{
	signed int sign = +1;
	UINT base = 10;
	LONG l = 0;

	if (NULL==lpsz)
	{
		OleDbgAssert (0);
		return 0;
	}
	while (isspace(*lpsz))
		lpsz++;

	if (*lpsz=='-')
	{
		lpsz++;
		sign = -1;
	}
	if (lpsz[0]==TEXT('0') && lpsz[1]==TEXT('x'))
	{
		base = 16;
		lpsz+=2;
	}

	if (base==10)
	{
		while (isdigit(*lpsz))
		{
			l = l * base + *lpsz - '0';
			lpsz++;
		}
	}
	else
	{
		OleDbgAssert (base==16);
		while (isxdigit(*lpsz))
		{
			l = l * base + isdigit(*lpsz) ? *lpsz - '0' : toupper(*lpsz) - 'A' + 10;
			lpsz++;
		}
	}
	return l * sign;
}

/////////////////////////////////////////////////////////////////////////////
