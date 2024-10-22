/* 
 * go.h	
 */


/* globals */
extern char		szAppName[];	// the name of the application
extern HANDLE		hInstApp;	// application instance handle
extern HWND		hwndApp;	// application main window
extern MEDTYPE		medtypePhys;	// Physical type for Load/Save


/* GoWinInfo -- instance information associated with application window.
 * This is a fixed LocalAlloc'd structure, the local pointer to which
 * is placed in WindowWord(0).
 */

typedef struct _GoWinInfo
{
	MEDUSER	meduser;
	MEDID	medid;
	BOOL	fFileOp;
} GoWinInfo;

typedef GoWinInfo NEAR *PWinInfo;


/* macro to get the structure pointer */
#define GetWinInfo(WINDOW)	((PWinInfo) GetWindowWord((WINDOW), 0))


/* custom window messages */
#define WM_SETMEDID	(WM_USER + 1)		// switch resource id's


/* string table id's... */

#define IDS_APPTITLE		100
#define IDS_APPTITLEHEADER	101
#define IDS_UNTITLED		102
#define IDS_NORESOURCE		103

#define IDS_OPENTITLE		105
#define IDS_SAVETITLE		106
#define IDS_UNTITLEDCHANGE	107
#define IDS_RESOURCECHANGE	108

#define IDS_CANTLOADMEDGO	113

#define IDS_GONAME		115
#define IDS_GOTXNAME		116
#define IDS_GOEXT		117

#define IDS_ERRORLOAD		118


/* menu item id's... */

#define IDM_FILEEXIT	100
#define IDM_FILEABOUT	101
#define IDM_FILEOPEN	102
#define IDM_FILECLOSE	103
#define IDM_FILESAVE	104
#define IDM_FILESAVEAS	105
#define IDM_FILENEW	106

/* Dialog button ID's */
#define IDB_GOHAND	120
#define IDB_GOTXHAND	121



/* prototypes */
DWORD GoUserProc( HWND hwnd, MEDID medid, 
	MEDMSG medmsg, MEDINFO medinfo,
	LONG lParam, DWORD dwInst);

WORD DoAbout(HWND hwnd, HANDLE hInst);

VOID AppPaint( HDC hdc, MEDID medid, LPRECT prcPaint, POINT ptClientSize );

VOID AppClick( HWND hwnd, MEDID medid, POINT ptClick, BOOL fRight );

VOID ResourceFixTitle(HWND hwnd, MEDID medid);

BOOL ResourceAskToSave(HWND hwnd);

BOOL ResourceRelease(HWND hwnd, BOOL fImmediate);

BOOL ResourceSet(HWND hwnd, MEDID medidNew);

BOOL ResourceSaveFile(HWND hwnd, BOOL fSaveAs);


LONG InitMenuPopup(HWND hwnd, WORD wParam, LONG lParam);

LONG AppCommand(HWND hwnd, unsigned msg, WORD wParam, LONG lParam);


BOOL FAR PASCAL PhysTypeDlgProc(HWND hwnd, unsigned msg, 
		WORD wParam, LONG lParam);


