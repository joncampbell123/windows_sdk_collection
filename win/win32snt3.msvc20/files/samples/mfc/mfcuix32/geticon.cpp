/*************************************************************************
**
** The following API's are now OBSOLETE because equivalent API's have been
** added to the OLE2.DLL library
**      GetIconOfFile       superceeded by OleGetIconOfFile
**      GetIconOfClass      superceeded by OleGetIconOfClass
**      OleUIMetafilePictFromIconAndLabel
**                          superceeded by OleMetafilePictFromIconAndLabel
*************************************************************************/

/*
 *  GETICON.C
 *
 *  Functions to create DVASPECT_ICON metafile from filename or classname.
 *
 *  GetIconOfFile
 *  GetIconOfClass
 *  OleUIMetafilePictFromIconAndLabel
 *  HIconAndSourceFromClass Extracts the first icon in a class's server path
 *                          and returns the path and icon index to caller.
 *  FIconFileFromClass      Retrieves the path to the exe/dll containing the
 *                           default icon, and the index of the icon.
 *  OleStdIconLabelTextOut  Draw icon label text (line break if necessary)
 *
 *    (c) Copyright Microsoft Corp. 1992-1993 All Rights Reserved
 */


/*******
 *
 * ICON (DVASPECT_ICON) METAFILE FORMAT:
 *
 * The metafile generated with OleUIMetafilePictFromIconAndLabel contains
 * the following records which are used by the functions in DRAWICON.C
 * to draw the icon with and without the label and to extract the icon,
 * label, and icon source/index.
 *
 *  SetWindowOrg
 *  SetWindowExt
 *  DrawIcon:
 *      Inserts records of DIBBITBLT or DIBSTRETCHBLT, once for the
 *      AND mask, one for the image bits.
 *  Escape with the comment "IconOnly"
 *      This indicates where to stop record enumeration to draw only
 *      the icon.
 *  SetTextColor
 *  SetTextAlign
 *  SetBkColor
 *  CreateFont
 *  SelectObject on the font.
 *  ExtTextOut
 *      One or more ExtTextOuts occur if the label is wrapped.  The
 *      text in these records is used to extract the label.
 *  SelectObject on the old font.
 *  DeleteObject on the font.
 *  Escape with a comment that contains the path to the icon source.
 *  Escape with a comment that is the ASCII of the icon index.
 *
 *******/

#include "precomp.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <commdlg.h>
#include <memory.h>
#include <cderr.h>
#include "utility.h"

static const TCHAR szSeparators[] = TEXT(" \t\\/!:");

#define IS_SEPARATOR(c)         ( (c) == ' ' || (c) == '\\' \
								  || (c) == '/' || (c) == '\t' \
								  || (c) == '!' || (c) == ':')
#define IS_FILENAME_DELIM(c)    ( (c) == '\\' || (c) == '/' || (c) == ':' )

/*
 * GetAssociatedExecutable
 *
 * Purpose:  Finds the executable associated with the provided extension
 *
 * Parameters:
 *   lpszExtension   LPSTR points to the extension we're trying to find
 *                   an exe for. Does **NO** validation.
 *
 *   lpszExecutable  LPSTR points to where the exe name will be returned.
 *                   No validation here either - pass in 128 char buffer.
 *
 * Return:
 *   BOOL            TRUE if we found an exe, FALSE if we didn't.
 *
 */
BOOL FAR PASCAL GetAssociatedExecutable(LPTSTR lpszExtension, LPTSTR lpszExecutable)
{
	HKEY    hKey;
	LONG    dw;
	LRESULT lRet;
	TCHAR    szValue[OLEUI_CCHKEYMAX];
	TCHAR    szKey[OLEUI_CCHKEYMAX];
	LPTSTR   lpszTemp, lpszExe;


	lRet = RegOpenKey(HKEY_CLASSES_ROOT, NULL, &hKey);

	if (ERROR_SUCCESS != lRet)
		return FALSE;

	dw = MAX_PATH_SIZE;
	lRet = RegQueryValue(hKey, lpszExtension, (LPTSTR)szValue, &dw);  //ProgId

	if (ERROR_SUCCESS != lRet)
	{
		RegCloseKey(hKey);
		return FALSE;
	}

	// szValue now has ProgID
	lstrcpy(szKey, szValue);
	lstrcat(szKey, TEXT("\\Shell\\Open\\Command"));

	dw = MAX_PATH_SIZE;
	lRet = RegQueryValue(hKey, (LPTSTR)szKey, (LPTSTR)szValue, &dw);

	if (ERROR_SUCCESS != lRet)
	{
		RegCloseKey(hKey);
		return FALSE;
	}

	// szValue now has an executable name in it.  Let's null-terminate
	// at the first post-executable space (so we don't have cmd line
	// args.

	lpszTemp = (LPTSTR)szValue;

	while ('\0' != *lpszTemp && _istspace(*lpszTemp))
		lpszTemp = CharNext(lpszTemp);      // Strip off leading spaces
	lpszExe = lpszTemp;

	while ('\0' != *lpszTemp && !_istspace(*lpszTemp))
		lpszTemp = CharNext(lpszTemp);     // Step through exe name
	*lpszTemp = '\0';  // null terminate at first space (or at end).

	lstrcpy(lpszExecutable, lpszExe);
	return TRUE;
}


/*
 * HIconAndSourceFromClass
 *
 * Purpose:
 *  Given an object class name, finds an associated executable in the
 *  registration database and extracts the first icon from that
 *  executable.  If none is available or the class has no associated
 *  executable, this function returns NULL.
 *
 * Parameters:
 *  rclsid          pointer to clsid to look up.
 *  pszSource       LPSTR in which to place the source of the icon.
 *                  This is assumed to be MAX_PATH
 *  puIcon          UINT FAR * in which to store the index of the
 *                  icon in pszSource.
 *
 * Return Value:
 *  HICON           Handle to the extracted icon if there is a module
 *                  associated to pszClass.  NULL on failure to either
 *                  find the executable or extract and icon.
 */
HICON FAR PASCAL HIconAndSourceFromClass(REFCLSID rclsid, LPTSTR pszSource, UINT FAR *puIcon)
{
	HICON           hIcon;
	UINT            IconIndex;

	if (NULL == &rclsid || NULL == pszSource || IsEqualCLSID(rclsid, CLSID_NULL))
		return NULL;

	if (!FIconFileFromClass(rclsid, pszSource, MAX_PATH_SIZE, &IconIndex))
		return NULL;

	hIcon = ExtractIcon(_g_hOleStdInst, pszSource, IconIndex);
	*puIcon= IconIndex;

	return hIcon;
}


/*
 * PointerToNthField
 *
 * Purpose:
 *  Returns a pointer to the beginning of the nth field.
 *  Assumes null-terminated string.
 *
 * Parameters:
 *  lpszString        string to parse
 *  nField            field to return starting index of.
 *  chDelimiter       char that delimits fields
 *
 * Return Value:
 *  LPSTR             pointer to beginning of nField field.
 *                    NOTE: If the null terminator is found
 *                          Before we find the Nth field, then
 *                          we return a pointer to the null terminator -
 *                          calling app should be sure to check for
 *                          this case.
 *
 */
LPTSTR FAR PASCAL PointerToNthField(LPTSTR lpszString, int nField, TCHAR chDelimiter)
{
	LPTSTR lpField = lpszString;
	int   cFieldFound = 1;

	if (1 ==nField)
		return lpszString;

	while (*lpField != '\0')
	{
		if (*lpField++ == chDelimiter)
		{
			cFieldFound++;

			if (nField == cFieldFound)
				return lpField;
		}
	}
	return lpField;
}


/*
 * FIconFileFromClass
 *
 * Purpose:
 *  Looks up the path to executable that contains the class default icon.
 *
 * Parameters:
 *  rclsid          pointer to CLSID to look up.
 *  pszEXE          LPSTR at which to store the server name
 *  cch             UINT size of pszEXE
 *  lpIndex         LPUINT to index of icon within executable
 *
 * Return Value:
 *  BOOL            TRUE if one or more characters were loaded into pszEXE.
 *                  FALSE otherwise.
 */

BOOL FAR PASCAL FIconFileFromClass(REFCLSID rclsid, LPTSTR pszEXE, UINT cchBytes, UINT FAR *lpIndex)
{
	LONG          dw;
	LONG          lRet;
	HKEY          hKey;
	LPTSTR        lpBuffer;
	LPTSTR        lpIndexString;
	// room for 128 char path and icon's index
	UINT          cBufferSize = 136*sizeof(TCHAR);
	TCHAR         szKey[64];
	LPTSTR        pszClass;
	UINT          cch=cchBytes / sizeof(TCHAR);  // number of characters

	if (NULL == &rclsid || NULL == pszEXE ||
		0 == cch || IsEqualCLSID(rclsid, CLSID_NULL))
	{
		return FALSE;
	}

	lpBuffer = (LPTSTR)OleStdMalloc(cBufferSize);
	if (NULL == lpBuffer)
		return FALSE;

	if (CoIsOle1Class(rclsid))
	{
		LPTSTR lpszProgID;

		// we've got an ole 1.0 class on our hands, so we look at
		// progID\protocol\stdfileedting\server to get the
		// name of the executable.

		ProgIDFromCLSID(rclsid, &lpszProgID);

		//Open up the class key
		lRet=RegOpenKey(HKEY_CLASSES_ROOT, lpszProgID, &hKey);

		if (ERROR_SUCCESS != lRet)
		{
			OleStdFree(lpszProgID);
			OleStdFree(lpBuffer);
			return FALSE;
		}

		dw=(LONG)cBufferSize;
		lRet = RegQueryValue(hKey, TEXT("Protocol\\StdFileEditing\\Server"), lpBuffer, &dw);

		if (ERROR_SUCCESS != lRet)
		{
			RegCloseKey(hKey);
			OleStdFree(lpszProgID);
			OleStdFree(lpBuffer);
			return FALSE;
		}

		// Use server and 0 as the icon index
		lstrcpyn(pszEXE, lpBuffer, cch);
		*lpIndex = 0;

		RegCloseKey(hKey);
		OleStdFree(lpszProgID);
		OleStdFree(lpBuffer);
		return TRUE;
	}

	/*
	 * We have to go walking in the registration database under the
	 * classname, so we first open the classname key and then check
	 * under "\\DefaultIcon" to get the file that contains the icon.
	 */

	StringFromCLSID(rclsid, &pszClass);

	lstrcpy(szKey, TEXT("CLSID\\"));
	lstrcat(szKey, pszClass);

	//Open up the class key
	lRet=RegOpenKey(HKEY_CLASSES_ROOT, szKey, &hKey);

	if (ERROR_SUCCESS != lRet)
	{
		OleStdFree(lpBuffer);
		OleStdFree(pszClass);
		return FALSE;
	}

	//Get the executable path and icon index.

	dw=(LONG)cBufferSize;
	lRet=RegQueryValue(hKey, TEXT("DefaultIcon"), lpBuffer, &dw);

	if (ERROR_SUCCESS != lRet)
	{
		// no DefaultIcon  key...try LocalServer

		dw=(LONG)cBufferSize;
		lRet = RegQueryValue(hKey, TEXT("LocalServer"), lpBuffer, &dw);

		if (ERROR_SUCCESS != lRet)
		{
			// no LocalServer entry either...they're outta luck.
			RegCloseKey(hKey);
			OleStdFree(lpBuffer);
			OleStdFree(pszClass);
			return FALSE;
		}

		// Use server from LocalServer or Server and 0 as the icon index
		lstrcpyn(pszEXE, lpBuffer, cch);
		*lpIndex = 0;

		RegCloseKey(hKey);
		OleStdFree(lpBuffer);
		OleStdFree(pszClass);
		return TRUE;
	}

	RegCloseKey(hKey);

	// lpBuffer contains a string that looks like "<pathtoexe>,<iconindex>",
	// so we need to separate the path and the icon index.

	lpIndexString = PointerToNthField(lpBuffer, 2, ',');
	if ('\0' == *lpIndexString)  // no icon index specified - use 0 as default.
	{
		*lpIndex = 0;
	}
	else
	{
		LPTSTR lpTemp;
		TCHAR szTemp[16];

		lstrcpy(szTemp, lpIndexString);

		// Put the icon index part into *pIconIndex
		*lpIndex = (UINT)Atol(szTemp);

		// Null-terminate the exe part.
		lpTemp = CharPrev(lpBuffer, lpIndexString);
		*lpTemp = '\0';
	}

	if (!lstrcpyn(pszEXE, lpBuffer, cch))
	{
		OleStdFree(lpBuffer);
		OleStdFree(pszClass);
		return FALSE;
	}

	// Free the memory we alloc'd and leave.
	OleStdFree(lpBuffer);
	OleStdFree(pszClass);
	return TRUE;
}

/*
 * OleStdIconLabelTextOut
 *
 * Purpose:
 *  Replacement for DrawText to be used in the "Display as Icon" metafile.
 *  Uses ExtTextOut to output a string center on (at most) two lines.
 *  Uses a very simple word wrap algorithm to split the lines.
 *
 * Parameters:  (same as for ExtTextOut, except for hFont)
 *  hDC           device context to draw into; if this is NULL, then we don't
 *                ETO the text, we just return the index of the beginning
 *                of the second line
 *  hFont         font to use
 *  nXStart       x-coordinate of starting position
 *  nYStart       y-coordinate of starting position
 *  fuOptions     rectangle type
 *  lpRect        rect far * containing rectangle to draw text in.
 *  lpszString    string to draw
 *  cchString     length of string (truncated if over OLEUI_CCHLABELMAX)
 *  lpDX          spacing between character cells
 *
 * Return Value:
 *  UINT          Index of beginning of last line (0 if there's only one
 *                line of text).
 *
 */
STDAPI_(UINT) OleStdIconLabelTextOut(
	HDC         hDC,
	HFONT       hFont,
	int         nXStart,
	int         nYStart,
	UINT        fuOptions,
	RECT FAR *  lpRect,
	LPTSTR      lpszString,
	UINT        cchString,
	int FAR *   lpDX)
{
	HDC         hDCScreen;
	TCHAR       szTempBuff[OLEUI_CCHLABELMAX];
	int         cxString, cyString, cxMaxString;
	int         cxFirstLine, cyFirstLine, cxSecondLine;
	int         index;
	int         cch = cchString;
	TCHAR       chKeep;
	LPTSTR      lpszSecondLine;
	HFONT       hFontT;
	BOOL        fPrintText = TRUE;
	UINT        iLastLineStart = 0;
	SIZE        size;

	// Initialization stuff...

	if (NULL == hDC)  // If we got NULL as the hDC, then we don't actually call ETO
		fPrintText = FALSE;

	// Make a copy of the string (NULL or non-NULL) that we're using
	if (NULL == lpszString)
		*szTempBuff = '\0';
	else
		lstrcpyn(szTempBuff, lpszString, sizeof(szTempBuff)/sizeof(TCHAR));

	// set maximum width
	cxMaxString = lpRect->right - lpRect->left;

	// get screen DC to do text size calculations
	hDCScreen = GetDC(NULL);

	hFontT = (HFONT)SelectObject(hDCScreen, hFont);

	// get the extent of our label
	// GetTextExtentPoint32 has fixed the off-by-one bug, but is not
	//  provided when running under Win32s.
	if (!GetTextExtentPoint32(hDCScreen, szTempBuff, cch, &size))
		GetTextExtentPoint(hDCScreen, szTempBuff, cch, &size);

	cxString = size.cx;
	cyString = size.cy;

	// Select in the font we want to use
	if (fPrintText)
		 SelectObject(hDC, hFont);

	// String is smaller than max string - just center, ETO, and return.
	if (cxString <= cxMaxString)
	{
		if (fPrintText)
			ExtTextOut(hDC,
				  nXStart + (lpRect->right - cxString) / 2,
				  nYStart,
				  fuOptions,
				  lpRect,
				  szTempBuff,
				  cch,
				  NULL);

		iLastLineStart = 0;  // only 1 line of text
		goto CleanupAndLeave;
	}

	// String is too long...we've got to word-wrap it.

	// Are there any spaces, slashes, tabs, or bangs in string?

	if (lstrlen(szTempBuff) != (int)_tcsspn(szTempBuff, szSeparators))
	{
		// Yep, we've got spaces, so we'll try to find the largest
		// space-terminated string that will fit on the first line.
		index = cch;
		while (index >= 0)
		{
			TCHAR cchKeep;

			// scan the string backwards for spaces, slashes, tabs, or bangs
			while (!IS_SEPARATOR(szTempBuff[index]) )
				index--;

			if (index <= 0)
				break;

			cchKeep = szTempBuff[index];  // remember what char was there

			szTempBuff[index] = '\0';   // just for now

			if (!GetTextExtentPoint32(hDCScreen, szTempBuff, lstrlen(szTempBuff), &size))
				GetTextExtentPoint(hDCScreen, szTempBuff, lstrlen(szTempBuff), &size);

			cxFirstLine = size.cx;
			cyFirstLine = size.cy;

			szTempBuff[index] = cchKeep;   // put the right char back

			if (cxFirstLine <= cxMaxString)
			{
				iLastLineStart = index + 1;
				if (!fPrintText)
					goto CleanupAndLeave;

				ExtTextOut(hDC,
					  nXStart +  (lpRect->right - cxFirstLine) / 2,
					  nYStart,
					  fuOptions,
					  lpRect,
					  (LPTSTR)szTempBuff,
					  index + 1,
					  lpDX);

				lpszSecondLine = (LPTSTR)szTempBuff;
				lpszSecondLine += (index + 1) ;

				GetTextExtentPoint(hDCScreen,
									lpszSecondLine,
									lstrlen(lpszSecondLine),
									&size);

				// If the second line is wider than the rectangle, we
				// just want to clip the text.
				cxSecondLine = min(size.cx, cxMaxString);

				ExtTextOut(hDC,
					  nXStart + (lpRect->right - cxSecondLine) / 2,
					  nYStart + cyFirstLine,
					  fuOptions,
					  lpRect,
					  lpszSecondLine,
					  lstrlen(lpszSecondLine),
					  lpDX);

				goto CleanupAndLeave;

			}  // end if
			index--;
		}  // end while
	}  // end if

	// Here, there are either no spaces in the string (strchr(szTempBuff, ' ')
	// returned NULL), or there spaces in the string, but they are
	// positioned so that the first space terminated string is still
	// longer than one line. So, we walk backwards from the end of the
	// string until we find the largest string that will fit on the first
	// line , and then we just clip the second line.

	cch = lstrlen((LPTSTR)szTempBuff);

	chKeep = szTempBuff[cch];
	szTempBuff[cch] = '\0';

	GetTextExtentPoint(hDCScreen, szTempBuff, lstrlen(szTempBuff),&size);

	cxFirstLine = size.cx;
	cyFirstLine = size.cy;

	while (cxFirstLine > cxMaxString)
	{
		// We allow 40 characters in the label, but the metafile is
		// only as wide as 10 W's (for aesthetics - 20 W's wide looked
		// dumb.  This means that if we split a long string in half (in
		// terms of characters), then we could still be wider than the
		// metafile.  So, if this is the case, we just step backwards
		// from the halfway point until we get something that will fit.
		// Since we just let ETO clip the second line

		szTempBuff[cch--] = chKeep;
		if (0 == cch)
			goto CleanupAndLeave;

		chKeep = szTempBuff[cch];
		szTempBuff[cch] = '\0';

		GetTextExtentPoint(
			 hDCScreen, szTempBuff, lstrlen(szTempBuff), &size);
		cxFirstLine = size.cx;
	}
	iLastLineStart = cch;

	if (!fPrintText)
		goto CleanupAndLeave;

	ExtTextOut(hDC,
		nXStart + (lpRect->right - cxFirstLine) / 2,
		nYStart,
		fuOptions,
		lpRect,
		(LPTSTR)szTempBuff,
		lstrlen((LPTSTR)szTempBuff),
		lpDX);

	szTempBuff[cch] = chKeep;
	lpszSecondLine = szTempBuff;
	lpszSecondLine += cch ;

	GetTextExtentPoint(
		  hDCScreen, (LPTSTR)lpszSecondLine, lstrlen(lpszSecondLine), &size);

	// If the second line is wider than the rectangle, we
	// just want to clip the text.
	cxSecondLine = min(size.cx, cxMaxString);

	ExtTextOut(hDC,
		nXStart + (lpRect->right - cxSecondLine) / 2,
		nYStart + cyFirstLine,
		fuOptions,
		lpRect,
		lpszSecondLine,
		lstrlen(lpszSecondLine),
		lpDX);

CleanupAndLeave:
	SelectObject(hDCScreen, hFontT);
	ReleaseDC(NULL, hDCScreen);
	return iLastLineStart;
}
