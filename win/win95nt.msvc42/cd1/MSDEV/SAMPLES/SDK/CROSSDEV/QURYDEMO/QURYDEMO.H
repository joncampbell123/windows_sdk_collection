/*
	Header File for QURYDEMO application
	Created by Microsoft Corporation.
*/

#include <windows.h>
#include <windowsx.h>

#ifndef _MAC
#include "ctl3d.h"
#endif

#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <sqlext.h>
#include <odbcinst.h>

#include "resource.h"

/* MDI Child Window Offsets */

#define CBWNDEXTRA			4	// storage in MDI Child Window
#define GWLAPP_HDLG			0	// child dialog handle


/* Help File */

#define SAMPLESHELPFILE		"..\\help\\odbcsmpl.hlp"
#define HLP_QURYDEMO		30

/* Other Constants */

#define MAXCHILDWNDS        10						//maximum child windows allowed
#define MAXBUFLEN			256						//display buffer size
#define MAX_COL				15						//maximum column in result set
#define MAX_ROW				100						//maximum number of rows
#define MAXDATALEN			25						//maximum data length per column
#define MAXDISPLAYSIZE		MAX_COL*(MAXDATALEN+1)
#define	MINWIDTH			415						//minimum width of app window
#define	MINHEIGHT			350						//minimum height of app window
#define LISTTABSTOP			100						//tabstop in the display listbox
#define LISTHORZSCROLL		(LISTTABSTOP*2)			//horizontal scroll for the listbox
#define LISTHORZEXT			0						//horizontal extent inthe list box
#define WINDOWMENUPLACE		2						//Window menu place for adding MDI child windows
#define IDT_STATUSTIMER		1						//timer id
#define TIMERDELAY			60000					//timer duration
#define BTTNX				345						//first toolbar button location
#define BTTNY				2						//button y position
#define	BTTNWIDTH			25                      //button width
#define BTTNHEIGHT			23                      //button height
#define BTTNMARGIN			5						//distance between toolbar buttons
#define TOOLBARMARGINY		5						//fixed margin w/o combobox height
#define STATUSBARMARGINY	8						//fixed margin w/o textbox height

/* String Constants */

#if 0
#define POPUPLOGIN		"&Login"
#define MENUITEMCONNECT		"&Connect... \aCtrl+C"
#define MENUITEMDRIVERCONNECT	"D&river Connect... \aCtrl+R"
#define MENUITEMDISCONNECT	"&Disconnect... \aCtrl+D"
#define	MENUITEMEXIT		"E&xit"
#define POPUPQUERY		"&Query"
#define MENUITEMQUERY		"&Execute SQL \aCtrl+E"
#define MENUITEMNEW		"&New Query \aCtrl+N"
#define POPUPWINDOW		"&Window"
#define MENUITEMTILE		"&Tile"
#define	MENUITEMCASCADE		"&Cascade"
#define MENUITEMICONS		"Arrange &Icons"
#define	MENUITEMCLOSEALL	"Close &All"
#define POPUPHELP		"&Help"
#define MENUITEMAPPHELP		"&Help on Qurydemo \aF1"
#define	MENUITEMABOUT		"&About Qurydemo..."
#endif

#ifdef WIN32
#define ODBCFRAMECLASS		"QDEMO32MAIN"
#else
#define ODBCFRAMECLASS		"QDEMOMAIN"
#endif

#define ODBCTOOLCLASS		"QDEMOTool"
#define ODBCSTATUSCLASS		"QDEMOStatus"
#define ODBCMDICLASS		"QDEMOMDI"
#define COMBOBOXCLASS		"COMBOBOX"
#define MDICLIENTCLASS		"MDICLIENT"


#define UNTITLED			"Untitled"
#define APPTITLE			"ODBC Query Demo"
#define	EXECERROR			"Execution Error"
#define INITERROR			"Failed to initialize ODBC interface"
#define CLASSERROR			"Failed to register application class"
#define CREATEMAINERR		"Failed to create application main window"
#define CREATECHILDERR		"Failed to create a new window"
#define ALPHABETS			"abcdefghijklmnopqrstuvwxyz"
#define TIMETEXT			"MM:MM MM"
#define DATETEXT			"MMM-MM"
#define TIMEFORMAT			" %I:%M %p"
#define DATEFORMAT			" %b-%d"
#define NODSNERR			"Please provide a Data Source"
#define MOREINFO			"Incomplete Information"

#define DEFDBNAME				"?"
#define DSN_HDBC_FORMAT			"%s!%lX"
#define DSN_HDBC_HSTMT_FORMAT	"DSN=%s!HDBC=%lX!HSTMT=%lX"
#define SCANDSNHDBC_FORMAT		"%[^!]!%lX"
#define SCANHDBC_FORMAT			"%*[^!]!%lX"
#define SCANHDBC_TITLEFORMAT	"DSN=%*[^!]!HDBC=%lX"
#define SCANHDBCHSTMT_FORMAT	"DSN=%*[^!]!HDBC=%lX!HSTMT=%lX"
#define SCANHSTMT_TITLEFORMAT	"DSN=%*[^!]!HDBC=%*lX!HSTMT=%lX"
#define CUR_MARK				"<-"
#define CURQUERY_STRING			"Query!%lX<-"
#define QUERY_STRING			"Query!%lX"
#define MAKECONNECT				"Please connect to a data source using Login menu."
#define	OPENWINDOW				"Please open a window from Query menu."
#define NOHDBCERROR				"No Connection Available"
#define NOHSTMTERROR			"No Query Window Available"
#define MAXCHILDEXCEEDED		"Please close a child window and try again."
#define MAXCHLDERR				"Error - Too many Child Windows"
#define COLTRUNC_WARNG			"Number of columns in display truncated to %u"
#define ROWTRUNC_WARNG			"Number of rows in display truncated to %u"
#define TRUNCERR				"Data Truncation"
#define CLOSEALLHDBC			"All open connections have been closed."
#define LOGOUTINFO				"Log Out Information"
#define SQLERR_FORMAT			"SQL Error State:%s, Native Error Code: %lX, ODBC Error: %s"
#define SQLERRMSGTITLE			"SQL_ERROR results"
#define SQLERRCNTDTITLE			"SQL_ERROR results continued"
#define SQLWRNMSGTITLE			"SQL_SUCCESS_WITH_INFO results"
#define SQLWRNCNTDTITLE			"SQL_SUCCESS_WITH_INFO results continued"
#define NULLDATASTRING			"SQL_NULL_DATA"

/* Globals */

extern HWND			hWndFrame;			// Main Frame Window handle
extern HWND			hWndToolbar;		// Toolbar window handle
extern HWND			hWndCrsrList;		// hdbc(s) combobox on the tool bar
extern HWND			hWndStmtList;		// hstmt(s) combobox on the tool bar
extern HWND			hWndStatusbar;		// status bar handle
extern HWND			hWndMDIClient;		// MDI Client window handle
extern HWND			hWndActiveChild;	// Current active MDI Child window
extern HINSTANCE	hAppInstance;		// Application instance
extern DWORD		wStatusText;		// Status text state
extern int			iTimex;				// Time box (on the status bar) width
extern int			iDatex;				// Date box (on the status bar) width
extern RECT			rectStatusText;		// Text box on the status bar
extern char			szAppName[MAXBUFLEN];


#ifdef __SQL	// internal to query.c

int		nChildCount;			// Number of child windows currently open
HENV		henv;				// Environment Handle
char		szDispBuffer[MAXDISPLAYSIZE+1];	// Display Buffer
UCHAR		rgbData[MAX_COL][MAXDATALEN];	// Results Data Array
SDWORD		dwDataLen[MAX_COL];		// Results Data Length Array

#endif

/* Macros and Function Prototypes */

#ifdef WIN32
#define		ACTIVATEWINDOW(h)	SetForegroundWindow(h)
#else
#define		ACTIVATEWINDOW(h)	SetFocus(h)
#endif

#define DRAWBTTNRECT(h,x,y,cx,cy)	MoveToEx(h,x+1,y,NULL);\
					LineTo(h,x+cx,y);\
					MoveToEx(h,x+cx,y+1,NULL);\
					LineTo(h,x+cx,y+cy);\
					MoveToEx(h,x+cx-1,y+cy,NULL);\
					LineTo(h,x,y+cy);\
					MoveToEx(h,x,y+cy-1,NULL);\
					LineTo(h,x,y);

#define DRAWBTTNLIFT1(h,x,y,cx,cy)	MoveToEx(h,x+1,y+cy-1,NULL);\
					LineTo(h,x+1,y+1);\
					LineTo(h,x+cx-1,y+1);

#define	DRAWBTTNLIFT2(h,x,y,cx,cy)	MoveToEx(h,x+1,y+cy-1,NULL);\
					LineTo(h,x+cx-1,y+cy-1);\
					LineTo(h,x+cx-1,y);\
					MoveToEx(h,x+2,y+cy-2,NULL);\
					LineTo(h,x+cx-2,y+cy-2);\
					LineTo(h,x+cx-2,y+1);

int	PASCAL		WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
long	CALLBACK	MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
long	CALLBACK	ToolbarProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
long	CALLBACK	StatusbarProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
long	CALLBACK	MDIChildProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL	CALLBACK	ConnectDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL	CALLBACK	DisconnectDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL	CALLBACK	AboutDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL	CALLBACK	MDIChildDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID	WINAPI	DrawBitmap(HDC hDC, int iLeft, int iTop, HBITMAP hBitmap);

BOOL	WINAPI	InitSQLEnvironment();
void	WINAPI	DisplayDatabases(HWND hWnd);
BOOL	WINAPI	ConnectDatabase(HWND hWnd);
BOOL	WINAPI	DriverConnectDatabase(HWND hWnd);
void	WINAPI	DisplayConnections(HWND hWndhdbc);
void	WINAPI	DisplayQueries(HWND hWndhstmt, HWND hWndhdbc, int nCrsrIndex);
void	WINAPI	NewQueryWindow();
void	WINAPI	ChangeCurrentQuery(HWND hWndStmtList);
void	WINAPI	ChangeCurrentCursor(HWND hWndCrsrList);
void	WINAPI	DisplayNewCrsrAndStmt();
void	WINAPI	FreeConnect(HWND hWndhdbc);
void	WINAPI	FreeQuery(HWND hWndhstmt, HWND hWndhdbc, int nIndex);
void	WINAPI	CloseQueryWindow(HWND hWnd);
BOOL	WINAPI	FreeSQLEnvironment();
void	WINAPI	ExecuteQuery();
BOOL	WINAPI	CloseDatabases();

#ifdef __SQL	// internal to query.c

void	WINAPI	DisplayError(RETCODE nResult, HWND hWnd, HENV henv, HDBC hdbc, HSTMT hstmt);

#endif
/******************************* End of File ********************************/
