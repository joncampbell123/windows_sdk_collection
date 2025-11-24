#include <windows.h>
#include "setupapi.h"
#include "msdetect.h"
#include <string.h>
#include <stdlib.h>


#define cchNum 10


// *************************************************************************
LPSTR lstrncpy(LPSTR szDst, LPSTR szSrc, INT n)
{
	LPSTR szSav = szDst;

	while (n-- && (*szDst++ = *szSrc++) != '\0')
		;

	while (n--)
		*szDst++ = '\0';

	return(szSav);
}


// *************************************************************************
INT IsDriveValid(LPSTR szDrive)
{
#ifdef DEBUG
	if (FValidDrive(szDrive) == 0)
		BadArgErr(1, "IsDriveValid", szDrive);
#endif // DEBUG

	AnsiUpperBuff(szDrive, 1);

	return(FIsValidDrive(*szDrive - 'A' + 1));
}


// *************************************************************************
INT IsDriveLocalHard(LPSTR szDrive)
{
#ifdef DEBUG
	if (FValidDrive(szDrive) == 0)
		BadArgErr(1, "IsDriveLocalHard", szDrive);
#endif // DEBUG

	return(FIsLocalHardDrive((*AnsiUpper(szDrive)) - 'A' + 1));
}


// *************************************************************************
INT IsDriveRemovable(LPSTR szDrive)
{
#ifdef DEBUG
	if (FValidDrive(szDrive) == 0)
		BadArgErr(1, "IsDriveRemovable", szDrive);
#endif // DEBUG

	AnsiUpperBuff(szDrive, 1);

	return(FIsRemoveDrive(*szDrive - 'A' + 1));
}


// *************************************************************************
INT IsDriveNetwork(LPSTR szDrive)
{
	CHAR ch;

#ifdef DEBUG
	if (FValidDrive(szDrive) == 0)
		BadArgErr(1, "IsDriveNetwork", szDrive);
#endif // DEBUG

	ch = *szDrive;
	AnsiUpperBuff(&ch, 1);

	return(FIsRemoteDrive(ch - 'A' + 1));
}


// *************************************************************************
LONG GetTotalSpaceForDrive(LPSTR szDrive)
{
	LONG totalspc;
#ifdef DEBUG
	if (FValidDrive(szDrive) == 0)
		BadArgErr(1, "GetTotalSpaceForDrive", szDrive);
#endif // DEBUG

	AnsiUpperBuff(szDrive, 1);
	totalspc=LcbTotalDrive(*szDrive - 'A' + 1);
	return(totalspc);
}


// *************************************************************************
LONG GetFreeSpaceForDrive(LPSTR szDrive)
{
	CHAR ch;
	LONG freespc;

#ifdef DEBUG
	if (FValidDrive(szDrive) == 0)
		BadArgErr(1, "GetFreeSpaceForDrive", szDrive);
#endif // DEBUG

	ch = *szDrive;
	AnsiUpperBuff(&ch, 1);

	freespc = LcbFreeDrive(ch - 'A' + 1);
	return(freespc);
}


// *************************************************************************
void GetValidDrivesList(LPSTR szSymbol)
{
#ifdef DEBUG
	if (FEmptySz(szSymbol))
		BadArgErr(1, "GetValidDrivesList", szSymbol);
#endif // DEBUG

	if (FGetValidDrivesList(szSymbol) == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "GetValidDrivesList", szSymbol);
#endif // DEBUG
		SetupError(STFERR);
		}
}


// *************************************************************************
void GetLocalHardDrivesList(LPSTR szSymbol)
{
#ifdef DEBUG
	if (FEmptySz(szSymbol))
		BadArgErr(1, "GetLocalHardDrivesList", szSymbol);
#endif // DEBUG

	if (FGetLocalHardDrivesList(szSymbol) == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "GetLocalHardDrivesList", szSymbol);
#endif // DEBUG
		SetupError(STFERR);
		}
}


#ifndef STF_LITE
// *************************************************************************
void GetRemovableDrivesList(LPSTR szSymbol)
{
#ifdef DEBUG
	if (FEmptySz(szSymbol))
		BadArgErr(1, "GetRemovableDrivesList", szSymbol);
#endif // DEBUG

	if (FGetRemovableDrivesList(szSymbol) == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "GetRemovableDrivesList", szSymbol);
#endif // DEBUG
		SetupError(STFERR);
		}
}

#endif  /* !STF_LITE */

// *************************************************************************
void GetNetworkDrivesList(LPSTR szSymbol)
{
#ifdef DEBUG
	if (FEmptySz(szSymbol))
		BadArgErr(1, "GetNetworkDrivesList", szSymbol);
#endif // DEBUG

	if (FGetNetworkDrivesList(szSymbol) == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "GetNetworkDrivesList", szSymbol);
#endif // DEBUG
		SetupError(STFERR);
		}
}


#ifndef STF_LITE 
// *************************************************************************
INT GetDOSMajorVersion(void)
{
#ifndef WIN32
	return(WGetDOSMajorVersion());
#else
	return(0);
#endif
}


// *************************************************************************
INT GetDOSMinorVersion(void)
{
#ifndef WIN32
	return(WGetDOSMinorVersion());
#else
	return(0);
#endif
}


// *************************************************************************
LPSTR GetEnvVariableValue(LPSTR szEnvVar, LPSTR szBuf, INT cbBuf)
{
	INT cbRet;

	if ((szBuf != NULL) && (cbBuf > 0))
		*szBuf = '\0';

#ifdef DEBUG
	if (FEmptySz(szEnvVar))
		BadArgErr(1, "GetEnvVariableValue", szEnvVar);
#endif // DEBUG

	cbRet = CbGetEnvVariableValue(szEnvVar, szBuf, cbBuf);

	if (cbRet >= cbBuf)
		{
#ifdef DEBUG
		StfApiErr(saeOvfl, "GetEnvVariableValue", szEnvVar);
#endif // DEBUG
		SetupError(STFERR);
		}

	return(szBuf);
}


// *************************************************************************
INT GetNumWinApps(void)
{
#ifndef WIN32
	return(WGetNumWinApps());
#else
	return(0);
#endif
}
#endif  /* !STF_LITE */


// *************************************************************************
INT DoesFileExist(LPSTR szFileName, INT mode)
{
	if (FValidPath(szFileName) == 0)
		return(0);

	return(FDoesFileExist(szFileName, mode));
}


// *************************************************************************
LPSTR GetDateOfFile(LPSTR szFile, LPSTR szBuf, INT cbBuf)
{
	INT cbRet;

#ifdef DEBUG
	if (FValidPath(szFile) == 0)
		BadArgErr(1, "GetDateOfFile", szFile);
#endif // DEBUG

	cbRet = CbGetDateOfFile(szFile, szBuf, cbBuf);

	if (cbRet >= cbBuf)
		{
#ifdef DEBUG
		StfApiErr(saeOvfl, "GetDateOfFile", szFile);
#endif // DEBUG
		SetupError(STFERR);
		}

	return(szBuf);
}


// *************************************************************************
INT GetYearFromDate(LPSTR szDate)
{
	CHAR szYear[5];

#ifdef DEBUG
	if (FEmptySz(szDate))
		BadArgErr(1, "GetYearFromDate", szDate);
#endif // DEBUG

	lstrncpy(szYear, szDate, 4);

	return(atoi(szYear));
}


// *************************************************************************
INT GetMonthFromDate(LPSTR szDate)
{
	CHAR szMonth[3];

#ifdef DEBUG
	if (FEmptySz(szDate))
		BadArgErr(1, "GetMonthFromDate", szDate);
#endif // DEBUG

	lstrncpy(szMonth, szDate + 5, 2);

	return(atoi(szMonth));
}


// *************************************************************************
INT GetDayFromDate(LPSTR szDate)
{
	CHAR szDay[3];

#ifdef DEBUG
	if (FEmptySz(szDate))
		BadArgErr(1, "GetDayFromDate", szDate);
#endif // DEBUG

	lstrncpy(szDay, szDate + 8, 2);

	return(atoi(szDay));
}

#ifndef STF_LITE

// *************************************************************************
INT GetHourFromDate(LPSTR szDate)
{
	CHAR szHour[2];

#ifdef DEBUG
	if (FEmptySz(szDate))
		BadArgErr(1, "GetHourFromDate", szDate);
#endif // DEBUG

	lstrncpy(szHour, szDate + 11, 2);

	return(atoi(szHour));
}


// *************************************************************************
INT GetMinuteFromDate(LPSTR szDate)
{
	CHAR szMinute[3];

#ifdef DEBUG
	if (FEmptySz(szDate))
		BadArgErr(1, "GetMinuteFromDate", szDate);
#endif // DEBUG

	lstrncpy(szMinute, szDate + 14, 2);

	return(atoi(szMinute));
}


// *************************************************************************
INT GetSecondFromDate(LPSTR szDate)
{
	CHAR szSecond[3];

#ifdef DEBUG
	if (FEmptySz(szDate))
		BadArgErr(1, "GetSecondFromDate", szDate);
#endif // DEBUG

	lstrncpy(szSecond, szDate + 17, 2);

	return(atoi(szSecond));
}


// *************************************************************************
LPSTR GetVersionOfFile(LPSTR szFile, LPSTR szBuf, INT cbBuf)
{
	INT cbRet;

	if (szBuf != NULL && cbBuf > 0)
		*szBuf = '\0';

#ifdef DEBUG
	if (FValidPath(szFile) == 0)
		BadArgErr(1, "GetVersionOfFile", szFile);
#endif // DEBUG

	cbRet = CbGetVersionOfFile(szFile, szBuf, cbBuf);

	if (cbRet >= cbBuf)
		{
#ifdef DEBUG
		StfApiErr(saeOvfl, "GetVersionOfFile", szFile);
#endif // DEBUG
		SetupError(STFERR);
		}

	return(szBuf);
}


// *************************************************************************
LONG GetVersionNthField(LPSTR szVersion, INT nField)
{
#ifdef DEBUG
	CHAR szField[cchNum];

	if ((nField < 1) || (nField > 4))
		{
		wsprintf(szField, "%d", nField);
		BadArgErr(2, "GetVersionNthField",SzCat2Str(szVersion,", ",szField));
		}
#endif // DEBUG

	return(LGetVersionNthField(szVersion, nField));
}
#endif  /* !STF_LITE */


// *************************************************************************
LONG GetSizeOfFile(LPSTR szFile)
{
#ifdef DEBUG
	if (FValidPath(szFile) == 0)
		BadArgErr(1, "GetSizeOfFile", szFile);
#endif // DEBUG

	return(LcbGetSizeOfFile(szFile));
}


#ifndef STF_LITE
// *************************************************************************
LPSTR FindTargetOnEnvVar(LPSTR szFile, LPSTR szEnvVar, LPSTR szBuf,
		INT cbBuf)
{
	INT cbRet;
#ifdef DEBUG
	INT n;
#endif /* DEBUG */

	if (szBuf != NULL && cbBuf > 0)
		*szBuf = '\0';

#ifdef DEBUG
	if (CchlValidSubPath(szFile) == 0)
		n = 1;
	else if (FEmptySz(szEnvVar))
		n = 2;
	else
		n = 0;
	if (n > 0)
		BadArgErr(n, "FindTargetOnEnvVar",SzCat2Str(szFile,", ",szEnvVar));
#endif // DEBUG

	cbRet = CbFindTargetOnEnvVar(szFile, szEnvVar, szBuf, cbBuf);

	if (cbRet >= cbBuf)
		{
#ifdef DEBUG
		StfApiErr(saeOvfl,"FindTargetOnEnvVar",SzCat2Str(szFile,", ",szEnvVar));
#endif // DEBUG
		SetupError(STFERR);
		}

	return(szBuf);
}


// *************************************************************************
LPSTR FindFileInTree(LPSTR szFile, LPSTR szDir, LPSTR szBuf,
		INT cbBuf)
{
	INT cbRet;
#ifdef DEBUG
	INT n;

	if (CchlValidSubPath(szFile) == 0)
		n = 1;
	else if (FValidDir(szDir) == 0)
		n = 2;
	else
		n = 0;
	if (n > 0)
		BadArgErr(n, "FindFileInTree",SzCat2Str(szFile,", ",szDir));
#endif // DEBUG

	cbRet = CbFindFileInTree(szFile, szDir, szBuf, cbBuf);

	if (cbRet >= cbBuf)
		{
#ifdef DEBUG
		StfApiErr(saeOvfl, "FindFileInTree",SzCat2Str(szFile,", ",szDir));
#endif // DEBUG
		SetupError(STFERR);
		}

	return(szBuf);
}


#ifdef WIN16
// *************************************************************************
INT GetConfigSmartdrvSize(void)
{
	return(WGetConfigSmartdrvSize());
}


// *************************************************************************
INT GetConfigRamdriveSize(void)
{
	return(WGetConfigRamdriveSize());
}


// *************************************************************************
INT GetConfigNumBuffers(void)
{
	return(WGetConfigNumBuffers());
}


// *************************************************************************
INT GetConfigNumFiles(void)
{
	return(WGetConfigNumFiles());
}


// *************************************************************************
LPSTR GetConfigLastDrive(LPSTR szBuf, INT cbBuf)
{
	INT chRet = WGetConfigLastDrive();

	if (chRet == 0)
		lstrcpy(szBuf, "");
	else
		wsprintf(szBuf, "%c", chRet);

	return(szBuf);
}
#endif

// *************************************************************************
INT IsDriverInConfig(LPSTR szDrv)
{
#ifdef DEBUG
	if (FEmptySz(szDrv))
		BadArgErr(1, "IsDriverInConfig", szDrv);
#endif // DEBUG

	return(FIsDriverInConfig(szDrv));
}


// *************************************************************************
INT GetProcessorType(void)
{
#if defined(WIN16)
	LONG longTmp = GetWinFlags();

	if (longTmp && WF_CPUR4000)
		return(5);
	else if (longTmp && WF_CPU486)
		return(4);
	else if (longTmp && WF_CPU386)
		return(3);
	else if (longTmp && WF_CPU286)
		return(2);
	else if (longTmp && WF_CPU186)
		return(1);
	else
		return(0);
#elif defined(WIN32)
	CHAR szResult[32];
	unsigned cb=32;

	if (FGetProcessorType("", 0, szResult, cb)) {
	    if (lstrcmpi(szResult, "C400") == 0)
		return(400) ;
	    if (lstrcmpi(szResult, "AXP21064") == 0)
		return(21064) ;
	    if (lstrcmpi(szResult, "R4000") == 0)
		return(4000) ;
	    if (lstrcmpi(szResult, "R3000") == 0)
		return(3000) ;
	    if (lstrcmpi(szResult, "R2000") == 0)
		return(2000) ;
	    if (lstrcmpi(szResult, "80486") == 0)
		return(4) ;
	    if (lstrcmpi(szResult, "80386") == 0)
		return(3) ;
	    if (lstrcmpi(szResult, "80286") == 0)
		return(2) ;
	    if (lstrcmpi(szResult, "80186") == 0)
		return(1) ;
	    if (lstrcmpi(szResult, "8086") == 0)
		return(0);
	}
	else
		return(0);
#endif
}


// *************************************************************************
void GetParallelPortsList(LPSTR szSymbol)
{
#ifdef DEBUG
	if (FEmptySz(szSymbol))
		BadArgErr(1, "GetParallelPortsList", szSymbol);
#endif // DEBUG

	if (FGetParallelPortsList(szSymbol) == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "GetParallelPortsList", szSymbol);
#endif // DEBUG
		SetupError(STFERR);
		}
}


// *************************************************************************
void GetSerialPortsList(LPSTR szSymbol)
{
#ifdef DEBUG
	if (FEmptySz(szSymbol))
		BadArgErr(1, "GetSerialPortsList", szSymbol);
#endif // DEBUG

	if (FGetSerialPortsList(szSymbol) == 0)
		{
#ifdef DEBUG
		StfApiErr(saeFail, "GetSerialPortsList", szSymbol);
#endif // DEBUG
		SetupError(STFERR);
		}
}


// *************************************************************************
INT Has87MathChip(void)
{
	return(FHas87MathChip());
}

#ifdef WIN32
INT HasFPPMathChip(void)
{
	return(FHas87MathChip());
}
#endif

// *************************************************************************
INT HasMonochromeDisplay(void)
{
	return(FHasMonochromeDisplay());
}


// *************************************************************************
INT HasMouseInstalled(void)
{
	return(FHasMouseInstalled());
}
#endif  /* !STF_LITE */


// *************************************************************************
INT DoesDirExist(LPSTR szDir)
{
#ifdef DEBUG
	if (FValidDir(szDir) == 0)
		BadArgErr(1, "DoesDirExist", szDir);
#endif // DEBUG

	return(FDirExists(szDir));
}


#ifndef STF_LITE
// *************************************************************************
INT DoesIniSectionExist(LPSTR szFile, LPSTR szSect)
{
#ifdef DEBUG
	if (FValidIniFile(szFile) == 0)
		BadArgErr(1, "DoesIniSectionExist",SzCat2Str(szFile,", ",szSect));
#endif // DEBUG

	return(FDoesIniSectionExist(szFile, szSect));
}


// *************************************************************************
INT DoesIniKeyExist(LPSTR szFile, LPSTR szSect, LPSTR szKey)
{
#ifdef DEBUG
	INT n;
	LPSTR szTmp;

	if (FValidIniFile(szFile) == 0)
		n = 1;
	else if (FEmptySz(szKey))
		n = 3;
	else
		n = 0;
	if (n > 0)
		{
		szTmp = SzCat3Str(szFile, ", ", szSect, ", ");
		BadArgErr(n, "DoesIniKeyExist",SzCatStr(szTmp, szKey));
		}
#endif // DEBUG

	return(FDoesIniKeyExist(szFile, szSect, szKey));
}
#endif  /* !STF_LITE */


// *************************************************************************
LPSTR GetIniKeyString(LPSTR szFile, LPSTR szSect, LPSTR szKey,
		LPSTR szBuf, INT cbBuf)
{
	INT cbRet;
#ifdef DEBUG
	INT n;
	LPSTR szTmp;
#endif // DEBUG

	if (szBuf != NULL && cbBuf > 0)
		*szBuf = '\0';

#ifdef DEBUG
	if (FValidIniFile(szFile) == 0)
		n = 1;
	else if (FEmptySz(szKey))
		n = 3;
	else
		n = 0;
	if (n > 0)
		{
		szTmp = SzCat3Str(szFile, ", ", szSect, ", ");
		BadArgErr(n, "GetIniKeyString", SzCatStr(szTmp, szKey));
		}
#endif // DEBUG

	cbRet =   CbGetIniKeyString(szFile, szSect, szKey, szBuf, cbBuf);

	if (cbRet >= cbBuf)
		{
#ifdef DEBUG
		szTmp = SzCat3Str(szFile, ", ", szSect, ", ");
		StfApiErr(saeOvfl, "GetIniKeyString",SzCatStr(szTmp,szKey));
#endif // DEBUG
		SetupError(STFERR);
		}

	return(szBuf);
}
