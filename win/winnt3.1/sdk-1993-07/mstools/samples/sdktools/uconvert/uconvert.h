/**************************************************************************\
* uconvert.h -- header file for uconvert program.
*
* Function prototypes, global variables, & preprocessor defines.
\**************************************************************************/

/**************************************************************************\
*  Function prototypes, window procedures first.
\**************************************************************************/

LRESULT CALLBACK MainWndProc            (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK AboutProc              (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK SourceOptionsProc      (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DestinationOptionsProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ConversionOptionsProc  (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ViewSourceProc         (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ViewDestinationProc    (HWND, UINT, WPARAM, LPARAM);

BOOL IsUnicode (PBYTE );
BOOL IsBOM (PBYTE );
BOOL IsRBOM (PBYTE );

VOID framechildwindow     (HDC, HWND, HWND);
VOID underlinechildwindow (HDC, HWND, HWND);

VOID createwindows(PRECT, HWND, HWND*, HWND*, HWND*, HWND*, HWND*, HWND*);

LPVOID ManageMemory (UINT, UINT, DWORD, LPVOID);



/**************************************************************************\
*  Global variables (declared in uconvert.c).
\**************************************************************************/

/* Child windows appearing on the main frame window. */
extern HWND hwndLabel0,     hwndLabel1;
extern HWND hwndName0,      hwndName1;
extern HWND hwndSize0,      hwndSize1;
extern HWND hwndCodePage0,  hwndCodePage1;
extern HWND hwndByteOrder0, hwndByteOrder1;
extern HWND hwndButton0,    hwndButton1;

/* Information specifying which is unicode and what the other code page is. */
extern int  gTypeSource;
extern UINT giSourceCodePage;
extern UINT giDestinationCodePage;

/* pointers to global source & destination data, and byte count. */
extern PBYTE pSourceData;
extern PBYTE pDestinationData;
extern int   nBytesSource;
extern int   nBytesDestination;

/* Conversion Options variables. */
extern DWORD gMBFlags;
extern DWORD gWCFlags;
extern char  glpDefaultChar[];
extern BOOL  gUsedDefaultChar;



/**************************************************************************\
*  Defined constants.
\**************************************************************************/

/* Messages that can be send to ManageMemory() as first param */
#define MMALLOC 1
#define MMFREE  2
/* Messages that can be send to ManageMemory() as second param */
#define MMSOURCE       3
#define MMDESTINATION  4

/* Allowed values for the global variable gTypeSource */
#define TYPEUNKNOWN     0
#define TYPEUNICODE     1
#define TYPECODEPAGE    2

/* "user message."  Used by main window.  */
#define  WMU_SETTYPESTRINGS     WM_USER +1
#define  WMU_ADJUSTFORNEWSOURCE WM_USER +2

/* menu IDs */
#define  MID_OPEN                    1001
#define  MID_SAVEAS                  1002
#define  MID_EXIT                    1010

#define  MID_COPYDESTINATION         2001
#define  MID_PASTESOURCE             2002
#define  MID_CLEARSOURCE             2003
#define  MID_CLEARDESTINATION        2004

#define  MID_CONVERTNOW              3001
#define  MID_SOURCEOPT               3002
#define  MID_DESTINATIONOPT          3003
#define  MID_CONVERSIONOPT           3004
#define  MID_SWAPSOURCE              3005
#define  MID_SWAPDESTINATION         3006
#define  MID_INSTALLTABLES           3007

#define  MID_HELP                    4001
#define  MID_ABOUT                   4002

/* button IDs (should be disjoint from menu IDs) */
#define  BID_VIEWSOURCE                59
#define  BID_VIEWDESTINATION           60

/* dialog IDs */
#define  DID_NAME                     100
#define  DID_TYPE                     101
#define  DID_FONT                     102
#define  DID_TEXT                     103

#define  DID_CODEPAGELIST             300
#define  DID_RBUNICODE                301
#define  DID_RBANSICP                 302
#define  DID_RBOEMCP                  303
#define  DID_RBOTHERCP                304
#define  DID_CBBYTEORDER              305

#define  DID_PRECOMPOSED              401
#define  DID_COMPOSITE                402
#define  DID_USEGLYPHCHARS            403

#define  DID_COMPOSITECHECK           451
#define  DID_DISCARDNS                452
#define  DID_SEPCHARS                 453
#define  DID_DEFAULTCHAR              454
#define  DID_EFDEFAULTCHAR            455
#define  DID_USEDDEFAULTCHAR          456

/* Define a value for the LOGFONT.lfCharSet
 *  This should be included in wingdi.h, but it
 *  was removed because the font mapper is not
 *  using it anyway in version 1.0.  Currently
 *  scheduled to be included in NT ver 1.1.
 */
#define UNICODE_CHARSET  1
