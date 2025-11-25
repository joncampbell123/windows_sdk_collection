#include <windows.h>
#include "setupapi.h"
#include "mscpydis.h"

#define cchMax 255

static char szCuiDll[] = "mscuistf.dll";


// **************************************************************************
int FAR PASCAL InitSystemRead(int did, LPSTR sz, int fNet)
{
#ifdef DEBUG
	int   n;
	char  szDid[cchMax*3], szNet[cchMax];
	LPSTR szTmp;

	n = 0;
	if ((did < 1) || (did > 999))
		n = 1;
	else if (FValidSz(sz) && (InStr(1, sz, "\\") != 0))
		n = 2;

	if (n > 0)
		{
		wsprintf(szDid, "%d", did);
		wsprintf(szNet, "%d", fNet);
		szTmp = SzCat2Str(szDid, ", ", sz);
		BadArgErr(n, "InitSystemRead", SzCat2Str(szTmp, ", ", szNet));
		}
#endif // DEBUG

	return(FInitSystem1(did, sz, fNet));
}


// **************************************************************************
int FAR PASCAL InitSystemWrite(int did, LPSTR sz, int fAlways)
{
#ifdef DEBUG
	int  n;
	char szDid[cchMax * 3], szAlways[cchMax];
	LPSTR szTmp;

	n = 0;
	if ((did < 1) || (did > 999))
		n = 1;
	else if (FValidSz(sz) && (InStr(1, sz,"\\") != 0))
		n = 2;
	if (n > 0)
		{
		wsprintf(szDid, "%d", did);
		wsprintf(szAlways, "%d", fAlways);
		szTmp = SzCat2Str(szDid, ", ", sz);
		BadArgErr(n, "InitSystemWrite", SzCat2Str(szTmp, ", ", szAlways));
		}
#endif // DEBUG

	return(FInitSystem3(did, sz, fAlways));
}


// *************************************************************************
void FAR PASCAL CloseSystem(LPSTR szSect, LPSTR szKey, LPSTR szDst,
		int wResType, int wResId)
{
#ifdef DEBUG
	int   n;
	LPSTR szTmp;
	char  szResType[cchMax], szResId[cchMax];

	if (FValidInfSect(szSect) == 0)
		n = 1;
	else if (FEmptySz(szKey))
		n = 2;
#if defined(WIN16)
	else if (FValidFATDir(szDst) == 0)
#elif defined(WIN32)
	else if (FValidDir(szDst) == 0)
#endif
		n = 3;
	else
		n = 0;
	if (n > 0)
		{
		szTmp = SzCat2Str(szSect, szKey, szDst);
		wsprintf(szResType, "%d", wResType);
		wsprintf(szResId, "%d", wResId);
		BadArgErr(n, "CloseSystem", SzCat2Str(szTmp, szResType, szResId));
		}
#endif // DEBUG

	if (FCloseSystem(szSect, szKey, szDst, wResType, wResId) == 0)
		{
#ifdef DEBUG
		LPSTR szTmp;
		char  szResType[cchMax], szResId[cchMax];

		szTmp = SzCat2Str(szSect, szKey, szDst);
		wsprintf(szResType, "%d", wResType);
		wsprintf(szResId, "%d", wResId);
		StfApiErr(saeFail, "CloseSystem", SzCat2Str(szTmp, szResType, szResId));
#endif // DEBUG
		SetupError(STFERR);
		}
}


// **************************************************************************
int FAR PASCAL InitSystem(int did, LPSTR sz, int fNet)
{
	return(InitSystemRead(did, sz, fNet)
			&& InitSystemDlgs(fNet)
			&& InitSystemWrite(did, sz, 0));
}


// **************************************************************************
int FAR PASCAL InitSystemDlgs(int fNet)
{
	int   fOkay = 0;
	char  rgchName[64];
	char  rgchOrg[64];
	char  rgchType[16];
	char  rgchReturn[32];
	char  rgchTmp[64];
	LPSTR szTmp;

	GetSymbolValue("STF_CD_NAME", rgchName, 64);
	GetSymbolValue("STF_CD_ORG",  rgchOrg,  64);
	GetSymbolValue("STF_CD_TYPE", rgchType, 16);

	if (*rgchType == 'N')
		{
		if (fNet)
			{
			SetSymbolValue("EditTextIn", "");
			SetSymbolValue("EditFocus",  "");
LGetOrgDialog:
			UIStartDlg(szCuiDll, CDGETORG, "FGetNameDlgProc", 0, "",
					rgchReturn, 32);

			if (*rgchReturn == 'C')
				{
				GetSymbolValue("EditTextOut", rgchTmp, 64);
				szTmp = rgchTmp;
				while (*szTmp == ' ')
					szTmp++;
				lstrcpy(rgchOrg, szTmp);
				RightTrim(rgchOrg);

				if (*rgchOrg == '\0')
					{
					BadNameOrg(CDBADORG);
					goto LGetOrgDialog;
					}

				if (!NameOrgOkay(CDCONFIRMINFO, rgchOrg, ""))
					goto LGetOrgDialog;

				SetSymbolValue("STF_CD_ORG", rgchOrg);
				fOkay = 1;
				}
			else if (*rgchReturn == 'R')
				goto LGetOrgDialog;
			else
				SetSymbolValue("STF_CD_TYPE", "ERROR");
			UIPop(1);
			}
		else
			{
LGetNameOrgDialog:
			UIStartDlg(szCuiDll, CDGETNAMEORG, "FNameOrgDlgProc", 0, "",
					rgchReturn, 32);

			if (*rgchReturn == 'C')
				{
				GetSymbolValue("NameOut", rgchTmp, 64);
				szTmp = rgchTmp;
				while (*szTmp == ' ')
					szTmp++;
				lstrcpy(rgchName, szTmp);
				RightTrim(rgchName);

				GetSymbolValue("OrgOut", rgchTmp, 64);
				szTmp = rgchTmp;
				while (*szTmp == ' ')
					szTmp++;
				lstrcpy(rgchOrg, szTmp);
				RightTrim(rgchOrg);

				if (*rgchName == '\0')
					{
					BadNameOrg(CDBADNAME);
					goto LGetNameOrgDialog;
					}
				if (*rgchOrg == '\0')
					lstrcpy(rgchOrg, " ");
				if (!NameOrgOkay(CDCONFIRMINFO, rgchName, rgchOrg))
					goto LGetNameOrgDialog;

				SetSymbolValue("STF_CD_NAME", rgchName);
				SetSymbolValue("STF_CD_ORG",  rgchOrg);
				fOkay = 1;
				}
			else
				SetSymbolValue("STF_CD_TYPE", "ERROR");
			UIPop(1);
			}
		}
	else if ((*rgchType == 'U') || (*rgchType == 'O'))
		return(NameOrgOkay(CDALREADYUSED, rgchName, rgchOrg));
	else if (*rgchType == 'W')
		{
		SetSymbolValue("EditTextIn", "");
		SetSymbolValue("EditFocus",  "");
LGetNameDialog:
		UIStartDlg(szCuiDll, CDGETNAME, "FGetNameDlgProc", 0, "",
				rgchReturn, 32);

		if (*rgchReturn == 'C')
			{
			GetSymbolValue("EditTextOut", rgchTmp, 64);
			szTmp = rgchTmp;
			while (*szTmp == ' ')
				szTmp++;
			lstrcpy(rgchName, szTmp);
			RightTrim(rgchName);

			if (*rgchName == '\0')
				{
				BadNameOrg(CDBADNAME);
				goto LGetNameDialog;
				}

			if (!NameOrgOkay(CDCONFIRMINFO, rgchName, ""))
				goto LGetNameDialog;

			SetSymbolValue("STF_CD_NAME", rgchName);
			fOkay = 1;
			}
		else if (*rgchReturn == 'R')
			goto LGetNameDialog;
		else
			SetSymbolValue("STF_CD_TYPE", "ERROR");
		UIPop(1);
		}
	else
		{
		UIStartDlg(szCuiDll, CDBADFILE, "FInfoDlgProc", 0, "",
				rgchReturn, 32);

		if (*rgchReturn == 'C')
			fOkay = 1;
		UIPop(1);
		}

	return(fOkay);
}


// **************************************************************************
void FAR PASCAL BadNameOrg(int dlg)
{
	char rgchReturn[16];

	UIStartDlg(szCuiDll, dlg, "FInfoDlgProc", 0, "", rgchReturn, 16);
	UIPop(1);
}


// **************************************************************************
int FAR PASCAL NameOrgOkay(int dlg, LPSTR szName, LPSTR szOrg)
{
	char rgchReturn[16];

	RemoveSymbol("ConfirmTextIn");
	AddListItem("ConfirmTextIn", szName);
	AddListItem("ConfirmTextIn", szOrg);

	UIStartDlg(szCuiDll, dlg, "FConfirmDlgProc", 0, "", rgchReturn, 16);
	UIPop(1);

	if (*rgchReturn == 'C')
		return(1);

	return(0);
}
