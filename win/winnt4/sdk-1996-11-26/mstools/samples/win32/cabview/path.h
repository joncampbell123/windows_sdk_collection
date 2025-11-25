//*******************************************************************************************
//
// Filename : Path.h
//	
//				Path APIs
//
// Copyright (c) 1994 - 1996 Microsoft Corporation. All rights reserved
//
//*******************************************************************************************


#ifndef _PATH_H_
#define _PATH_H_

#ifdef __cplusplus
extern "C" {
#endif



extern const TCHAR c_szStarDotStar[];
extern const TCHAR c_szStar[];
extern const TCHAR c_szSlash[];
extern const TCHAR c_szNULL[];

#define DRIVEID(path)   ((path[0] - 'A') & 31)
#define ARRAYSIZE(a)    (sizeof(a)/sizeof(a[0]))
#define SIZEOF(v)       sizeof(v)


BOOL   PathIsDotOrDotDot(LPTSTR pszDir);
BOOL   PathStripToRoot(LPTSTR pszRoot);
LPTSTR PathAddBackslash(LPTSTR lpszPath);
LPTSTR PathRemoveBackslash(LPTSTR lpszPath);
void   PathRemoveBlanks(LPTSTR lpszString);
BOOL   PathRemoveFileSpec(LPTSTR lpszPath);
LPTSTR PathFindFileName(LPCTSTR pPath);
BOOL   PathIsRoot(LPCTSTR lpszPath);
BOOL   PathIsRelative(LPCTSTR lpszPath);
BOOL   PathIsUNC(LPCTSTR lpsz);
BOOL   PathIsDirectory(LPCTSTR lpszPath);
BOOL   PathIsExe(LPCTSTR lpszPath);
int    PathGetDriveNumber(LPCTSTR lpszPath);
LPTSTR PathCombine(LPTSTR szDest, LPCTSTR lpszDir, LPCTSTR lpszFile);
BOOL   PathAppend(LPTSTR pPath, LPCTSTR pMore);
LPTSTR PathBuildRoot(LPTSTR szRoot, int iDrive);
int    PathCommonPrefix(LPCTSTR pszFile1, LPCTSTR pszFile2, LPTSTR achPath);
LPTSTR PathFindExtension(LPCTSTR pszPath);
BOOL   PathFileExists(LPCTSTR lpszPath);
BOOL   PathMatchSpec(LPCTSTR pszFile, LPCTSTR pszSpec);
BOOL   PathMakeUniqueName(LPTSTR pszUniqueName, UINT cchMax, LPCTSTR pszTemplate, LPCTSTR pszLongPlate, LPCTSTR pszDir);
LPTSTR PathGetArgs(LPCTSTR pszPath);
BOOL   PathGetShortName(LPCTSTR lpszLongName, LPTSTR lpszShortName, UINT cbShortName);
void   PathQuoteSpaces(LPTSTR lpsz);
void   PathUnquoteSpaces(LPTSTR lpsz);
BOOL   PathDirectoryExists(LPCTSTR lpszDir);

#define PQD_NOSTRIPDOTS 0x00000001
void   PathQualifyDef(LPTSTR psz, LPCTSTR szDefDir, DWORD dwFlags);
void   PathQualify(LPTSTR lpsz);


#define PRF_VERIFYEXISTS            0x0001
#define PRF_TRYPROGRAMEXTENSIONS    (0x0002 | PRF_VERIFYEXISTS)
#define PRF_FIRSTDIRDEF             0x0004
#define PRF_DONTFINDLNK             0x0008      // if PRF_TRYPROGRAMEXTENSIONS is specified

int    PathResolve(LPTSTR lpszPath, LPCTSTR FAR dirs[], UINT fFlags);


LPTSTR PathGetNextComponent(LPCTSTR lpszPath, LPTSTR lpszComponent);
LPTSTR PathFindNextComponent(LPCTSTR lpszPath);
BOOL   PathIsSameRoot(LPCTSTR pszPath1, LPCTSTR pszPath2);

//
//  Return codes from PathCleanupSpec.  Negative return values are
//  unrecoverable errors
//
#define PCS_FATAL           0x80000000
#define PCS_REPLACEDCHAR    0x00000001
#define PCS_REMOVEDCHAR     0x00000002
#define PCS_TRUNCATED       0x00000004
#define PCS_PATHTOOLONG     0x00000008  // Always combined with FATAL

int    PathCleanupSpec(LPCTSTR pszDir, LPTSTR pszSpec);

BOOL   PathCompactPath(HDC hDC, LPTSTR lpszPath, UINT dx);
void   PathSetDlgItemPath(HWND hDlg, int id, LPCTSTR pszPath);

#define GCT_INVALID             0x0000
#define GCT_LFNCHAR             0x0001
#define GCT_SHORTCHAR           0x0002
#define GCT_WILD                0x0004
#define GCT_SEPERATOR           0x0008
UINT PathGetCharType(TCHAR ch);

void PathRemoveArgs(LPTSTR pszPath);
BOOL PathMakePretty(LPTSTR lpPath);

BOOL PathIsFileSpec(LPCTSTR lpszPath);
BOOL PathIsLink(LPCTSTR szFile);

BOOL PathRenameExtension(LPTSTR pszPath, LPCTSTR pszExt);

int DriveType(int iDrive);

//-------- drive type identification --------------
// iDrive      drive index (0=A, 1=B, ...)
//
#define DRIVE_CDROM     5           // extended DriveType() types
#define DRIVE_RAMDRIVE  6

#define DRIVE_TYPE      0x000F      // type masek
#define DRIVE_SLOW      0x0010      // drive is on a slow link
#define DRIVE_LFN       0x0020      // drive supports LFNs
#define DRIVE_AUTORUN   0x0040      // drive has AutoRun.inf in root.
#define DRIVE_AUDIOCD   0x0080      // drive is a AudioCD
#define DRIVE_AUTOOPEN  0x0100      // should *always* auto open on insert
#define DRIVE_NETUNAVAIL 0x0200     // Network drive that is not available
#define DRIVE_SHELLOPEN  0x0400     // should auto open on insert, if shell has focus

#define DriveTypeFlags(iDrive)      DriveType('A' + (iDrive))
#define DriveIsSlow(iDrive)         (DriveTypeFlags(iDrive) & DRIVE_SLOW)
#define DriveIsLFN(iDrive)          (DriveTypeFlags(iDrive) & DRIVE_LFN)
#define DriveIsAutoRun(iDrive)      (DriveTypeFlags(iDrive) & DRIVE_AUTORUN)
#define DriveIsAutoOpen(iDrive)     (DriveTypeFlags(iDrive) & DRIVE_AUTOOPEN)
#define DriveIsShellOpen(iDrive)    (DriveTypeFlags(iDrive) & DRIVE_SHELLOPEN)
#define DriveIsAudioCD(iDrive)      (DriveTypeFlags(iDrive) & DRIVE_AUDIOCD)
#define DriveIsNetUnAvail(iDrive)   (DriveTypeFlags(iDrive) & DRIVE_NETUNAVAIL)

#define IsCDRomDrive(iDrive)        (DriveType(iDrive) == DRIVE_CDROM)
#define IsRamDrive(iDrive)          (DriveType(iDrive) == DRIVE_RAMDRIVE)
#define IsRemovableDrive(iDrive)    (DriveType(iDrive) == DRIVE_REMOVABLE)
#define IsRemoteDrive(iDrive)       (DriveType(iDrive) == DRIVE_REMOTE)


#ifdef __cplusplus
}
#endif

#endif // _PATH_H_