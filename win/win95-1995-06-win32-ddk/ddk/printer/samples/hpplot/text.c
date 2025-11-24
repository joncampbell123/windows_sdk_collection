/* (c) Copyright 1987 MICROGRAFX, Inc., All Rights Reserved.  Permission to
   use this work for any purpose must be obtained in writing from MICROGRAFX,
   1820 N. Greenville Ave., Richardson, Tx.  75081.

*******************************************************************************
*******************************************************************************

                                   text

*******************************************************************************
*******************************************************************************

*/

#define PRINTDRIVER
#define NOPQ
#define NOINSTALLFILES
#include <print.h>
#include <gdidefs.inc>

#include "device.h"
#include "driver.h"
#include "control.h"
#include "output.h"
#include "print.h"
#include "text.h"
#include "utils.h"
/* ****************************** History ********************************** */
/* 10/31/87 (RWM) - signoff                                                  */
/* ***************************** Constants ********************************* */

#define LOCAL static
/* **************************** Local Data ********************************* */

BYTE CharSetSeven [95] =
{
    56,                 /* (161) Upside down exclamation */
    63,                 /* (162) Cent sign */
    47,                 /* (163) */
    58,                 /* (164) */
    60,                 /* (165) Y with equal sign */
    32,                 /* (166) */
    61,                 /* (167) Double S */
    43,                 /* (168) Two dots */
    32,                 /* (169) Copyright */
    121,                /* (170) 'a' underlined */
    123,                /* (171) Two less thans */
    32,                 /* (172) */
    118,                /* (173) dash */
    32,                 /* (174) Registered */
    48,                 /* (175) Overline */
    51,                 /* (176) Single dot */
    126,                /* (177) +/- sign */
    32,                 /* (178) Power of 2 */
    32,                 /* (179) Power of 3 */
    40,                 /* (180) accent to right */
    32,                 /* (181) */
    32,                 /* (182) Paragraph */
    32,                 /* (183) Solid single dot */
    32,                 /* (184) */
    32,                 /* (185) Power of 1 */
    122,                /* (186) 'o' underlined */
    125,                /* (187) Two greater thans */
    119,                /* (188) 1/4 */
    120,                /* (189) 1/2 */
    32,                 /* (190) 3/4 */
    57,                 /* (191) Upside down question mark */
    33,                 /* (192) 'A' accent left */
    96,                 /* (193) 'A' accent right */
    34,                 /* (194) 'A' carrot */
    97,                 /* (195) 'A' tilda */
    88,                 /* (196) 'A' two dots */
    80,                 /* (197) 'A' single dot */
    83,                 /* (198) 'AE' combination */
    52,                 /* (199) 'C' with beard */
    35,                 /* (200) 'E' accent left */
    92,                 /* (201) 'E' accent right */
    36,                 /* (202) 'E' carrot */
    37,                 /* (203) 'E' two dots */
    102,                /* (204) 'I' accent left */
    101,                /* (205) 'I' accent right */
    38,                 /* (206) 'I' carrot */
    39,                 /* (207) 'I' two dots */
    99,                 /* (208) 'D' with dash */
    54,                 /* (209) 'N' with tilda */
    104,                /* (210) 'O' accent left */
    103,                /* (211) 'O' accent right */
    32,                 /* (212) 'O' carrot */
    105,                /* (213) 'O' tilda */
    90,                 /* (214) 'O' two dots */
    32,                 /* (215) */
    82,                 /* (216) 'O' slashed */
    45,                 /* (217) 'U' accent left */
    32,                 /* (218) 'U' accent right */
    46,                 /* (219) 'U' carrot */
    91,                 /* (220) 'U' two dots */
    32,                 /* (221) 'Y' accent right */
    112,                /* (222) */
    94,                 /* (223) */
    72,                 /* (224) 'a' accent left */
    68,                 /* (225) 'a' accent right */
    64,                 /* (226) 'a' carrot */
    98,                 /* (227) 'a' tilda */
    76,                 /* (228) 'a' two dots */
    84,                 /* (229) 'a' single dot */
    87,                 /* (230) 'ae' combination */
    53,                 /* (231) 'c' with beard */
    73,                 /* (232) 'e' accent left */
    69,                 /* (233) 'e' accent right */
    65,                 /* (234) 'e' carrot */
    77,                 /* (235) 'e' two dots */
    89,                 /* (236) 'i' accent left */
    85,                 /* (237) 'i' accent right */
    81,                 /* (238) 'i' carrot */
    93,                 /* (239) 'i' two dots */
    32,                 /* (240) */
    55,                 /* (241) 'n' tilda */
    74,                 /* (242) 'o' accent left */
    70,                 /* (243) 'o' accent right */
    66,                 /* (244) 'o' carrot */
    106,                /* (245) 'o' tilda */
    78,                 /* (246) 'o' two dots */
    32,                 /* (247) */
    86,                 /* (248) 'o' slashed */
    75,                 /* (249) 'u' accent left */
    71,                 /* (250) 'u' accent right */
    67,                 /* (251) 'u' carrot */
    79,                 /* (252) 'u' two dots */
    32,                 /* (253) 'y' accent right */
    112,                /* (254) */
    111                 /* (255) 'y' two dots */
};

char MixedCharSet [95] [4] =
{
    { 4, 92, -1 },      /* (161) Upside down exclamation */
    { 2, 32, -1 },      /* (162) Cent sign */
    { 2, 35, -1},       /* (163) */
    { 2, 32, -1 },      /* (164) */
    { 2, 32, -1 },      /* (165) Y with equal sign */
    { 2, 32, -1 },      /* (166) */
    { 2, 32, -1 },      /* (167) Double S */
    { 2, 32, 2, 123 },  /* (168) Two dots */
    { 2, 32, -1 },      /* (169) Copyright */
    { 1, 97, 1, 95 },   /* (170) 'a' underlined */
    { 2, 32, -1 },      /* (171) Two less thans */
    { 2, 32, -1 },      /* (172) */
    { 0, 45, -1 },      /* (173) dash */
    { 2, 32, -1 },      /* (174) Registered */
    { 2, 32, -1 },      /* (175) Overline */
    { 2, 124, -1 },     /* (176) Single dot */
    { 2, 32, -1 },      /* (177) +/- sign */
    { 2, 32, -1 },      /* (178) Power of 2 */
    { 2, 32, -1 },      /* (179) Power of 3 */
    { 2, 32, 2, 39 },   /* (180) accent to right */
    { 2, 32, -1 },      /* (181) */
    { 2, 32, -1 },      /* (182) Paragraph */
    { 3, 32, 3, 126 },  /* (183) Solid single dot */
    { 2, 32, -1 },      /* (184) */
    { 2, 32, -1 },      /* (185) Power of 1 */
    { 1, 111, 1, 95 },  /* (186) 'o' underlined */
    { 2, 32, -1 },      /* (187) Two greater thans */
    { 2, 32, -1 },      /* (188) 1/4 */
    { 2, 32, -1 },      /* (189) 1/2 */
    { 2, 32, -1 },      /* (190) 3/4 */
    { 2, 32, -1 },      /* (191) Upside down question mark */
    { 1, 65, 1, 96 },   /* (192) 'A' accent left */
    { 2, 65, 2, 39 },   /* (193) 'A' accent right */
    { 2, 65, 2, 94 },   /* (194) 'A' carrot */
    { 1, 65, 1, 126 },  /* (195) 'A' tilda */
    { 2, 65, 2, 123 },  /* (196) 'A' two dots */
    { 2, 65, 2, 124 },  /* (197) 'A' single dot */
    { 3, 92, -1 },      /* (198) 'AE' combination */
    { 2, 32, -1 },      /* (199) 'C' with beard */
    { 1, 69, 1, 96 },   /* (200) 'E' accent left */
    { 2, 69, 2, 39 },   /* (201) 'E' accent right */
    { 2, 69, 2, 94 },   /* (202) 'E' carrot */
    { 2, 69, 2, 123 },  /* (203) 'E' two dots */
    { 1, 73, 1, 96 },   /* (204) 'I' accent left */
    { 2, 73, 2, 39 },   /* (205) 'I' accent right */
    { 2, 73, 2, 94 },   /* (206) 'I' carrot */
    { 2, 73, 2, 123 },  /* (207) 'I' two dots */
    { 2, 32, -1 },      /* (208) 'D' with dash */
    { 1, 78, 1, 126 },  /* (209) 'N' with tilda */
    { 1, 79, 1, 96 },   /* (210) 'O' accent left */
    { 2, 79, 2, 39 },   /* (211) 'O' accent right */
    { 2, 79, 2, 94 },   /* (212) 'O' carrot */
    { 1, 79, 1, 126 },  /* (213) 'O' tilda */
    { 2, 79, 2, 123 },  /* (214) 'O' two dots */
    { 2, 32, -1 },      /* (215) */
    { 3, 91, -1 },      /* (216) 'O' slashed */
    { 1, 85, 1, 96 },   /* (217) 'U' accent left */
    { 2, 85, 2, 39 },   /* (218) 'U' accent right */
    { 2, 85, 2, 94 },   /* (219) 'U' carrot */
    { 2, 85, 2, 94 },   /* (220) 'U' two dots */
    { 2, 89, 2, 39 },   /* (221) 'Y' accent right */
    { 2, 32, -1 },      /* (222) */
    { 2, 32, -1 },      /* (223) */
    { 1, 97, 1, 96 },   /* (224) 'a' accent left */
    { 2, 97, 2, 39 },   /* (225) 'a' accent right */
    { 2, 97, 2, 94 },   /* (226) 'a' carrot */
    { 1, 97, 1, 126 },  /* (227) 'a' tilda */
    { 2, 97, 2, 123 },  /* (228) 'a' two dots */
    { 2, 97, 2, 124 },  /* (229) 'a' single dot */
    { 3, 94, -1 },      /* (230) 'ae' combination */
    { 2, 92, -1 },      /* (231) 'c' with beard */
    { 1, 101, 1, 96 },  /* (232) 'e' accent left */
    { 2, 101, 2, 39 },  /* (233) 'e' accent right */
    { 2, 101, 2, 94 },  /* (234) 'e' carrot */
    { 2, 101, 2, 123 }, /* (235) 'e' two dots */
    { 1, 105, 1, 96 },  /* (236) 'i' accent left */
    { 2, 105, 2, 39 },  /* (237) 'i' accent right */
    { 2, 105, 2, 94 },  /* (238) 'i' carrot */
    { 2, 105, 2, 123 }, /* (239) 'i' two dots */
    { 2, 32, -1 },      /* (240) */
    { 1, 110, 1, 126 }, /* (241) 'n' tilda */
    { 1, 111, 1, 96 },  /* (242) 'o' accent left */
    { 2, 111, 2, 39 },  /* (243) 'o' accent right */
    { 2, 111, 2, 94 },  /* (244) 'o' carrot */
    { 1, 111, 1, 126 }, /* (245) 'o' tilda */
    { 2, 111, 2, 123 }, /* (246) 'o' two dots */
    { 2, 32, -1 },      /* (247) */
    { 3, 93, -1 },      /* (248) 'o' slashed */
    { 1, 117, 1, 96 },  /* (249) 'u' accent left */
    { 2, 117, 2, 39 },  /* (250) 'u' accent right */
    { 2, 117, 2, 94 },  /* (251) 'u' carrot */
    { 2, 117, 2, 123 }, /* (252) 'u' two dots */
    { 2, 121, 2, 39 },  /* (253) 'y' accent right */
    { 2, 32, -1 },      /* (254) */
    { 2, 121, 2, 123 }  /* (255) 'y' two dots */
};
/* *************************** Exported Data ******************************* */
/* *************************** Local Routines ****************************** */

LOCAL void NEAR PASCAL underline_text ()
{
}

LOCAL void NEAR PASCAL strikeout_text ()
{
}

LOCAL void NEAR PASCAL output_a_number (lpPDevice,Number)
LPPDEVICE lpPDevice;
int Number;
{
    char Coordinate [7];

    ltoa((LPSTR) Coordinate,Number);
    PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) Coordinate,lstrlen ((LPSTR)
      Coordinate));
}

LOCAL void NEAR PASCAL construct_string (lpPDevice,lpString,Count)
LPPDEVICE lpPDevice;
LPSTR lpString;
short Count;
{
    BOOL StandardCharSetActive = lpPDevice->StandardCharSetActive;
    short Index,
          AltCharSet = lpPDevice->AltCharSet,
          CurStringLength = 0;
    char CurString [500];

    for (Index = 0; Index < Count; Index++)
    {
        BYTE Char = *(lpString++);

        if (Char < 128 && Char > 31)
        {
            if (!StandardCharSetActive)
            {
                CurString [CurStringLength++] = 15;
                StandardCharSetActive = TRUE;
            }
            CurString [CurStringLength++] = Char;
        }
        else if (Char > 160)
        {
            Char -= 161;
            if (StandardCharSetActive || !CharSetSevenSupport [lpPDevice->Setup.
              Plotter])
            {
                if (CharSetSevenSupport [lpPDevice->Setup.Plotter])
                    CurString [CurStringLength++] = 14;
                else
                {
                    char AltSet = MixedCharSet[Char][0];

                    if (AltCharSet != AltSet)
                    {
                        if (CurStringLength > 0)
                        {
                            PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "LB",2);
                            PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) CurString,
                              CurStringLength);
                            PutJob (lpPDevice,lpPDevice->hJob,(LPSTR)"\x03",1);
                        }
                        CurStringLength = 0;
                        PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "CA",2);
                        output_a_number (lpPDevice,AltSet);
                        AltCharSet = AltSet;
                    }
                    if (StandardCharSetActive)
                        CurString [CurStringLength++] = 14;
                }
                StandardCharSetActive = FALSE;
            }
            if (CharSetSevenSupport [lpPDevice->Setup.Plotter])
                CurString [CurStringLength++] = CharSetSeven [Char];
            else
            {
                CurString [CurStringLength++] = MixedCharSet[Char][1];
                if (MixedCharSet[Char][2] != -1)
                    CurString [CurStringLength++] = MixedCharSet[Char][3];
            }
        }
        else
          CurString [CurStringLength++] = ' ';
    }
    lpPDevice->StandardCharSetActive = StandardCharSetActive;
    lpPDevice->AltCharSet = AltCharSet;
    if (CurStringLength > 0)
    {
        PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "LB",2);
        PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) CurString,CurStringLength);
        PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "\x03",1);
    }
}
/* ************************** Exported Routines **************************** */

void FAR PASCAL draw_text (lpPDevice,DstXOrg,DstYOrg,lpClipRect,lpString,
  Count,lpFontInfo,lpDrawMode,lpTextXForm)
LPPDEVICE lpPDevice;
short DstXOrg,
      DstYOrg;
LPRECT lpClipRect;
LPSTR lpString;
short Count;
LPFONTINFO lpFontInfo;
LPDRAWMODE lpDrawMode;
LPTEXTXFORM lpTextXForm;
{
    POINT Origin, OOrigin;
    short Height, Width;

    check_line_construction (lpPDevice);
    if (lpClipRect)
    {
        PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "IW",2);
        Origin.x = (lpClipRect->left < lpClipRect->right) ? lpClipRect->left
                    : lpClipRect->right;
        if (lpPDevice->Setup.Orientation == 0)        /* Portrait */
            Origin.y = (lpClipRect->top < lpClipRect->bottom) ? lpClipRect->top
                    : lpClipRect->bottom;
        else
            Origin.y = (lpClipRect->bottom > lpClipRect->top) ? lpClipRect->bottom
                    : lpClipRect->top;

        OOrigin.x = Origin.x;
	     OOrigin.y = Origin.y;
        construct_point (lpPDevice,(LPPOINT) &Origin,FALSE);
        PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) ",",1);
        Origin.x = (lpClipRect->right > lpClipRect->left) ? lpClipRect->right
                    : lpClipRect->left;
        if (lpPDevice->Setup.Orientation == 0)
            Origin.y = (lpClipRect->bottom > lpClipRect->top) ? lpClipRect->bottom
                    : lpClipRect->top;
        else
            Origin.y = (lpClipRect->top < lpClipRect->bottom) ? lpClipRect->top
                    : lpClipRect->bottom;

	     if ((Origin.x == OOrigin.x) && (Origin.y == OOrigin.y))
        {
	         ++Origin.x;
	         ++Origin.y;
	     }
        construct_point (lpPDevice,(LPPOINT) &Origin,FALSE);
    }
    Origin.x = DstXOrg;

    // bug fix #4867
    // what the hell does dfAscent/6 indicate?  I changed to move down
    // dfInternalLeading
    // Origin.y = DstYOrg + lpFontInfo->dfAscent/6;

    Origin.y = DstYOrg + lpFontInfo->dfInternalLeading; 
    PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "PA",2);
    construct_point (lpPDevice,(LPPOINT) &Origin,FALSE);

    // bug fix #4867
    // changed Height to indicate INK height and not cell ht
    Height = (short)((long)(lpFontInfo->dfPixHeight - lpFontInfo->dfInternalLeading)*
                           RESOLUTION * 1000 / 600);
    Width = (short)((long) lpFontInfo->dfAvgWidth * RESOLUTION * 2000 / 1190);
    if (lpPDevice->CurrentHeight != Height || lpPDevice->CurrentWidth != Width)
    {
        PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "SI",2);
        construct_float (lpPDevice,lpPDevice->hJob,Width,3);
        PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) ",",1);
        construct_float (lpPDevice,lpPDevice->hJob,Height,3);
        lpPDevice->CurrentHeight = Height;
        lpPDevice->CurrentWidth = Width;
    }
    if (lpPDevice->TextSlanted && !lpFontInfo->dfItalic)
    {
        PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "SL",2);
        lpPDevice->TextSlanted = FALSE;
    }
    else if (!lpPDevice->TextSlanted && lpFontInfo->dfItalic)
    {
        PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "SL.5",4);
        lpPDevice->TextSlanted = TRUE;
    }
    PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "CP0,-.5",7);
    construct_string (lpPDevice,(LPSTR) lpString,Count);
    if (lpFontInfo->dfUnderline || lpFontInfo->dfStrikeOut)
    {
        POINT Line [2];
        short HalfWidth = lpFontInfo->dfAvgWidth / 2;
        DWORD TextExtent = get_text_extent (lpPDevice,DstXOrg,DstYOrg,
          lpClipRect,lpString,Count,lpFontInfo,lpDrawMode,lpTextXForm);
        PPEN WorkPhysPen;

        WorkPhysPen.PhysicalColor = lpDrawMode->TextColor;
        WorkPhysPen.Style = PS_SOLID;
        WorkPhysPen.Width.x = 0;
        if (lpFontInfo->dfUnderline)
        {
            Line [0].x = DstXOrg;
            Line [1].x = DstXOrg + LOWORD(TextExtent);
            Line [0].y = Line [1].y = DstYOrg + HIWORD(TextExtent) * 8 / 10;
            draw_polyline (lpPDevice,2,Line,(LPPPEN) &WorkPhysPen,
              (LPPBRUSH) NULL,(LPDRAWMODE) NULL,(LPRECT) NULL);
        }
        if (lpFontInfo->dfStrikeOut)
        {
            Line [0].x = DstXOrg + (lpFontInfo->dfItalic ? HalfWidth / 2 : 0);
            Line [1].x = DstXOrg + LOWORD(TextExtent) + (lpFontInfo->dfItalic ?
              HalfWidth / 2 : 0);
            Line [0].y = Line [1].y = DstYOrg + HIWORD(TextExtent) * 4 / 10;
            draw_polyline (lpPDevice,2,Line,(LPPPEN) &WorkPhysPen,
              (LPPBRUSH) NULL,(LPDRAWMODE) NULL,(LPRECT) NULL);
        }
    }
    check_line_construction (lpPDevice);
    PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "IW",2);
}

DWORD FAR PASCAL get_text_extent (lpPDevice,DstXOrg,DstYOrg,lpClipRect,lpString,
  Count,lpFontInfo,lpDrawMode,lpTextXForm)
LPPDEVICE lpPDevice;
short DstXOrg,
      DstYOrg;
LPRECT lpClipRect;
LPSTR lpString;
short Count;
LPFONTINFO lpFontInfo;
LPDRAWMODE lpDrawMode;
LPTEXTXFORM lpTextXForm;
{
    short Extent;
    DWORD Result;

    Extent = Count * (lpFontInfo->dfAvgWidth);
    Result = (((DWORD) (lpFontInfo->dfPixHeight) << 16) | Extent);
    return (Result);
}
