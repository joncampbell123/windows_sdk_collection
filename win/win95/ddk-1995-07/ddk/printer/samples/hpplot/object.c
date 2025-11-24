/* (c) Copyright 1987 MICROGRAFX, Inc., All Rights Reserved.  Permission to
   use this work for any purpose must be obtained in writing from MICROGRAFX,
   1820 N. Greenville Ave., Richardson, Tx.  75081.

*******************************************************************************
*******************************************************************************

                                  object

*******************************************************************************
*******************************************************************************

*/

#define DF_MAPSIZE 20

#define PRINTDRIVER
#define NOPQ
#define NOINSTALLFILES
#include <print.h>
#include <gdidefs.inc>

#include "device.h"
#include "driver.h"
#include "color.h"
#include "dialog.h"
#include "object.h"
#include "utils.h"
#include <memory.h>

/* ****************************** History ********************************** */
/* 10/31/87 (RWM) - signoff                                                  */
/* ***************************** Constants ********************************* */

#define LOCAL static

#define FONTWEIGHT          FW_NORMAL
#define CHARSET             0
#define FIRSTCHAR           32
#define LASTCHAR            255
#define DEFAULTCHAR         ('.'-FIRSTCHAR)
#define BREAKCHAR           (32-FIRSTCHAR)
/* **************************** Local Data ********************************* */

LOCAL LOGFONT DefaultLogicalFont =
{
    65,                         /* Height */
    32,                         /* Width */
    0,                          /* Escapement */
    0,                          /* Orientation */
    200,                        /* Weight */
    0,                          /* Italic */
    0,                          /* Underline */
    0,                          /* Strike Out */
    ANSI_CHARSET,               /* Character Set */
    OUT_CHARACTER_PRECIS,       /* Output Precision */
    CLIP_STROKE_PRECIS,         /* Clip Precision */
    PROOF_QUALITY,              /* Quality */
    FF_MODERN | FIXED_PITCH,    /* Pitch and Family */
    "Plotter"                   /* Face Name */
};

LOCAL TEXTMETRIC DefaultTextMetrics =
{
    65,                         /* Height */
#if 0
    65 * 7 / 6,                 /* Orig. Ascent */
    65 / 3,                     /* Orig. Descent */
  --ssn 11/19/91 To get baselines of different font sizes to align properly.
#endif
    65 * 3 / 4,                 /* New Ascent such that asc + desc = height */
    65 * 1 / 4,                 /* New Descent */
    65 / 3,                     /* Internal Leading */
    0,                          /* External Leading */
    32,                         /* Average Character Width */
    32,                         /* Maximum Character Width */
    200,                        /* Weight */
    0,                          /* Italic */
    0,                          /* Underlined */
    0,                          /* Strike Out */
    32,                         /* First Character */
    255,                        /* Last Character */
    32,                         /* Default Character */
    14,                         /* Break Character */
    FIXED_PITCH | FF_MODERN,    /* Pitch and Family */
    0,                          /* Character Set */
    0,                          /* Overhang */
    90,                         /* Digitized Aspect X */
    90                          /* Digitized Aspect Y */
};

LOCAL FONTINFO DefaultFont =
{
    PF_VECTOR_TYPE | PF_BITS_IS_ADDRESS | PF_DEVICE_REALIZED,
                    /* Type field for the font. */
    12,             /* Point size of font. */
    1,              /* Vertical digitization. */
    1,              /* Horizontal digitization. */
    1,              /* Baseline offset from top of character cell. */
    0,              /* The internal leading for the font. */
    1,              /* The external leading for the font. */
    0,              /* Flag specifying if italic. */
    0,              /* Flag specifying if underlined. */
    0,              /* Flag specifying if struck out. */
    FONTWEIGHT,     /* Weight of font. */
    CHARSET,        /* Character set of font. */
    1,              /* Width field for the font. */
    1,              /* Height field for the font. */
    FF_MODERN,      /* Flag specifying family and pitch. */
    1,              /* Average character width. */
    1,              /* Maximum character width. */
    FIRSTCHAR,      /* First character in the font. */
    LASTCHAR,       /* Last character in the font. */
    DEFAULTCHAR,    /* Default character for out of range. */
    BREAKCHAR,      /* Character to define wordbreaks. */
    1,              /* Number of bytes in each row. */
    sizeof(FONTINFO)-20, /* Offset to device name. */
    sizeof(FONTINFO)-9,  /* Offset to face name. */
    0L,             /* Reserved pointer. */
    0L,             /* Offset to the begining of the bitmap. */
                    /* Maps, etc. */
    'H', 'P', ' ', 'P', 'L', 'O', 'T', 'T', 'E', 'R', 0,
    'P', 'l', 'o', 't', 't', 'e', 'r', 0, 0
};

LOCAL TEXTXFORM DefaultTextXForm =
{
    1,                  /* Height of characters */
    1,                  /* Width of characters */
    0,                  /* Escapement */
    0,                  /* Orientation */
    FONTWEIGHT,         /* Weight */
    0,                  /* Italic */
    0,                  /* Underline */
    0,                  /* Strike Out */
    0,                  /* Output Precision */
    0,                  /* Clip Precision */
    TC_CP_STROKE |      /* Accelerator */
    TC_SF_X_YINDEP |
    TC_SA_DOUBLE |
    TC_SA_INTEGER |
    TC_SA_CONTIN |
    TC_IA_ABLE |
    TC_UA_ABLE |
    TC_SO_ABLE ,
    0                   /* Overhang */
};
/* *************************** Exported Data ******************************* */
/* *************************** Local Routines ****************************** */

LOCAL void NEAR PASCAL check_carousel (lpPDevice,Carousel,lpResult,lpPriority,
  Style,PhysicalColor)
LPPDEVICE lpPDevice;
short Carousel;
long far *lpResult;
short far *lpPriority;
short Style,
      PhysicalColor;
{
    short Index;

    for (Index = 0; Index < DeviceInfo [lpPDevice->Setup.Plotter].
      PensInCarousel; Index++)
    {
        short PenPriority = lpPDevice->Setup.Carousel [Carousel].Pen [Index].
          Usage;

        if (lpPDevice->Setup.Carousel [Carousel].Pen [Index].Color ==
          PhysicalColor && *lpPriority != PenPriority)
        {
            if ( (Style == OBJ_BRUSH && PenPriority == 2) ||
              (Style == OBJ_PEN && PenPriority == 1) || *lpPriority == -1 ||
              (PenPriority == 0 && ((*lpPriority == 1 && Style == OBJ_BRUSH) ||
              (*lpPriority == 2 && Style == OBJ_PEN))) )
            {
                *lpResult = MAKELONG (Index + 1,Carousel);
                *lpPriority = PenPriority;
            }
        }
    }
}
/* ************************** Exported Routines **************************** */

WORD FAR PASCAL enum_brushes (lpPDevice,CallbackFunc,lpData)
LPPDEVICE lpPDevice;
FARPROC CallbackFunc;
LPSTR lpData;
{
    short Index,
          Style,
          Hatch;
    LOGBRUSH LogBrush;

    for (Index = 0; Index < MAX_COLORS; Index++)
    {
        if (lpPDevice->ColorsAvailable [Index].Available)
        {
            LogBrush.lbColor = lpPDevice->ColorsAvailable [Index].Color;
            for (Style = 0; Style < 2; Style++)
            {
                LogBrush.lbStyle = Style;
                if (Style == 2)         /* Hatched */
                {
                    for (Hatch = 0; Hatch < 5; Hatch++)
                    {
                        LogBrush.lbHatch = Hatch;
                        if (!(*CallbackFunc) ((LPLOGBRUSH) &LogBrush,(LPSTR) lpData))
                           return (FALSE);
                    }
                }
                else
                    if (!(*CallbackFunc) ((LPLOGBRUSH) &LogBrush,(LPSTR) lpData))
                        return (FALSE);
            }
        }
    }
    return (TRUE);
}

int FAR PASCAL enum_fonts (lpPDevice,lpFacename,CallbackFunc,lpData)
LPPDEVICE lpPDevice;
LPSTR lpFacename;
FARPROC CallbackFunc;
LPSTR lpData;
{
    if (lpFacename ? !lstrcmpi ((LPSTR) lpFacename,(LPSTR) "Plotter") : TRUE)
    {
        int ret;

        ret = (*CallbackFunc) ((LPLOGFONT) &DefaultLogicalFont,(LPTEXTMETRIC)
          &DefaultTextMetrics,0,(LPSTR) lpData);

        return (ret);
    }
    else
    {
        return (1);
    }
}

WORD FAR PASCAL enum_pens (lpPDevice,CallbackFunc,lpData)
LPPDEVICE lpPDevice;
FARPROC CallbackFunc;
LPSTR lpData;
{
    short Index,
          Style;
    LOGPEN LogPen;

    LogPen.lopnWidth.x = LogPen.lopnWidth.y = 0;
    LogPen.lopnColor = 0x00FFFFFF;
    for (Style = 0; Style < 5; Style++)
    {
        LogPen.lopnStyle = Style;
        if (!(*CallbackFunc) ((LPLOGPEN) &LogPen,(LPSTR) lpData))
            return (FALSE);
    }
    for (Index = 0; Index < MAX_COLORS; Index++)
    {
        if (lpPDevice->ColorsAvailable [Index].Available)
        {
            LogPen.lopnColor = lpPDevice->ColorsAvailable [Index].Color;
            for (Style = 0; Style < 5; Style++)
            {
                LogPen.lopnStyle = Style;
                if (!(*CallbackFunc) ((LPLOGPEN) &LogPen,(LPSTR) lpData))
                  return (FALSE);
            }
        }
    }
    return (TRUE);
}

int FAR PASCAL get_physical_brush_size (lpPDevice,lpLogBrush)
LPPDEVICE lpPDevice;
LPLOGBRUSH lpLogBrush;
{
    return (sizeof (PBRUSH));
}

int FAR PASCAL get_physical_font_size (lpPDevice,lpLogFont,lpTextXForm)
LPPDEVICE lpPDevice;
LPLOGFONT lpLogFont;
LPTEXTXFORM lpTextXForm;
{
    return (sizeof (FONTINFO));
}

int FAR PASCAL get_physical_pen_size (lpPDevice,lpLogPen)
LPPDEVICE lpPDevice;
LPLOGPEN lpLogPen;
{
    return (sizeof (PPEN));
}

int FAR PASCAL realize_brush (lpPDevice,lpLogBrush,lpPBrush)
LPPDEVICE lpPDevice;
LPLOGBRUSH lpLogBrush;
LPPBRUSH lpPBrush;
{
    int retval = PBF_SUCCESS;

    if ((lpPBrush->Style = lpLogBrush->lbStyle) == BS_PATTERN)
        lpPBrush->Style = BS_HOLLOW;
    lpPBrush->Hatch = lpLogBrush->lbHatch;
    if (lpPBrush->Style != BS_HOLLOW)
    {
        DWORD Color;
        Color = get_nearest_rgb (lpPDevice,lpLogBrush->lbColor,(LPPCOLOR)
          &lpPBrush->PhysicalColor);
        lpPBrush->PhysicalColor = rgb_to_pen (lpPDevice,Color,OBJ_BRUSH);
    }
    else
        lpPBrush->PhysicalColor = 0x00FFFFFF;   /* white */

    if (lpPBrush->Style == BS_SOLID)
        retval |= PBF_SOLIDCLRBRUSH;
    if ((lpPBrush->PhysicalColor == 0x00FFFFFF) || (lpPBrush->PhysicalColor == 0))
        retval |= PBF_SOLIDMONOBRUSH;
    
    return retval;
}

int FAR PASCAL realize_font (lpPDevice,lpLogFont,lpFontInfo,lpTextXForm)
LPPDEVICE lpPDevice;
LPLOGFONT lpLogFont;
LPFONTINFO lpFontInfo;
LPTEXTXFORM lpTextXForm;
{

#ifdef NICK_DEBUG
char buf[64];
int retval;
#endif
   // Bug #4867 fix
   // dfPixHeight now indicates the font's cell height and not its 
   // ink height.  Ink height is equal to cell height - internal 
   // leading.

   // lfHeight represents ink ht
   if (lpLogFont->lfHeight < 0)
   {
      int lfHt = ABS(lpLogFont->lfHeight);

      DefaultFont.dfPixHeight = lfHt*4/3;
      DefaultFont.dfInternalLeading = lfHt/3;
   }
   // lfHeight represents cell ht
   else if (lpLogFont->lfHeight > 0)
   {
      DefaultFont.dfPixHeight = lpLogFont->lfHeight;
      DefaultFont.dfInternalLeading = lpLogFont->lfHeight/4;
   }
   // use default choice
   else
   {
      DefaultFont.dfPixHeight = 40;
      DefaultFont.dfInternalLeading = DefaultFont.dfPixHeight/4;
   }

   // fill in width, prevent negative values
   // dfPixWidth will be equal to the passed in positive value or
   // half of the INK height of the font if passed in value is <= 0

   if (lpLogFont->lfWidth > 0)
      DefaultFont.dfPixWidth = lpLogFont->lfWidth;
   else
      DefaultFont.dfPixWidth = (DefaultFont.dfPixHeight -
                               DefaultFont.dfInternalLeading) / 2;

    // Bug #4867 fix
    // dfAscent is the distance from top of ink to baseline, is this correct?
    // old setting - DefaultFont.dfAscent = DefaultFont.dfPixHeight * 3 / 4;
    // the above seems to be bogus since the driver sets the character ht to
    //  dfPixHeight and dfAscent is set to 3/4 of dfPixHeight. huh?!?!

    DefaultFont.dfAscent = DefaultFont.dfPixHeight - DefaultFont.dfInternalLeading;

    DefaultFont.dfExternalLeading = 0;
    DefaultFont.dfItalic = lpLogFont->lfItalic;
    DefaultFont.dfUnderline = lpLogFont->lfUnderline;
    DefaultFont.dfStrikeOut = lpLogFont->lfStrikeOut;
    DefaultFont.dfAvgWidth = DefaultFont.dfPixWidth;
    DefaultFont.dfMaxWidth = DefaultFont.dfPixWidth;
    DefaultFont.dfCharSet = lpLogFont->lfCharSet;
    _fmemcpy ((LPSTR) lpFontInfo,(LPSTR) &DefaultFont.dfType,sizeof(FONTINFO));

    DefaultTextXForm.ftHeight = DefaultFont.dfPixHeight;
    DefaultTextXForm.ftWidth = DefaultFont.dfPixWidth;
    DefaultTextXForm.ftItalic = DefaultFont.dfItalic;
    DefaultTextXForm.ftUnderline = DefaultFont.dfUnderline;
    DefaultTextXForm.ftStrikeOut = DefaultFont.dfStrikeOut;

    _fmemcpy ((LPSTR) lpTextXForm,(LPSTR) &DefaultTextXForm.ftHeight, sizeof(TEXTXFORM));

    return (sizeof (FONTINFO));
}

int FAR PASCAL realize_pen (lpPDevice,lpLogPen,lpPPen)
LPPDEVICE lpPDevice;
LPLOGPEN lpLogPen;
LPPPEN lpPPen;
{
    DWORD Color;

    Color = get_nearest_rgb (lpPDevice,lpLogPen->lopnColor,(LPPCOLOR) &lpPPen->
      PhysicalColor);
    lpPPen->PhysicalColor = rgb_to_pen (lpPDevice,Color,lpLogPen->lopnWidth.x >
      0 ? OBJ_BRUSH : OBJ_PEN);

    lpPPen->Style = lpLogPen->lopnStyle;
    lpPPen->Width.x = lpLogPen->lopnWidth.x;
    lpPPen->Width.y = lpLogPen->lopnWidth.x;
    return (sizeof (PPEN));
}

DWORD FAR PASCAL rgb_to_pen (lpPDevice,Color,Style)
LPPDEVICE lpPDevice;
DWORD Color;
int Style;
{
    DWORD SaveColor = Color;
    short Index,
          PhysicalColor,
          Priority = -1;
    long Result = 0x00FFFFFF;
    BOOL Found = FALSE;

    if (Color == (DWORD)Result)     /* white */
        return (Color);
    for (Index = 0; Index < MAX_COLORS && !Found; Index++)
    {
        if ((DWORD)lpPDevice->ColorsAvailable [Index].Color == Color)
        {
            PhysicalColor = Index + 1;
            Found = TRUE;
        }
    }
    if (Found)
    {
        short CurrentCarousel = lpPDevice->Setup.CurrentCarousel;

        check_carousel (lpPDevice,CurrentCarousel,(long far *) &Result,
          (short far *) &Priority,Style,PhysicalColor);
        for (Index = 0; Index < MAX_CAROUSELS; Index++)
        {
            if (lpPDevice->Setup.ActiveCarousel [Index] && Index !=
              CurrentCarousel)
                check_carousel (lpPDevice,Index,(long far *) &Result,
                  (short far *) &Priority,Style,PhysicalColor);
        }
    }
    return (Result);
}
