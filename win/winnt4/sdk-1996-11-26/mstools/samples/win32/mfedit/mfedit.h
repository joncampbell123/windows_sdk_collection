/******************************Module*Header*******************************\
* Module Name: mfedit.h
*
* Header file for mfedit
*
* Copyright (c) 1996 Microsoft Corporation
*
* Contains all the definitions, global variables, structures, etc
*
* Dependencies:
*
\**************************************************************************/
#define LIGHTGRAY               RGB(192, 192, 192)
#define DARKGRAY                RGB(128, 128, 128)
#define MAX_POINTS              256
#define MAX_POINTS_MF           3
#define MAX_POINTS_BMP          3
#define MAX_FRAME               800
#define MAX_FILE                800
#define MAX_ERR_STRING          128

//
// String ID's
//
#define IDS_INITAPPFAIL		16
#define IDS_LOADACCFAIL		17
#define IDS_MFDEMOCL		18
#define IDS_NOMFFORCPY		19
#define IDS_CNVEMFTO3X		20
#define IDS_GETWMFBTSFAIL	21
#define IDS_MALLOCFAIL		22
#define IDS_GETWMFBTSFAIL2	23
#define IDS_MALLOCFAIL2		24
#define IDS_CPY3XMFTOCLP	25
#define IDS_GETWMFBTSFAIL2X	26
#define IDS_CNVFAIL		27
#define IDS_CPYEMFTOCLP		28
#define IDS_PST3XMF		29
#define IDS_NOINFO3XMFX		30

#define IDS_SAVEMF		32
#define IDS_FILEOPNFAIL		33
#define IDS_MAPFCREFAIL		34
#define IDS_MAPVWMAPFOFAIL	35
#define IDS_GET3XFRMCLPFAIL	36
#define IDS_PSTEMF		37
#define IDS_GETEMFFROMCLPFAIL	38
#define IDS_ERROR		39
#define IDS_NOMFLDFORHTST	40
#define IDS_DLGABOUTCREFAIL	41

#define IDS_LDMSKBM		43
#define IDS_LDBM		44
#define IDS_SVDRAWSFASBM	45
#define IDS_LDMF		46
#define IDS_MUSTESCHTSTMD	47
#define IDS_REC			48
#define IDS_CREMFDCFAIL		49
#define IDS_ADDCMNTFAIL		50

#define IDS_STOP		55
#define IDS_FMT_PLAYMF		56
#define IDS_CREPALFAIL		57

#define IDS_NOMFFORPLAY		61
#define IDS_ALDUSMF		62
#define IDS_DRAWSFCLR		63
#define IDS_PEN			64
#define IDS_TXT			65
#define IDS_RECT		66
#define IDS_FILLRECT		67
#define IDS_ELLIPSE		68
#define IDS_FILLELLIPSE		69
#define IDS_LN			70
#define IDS_MSTLBCLKFORBZ	71
#define IDS_MUST3CLKFORBM	72
#define IDS_MUST3CLKFORXMF	73
#define IDS_NOMFFORPRT		74

#define IDS_CREPRTTHRDFAIL	76
#define IDS_CREPNTFAIL		77
#define IDS_CREPBYTEFAIL	78
#define IDS_NOBMFOREMBED	79
#define IDS_NOMFFOREMBED	80
#define IDS_FT_DIB		81
#define IDS_FM_DIB		82
#define IDS_FT_RLE		83
#define IDS_FM_RLE		84
#define IDS_FT_EPS		85
#define IDS_FM_EPS		86
#define IDS_FT_EMF		87
#define IDS_FM_EMF		88
#define IDS_FT_WMF		89
#define IDS_FM_WMF		90

#define IDS_SAVEMFAS		92
#define IDS_LDMFMTFILEFAIL	93
#define IDS_CREMAPFILEFAIL	94
#define IDS_MAPVWOFMAPFOFAIL	95
#define IDS_NOTDIBBMFILE	96
#define IDS_GETMFBTSXFAIL	97
#define IDS_GETMFBTSXFAIL2	98
#define IDS_LDMSK		99
#define IDS_MALLOCFAILNOHIT	100

#define IDS_NULLRGNHTSTFAIL	102
#define IDS_EMF			103
#define IDS_NOHIT		104
#define IDS_NOMFTOPRT		105
#define IDS_ESCPRT		106
#define IDS_CREPRTDCFAIL	107
#define IDS_PRT			108

#define IDS_PRTTHRDDONE		111
#define IDS_ALLOCPALFAIL	112

#define IDS_HFTONEPALNULL	114
#define IDS_PALNULL		115
#define IDS_MUSTMSKMCHROMEBM	116
#define IDS_NOPALENTRYFORCPY	117
#define IDS_CPYPALFAIL		118

#define IDS_NULLPTR		121
#define IDS_NOTADOBE		122
#define IDS_SKPTOEOL		123
#define IDS_MUSTNOTEOF		124
#define IDS_GETBNDLFTFAIL	125
#define IDS_GETBNDTOPFAIL	126
#define IDS_GETBNDRGTFAIL	127
#define IDS_GETBNDBOTFAIL	128
#define IDS_SETADVGRPHMDFAIL	129

#define IDS_FMT_HIT		131
#define IDS_FMT_NOHTST		132
#define IDS_FMT_PLAYEMFFAIL	133
#define IDS_FMT_FILEOPNFAIL	134
#define IDS_FMT_CREMAPFILEFAIL	135
#define IDS_FMT_MAPVWMAPFOFAIL	136
#define IDS_FMT_SETWMFBTSFAIL	137
#define IDS_FMT_SETWMFBTSXFAIL	138
#define IDS_FMT_PLAYMFFAIL	139
#define IDS_FMT_GETMFFAIL	140

//
// Menu ID's
//
#define MM_LOAD                 7001
#define MM_RECORD               7002
#define MM_PRINT                7003
#define MM_PAGESETUP            7004
#define MM_PRINTSETUP           7005
#define MM_ABOUT                7006
#define MM_CUT                  7007
#define MM_COPY                 7008
#define MM_PASTE                7009
#define MM_DEL                  7010
#define MM_LOAD_BMP             7011
#define MM_SAVE_BMP             7012
#define MM_LOAD_MASKBMP         7013
#define MM_REMOVE               7014
#define MM_INSERT               7015
#define MM_HITTEST              7016
#define MM_REPEAT               7017
#define MM_INCREASE             7018
#define MM_NORMAL               7019
#define MM_DECREASE             7020
#define MM_PEN                  7021
#define MM_BRUSH                7022
#define MM_FONT                 7023
#define MM_LEABOUT              7024
#define MM_FIT2WND              7025
#define MM_IMPORT_3X            7026
#define MM_EXPORT_3X            7027
#define MM_TTOUTLN_STROKEFILL   7028
#define MM_TTOUTLN_POLYDRAW     7029
#define MM_C_WND_MF             7030
#define MM_C_BEGIN_GP           7031
#define MM_C_END_GP             7032
#define MM_C_MLTFMTS            7033

#define SIZEOFCAPTIONTEXT	20

#define ACCEL_ID                100
#define APP_ICON                 100

//
// Control Panel ID's
//
#define OD_BTN_CNT              5
#define DID_CTRLPANEL           100
#define DID_ZERO                1000
#define DID_ONE                 1001
#define DID_TWO                 1002
#define DID_THREE               1003
#define DID_FOUR                1004
#define DID_FIVE                1005
#define DID_SIX                 1006
#define DID_SEVEN               1007
#define DID_EIGHT               1008
#define DID_NINE                1009
#define DID_TEN_PLUS            1010
#define ID_OD_BTN_BASE          1011
#define DID_OPEN                1011
#define DID_RECORD              1012
#define DID_STOP                1013
#define DID_PLAY                1014
#define DID_FF                  1015
#define DID_CLEAR               1016
#define DID_COUNTER             1017
#define DID_TMP                 1018

//
// Control Panel's Owner draw bitmaps
//
#define BMID_BASED              1011
#define BMID_OPEND              1011
#define BMID_RECORDD            1012
#define BMID_STOPD              1013
#define BMID_PLAYD              1014
#define BMID_FFD                1015
#define BMID_BASEU              2011
#define BMID_OPENU              2011
#define BMID_RECORDU            2012
#define BMID_STOPU              2013
#define BMID_PLAYU              2014
#define BMID_FFU                2015

//
// Tools ID's
//
#define OD_TOOL_CNT             10
#define ID_OD_TOOL_BASE         3001
#define DID_PEN                 3001
#define DID_TEXT                3002
#define DID_RECT                3003
#define DID_FILLRECT            3004
#define DID_ELLIPSE             3005
#define DID_FILLELLIPSE         3006
#define DID_LINE                3007
#define DID_BEZIER              3008
#define DID_BMPOBJ              3009
#define DID_METAF               3010

//
// Tools Owner Draw bitmaps
//
#define BMID_TOOLBASED          3001
#define BMID_PEND               3001
#define BMID_TEXTD              3002
#define BMID_RECTD              3003
#define BMID_FILLRECTD          3004
#define BMID_ELLIPSED           3005
#define BMID_FILLELLIPSED       3006
#define BMID_LINED              3007
#define BMID_BEZIERD            3008
#define BMID_BMPOBJD            3009
#define BMID_METAFD             3010
#define BMID_TOOLBASEU          4001
#define BMID_PENU               4001
#define BMID_TEXTU              4002
#define BMID_RECTU              4003
#define BMID_FILLRECTU          4004
#define BMID_ELLIPSEU           4005
#define BMID_FILLELLIPSEU       4006
#define BMID_LINEU              4007
#define BMID_BEZIERU            4008
#define BMID_BMPOBJU            4009
#define BMID_METAFU             4010

//
// some handy macros
//
#define GET_WM_CTLCOLOR_HDC(wp, lp, msg)        (HDC)(wp)
#define GET_WM_CTLCOLOR_HWND(wp, lp, msg)       (HWND)(lp)
#define GET_WM_CTLCOLOR_TYPE(wp, lp, msg)       (WORD)(msg - WM_CTLCOLORMSGBOX)

#define META32_SIGNATURE        0x464D4520      // ' EMF'
#define ALDUS_ID		0x9AC6CDD7

#define CARET_HEIGHT    16

typedef struct
  {
  DWORD   key;
  WORD          hmf;
  SMALL_RECT    bbox;
  WORD    inch;
  DWORD   reserved;
  WORD    checksum;
  } APMFILEHEADER;
typedef APMFILEHEADER * PAPMFILEHEADER;
#define APMSIZE 22


#ifndef RC_INVOKED
#pragma pack(2)
typedef struct tagMETA16HEADER
{
    WORD	mtType;
    WORD	mtHeaderSize;
    WORD	mtVersion;
    DWORD	mtSize;
    WORD	mtNoObjects;
    DWORD	mtMaxRecord;
    WORD	mtNoParameters;
} META16HEADER, LPMETA16HEADER;
#pragma pack()

#endif // RC_INVOKED


typedef struct _playinfo {
    int iRecord;
    BOOL bPlayContinuous;
} PLAYINFO;

typedef struct _metalookup {
    LPSTR   szGDIName;
    int     iMRNumber;
} METALOOKUP;

typedef struct _htdata {
    POINT   point;
    int     iRecord;
} HTDATA;

METALOOKUP rgMetaName[] =
{
   { "                           ", 0                              },
   { "HEADER                     ", EMR_HEADER                     },
   { "POLYBEZIER                 ", EMR_POLYBEZIER                 },
   { "POLYGON                    ", EMR_POLYGON                    },
   { "POLYLINE                   ", EMR_POLYLINE                   },
   { "POLYBEZIERTO               ", EMR_POLYBEZIERTO               },
   { "POLYLINETO                 ", EMR_POLYLINETO                 },
   { "POLYPOLYLINE               ", EMR_POLYPOLYLINE               },
   { "POLYPOLYGON                ", EMR_POLYPOLYGON                },
   { "SETWINDOWEXTEX             ", EMR_SETWINDOWEXTEX             },
   { "SETWINDOWORGEX             ", EMR_SETWINDOWORGEX             },
   { "SETVIEWPORTEXTEX           ", EMR_SETVIEWPORTEXTEX           },
   { "SETVIEWPORTORGEX           ", EMR_SETVIEWPORTORGEX           },
   { "SETBRUSHORGEX              ", EMR_SETBRUSHORGEX              },
   { "EOF                        ", EMR_EOF                        },
   { "SETPIXELV                  ", EMR_SETPIXELV                  },
   { "SETMAPPERFLAGS             ", EMR_SETMAPPERFLAGS             },
   { "SETMAPMODE                 ", EMR_SETMAPMODE                 },
   { "SETBKMODE                  ", EMR_SETBKMODE                  },
   { "SETPOLYFILLMODE            ", EMR_SETPOLYFILLMODE            },
   { "SETROP2                    ", EMR_SETROP2                    },
   { "SETSTRETCHBLTMODE          ", EMR_SETSTRETCHBLTMODE          },
   { "SETTEXTALIGN               ", EMR_SETTEXTALIGN               },
   { "SETCOLORADJUSTMENT         ", EMR_SETCOLORADJUSTMENT         },
   { "SETTEXTCOLOR               ", EMR_SETTEXTCOLOR               },
   { "SETBKCOLOR                 ", EMR_SETBKCOLOR                 },
   { "OFFSETCLIPRGN              ", EMR_OFFSETCLIPRGN              },
   { "MOVETOEX                   ", EMR_MOVETOEX                   },
   { "SETMETARGN                 ", EMR_SETMETARGN                 },
   { "EXCLUDECLIPRECT            ", EMR_EXCLUDECLIPRECT            },
   { "INTERSECTCLIPRECT          ", EMR_INTERSECTCLIPRECT          },
   { "SCALEVIEWPORTEXTEX         ", EMR_SCALEVIEWPORTEXTEX         },
   { "SCALEWINDOWEXTEX           ", EMR_SCALEWINDOWEXTEX           },
   { "SAVEDC                     ", EMR_SAVEDC                     },
   { "RESTOREDC                  ", EMR_RESTOREDC                  },
   { "SETWORLDTRANSFORM          ", EMR_SETWORLDTRANSFORM          },
   { "MODIFYWORLDTRANSFORM       ", EMR_MODIFYWORLDTRANSFORM       },
   { "SELECTOBJECT               ", EMR_SELECTOBJECT               },
   { "CREATEPEN                  ", EMR_CREATEPEN                  },
   { "CREATEBRUSHINDIRECT        ", EMR_CREATEBRUSHINDIRECT        },
   { "DELETEOBJECT               ", EMR_DELETEOBJECT               },
   { "ANGLEARC                   ", EMR_ANGLEARC                   },
   { "ELLIPSE                    ", EMR_ELLIPSE                    },
   { "RECTANGLE                  ", EMR_RECTANGLE                  },
   { "ROUNDRECT                  ", EMR_ROUNDRECT                  },
   { "ARC                        ", EMR_ARC                        },
   { "CHORD                      ", EMR_CHORD                      },
   { "PIE                        ", EMR_PIE                        },
   { "SELECTPALETTE              ", EMR_SELECTPALETTE              },
   { "CREATEPALETTE              ", EMR_CREATEPALETTE              },
   { "SETPALETTEENTRIES          ", EMR_SETPALETTEENTRIES          },
   { "RESIZEPALETTE              ", EMR_RESIZEPALETTE              },
   { "REALIZEPALETTE             ", EMR_REALIZEPALETTE             },
   { "EXTFLOODFILL               ", EMR_EXTFLOODFILL               },
   { "LINETO                     ", EMR_LINETO                     },
   { "ARCTO                      ", EMR_ARCTO                      },
   { "POLYDRAW                   ", EMR_POLYDRAW                   },
   { "SETARCDIRECTION            ", EMR_SETARCDIRECTION            },
   { "SETMITERLIMIT              ", EMR_SETMITERLIMIT              },
   { "BEGINPATH                  ", EMR_BEGINPATH                  },
   { "ENDPATH                    ", EMR_ENDPATH                    },
   { "CLOSEFIGURE                ", EMR_CLOSEFIGURE                },
   { "FILLPATH                   ", EMR_FILLPATH                   },
   { "STROKEANDFILLPATH          ", EMR_STROKEANDFILLPATH          },
   { "STROKEPATH                 ", EMR_STROKEPATH                 },
   { "FLATTENPATH                ", EMR_FLATTENPATH                },
   { "WIDENPATH                  ", EMR_WIDENPATH                  },
   { "SELECTCLIPPATH             ", EMR_SELECTCLIPPATH             },
   { "ABORTPATH                  ", EMR_ABORTPATH                  },
   { "UNKNOWN                    ", 69                             },
   { "GDICOMMENT                 ", EMR_GDICOMMENT                 },
   { "FILLRGN                    ", EMR_FILLRGN                    },
   { "FRAMERGN                   ", EMR_FRAMERGN                   },
   { "INVERTRGN                  ", EMR_INVERTRGN                  },
   { "PAINTRGN                   ", EMR_PAINTRGN                   },
   { "EXTSELECTCLIPRGN           ", EMR_EXTSELECTCLIPRGN           },
   { "BITBLT                     ", EMR_BITBLT                     },
   { "STRETCHBLT                 ", EMR_STRETCHBLT                 },
   { "MASKBLT                    ", EMR_MASKBLT                    },
   { "PLGBLT                     ", EMR_PLGBLT                     },
   { "SETDIBITSTODEVICE          ", EMR_SETDIBITSTODEVICE          },
   { "STRETCHDIBITS              ", EMR_STRETCHDIBITS              },
   { "EXTCREATEFONTINDIRECTW     ", EMR_EXTCREATEFONTINDIRECTW     },
   { "EXTTEXTOUTA                ", EMR_EXTTEXTOUTA                },
   { "EXTTEXTOUTW                ", EMR_EXTTEXTOUTW                },
   { "POLYBEZIER16               ", EMR_POLYBEZIER16               },
   { "POLYGON16                  ", EMR_POLYGON16                  },
   { "POLYLINE16                 ", EMR_POLYLINE16                 },
   { "POLYBEZIERTO16             ", EMR_POLYBEZIERTO16             },
   { "POLYLINETO16               ", EMR_POLYLINETO16               },
   { "POLYPOLYLINE16             ", EMR_POLYPOLYLINE16             },
   { "POLYPOLYGON16              ", EMR_POLYPOLYGON16              },
   { "POLYDRAW16                 ", EMR_POLYDRAW16                 },
   { "CREATEMONOBRUSH            ", EMR_CREATEMONOBRUSH            },
   { "CREATEDIBPATTERNBRUSHPT    ", EMR_CREATEDIBPATTERNBRUSHPT    },
   { "EXTCREATEPEN               ", EMR_EXTCREATEPEN               },
   { "POLYTEXTOUTA               ", EMR_POLYTEXTOUTA               },
   { "POLYTEXTOUTW               ", EMR_POLYTEXTOUTW               },
   { "SETICMMODE                 ", EMR_SETICMMODE                 },
   { "CREATECOLORSPACE           ", EMR_CREATECOLORSPACE           },
   { "SETCOLORSPACE              ", EMR_SETCOLORSPACE              },
   { "DELETECOLORSPACE           ", EMR_DELETECOLORSPACE           },
#ifdef OPENGL_EMF
   { "GLSRECORD                  ", EMR_GLSRECORD                  },
   { "GLSBOUNDEDRECORD           ", EMR_GLSBOUNDEDRECORD           },
   { "PIXELFORMAT                ", EMR_PIXELFORMAT                },
#endif
};

typedef struct _PRTDATA {
    BOOL            bFit2Wnd;
    HENHMETAFILE    hMetaf;
} PRTDATA, *PPRTDATA;

PROC    gpfnGetEnhMetaFilePixelFormat = (PROC)NULL;

HANDLE  ghModule;
HWND    ghwndMain   = NULL;
HWND    ghwndClient = NULL;
HWND    ghwndDrawSurf = NULL;
HWND    ghwndCtrlPanel = NULL;
HWND    ghwndTools = NULL;
HWND    ghTextWnd = NULL;
HPALETTE ghPal = NULL;
HPALETTE ghHT = NULL;

LONG    glcyStatus;
BOOL    gbRecording = FALSE;
BOOL    gbHitTest = FALSE;

#if 0   // STRICT doesn't like this
HWND    grHwndCtrlBtn[OD_BTN_CNT];
HWND    grHwndToolBtn[OD_TOOL_CNT];
#endif
PVOID   grHwndCtrlBtn[OD_BTN_CNT];
PVOID   grHwndToolBtn[OD_TOOL_CNT];

DWORD   gdwCurCtrl = DID_STOP;
DWORD   gdwCurTool = DID_PEN;

HFONT    ghCurFont = (HFONT)NULL;
LOGFONT  glf;
COLORREF gCrText=0;                         // Black

HMENU   hMenu,      hMenuWindow;
HMENU   hChildMenu, hChildMenuWindow;
HDC     ghDCMem;
HDC     ghDCMetaf = (HDC) NULL;
HENHMETAFILE ghMetaf = NULL;
HMETAFILE ghmf=NULL;

HBITMAP ghBmp = NULL, ghBmpMask = NULL;

#if 0   // STRICT doesn't like this
HBITMAP ghBmpDn[OD_BTN_CNT], ghBmpUp[OD_BTN_CNT];
HBITMAP ghToolBmpDn[OD_TOOL_CNT], ghToolBmpUp[OD_TOOL_CNT];
#endif
PVOID   ghBmpDn[OD_BTN_CNT], ghBmpUp[OD_BTN_CNT];
PVOID   ghToolBmpDn[OD_TOOL_CNT], ghToolBmpUp[OD_TOOL_CNT];

HBRUSH  ghbrRed;
HBRUSH  ghbrCur = (HBRUSH)NULL;
HPEN    ghpnCur = (HPEN)NULL;
HBRUSH  ghbrAppBkgd;
HPEN    ghpnWide;

HWND    ghwndNext = NULL;
BOOL    gbFit2Wnd = TRUE;
BOOL    gbImport3X = FALSE;
BOOL    gbExport3X = FALSE;
BOOL    gbSFOutln = FALSE;
BOOL    gbPDOutln = FALSE;
BOOL    gbTT = FALSE;
BOOL    gb3D = FALSE;
BOOL    gbRCSet = FALSE;
BOOL    gbDebug = FALSE;
BOOL    gbUseMfPFD = TRUE;
BOOL    gbDB = FALSE;

BOOL    gbUseDIB = FALSE;

typedef struct _FileInfo{
    HANDLE      hFile;
    HANDLE      hMapFile;
    LPVOID      lpvMapView;
} FILEINFO, *PFILEINFO;

typedef struct _DIBDATA{
    ULONG           ulFiles;
    ULONG           ulFrames;
    FILEINFO        rgFileInfo[MAX_FILE];
    PBYTE           rgpjFrame[MAX_FRAME];
    PBITMAPINFO     rgpbmi[MAX_FILE];
    BOOL            rgbCoreHdr[MAX_FILE];
} DIBDATA, *PDIBDATA;

DIBDATA gDib;

typedef struct tagCMTMLTFMT {
    DWORD       ident;
    DWORD       iComment;
    RECTL       rclOutput;
    DWORD       nFormats;
    EMRFORMAT   aemrformat[1];
} CMTMLTFMT;

#define GDICOMMENT_IDENTIFIER   0x43494447
#define GDICOMMENT_MULTIFORMATS 0x40000004

#define BLACK               PALETTERGB(0,0,0)
#define WHITE               PALETTERGB(255,255,255)
#define NUM_STATIC_COLORS   (COLOR_BTNHIGHLIGHT - COLOR_SCROLLBAR + 1)

// Maximum color distance with 8-bit components
#define MAX_COL_DIST (3*256*256L)

// Number of static colors
#define STATIC_COLORS 20

// Flags used when matching colors
#define EXACT_MATCH 1
#define COLOR_USED 1

// TRUE if app wants to take over palette
static BOOL gbUseStaticColors = FALSE;

// TRUE if static system color settings have been replaced with B&W settings.
static BOOL gbSystemColorsInUse = FALSE;

// TRUE if static colors have been saved
static BOOL gbStaticSaved = FALSE;

// saved system static colors
static COLORREF gacrSave[NUM_STATIC_COLORS];

// new B&W system static colors
static COLORREF gacrBlackAndWhite[NUM_STATIC_COLORS] = {
    WHITE,  // COLOR_SCROLLBAR
    BLACK,  // COLOR_BACKGROUND
    BLACK,  // COLOR_ACTIVECAPTION
    WHITE,  // COLOR_INACTIVECAPTION
    WHITE,  // COLOR_MENU
    WHITE,  // COLOR_WINDOW
    BLACK,  // COLOR_WINDOWFRAME
    BLACK,  // COLOR_MENUTEXT
    BLACK,  // COLOR_WINDOWTEXT
    WHITE,  // COLOR_CAPTIONTEXT
    WHITE,  // COLOR_ACTIVEBORDER
    WHITE,  // COLOR_INACTIVEBORDER
    WHITE,  // COLOR_APPWORKSPACE
    BLACK,  // COLOR_HIGHLIGHT
    WHITE,  // COLOR_HIGHLIGHTTEXT
    WHITE,  // COLOR_BTNFACE
    BLACK,  // COLOR_BTNSHADOW
    BLACK,  // COLOR_GRAYTEXT
    BLACK,  // COLOR_BTNTEXT
    BLACK,  // COLOR_INACTIVECAPTIONTEXT
    BLACK   // COLOR_BTNHIGHLIGHT
    };
static INT gaiStaticIndex[NUM_STATIC_COLORS] = {
    COLOR_SCROLLBAR          ,
    COLOR_BACKGROUND         ,
    COLOR_ACTIVECAPTION      ,
    COLOR_INACTIVECAPTION    ,
    COLOR_MENU               ,
    COLOR_WINDOW             ,
    COLOR_WINDOWFRAME        ,
    COLOR_MENUTEXT           ,
    COLOR_WINDOWTEXT         ,
    COLOR_CAPTIONTEXT        ,
    COLOR_ACTIVEBORDER       ,
    COLOR_INACTIVEBORDER     ,
    COLOR_APPWORKSPACE       ,
    COLOR_HIGHLIGHT          ,
    COLOR_HIGHLIGHTTEXT      ,
    COLOR_BTNFACE            ,
    COLOR_BTNSHADOW          ,
    COLOR_GRAYTEXT           ,
    COLOR_BTNTEXT            ,
    COLOR_INACTIVECAPTIONTEXT,
    COLOR_BTNHIGHLIGHT
    };

static VOID SaveStaticEntries(HDC);
static VOID UseStaticEntries(HDC);

//
// Forward declarations.
//
BOOL InitializeApp   (void);
LONG APIENTRY MainWndProc     (HWND, UINT, DWORD, LONG);
LONG APIENTRY DrawSurfWndProc (HWND, UINT, DWORD, LONG);
BOOL CALLBACK About           (HWND, UINT, DWORD, LONG);
LONG APIENTRY TextWndProc     (HWND, UINT, DWORD, LONG);
LONG APIENTRY CtrlPanelDlgProc(HWND, UINT, DWORD, LONG);
BOOL bDrawStuff      (HDC, INT, INT, INT, INT, BOOL, BOOL, BOOL, LPSTR);
HENHMETAFILE hemfLoadMetafile(HWND);
HDC  hDCRecordMetafileAs(HWND, LPSTR);
BOOL APIENTRY bPlayRecord(HDC, LPHANDLETABLE, LPENHMETARECORD, UINT, LPVOID);
BOOL APIENTRY bDoHitTest(HDC, LPHANDLETABLE, LPENHMETARECORD, UINT, LPVOID);
BOOL bHitTest(HDC, INT, INT);
HBITMAP hBmpLoadBitmapFile(HDC, PSTR);
BOOL bGetBMP(HWND, BOOL);
BOOL bChooseNewFont(HWND, PLOGFONT, COLORREF * );
BOOL bChooseNewColor(HWND, LPDWORD);
BOOL bPrintMf(PPRTDATA);
HBRUSH hBrCreateBrush(HDC, DWORD);
BOOL bSelectDIBPal(HDC, LPBITMAPINFO, BOOL);
BOOL bFreeDibFile(PDIBDATA);
BOOL bPlgBlt(HDC, LPPOINT);
HPALETTE CopyPalette(HPALETTE hPalSrc);
int CALLBACK iTT(LPLOGFONT, LPTEXTMETRIC, DWORD, LPARAM);
CMTMLTFMT *pLoadMltFmtFile(VOID);
HLOCAL Free(CMTMLTFMT *pMfmt);
BOOL bGetEPSBounds(LPVOID, RECTL *);
BOOL bIsAdobe(char *szStr);
BOOL bIsEPS(char *szStr);
BOOL bIsBndBox(char *szStr);
BOOL bIsEOF(char *szStr);
//BOOL bGetWord(LPVOID, char *, char **);
//BOOL bGoNextLine(LPVOID, char **);
BOOL bGetWord(LPVOID, char *, int*);
BOOL bGoNextLine(LPVOID, int*);
BOOL bSetAdvancedGraphics(HDC hdc);

extern unsigned char __stdcall ComponentFromIndex(INT, INT, INT);
extern int aiDefaultOverride[20];
extern PALETTEENTRY apeDefaultPalEntry[20];

BOOL bCleanUpRC(VOID);
BOOL bSetupRC(HDC hDC, PIXELFORMATDESCRIPTOR *ppfdIn);

//
// function prototype for looking up string resources
//

LPTSTR GetStringRes (int);
LPTSTR GetStringRes2 (int);
LPTSTR BuildFilterStrs(int);
