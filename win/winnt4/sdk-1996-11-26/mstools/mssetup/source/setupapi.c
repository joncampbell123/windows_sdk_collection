//*************************************************************************
//*************************  Setup 'C' Wrappers  **************************
//*************************************************************************


// NOTE: setup code WinMain() must call InitSetupToolkit(szCmdLineArgs) at
// start and EndSetupToolkit() [CleaupTrap] at exit.  It must also define the
// routine SetupError() for error handling.


#include <windows.h>
#include <stdlib.h>		//atol
#include <direct.h>
#include "setupapi.h"
#include "msdetect.h"
#include "msregdb.h"
#include "msshared.h"
#ifdef USE_3D_LOOK
#include "ctl3d.h"
#endif

#define	cchMax	255
#define cchNum	10


/* GLOBALS */
INT  hSetup       = 0;
CHAR szCurDir[255];


// **************************************************************************
LPSTR lstrchr(LPSTR sz, CHAR ch)
{
	while (*sz)
		{
		if (ch == *sz)
			return(sz);
		sz = AnsiNext(sz);
		}
	return(NULL);
}



#ifndef STF_LITE
// **************************************************************************
INT InStr(INT cch, LPSTR sz1, LPSTR sz2)
{
	LPSTR szTmp1, szTmp2;
	INT i = 1;

	if (cch == 0)
		return(0);

	for (cch--; cch > 0; cch--, i++, sz1++)
		{
		if (IsDBCSLeadByte(*sz1))
			{
			if (cch == 1)
				break;
			sz1++;
			cch--;
			i++;
			}
		}

	while (*sz1 != '\0')
		{
		szTmp1 = sz1;
		szTmp2 = sz2;
		while ((*szTmp1 != '\0') && (*szTmp2 != '\0'))
			{
			if (*szTmp1 != *szTmp2)
				break;
			if (IsDBCSLeadByte(*szTmp1))
				{
				if (!IsDBCSLeadByte(*szTmp2))
					break;
				if (*(szTmp1+1) != *(szTmp2+1))
					break;
				}
			szTmp1 = AnsiNext(szTmp1);
			szTmp2 = AnsiNext(szTmp2);
			}
		if (*szTmp2 == '\0')
			return(i);
		if (*sz1 != '\0')
			{
			i += (IsDBCSLeadByte(*sz1) ? 2 : 1);
			sz1 = AnsiNext(sz1);
			}
		}

	return(0);
}
#endif  /* !STF_LITE */

//**************************************************************************
INT InitSetupToolkit(LPSTR szCmdLine)
{
    return InitSetupToolkitEx(szCmdLine, FALSE);
}

//**************************************************************************
INT InitSetupToolkitEx(LPSTR szCmdLine, BOOL fUse3D)
{
	INT  i;
	WORD wErrorModeSav;

	if (hSetup > 0)
		{
#ifdef DEBUG
		StfApiErr(saeInit, "InitSetupToolkit", szCmdLine);
#endif //DEBUG
		return(0);
		}

	if ((i = InitFrame(szCmdLine)) == -1)
		return(-1);

	/* do NOT call FInitRegDb() inside here since some Win3.0 apps don't
	** use or ship it!  Your WinMain() should call it after this routine.
	*/
        if (i == 0 || !InitInstallEx(fUse3D))
		{
		EndSetupToolkit();
		return(0);
		}

	hSetup = i;

	wErrorModeSav = SetErrorMode(1);
	getcwd(szCurDir, 255);             /* for C7 use _getcwd() */
	SetErrorMode(wErrorModeSav);
	OemToAnsi(szCurDir, szCurDir);

	i = lstrlen(szCurDir);
	if (*AnsiPrev(szCurDir, szCurDir + i) != '\\')
		{
		szCurDir[i] = '\\';
		szCurDir[i + 1] = '\0';
		}

	return(hSetup);
}


//**************************************************************************
INT InitFrame(LPSTR szCmdLine)
{
	if (hSetup > 0)
		{
#ifdef DEBUG
		StfApiErr(saeInit, "InitFrame", szCmdLine);
#endif //DEBUG
		return(0);
		}
	else
		{
		INT	i = InitializeFrame(szCmdLine);

		if (i == 0)
			{
#ifdef DEBUG
			StfApiErr(saeFail, "InitFrame", szCmdLine);
#endif //DEBUG
			return(0);
			}
		else
			return(i);
		}
}


//**************************************************************************
VOID SetBitmap(LPSTR szDll, INT Bitmap)
{
	if (FSetBitmap(szDll, Bitmap) == 0)
		{
#ifdef DEBUG
		CHAR rgch[10];

		itoa(Bitmap, rgch, 10);    /* for C7 use _itoa() */
		StfApiErr(saeFail, "SetBitmap", SzCat2Str(szDll, ", ", rgch));
#endif //DEBUG
		SetupError(STFERR);
		}
}


//**************************************************************************
VOID SetAbout(LPSTR szAbout1, LPSTR szAbout2)
{
	if (FSetAbout(szAbout1, szAbout2) == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "SetAbout", SzCat2Str(szAbout1, ",", szAbout2));
#endif //DEBUG
		SetupError(STFERR);
		}
}


//*************************************************************************
VOID SetTitle(LPSTR sz)
{
	SetWindowText((HWND)HwndFrame(), sz);
}


//*************************************************************************
VOID ReadInfFile(LPSTR szFile)
{
#ifdef DEBUG
	if (FValidPath(szFile) == 0)
		BadArgErr(1, "ReadInfFile", szFile);

	if (FOpenInf(szFile, 1, 1) == 0)
		{
		StfApiErr(saeFail, "ReadInfFile", szFile);
		SetupError(STFERR);
		}
#else  //!DEBUG
	if (FOpenInf(szFile, 1, 0) == 0)
		SetupError(STFERR);
#endif //!DEBUG
}


//*************************************************************************
VOID OurYield()
{
	MSG msg;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
		if (msg.message == WM_QUIT)
			SetupError(STFQUIT);

		TranslateMessage(&msg);
		DispatchMessage(&msg);
		}
}


#ifndef STF_LITE
//*************************************************************************
LPSTR UIStartDlgExt(LPSTR szDll, INT Dlg, LPSTR szDlgProc,
		LPSTR szHelpDll, INT HelpDlg, LPSTR szHelpProc, LPSTR szBfr,
		INT cbBfrMax)
{
#ifdef DEBUG
	INT   n;
	LPSTR szTmp;
	CHAR  szDlg[cchNum], szHelpDlg[cchNum];

	if (FEmptySz(szDll))
		n = 1;
	else if (FEmptySz(szDlgProc))
		n = 3;
	else
		n = 0;

	if (n > 0)
		{
		wsprintf(szDlg, "%d", Dlg);
		wsprintf(szHelpDlg, "%d", HelpDlg);
		szTmp = SzCat3Str(szDll, szDlg, ", ", szDlgProc);
		szTmp = SzCat3Str(szTmp, ", ", szHelpDll, ", ");
		BadArgErr(n, "UIStartDlgExt", SzCat3Str(szTmp, szHelpDlg,", ",
				szHelpProc));
		}
#endif //DEBUG

	if (FDoDialogExt((HWND)HwndFrame(), szDll, Dlg, szDlgProc, szHelpDll, HelpDlg,
			szHelpProc) == 0)
		{
#ifdef DEBUG
		wsprintf(szDlg, "%d", Dlg);
		wsprintf(szHelpDlg, "%d", HelpDlg);
		szTmp = SzCat3Str(szDll, szDlg, ", ", szDlgProc);
		szTmp = SzCat3Str(szTmp, ", ", szHelpDll, ", ");
		StfApiErr(saeFail, "UIStartDlgExt", SzCat3Str(szTmp, szHelpDlg, ", ",
				szHelpProc));
#endif //DEBUG
		SetupError(STFERR);
		}
	else
		{
		OurYield();
		return(GetSymbolValue("DLGEVENT", szBfr, cbBfrMax));
		}
}
#endif  /* !STF_LITE */


//*************************************************************************
LPSTR UIStartDlg(LPSTR szDll, INT Dlg, LPSTR szDlgProc, INT HelpDlg,
				LPSTR szHelpProc, LPSTR szBfr, INT cbBfrMax)
{
#ifdef DEBUG
	INT   n;
	LPSTR szTmp;
	CHAR  szDlg[cchNum], szHelpDlg[cchNum];

	if (FEmptySz(szDll))
		n = 1;
	else if (FEmptySz(szDlgProc))
		n = 3;
	else
		n = 0;

	if (n > 0)
		{
		wsprintf(szDlg, "%d", Dlg);
		wsprintf(szHelpDlg, "%d", HelpDlg);
		szTmp = SzCat3Str(szDll, szDlg, ", ", szDlgProc);
		BadArgErr(n, "UIStartDlg", SzCat3Str(szTmp, szHelpDlg,", ",szHelpProc));
		}
#endif //DEBUG

	if (FDoDialogExt((HWND)HwndFrame(), szDll, Dlg, szDlgProc, szDll, HelpDlg,
			szHelpProc) == 0)
		{
#ifdef DEBUG
		wsprintf(szDlg, "%d", Dlg);
		wsprintf(szHelpDlg, "%d", HelpDlg);
		szTmp = SzCat3Str(szDll, szDlg, ", ", szDlgProc);
		StfApiErr(saeFail, "UIStartDlg", SzCat3Str(szTmp, szHelpDlg, ", ",
				szHelpProc));
#endif //DEBUG
		SetupError(STFERR);
		}
	else
		{
		OurYield();
		return(GetSymbolValue("DLGEVENT", szBfr, cbBfrMax));
		}
}


//*************************************************************************
void UIPop(INT n)
{
	if (FKillNDialogs(n) == 0)
		{
#ifdef DEBUG
		CHAR szNum[cchNum];

		wsprintf(szNum, "%d", n);
		StfApiErr(saeFail, "UIPop", szNum);
#endif //DEBUG
		SetupError(STFERR);
		}
	OurYield();
}


//*************************************************************************
VOID UIPopAll(VOID)
{
	if (FKillNDialogs(-1) == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "UIPopAll", "");
#endif //DEBUG
		SetupError(STFERR);
		}
	OurYield();
}


//*************************************************************************
LPSTR GetSymbolValue(LPSTR szSymbol, LPSTR szBfr, INT cbBfrMax)
{
	INT length;

#ifdef DEBUG
	if (FEmptySz(szSymbol))
		BadArgErr(1, "GetSymbolValue", szSymbol);
#endif //DEBUG

	if (szSymbol == NULL || *szSymbol == '\0')
		SetupError(STFERR);

	length = CbGetSymbolValue(szSymbol, szBfr, cbBfrMax);
	if (length >= cbBfrMax)
		{
		DoMsgBox("Buffer Overflow", "MS-Setup Error", MB_ICONHAND+MB_OK);
		SetupError(STFERR);
		}

	return(szBfr);
}


//*************************************************************************
INT GetListLength(LPSTR szSymbol)
{
	USHORT cb;

#ifdef DEBUG
	if (FEmptySz(szSymbol))
		BadArgErr(1, "GetListLength", szSymbol);
#endif //DEBUG

	if (szSymbol == NULL || *szSymbol == '\0')
		SetupError(STFERR);
	cb = UsGetListLength(szSymbol);

	return((INT)cb);
}


//*************************************************************************
LPSTR GetListItem(LPSTR szListSymbol, INT nItem, LPSTR szBfr,
		INT cbBfrMax)
{
	INT	length;

#ifdef DEBUG
	CHAR szItem[cchNum];

	if (FEmptySz(szListSymbol))
		{
		wsprintf(szItem, "%d", nItem);
		BadArgErr(1, "GetListItem", SzCat2Str(szListSymbol,", ", szItem));
		}

	if ((nItem <= 0) || (nItem > GetListLength(szListSymbol)))
		{
		wsprintf(szItem, "%d", nItem);
		BadArgErr(2, "GetListItem",SzCat2Str(szListSymbol,", ",szItem));
		}
#endif //DEBUG

	if (szListSymbol == NULL || *szListSymbol == '\0')
		SetupError(STFERR);
	if ((nItem <= 0) || (nItem > GetListLength(szListSymbol)))
		SetupError(STFERR);
	length = CbGetListItem(szListSymbol, nItem, szBfr, cbBfrMax);
	if (length >= cbBfrMax)
		{
		DoMsgBox("Buffer Overflow", "MS-Setup Error", MB_ICONHAND+MB_OK);
		SetupError(STFERR);
		}

	return(szBfr);
}


//*************************************************************************
VOID AddListItem(LPSTR szSymbol, LPSTR szItem)
{
#ifdef DEBUG
	if (FEmptySz(szSymbol))
		BadArgErr(1, "AddListItem", SzCat2Str(szSymbol,", ",szItem));
#endif //DEBUG

	if (FAddListItem(szSymbol, szItem) == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "AddListItem",SzCat2Str(szSymbol,", ",szItem));
#endif //DEBUG
		SetupError(STFERR);
		}
}


//*************************************************************************
VOID ReplaceListItem(LPSTR szSymbol, INT n, LPSTR szItem)
{
#ifdef DEBUG
	INT   nArg;
	CHAR  szNum[cchNum];
	LPSTR szTmp;

	if (FEmptySz(szSymbol))
		nArg = 1;
	else if ((n <= 0) || (n > GetListLength(szSymbol)))
		nArg = 2;
	else
		nArg = 0;

	if (nArg > 0)
		{
		wsprintf(szNum,"%d", n);
		szTmp = SzCat3Str(szSymbol, ", ", szNum, ", ");
		BadArgErr(nArg, "ReplaceListItem", SzCatStr(szTmp,szItem));
		}
#endif //DEBUG

	if (FReplaceListItem(szSymbol, n, szItem) == 0)
		{
#ifdef DEBUG
		wsprintf(szNum,"%d", n);
		szTmp = SzCat3Str(szSymbol, ", ", szNum, ", ");
		StfApiErr(saeFail, "ReplaceListItem", SzCatStr(szTmp,szItem));
#endif //DEBUG
		SetupError(STFERR);
		}
}


//**************************************************************************
VOID MakeListFromSectionKeys(LPSTR szSymbol, LPSTR szSect)
{
#ifdef DEBUG
	INT n;

	if (FEmptySz(szSymbol))
		n = 1;
	else if (FValidInfSect(szSect) == 0)
		n = 2;
	else
		n = 0;
	if (n > 0)
		BadArgErr(n, "MakeListFromSectionKeys",SzCat2Str(szSymbol,", ",szSect));
#endif //DEBUG

	if (FSetSymbolToListOfInfKeys(szSymbol, szSect, 1) == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "MakeListFromSectionKeys", SzCat2Str(szSymbol, ", ",
				szSect));
#endif //DEBUG
		SetupError(STFERR);
		}
}


//*************************************************************************
VOID SetSymbolValue(LPSTR szSymbol, LPSTR szValue)
{
#ifdef DEBUG
	if (FEmptySz(szSymbol))
		BadArgErr(1, "SetSymbolValue",SzCat2Str(szSymbol,", ",szValue));
#endif //DEBUG

	if (FSetSymbolValue(szSymbol, szValue) == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "SetSymbolValue", SzCat2Str(szSymbol,", ",szValue));
#endif //DEBUG
		SetupError(STFERR);
		}
}


//*************************************************************************
VOID RemoveSymbol(LPSTR szSym)
{
#ifdef DEBUG
	if (FEmptySz(szSym))
		BadArgErr(1, "RemoveSymbol", szSym);
#endif //DEBUG

	if (FRemoveSymbol(szSym) == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "RemoveSymbol", szSym);
#endif //DEBUG
		SetupError(STFERR);
		}
}


//*************************************************************************
INT ShowWaitCursor(VOID)
{
	return(HShowWaitCursor());
}


//*************************************************************************
VOID RestoreCursor(INT hPrev)
{
	if (FRestoreCursor(hPrev) == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "RestoreCursor", "");
#endif //DEBUG
		SetupError(STFERR);
		}
}


#ifndef STF_LITE
//*************************************************************************
INT SetBeepingMode(INT mode)
{
	return(FSetBeepingMode(mode));
}


//*************************************************************************
INT SetSilentMode(INT mode)
{
	return(FSetSilent(mode));
}

#endif  /* !STF_LITE */

//*************************************************************************
LPSTR GetSectionKeyDate(LPSTR szSect, LPSTR szKey, LPSTR szBfr,
		INT cbBfrMax)
{
	INT	length;
#ifdef DEBUG
	INT n;

	if (FValidInfSect(szSect) == 0)
		n = 1;
	else if (FEmptySz(szKey))
		n = 2;
	else
		n = 0;
	if (n > 0)
		BadArgErr(n, "GetSectionKeyDate",SzCat2Str(szSect,", ",szKey));
#endif //DEBUG

	length = CbGetInfSectionKeyField(szSect, szKey, 5, szBfr, cbBfrMax);
	if (length >= cbBfrMax)
		{
		DoMsgBox("Buffer Overflow", "MS-Setup Error", MB_ICONHAND+MB_OK);
		SetupError(STFERR);
		}

	if (length == -1)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "GetSectionKeyDate",SzCat2Str(szSect,", ",szKey));
#endif //DEBUG
		SetupError(STFERR);
		}

	return(szBfr);
}


//*************************************************************************
LPSTR GetSectionKeyFilename(LPSTR szSect, LPSTR szKey, LPSTR szBfr,
		INT cbBfrMax)
{
	INT	length;
#ifdef DEBUG
	INT n;

	if (FValidInfSect(szSect) == 0)
		n = 1;
	else if (FEmptySz(szKey))
		n = 2;
	else
		n = 0;
	if (n > 0)
		BadArgErr(n, "GetSectionKeyFilename",SzCat2Str(szSect,", ",szKey));
#endif //DEBUG

	length = CbGetInfSectionKeyField(szSect, szKey, 1, szBfr, cbBfrMax);
	if (length >= cbBfrMax)
		{
		DoMsgBox("Buffer Overflow", "MS-Setup Error", MB_ICONHAND+MB_OK);
		SetupError(STFERR);
		}

	if (length == -1)
		{
#ifdef DEBUG
		StfApiErr(saeFail,"GetSectionKeyFilename",SzCat2Str(szSect,", ",szKey));
#endif //DEBUG
		SetupError(STFERR);
		}

	return(szBfr);
}


//*************************************************************************
LONG GetSectionKeySize(LPSTR szSect, LPSTR szKey)
{
	CHAR szBfr[128];
	INT	 length;
#ifdef DEBUG
	INT  n;

	if (FValidInfSect(szSect) == 0)
		n = 1;
	else if (FEmptySz(szKey))
		n = 2;
	else
		n = 0;
	if (n > 0)
		BadArgErr(n, "GetSectionKeySize",SzCat2Str(szSect,", ",szKey));
#endif //DEBUG

	length = CbGetInfSectionKeyField(szSect, szKey, 15, szBfr, 128);
	if (length >= 128)
		{
		DoMsgBox("Buffer Overflow", "MS-Setup Error", MB_ICONHAND+MB_OK);
		SetupError(STFERR);
		}

	if (length == -1)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "GetSectionKeySize",SzCat2Str(szSect,", ",szKey));
#endif //DEBUG
		SetupError(STFERR);
		}

	return(atol(szBfr));
}


#ifndef STF_LITE
//*************************************************************************
LPSTR GetSectionKeyVersion(LPSTR szSect, LPSTR szKey, LPSTR szBfr,
		INT cbBfrMax)
{
	INT	length;
#ifdef DEBUG
	INT n;

	if (FValidInfSect(szSect) == 0)
		n = 1;
	else if (FEmptySz(szKey))
		n = 2;
	else
		n = 0;
	if (n > 0)
		BadArgErr(n, "GetSectionKeyVersion",SzCat2Str( szSect,", ",szKey));
#endif //DEBUG

	length = CbGetInfSectionKeyField(szSect, szKey, 19, szBfr, cbBfrMax);
	if (length >= cbBfrMax)
		{
		DoMsgBox("Buffer Overflow", "MS-Setup Error", MB_ICONHAND+MB_OK);
		SetupError(STFERR);
		}

	if (length == -1)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "GetSectionKeyVersion",SzCat2Str(szSect,", ",szKey));
#endif //DEBUG
		SetupError(STFERR);
		}

	return(szBfr);
}
#endif  /* !STF_LITE */


#ifndef STF_LITE
//*************************************************************************
VOID MakeListFromSectionDate(LPSTR szSym, LPSTR szSect)
{
#ifdef DEBUG
	INT n;

	if (FEmptySz(szSym))
		n = 1;
	else if (FValidInfSect(szSect) == 0)
		n = 2;
	else
		n = 0;
	if (n > 0)
		BadArgErr(n, "MakeListFromSectionDate",SzCat2Str( szSym,", ",szSect));
#endif //DEBUG

	if (FMakeListInfSectionField(szSym, szSect, 5) == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "MakeListFromSectionDate", SzCat2Str(szSym, ", ",
				szSect));
#endif //DEBUG
		SetupError(STFERR);
		}
}

#endif  /* !STF_LITE */

//*************************************************************************
VOID MakeListFromSectionFilename(LPSTR szSym, LPSTR szSect)
{
#ifdef DEBUG
	INT n;

	if (FEmptySz(szSym))
		n = 1;
	else if (FValidInfSect(szSect) == 0)
		n = 2;
	else
		n = 0;
	if (n > 0)
		BadArgErr(n, "MakeListFromSectionFilename", SzCat2Str(szSym, ", ",
				szSect));
#endif //DEBUG

	if (FMakeListInfSectionField(szSym, szSect, 1) == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "MakeListFromSectionFilename", SzCat2Str(szSym, ", ",
				szSect));
#endif //DEBUG
		SetupError(STFERR);
		}
}


//*************************************************************************
VOID MakeListFromSectionRename(LPSTR szSym, LPSTR szSect)
{
#ifdef DEBUG
	INT n;

	if (FEmptySz(szSym))
		n = 1;
	else if (FValidInfSect(szSect) == 0)
		n = 2;
	else
		n = 0;
	if (n > 0)
		BadArgErr(n, "MakeListFromSectionFilename", SzCat2Str(szSym, ", ",
				szSect));
#endif //DEBUG

	if (FMakeListInfSectionField(szSym, szSect, 11) == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "MakeListFromSectionRename", SzCat2Str(szSym, ", ",
				szSect));
#endif //DEBUG
		SetupError(STFERR);
		}
}



//*************************************************************************
VOID MakeListFromSectionSize(LPSTR szSym, LPSTR szSect)
{
#ifdef DEBUG
	INT n;

	if (FEmptySz(szSym))
		n = 1;
	else if (FValidInfSect(szSect) == 0)
		n = 2;
	else
		n = 0;
	if (n > 0)
		BadArgErr(n, "MakeListFromSectionSize",SzCat2Str( szSym,", ",szSect));
#endif //DEBUG

	if (FMakeListInfSectionField(szSym, szSect, 15) == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "MakeListFromSectionSize", SzCat2Str(szSym, ", ",
				szSect));
#endif //DEBUG
		SetupError(STFERR);
		}
}


#ifndef STF_LITE
//*************************************************************************
VOID MakeListFromSectionVersion(LPSTR szSym, LPSTR szSect)
{
#ifdef DEBUG
	INT n;

	if (FEmptySz(szSym))
		n = 1;
	else if (FValidInfSect(szSect) == 0)
		n = 2;
	else
		n = 0;
	if (n > 0)
		BadArgErr(n, "MakeListFromSectionVersion",SzCat2Str(szSym,", ",szSect));
#endif //DEBUG

	if (FMakeListInfSectionField(szSym, szSect, 19) == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "MakeListFromSectionVersion", SzCat2Str(szSym, ", ",
				szSect));
#endif //DEBUG
		SetupError(STFERR);
		}
}
#endif  /* !STF_LITE */

//*************************************************************************
INT InitInstall()
{
    return InitInstallEx(FALSE);
}

//*************************************************************************
INT InitInstallEx(BOOL fUse3D)
{
	HWND hFrame;
	HINSTANCE hInsta;

	if (hSetup > 0)
		{
#ifdef DEBUG
		StfApiErr(saeInit, "InitInstall", "");
#endif //DEBUG
		return(0);
		}
	
	hFrame = HwndFrame();
	hInsta = HinstFrame();


	if (FInitializeInstall((HANDLE)hInsta, hFrame) == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "InitInstall", "");
#endif //DEBUG
		return(0);
		}

#ifdef USE_3D_LOOK
        if (fUse3D) {
            Ctl3dRegister(GetModuleHandle(NULL));
            Ctl3dAutoSubclass(GetModuleHandle(NULL));
        }
#endif
	return(1);
}


//*************************************************************************
VOID CreateDir(LPSTR szDir, INT cmo)
{
#ifdef DEBUG
	CHAR szCmo[cchNum];

	if (FValidDir(szDir) == 0)
		{
		wsprintf(szCmo, "%d", cmo);
		BadArgErr(1, "CreateDir",SzCat2Str(szDir,", ",szCmo));
		}
#endif //DEBUG

	if (FCreateDir(szDir, cmo) == 0)
		{
#ifdef DEBUG
		CHAR szCmo[cchNum];

		wsprintf(szCmo, "%d", cmo);
		StfApiErr(saeFail, "CreateDir",SzCat2Str(szDir,", ",szCmo));
#endif //DEBUG
		SetupError(STFERR);
		}
}


#ifndef STF_LITE
//*************************************************************************
VOID RemoveDir(LPSTR szDir, INT cmo)
{
#ifdef DEBUG
	CHAR szCmo[cchNum];

	if (FValidDir(szDir) == 0)
		{
		wsprintf(szCmo, "%d", cmo);
		BadArgErr(1, "RemoveDir",SzCat2Str(szDir,", ",szCmo));
		}
#endif //DEBUG

	if (FRemoveDir(szDir, cmo) == 0)
		{
#ifdef DEBUG
		wsprintf(szCmo, "%d", cmo);
		StfApiErr(saeFail, "RemoveDir",SzCat2Str(szDir,", ",szCmo));
#endif //DEBUG
		SetupError(STFERR);
		}
}


//*************************************************************************
VOID RemoveIniSection(LPSTR szFile, LPSTR szSect, INT cmo)
{
#ifdef DEBUG
	INT   n;
	CHAR  szCmo[cchNum];
	LPSTR szTmp;

	if (FValidIniFile(szFile) == 0)
		n = 1;
	else if (FValidInfSect(szSect) == 0)
		n = 2;
	else
		n = 0;
	if (n > 0)
		{
		wsprintf(szCmo, "%d", cmo);
		szTmp = SzCat3Str(szFile, ", ", szSect, ", ");
		BadArgErr(n, "RemoveIniSection",SzCatStr(szTmp,szCmo));
		}
#endif //DEBUG

	if (FRemoveIniSection(szFile, szSect, cmo) == 0)
		{
#ifdef DEBUG
		wsprintf(szCmo, "%d", cmo);
		szTmp = SzCat3Str(szFile, ", ", szSect, ", ");
		StfApiErr(saeFail, "RemoveIniSection", SzCatStr(szTmp,szCmo));
#endif //DEBUG
		SetupError(STFERR);
		}
}
#endif  /* !STF_LITE */


//*************************************************************************
VOID CreateIniKeyValue(LPSTR szFile, LPSTR szSect, LPSTR szKey,
		LPSTR szValue, INT cmo)
{
#ifdef DEBUG
	CHAR  szCmo[cchNum];
	LPSTR szTmp1;
	INT   n;

	if (FValidIniFile(szFile) == 0)
		n = 1;
	else if (FValidInfSect(szSect) == 0)
		n = 2;
	else
		n = 0;
	if (n > 0)
		{
		wsprintf(szCmo, "%d", cmo);
		szTmp1 = SzCat3Str(szFile, ", ", szSect, ", ");
		szTmp1 = SzCat3Str(szTmp1, szKey, ", ", szValue);
		BadArgErr(n, "CreateIniKeyValue", SzCat2Str(szTmp1, ", ", szCmo));
		}
#endif //DEBUG

	if (FCreateIniKeyValue(szFile, szSect, szKey, szValue, cmo) == 0)
		{
#ifdef DEBUG
		wsprintf(szCmo, "%d", cmo);
		szTmp1 = SzCat3Str(szFile, ", ", szSect, ", ");
		szTmp1 = SzCat3Str(szTmp1, szKey, ", ", szValue);
		StfApiErr(saeFail, "CreateIniKeyValue", SzCat2Str(szTmp1, ", ", szCmo));
#endif //DEBUG
		SetupError(STFERR);
		}
}


#ifndef STF_LITE
//*************************************************************************
VOID RemoveIniKey(LPSTR szFile, LPSTR szSect, LPSTR szKey, INT cmo)
{
#ifdef DEBUG
	CHAR  szCmo[cchNum];
	LPSTR szTmp1;
	INT   n;

	if (FValidIniFile(szFile) == 0)
		n = 1;
	else if (FValidInfSect(szSect) == 0)
		n = 2;
	else if (FEmptySz(szKey))
		n = 3;
	else
		n = 0;
	if (n > 0)
		{
		wsprintf(szCmo, "%d", cmo);
		szTmp1 = SzCat3Str(szFile, ", ", szSect, ", ");
		szTmp1 = SzCat3Str(szTmp1, szKey, ", ", szCmo);
		BadArgErr(n, "RemoveIniKey", szTmp1);
		}
#endif //DEBUG

	if (FRemoveIniKey(szFile, szSect, szKey, cmo) == 0)
		{
#ifdef DEBUG
		wsprintf(szCmo, "%d", cmo);
		szTmp1 = SzCat3Str(szFile, ", ", szSect, ", ");
		szTmp1 = SzCat3Str(szTmp1, szKey, ", ", szCmo);
		StfApiErr(saeFail, "RemoveIniKey", szTmp1);
#endif //DEBUG
		SetupError(STFERR);
		}
}
#endif  /* !STF_LITE */


#ifndef STF_LITE
//*************************************************************************
VOID CreateSysIniKeyValue(LPSTR szFile, LPSTR szSect, LPSTR szKey,
				LPSTR szValue, INT cmo)
{
#ifdef DEBUG
	CHAR  szCmo[cchNum];
	LPSTR szTmp1;
	INT   n;

	if (FValidPath(szFile) == 0)
		n = 1;
	else if (FValidInfSect(szSect) == 0)
		n = 2;
	else if (FEmptySz(szKey))
		n = 3;
	else
		n = 0;
	if (n > 0)
		{
		wsprintf(szCmo, "%d", cmo);
		szTmp1 = SzCat3Str(szFile, ", ", szSect, ", ");
		szTmp1 = SzCat3Str(szTmp1, szKey, ", ", szValue);
		BadArgErr(n, "CreateSysIniKeyValue", SzCat2Str(szTmp1, ", ", szCmo));
		}
#endif //DEBUG

	if (FCreateSysIniKeyValue(szFile, szSect, szKey, szValue, cmo) == 0)
		{
#ifdef DEBUG
		wsprintf(szCmo, "%d", cmo);
		szTmp1 = SzCat3Str(szFile, ", ", szSect, ", ");
		szTmp1 = SzCat3Str(szTmp1, szKey, ", ", szValue);
		StfApiErr(saeFail, "CreateSysIniKeyValue",SzCat2Str(szTmp1, ", ",
				szCmo));
#endif //DEBUG
		SetupError(STFERR);
		}
}
#endif  /* !STF_LITE */


//*************************************************************************
VOID CreateProgmanGroup(LPSTR szGroup, LPSTR szPath, INT cmo)
{
#ifdef DEBUG
	CHAR  szCmo[cchNum];
	LPSTR szTmp;

	if (FEmptySz(szGroup) || (lstrlen(szGroup) > 24))
		{
		szTmp = SzCat3Str(szGroup, ", ", szPath, ", ");
		BadArgErr(1, "CreateProgmanGroup",SzCatStr(szTmp, szCmo));
		}
#endif //DEBUG

	if (FCreateProgManGroup(szGroup, szPath, cmo) == 0)
		{
#ifdef DEBUG
		szTmp = SzCat3Str(szGroup, ", ", szPath, ", ");
		StfApiErr(saeFail, "CreateProgmanGroup",SzCatStr(szTmp,szCmo));
#endif //DEBUG
		SetupError(STFERR);
		}
}

#ifdef WIN32
//*************************************************************************
VOID CreateProgmanGroupEx(LPSTR szGroup, BOOL fCommon, INT cmo)
{
#ifdef DEBUG
	CHAR  szCmo[cchNum];
	LPSTR szTmp;

	if (FEmptySz(szGroup) || (lstrlen(szGroup) > 24))
		{
		szTmp = SzCat3Str(szGroup, ", ", fCommon?"Common":"Private", ", ");
		BadArgErr(1, "CreateProgmanGroupEx",SzCatStr(szTmp, szCmo));
		}
#endif //DEBUG

	if (FCreateProgManGroupEx(szGroup, fCommon, cmo) == 0)
		{
#ifdef DEBUG
		szTmp = SzCat3Str(szGroup, ", ", fCommon?"Common":"Private", ", ");
		StfApiErr(saeFail, "CreateProgmanGroupEx",SzCatStr(szTmp,szCmo));
#endif //DEBUG
		SetupError(STFERR);
		}
}
#endif //WIN32


//*************************************************************************
VOID ShowProgmanGroup(LPSTR szGroup, INT Cmd, INT cmo)
{
	CHAR  szT[255];
#ifdef DEBUG
	CHAR  szCmo[cchNum];
	CHAR  szCmd[cchNum];
	LPSTR szTmp;

	if (FEmptySz(szGroup) || (lstrlen(szGroup) > 24))
		{
		wsprintf(szCmo, "%d", cmo);
		wsprintf(szCmd, "%d", Cmd);
		szTmp = SzCat3Str(szGroup, ", ", szCmd, ", ");
		BadArgErr(1, "ShowProgmanGroup",SzCatStr(szTmp, szCmo));
		}
#endif //DEBUG

	/* for C7 use _itoa() */
	if (FShowProgManGroup(szGroup, itoa(Cmd, szT, 10), cmo) == 0)
		{
#ifdef DEBUG
		wsprintf(szCmo, "%d", cmo);
		wsprintf(szCmd, "%d", Cmd);
		szTmp = SzCat3Str(szGroup, ", ", szCmd, ", ");
		StfApiErr(saeFail, "ShowProgmanGroup",SzCatStr(szTmp, szCmo));
#endif //DEBUG
		SetupError(STFERR);
		}
}


#ifdef WIN32
//*************************************************************************
VOID ShowProgmanGroupEx(LPSTR szGroup, BOOL fCommon, INT Cmd, INT cmo)
{
	CHAR  szT[255];
#ifdef DEBUG
	CHAR  szCmo[cchNum];
	CHAR  szCmd[cchNum];
	LPSTR szTmp;

	if (FEmptySz(szGroup) || (lstrlen(szGroup) > 24))
		{
		wsprintf(szCmo, "%d", cmo);
		wsprintf(szCmd, "%d", Cmd);
		szTmp = SzCat3Str(szGroup, ", ", szCmd, ", ");
		BadArgErr(1, "ShowProgmanGroup",SzCatStr(szTmp, szCmo));
		}
#endif //DEBUG

	/* for C7 use _itoa() */
	if (FShowProgManGroupEx(szGroup, fCommon, itoa(Cmd, szT, 10), cmo) == 0)
		{
#ifdef DEBUG
		wsprintf(szCmo, "%d", cmo);
		wsprintf(szCmd, "%d", Cmd);
		szTmp = SzCat3Str(szGroup, ", ", szCmd, ", ");
		StfApiErr(saeFail, "ShowProgmanGroupEx",SzCatStr(szTmp, szCmo));
#endif //DEBUG
		SetupError(STFERR);
		}
}
#endif


//*************************************************************************
VOID StampResource(LPSTR szSect, LPSTR szKey, LPSTR szDst,
		INT wResType, INT wResId, LPSTR szData, INT cbData)
{
#ifdef DEBUG
	INT   n;
	CHAR  szResType[cchNum], szResId[cchNum], szcbData[cchNum];
	LPSTR szTmp1;

	if (FValidInfSect(szSect) == 0)
		n = 1;
	else if (FEmptySz(szKey))
		n = 2;
	else if (FValidDir(szDst) == 0)
		n = 3;
	else
		n = 0;
	if (n > 0)
		{
		wsprintf(szResType, "%d", wResType);
		wsprintf(szResId, "%d", wResId);
		wsprintf(szcbData, "%d", cbData);
		szTmp1 = SzCat3Str(szSect, ", ", szKey, ", ");
		szTmp1 = SzCat3Str(szTmp1, szDst, ", ", szResType);
		szTmp1 = SzCat3Str(szTmp1, ", ", szResId, ", ");
		BadArgErr(n,"StampResource",SzCat3Str(szTmp1, szData, ", ",szcbData));
		}
#endif //DEBUG

	if (FStampResource(szSect,szKey,szDst,wResType,wResId,szData,cbData) == 0)
		{
#ifdef DEBUG
		wsprintf(szResType, "%d", wResType);
		wsprintf(szResId, "%d", wResId);
		wsprintf(szcbData, "%d", cbData);
		szTmp1 = SzCat3Str(szSect, ", ", szKey, ", ");
		szTmp1 = SzCat3Str(szTmp1, szDst, ", ", szResType);
		szTmp1 = SzCat3Str(szTmp1, ", ", szResId, ", ");
		StfApiErr(saeFail,"StampResource",SzCat3Str(szTmp1, szData, ", ",
				szcbData));
#endif //DEBUG
		SetupError(STFERR);
		}
}

#ifndef STF_LITE

//*************************************************************************
VOID DumpCopyList(LPSTR szFile)
{
#ifdef DEBUG
	if (FEmptySz(szFile))
		BadArgErr(1, "DumpCopyList", szFile);
#endif //DEBUG

	if (FDumpCopyListToFile(szFile) == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "DumpCopyList", szFile);
#endif //DEBUG
		SetupError(STFERR);
		}
}
#endif  /* !STF_LITE */

//*************************************************************************
VOID ClearCopyList(VOID)
{
	ResetCopyList();
}


#ifdef WIN16
//*************************************************************************
LONG GetCopyListCost(LPSTR szExtraList, LPSTR szCostList,
		LPSTR szNeedList)
{
	LONG lNeed = LcbGetCopyListCost(szExtraList, szCostList, szNeedList);

	if (lNeed < 0)
		{
#ifdef DEBUG
	LPSTR szTmp;
		szTmp = SzCat3Str(szExtraList, ", ", szCostList, ", ");
		StfApiErr(saeFail, "GetCopyListCost", SzCatStr(szTmp,szNeedList));
#endif //DEBUG
		SetupError(STFERR);
		}

	return(lNeed);
}
#endif
#ifdef WIN32
//*************************************************************************
LONGLONG GetCopyListCost(LPSTR szExtraList, LPSTR szCostList,
		LPSTR szNeedList)
{
	LONGLONG lNeed = LcbGetCopyListCost(szExtraList, szCostList, szNeedList);

	if (lNeed < 0)
		{
#ifdef DEBUG
	LPSTR szTmp;
		szTmp = SzCat3Str(szExtraList, ", ", szCostList, ", ");
		StfApiErr(saeFail, "GetCopyListCost", SzCatStr(szTmp,szNeedList));
#endif //DEBUG
		SetupError(STFERR);
		}

	return(lNeed);
}
#endif




//*************************************************************************
VOID CreateProgmanItem(LPSTR szGroup, LPSTR szItem, LPSTR szCmd,
		LPSTR szOther, INT cmo)
{
	LPSTR szItemNew = szItem;
#ifdef DEBUG
	CHAR  szCmo[cchNum];
	LPSTR szTmp1;
#endif //DEBUG

	if (szOther != NULL && *szOther != '\0')
		szItemNew = SzCat2Str(szItem, ",", szOther);

#ifdef DEBUG
	if (FEmptySz(szGroup) || (lstrlen(szGroup) > 24))
		{
		wsprintf(szCmo, "%d", cmo);
		szTmp1 = SzCat3Str(szGroup, ", ", szCmd, ", ");
		BadArgErr(1, "CreateProgmanItem",SzCatStr(szTmp1,szCmo));
		}
#endif //DEBUG

	if (FCreateProgManItem(szGroup, szItemNew, szCmd, cmo) == 0)
		{
#ifdef DEBUG
		wsprintf(szCmo, "%d", cmo);
		szTmp1 = SzCat3Str(szGroup, ", ", szItem, ", ");
		szTmp1 = SzCat3Str(szTmp1, szCmd, ", ", szOther);
		StfApiErr(saeFail,"CreateProgmanItem", SzCat2Str(szTmp1, ", ", szCmo));
#endif //DEBUG
		SetupError(STFERR);
		}
}

#ifdef WIN32
//*************************************************************************
VOID CreateProgmanItemEx(LPSTR szGroup, BOOL fCommon, LPSTR szItem, LPSTR szCmd,
		LPSTR szOther, INT cmo)
{
	LPSTR szItemNew = szItem;
#ifdef DEBUG
	CHAR  szCmo[cchNum];
	LPSTR szTmp1;
#endif //DEBUG

	if (szOther != NULL && *szOther != '\0')
		szItemNew = SzCat2Str(szItem, ",", szOther);

#ifdef DEBUG
	if (FEmptySz(szGroup) || (lstrlen(szGroup) > 24))
		{
		wsprintf(szCmo, "%d", cmo);
		szTmp1 = SzCat3Str(szGroup, ", ", szCmd, ", ");
		BadArgErr(1, "CreateProgmanItemEx",SzCatStr(szTmp1,szCmo));
		}
#endif //DEBUG

	if (FCreateProgManItemEx(szGroup, fCommon, szItemNew, szCmd, cmo) == 0)
		{
#ifdef DEBUG
		wsprintf(szCmo, "%d", cmo);
		szTmp1 = SzCat3Str(szGroup, ", ", szItem, ", ");
		szTmp1 = SzCat3Str(szTmp1, szCmd, ", ", szOther);
		StfApiErr(saeFail,"CreateProgmanItemEx", SzCat2Str(szTmp1, ", ", szCmo));
#endif //DEBUG
		SetupError(STFERR);
		}
}
#endif //WIN32


#ifndef STF_LITE
//*************************************************************************
VOID AddDos5Help(LPSTR szProgName, LPSTR szProgHelp, INT cmo)
{
#ifndef WIN32
#ifdef DEBUG
	INT   n;
	CHAR  szCmo[cchNum];
	LPSTR szTmp;

	if (FEmptySz(szProgName)
			|| (szProgName[0] == '@')
			|| (lstrlen(szProgName) > 8))
		n = 1;
	else if ((lstrchr(szProgName, ' ') != NULL)
			|| (lstrchr(szProgName,'\t') != NULL))
		n = 1;
	else if (FEmptySz(szProgHelp))
		n = 2;
	else
		n = 0;
	if (n > 0)
		{
		wsprintf(szCmo, "%d", cmo);
		szTmp = SzCat3Str(szProgName, ", ",szProgHelp, ", ");
		BadArgErr(n, "AddDos5Help", SzCatStr(szTmp,szCmo));
		}
#endif //DEBUG

	if (FAddDos5Help(szProgName, szProgHelp, cmo) == 0)
		{
#ifdef DEBUG
		wsprintf(szCmo, "%d", cmo);
		szTmp = SzCat3Str(szProgName, ", ",szProgHelp, ", ");
		StfApiErr(saeFail, "AddDos5Help",SzCatStr(szTmp, szCmo));
#endif //DEBUG
		SetupError(STFERR);
		}
#endif
}
#endif  /* !STF_LITE */


//*************************************************************************
VOID CopyFilesInCopyList(VOID)
{
	INT	grc = GrcCopyFilesInCopyList((HANDLE)HinstFrame());

	if (grc == grcUserQuit)
		SetupError(STFQUIT);
	else if (grc > 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "CopyFilesInCopyList", "");
#endif //DEBUG
		SetupError(STFERR);
		}
}


//*************************************************************************
VOID CopyAFile(LPSTR szFullPathSrc, LPSTR szFullPathDst, INT cmo,
		INT fAppend)
{
#ifdef DEBUG
	INT   n;
	CHAR  szCmo[cchNum], szAppend[cchNum];
	LPSTR szTmp;

	if (FValidPath(szFullPathSrc) == 0)
		n = 1;
	else if (FValidPath(szFullPathDst) == 0)
		n = 2;
	else
		n = 0;
	if (n > 0)
		{
		wsprintf(szCmo, "%d", cmo);
		wsprintf(szAppend, "%d", fAppend);
		szTmp = SzCat3Str(szFullPathSrc, ", ", szFullPathDst, ", ");
		BadArgErr(n, "CopyAFile",SzCat3Str(szTmp,szCmo,", ",szAppend));
		}
#endif //DEBUG

	if (FCopyOneFile(szFullPathSrc, szFullPathDst, (cmo|cmoCopy), fAppend) == 0)
		{
#ifdef DEBUG
		wsprintf(szCmo, "%d", cmo);
		wsprintf(szAppend, "%d", fAppend);
		szTmp = SzCat3Str(szFullPathSrc, ", ", szFullPathDst, ", ");
		StfApiErr(saeFail, "CopyAFile",SzCat3Str(szTmp, szCmo,", ",szAppend));
#endif //DEBUG
		SetupError(STFERR);
		}
}


//*************************************************************************
VOID RemoveFile(LPSTR szFullPathSrc, INT cmo)
{
#ifdef DEBUG
	CHAR szCmo[cchNum];

	if (FValidPath(szFullPathSrc) == 0)
		{
		wsprintf(szCmo, "%d", cmo);
		BadArgErr(1, "RemoveFile",SzCat2Str(szFullPathSrc,", ",szCmo));
		}
#endif //DEBUG

	if (YnrcRemoveFile(szFullPathSrc, cmo) == ynrcNo)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "RemoveFile",SzCat2Str(szFullPathSrc,", ",szCmo));
#endif //DEBUG
		SetupError(STFERR);
		}
}


#ifndef STF_LITE
//*************************************************************************
VOID BackupFile(LPSTR szFullPath, LPSTR szBackup)
{
#ifdef DEBUG
	INT n;

	if (FValidPath(szFullPath) == 0)
		n = 1;
	else if (FEmptySz(szBackup))
		n = 2;
	else
		n = 0;
	if (n > 0)
		BadArgErr(n, "BackupFile",SzCat2Str(szFullPath,", ",szBackup));
#endif //DEBUG

	if (YnrcBackupFile(szFullPath, szBackup, cmoNone) == ynrcNo)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "BackupFile",SzCat2Str(szFullPath,", ",szBackup));
#endif //DEBUG
		SetupError(STFERR);
		}
}


//*************************************************************************
VOID RenameFile(LPSTR szFullPath, LPSTR szBackup)
{
#ifdef DEBUG
	INT n;

	if (FValidPath(szFullPath) == 0)
		n = 1;
	else if (FEmptySz(szBackup))
		n = 2;
	else
		n = 0;
	if (n > 0)
		BadArgErr(n, "RenameFile",SzCat2Str(szFullPath,", ",szBackup));
#endif //DEBUG

	if (YnrcBackupFile(szFullPath, szBackup, cmoNone) == ynrcNo)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "RenameFile",SzCat2Str(szFullPath,", ",szBackup));
#endif //DEBUG
		SetupError(STFERR);
		}
}
#endif  /* !STF_LITE */


//*************************************************************************
VOID AddSectionFilesToCopyList(LPSTR szSect, LPSTR szSrc,
		LPSTR szDest)
{
#ifdef DEBUG
	INT   n;
	LPSTR szTmp;

	if (FValidInfSect(szSect) == 0)
		n = 1;
	else if (FValidDir(szSrc) == 0)
		n = 2;
	else if (FValidDir(szDest) == 0)
		n = 3;
	else
		n = 0;
	if (n > 0)
		{
		szTmp = SzCat3Str(szSect, ", ", szSrc, ", ");
		BadArgErr(n, "AddSectionFilesToCopyList",SzCatStr(szTmp,szDest));
		}
#endif //DEBUG

	if (FAddSectionFilesToCopyList(szSect, szSrc, szDest) == 0)
		{
#ifdef DEBUG
		szTmp = SzCat3Str(szSect, ", ", szSrc, ", ");
		StfApiErr(saeFail, "AddSectionFilesToCopyList",SzCatStr(szTmp, szDest));
#endif //DEBUG
		SetupError(STFERR);
		}
}


//*************************************************************************
VOID AddSectionKeyFileToCopyList(LPSTR szSect, LPSTR szKey,
		LPSTR szSrc, LPSTR szDest)
{
#ifdef DEBUG
	INT   n;
	LPSTR szTmp;

	if (FValidInfSect(szSect) == 0)
		n = 1;
	else if (FEmptySz(szKey))
		n = 2;
	else if (FValidDir(szSrc) == 0)
		n = 3;
	else if (FValidDir(szDest) == 0)
		n = 4;
	else
		n = 0;
	if (n > 0)
		{
		szTmp = SzCat3Str(szSect, ", ", szKey, ", ");
		BadArgErr(n, "AddSectionKeyFileToCopyList", SzCat3Str(szTmp, szSrc,
				", ", szDest));
		}
#endif //DEBUG

	if (FAddSectionKeyFileToCopyList(szSect, szKey, szSrc, szDest) == 0)
		{
#ifdef DEBUG
		szTmp = SzCat3Str(szSect, ", ", szKey, ", ");
		StfApiErr(saeFail, "AddSectionKeyFileToCopyList", SzCat3Str(szTmp,
				szSrc, ", ", szDest));
#endif //DEBUG
		SetupError(STFERR);
		}
}


#ifndef STF_LITE
//*************************************************************************
VOID AddSpecialFileToCopyList(LPSTR szSect, LPSTR szKey, LPSTR szSrc,
				LPSTR szDest)
{
#ifdef DEBUG
	INT   n;
	LPSTR szTmp;

	if (FValidInfSect(szSect) == 0)
		n = 1;
	else if (FEmptySz(szKey))
		n = 2;
	else if (FValidDir(szSrc) == 0)
		n = 3;
	else if (FValidPath(szDest) == 0)
		n = 4;
	else
		n = 0;
	if (n > 0)
		{
		szTmp = SzCat3Str(szSect, ", ", szKey, ", ");
		BadArgErr(n, "AddSpecialFileToCopyList", SzCat3Str(szTmp, szSrc, ", ",
				szDest));
		}
#endif //DEBUG

	if (FAddSpecialFileToCopyList(szSect, szKey, szSrc, szDest) == 0)
		{
#ifdef DEBUG
		szTmp = SzCat3Str(szSect, ", ", szKey, ", ");
		StfApiErr(saeFail, "AddSpecialFileToCopyList", SzCat3Str(szTmp, szSrc,
				", ", szDest));
#endif //DEBUG
		SetupError(STFERR);
		}
}
#endif  /* !STF_LITE */


//*************************************************************************
VOID AddToBillboardList(LPSTR szDll, INT idDlg, LPSTR szProc,
		LONG lTicks)
{
#ifdef DEBUG
	INT   n;
	CHAR  szidDlg[cchNum], szlTicks[cchNum];
	LPSTR szTmp;

	if (FEmptySz(szDll))
		n = 1;
	else if (idDlg == 0)
		n = 2;
	else if (FEmptySz(szProc))
		n = 3;
	else if (lTicks <= 0)
		n = 4;
	else
		n = 0;
	if (n > 0)
		{
		wsprintf(szidDlg, "%d", idDlg);
		wsprintf(szlTicks, "%d", lTicks);
		szTmp = SzCat3Str(szDll,", ", szidDlg,", ");
		BadArgErr(n,"AddToBillboardList",SzCat3Str(szTmp,szProc,", ",szlTicks));
		}
#endif //DEBUG

	if (FAddToBillboardList(szDll, idDlg, szProc, lTicks) == 0)
		{
#ifdef DEBUG
		wsprintf(szidDlg, "%d", idDlg);
		wsprintf(szlTicks, "%d", lTicks);
		szTmp = SzCat3Str(szDll,", ", szidDlg,", ");
		StfApiErr(saeFail, "AddToBillboardList", SzCat3Str(szTmp, szProc, ", ",
				szlTicks));
#endif //DEBUG
		SetupError(STFERR);
		}
}


#ifndef STF_LITE
//*************************************************************************
VOID AddBlankToBillboardList(LONG lTicks)
{
#ifdef DEBUG
	CHAR szlTicks[cchNum];

	if (lTicks <= 0)
		{
		wsprintf(szlTicks, "%d", lTicks);
		BadArgErr(1, "AddBlankToBillboardList", szlTicks);
		}
#endif //DEBUG

	if (FAddToBillboardList(NULL, 0, NULL, lTicks) == 0)
		{
#ifdef DEBUG
		wsprintf(szlTicks, "%d", lTicks);
		StfApiErr(saeFail, "AddBlankToBillboardList",szlTicks);
#endif //DEBUG
		SetupError(STFERR);
		}
}


//*************************************************************************
VOID ClearBillboardList(VOID)
{
	if (FClearBillboardList() == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "ClearBillboardList", "");
#endif //DEBUG
		SetupError(STFERR);
		}
}
#endif  /* !STF_LITE */


#ifndef STF_LITE
//*************************************************************************
VOID OpenLogFile(LPSTR szFile, INT fAppend)
{
#ifdef DEBUG
	CHAR szAppend[cchNum];

	if (FValidPath(szFile) == 0)
		{
		wsprintf(szAppend, "%d", fAppend);
		BadArgErr(1, "OpenLogFile",SzCat2Str(szFile,", ",szAppend));
		}
#endif //DEBUG

	if (FOpenLogFile(szFile, fAppend) == 0)
		{
#ifdef DEBUG
		wsprintf(szAppend, "%d", fAppend);
		StfApiErr(saeFail, "OpenLogFile",SzCat2Str(szFile,", ",szAppend));
#endif //DEBUG
		SetupError(STFERR);
		}
}


//*************************************************************************
VOID CloseLogFile(VOID)
{
	if (FCloseLogFile() == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "CloseLogFile", "");
#endif //DEBUG
		SetupError(STFERR);
		}
}


//*************************************************************************
VOID WriteToLogFile(LPSTR szStr)
{
	if (FWriteToLogFile(szStr, 1) == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "WriteToLogFile", szStr);
#endif //DEBUG
		SetupError(STFERR);
		}
}
#endif  /* !STF_LITE */


//'' -1 in either parameter will mean 'center in frame client area'
//*************************************************************************
VOID SetCopyGaugePosition(INT x, INT y)
{
	ProSetPos(x, y);
}


#ifndef STF_LITE
//*************************************************************************
LPSTR FindFileUsingFileOpen(LPSTR szFile, LPSTR szBfr, INT cbBfrMax)
{
	INT	wRet;

	if (szBfr)
		*szBfr = ' ';		//REVIEW: KLUDGE: See POOF Bug #635
	wRet = WFindFileUsingFileOpen(szFile, szBfr, cbBfrMax);

	if (wRet == 1)
		{
		szBfr[0] = '\0';
		return(szBfr);
		}
	if (wRet == 0)
		return(szBfr);
	else
		{
		szBfr[0] = '\0';
#ifdef DEBUG
		StfApiErr(saeFail, "FindFileUsingFileOpen", szFile);
#endif //DEBUG
		SetupError(STFERR);
		}
}
#endif  /* !STF_LITE */


//*************************************************************************
INT IsDirWritable(LPSTR szDir)
{
	return(FIsDirWritable(szDir));
}


//*************************************************************************
INT IsFileWritable(LPSTR szFile)
{
#ifdef DEBUG
	if (FValidDir(szFile) == 0)
		BadArgErr(1, "IsFileWritable", szFile);
#endif //DEBUG

	return(FIsFileWritable(szFile));
}


//*************************************************************************
INT GetWindowsMajorVersion(VOID)
{
	DWORD dw = GetVersion() & 0x7fffffff;

	return((INT)(LOBYTE(LOWORD(dw))));
}


//*************************************************************************
INT GetWindowsMinorVersion(VOID)
{
	DWORD dw = GetVersion() & 0x7fffffff;

	return((INT)(HIBYTE(LOWORD(dw))));
}


#ifndef STF_LITE
//*************************************************************************
LPSTR GetNthFieldFromIniString(LPSTR szLine, INT iField, LPSTR szBfr,
		INT cbBfrMax)
{
	LPSTR szEnd;

	if (iField < 1)
		{
#ifdef DEBUG
		CHAR szField[cchNum];

		wsprintf(szField, "%d", iField);
		StfApiErr(saeFail, "GetNthFieldFromIniString", SzCat2Str(szLine, ", ",
				szField));
#endif //DEBUG
		SetupError(STFERR);
		}

	while (--iField > 0)
		{
		while (*szLine != '\0' && *szLine != ',')
			szLine = AnsiNext(szLine);
		szLine = AnsiNext(szLine);
		}

	while (*szLine == ' ' || *szLine == '\t')
		szLine = AnsiNext(szLine);

	szEnd = szLine;
	while (*szEnd != '\0' && *szEnd != ',')
		szEnd = AnsiNext(szEnd);

	if (cbBfrMax < szEnd - szLine)
		{
		DoMsgBox("Buffer Overflow","MS-Setup Error",MB_ICONHAND+MB_OK);
		SetupError(STFERR);
		}

	if (*szEnd != '\0')
		{
		Assert(*szEnd == ',');
		*szEnd = '\0';
		lstrcpy(szBfr, szLine);
		*szEnd = ',';
		}
	else
		lstrcpy(szBfr, szLine);

	return(szBfr);
}


//*************************************************************************
INT GetWindowsMode(VOID)
{
#if defined(WIN16)
	LONG longTmp = GetWinFlags();

	if (longTmp & WF_WINNT)
		return(3);
	else if (longTmp & WF_STANDARD)
		return(1);
	else if (longTmp & WF_ENHANCED)
		return(2);
	else
		return(0);
#elif defined(WIN32)
	return(2);
#endif
}
#endif  /* !STF_LITE */


//*************************************************************************
LPSTR GetWindowsDir(LPSTR szBuf, INT cbBufMax)
{
	CHAR szBufT[255];
	INT  cbBuf = GetWindowsDirectory(szBufT, 255);
	INT  dch;

	if (cbBuf == 0)
		{
		*szBuf = '\0';
#ifdef DEBUG
		StfApiErr(saeFail, "GetWindowsDir", "");
#endif //DEBUG
		SetupError(STFERR);
		}
	else
		{
		if (cbBuf > 255)
			{
			DoMsgBox("Buffer Overflow", "MS-Setup Error", MB_ICONHAND+MB_OK);
			SetupError(STFERR);
			}
		if (szBufT[0] == '\\')
			{
			if (cbBufMax < 2)
				{
				DoMsgBox("Buffer Overflow","MS-Setup Error",MB_ICONHAND+MB_OK);
				SetupError(STFERR);
				}
			lstrcpy(szBuf, szCurDir);
			dch = 2;
			}
		else if (szBufT[1] != ':')
			{
			if (cbBufMax < 3)
				{
				DoMsgBox("Buffer Overflow","MS-Setup Error",MB_ICONHAND+MB_OK);
				SetupError(STFERR);
				}
			lstrcpy(szBuf, szCurDir);
			dch = 3;
			}
		else
			dch = 0;

		if (cbBufMax - dch < (INT)lstrlen(szBufT))
			{
			DoMsgBox("Buffer Overflow","MS-Setup Error",MB_ICONHAND+MB_OK);
			SetupError(STFERR);
			}
		lstrcpy(szBuf + dch, szBufT);

		if (*AnsiPrev(szBufT, szBufT + lstrlen(szBufT)) != '\\')
			{
			if (cbBufMax < (INT)lstrlen(szBuf) + 1)
				{
				DoMsgBox("Buffer Overflow","MS-Setup Error",MB_ICONHAND+MB_OK);
				SetupError(STFERR);
				}
			lstrcat(szBuf, "\\");
			}

		return(szBuf);
		}
}


//*************************************************************************
LPSTR GetWindowsSysDir(LPSTR szBuf, INT cbBufMax)
{
	CHAR szBufT[255];
	INT  cbBuf = GetSystemDirectory(szBufT, 255);
	INT  dch;

	if (cbBuf == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "GetWindowsSysDir", "");
#endif //DEBUG
		SetupError(STFERR);
		}
	else
		{
		if (cbBuf > 255)
			{
			DoMsgBox("Buffer Overflow", "MS-Setup Error", MB_ICONHAND+MB_OK);
			SetupError(STFERR);
			}
		if (szBufT[0] == '\\')
			{
			if (cbBufMax < 2)
				{
				DoMsgBox("Buffer Overflow","MS-Setup Error",MB_ICONHAND+MB_OK);
				SetupError(STFERR);
				}
			lstrcpy(szBuf, szCurDir);
			dch = 2;
			}
		else if (szBufT[1] != ':')
			{
			if (cbBufMax < 3)
				{
				DoMsgBox("Buffer Overflow","MS-Setup Error",MB_ICONHAND+MB_OK);
				SetupError(STFERR);
				}
			lstrcpy(szBuf, szCurDir);
			dch = 3;
			}
		else
			dch = 0;

		if (cbBufMax - dch < (INT)lstrlen(szBufT))
			{
			DoMsgBox("Buffer Overflow","MS-Setup Error",MB_ICONHAND+MB_OK);
			SetupError(STFERR);
			}
		lstrcpy(szBuf + dch, szBufT);

		if (*AnsiPrev(szBufT, szBufT + lstrlen(szBufT)) != '\\')
			{
			if (cbBufMax < (INT)lstrlen(szBuf) + 1)
				{
				DoMsgBox("Buffer Overflow","MS-Setup Error",MB_ICONHAND+MB_OK);
				SetupError(STFERR);
				}
			lstrcat(szBuf, "\\");
			}

		return(szBuf);
		}
}


#ifndef STF_LITE
//*************************************************************************
INT IsWindowsShared(VOID)
{
	CHAR  szWin[255];
	CHAR  szSys[255];
	LPSTR szWinT;
	LPSTR szSysT;

	GetWindowsDir(szWin, 255);
	GetWindowsSysDir(szSys, 255);

	AnsiUpper(szWin);
	AnsiUpper(szSys);

	if (lstrlen((LPSTR)szWin) == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "IsWindowsShared", "");
#endif //DEBUG
		SetupError(STFERR);
		}

	for (szWinT = szWin, szSysT = szSys; *szWinT; szWinT++, szSysT++)
		if (*szWinT != *szSysT)
			return(TRUE);

	return(FALSE);
}


//*************************************************************************
INT GetScreenWidth(VOID)
{
	return(GetSystemMetrics(SM_CXSCREEN));
}


//*************************************************************************
INT GetScreenHeight(VOID)
{
	return(GetSystemMetrics(SM_CYSCREEN));
}


//*************************************************************************
VOID SetRestartDir(LPSTR szDir)
{
#ifdef DEBUG
	if (FValidDir(szDir) == 0)
		BadArgErr(1, "SetRestartDir", szDir);
#endif //DEBUG

	if (FSetRestartDir(szDir) == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "SetRestartDir", szDir);
#endif //DEBUG
		SetupError(STFERR);
		}
}


//*************************************************************************
INT RestartListEmpty(VOID)
{
	return(FRestartListEmpty());
}


//*************************************************************************
INT ExitExecRestart(VOID)
{
	return(FExitExecRestart());
}
#endif  /* !STF_LITE */


#ifndef STF_LITE
//*************************************************************************
VOID PrependToPath(LPSTR szSrc, LPSTR szDst, LPSTR szDir, INT cmo)
{
#ifdef DEBUG
	INT   n;
	CHAR  szCmo[cchNum];
	LPSTR szTmp;

	if ((FValidPath(szSrc) == 0) && (FValidSz(szSrc)))
		n = 1;
	else if (FValidPath(szDst) == 0)
		n = 2;
	else if (FValidDir(szDir) == 0)
		n = 3;
	else
		n = 0;

	if (n > 0)
		{
		wsprintf(szCmo, "%d", cmo);
		szTmp = SzCat3Str(szSrc, ", ", szDst, ", ");
		BadArgErr(n, "PrependToPath",SzCat3Str(szTmp,szDir,", ",szCmo));
		}
#endif //DEBUG

	if (FPrependToPath(szSrc, szDst, szDir, cmo) == 0)
		{
#ifdef DEBUG
		wsprintf(szCmo, "%d", cmo);
		szTmp = SzCat3Str(szSrc, ", ", szDst, ", ");
		StfApiErr(saeFail, "PrependToPath",SzCat3Str(szTmp,szDir,", ",szCmo));
#endif //DEBUG
		SetupError(STFERR);
		}
}
#endif  /* !STF_LITE */



//**************************************************************************
//***************************  Error Handlers  *****************************
//**************************************************************************

#ifdef DEBUG

//**************************************************************************
VOID StfApiErr(INT nMsg, LPSTR szApi, LPSTR szArgs)
{
	CHAR rgchCapion[] = "MS-Setup Toolkit API Error";
	CHAR rgchText[cchMax * 2];

	if (szApi == NULL)
		*szApi = '\0';
	if (szArgs == NULL)
		*szArgs = '\0';

	switch(nMsg)
		{
	case saeFail:
		lstrcpy(rgchText, "Failed");
		break;
	case saeInit:
		lstrcpy(rgchText, "Already Initialized");
		break;
	case saeNYI:
		lstrcpy(rgchText, "NYI");
		break;
	default:
		lstrcpy(rgchText, "Bad Arg ");
		/* for C7 use _itoa() */
		itoa(nMsg - saeArg, (CHAR *)(rgchText+lstrlen(rgchText)), 10);
		break;
		}

	lstrcat(rgchText, ": ");
	lstrcat(rgchText, szApi);
	if (*szArgs != '\0')
		{
		lstrcat(rgchText, " (");
		lstrcat(rgchText, szArgs);
		lstrcat(rgchText, ")");
		}
#ifndef STF_LITE
	Assert(lstrlen(rgchText) < cchMax * 2);
#endif  /* !STF_LITE */
	DoMsgBox(rgchText, rgchCapion, MB_TASKMODAL+MB_ICONHAND+MB_OK);
}


//**************************************************************************
VOID BadArgErr(INT nArg, LPSTR szApi, LPSTR szArgs)
{
	StfApiErr(nArg+saeArg, szApi, szArgs);
	SetupError(STFERR);
}


//**************************************************************************
INT FValidInfSect(LPSTR szSect)
{
	if (szSect == NULL || *szSect == '\0')
		return(0);

	while (*szSect)
		{
		if (*szSect == ']')
			return(0);
		szSect = AnsiNext(szSect);
		}

	return(1);
}


//**************************************************************************
INT FValidIniFile(LPSTR szFile)
{
	if (!FValidPath(szFile) && lstrcmpi(szFile, "WIN.INI") != 0)
		return(0);

	return(1);
}


//**************************************************************************
INT FValidDrive(LPSTR szDrive)
{
	if (szDrive == NULL || *szDrive == '\0')
		return(0);
	if (szDrive[0] == '\\' && szDrive[1] == '\\')	/* UNC path */
		return(1);
	if ((*szDrive < 'a' || *szDrive > 'z')
		&& (*szDrive < 'A' || *szDrive > 'Z'))
		return(0);
	if (lstrlen(szDrive) == 1)
		return(1);
	if (szDrive[1] == ':')
		return(1);

	return(0);
}

#endif //DEBUG


LPSTR SzCatStr(LPSTR sz1, LPSTR sz2)
{
	static CHAR szT[cchMax * 2];

#ifndef STF_LITE
	Assert(lstrlen(sz1)+lstrlen(sz2) < cchMax * 2);
#endif  /* !STF_LITE */
	lstrcpy(szT, sz1);
	lstrcat(szT, sz2);

	return(szT);
}


LPSTR SzCat2Str(LPSTR sz1, LPSTR sz2, LPSTR sz3)
{
	static CHAR szT[cchMax * 3];

#ifndef STF_LITE
	Assert(lstrlen(sz1)+lstrlen(sz2)+lstrlen(sz3) < cchMax * 3);
#endif  /* !STF_LITE */
	lstrcpy(szT, sz1);
	lstrcat(szT, sz2);
	lstrcat(szT, sz3);

	return(szT);
}


LPSTR SzCat3Str(LPSTR sz1, LPSTR sz2, LPSTR sz3, LPSTR sz4)
{
	static CHAR szT[cchMax * 4];

#ifndef STF_LITE
	Assert(lstrlen(sz1)+lstrlen(sz2)+lstrlen(sz3)+lstrlen(sz4) < cchMax * 4);
#endif  /* !STF_LITE */
	lstrcpy(szT, sz1);
	lstrcat(szT, sz2);
	lstrcat(szT, sz3);
	lstrcat(szT, sz4);

	return(szT);
}


VOID EndSetupToolkit()
{
	TerminateFrame();
	TerminateInstall();
#ifndef STF_LITE
	TerminateRegDb();
#endif
}


// **************************************************************************
void RightTrim(LPSTR sz)
{
	LPSTR szBlank = NULL;

	while (*sz != '\0')
		{
		if (*sz != ' ')
			szBlank = NULL;
		else if (szBlank == NULL)
			szBlank = sz;
		sz = AnsiNext(sz);
		}

	if (szBlank != NULL)
		*szBlank = '\0';
}
