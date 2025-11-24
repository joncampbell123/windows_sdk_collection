#ifdef DEFINE_GLOBALS

#define GLOBAL   

// #include "counters.h"

// initialize some of the globals
// only perfmon.c will define DEFINE_GLOBALS

int     aiIntervals [] = { 1, 5, 15, 30, 60, 120, 300, 600, 3600 } ;


// setup CVS delimiter and line ending used by Export routines
CHAR           CommasStr[] = "," ;
CHAR           TabStr[] = "\t" ;
CHAR           *pDelimiter = TabStr ;
CHAR           LineEndStr[] = "\n" ;

// PDFN           CounterFuncEntry = CounterEntry ;


#else
// only perfmon.c define DEFINE_GLOBALS,
// all other references to them as extern
#define GLOBAL extern

#define  NumIntervals   9
GLOBAL   int            aiIntervals [] ;

// setup CVS delimiter and line ending used by Export routines
GLOBAL   CHAR           CommasStr[] ;
GLOBAL   CHAR           TabStr[] ;
GLOBAL   CHAR           *pDelimiter ;
GLOBAL   CHAR           LineEndStr[] ;
// GLOBAL   PDFN           CounterFuncEntry ;
#endif



//=============================//
// Graph Data Information      //
//=============================//


GLOBAL   PPERFSYSTEM    pSysInfo ;
GLOBAL   PGRAPHSTRUCT   pGraphs;


//=============================//
// Font Information            //
//=============================//


GLOBAL   HFONT          hFontScales ;
GLOBAL   HFONT          hFontScalesBold ;
GLOBAL   LONG           HalfTextHeight;

GLOBAL   HFONT          hFontPrinterScales ;
GLOBAL   HFONT          hFontPrinterScalesBold ;


//=============================//
// Control Information         //
//=============================//


GLOBAL   INT            iPerfmonView ;
GLOBAL   LANGID         iLanguage ;
GLOBAL   LANGID         iEnglishLanguage ;
GLOBAL   OPTIONS        Options ;

GLOBAL   HICON          hIcon ;
GLOBAL   HANDLE         hInstance;
GLOBAL   HANDLE         hAccelerators ;

GLOBAL   HMENU          hMenuChart ;
GLOBAL   HMENU          hMenuAlert ;
GLOBAL   HMENU          hMenuLog ;
GLOBAL   HMENU          hMenuReport ;


//=============================//
// Windows                     //
//=============================//


GLOBAL   HWND    hWndMain ;
GLOBAL   HWND    hWndGraph ;
GLOBAL   HWND    hWndLog ;
GLOBAL   HWND    hWndAlert ;
GLOBAL   HWND    hWndReport ;

GLOBAL   HWND    hWndGraphDisplay ;
GLOBAL   HWND    hWndGraphLegend ;
GLOBAL   HWND    hWndGraphStatus ;

GLOBAL   HWND    hWndAlertLegend ;

GLOBAL   HWND    hWndToolbar ;
GLOBAL   HWND    hWndStatus ;


//=============================//
// System Metrics              //
//=============================//


GLOBAL   int     xScreenWidth ;
GLOBAL   int     yScreenHeight ;

GLOBAL   int     xBorderWidth ;
GLOBAL   int     yBorderHeight ;

GLOBAL   int     xScrollWidth ;
GLOBAL   int     yScrollHeight ;

GLOBAL   int     xScrollThumbWidth ;
GLOBAL   int     yScrollThumbHeight ;


GLOBAL   int     xDlgBorderWidth ;
GLOBAL   int     yDlgBorderHeight ;

GLOBAL   int     MinimumSize ;

//=============================//
// Miscellaneous               //
//=============================//

GLOBAL   int            iUnviewedAlerts ;
GLOBAL   COLORREF       crLastUnviewedAlert ;

GLOBAL   LPTSTR         pChartFileName ;
GLOBAL   LPTSTR         pChartFullFileName ;
GLOBAL   LPTSTR         pAlertFileName ;
GLOBAL   LPTSTR         pAlertFullFileName ;
GLOBAL   LPTSTR         pLogFileName ;
GLOBAL   LPTSTR         pLogFullFileName ;
GLOBAL   LPTSTR         pReportFileName ;
GLOBAL   LPTSTR         pReportFullFileName ;


// globals for perfmornance improvements

// frequently used GDI objects 
GLOBAL   UINT     ColorBtnFace ;  // for concave/convex button painting
GLOBAL   HBRUSH   hBrushFace ;    // for concave/convex button painting
GLOBAL   HPEN     hPenHighlight ; // for concave/convex button painting
GLOBAL   HPEN     hPenShadow ;    // for concave/convex button painting
GLOBAL   HPEN     hWhitePen ;     // for chart highlighting
GLOBAL   HANDLE   hbLightGray ;   // for painting the background

// bPerfmonIconic is TRUE when perfmon is minimized.
// Thus, we don't need to update chart or report view until
// it is not iconized
GLOBAL   BOOL     bPerfmonIconic ;

// bAddLineInPorgress is TRUE when Addline dialog is up.  It is used
// in freeing unused system during data collecting. (But not while
// addline dialog is still up)
GLOBAL   BOOL     bAddLineInProgress ;

// bDelayAddAction is TRUE when reading setting files or adding more
// than 1 counter.  This is to delay some of the costly screen adjustments
// until we have added all the lines.
GLOBAL   BOOL     bDelayAddAction ;


// bExplainTxtButtonHit is TRUE when the ExplainText button in addline 
// dialog is clicked.  This is to save time and memory for fetching the
// help text, during BuildNameTable(), unless it is needed.
GLOBAL   BOOL     bExplainTextButtonHit ;

// globals used for WinHelp
GLOBAL   DWORD          dwCurrentDlgID ;
GLOBAL   DWORD          dwCurrentMenuID ;
GLOBAL   LPTSTR         pszHelpFile ;

// Following includes space for trailing NULL and preceeding \\'s
GLOBAL   TCHAR  LocalComputerName[MAX_COMPUTERNAME_LENGTH + 3];

//=============================//
// Log Playback Information    //
//=============================//

GLOBAL   PLAYBACKLOG    PlaybackLog ;

GLOBAL   REPORT         Report ;
GLOBAL   ALERT          Alert ;
GLOBAL   LOG            Log ;


