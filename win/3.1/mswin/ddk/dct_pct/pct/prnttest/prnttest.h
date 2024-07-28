/*******************************************************************************

    Header File:    PrntTest.H

    Purpose:        Primary header file for the PrntTest application

    This is included in every source module.  It contains global constants
    and declarations used throughout the application.

    Copyright   1989-1992 by Microsoft Corporation

*******************************************************************************/

#include <print.h>
#include "isg_test.h"   /* 05-23-1991  */
#include "dialog.h"     /* 08/27/1991  */

//  Typedefs to make the compiler happier.  These are only used because we
// want to be able to run on 3.0, as well.
typedef int  (FAR PASCAL* FIND31GDICALL)(HDC);
typedef int  (FAR PASCAL* FINDSTARTDOC)(HDC,LPDOCINFO);
typedef int  (FAR PASCAL* FINDSETABORTPROC)(HDC,ABORTPROC);

#define STRING_SIZE 256

#define IDD_HEAD_ALL (IDD_HEAD_TITLEPAGE  | IDD_HEAD_CON   | IDD_HEAD_CAPS |\
                      IDD_HEAD_FONT       | IDD_HEAD_BRSH  | IDD_HEAD_PEN  |\
                      IDD_HEAD_BOUNDS     | IDD_HEAD_ENTRY | IDD_HEAD_GRAY)
                          
#define IDD_TEST_ALL (IDD_TEST_TEXT     | IDD_TEST_BITMAPS  | \
                      IDD_TEST_RESETDC  | IDD_TEST_ABORTDOC | \
                      IDD_TEST_POLYGONS | IDD_TEST_CURVES   | \
                      IDD_TEST_LINES    | IDD_TEST_CHARWIDTH)

/*---------------------------------------------------------------------------*\
| WINDOW CLASS/CREATE VALUES                                                  |
\*---------------------------------------------------------------------------*/
#define PRNTTESTCLASS "PRNTTEST"
#define PRNTTESTMENU  "PRNTTEST"
#define PRNTTESTICON  "MAINICON"
#define PRNTTESTTITLE "Windows Printer Test Application"

/*---------------------------------------------------------------------------*\
| MENU COMMAND ID'S                                                           |
\*---------------------------------------------------------------------------*/
#define IDM_SETTINGS_HEADER      100
#define IDM_SETTINGS_TESTS       101
#define IDM_OPTIONS_STYLES       200
#define IDM_OPTIONS_FONTS        201
#define IDM_TEST_RUN             300
#define IDM_HELP_ABOUT           400
#define IDM_HELP_DESCR           401

/*---------------------------------------------------------------------------*\
| STRING TABLE IDENTIFIERS - Separated into logical 16 value blocks.          |
\*---------------------------------------------------------------------------*/
#define IDS_ERROR_STARTDOC      8001
#define IDS_ERROR_GETDC         8002
#define IDS_ERROR_GETCAPS       8003
#define IDS_TEST_JOBTITLE       8004

#define IDS_INTRFACE_PROF       8017
#define IDS_INTRFACE_NAME       8018
#define IDS_INTRFACE_DRIV       8019
#define IDS_INTRFACE_PORT       8020
#define IDS_INTRFACE_ADD        8021
#define IDS_INTRFACE_MOD        8022
#define IDS_INTRFACE_REM        8023
#define IDS_INTRFACE_SET        8024

#define IDS_HEAD_TITLEPAGE      8032
#define IDS_HEAD_PRINTAREA      8033
#define IDS_HEAD_PRINTAREA1     8034

#define IDS_TEST_RAST_NO        8048
#define IDS_TEST_TEXT_STR1      8049

#define IDS_TST_HEAD_FUNC       8064
#define IDS_TST_HEAD_GRAY       8065
#define IDS_TST_DSCR_TEXT       8066
#define IDS_TST_DSCR_RAST       8067
#define IDS_TST_DSCR_POLY       8068
#define IDS_TST_DSCR_WDTH       8069
#define IDS_TST_DSCR_CURV       8070
#define IDS_TST_DSCR_LINE       8071

#define IDS_FOOT_TITLE          9000
#define IDS_FOOT_ENTRY          9001
#define IDS_FOOT_GRAY           9002
#define IDS_FOOT_CAPS           9003
#define IDS_FOOT_FONT_DETAIL    9004
#define IDS_FOOT_FONT_SAMPLES   9005
#define IDS_FOOT_PEN_INFO       9006
#define IDS_FOOT_PENS           9007
#define IDS_FOOT_BRUSH_INFO     9008
#define IDS_FOOT_BRUSHES        9009
#define IDS_FOOT_TEXT           9010
#define IDS_FOOT_BITMAPS        9011
#define IDS_FOOT_POLYGONS       9012
#define IDS_FOOT_CHARWIDTH      9013
#define IDS_FOOT_CURVES         9014
#define IDS_FOOT_LINES          9015

#define FLAGMASK              0x0FFF

/*----------------------------------------------*\
| Flags for the Justify/Text Functions used in   |
| the TEXT Tests.                                |
\*----------------------------------------------*/
#define ID_LEFT                  100
#define ID_RIGHT                 101
#define ID_CENTER                102
#define ID_JUSTIFY               103

#define SYNFONT_ITALIC        0x0001
#define SYNFONT_UNDERLINED    0x0002
#define SYNFONT_STRIKEOUT     0x0004

#define TEXT_OBJECT           0x0001
#define GRAPHICS_OBJECT       0x0002

#if !defined(GLOBALVAR)
#define GLOBALVAR extern
char szDescription[STRING_SIZE];
#else
extern   char szDescription[];
#endif

GLOBALVAR BOOL          bAutoRun, bAbort, bUseBanding, bMetafile;
GLOBALVAR HDEVOBJECT    hFonts, hPens, hBrushes;
GLOBALVAR PRINTER       pPrinter;
GLOBALVAR DEVINFO       dcDevCaps;
GLOBALVAR FARPROC       lpAbortProc, lpAbortDlg;
GLOBALVAR HANDLE        hInst, hDevMode;
GLOBALVAR HWND          hAbortDlg, hPrntDlg;
GLOBALVAR WORD          wHeaderSet, wTestsSet, wTestsEnded, wStyleSelected;
GLOBALVAR WORD          wFontOptions;
GLOBALVAR TEST          tlTest;
GLOBALVAR HDC           hdcPrinter, hdcTarget;
GLOBALVAR TEXTMETRIC    tmDefault;

#define iRight1Inch             (dcDevCaps.nLogPixelsX)
#define iRight1HalfInch         (iRight1Inch/2)
#define iRight1QuarterInch      (iRight1Inch/4)
#define iDown1Inch              (dcDevCaps.nLogPixelsY)
#define iDown1HalfInch          (iDown1Inch/2)
#define iDown1QuarterInch       (iDown1Inch/4)
#define iRight3QuarterInches    (iRight1Inch - iRight1QuarterInch)
#define iDown3QuarterInches     (iDown1Inch - iDown1QuarterInch)
#define iRight5QuarterInches    (iRight1Inch + iRight1QuarterInch)
#define iDown3HalfInches        (iDown1Inch + iDown1HalfInch)

#define iHeightOfOneTextLine    (tmDefault.tmHeight)
#define iHeightOfOneTextLineWithLeading \
                                (tmDefault.tmHeight+tmDefault.tmExternalLeading)
#define iMaxCharWidth           (tmDefault.tmMaxCharWidth)
#define iAverageCharWidth       (tmDefault.tmAveCharWidth)
#define iTextAscent             (tmDefault.tmAscent)
#define iTextDescent            (tmDefault.tmDescent)

#define iPageHeight             (dcDevCaps.nVertRes)
#define iPageWidth              (dcDevCaps.nHorzRes)

#define DEVICE_TO_POINTSIZE(a)  ((a) * 72 / dcDevCaps.nLogPixelsY)
#define POINTSIZE_TO_DEVICE(a)  ((a) * dcDevCaps.nLogPixelsY / 72)

#define RectHeight(x)       ((x).bottom-(x).top)
#define RectWidth(x)        ((x).right-(x).left)
#define StringWidth(x)      (lstrlen(x) * iAverageCharWidth)
#define ExtendRect(r,x,y)   (((r).right+=(x)),((r).bottom+=(y)))

#define strApplicationProfile   "prnttest.ini"

#undef  GLOBALVAR

/*---------------------------------------------------------------------------*\
| FUNCTION DECLARATIONS                                                       |
\*---------------------------------------------------------------------------*/

BOOL    ProcessPrntTestCommands(HWND,WORD);
HFONT   GetDefaultFont(void);

BOOL    PrintTitlePage(void);
BOOL    PrintFunctionSupport(void);
BOOL    PrintGrayScale(void);
BOOL    PrintPrintableArea(void);
BOOL    PrintDeviceCapabilities(void);
BOOL    PrintDeviceFonts(void);
BOOL    PrintDeviceBrushes(void);
BOOL    PrintDevicePens(void);
BOOL    PrintText(void);
BOOL    PrintBitmaps(void);
BOOL    PrintPolygons(void);
BOOL    PrintCharWidths(void);
BOOL    PrintCurves(void);
BOOL    PrintLines(void);
BOOL    TestAbortDoc(void);
BOOL    PrintBrushSummary(void);
BOOL    PrintPenSummary(void);

BOOL    GetPrinterInformation(HDC,LPPRINTER,LPSTR,LPSTR);
int     GetFontName(LPLOGFONT,LPSTR);
FARPROC Find31GDICall(LPCSTR);

// Ordinals of 3.1 GDI functions--ordinals are faster and safe because
// we do a version check before ever trying to load them.
#define RESETDC_ORDINAL       MAKEINTRESOURCE(376)
#define STARTDOC_ORDINAL      MAKEINTRESOURCE(377)
#define ENDDOC_ORDINAL        MAKEINTRESOURCE(378)
#define STARTPAGE_ORDINAL     MAKEINTRESOURCE(379)
#define ENDPAGE_ORDINAL       MAKEINTRESOURCE(380)
#define SETABORTPROC_ORDINAL  MAKEINTRESOURCE(381)
#define ABORTDOC_ORDINAL      MAKEINTRESOURCE(382)

/*----------------------------------------------*\
| WINDOW CALL BACK FUNCTIONS.                    |
\*----------------------------------------------*/
BOOL FAR PASCAL AbortDlg(HWND,WORD,WORD,LONG);
BOOL FAR PASCAL AboutDlg(HWND,WORD,WORD,LONG);
BOOL FAR PASCAL PrintAbortProc(HDC,short);
BOOL FAR PASCAL PrntTestDlg(HWND,WORD,WORD,LONG);
LONG FAR PASCAL PrntTestProc(HWND,WORD,WORD,LONG);
BOOL FAR PASCAL FontOptionsDlg(HWND,WORD,WORD,LONG);
BOOL FAR PASCAL ResetDCDlg(HWND,WORD,WORD,LONG);
BOOL FAR PASCAL SetupHeaderDlg(HWND,WORD,WORD,LONG);
BOOL FAR PASCAL SetupTestsDlg(HWND,WORD,WORD,LONG);
BOOL FAR PASCAL SelectStyleDlg(HWND,WORD,WORD,LONG);
BOOL FAR PASCAL SetupObjectsDlg(HWND,WORD,WORD,LONG);
BOOL            InitObjectsFontList(HWND);

/******************************************************************************

    Following is the interface to the page image management routines.

******************************************************************************/

typedef BOOL (FAR *LPDRAWFN)(LPRECT lprcBounds, WORD wBlkSize, LPVOID lpArgs);

BOOL cdecl  AddDrawObject(PRECT prcBounds, LPDRAWFN lpDrawFn, WORD wAttr,
                          WORD  wArgBlockSize, LPVOID lpArgBlock);

BOOL cdecl  AdjustObject(LPRECT lprcBounds, int iXOffset, int iYOffset,
                         LPSTR lpstrDescription);

BOOL cdecl  PrintPage(LPSTR lpstrFooter);
int  cdecl  PrintTestDescription(WORD wDescriptionFileName);
BOOL cdecl  DoSomeText(LPRECT lprcBounds, WORD wLength, LPVOID lpString);
