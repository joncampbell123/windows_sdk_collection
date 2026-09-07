/******************************Module*Header*******************************\
* Module Name: mfedit.h
*
* Header file for mfedit
*
* Copyright (C) 1993-1995 Microsoft Corporation
*
* Contains all the definitions, global variables, structures, etc
*
* Dependencies:
*
*   metadef.h
*
\**************************************************************************/
#include <windows.h>
#include <string.h>

#define LIGHTGRAY               RGB(192, 192, 192)
#define DARKGRAY                RGB(128, 128, 128)
#define MAX_POINTS              256
#define MAX_POINTS_MF           3
#define MAX_POINTS_BMP          3
#define MAX_FRAME           800
#define MAX_FILE            800

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

#define MR_MIN			    1

#define MR_METAFILE		    1
#define MR_POLYBEZIER		    2
#define MR_POLYGON		    3
#define MR_POLYLINE		    4
#define MR_POLYBEZIERTO		    5
#define MR_POLYLINETO		    6
#define MR_POLYPOLYLINE		    7
#define MR_POLYPOLYGON		    8
#define MR_SETWINDOWEXTEX	    9
#define MR_SETWINDOWORGEX	    10
#define MR_SETVIEWPORTEXTEX	    11
#define MR_SETVIEWPORTORGEX	    12
#define MR_SETBRUSHORGEX	    13
#define MR_EOF			    14
#define MR_SETPIXELV		    15
#define MR_SETMAPPERFLAGS	    16
#define MR_SETMAPMODE		    17
#define MR_SETBKMODE		    18
#define MR_SETPOLYFILLMODE	    19
#define MR_SETROP2		    20
#define MR_SETSTRETCHBLTMODE	    21
#define MR_SETTEXTALIGN		    22

#define MR_SETTEXTCOLOR		    24
#define MR_SETBKCOLOR		    25
#define MR_OFFSETCLIPRGN	    26
#define MR_MOVETOEX		    27
#define MR_SETMETARGN		    28
#define MR_EXCLUDECLIPRECT	    29
#define MR_INTERSECTCLIPRECT	    30
#define MR_SCALEVIEWPORTEXTEX	    31
#define MR_SCALEWINDOWEXTEX	    32
#define MR_SAVEDC		    33
#define MR_RESTOREDC		    34
#define MR_SETWORLDTRANSFORM	    35
#define MR_MODIFYWORLDTRANSFORM	    36
#define MR_SELECTOBJECT		    37
#define MR_CREATEPEN		    38
#define MR_CREATEBRUSHINDIRECT	    39
#define MR_DELETEOBJECT		    40
#define MR_ANGLEARC		    41
#define MR_ELLIPSE		    42
#define MR_RECTANGLE		    43
#define MR_ROUNDRECT		    44
#define MR_ARC			    45
#define MR_CHORD		    46
#define MR_PIE			    47
#define MR_SELECTPALETTE	    48
#define MR_CREATEPALETTE	    49
#define MR_SETPALETTEENTRIES	    50
#define MR_RESIZEPALETTE	    51
#define MR_REALIZEPALETTE	    52
#define MR_EXTFLOODFILL		    53
#define MR_LINETO		    54
#define MR_ARCTO		    55
#define MR_POLYDRAW		    56
#define MR_SETARCDIRECTION	    57
#define MR_SETMITERLIMIT	    58
#define MR_BEGINPATH		    59
#define MR_ENDPATH		    60
#define MR_CLOSEFIGURE		    61
#define MR_FILLPATH		    62
#define MR_STROKEANDFILLPATH	    63
#define MR_STROKEPATH		    64
#define MR_FLATTENPATH		    65
#define MR_WIDENPATH		    66
#define MR_SELECTCLIPPATH	    67
#define MR_ABORTPATH		    68

#define MR_GDICOMMENT		    70
#define MR_FILLRGN		    71
#define MR_FRAMERGN		    72
#define MR_INVERTRGN		    73
#define MR_PAINTRGN		    74
#define MR_EXTSELECTCLIPRGN	    75
#define MR_BITBLT		    76
#define MR_STRETCHBLT		    77
#define MR_MASKBLT		    78
#define MR_PLGBLT		    79
#define MR_SETDIBITSTODEVICE	    80
#define MR_STRETCHDIBITS	    81
#define MR_EXTCREATEFONTINDIRECTW   82
#define MR_EXTTEXTOUTA   	    83
#define MR_EXTTEXTOUTW   	    84
#define MR_POLYBEZIER16		    85
#define MR_POLYGON16		    86
#define MR_POLYLINE16		    87
#define MR_POLYBEZIERTO16	    88
#define MR_POLYLINETO16		    89
#define MR_POLYPOLYLINE16	    90
#define MR_POLYPOLYGON16	    91
#define MR_POLYDRAW16		    92
#define MR_CREATEMONOBRUSH	    93
#define MR_CREATEDIBPATTERNBRUSHPT  94
#define MR_EXTCREATEPEN		    95
#define MR_MAX			    95


//
// some handy macros
//
#define GET_WM_CTLCOLOR_HDC(wp, lp, msg)        (HDC)(wp)
#define GET_WM_CTLCOLOR_HWND(wp, lp, msg)       (HWND)(lp)
#define GET_WM_CTLCOLOR_TYPE(wp, lp, msg)       (WORD)(msg - WM_CTLCOLORMSGBOX)

#define META32_SIGNATURE        0x464D4520      // ' EMF'
#define ALDUS_ID		0x9AC6CDD7

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
   { "                          ", 0                         },
   { "METAFILE                  ", MR_METAFILE               },
   { "POLYBEZIER                ", MR_POLYBEZIER             },
   { "POLYGON                   ", MR_POLYGON                },
   { "POLYLINE                  ", MR_POLYLINE               },
   { "POLYBEZIERTO              ", MR_POLYBEZIERTO           },
   { "POLYLINETO                ", MR_POLYLINETO             },
   { "POLYPOLYLINE              ", MR_POLYPOLYLINE           },
   { "POLYPOLYGON               ", MR_POLYPOLYGON            },
   { "SETWINDOWEXTEX            ", MR_SETWINDOWEXTEX         },
   { "SETWINDOWORGEX            ", MR_SETWINDOWORGEX         },
   { "SETVIEWPORTEXTEX          ", MR_SETVIEWPORTEXTEX       },
   { "SETVIEWPORTORGEX          ", MR_SETVIEWPORTORGEX       },
   { "SETBRUSHORGEX             ", MR_SETBRUSHORGEX          },
   { "EOF                       ", MR_EOF                    },
   { "SETPIXELV                 ", MR_SETPIXELV              },
   { "SETMAPPERFLAGS            ", MR_SETMAPPERFLAGS         },
   { "SETMAPMODE                ", MR_SETMAPMODE             },
   { "SETBKMODE                 ", MR_SETBKMODE              },
   { "SETPOLYFILLMODE           ", MR_SETPOLYFILLMODE        },
   { "SETROP2                   ", MR_SETROP2                },
   { "SETSTRETCHBLTMODE         ", MR_SETSTRETCHBLTMODE      },
   { "SETTEXTALIGN              ", MR_SETTEXTALIGN           },
   { "UNKNOWN                   ", 23                        },
   { "SETTEXTCOLOR              ", MR_SETTEXTCOLOR           },
   { "SETBKCOLOR                ", MR_SETBKCOLOR             },
   { "OFFSETCLIPRGN             ", MR_OFFSETCLIPRGN          },
   { "MOVETOEX                  ", MR_MOVETOEX               },
   { "SETMETARGN                ", MR_SETMETARGN             },
   { "EXCLUDECLIPRECT           ", MR_EXCLUDECLIPRECT        },
   { "INTERSECTCLIPRECT         ", MR_INTERSECTCLIPRECT      },
   { "SCALEVIEWPORTEXTEX        ", MR_SCALEVIEWPORTEXTEX     },
   { "SCALEWINDOWEXTEX          ", MR_SCALEWINDOWEXTEX       },
   { "SAVEDC                    ", MR_SAVEDC                 },
   { "RESTOREDC                 ", MR_RESTOREDC              },
   { "SETWORLDTRANSFORM         ", MR_SETWORLDTRANSFORM      },
   { "MODIFYWORLDTRANSFORM      ", MR_MODIFYWORLDTRANSFORM   },
   { "SELECTOBJECT              ", MR_SELECTOBJECT           },
   { "CREATEPEN                 ", MR_CREATEPEN              },
   { "CREATEBRUSHINDIRECT       ", MR_CREATEBRUSHINDIRECT    },
   { "DELETEOBJECT              ", MR_DELETEOBJECT           },
   { "ANGLEARC                  ", MR_ANGLEARC               },
   { "ELLIPSE                   ", MR_ELLIPSE                },
   { "RECTANGLE                 ", MR_RECTANGLE              },
   { "ROUNDRECT                 ", MR_ROUNDRECT              },
   { "ARC                       ", MR_ARC                    },
   { "CHORD                     ", MR_CHORD                  },
   { "PIE                       ", MR_PIE                    },
   { "SELECTPALETTE             ", MR_SELECTPALETTE          },
   { "CREATEPALETTE             ", MR_CREATEPALETTE          },
   { "SETPALETTEENTRIES         ", MR_SETPALETTEENTRIES      },
   { "RESIZEPALETTE             ", MR_RESIZEPALETTE          },
   { "REALIZEPALETTE            ", MR_REALIZEPALETTE         },
   { "EXTFLOODFILL              ", MR_EXTFLOODFILL           },
   { "LINETO                    ", MR_LINETO                 },
   { "ARCTO                     ", MR_ARCTO                  },
   { "POLYDRAW                  ", MR_POLYDRAW               },
   { "SETARCDIRECTION           ", MR_SETARCDIRECTION        },
   { "SETMITERLIMIT             ", MR_SETMITERLIMIT          },
   { "BEGINPATH                 ", MR_BEGINPATH              },
   { "ENDPATH                   ", MR_ENDPATH                },
   { "CLOSEFIGURE               ", MR_CLOSEFIGURE            },
   { "FILLPATH                  ", MR_FILLPATH               },
   { "STROKEANDFILLPATH         ", MR_STROKEANDFILLPATH      },
   { "STROKEPATH                ", MR_STROKEPATH             },
   { "FLATTENPATH               ", MR_FLATTENPATH            },
   { "WIDENPATH                 ", MR_WIDENPATH              },
   { "SELECTCLIPPATH            ", MR_SELECTCLIPPATH         },
   { "ABORTPATH                 ", MR_ABORTPATH              },
   { "UNKNOWN                   ", 69                        },
   { "GDICOMMENT                ", MR_GDICOMMENT             },
   { "FILLRGN                   ", MR_FILLRGN                },
   { "FRAMERGN                  ", MR_FRAMERGN               },
   { "INVERTRGN                 ", MR_INVERTRGN              },
   { "PAINTRGN                  ", MR_PAINTRGN               },
   { "EXTSELECTCLIPRGN          ", MR_EXTSELECTCLIPRGN       },
   { "BITBLT                    ", MR_BITBLT                 },
   { "STRETCHBLT                ", MR_STRETCHBLT             },
   { "MASKBLT                   ", MR_MASKBLT                },
   { "PLGBLT                    ", MR_PLGBLT                 },
   { "SETDIBITSTODEVICE         ", MR_SETDIBITSTODEVICE      },
   { "STRETCHDIBITS             ", MR_STRETCHDIBITS          },
   { "EXTCREATEFONTINDIRECTW    ", MR_EXTCREATEFONTINDIRECTW },
   { "EXTTEXTOUTA               ", MR_EXTTEXTOUTA            },
   { "EXTTEXTOUTW               ", MR_EXTTEXTOUTW            },
   { "POLYBEZIER16              ", MR_POLYBEZIER16           },
   { "POLYGON16                 ", MR_POLYGON16              },
   { "POLYLINE16                ", MR_POLYLINE16             },
   { "POLYBEZIERTO16            ", MR_POLYBEZIERTO16         },
   { "POLYLINETO16              ", MR_POLYLINETO16           },
   { "POLYPOLYLINE16            ", MR_POLYPOLYLINE16         },
   { "POLYPOLYGON16             ", MR_POLYPOLYGON16          },
   { "POLYDRAW16                ", MR_POLYDRAW16             },
   { "CREATEMONOBRUSH           ", MR_CREATEMONOBRUSH        },
   { "CREATEDIBPATTERNBRUSHPT   ", MR_CREATEDIBPATTERNBRUSHPT},
   { "EXTCREATEPEN              ", MR_EXTCREATEPEN           },
   };


typedef struct _PRTDATA {
    BOOL            bFit2Wnd;
    HENHMETAFILE    hMetaf;
} PRTDATA, *PPRTDATA;


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

HFONT    ghCurFont;
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
HBRUSH  ghbrCur;
HPEN    ghpnCur;
HBRUSH  ghbrAppBkgd;
HPEN    ghpnWide;

HWND    ghwndNext = NULL;
BOOL    gbFit2Wnd = TRUE;
BOOL    gbImport3X = FALSE;
BOOL    gbExport3X = FALSE;
BOOL    gbSFOutln = FALSE;
BOOL    gbPDOutln = FALSE;
BOOL    gbTT = FALSE;

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
