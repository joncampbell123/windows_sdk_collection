/*------------------------------------------------------------------------|
  | Copyright (c) 1990, MicroSoft Corporation                             |
  |                                                                       |
  | verstamp.h - Header file for version fuction sample                    |
  |-----------------------------------------------------------------------|
  | modification history                                                  |
  | Date    Reason                                                        |
  | 911201  Creation date                                                 |
  |------------------------------------------------------------------------*/
#define VERINFOLEN    512             // Bytes to hold version info resource
#define BUFLEN        256             // Buffer lengths

#define IDM_ABOUT         100         // menu commands
#define IDM_FILEOPEN      101
#define IDM_EXIT          102

#define IDD_MOREINFO      202       
#define IDD_VERINSTALL    203

#define IDD_ICON          205          

#define DLG_WINDIR          100       // DLG item id's
#define DLG_SYSDIR          101       // for main dialog window
#define DLG_SRCFILENAME     102
#define DLG_SRCDIRNAME      103
#define DLG_SRCFILEVER      104
#define DLG_SRCPRODVER      105
#define DLG_SRCFILEFLAG     106
#define DLG_SRCOS           107
#define DLG_SRCFILETYPE     108
#define DLG_SRCSUB          109
#define DLG_SRCLANGUAGE     110
#define DLG_DSTFILENAME     111
#define DLG_DSTDIRNAME      112
#define DLG_DSTFILEVER      113
#define DLG_DSTPRODVER      114
#define DLG_DSTFILEFLAG     115
#define DLG_DSTFILEOS       116
#define DLG_DSTFILETYPE     117
#define DLG_DSTSUB          118
#define DLG_DSTLANGUAGE     119

#define DLG_COMPANYNAME     120       // DLG item id's
#define DLG_FILEDESCRIPTION 121       // for more info dialog
#define DLG_FILEVERSION     122
#define DLG_INTERNALNAME    123
#define DLG_COPYRIGHT       124
#define DLG_TRADEMARKS      125
#define DLG_PRODUCTNAME     126
#define DLG_PRODUCTVERSION  127

DWORD gdwVerInfo       = 0L;           // Global data 
BOOL  bFirst           = TRUE;

typedef struct tagVS_VERSION
  {
    WORD wTotLen;
    WORD wValLen;
    char szSig[16];
    VS_FIXEDFILEINFO vffInfo;
  } VS_VERSION;

typedef struct tagLANGANDCP
  {
    WORD wLanguage;
    WORD wCodePage;
  } LANGANDCP;

HANDLE  ghInst;

char gszUserMsg[BUFLEN];

char  gszCurDir[_MAX_PATH],
      gszWinDir[_MAX_PATH],           // Windows directory
      gszSysDir[_MAX_PATH],           // Windows system directory
      gszSrcDir[_MAX_PATH],           // source dir name
      gszSrcFile[BUFLEN],             // source file name
      gszTrgDir[_MAX_PATH],
      gsqTrgFile[BUFLEN],
      gszCurDir[_MAX_PATH], 
      gszDstDir[_MAX_PATH]; 


WORD  gwCurDirLen = _MAX_PATH;
WORD  gwDstDirLen = _MAX_PATH;


char gszDlgClear[] = ".";               // Indicates no info
char gszTrans[] = "\\VarFileInfo\\Translation";
char *gszVerString[] = 
  {
  "Illegal string",
  "CompanyName",
  "FileDescription",
  "FileVersion",
  "InternalName",
  "LegalCopyright",
  "LegalTrademarks",
  "ProductName",
  "ProductVersion"
  };
char gszAppName[] = "VerStamp";



int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
long FAR PASCAL WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL MoreVerInfo(HWND, unsigned, WORD, LONG);
BOOL MyGetOpenFileName(HWND, LPSTR, LPSTR);
BOOL ShowVerInfo(HWND, LPSTR, LPSTR, WORD);
void ClearDlgVer(HWND, int);
void FillVerDialog(HWND, VS_VERSION FAR *, WORD);
WORD MyVerFindFile(HWND, LPSTR, LPSTR, LPSTR);
DWORD MyVerInstallFile(LPSTR, LPSTR, LPSTR);
BOOL PASCAL HandleVerFindFileRes(HWND, WORD, LPSTR, LPSTR);
void PostInstallProcessing(HWND, LPSTR, LPSTR);
void HandleVerInstallFileRes(HWND, DWORD, LPSTR, LPSTR);
