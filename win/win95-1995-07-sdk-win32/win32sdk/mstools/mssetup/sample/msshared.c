#ifndef STF_LITE

#include <windows.h>
#include "setupapi.h"
#include "msdetect.h"
#include "msregdb.h"
#include "msshared.h"

VOID NewFindFileUsingFileOpen(LPSTR szFile, LPSTR szBuf, INT cbBuf);


/* REVIEW -- Language differences are not considered!!! */
INT SharedFileNeedsCopying = 0;

static char szWinIni[] = "WIN.INI";
static char szMsapps[] = "MSAPPS";



//  private to this file - needed in next api
/************************************************************************/
BOOL FixupOneMsappsEntry(LPSTR szKey, LPSTR szDef)
{
	char szBuf[256];

	if (!DoesIniKeyExist(szWinIni, szMsapps, szKey)
			|| !GetIniKeyString(szWinIni, szMsapps, szKey, szBuf, 255)
			|| !FValidDir(szBuf))
		{
		lstrcpy(szBuf, szDef);
		lstrcat(szBuf, szKey);
		return(FCreateIniKeyValue(szWinIni, szMsapps, szKey, szBuf,
				cmoOverwrite));
		}

	return(TRUE);
}


/************************************************************************/
BOOL FixupWinIniMsappsSection(VOID)
{
	static BOOL fMsappsNeedsFixing = TRUE;
	char  szBuf[256];
	LPSTR sz;

	if (!fMsappsNeedsFixing)
		return(TRUE);

	fMsappsNeedsFixing = FALSE;

	if (!DoesIniKeyExist(szWinIni, szMsapps, szMsapps)
			|| !GetIniKeyString(szWinIni, szMsapps, szMsapps, szBuf, 255)
			|| !FValidDir(szBuf))
		{
		GetWindowsDir(szBuf, 255 - lstrlen(szMsapps) - 2);
		for (sz = szBuf; *(AnsiNext(sz)) != '\0'; sz = AnsiNext(sz))
			;
		if (*sz != '\\')
			lstrcat(sz, "\\");
		lstrcat(sz, szMsapps);
		if (!FCreateIniKeyValue(szWinIni, szMsapps, szMsapps, szBuf,
				cmoOverwrite))
			return(FALSE);
		}

	if (!GetIniKeyString(szWinIni, szMsapps, szMsapps, szBuf, 255)
			|| !FValidDir(szBuf))
		return(FALSE);
	for (sz = szBuf; *(AnsiNext(sz)) != '\0'; sz = AnsiNext(sz))
		;
	if (*sz != '\\')
		lstrcat(sz, "\\");

	return(FixupOneMsappsEntry("CLIPART",  szBuf)
			&& FixupOneMsappsEntry("EQUATION", szBuf)
			&& FixupOneMsappsEntry("GRPHFLT",  szBuf)
			&& FixupOneMsappsEntry("MSDRAW",   szBuf)
			&& FixupOneMsappsEntry("MSGRAPH",  szBuf)
			&& FixupOneMsappsEntry("PROOF",    szBuf)
			&& FixupOneMsappsEntry("TEXTCONV", szBuf)
			&& FixupOneMsappsEntry("WORDART",  szBuf));
}


/************************************************************************/
BOOL DoesSharedFileNeedCopying(VOID)
{
	return(SharedFileNeedsCopying != 0);
}


//  returns: 1 if szVer1 > szVer2;  0 if equal;  -1 if szVer1 < szVer2
/************************************************************************/
INT WCompareVersion(LPSTR szVer1, LPSTR szVer2)
{
	INT i;

	if (*szVer1 == '\0')
		if (*szVer2 == '\0')
			return(0);
		else
			return(-1);

	if (*szVer2 == '\0')
		return(1);

	for (i = 1; i <= 4; i++)
		{
		LONG piece1 = GetVersionNthField(szVer1, i);
		LONG piece2 = GetVersionNthField(szVer2, i);

		if (piece1 > piece2)
			return(1);
		if (piece1 < piece2)
			return(-1);
		}

	return(0);
}


//  private to this file - needed in next api
//  NOTE - Win3.1 OpenFile() uses Task EXE/DLL's dir for searching!!
//     also avoids OpenFile()'s check of CWD.
/************************************************************************/
VOID NewFindFileUsingFileOpen(LPSTR szFile, LPSTR szBuf, INT cbBuf)
{
	CHAR szTmp[256];

	GetWindowsDir(szTmp, 255);
	Assert(lstrlen(szTmp) + lstrlen(szFile) < 256);
	lstrcat(szTmp, szFile);
	if (!DoesFileExist(szTmp, femExists))
		{
		GetWindowsSysDir(szTmp, 255);
		Assert(lstrlen(szTmp) + lstrlen(szFile) < 256);
		lstrcat(szTmp, szFile);
		if (!DoesFileExist(szTmp, femExists))
			FindTargetOnEnvVar(szFile, "PATH", szTmp, 255);
		}

	Assert(lstrlen(szTmp) < cbBuf);
	lstrcpy(szBuf, szTmp);
}


//  private to this file - needed in next api
/************************************************************************/
LPSTR FindSharedFileFromPath(LPSTR szField, LPSTR szVersion,
		LPSTR szBuf, INT cbBuf)
{
	CHAR szTmp[32];

	if (szBuf == NULL || cbBuf <= 0)
		return(NULL);

	*szBuf = '\0';
	if (lstrlen(szField) < cbBuf)
		lstrcpy(szBuf, szField);
	SharedFileNeedsCopying = 0;

	if (*szBuf != '\0' && !DoesFileExist(szBuf, femExists))
		{
		if (FValidPath(szBuf)
				&& FParsePathIntoPieces(szBuf, NULL, 0, NULL, 0, szTmp, 31))
			NewFindFileUsingFileOpen(szTmp, szBuf, cbBuf);
		else
			*szBuf = '\0';
		}

	if (*szBuf != '\0')
		{
		if (WCompareVersion(GetVersionOfFile(szBuf, szTmp, 31), szVersion) < 0)
			{
			if (IsFileWritable(szBuf))
				SharedFileNeedsCopying = 1;
			else
				*szBuf = '\0';
			}
		}

	return((*szBuf == '\0') ? NULL : szBuf);
}


/************************************************************************/
LPSTR SearchForLocationForSharedFile(LPSTR szRegDbKey,
		LPSTR szWinIniSect, LPSTR szWinIniKey, INT iWinIniField,
		LPSTR szDefault, LPSTR szVersion, LPSTR szBuf, INT cbBuf)
{
	CHAR szTmp[256];

	if (szBuf == NULL || cbBuf <= 0)
		return(NULL);

#ifdef	REG_DB_ENABLED
	if (*szRegDbKey != '\0')
		{
		GetRegKeyValue(szRegDbKey, szTmp, 255);
		FindSharedFileFromPath(szTmp, szVersion, szBuf, cbBuf);
		if (*szBuf != '\0')
			return(szBuf);
		}
#endif //  REG_DB_ENABLED

	if (*szWinIniSect != '\0' && *szWinIniKey != '\0')
		{
		GetIniKeyString("WIN.INI", szWinIniSect, szWinIniKey, szTmp, 255);
		if (*szTmp != '\0')
			{
			CHAR szField[256];

			GetNthFieldFromIniString(szTmp, iWinIniField, szField, 255);
			FindSharedFileFromPath(szField, szVersion, szBuf, cbBuf);
			if (*szBuf != '\0')
				return(szBuf);
			}
		}

	*szBuf = '\0';
	if (lstrlen(szDefault) < cbBuf)
		lstrcpy(szBuf, szDefault);
	SharedFileNeedsCopying = 1;

	if (DoesFileExist(szBuf, femExists)
			&& WCompareVersion(GetVersionOfFile(szBuf, szTmp, 255), szVersion)
					>= 0)
		SharedFileNeedsCopying = 0;

	return((*szBuf == '\0') ? NULL : szBuf);
}


/*
**	szInfSection: INF Section which contains shared app description line.
**	szInfKey:     INF reference key which is for the shared app description
**					line.
**	szSubDir:     Shared App WIN.INI key (such as PROOF, TEXTCONV, MSDRAW).
**	szRegDbKey:   Registration DB key which might contain full path of an
**					existing copy of the Shared App.
**	szWinIniSect: WIN.INI section which might contain full path of an
**					existing copy of the Shared App (such as 'MS Proofing
**					Tools').
**	szWinIniKey:  WIN.INI key which might reference the full path of an
**					existing copy of the Shared App (such as 'Spelling 1033,0').
**	iWinIniField: index of path in above WIN.INI value (such as '1' for speller,
**					'2' for dictionary).
**	szBuf:        buffer to return full path of Shared App.
**	cbBuf:        size of szBuf.
**
*************************************************************************/
LPSTR HandleSharedFile(LPSTR szInfSection, LPSTR szInfKey,
		LPSTR szSubDir, LPSTR szRegDbKey, LPSTR szWinIniSect,
		LPSTR szWinIniKey, INT iWinIniField, LPSTR szBuf, INT cbBuf)
{
	CHAR  szDefault[256];
	CHAR  szVersion[32];
	LPSTR sz;

	FixupWinIniMsappsSection();

	if (!DoesIniKeyExist(szWinIni, szMsapps, szSubDir)
			|| !GetIniKeyString(szWinIni, szMsapps, szSubDir, szDefault, 255))
		{
		if (!DoesIniKeyExist(szWinIni, szMsapps, szMsapps)
				|| !GetIniKeyString(szWinIni, szMsapps, szMsapps, szDefault,
						255))
			return(FALSE);

		for (sz = szDefault; *(AnsiNext(sz)) != '\0'; sz = AnsiNext(sz))
			;
		if (*sz != '\\')
			lstrcat(sz, "\\");

		lstrcat(sz, szSubDir);

		FCreateIniKeyValue(szWinIni, szMsapps, szSubDir, szDefault,
				cmoOverwrite);
		}
	
	for (sz = szDefault; *(AnsiNext(sz)) != '\0'; sz = AnsiNext(sz))
		;
	if (*sz != '\\')
		lstrcat(sz, "\\");

	while (*sz != '\0')
		sz = AnsiNext(sz);

	/* Get file name from INF */
	if (CbGetInfSectionKeyField(szInfSection, szInfKey, 1, sz,
			255 - lstrlen(szDefault)) == -1)
		return(FALSE);

	/* Get file version from INF */
	if (CbGetInfSectionKeyField(szInfSection, szInfKey, 19, szVersion, 31)
			== -1)
		return(FALSE);

	return(SearchForLocationForSharedFile(szRegDbKey, szWinIniSect,
			szWinIniKey, iWinIniField, szDefault, szVersion, szBuf, cbBuf));
}
#endif  /* !STF_LITE */
