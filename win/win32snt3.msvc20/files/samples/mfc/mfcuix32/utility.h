/*
 * UTILITY.H
 *
 * Miscellaneous prototypes and definitions for OLE UI dialogs.
 *
 * Copyright (c)1992 Microsoft Corporation, All Right Reserved
 */


#ifndef _UTILITY_H_
#define _UTILITY_H_

//Function prototypes
//UTILITY.C
HCURSOR  WINAPI HourGlassOn(void);
void     WINAPI HourGlassOff(HCURSOR);

BOOL     WINAPI Browse(HWND, LPTSTR, LPTSTR, UINT, UINT, DWORD);
int      WINAPI ReplaceCharWithNull(LPTSTR, int);
int      WINAPI ErrorWithFile(HWND, HINSTANCE, UINT, LPTSTR, UINT);
BOOL     WINAPI DoesFileExist(LPTSTR lpszFile);
LONG     WINAPI Atol(LPTSTR lpsz);

HICON FAR PASCAL    HIconAndSourceFromClass(REFCLSID, LPTSTR, UINT FAR *);
BOOL FAR PASCAL FIconFileFromClass(REFCLSID, LPTSTR, UINT, UINT FAR *);
LPTSTR FAR PASCAL PointerToNthField(LPTSTR, int, TCHAR);
BOOL FAR PASCAL GetAssociatedExecutable(LPTSTR, LPTSTR);
HICON    WINAPI HIconFromClass(LPTSTR);
BOOL     WINAPI FServerFromClass(LPTSTR, LPTSTR, UINT);
UINT     WINAPI UClassFromDescription(LPTSTR, LPTSTR, UINT);
UINT     WINAPI UDescriptionFromClass(LPTSTR, LPTSTR, UINT);
BOOL     WINAPI FVerbGet(LPTSTR, UINT, LPTSTR);
LPTSTR   WINAPI ChopText(HWND hwndStatic, int nWidth, LPTSTR lpch,
	int nMaxChars);
void     WINAPI OpenFileError(HWND hDlg, UINT nErrCode, LPTSTR lpszFile);

// string formatting APIs
void WINAPI FormatStrings(LPTSTR, LPCTSTR, LPCTSTR*, int);
void WINAPI FormatString1(LPTSTR, LPCTSTR, LPCTSTR);
void WINAPI FormatString2(LPTSTR, LPCTSTR, LPCTSTR, LPCTSTR);

// QueryInterface is easy with wrappers and UNICODE
#define QUERYINTERFACE(lpUnkIn, IFaceName, lplpUnkOut) \
	lpUnkIn->QueryInterface(IID_##IFaceName, (LPVOID*)lplpUnkOut)

// global instance to load strings/resources from
extern HINSTANCE _g_hOleStdInst;
extern HINSTANCE _g_hOleStdResInst;

// standard OLE 2.0 clipboard formats
extern UINT _g_cfObjectDescriptor;
extern UINT _g_cfLinkSrcDescriptor;
extern UINT _g_cfEmbedSource;
extern UINT _g_cfEmbeddedObject;
extern UINT _g_cfLinkSource;
extern UINT _g_cfOwnerLink;
extern UINT _g_cfFileName;

//Metafile utility functions
STDAPI_(void)    OleUIMetafilePictIconFree(HGLOBAL);
STDAPI_(BOOL)    OleUIMetafilePictIconDraw(HDC, LPCRECT, HGLOBAL, BOOL);
STDAPI_(UINT)    OleUIMetafilePictExtractLabel(HGLOBAL, LPTSTR, UINT, LPDWORD);
STDAPI_(HICON)   OleUIMetafilePictExtractIcon(HGLOBAL);
STDAPI_(BOOL)    OleUIMetafilePictExtractIconSource(HGLOBAL, LPTSTR, UINT FAR *);

#endif //_UTILITY_H_
