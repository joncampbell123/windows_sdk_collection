/*--------------------------------------------------------------------------
  CPPDEMO.H -- Shared constants, types, prototypes, variables

  This code is furnished on an as-is basis as part of the ODBC SDK and is
  intended for example purposes only.

--------------------------------------------------------------------------*/
      
#ifndef __CPPDEMO_H
#define __CPPDEMO_H

#ifdef	INCL_GLOBAL
#define	GLOBAL
#define	INITHAND(name, type)		type name = NULL;
#define	CONSTSTR(name, str)			const char* name = str
#else
#define	GLOBAL						extern
#define	INITHAND(name, type)		extern type name
#define CONSTSTR(name, str)			extern const char* name
#endif


//////////////////////////////////////////////////////////////////////////////
//
//	Defines
//
#ifdef _AFX
IMPLEMENT_DYNAMIC(CODBCException, CException);
IMPLEMENT_DYNAMIC(CODBC, CObject);
IMPLEMENT_DYNAMIC(CENV,  CODBC);
IMPLEMENT_DYNAMIC(CDBC,  CODBC);
IMPLEMENT_DYNAMIC(CSTMT, CODBC);
#endif

#ifdef WIN32
#define EXPFUNC CALLBACK
#define INTFUNC WINAPI
#else
#ifndef EXPORT
#define EXPORT		_export
#endif
#define EXPFUNC EXPORT CALLBACK
#define INTFUNC WINAPI
#endif

#ifdef	_DEBUG
#define	DASSERT(x)		assert(x)
#else
#define	DASSERT(x)		(x)
#endif

#define	AllocPtr(x)		GlobalAllocPtr(GHND, (x))
#define	FreePtr(x)		if( (x) ) GlobalFreePtr((x))

#define	MF_ENABLE		MF_ENABLED | MF_BYCOMMAND
#define MF_DISABLE		MF_GRAYED | MF_BYCOMMAND

#define	HOURGLASS(h)	{\
						h = SetCursor(LoadCursor(NULL, IDC_WAIT));\
						ShowCursor(TRUE);\
						}
#define ARROW(h)		{\
						ShowCursor(FALSE);\
						SetCursor(h);\
                        }

#ifndef min
#define min(a,b)        (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b)        (((a) > (b)) ? (a) : (b))
#endif


//////////////////////////////////////////////////////////////////////////////
//
//	Constants
//

// Miscellaneous
GLOBAL  const           cbSTRLEN        = 256;      // Max string length
GLOBAL  const           cbMAXSQL        = 512;      // Max SQL string length
GLOBAL  const           cPOINTS         = 10;       // Font point size
GLOBAL  const           cxBORDER        = 6;        // Column border
GLOBAL  const           cbCOLMEMMAX     = 10000L;   // Maximum column mem size
GLOBAL  const           HLP_CPPDEMO     = 20;       // Help context number

// Strings
CONSTSTR(szCLASSNAME,   "ODBC_CPP_DEMO");           // Window class name
CONSTSTR(szDATATRUNC,   "01004");                   // ODBC truncation error
CONSTSTR(szDSNKEY,      "DSN=");                    // Data source name key
CONSTSTR(szDASH,        " - ");                     // Title separator
CONSTSTR(szNODSN,       "no DSN");                  // DSN name when no DSN
CONSTSTR(szFONT,        "MS Sans Serif");           // Window font name
CONSTSTR(szSCROLLCLASS, "SCROLLBAR");               // Scrollbar class name
CONSTSTR(szHELPFILE,    "..\\help\\odbcsmpl.hlp");  // Samples help file


//////////////////////////////////////////////////////////////////////////////
//
//	Types
//
typedef struct tagCOL
	{
	char		szName[cbSTRLEN];	// Column name
	SDWORD		cb;					// Column width (transfer width)
	SDWORD		cbc;				// Column width (display  width)
	SDWORD		fCType;				// C data type
	SDWORD		fSqlType;			// ODBC SQL data type
	LPSDWORD	lpcb;				// Pointer to returned width
	LPBYTE		lpb;				// Pointer to returned data
	LPSDWORD	lpcbuf;				// Aggregate width buffer
	LPBYTE		lpbuf;				// Aggregate data buffer
	}
	COL, FAR *LPCOL;
	
typedef struct tagAPPINST
	{
	HINSTANCE	hinst;				// Application instance handle
	char		szTitle[cbSTRLEN];	// Current window title

	HWND		hwndVScroll;		// Vertical scrollbar handle
	HWND		hwndHScroll;		// Horizontal scrollbar handle
	
	BOOL		fVScroll;			// Vertical scrollbar present
	BOOL		fHScroll;			// Horizontal scrollbar present
	
	BOOL		fIsMinimized;		// Window is minimized
	
	BOOL		fCtl3d;				// Ctl3d enabled
	BOOL		fAutoCtl3d;			// Ctl3d in auto-subclass mode
	
	CENV*		cEnv;				// Environment object
	CDBC*		cDbc;				// Connection object
	CSTMT*		cStmt;				// Statement object
	
	BOOL		fConnected;			// Connected to data source
	BOOL		fData;				// Data fetched from data source
	BOOL		fResultSet;			// Result set exists
	
	UWORD		cCol;				// # of columns in the result set
	UWORD		cRow;				// # of rows in the result set
	UWORD		cMaxRow;			// max # of rows in the result set
	UWORD		crowwin;			// # of rows to display at once
	UWORD		ccolwin;			// # of cols to display at once
	UWORD		ccols;				// Number of display chars per row
	UWORD		cbrow;				// Number of storage bytes per row
	
	LPCOL		lpcol;				// Column information array
	
	char		szSQL[cbMAXSQL];	// SQL statement buffer
	char
		szDSN[SQL_MAX_DSN_LENGTH+1];// DSN buffer
	}
	APPINST, FAR *LPAPPINST;
	
typedef struct tagAPPGLOB
	{
	char		szTitle[cbSTRLEN];	// Application title
	
	HICON		hicon;				// Application icon
	
	HBRUSH		hbrWin;				// Window background brush
	HBRUSH		hbrBtn;				// Button face brush
	HBRUSH		hbrScroll;			// Scrollbar brush
	
	HFONT		hfontName;			// Column name font
	HFONT		hfontData;			// Data item font
	
	int			cxVScroll;			// Width of vertical scrollbar
	int			cyHScroll;			// Height of horizontal scrollbar
	
	int			cx;					// Width of a character
	int			cy;					// Height of a character
	
	char		szNull[cbSTRLEN];	// <NULL> string
	
	int			cInstance;			// instance counter
	}
	APPGLOB, FAR *LPAPPGLOB;
        
        
//////////////////////////////////////////////////////////////////////////////
//
//	Globals
//


//////////////////////////////////////////////////////////////////////////////
//
//	Prototypes
//

//	General routines
void	INTFUNC	vDoMessage(HWND, UINT);
BOOL	INTFUNC	fODBCError(HWND, RETCODE);

//	Window and dialog procedures
BOOL	EXPFUNC AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL	EXPFUNC	ConfirmDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL	EXPFUNC OkDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL	EXPFUNC	SQLDlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT	EXPFUNC WndProc(HWND, UINT, WPARAM, LPARAM);


#endif	// __CPPDEMO_H
