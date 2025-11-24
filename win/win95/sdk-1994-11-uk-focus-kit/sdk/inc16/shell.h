/*****************************************************************************\
*                                                                             *
* shell.h -  SHELL.DLL functions, types, and definitions		      *
*                                                                             *
* Copyright (c) 1993-1994, Microsoft Corp.	All rights reserved	      *
*                                                                             *
\*****************************************************************************/

#ifndef _INC_SHELL
#define _INC_SHELL

#include <commctrl.h>	// for ImageList_ and other this depends on
#include <shellapi.h>




#ifndef RC_INVOKED
#pragma pack(1)         /* Assume byte packing throughout */
#endif /* !RC_INVOKED */

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

//====== Ranges for WM_NOTIFY codes ==================================

// Note that these are defined to be unsigned to avoid compiler warnings
// since NMHDR.code is declared as UINT.

// NM_FIRST - NM_LAST defined in commctrl.h (0U-0U) - (OU-99U)

// LVN_FIRST - LVN_LAST defined in commctrl.h (0U-100U) - (OU-199U)

// PSN_FIRST - PSN_LAST defined in prsht.h (0U-200U) - (0U-299U)

// HDN_FIRST - HDN_LAST defined in commctrl.h (0U-300U) - (OU-399U)

// TVN_FIRST - TVN_LAST defined in commctrl.h (0U-400U) - (OU-499U)

#define RDN_FIRST       (0U-500U)
#define RDN_LAST        (0U-509U)

#define RFN_FIRST       (0U-510U) // run file dialog notify
#define RFN_LAST        (0U-519U)

// TTN_FIRST - TTN_LAST defined in commctrl.h (0U-520U) - (OU-549U)

#define SEN_FIRST       (0U-550U)
#define SEN_LAST        (0U-559U)




#define EXN_FIRST       (0U-1000U)  // shell explorer/browser
#define EXN_LAST        (0U-1199U)

#define MAXPATHLEN      MAX_PATH

#ifndef FO_MOVE //these need to be kept in sync with the ones in shlobj.h

#define FO_MOVE           0x0001
#define FO_COPY           0x0002
#define FO_DELETE         0x0003
#define FO_RENAME         0x0004

#define FOF_MULTIDESTFILES         0x0001
#define FOF_CONFIRMMOUSE           0x0002
#define FOF_SILENT                 0x0004  // don't create progress/report
#define FOF_RENAMEONCOLLISION      0x0008
#define FOF_NOCONFIRMATION         0x0010  // Don't prompt the user.
#define FOF_WANTMAPPINGHANDLE      0x0020  // Fill in SHFILEOPSTRUCT.hNameMappings
                                      // Must be freed using SHFreeNameMappings
#define FOF_ALLOWUNDO              0x0040
#define FOF_FILESONLY              0x0080  // on *.*, do only files
#define FOF_SIMPLEPROGRESS         0x0100  // means don't show names of files

typedef WORD FILEOP_FLAGS;

#define PO_DELETE	0x0013  // printer is being deleted
#define PO_RENAME	0x0014  // printer is being renamed
#define PO_PORTCHANGE	0x0020  // port this printer connected to is being changed
				// if this id is set, the strings received by
				// the copyhook are a doubly-null terminated
				// list of strings.  The first is the printer
				// name and the second is the printer port.
#define PO_REN_PORT	0x0034  // PO_RENAME and PO_PORTCHANGE at same time.

// no POF_ flags currently defined

typedef WORD PRINTEROP_FLAGS;

#endif // FO_MOVE

// implicit parameters are:
//      if pFrom or pTo are unqualified names the current directories are
//      taken from the global current drive/directory settings managed
//      by Get/SetCurrentDrive/Directory
//
//      the global confirmation settings
typedef struct _SHFILEOPSTRUCT
{
	HWND		hwnd;
	UINT		wFunc;
	LPCSTR		pFrom;
	LPCSTR		pTo;
	FILEOP_FLAGS	fFlags;
	BOOL		fAnyOperationsAborted;
	LPVOID		hNameMappings;
        LPCSTR          lpszProgressTitle; // only used if FOF_SIMPLEPROGRESS
} SHFILEOPSTRUCT, FAR *LPSHFILEOPSTRUCT;


int WINAPI SHFileOperation(LPSHFILEOPSTRUCT lpFileOp);
void WINAPI SHFreeNameMappings(HANDLE hNameMappings);

typedef struct _SHNAMEMAPPING
{
    LPSTR pszOldPath;
    LPSTR pszNewPath;
    int   cchOldPath;
    int   cchNewPath;
} SHNAMEMAPPING, FAR *LPSHNAMEMAPPING;

#define SHGetNameMappingCount(_hnm) \
	DSA_GetItemCount(_hnm)
#define SHGetNameMappingPtr(_hnm, _iItem) \
	(LPSHNAMEMAPPING)DSA_GetItemPtr(_hnm, _iItem)

#define STRREG_SHEX             "shellex"
#ifdef _WIN32



// Note CLASSKEY overrides CLASSNAME
#define SEE_MASK_CLASSNAME	0x00000001
#define SEE_MASK_CLASSKEY	0x00000003
// Note INVOKEIDLIST overrides IDLIST
#define SEE_MASK_IDLIST		0x00000004
#define SEE_MASK_INVOKEIDLIST	0x0000000c
#define SEE_MASK_ICON		0x00000010
#define SEE_MASK_HOTKEY		0x00000020
#define SEE_MASK_NOCLOSEPROCESS 0x00000040
#define SEE_MASK_CONNECTNETDRV	0x00000080
#define SEE_MASK_FLAG_DDEWAIT	0x00000100
#define SEE_MASK_DOENVSUBST	0x00000200
#define SEE_MASK_FLAG_NO_UI	0x00000400
#define SEE_MASK_FLAG_SHELLEXEC 0x00000800				/* ; Internal */
// All other bits are masked off when we do an InvokeCommand		/* ; Internal */
#define SEE_VALID_CMIC_BITS	0x00000FF0				/* ; Internal */
#define SEE_VALID_CMIC_FLAGS	0x00000FC0				/* ; Internal */

typedef struct _SHELLEXECUTEINFO
{
	DWORD cbSize;
	ULONG fMask;
	HWND hwnd;
	LPCSTR lpVerb;
	LPCSTR lpFile;
	LPCSTR lpParameters;
	LPCSTR lpDirectory;
	int nShow;
	HINSTANCE hInstApp;

	// Optional fields
	LPVOID lpIDList;
	LPCSTR lpClass;
	HKEY hkeyClass;
        DWORD dwHotKey;
        HANDLE hIcon;
	HANDLE hProcess;
} SHELLEXECUTEINFO, FAR *LPSHELLEXECUTEINFO;

BOOL WINAPI ShellExecuteEx(LPSHELLEXECUTEINFO lpExecInfo);
void WINAPI WinExecError(HWND hwnd, int error, LPCSTR lpstrFileName, LPCSTR lpstrTitle);

#define EIRESID(x) (-1 * (int)(x))

UINT WINAPI ExtractIconEx(LPCSTR lpszFile, int nIconIndex,
        HICON FAR *phiconLarge, HICON FAR *phiconSmall, UINT nIcons);

UINT WINAPI ExtractIcons(LPCSTR szFileName, int nIconIndex,
        int cxIcon, int cyIcon, HICON FAR *phicon, UINT nIcons, UINT flags);

// Tray notification definitions
typedef struct _NOTIFYICONDATA
{
	DWORD cbSize;
	HWND hWnd;
	UINT uID;

	UINT uFlags;
	UINT uCallbackMessage;
	HICON hIcon;
	char szTip[64];
} NOTIFYICONDATA, *PNOTIFYICONDATA;




#define NIM_ADD         0x00000000
#define NIM_MODIFY	0x00000001
#define NIM_DELETE	0x00000002

#define NIF_MESSAGE	0x00000001
#define NIF_ICON	0x00000002
#define NIF_TIP		0x00000004

BOOL WINAPI Shell_NotifyIcon(DWORD dwMessage, PNOTIFYICONDATA lpData);

#endif // _WIN32

/*
 * The SHFormatDrive API provides access to the Shell
 *   format dialog. This allows apps which want to format disks
 *   to bring up the same dialog that the Shell does to do it.
 *
 *   This dialog is not sub-classable. You cannot put custom
 *   controls in it. If you want this ability, you will have
 *   to write your own front end for the DMaint_FormatDrive
 *   engine.
 *
 *   NOTE that the user can format as many diskettes in the specified
 *   drive, or as many times, as he/she wishes to. There is no way to
 *   force any specififc number of disks to format. If you want this
 *   ability, you will have to write your own front end for the
 *   DMaint_FormatDrive engine.
 *
 *   NOTE also that the format will not start till the user pushes the
 *   start button in the dialog. There is no way to do auto start. If
 *   you want this ability, you will have to write your own front end
 *   for the DMaint_FormatDrive engine.
 *
 *   PARAMETERS
 *
 *     hwnd    = The window handle of the window which will own the dialog
 *     drive   = The 0 based (A: == 0) drive number of the drive to format
 *     fmtID   = The ID of the physical format to format the disk with
 *		 NOTE: The special value SHFMT_ID_DEFAULT means "use the
 *		       default format specified by the DMaint_FormatDrive
 *		       engine". If you want to FORCE a particular format
 *		       ID "up front" you will have to call
 *		       DMaint_GetFormatOptions yourself before calling
 *		       this to obtain the valid list of phys format IDs
 *		       (contents of the PhysFmtIDList array in the
 *		       FMTINFOSTRUCT).
 *     options = There is currently only two option bits defined
 *
 *		  SHFMT_OPT_FULL
 *                SHFMT_OPT_SYSONLY
 *
 *		 The normal defualt in the Shell format dialog is
 *		 "Quick Format", setting this option bit indicates that
 *		 the caller wants to start with FULL format selected
 *		 (this is useful for folks detecting "unformatted" disks
 *		 and wanting to bring up the format dialog).
 *
 *               The SHFMT_OPT_SYSONLY initializes the dialog to
 *               default to just sys the disk.
 *
 *		 All other bits are reserved for future expansion and
 *		 must be 0.
 *
 *		 Please note that this is a bit field and not a value
 *		 and treat it accordingly.
 *
 *   RETURN
 *	The return is either one of the SHFMT_* values, or if the
 *	returned DWORD value is not == to one of these values, then
 *	the return is the physical format ID of the last succesful
 *	format. The LOWORD of this value can be passed on subsequent
 *	calls as the fmtID parameter to "format the same type you did
 *	last time".
 *
 */
DWORD WINAPI SHFormatDrive(HWND hwnd, UINT drive, UINT fmtID,
				 UINT options);

//
// Special value of fmtID which means "use the default format"
//
#define SHFMT_ID_DEFAULT    0xFFFF

//
// Option bits for options parameter
//
#define SHFMT_OPT_FULL     0x0001
#define SHFMT_OPT_SYSONLY  0x0002

//
// Special return values. PLEASE NOTE that these are DWORD values.
//
#define SHFMT_ERROR	0xFFFFFFFFL	// Error on last format, drive may be formatable
#define SHFMT_CANCEL	0xFFFFFFFEL	// Last format was canceled
#define SHFMT_NOFORMAT 0xFFFFFFFDL	// Drive is not formatable

/*
 * The SHCheckDrive API provides access to the Shell
 *   Check Disk dialog. This allows apps which want to check disks
 *   to bring up the same dialog that Shell does to do it.
 *
 *   This dialog is not sub-classable. You cannot put custom
 *   controls in it. If you want this ability, you will have
 *   to write your own front end for the DMaint_FixDrive
 *   engine.
 *
 *   NOTE that the check will not start till the user pushes the
 *   start button in the dialog unless the SHCHK_OPT_AUTO option is set.
 *
 *   PARAMETERS
 *
 *     hwnd    = The window handle of the window which will own the dialog
 *     options = These options basically coorespond to the check boxes
 *		 in the Advanced Options dialog. See SHCHK_OPT_ defines
 *		 below.
 *     DrvList = This is a DWORD bit field which indicates the 0 based
 *		 drive numbers to check. Bit 0 = A, Bit 1 = B, ...
 *		 For use on this API at least one bit must be set (if
 *		 this argument is 0, the call will return SHCHK_NOCHK).
 *     lpHwnd  = An optional argument (can be NULL). If it is non-NULL,
 *		 SHCheckDrive will place the window handle of the top
 *		 level window it creates in this location.
 *
 *   RETURN
 *	The return is either one of the SHCHK_* values.
 *
 */
DWORD WINAPI SHCheckDrive(HWND hwnd, DWORD options, DWORD DrvList, HWND FAR* lpHwnd);

//
// Special return values. PLEASE NOTE that these are DWORD values.
//
#define SHCHK_ERROR	0xFFFFFFFFL	// Fatal Error on check
#define SHCHK_CANCEL	0xFFFFFFFEL	// Check was canceled
#define SHCHK_NOCHK	0xFFFFFFFDL	// At least one Drive is not "checkable"
					//   or no drives were specified
#define SHCHK_SMNOTFIX	0xFFFFFFFCL	// Some errors were not fixed
#define SHCHK_ERRORMEM	0xFFFFFFFBL	// Couldn't alloc memory to start check
#define SHCHK_ERRORINIT 0xFFFFFFFAL	// Couldn't access DSKMAINT.DLL


//
// Option bits
//
// IMPORTANT NOTE: These are set up so that the default setting is 0
//		   for all bits WITH ONE EXCEPTION. Currently the default
//		   setting has the SHCHK_OPT_XLCPY bit set......
//
// Also note that specification of invalid combonations of bits (for example
// setting both SHCHK_OPT_XLCPY and SHCHK_OPT_XLDEL) will result in very random
// behavior.
//
#define SHCHK_OPT_REP		0x00000001L  // Generate detail report
#define SHCHK_OPT_RO		0x00000002L  // Run in preview mode
#define SHCHK_OPT_NOSYS 	0x00000004L  // Surf Anal don't check system area
#define SHCHK_OPT_NODATA	0x00000008L  // Surf Anal don't check data area
#define SHCHK_OPT_NOBAD 	0x00000010L  // Disable Surface Analysis
#define SHCHK_OPT_LSTMF 	0x00000020L  // Convert lost clusters to files
#define SHCHK_OPT_NOCHKNM	0x00000040L  // Don't check file names
#define SHCHK_OPT_NOCHKDT	0x00000080L  // DOn't check date/time fields
#define SHCHK_OPT_INTER 	0x00000100L  // Interactive mode
#define SHCHK_OPT_XLCPY 	0x00000200L  // Def cross link resolution is COPY
#define SHCHK_OPT_XLDEL 	0x00000400L  // Def cross link resolution is DELETE
#define SHCHK_OPT_ALLHIDSYS	0x00000800L  // All HID SYS files are unmovable
#define SHCHK_OPT_NOWRTTST	0x00001000L  // Surf Anal no write testing.
#define SHCHK_OPT_DEFOPTIONS	0x00002000L  // Get above options from registry
#define SHCHK_OPT_DRVLISTONLY	0x00004000L  // Normaly all drives in the system
					     // are shown in the drive list box
					     // and those on the DrvList are selected
					     // This option says put only the drives
					     // in DrvList in the list box and
					     // disable the control
#define SHCHK_OPT_AUTO		0x00008000L  // Auto push start button
#define SHCHK_OPT_EXCLULOCK	0x00010000L  // Exclusive lock drive
#define SHCHK_OPT_FLWRTLOCK	0x00020000L  // Allow RD fail write
#define SHCHK_OPT_MKOLDFS	0x00040000L  // Remove new FS stuff
					     // WARNING: This function is
					     // reserved for SETUP, specifying
					     // This will TURN OFF the Read
					     // Only (preview), Interactive
					     // and report settings, and turn
					     // off surface analysis
					     // automatically.....
#define SHCHK_OPT_PROGONLY	0x00080000L  // Put up front end dialog with
					     // the progress bar, but run
					     // "silently", no errors or
					     // warnings...
#define SHCHK_OPT_NOWND 	0x00100000L  // No window at all and run
					     // silently as above...
#define SHCHK_OPT_NOCHKHST	0x00200000L  // By default a check of the
					     // host drive of compresssed
					     // volumes is done first as
					     // part of the check of the
					     // compressed drive. This
					     // option disables this.

// NOTE: This function should be public, as it is used by print common dlgs.
/*
 * The SHObjectProperties API provides an easy way to invoke
 *   the Properties context menu command on shell objects.
 *
 *   PARAMETERS
 *
 *     hwndOwner    The window handle of the window which will own the dialog
 *     dwType       A SHOP_ value as defined below
 *     lpObject     Name of the object, see SHOP_ values below
 *     lpPage       The name of the property sheet page to open to or NULL.
 *
 *   RETURN
 *
 *     TRUE if the Properties command was invoked
 */
BOOL WINAPI SHObjectProperties(HWND hwndOwner, DWORD dwType, LPCSTR lpObject, LPCSTR lpPage);

#define SHOP_PRINTERNAME 1  // lpObject points to a printer friendly name
#define SHOP_FILEPATH    2  // lpObject points to a fully qualified path+file name

/*
 * The SHGetFileInfo API provides an easy way to get attributes
 * for a file given a pathname.
 *
 *   PARAMETERS
 *
 *     pszPath              file name to get info about
 *     dwFileAttributes     file attribs, only used with SHGFI_USEFILEATTRIBUTES
 *     psfi                 place to return file info
 *     cbFileInfo           size of structure
 *     uFlags               flags
 *
 *   RETURN
 *     TRUE if things worked
 */

typedef struct _SHFILEINFO
{
        HICON       hIcon;                      // out icon
        int         iIcon;                      // out icon index
        DWORD       dwAttributes;               // in/out SFGAO_ flags
        char        szDisplayName[MAX_PATH];    // out display name (or path)
        char        szTypeName[80];             // out
} SHFILEINFO;

#define SHGFI_ICON              0x000000100     // get icon
#define SHGFI_DISPLAYNAME       0x000000200     // get display name
#define SHGFI_TYPENAME          0x000000400     // get type name
#define SHGFI_ATTRIBUTES        0x000000800     // get attributes
#define SHGFI_ICONLOCATION      0x000001000     // get icon location
#define SHGFI_EXETYPE           0x000002000     // return exe type
#define SHGFI_SYSICONINDEX      0x000004000     // get system icon index
#define SHGFI_LARGEICON         0x000000000     // get large icon
#define SHGFI_SMALLICON         0x000000001     // get small icon
#define SHGFI_OPENICON          0x000000002     // get open icon
#define SHGFI_TINYICON          0x000000004     // get tiny (always 16x16) icon
#define SHGFI_PIDL              0x000000008     // pszPath is a pidl
#define SHGFI_USEFILEATTRIBUTES 0x000000010     // use passed dwFileAttribute

DWORD WINAPI SHGetFileInfo(LPCSTR pszPath, DWORD dwFileAttributes, SHFILEINFO FAR *psfi, UINT cbFileInfo, UINT uFlags);


BOOL WINAPI SHGetNewLinkInfo(LPCSTR pszLinkTo, LPCSTR pszDir, LPSTR pszName,
			     BOOL FAR * pfMustCopy, UINT uFlags);

#define SHGNLI_PIDL		0x000000001	// pszLinkTo is a pidl
#define SHGNLI_PREFIXNAME	0x000000002	// Make name "Shortcut to xxx"


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#ifndef RC_INVOKED
#pragma pack()
#endif  /* !RC_INVOKED */


#endif  // !_INC_SHELL
