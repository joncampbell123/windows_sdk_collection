#ifdef WIN16
typedef int  INT;
typedef char CHAR;
#define APIENTRY	FAR PASCAL
#endif

extern INT   hSetup;


#define cmoVital         1
#define cmoCopy          2
#define cmoUndo          4
#define cmoRoot          8
#define cmoDecompress    16
#define cmoTimeStamp     32
#define cmoReadOnly      64
#define cmoBackup        128
#define cmoForce         256
#define cmoRemove        512
#define cmoOverwrite     1024
#define cmoAppend        2048
#define cmoPrepend       4096
#define cmoNone          0
#define cmoAll           65535

#define femExists        0
#define femRead          1
#define femWrite         2
#define femReadWrite     3

#define ynrcNo           0
#define ynrcYes          1
#define ynrcErr1         2
#define ynrcErr2         3
#define ynrcErr3         4
#define ynrcErr4         5
#define ynrcErr5         6
#define ynrcErr6         7
#define ynrcErr7         8
#define ynrcErr8         9
#define ynrcErr9         10

#define grcOkay          0
#define grcNotOkay       1
#define grcUserQuit      48

#define cbSymValMax  8192

#define STFERR       1024
#define STFQUIT      1025

#define scmOff       0
#define scmOnIgnore  1
#define scmOnFatal   2


// common

extern INT   APIENTRY FOpenInf(LPSTR szFile, INT fCheck, INT fCheckSyms);
extern INT   APIENTRY FSetSilent(INT mode);
extern INT   APIENTRY FSetBeepingMode(INT mode);
extern INT   APIENTRY HShowWaitCursor(VOID);
extern INT   APIENTRY FRestoreCursor(INT hPrev);
extern INT   APIENTRY FOpenLogFile(LPSTR szFile, INT fAppend);
extern INT   APIENTRY FCloseLogFile(VOID);
extern INT   APIENTRY FWriteToLogFile(LPSTR sz, INT fRequire);
extern INT   APIENTRY CbGetInfSectionKeyField(LPSTR szSect, LPSTR szKey,
				INT iField, LPSTR szBuf, INT cbBuf);
extern INT   APIENTRY FMakeListInfSectionField(LPSTR szSym, LPSTR szSect,
				INT iField);
extern INT   APIENTRY DoMsgBox(LPSTR lpText, LPSTR lpCaption, INT wType);

#ifdef WIN16
extern INT   APIENTRY FValidFATDir(LPSTR szDir);
extern INT   APIENTRY CchlFATValidSubPath(LPSTR szPath);
extern INT   APIENTRY FValidFATPath(LPSTR szPath);
extern INT   APIENTRY FMakeFATPathFromDirAndSubPath(LPSTR szDir,
				LPSTR szSubPath, LPSTR szBuf, INT cchpBufMax);
extern INT   APIENTRY FParseFATPathIntoPieces(LPSTR szPath, LPSTR szVol,
				INT cbVol, LPSTR szDir, INT cbDir, LPSTR szFile, INT cbFile);
#elif defined(WIN32)
extern BOOL	APIENTRY FValidDir(LPSTR szDir);
extern unsigned	APIENTRY CchlValidSubPath(LPSTR szSubPath);
extern BOOL	APIENTRY FValidPath(LPSTR szPath);
extern BOOL	APIENTRY FMakePathFromDirAndSubPath(LPSTR szDir, LPSTR szSubPath,
					LPSTR szBuffer, unsigned cchpBufMax);
extern BOOL	APIENTRY FParsePathIntoPieces(LPSTR szPath, LPSTR szBufVol, unsigned cchpBufVolMax,  
            LPSTR szBufPath, unsigned cchpBufPathMax, LPSTR szBufFile, unsigned cchpBufFileMax);

#endif


//shell

extern INT   APIENTRY FSetBitmap(LPSTR szDll, INT Bitmap);
extern INT   APIENTRY FSetAbout(LPSTR sz1, LPSTR sz2);
extern INT   APIENTRY FDoDialogExt(HWND hwnd, LPSTR szDll, INT Dlg,
				LPSTR szDlgProc, LPSTR szHelpDll, INT HelpDlg,
				LPSTR szHelpProc);
extern INT   APIENTRY FKillNDialogs(INT n);
extern INT   APIENTRY FSetSymbolValue(LPSTR szSymbol, LPSTR szValue);
extern INT   APIENTRY FRemoveSymbol(LPSTR szSym);
extern INT   APIENTRY CbGetSymbolValue(LPSTR szSymbol, LPSTR szValue,
				INT Length);
extern INT   APIENTRY UsGetListLength(LPSTR szSymbol);
extern INT   APIENTRY CbGetListItem(LPSTR szListSymbol, INT n,
				LPSTR szListItem, INT cbMax);
extern INT   APIENTRY FAddListItem(LPSTR szListSymbol, LPSTR szListItem);
extern INT   APIENTRY FReplaceListItem(LPSTR szListSymbol, INT n,
				LPSTR szListItem);
extern INT   APIENTRY FSetSymbolToListOfInfKeys(LPSTR szSym, LPSTR szSect,
				INT fNulls);

#if defined(WIN16)
extern INT   APIENTRY InitializeFrame(LPSTR szCmdLine);
extern INT   APIENTRY HwndFrame(VOID);
extern INT   APIENTRY HinstFrame(VOID);
#elif defined(WIN32)
extern INT	APIENTRY InitializeFrame(LPSTR szCmdLine);
extern HWND	APIENTRY HwndFrame(VOID);
extern HANDLE	APIENTRY HinstFrame(VOID);
#endif
extern VOID  APIENTRY TerminateFrame(VOID);

//
//install
//
extern VOID  APIENTRY ProSetPos(INT x, INT y);
extern INT   APIENTRY FCreateDir(LPSTR szDir, INT cmo);
extern INT   APIENTRY FRemoveDir(LPSTR szDir, INT cmo);
extern INT   APIENTRY FAddSectionFilesToCopyList(LPSTR szSect, LPSTR szSrc,
				LPSTR szDest);
extern INT   APIENTRY FAddSectionKeyFileToCopyList(LPSTR szSect, LPSTR szKey,
				LPSTR szSrc, LPSTR szDest);
extern INT   APIENTRY FAddSpecialFileToCopyList(LPSTR szSect, LPSTR szKey,
				LPSTR szSrc, LPSTR szDest);
extern INT   APIENTRY GrcCopyFilesInCopyList(HANDLE hInstance);
extern INT   APIENTRY FRemoveIniSection(LPSTR szFile, LPSTR szSect, INT cmo);
extern INT   APIENTRY FCreateIniKeyValue(LPSTR szFile, LPSTR szSect,
				LPSTR szKey, LPSTR szValue, INT cmo);
extern INT   APIENTRY FRemoveIniKey(LPSTR szFile, LPSTR szSect, LPSTR szKey,
				INT cmo);
extern INT   APIENTRY FCreateSysIniKeyValue(LPSTR szFile, LPSTR szSect,
				LPSTR szKey, LPSTR szValue, INT cmo);

extern INT   APIENTRY FCreateProgManGroup(LPSTR szGroup, LPSTR szPath,
				INT cmo);
extern INT   APIENTRY FCreateProgManItem(LPSTR szGroup, LPSTR szItem,
				LPSTR szCmd, INT cmo);

#ifdef WIN32
extern INT   APIENTRY   FCreateProgManGroupEx(LPSTR szGroup, BOOL fCommon, INT cmo);
extern INT   APIENTRY   FCreateProgManItemEx(LPSTR szGroup, BOOL fCommon, LPSTR szItem,
				LPSTR szCmd, INT cmo);
#endif

extern INT   APIENTRY FStampResource(LPSTR szSect, LPSTR szKey, LPSTR szDst,
				INT wResType, INT wResId, LPSTR szData, INT cbData);
extern INT   APIENTRY FDumpCopyListToFile(LPSTR szFile);
extern VOID  APIENTRY ResetCopyList(VOID);
extern LONG  APIENTRY LcbGetCopyListCost(LPSTR szExtraList, LPSTR szCostList,
				LPSTR szNeedList);
extern INT   APIENTRY FAddDos5Help(LPSTR szProgName, LPSTR szProgHelp,
				INT cmo);
extern INT   APIENTRY FCopyOneFile(LPSTR szSrc, LPSTR szDest, INT cmo,
				INT fAppend);
extern INT   APIENTRY YnrcRemoveFile(LPSTR szFullPathSrc, INT cmo);
extern INT   APIENTRY YnrcBackupFile(LPSTR szFullPath, LPSTR szBackup,
				INT cmo);
extern INT   APIENTRY FInitializeInstall(HANDLE hinst, HWND hwndFrame);
extern INT   APIENTRY WFindFileUsingFileOpen(LPSTR szFile, LPSTR szBuf,
				INT cbBuf);
extern INT   APIENTRY FIsDirWritable(LPSTR szDir);
extern INT   APIENTRY FIsFileWritable(LPSTR szFile);
extern INT   APIENTRY FAddToBillboardList(LPSTR szDll, INT idDlg,
				LPSTR szProc, LONG lTicks);
extern INT   APIENTRY FClearBillboardList(VOID);
extern INT   APIENTRY FSetRestartDir(LPSTR szDir);
extern INT   APIENTRY FRestartListEmpty(VOID);
extern INT   APIENTRY FExitExecRestart(VOID);
extern INT   APIENTRY SetCopyMode(INT fMode);
extern INT   APIENTRY GetCopyMode(VOID);
extern INT   APIENTRY SetDecompMode(INT fMode);
extern INT   APIENTRY GetDecompMode(VOID);
extern INT   APIENTRY SetSizeCheckMode(INT scmMode);
extern INT   APIENTRY FPrependToPath(LPSTR szSrc, LPSTR szDst, LPSTR szDir,
				INT cmo);

extern INT   APIENTRY SetTimeValue(unsigned usHours, unsigned usMinutes,
				unsigned Seconds);
extern VOID  APIENTRY ForceRestartOn(VOID);
extern VOID  APIENTRY TerminateInstall(VOID);



//****************  Setup Basic Wrapper Declarations  *********************

//shell

extern INT  InitSetupToolkit(LPSTR szCmdLine);
extern VOID EndSetupToolkit(VOID);

extern VOID  SetBitmap(LPSTR szDll, INT Bitmap);
extern LPSTR UIStartDlgExt(LPSTR szDll, INT Dlg, LPSTR szDlgProc,
				LPSTR szHelpDll, INT HelpDlg, LPSTR szHelpProc, LPSTR szBfr,
				INT cbBfrMax);
extern LPSTR UIStartDlg(LPSTR szDll, INT Dlg, LPSTR szDlgProc,
				INT HelpDlg, LPSTR szHelpProc, LPSTR szBuf, INT cbBuf);
extern VOID  UIPop(INT n);
extern VOID  UIPopAll(VOID);
extern VOID  SetTitle(LPSTR sz);
extern VOID  ReadInfFile(LPSTR szFile);
extern VOID  SetSymbolValue(LPSTR szSymbol, LPSTR szValue);
extern LPSTR GetSymbolValue(LPSTR szSymbol, LPSTR szBuf, INT cbBuf);
extern INT   GetListLength(LPSTR szSymbol);
extern VOID  MakeListFromSectionKeys(LPSTR szSymbol, LPSTR szSect);
extern LPSTR GetListItem(LPSTR szSymbol, INT n, LPSTR szBuf,
				INT cbBuf);
extern VOID  AddListItem(LPSTR szSymbol, LPSTR szItem);
extern VOID  ReplaceListItem(LPSTR szSymbol, INT n, LPSTR szItem);

extern INT   InitFrame(LPSTR szCmdLine);

extern INT   ShowWaitCursor(VOID);
extern VOID  RestoreCursor(INT hPrev);


extern VOID  AddSectionFilesToCopyList(LPSTR szSect, LPSTR szSrc,
				LPSTR szDest);
extern VOID  AddSectionKeyFileToCopyList(LPSTR szSect, LPSTR szKey,
				LPSTR szSrc, LPSTR szDest);

extern VOID  CopyFilesInCopyList(VOID);
extern VOID  CreateIniKeyValue(LPSTR szFile, LPSTR szSect,
				LPSTR szKey, LPSTR szValue, INT cmo);


extern VOID  CreateProgmanGroup(LPSTR szGroup, LPSTR szPath,
				INT cmo);
extern VOID  CreateProgmanItem(LPSTR szGroup, LPSTR szItem,
				LPSTR szCmd, LPSTR szOther, INT cmo);

#ifdef WIN32
extern VOID  CreateProgmanGroupEx(LPSTR szGroup, BOOL fCommon,
				INT cmo);
extern VOID  CreateProgmanItemEx(LPSTR szGroup, BOOL fCommon, LPSTR szItem,
				LPSTR szCmd, LPSTR szOther, INT cmo);
#endif

extern VOID  AddToBillboardList(LPSTR szDll, INT idDlg, LPSTR szProc,
				LONG lTicks);
extern VOID  SetCopyGaugePosition(INT x, INT y);
extern INT   IsDirWritable(LPSTR szDir);

extern LPSTR GetWindowsDir(LPSTR szBuf, INT cbBuf);
extern LPSTR GetWindowsSysDir(LPSTR szBuf, INT cbBuf);

// NOTE: this routine must exist in code files supplied by app
extern	VOID SetupError(INT);

extern INT   InitInstall(VOID);

extern LPSTR	SzCatStr(LPSTR sz1, LPSTR sz2);
extern LPSTR	SzCat2Str(LPSTR sz1, LPSTR sz2, LPSTR sz3);
extern LPSTR	SzCat3Str(LPSTR sz1, LPSTR sz2, LPSTR sz3,
				LPSTR sz4);

extern LPSTR GetSectionKeyFilename(LPSTR szSect, LPSTR szKey,
				LPSTR szBuf, INT cbBuf);
extern VOID  ShowProgmanGroup(LPSTR szGroup, INT Cmd, INT cmo);
extern INT   APIENTRY  FShowProgManGroup(LPSTR szGroup, LPSTR szCmd, INT cmo);

#ifdef WIN32
extern VOID  ShowProgmanGroupEx(LPSTR szGroup, BOOL fCommon, INT Cmd, INT cmo);
extern INT   APIENTRY  FShowProgManGroupEx(LPSTR szGroup, BOOL fCommon, LPSTR szCmd, INT cmo);
#endif

extern VOID  ClearCopyList(VOID);
extern VOID  MakeListFromSectionSize(LPSTR szSym, LPSTR szSect);
extern LONG  GetSectionKeySize(LPSTR szSect, LPSTR szKey);
extern INT   GetWindowsMajorVersion(VOID);
extern INT   GetWindowsMinorVersion(VOID);
extern VOID  MakeListFromSectionFilename(LPSTR szSym, LPSTR szSect);
extern VOID  CreateDir(LPSTR szDir, INT cmo);
extern LPSTR GetSectionKeyDate(LPSTR szSect, LPSTR szKey,
				LPSTR szBuf, INT cbBuf);
#ifndef WIN32
extern VOID  CopyFile(LPSTR szFullPathSrc, LPSTR szFullPathDst,
				INT cmo, INT fAppend);
#endif
extern VOID  RemoveFile(LPSTR szFullPathSrc, INT cmo);
extern VOID  SetAbout(LPSTR szAbout1, LPSTR szAbout2);

extern VOID  RightTrim(LPSTR sz);
extern VOID  RemoveSymbol(LPSTR szSym);
extern LONG  GetCopyListCost(LPSTR szExtraList, LPSTR szCostList,
				LPSTR szNeedList);
extern VOID  StampResource(LPSTR szSect, LPSTR szKey, LPSTR szDst,
				INT wResType, INT wResId, LPSTR szData, INT cbData);
extern BOOL  DoesAnyFileNeedCopying(VOID);

extern	CHAR	szCurDir[];


#ifndef STF_LITE
extern VOID TerminateRegDb(VOID);


extern INT   SetBeepingMode(INT mode);
extern INT   SetSilentMode(INT mode);

extern LPSTR GetSectionKeyVersion(LPSTR szSect, LPSTR szKey,
				LPSTR szBuf, INT cbBuf);

extern VOID  MakeListFromSectionDate(LPSTR szSym, LPSTR szSect);
extern VOID  MakeListFromSectionVersion(LPSTR szSym, LPSTR szSect);


//install

extern VOID  RemoveDir(LPSTR szDir, INT cmo);
extern VOID  AddSpecialFileToCopyList(LPSTR szSect, LPSTR szKey,
				LPSTR szSrc, LPSTR szDest);
extern VOID  RemoveIniSection(LPSTR szFile, LPSTR szSect, INT cmo);
extern VOID  RemoveIniKey(LPSTR szFile, LPSTR szSect, LPSTR szKey,
				INT cmo);
extern VOID  CreateSysIniKeyValue(LPSTR szFile, LPSTR szSect,
				LPSTR szKey, LPSTR szValue, INT cmo);
extern VOID  DumpCopyList(LPSTR szFile);
extern VOID  AddDos5Help(LPSTR szProgName, LPSTR szProgHelp,
				INT cmo);
extern VOID  CopyAFile(LPSTR szFullPathSrc, LPSTR szFullPathDst, INT cmo,        
                                INT fAppend);                                   

extern VOID  BackupFile(LPSTR szFullPath, LPSTR szBackup);
extern VOID  RenameFile(LPSTR szFullPath, LPSTR szBackup);
extern VOID  AddBlankToBillboardList(LONG lTicks);
extern VOID  ClearBillboardList(VOID);
extern VOID  OpenLogFile(LPSTR szFile, INT fAppend);
extern VOID  CloseLogFile(VOID);
extern VOID  WriteToLogFile(LPSTR szStr);
extern VOID  SetRestartDir(LPSTR szDir);
extern BOOL  RestartListEmpty(VOID);
extern INT   ExitExecRestart(VOID);
extern LPSTR FindFileUsingFileOpen(LPSTR szFile, LPSTR szBuf,
				INT cbBuf);
extern BOOL  IsFileWritable(LPSTR szFile);
extern LPSTR GetNthFieldFromIniString(LPSTR szLine, INT iField,
				LPSTR szBuf, INT cbBuf);
extern VOID  PrependToPath(LPSTR szSrc, LPSTR szDst, LPSTR szDir,
				INT cmo);


// Error Handling


#ifdef DEBUG

// Setup API Errors
#define saeFail 0
#define saeInit 1
#define saeNYI  3
#define saeOvfl 4
#define saeArg  5	//must be max sae value

extern VOID  StfApiErr(INT nMsg, LPSTR szApi, LPSTR szArgs);
extern VOID  BadArgErr(INT nArg, LPSTR szApi, LPSTR szArgs);

extern INT   FValidDrive(LPSTR szDrive);
extern INT   FValidInfSect(LPSTR szSect);
extern INT   FValidIniFile(LPSTR szFile);

#define      FEmptySz(sz)            ((sz)==NULL || *(sz)=='\0')
#define      FValidSz(sz)            ((sz) && *(sz))

#endif //DEBUG


// Windows system detect

extern INT   GetWindowsMode(VOID);
extern BOOL  IsWindowsShared(VOID);
extern INT   GetScreenWidth(VOID);
extern INT   GetScreenHeight(VOID);

extern INT   InStr(INT cch, LPSTR sz1, LPSTR sz2);

extern BOOL     APIENTRY AssertSzUs(LPSTR, unsigned);
#ifdef DEBUG
#define  Assert(f)  ((f) ? (VOID)0 : (VOID)AssertSzUs(__FILE__,__LINE__))
#else
#define  Assert(f)  ((VOID)0)
#endif

typedef  BYTE FAR *  PB;

extern PB   APIENTRY PbSaveMemInf(VOID);
extern VOID APIENTRY RestoreMemInf(PB);
extern VOID APIENTRY FreeMemInf(PB);

extern BOOL APIENTRY AddSrcFileWithAttribsToCopyList(LPSTR szSect,
				LPSTR szKey, LPSTR szSrcPath, LPSTR szDstPath);
#endif  /* !STF_LITE */

#ifndef WF_WINNT
#define WF_WINNT 0x4000
#endif /* !WF_WINNT */

#ifndef WF_CPUR4000
#define WF_CPUR4000 0x0100
#endif /* !WF_CPUR4000 */
