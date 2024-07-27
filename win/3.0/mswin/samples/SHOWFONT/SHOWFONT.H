#define EOF                     -1
#define MAXFONT                 10
#define MAXSIZE                 20
#define ID_HEIGHT              101
#define ID_WIDTH               102
#define ID_ESCAPEMENT          103
#define ID_ORIENTATION         104
#define ID_FACE                105
#define ID_ITALIC              201
#define ID_UNDERLINE           202
#define ID_STRIKEOUT           203
#define ID_LIGHT               301
#define ID_NORMAL              302
#define ID_BOLD                303
#define ID_WEIGHT              304
#define ID_ANSI                401
#define ID_OEM                 402
#define ID_CHARSET             403
#define ID_OUT_STRING          501
#define ID_OUT_CHAR            502
#define ID_OUT_STROKE          503
#define ID_OUT_DEFAULT         504
#define ID_CLIP_CHAR           601
#define ID_CLIP_STROKE         602
#define ID_CLIP_DEFAULT        603
#define ID_PROOF               701
#define ID_DRAFT               702
#define ID_DEF_QUALITY         703
#define ID_FIXED               801
#define ID_VARIABLE            802
#define ID_DEF_PITCH           803
#define ID_ROMAN              1001
#define ID_SWISS              1002
#define ID_MODERN             1003
#define ID_SCRIPT             1004
#define ID_DECO               1005
#define ID_DEF_FAMILY         1006
#define ID_RED                 101
#define ID_GREEN               102
#define ID_BLUE                103
#define ID_TYPEFACE            101
#define ID_SIZE                102

/* Control IDs */

#define ID_FILENAME            400
#define ID_EDIT                401
#define ID_FILES               402
#define ID_PATH                403
#define ID_LISTBOX             405

#define IDM_ADDFONT            101
#define IDM_DELFONT            102
#define IDM_EXIT               103
#define IDM_ABOUT              104
#define IDM_SHOWSTRING         201
#define IDM_SHOWCHARSET        202
#define IDM_SHOWLOGFONT        203
#define IDM_SHOWTEXTMETRICS    204
#define IDM_CLEAR              205
#define IDM_SYSTEM             301
#define IDM_ANSIFIXED          302
#define IDM_ANSIVAR            303
#define IDM_OEM                304
#define IDM_DEVICEDEF          305
#define IDM_SELECTFONT         306
#define IDM_CFONT              307
#define IDM_TEXTCOLOR          401
#define IDM_BACKGROUNDCOLOR    402
#define IDM_OPAQUE             403
#define IDM_TRANSPARENT        404
#define IDM_ALIGNLEFT          405
#define IDM_ALIGNCENTER        406
#define IDM_ALIGNRIGHT         407
#define IDM_ALIGNTOP           408
#define IDM_ALIGNBASE          409
#define IDM_ALIGNBOTTOM        410

#define IDMB_HEIGHT            500
#define IDMB_ASCENT            501
#define IDMB_DESCENT           502
#define IDMB_WEIGHT            503
#define IDMB_ITALIC            504
#define IDMB_INTERNALLEADING   505
#define IDMB_EXTERNALLEADING   506
#define IDMB_AVECHARWIDTH      507
#define IDMB_MAXCHARWIDTH      508
#define IDMB_UNDERLINED        509
#define IDMB_STRUCKOUT         510
#define IDMB_FIRSTCHAR         511
#define IDMB_LASTCHAR          512
#define IDMB_DEFAULTCHAR       513
#define IDMB_BREAKCHAR         514
#define IDMB_PITCHANDFAMILY    515
#define IDMB_CHARSET           516
#define IDMB_OVERHANG          517
#define IDMB_DIGITIZEDASPECTX  518
#define IDMB_DIGITIZEDASPECTY  519

#define IDMI_HEIGHT            600
#define IDMI_WIDTH             601
#define IDMI_ESCAPEMENT        602
#define IDMI_ORIENTATION       603
#define IDMI_WEIGHT            604
#define IDMI_ITALIC            605
#define IDMI_UNDERLINED        606
#define IDMI_STRIKEOUT         607
#define IDMI_CHARSET           608
#define IDMI_OUTPRECISION      609
#define IDMI_CLIPPRECISION     510
#define IDMI_QUALITY           511
#define IDMI_PITCHANDFAMILY    512

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
long FAR PASCAL ShowFontWndProc(HWND, unsigned, WORD, LONG);
BOOL ShowFontInit(HANDLE);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL SelectFont(HWND, unsigned, WORD, LONG);
void GetSizes(HWND, int);
void GetFonts(HWND);
short GetStringExtent(HDC, PSTR, HFONT);
short StringOut(HDC, short, short, PSTR, HFONT);
void ShowString(HWND);
void ShowCharacterSet(HDC, HFONT);
void ShowLogFont(HWND, HFONT);
void ShowMetricFont(HWND, HFONT);
int FAR PASCAL EnumFunc(LPLOGFONT, LPTEXTMETRIC, short, LPSTR);
BOOL FAR PASCAL Metric(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL Log(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL AddDlg(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL RemoveDlg(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL CFontDlg(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL Colors(HWND, unsigned, WORD, LONG);
LOGFONT CLogFont;
void _lstrcpy(LPSTR, LPSTR);
void _lstrncpy(LPSTR, LPSTR, int);
int  _lstrlen(LPSTR);
BOOL CheckFileName(HWND, PSTR, PSTR);
BOOL SaveFile(HWND);
int  QuerySaveFile(HWND);
void SeparateFile(HWND, LPSTR, LPSTR, LPSTR);
void UpdateListBox(HWND);
void AddExt(PSTR, PSTR);
SetFaceName(HWND);
