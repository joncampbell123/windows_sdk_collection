/* (c) Copyright 1987 MICROGRAFX, Inc., All Rights Reserved.  Permission to
   use this work for any purpose must be obtained in writing from MICROGRAFX,
   1820 N. Greenville Ave., Richardson, Tx.  75081.

*******************************************************************************
*******************************************************************************

                                   print

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
#include "output.h"
#include "print.h"
#include "utils.h"
/* ****************************** History ********************************** */
/* 10/31/87 (RWM) - signoff                                                  */
/* ***************************** Constants ********************************* */

#define LOCAL static
/* **************************** Local Data ********************************* */

LOCAL unsigned char ShortEdge [NUM_PLOTTERS] [MAX_MEDIA_SIZES] =
{
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },     /* ColorPro */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },     /* ColorPro w/GEC */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },     /* 7470A */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },     /* 7475A */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },     /* 7550A */
    { 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0 },     /* 7580A */
    { 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0 },     /* 7580B */
    { 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0 },     /* 7585A */
    { 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0 },     /* 7585B */
    { 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0 },     /* 7586B */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },     /* DraftPro */
    { 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0 },     /* DMU */
    { 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0 },     /* EMU */
    { 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0 },     /* DraftMaster I */
    { 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0 }      /* DraftMaster II */
};

LOCAL short PenPixelThickness [MAX_PEN_TYPES] =
{
    1,    /* P3 */
    2,    /* P7 */
    1,    /* T3 */
    2,    /* T6 */
    1,    /* V25 */
    1,    /* V35 */
    2,    /* V50 */
    2,    /* V70 */
    1,    /* R3 */
    1,    /* .18 */
    1,    /* .25 */
    1,    /* .35 */
    2,    /* .50 */
    2,    /* .70 */
    3     /* 1.00 */
};
/* *************************** Exported Data ******************************* */

RECT Corner [NUM_PLOTTERS] [MAX_MEDIA_SIZES] =
{
    {    /* ColorPro */
        { 0,0,10300,7650 }, { 0 }, { 0 }, { 0 }, { 0 },
        { 0,0,10900,7650 }
    },
    {   /* ColorPro with GEC */
        { 0,0,10300,7650 }, { 0 }, { 0 }, { 0 }, { 0 },
        { 0,0,10900,7650 }
    },
    {   /* HP 7470A */
        { 0,0,10300,7650 }, { 0 }, { 0 }, { 0 }, { 0 },
        { 0,0,10900,7650 }
    },
    {   /* HP 7475A */
        { 0,0,10365,7962 }, { 0,0,16640,10365 }, { 0 }, { 0 }, { 0 },
        { 0,0,11040,7721 }, { 0,0,16158,11040 }
    },
    {   /* HP 7550A */
        { 0,0,10170,7840 }, { 0,0,16450,10170 }, { 0 }, { 0 }, { 0 },
        { 0,0,10870,7600 }, { 0,0,15970,10870 }
    },
    {   /* HP 7580A */
        { -3238,-4988,3238,4988 }, { -7556,-4988,7556,4988 },
        { -10096,-8036,10096,8036 }, { -16192,-10576,16192,10576 }, { 0 },
        { -3120,-5340,3120,5340 }, { -7320,-5340,7320,5340 },
        { -10800,-7800,10800,7800 }, { -15740,-11280,15740,11280 }
    },
    {   /* HP 7585A */
        { -3238,-4988,3238,4988 }, { -7556,-4988,7556,4988 },
        { -7556,-10576,7556,10576 }, { -16192,-10576,16192,10576 },
        { -21272,-16672,21272,16672 },
        { -3120,-5340,3120,5340 }, { -7320,-5340,7320,5340 },
        { -7320,-11280,7320,11280 }, { -15740,-11280,15740,11280 },
        { -22700,-16220,22700,16220 }
    },
    {   /* HP 7580B */
        { -3238,-4988,3238,4988 }, { -7556,-4988,7556,4988 },
        { -10096,-8036,10096,8036 }, { -16192,-10576,16192,10576 }, { 0 },
        { -3120,-5340,3120,5340 }, { -7320,-5340,7320,5340 },
        { -10800,-7800,10800,7800 }, { -15740,-11280,15740,11280 }
    },
    {   /* HP 7585B */
        { -3238,-4988,3238,4988 }, { -7556,-4988,7556,4988 },
        { -7556,-10576,7556,10576 }, { -16192,-10576,16192,10576 },
        { -21272,-16672,21272,16672 },
        { -3120,-5340,3120,5340 }, { -7320,-5340,7320,5340 },
        { -7320,-11280,7320,11280 }, { -15740,-11280,15740,11280 },
        { -22700,-16220,22700,16220 }
    },
    {   /* HP 7586B */
        { -3238,-4988,3238,4988 }, { -7556,-4988,7556,4988 },
        { -7556,-10576,7556,10756 }, { -16192,-10576,16192,10576 },
        { -21272,-16672,21272,16672 },
        { -3120,-5340,3120,5340 }, { -7320,-5340,7320,5340 },
        { -7320,-11280,7320,11280 }, { -15740,-11280,15740,11280 },
        { -22700,-16220,22700,16220 },
        { -17788,-11592,17788,11592 }, { -23784,-17688,23784,17688 }
    },
    {   /* DraftPro */
        { 0,0 }, { 0,0 }, { -10576,-7556,10576,7556 },
        { -16192,-10576,16192,10576 }, { 0,0 },
        { 0,0 }, { 0,0 }, { -11280,-7320,11280,7320 },
        { -15740,-11280,15740,11280 }
    },
    {   /* DRAFTPRO DXL */
        { -3678,-4468,3678,4468   }, { -7556 ,-4988 ,7556 , 4988 },
        { -7996,-10056,7996,10056 }, { -16192,-10576,16192,10576 },
        { 0, 0, 0, 0 },
        { -3560, -4820, 3560,4820 }, {  -7320, -5340, 7320, 5340 },
        { -7760,-10760,7760,10760 }, { -15720,-11280,15720,11280 },
        { 0, 0, 0, 0 },
    },
    {   /* DRAFTPRO EXL */
        { -3678,-4468,3678,4468   }, { -7556 ,-4988 ,7556 , 4988 },
        { -7996,-10056,7996,10056 }, { -16192,-10576,16192,10576 },
        { -21272, -16672, 21272, 16672 },
        { -3560, -4820, 3560,4820 }, {  -7320, -5340, 7320, 5340 },
        { -7760,-10760,7760,10760 }, { -15720,-11280,15720,11280 },
        { -22680,-16200,22680,16200 }
    },
    {   /* HP 7595A */
        { -3638,-4428,3638,4428 }, { -7516,-4948,7516,4948 },
        { -7476,-10496,7476,10496 }, { -10496,-16112,10496,16112 },
        { -21232,-16632,21232,16632 },
        { -3520,-4780,3520,4780 }, { -7280,-5300,7280,5300 },
        { -11240,-7280,11240,7280 }, { -11200,-15660,11200,15660 },
        { -22660,-16180,22660,16180 }
    },
    {   /* HP 7596A */
        { -3638,-4428,3638,4428 }, { -7516,-4948,7516,4948 },
        { -7476,-10496,7476,10496 }, { -10496,-16112,10496,16112 },
        { -21232,-16632,21232,16632 },
        { -3520,-4780,3520,4780 }, { -7280,-5300,7280,5300 },
        { -11240,-7280,11240,7280 }, { -11200,-15660,11200,15660 },
        { -22660,-16180,22660,16180 },
        { -17648,-11552,17648,11552 }, { -23744,-17648,23744,17648 }
    }
};

unsigned char PrintOffset [NUM_PLOTTERS] [MAX_MEDIA_SIZES] [3] =
{
    {   /* ColorPro */
        { 9, 14, 16 }, { 0 }, { 0 }, { 0 }, { 0 },
        { 9, 14, 16 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }
    },
    {   /* ColorPro w/GEC */
        { 9, 14, 16 }, { 0 }, { 0 }, { 0 }, { 0 },
        { 9, 14, 16 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }
    },
    {   /* 7470A */
        { 8, 14, 17 }, { 0 }, { 0 }, { 0 }, { 0 },
        { 8, 14, 17 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }
    },
    {   /* 7475A */
        { 4, 9, 15 }, { 9, 14, 12 }, { 0 }, { 0 }, { 0 },
        { 4, 9, 15 }, { 9, 14, 12 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }
    },
    {   /* 7550A */
        /* The next two comments are the really old values -- (nvf)	*/
        /* { 13, 15, 12 }, { 13, 15, 12 }, { 0 }, { 0 }, { 0 }, */
        /* { 13, 15, 12 }, { 13, 15, 12 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 } */
	/* The next two comments are the old values -- (ssn)	*/
        /* { 5, 13, 5 }, { 5, 13, 5 }, { 0 }, { 0 }, { 0 }, */
        /* { 5, 13, 5 }, { 5, 13, 5 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }*/
        { 5, 13, 12 }, { 5, 13, 5 }, { 0 }, { 0 }, { 0 },
        { 5, 13, 5 }, { 5, 13, 5 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }
    },
    {   /* 7580A */
        { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 },
        { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 },
        { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 }
    },
    {   /* 7585A */
        { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 },
        { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 },
        { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 }
    },
    {   /* 7580B */
        { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 },
        { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 },
        { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 }
    },
    {   /* 7585B */
        { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 },
        { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 },
        { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 }
    },
    {   /* 7586B */
        { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 },
        { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 },
        { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 }, { 15, 39, 15 }
    },
    {   /* DraftPro */
        { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 },
        { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 },
        { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 }
    },
    {   /* DraftPro DXL */
        { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 },
        { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 },
        { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 }
    },
    {   /* DraftPro EXL */
        { 16, 40, 16}, { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 },
        { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 },
        { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 }
    },
    {   /* DraftMaster I */
        { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 },
        { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 },
        { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 }
    },
    {   /* DraftMaster II */
	/* The next three comments are the old values -- (ssn)	*/
        /* { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 },*/
        /* { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 },*/
        /* { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 }*/

        { 16, 16, 16 }, { 16, 17, 16 }, { 45, 17, 16 }, { 16, 17, 16 },
        { 16, 20, 16 }, { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 },
        { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 }, { 16, 40, 16 }
    }
};

BOOL MyArc [NUM_PLOTTERS] =
{
    FALSE,
    TRUE,
    FALSE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE
};

BOOL Circle [NUM_PLOTTERS] =
{
    FALSE,
    TRUE,
    FALSE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE
};

BOOL EdgeRectangle [NUM_PLOTTERS] =
{
    FALSE,
    TRUE,
    FALSE,
    FALSE,
    TRUE,
    FALSE,
    FALSE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE
};

BOOL FillRectangle [NUM_PLOTTERS] =
{
    FALSE,
    TRUE,
    FALSE,
    TRUE,
    TRUE,
    FALSE,
    FALSE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE
};

BOOL MyPolygon [NUM_PLOTTERS] =
{
    FALSE,
    TRUE,
    FALSE,
    FALSE,
    TRUE,
    FALSE,
    FALSE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE
};

short MaxPolygonSize [NUM_PLOTTERS] =
{
    0,
    86,
    0,
    0,
    626,
    0,
    0,
    218,
    218,
    218,
    256,
    256,
    256,
    256,
    256
};

BOOL Wedge [NUM_PLOTTERS] =
{
    FALSE,   /* ColorPro */
    TRUE,    /* ColorPro w/GEC */
    FALSE,   /* 7470a */
    FALSE,   /* 7475a */
    TRUE,    /* 7550a */
    FALSE,   /* 7580a */
    FALSE,   /* 7585a */
    TRUE,    /* 7580b */
    TRUE,    /* 7585b */
    TRUE,    /* 7586b */
    TRUE,    /* DraftPro */
    TRUE,    /* DraftPro DXL */
    TRUE,    /* DraftPro EXL */
    TRUE,    /* DraftMaster I */
    TRUE     /* DraftMaster II */
};

BOOL PenThickness [NUM_PLOTTERS] =
{
    FALSE,
    FALSE,
    FALSE,
    TRUE,
    TRUE,
    FALSE,
    FALSE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE
};

BOOL AutoView [NUM_PLOTTERS] =
{
    FALSE,
    FALSE,
    FALSE,
    FALSE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE
};

BOOL CharSetSevenSupport [NUM_PLOTTERS] =
{
    FALSE,
    TRUE,
    FALSE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE,
    TRUE
};

POINT PhysicalPageSize [MAX_MEDIA_SIZES] =
{					/* These are in millimeters	*/
    { 216, 279 },
    { 279, 432 },
    { 432, 559 },
    { 559, 863 },
    { 863, 1118 },
    { 210, 297 },
    { 297, 420 },
    { 420, 594 },
    { 594, 841 },
    { 841, 1189 },
    { 610, 0 },
    { 914, 0 }
};

/* *************************** Local Routines ****************************** */

LOCAL void NEAR PASCAL end_page (lpPDevice)
LPPDEVICE lpPDevice;
{
    if (lpPDevice->LineInConstruction)
    {
        PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "PA",2);
        lpPDevice->LineInConstruction = FALSE;
    }
    PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "SP",2);
    if (AutoView [lpPDevice->Setup.Plotter] && lpPDevice->Setup.PaperFeed ==
      0)
        PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "NR;",3);
    else if (!AutoView [lpPDevice->Setup.Plotter] && lpPDevice->Setup.
      PaperFeed == 0)
    {
        PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "PU",2);
        if (lpPDevice->Setup.Plotter == 0 || lpPDevice->Setup.Plotter == 1 ||
          lpPDevice->Setup.Plotter == 2)
            PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "0,0",3);
        else
        {
            if (lpPDevice->Setup.Size == 0 || lpPDevice->Setup.Size == 5)
                PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "0,7796",6);
            else
                PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "0,0",3);
        }
        PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) ";",1);
    }
    else
        PutJob (lpPDevice,lpPDevice->hJob,"PG;",3);
    EndPlotPage (lpPDevice,lpPDevice->hJob);
    lpPDevice->NumBandsCompleted = 0;
    lpPDevice->PageStarted = FALSE;
    lpPDevice->bFirstPage  = FALSE;
}

LOCAL void NEAR PASCAL prompt_for_page (lpPDevice)
LPPDEVICE lpPDevice;
{
    HANDLE hPrompt;
    LPSTR lpPrompt, lpTempPrompt;
#ifdef NICK_DEBUG
    char buf[64];
#endif    

    if ((hPrompt = GlobalAlloc(GHND, 256)) == NULL)
      return;

    lpPrompt = lpTempPrompt = GlobalLock(hPrompt);

    lpTempPrompt += lstrlen (lstrcpy(lpTempPrompt,(LPSTR)GetString(COLORPRO + 
                            lpPDevice->Setup.Plotter)));
#ifdef NICK_DEBUG
    wsprintf(buf, "Plotter %d \n\r", lpPDevice->Setup.Plotter);
    OutputDebugString(buf);
#endif

    lpTempPrompt += lstrlen (lstrcpy((LPSTR)lpTempPrompt,(LPSTR) GetString(NEWSHEET)));

    if (ShortEdge [lpPDevice->Setup.Plotter] [lpPDevice->Setup.Size])
       lpTempPrompt += lstrlen(lstrcpy((LPSTR)lpTempPrompt,(LPSTR)GetString(SHORTEDGE)));

    if (!lpPDevice->bFirstPage)
    {
       lpTempPrompt += lstrlen(lstrcpy((LPSTR)lpTempPrompt,(LPSTR)GetString(PAGE_MSG1)));
       lstrcpy((LPSTR)lpTempPrompt,(LPSTR)GetString(PAGE_MSG2));
    }
    PutDialog(lpPDevice,lpPDevice->hJob,(LPSTR)lpPrompt,lstrlen((LPSTR)
               lpPrompt));

    GlobalUnlock(hPrompt);
    GlobalFree(hPrompt);
}

LOCAL void NEAR PASCAL prompt_for_carousel (lpPDevice,Carousel)
LPPDEVICE lpPDevice;
short Carousel;
{
    char Prompt [BUFSIZE];
    short Length;

    PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "PAPUSP;",7);
    Length = lstrlen (lstrcpy ((LPSTR) Prompt,(LPSTR) GetString (HP)));
    Length += lstrlen (lstrcpy ((LPSTR) &Prompt [Length],(LPSTR) GetString
      (COLORPRO + lpPDevice->Setup.Plotter)));
    Prompt [Length++] = ':';
    Prompt [Length++] = ' ';
    Length += lstrlen (lstrcpy ((LPSTR) &Prompt [Length],(LPSTR) GetString
      (NEWCAROUSEL)));
    ltoa((LPSTR) &Prompt [Length],Carousel);
    PutDialog (lpPDevice,lpPDevice->hJob,(LPSTR) Prompt,lstrlen ((LPSTR)
      Prompt));
}

static void NEAR PASCAL output_number (lpPDevice,Number)
LPPDEVICE lpPDevice;
int Number;
{
    char Coordinate [7];

    ltoa((LPSTR) Coordinate, Number);
    PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) Coordinate,lstrlen ((LPSTR)
      Coordinate));
}

LOCAL void NEAR PASCAL select_pen (lpPDevice,hJob,Pen,PhysColor)
LPPDEVICE lpPDevice;
short hJob,
      Pen;
DWORD
      PhysColor;
{
    char Number[7];
    short Value, 
          Type
,
          Carousel = lpPDevice->Setup.CurrentCarousel,
          Group = get_group (Type = lpPDevice->Setup.Carousel [Carousel].Pen
            [Pen - 1].Type,(LPENVIRONMENT)&lpPDevice->Setup);

    if (lpPDevice->LineInConstruction)
    {
        PutJob (lpPDevice,hJob,(LPSTR) "PAPU",4);
        lpPDevice->LineInConstruction = FALSE;
    }

    PutJob (lpPDevice,hJob,(LPSTR) "SP",2);
    ltoa((LPSTR) Number, Pen);
    PutJob (lpPDevice,hJob,(LPSTR) Number,1);

    if ((Value = lpPDevice->Setup.Carousel [Carousel].Pen [Pen - 1].Speed) == 0)
        Value = DefDeviceInfo [lpPDevice->Setup.Plotter].Speed [Group];
    if (lpPDevice->CurrentSpeed != Value)
    {
        lpPDevice->CurrentSpeed = Value;
        PutJob (lpPDevice,hJob,(LPSTR) "VS",2);
        ltoa((LPSTR) Number, Value);
     /*   if (Value != DefDeviceInfo [lpPDevice->Setup.Plotter].Speed [Group] ||
          (Value == DefDeviceInfo [lpPDevice->Setup.Plotter].Speed [Group] &&
          DeviceInfo [lpPDevice->Setup.Plotter].OptionsSupport [2])) */
            PutJob (lpPDevice,hJob,(LPSTR) Number,lstrlen ((LPSTR) Number));
    }
    if (DeviceInfo [lpPDevice->Setup.Plotter].OptionsSupport [1])
    {
        if ((Value = lpPDevice->Setup.Carousel [Carousel].Pen [Pen - 1].Force)
          == 0)
            Value = DefDeviceInfo [lpPDevice->Setup.Plotter].Force [Group];
        if (lpPDevice->CurrentForce != Value)
        {
            lpPDevice->CurrentForce = Value;
            PutJob (lpPDevice,hJob,(LPSTR) "FS",2);
            ltoa((LPSTR) Number,Value);
            PutJob (lpPDevice,hJob,(LPSTR) Number,lstrlen ((LPSTR) Number));
        }
    }
    if (DeviceInfo [lpPDevice->Setup.Plotter].OptionsSupport [2])
    {
        if ((Value = lpPDevice->Setup.Carousel [Carousel].Pen [Pen - 1].
          Acceleration) == 0)
            Value = DefDeviceInfo [lpPDevice->Setup.Plotter].Acceleration
              [Group];
        if (lpPDevice->CurrentAcceleration != Value)
        {
            lpPDevice->CurrentAcceleration = Value;
            PutJob (lpPDevice,hJob,(LPSTR) "AS",2);
            ltoa((LPSTR) Number, Value);
            PutJob (lpPDevice,hJob,(LPSTR) Number,lstrlen ((LPSTR) Number));
        }
    }
    lpPDevice->CurrentPenThickness = PenPixelThickness [Type];
    if (PenThickness [lpPDevice->Setup.Plotter])
    {
        PSTR Thickness;

        PutJob (lpPDevice,hJob,(LPSTR) "PT",2);
        Thickness = GetString (P3 + Type);
        if (Type != D100 - P3)
            Thickness++;
        PutJob (lpPDevice,hJob,(LPSTR) Thickness,lstrlen ((LPSTR) Thickness));
    }
}
/* ************************** Exported Routines **************************** */

int FAR PASCAL abort_doc (lpPDevice)
LPPDEVICE lpPDevice;
{
    if (lpPDevice->hJob)
    {
        if (lpPDevice->PageStarted)
            EndPlotPage (lpPDevice,lpPDevice->hJob);
        EndJob (lpPDevice,lpPDevice->hJob);
    }
    lpPDevice->hJob = NULL;
    lpPDevice->PageStarted = FALSE;
    return (1);
}

int FAR PASCAL device_control (lpPDevice,EscNum,lpInData,lpOutData)
LPPDEVICE lpPDevice;
short EscNum;
LPSTR lpInData,
      lpOutData;
{
    return (0);
}

int FAR PASCAL draft_mode (lpPDevice,lpSwitch)
LPPDEVICE lpPDevice;
LPINT lpSwitch;
{
    return (0);
}

int FAR PASCAL end_doc (lpPDevice)
LPPDEVICE lpPDevice;
{
#ifdef HPGL2
    gend();
#endif
    if (lpPDevice->PageStarted)
        end_page (lpPDevice);
    else if (lpPDevice->SpoolPageStarted)
        EndPlotPage (lpPDevice,lpPDevice->hJob);
    EndJob (lpPDevice,lpPDevice->hJob);
    return (1);
}

int FAR PASCAL flush_output (lpPDevice)
LPPDEVICE lpPDevice;
{
    return (1);
}

int FAR PASCAL get_color_table (lpPDevice,lpIndex,lpColor)
LPPDEVICE lpPDevice;
LPINT lpIndex;
DWORD FAR *lpColor;
{
	*lpColor = lpPDevice->Setup.Carousel [lpPDevice->Setup.CurrentCarousel].
	  Pen [*lpIndex].Color;
    return (1);
}

int FAR PASCAL get_phys_page_size (lpPDevice,lpPageSize)
LPPDEVICE lpPDevice;
LPPOINT lpPageSize;
{
    /* THE OLD WAY */
    lpPageSize->x = (int)((long) PhysicalPageSize [lpPDevice->Setup.Size].x * 40 /
      RESOLUTION);
    if (lpPDevice->Setup.Size < 10)
        lpPageSize->y = (int)((long) PhysicalPageSize [lpPDevice->Setup.Size].y * 40 /
          RESOLUTION);
    else
        lpPageSize->y = (int)((long) lpPDevice->Setup.Length * 254 * 40 / (10
          * RESOLUTION));
    /**/


/*
    THE NEW WAY

    w = Corner [lpPDevice->Setup.Plotter] [lpPDevice->Setup.Size].bottom 
      - Corner [lpPDevice->Setup.Plotter] [lpPDevice->Setup.Size].top   ;
    if (w < 0) w *= -1;
    h = Corner [lpPDevice->Setup.Plotter] [lpPDevice->Setup.Size].right
      - Corner [lpPDevice->Setup.Plotter] [lpPDevice->Setup.Size].left  ;
    if (h < 0) h *= -1;

    w = w * 254 / 10160; 
    h = h * 254 / 10160;
    w = w * 40 / RESOLUTION;
    h = h * 40 / RESOLUTION;
    if (w > h) { t = w; w = h; h = t; }
    lpPageSize->x = w;
    lpPageSize->y = h;

*/
    if (lpPDevice->Setup.Orientation == 1)
    {
        short Temp;

        Temp = lpPageSize->x;
        lpPageSize->x = lpPageSize->y;
        lpPageSize->y = Temp;
    }

    return (1);
}

int FAR PASCAL get_printing_offset (lpPDevice,lpPrintOffset)
LPPDEVICE lpPDevice;
LPPOINT lpPrintOffset;
{
    short Plotter = lpPDevice->Setup.Plotter,
          PageSize = lpPDevice->Setup.Size,
          a = PrintOffset [Plotter] [PageSize] [0] * STEPSPERMM / RESOLUTION,
          b = PrintOffset [Plotter] [PageSize] [1] * STEPSPERMM / RESOLUTION,
          c = PrintOffset [Plotter] [PageSize] [2] * STEPSPERMM / RESOLUTION;

    if (lpPDevice->Setup.Orientation == 0) /* Portrait */
    {
        lpPrintOffset->x = a;
        lpPrintOffset->y = b;
    }
    else
    {
        lpPrintOffset->x = b;
        lpPrintOffset->y = c;
    }
    return (1);
}

int FAR PASCAL get_vector_pen_size (lpPDevice,lpPoints)
LPPDEVICE lpPDevice;
LPPOINT lpPoints;
{
#if 0
    lpPoints->x = lpPoints->y = lpPDevice->CurrentPenThickness;
    return (1);
//  ssn -- driver crashes if we report pen width which is not 1 
#endif

    lpPoints->x = lpPoints->y = 1;
    return (1);

}

int FAR PASCAL new_frame (lpPDevice)
LPPDEVICE lpPDevice;
{
int
    result;

    if (lpPDevice->PageStarted)
        EndPlotPage (lpPDevice,lpPDevice->hJob);
    result = StartPlotPage (lpPDevice,lpPDevice->hJob);
    if (result < 0) return (result);

    /* commented out the next 3 lines.  next_band should take care of */
    /* prompting for the page. - ssn 6/26/91 */
    /* if (lpPDevice->Setup.PaperFeed == 0) */
    /*      prompt_for_page (lpPDevice); */
    /* lpPDevice->PageStarted = TRUE; */
#ifdef NICK_DEBUG
    OutputDebugString("Prompting for page in new_frame\r\n");
#endif
    lpPDevice->NumBandsCompleted = 0;
    return (1);
}

int FAR PASCAL next_band (lpPDevice,lpBandRect)
LPPDEVICE lpPDevice;
LPRECT lpBandRect;
{
    int result;
    long BandOffset = lpPDevice->BandOffset;
    short NumBands = lpPDevice->NumBands,
          NumBandsCompleted = ++lpPDevice->NumBandsCompleted;

    if (lpPDevice->LineInConstruction)
    {
        PutJob (lpPDevice,lpPDevice->hJob,"PA",2);
        lpPDevice->LineInConstruction = FALSE;
    }
    SetRectEmpty (lpBandRect);
    if (NumBandsCompleted <= lpPDevice->NumBands)
    {
        if (!lpPDevice->PageStarted)
        {
            result = StartPlotPage (lpPDevice,lpPDevice->hJob);
            if (result < 0) return(result);

            if ((lpPDevice->Setup.PaperFeed == 0) && (!lpPDevice->bFile))
                prompt_for_page (lpPDevice);
            lpPDevice->PageStarted = TRUE;
        }
        if (lpPDevice->Setup.Size > 9)          /* Roll Feed */
        {
            if (lpPDevice->Setup.Orientation == 0)      /* Portrait */
            {
                lpBandRect->top = (NumBands == NumBandsCompleted) ? 0 :
                  lpPDevice->LastBandLength + (NumBands - NumBandsCompleted - 1)
                  * lpPDevice->LengthRes;
                lpBandRect->right = lpPDevice->HorzRes;
                lpBandRect->bottom = lpBandRect->top + (NumBands ==
                  NumBandsCompleted ? lpPDevice->LastBandLength : lpPDevice->
                  LengthRes);
            }
            else
            {
                lpBandRect->left = (NumBands == NumBandsCompleted) ? 0 :
                  lpPDevice->LastBandLength + (NumBands - NumBandsCompleted - 1)
                  * lpPDevice->LengthRes;
                lpBandRect->bottom = lpPDevice->VertRes;
                lpBandRect->right = lpBandRect->left + (NumBands ==
                  NumBandsCompleted ? lpPDevice->LastBandLength : lpPDevice->
                  LengthRes);
            }
            lpPDevice->BandOffset = (long) lpPDevice->LengthRes / 2 -
              lpPDevice->LastBandLength;
            if (NumBands != NumBandsCompleted)
                lpPDevice->BandOffset -= (NumBands - NumBandsCompleted) *
                  lpPDevice->LengthRes;
            if (NumBandsCompleted != 1)
            {
                PutJob (lpPDevice,lpPDevice->hJob,"FR",2);
                set_p1p2 (lpPDevice,(LPRECT) &Corner [lpPDevice->Setup.Plotter]
                  [lpPDevice->Setup.Size]);
            }
        }
        else
        {
            lpBandRect->bottom = lpPDevice->VertRes;
            lpBandRect->right = lpPDevice->HorzRes;
        }
    }
    else
        end_page (lpPDevice);
    return (1);
}

#ifdef COLORSORTING
BOOL FAR PASCAL next_color (lpPDevice,PhysColor)
LPPDEVICE lpPDevice;
DWORD PhysColor;
{
    BOOL Result = TRUE;
    if (lpPDevice->ColorTableBuilt)
    {
        if ((short)(HIWORD(PhysColor)) != lpPDevice->Setup.CurrentCarousel)
        {
            if (lpPDevice->LocatingPen)
            {
                if (lpPDevice->SavePhysColor != PhysColor)
                    Result = FALSE;
                else
                {
                    lpPDevice->Setup.CurrentCarousel = HIWORD (PhysColor);
                    prompt_for_carousel (lpPDevice,HIWORD (PhysColor) + 1);
                }
            }
            else
            {
                lpPDevice->LocatingPen = TRUE;
                lpPDevice->SavePhysColor = PhysColor;
                Result = FALSE;
            }
        }
    }
    else
        Result = FALSE;
    if (Result)
    {
        select_pen (lpPDevice,lpPDevice->hJob,LOWORD (PhysColor), PhysColor);
        lpPDevice->LocatingPen = FALSE;
    }
    return (Result);
}
#endif

int FAR PASCAL set_abort_proc (lpPDevice,lpHDC)
LPPDEVICE lpPDevice;
HANDLE FAR *lpHDC;
{
    lpPDevice->hDC = *lpHDC;
    return (1);
}

int FAR PASCAL set_color_table (lpPDevice,lpColorTableEntry)
LPPDEVICE lpPDevice;
LPCOLORTABLEENTRY lpColorTableEntry;
{
    return (1);
}

void FAR PASCAL set_p1p2 (lpPDevice,lpCorner)
LPPDEVICE lpPDevice;
LPRECT lpCorner;
{
    PutJob (lpPDevice,lpPDevice->hJob,"IP",2);
    output_number (lpPDevice,lpCorner->left);
    PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) ",",1);
    output_number (lpPDevice,lpCorner->top);
    PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) ",",1);
    output_number (lpPDevice,lpCorner->right);
    PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) ",",1);
    output_number (lpPDevice,lpCorner->bottom);
}

int FAR PASCAL start_doc (lpPDevice,lpDocument,lpDocInfo)
LPPDEVICE lpPDevice;
LPSTR lpDocument;
LPSTR lpDocInfo;
{
int result;

    // check to see if lpDocInfo contains a valid filename, if so
    // use it.
    lpDocInfo = (lpDocInfo ? (LPSTR)((LPDOCINFO)lpDocInfo)->lpszOutput
                             : NULL);

    // if printing to FILE: then set flag so that we don't bring
    // up paper prompt dialog

    lpPDevice->bFile = FALSE;

    if ( (!lstrcmp((LPSTR)lpPDevice->OutputFile, "FILE")) ||
         (!lstrcmp((LPSTR)lpPDevice->OutputFile, "FILE:")) )
       lpPDevice->bFile = TRUE;

    lpPDevice->hJob = StartJob (lpPDevice,
                                (LPSTR) (lpDocInfo ?
                                lpDocInfo : lpPDevice->OutputFile),
                                lpDocument,
                                lpPDevice->hDC);

    result = StartPlotPage (lpPDevice,lpPDevice->hJob);
    if (result < 0) return (result);

    lpPDevice->bFirstPage  = TRUE;
    lpPDevice->PageStarted = TRUE;
    lpPDevice->LocatingPen = FALSE;

    if ((!lpPDevice->Setup.Preloaded && lpPDevice->Setup.PaperFeed == 0) &&
        (!lpPDevice->bFile))
        prompt_for_page (lpPDevice);

    PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "\x03IN",3);
    if (lpPDevice->Setup.Plotter + COLORPRO == HP7475A)
    {
        if (lpPDevice->Setup.Size + SIZE_A == SIZE_A || lpPDevice->Setup.Size
          + SIZE_A == SIZE_A4)
            PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "PS4",3);
        else
            PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "PS3",3);
    }
    if (lpPDevice->Setup.Plotter + COLORPRO == HP7580A || lpPDevice->Setup.
      Plotter + COLORPRO == HP7585A)
        PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "\x1B.@;1:",6);
    lpPDevice->CurrentHeight = 0;
    if (lpPDevice->Setup.PaperFeed == 1)
        PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "PG",2);
    lpPDevice->StandardCharSetActive = TRUE;
    if (CharSetSevenSupport [lpPDevice->Setup.Plotter])
    {
        PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "CA7",3);
        lpPDevice->AltCharSet = 7;
    }
    else
    {
        PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "CA2",3);
        lpPDevice->AltCharSet = 2;
    }
    set_p1p2 (lpPDevice,(LPRECT) &Corner [lpPDevice->Setup.Plotter]
      [lpPDevice->Setup.Size]);
    if (lpPDevice->Setup.Plotter == HP7550A - COLORPRO)
        PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "GM8776,0,0,3000",15);
    else if (lpPDevice->Setup.Plotter == DRAFTPRO - COLORPRO)
        PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "GM2816,0,0,0,3608",17);
    else if (lpPDevice->Setup.Plotter >= DRAFTMASTER1 - COLORPRO)
        PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "GM3584,0,0,3000,17992",21);
    if (lpPDevice->Setup.Orientation == 0)                   /* Portrait */
        PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "DI0,1",5);
#ifdef HPGL2
    gbegin();
#endif
    return (1);
}

int FAR PASCAL query_esc_support (lpPDevice,lpEscNum)
LPPDEVICE lpPDevice;
LPINT lpEscNum;
{
    switch (*lpEscNum)
    {
        case NEWFRAME:
        case FLUSHOUTPUT:
        case QUERYESCSUPPORT:
        case SETABORTPROC:
        case GETPHYSPAGESIZE:
        case GETPRINTINGOFFSET:
        case STARTDOC:
        case ENDDOC:
        case ABORTDOC:
        case NEXTBAND:
        case GETCOLORTABLE:
            return (TRUE);

        default:
            return (FALSE);
    }
}
