/* (c) Copyright 1987 MICROGRAFX, Inc., All Rights Reserved.  Permission to
   use this work for any purpose must be obtained in writing from MICROGRAFX,
   1820 N. Greenville Ave., Richardson, Tx.  75081.

*******************************************************************************
*******************************************************************************

				  control

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
#include "dialog.h"
#include "profile.h"
#include "utils.h"

#include "devmode.h"

#include <memory.h>


/* ****************************** History ********************************** */
/* 10/31/87 (RWM) - signoff                                                  */
/* ***************************** Constants ********************************* */

#define LOCAL static
/* **************************** Local Data ********************************* */
/* *************************** Exported Data ******************************* */

GDIINFO PlotterGDIInfo =
{
//    0x030A,       

    0x400,          /* Not a complete dumbass */
    DT_PLOTTER,     /* Device classification. */
    0,              /* Horizontal size in millimeters. */
    0,              /* Vertical   size in millimeters. */
    0,              /* Horizontal width in pixels. */
    0,              /* Vertical   width in pixels. */
    1,              /* Number of bits per pixel. */
    1,              /* Number of planes. */
    0,              /* Number of brushes the device has. */
    0,              /* Number of pens the device has. */
    0,              /* Number of markers the device has. (Reserved) */
    1,              /* Number of fonts the device has. */
    0,              /* Number of colors in color table. */
    0,              /* Size required for the device descriptor. */
    CC_CIRCLES      /* Curves    capabilities. */
     | CC_CHORD
     | CC_STYLED
     | CC_PIE
     | CC_WIDE
     | CC_INTERIORS
     | CC_ELLIPSES,
    LC_POLYLINE     /* Line      capabilities. */
     | LC_WIDE
     | LC_STYLED,
    PC_SCANLINE     /* Polygonal capabilities. */
     | PC_POLYGON
     | PC_RECTANGLE
     | PC_STYLED
     | PC_WIDE
     | PC_INTERIORS,
    TC_CP_STROKE |
    TC_SF_X_YINDEP |    /* Text capabilities. */
    TC_SA_DOUBLE |
    TC_SA_INTEGER |
    TC_SA_CONTIN |
    TC_IA_ABLE |
    TC_UA_ABLE |
    TC_SO_ABLE,
    CP_RECTANGLE,   /* Clipping  capabilities. */
    RC_BANDING | RC_GDI20_OUTPUT |
    RC_NONE,        /* Bitblt    capabilities. */
    10,             /* Length of X leg. */
    10,             /* Length of Y leg. */
    14,             /* Length of hypotenuse. */
    0,              /* Length of segment for line styles. */
		    /* These scalings get filled in at enable time. */
    1, 1,           /* Metric  Lo res WinX,WinY  (PTTYPE). */
    1, 1,           /* Metric  Lo res VptX,VptY  (PTTYPE). */
    1, 1,           /* Metric  Hi res WinX,WinY  (PTTYPE). */
    1, 1,           /* Metric  Hi res VptX,VptY  (PTTYPE). */
    1, 1,           /* English Lo res WinX,WinY  (PTTYPE). */
    1, 1,           /* English Lo res VptX,VptY  (PTTYPE). */
    1, 1,           /* English Hi res WinX,WinY  (PTTYPE). */
    1, 1,           /* English Hi res VptX,VptY  (PTTYPE). */
    1, 1,           /* Twips          WinX,WinY  (PTTYPE). */
    1, 1,           /* Twips          VptX,VptY  (PTTYPE). */
    0,              /* Logical pixels/inch in X. */
    0,              /* Logical pixels/inch in Y. */
    DC_SPDevice,    /* dpDCManage. Controls creation of PDevices. */
    0,              /* dpCaps1 */
    0,              /* dpSpotSizeX  */
    0,              /* dpSpotSizeY  */
    0,              /* dpPalColors  */
    0,              /* dpPalReserved  */
    0               /* dpPalResolution */
};

POINTL PaperResolution [NUM_PLOTTERS] [MAX_MEDIA_SIZES] =
{
    {  /* ColorPro */
	/* The next 2 lines are the old values - ssn 5/17/91/*
	/*{ 7650,10300 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },*/
	/*{ 7650,10900 } */
	{ 7645,10295 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 7650,10900 },
	{ 0, 0 },       { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
    },
    {  /* ColorPro with GEC */
	{ 7650,10300 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 7650,10900 },
	{ 0, 0 },       { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
    },
    {  /* HP 7470A */
	{ 7650,10300 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 7650,10900 },
	{ 0, 0 },       { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
    },
    {  /* HP 7475A */
	{ 7692,10365 }, { 10365,16640 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 7721,11040 },
	{ 11040,16158 },{ 0, 0 },        { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
    },
    {  /* HP 7550A */
	{ 7840,10170 }, { 10170,16450 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 7600,10870 },
	{ 10870,15970 },{ 0, 0 },        { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
    },
    {  /* HP 7580A */
	{ 9976,6476 }, { 9976,15112 }, { 16072,20192 }, { 21152,32384 },
	{ 0, 0 }, { 10680,6240 }, { 10680,14640 }, { 15600,21600 },
	{ 22560,31480 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
    },
    {  /* HP 7585A */
	{ 9976,6476 }, { 9976,15112 }, { 21152,15112 }, { 21152,32384 },
	{ 33344,42544 }, { 10680,6240 }, { 10680,14640 } ,
	{ 22560,14640 }, { 22560,31480 }, { 32440,45400 }, { 0, 0 }, { 0, 0 }
    },
    {  /* HP 7580B */
	{ 9976,6476 }, { 9976,15112 }, { 16072,20192 }, { 21152,32384 },
	{ 0, 0 }, { 10680,6240 }, { 10680,14640 }, { 15600,21600 },
	{ 22560,31480 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
    },
    {  /* HP 7585B */
	{ 9976,6476 }, { 9976,15112 }, { 21152,15112 }, { 21152,32384 },
	{ 33344,42544 }, { 10680,6240 }, { 10680,14640 } ,
	{ 22560,14640 }, { 22560,31480 }, { 32440,45400 }, { 0, 0 }, { 0, 0 }
    },
    {  /* HP 7588B */
	{ 9976,6476 }, { 9976,15112 }, { 21152,15112 }, { 21152,32384 },
	{ 33344,42544 }, { 10680,6240 }, { 10680,14640 } ,
	{ 22560,14640 }, { 22560,31480 }, { 32440,45400 },
	{ 23184,35576 }, { 35376,47568 }
    },
    {  /* DraftPro */
	{ 0, 0 }, { 0, 0 }, { 15112,21152 }, { 21152,32384 }, { 0, 0 }, { 0, 0 },
	{ 0, 0 }, { 14640,22560 }, { 22560,31480 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
    },
    {  /* DraftPro DXL */
	{ 7276 , 8856 }, {  9816, 14952 }, { 15992, 20112 }, { 20992, 32224 },
	{ 33184,42384 }, { 7040, 9560 }, { 10520, 14480 }, { 15520, 21520 }, 
	{ 22400, 31320 }, { 32280,45240 }, { 0, 0 }, { 0, 0 }
    },
    {  /* DraftPro EXL */
	{ 7276 , 8856 }, {  9816, 14952 }, { 15992, 20112 }, { 20992, 32224 },
	{ 33184,42384 }, { 7040, 9560 }, { 10520, 14480 }, { 15520, 21520 }, 
	{ 22400, 31320 },{ 32280,45240 }, { 0, 0 }, { 0, 0 }
    },
    {  /* DraftMaster I */
	{ 7276,8856 }, { 9816,14952 }, { 14912,20952 }, { 20992,32224 },
	{ 33184,42384 }, { 7040, 9560 }, { 10520,14480 }, { 14560,22480 }, 
	{ 22400,31320 }, { 32280,45240 }, { 0, 0 }, { 0, 0 }
    },
    {  /* DraftMaster II */
	{ 7276,8856 }, { 9816,14952 }, { 14912,20952 }, { 20992,32224 },
	{ 33184,42384 }, { 7040, 9560 }, { 10520,14480 }, { 14560,22480 }, 
	{ 22400,31320 }, { 32280,45240 }, { 23024,35216 }, { 35216,47408 }
    }
};

COLORSAVAILABLE ColorList [MAX_COLORS] =
{
    { 0x0000FFFF, TRUE },       /* Yellow */
    { 0x000080FF, TRUE },       /* Orange */
    { 0x000000FF, TRUE },       /* Red */
    { 0x0000FF00, TRUE },       /* Green */
    { 0x00FF00FF, TRUE },       /* Red Violet */
    { 0x00FFFF00, TRUE },       /* Aqua */
    { 0x0000C0FF, TRUE },       /* Brown */
    { 0x00FF0080, TRUE },       /* Violet */
    { 0x00FF0000, TRUE },       /* Blue */
    { 0x00000000, TRUE }        /* Black */
};

/* *************************** Local Routines ****************************** */

LOCAL short FAR PASCAL determine_colors (lpBoolTable,lpSetup)
LPINT lpBoolTable;
LPENVIRONMENT lpSetup;
{
    short Carousel,
	  Pen,
	  PensInCarousel = DeviceInfo [lpSetup->Plotter].PensInCarousel,
	  Index,
	  nColors = 1;          /* Start with white */

    for (Index = 0; Index < MAX_COLORS; *(lpBoolTable + Index++) = FALSE);
    for (Carousel = 0; Carousel < MAX_CAROUSELS; Carousel++)
    {
	if (lpSetup->ActiveCarousel [Carousel])
	{
	    for (Pen = 0; Pen < PensInCarousel; Pen++)
	    {
		short Color;

		if ( (Color = lpSetup->Carousel [Carousel].Pen [Pen].
		  Color) != 0 && !*(lpBoolTable + Color - 1))
		{
		    *(lpBoolTable + Color - 1) = TRUE;
		    nColors++;
		}
	    }
	}
    }
    return (nColors);
}

LOCAL void FAR PASCAL normalize_gdi_point (LX1,LY1,pPoint1,LX2,LY2,pPoint2)
long LX1,
     LY1;
PPOINT pPoint1;
long LX2,
     LY2;
PPOINT pPoint2;
{
    while ( (ABS (LX1) > 32767L) || (ABS (LX2) > 32767L))
    {
	LX1 /= 2;
	LX2 /= 2;
    }

    while ( (ABS (LY1) > 32767L) || (ABS (LY2) > 32767L))
    {
	LY1 /= 2;
	LY2 /= 2;
    }
    pPoint1->x = (int) LX1;
    pPoint1->y = (int) LY1;
    pPoint2->x = (int) LX2;
    pPoint2->y = (int) LY2;
}

LOCAL void FAR PASCAL rotate_dimensions (pGDIinfo)
PGDIINFO pGDIinfo;
{
    short Temp;

    Temp = pGDIinfo->dpHorzSize;
    pGDIinfo->dpHorzSize = pGDIinfo->dpVertSize;
    pGDIinfo->dpVertSize = Temp;
    Temp = pGDIinfo->dpHorzRes;
    pGDIinfo->dpHorzRes = pGDIinfo->dpVertRes;
    pGDIinfo->dpVertRes = Temp;
}

LOCAL void FAR PASCAL scale_gdi_info (pGDIInfo)
PGDIINFO pGDIInfo;
{
/* Metric  Lo res WinX,WinY. */
/* Metric  Lo res VptX,VptY. */
    normalize_gdi_point ((long) pGDIInfo->dpHorzSize * 10L,(long) pGDIInfo->
      dpVertSize * 10L,(PPOINT) &(pGDIInfo->dpMLoWin),(long) pGDIInfo->
      dpHorzRes,-(long) pGDIInfo->dpVertRes,(PPOINT) &(pGDIInfo->dpMLoVpt));

/* Metric  Hi res WinX,WinY. */
/* Metric  Hi res VptX,VptY. */
    normalize_gdi_point ((long) pGDIInfo->dpHorzSize * 100L,(long) pGDIInfo->
      dpVertSize * 100L,(PPOINT) &(pGDIInfo->dpMHiWin),(long) pGDIInfo->
      dpHorzRes,-(long) pGDIInfo->dpVertRes,(PPOINT) &(pGDIInfo->dpMHiVpt));

/* English Lo res WinX,WinY. */
/* English Lo res VptX,VptY. */
    normalize_gdi_point ((long) pGDIInfo->dpHorzSize * 1000L,(long)
      pGDIInfo->dpVertSize * 1000L,(PPOINT) &(pGDIInfo->dpELoWin),(long)
      pGDIInfo->dpHorzRes * 254L,-(long) pGDIInfo->dpVertRes * 254L,
      (PPOINT) &(pGDIInfo->dpELoVpt));

/* English Hi res WinX,WinY. */
/* English Hi res VptX,VptY. */
#if 0
    normalize_gdi_point ((long) pGDIInfo->dpHorzSize * 1000L,(long)
      pGDIInfo->dpVertSize * 1000L,(PPOINT) &(pGDIInfo->dpEHiWin),
      (long) pGDIInfo->dpHorzRes * 254L,-(long) pGDIInfo->dpVertRes * 254L,
      (PPOINT) &(pGDIInfo->dpEHiVpt));

    --ssn 11/12/91: since STEPSPERMM is 40 instead of 4, this means VertSize
      and HorzSize are actually 10 times too small, thus we multiply them
      by 10,000 instead of 1000.
#endif

    normalize_gdi_point ((long) pGDIInfo->dpHorzSize * 10000L,(long)
      pGDIInfo->dpVertSize * 10000L,(PPOINT) &(pGDIInfo->dpEHiWin),
      (long) pGDIInfo->dpHorzRes * 254L,-(long) pGDIInfo->dpVertRes * 254L,
      (PPOINT) &(pGDIInfo->dpEHiVpt));


/* Twips          WinX,WinY. */
/* Twips          VptX,VptY. */
    normalize_gdi_point ((long) pGDIInfo->dpHorzSize * 14400L / 254L,(long)
      pGDIInfo->dpVertSize * 14400L / 254L,(PPOINT) &(pGDIInfo->dpTwpWin),(long)
      pGDIInfo->dpHorzRes,-(long)pGDIInfo->dpVertRes,
      (PPOINT) &(pGDIInfo->dpTwpVpt));
}

/* ************************** Exported Routines **************************** */

void FAR PASCAL disable_device (lpPDevice)
LPPDEVICE lpPDevice;
{
}

int FAR PASCAL initialize_device (lpPDevice,lpDeviceType,lpOutputFile,lpData)
LPPDEVICE lpPDevice;
LPSTR lpDeviceType,
      lpOutputFile;
LPENVIRONMENT lpData;
{
    short Index;
    BOOL ColorsUsed [MAX_COLORS];

#ifdef COLORSORTING
    lpPDevice->NumColors = 0;           /* Initialize to create color table */
#endif

    lpPDevice->Type = 1;

    if ((ExtDeviceMode(NULL, NULL, &lpPDevice->Setup, lpDeviceType, lpOutputFile, 
		       lpData, NULL, DM_COPY|DM_MODIFY)) <= 0)
      return 0;

    determine_colors ((LPINT) ColorsUsed,(LPENVIRONMENT) &lpPDevice->
      Setup);
    for (Index = 0; Index < MAX_COLORS; Index++)
	ColorList [Index].Available = ColorsUsed [Index];
    _fmemcpy ((LPSTR) &lpPDevice->ColorsAvailable [0].Color,(LPSTR)
      &ColorList [0].Color,sizeof (COLORSAVAILABLE) * MAX_COLORS);
    lstrcpy ((LPSTR) lpPDevice->OutputFile,(LPSTR) lpOutputFile);
    lpPDevice->CurrentHeight = lpPDevice->CurrentWidth = 0;
    lpPDevice->PenStatus = UP;
    lpPDevice->LineInConstruction = FALSE;
    lpPDevice->StartingCarousel = lpPDevice->Setup.CurrentCarousel;
    lpPDevice->CurrentPenStyle = 0;      /* Solid Line */
    lpPDevice->CurrentPenThickness = 0;  /* force intial pen thickness */
    lpPDevice->CurrentBrushStyle = BS_SOLID;    /* Solid fill */
    lpPDevice->CurrentSpeed = lpPDevice->CurrentForce = lpPDevice->
      CurrentAcceleration = lpPDevice->CurrentBrushAngle = 0;
    lpPDevice->TextSlanted = FALSE;   /* Text slanted? */
    lpPDevice->HorzRes = (unsigned) (PaperResolution [lpPDevice->Setup.Plotter]
      [lpPDevice->Setup.Size].x / RESOLUTION);
    if (lpPDevice->Setup.Size > 9)      /* Roll Feed */
    {
	LONG PaperRes = PaperResolution [lpPDevice->Setup.Plotter]
			[lpPDevice->Setup.Size].y;

	lpPDevice->VertRes = (UINT)((long) (lpPDevice->Setup.Length * 254 * 40 / (10 *
	  RESOLUTION)));
	lpPDevice->NumBandsCompleted = 0;
	lpPDevice->NumBands = (short)((long) lpPDevice->VertRes * RESOLUTION / PaperRes
	  + 1);
	lpPDevice->LastBandLength = (short)((long) lpPDevice->VertRes % (PaperRes /
	  RESOLUTION));
	lpPDevice->LengthRes = (UINT)(PaperRes / RESOLUTION);
    }
    else
    {
	lpPDevice->VertRes = (unsigned) (PaperResolution [lpPDevice->Setup.
	  Plotter] [lpPDevice->Setup.Size].y / RESOLUTION);
	lpPDevice->NumBands = (lpPDevice->NumBandsCompleted = 0) + 1;
    }
    if (lpPDevice->Setup.Orientation == 1)      /* Landscape */
    {
	unsigned Temp = lpPDevice->HorzRes;

	lpPDevice->HorzRes = lpPDevice->VertRes;
	lpPDevice->VertRes = Temp;
    }
    return (sizeof (PDEVICE));
}

int FAR PASCAL initialize_gdi (lpGDIInfo,lpDeviceType,lpOutputFile,lpData)
LPGDIINFO lpGDIInfo;
LPSTR lpDeviceType,
      lpOutputFile;
LPENVIRONMENT lpData;
{
    PGDIINFO pGDIinfo;
    ENVIRONMENT Setup;

    if ((ExtDeviceMode(NULL, NULL, &Setup, lpDeviceType, lpOutputFile, 
		       lpData, NULL, DM_COPY|DM_MODIFY)) <= 0)
      return 0;

    pGDIinfo = (PGDIINFO) &PlotterGDIInfo;
    pGDIinfo->dpDEVICEsize = sizeof (PDEVICE);
    pGDIinfo->dpHorzRes = (unsigned) (PaperResolution [Setup.Plotter]
      [Setup.Size].x / RESOLUTION);
    pGDIinfo->dpVertRes = (unsigned) (PaperResolution [Setup.Plotter]
      [Setup.Size].y / RESOLUTION);
    pGDIinfo->dpHorzSize = (unsigned) (PaperResolution [Setup.Plotter]
      [Setup.Size].x / STEPSPERMM);
    pGDIinfo->dpVertSize = (unsigned) (PaperResolution [Setup.Plotter]
      [Setup.Size].y / STEPSPERMM);
    if (Setup.Size > 9)  /* Roll Feed */
    {
	pGDIinfo->dpVertSize = (short)((long) Setup.Length * 254 / 10);
	pGDIinfo->dpVertRes = (short)((long) Setup.Length * 254 * 40 / (10 *
	  RESOLUTION));
    }
    scale_gdi_info (pGDIinfo);
    if (Setup.Orientation == 1)     /* Landscape */
	rotate_dimensions (pGDIinfo);
    pGDIinfo->dpStyleLen = (500 / RESOLUTION) * 10 / 8;
    {
	BOOL ColorsUsed [MAX_COLORS];

	pGDIinfo->dpNumColors = determine_colors ((LPINT) ColorsUsed,
	  (LPENVIRONMENT) &Setup);
    }
    pGDIinfo->dpNumBrushes = pGDIinfo->dpNumColors;
    pGDIinfo->dpNumPens = pGDIinfo->dpNumColors * NUM_PEN_STYLES;
    pGDIinfo->dpLogPixelsX = (short)(((long) ((long) pGDIinfo->dpHorzRes * 254) /
      ((long) pGDIinfo->dpHorzSize * 10)));
    pGDIinfo->dpLogPixelsY = (short)(((long) ((long) pGDIinfo->dpVertRes * 254) /
      ((long) pGDIinfo->dpVertSize * 10)));


    // In order to be EMF compliant, the driver must support RC_BITBLT.
    // This driver does not so I have turned off the capability bit.
    //                                      [rayen]  15 March 1995
    //
    // pGDIinfo->dpCaps1 |= C1_EMF_COMPLIANT;

    _fmemcpy ((LPSTR) lpGDIInfo,(LPSTR) pGDIinfo,sizeof (GDIINFO));
    return (sizeof (GDIINFO));
}
