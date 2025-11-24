/* (c) Copyright 1987 MICROGRAFX, Inc., All Rights Reserved.  Permission to
   use this work for any purpose must be obtained in writing from MICROGRAFX,
   1820 N. Greenville Ave., Richardson, Tx.  75081.

*******************************************************************************
*******************************************************************************

                                  output

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
#include "fill.h"
#include "glib.h"
#include "output.h"
#include "print.h"
#include "utils.h"
#include <memory.h>

/* ****************************** History ********************************** */
/* 10/31/87 (RWM) - signoff                                                  */
/* 08/31/89 (NVF) - Made it so that the handle returned by GlobalReAlloc was */
/*                  no longer assumed to be the same. (Needed for WIN3.0)    */
/* ***************************** Constants ********************************* */

#define LOCAL
#define THRESHOLD 12
/* **************************** Local Data ********************************* */

LOCAL short StyleTable [5] =
{
    0,     /* Solid */
    2,     /* Dash */
    1,     /* Dot */
    5,     /* Dash Dot */
    6      /* Dash Dot Dot */
};

LOCAL short StyleLengthTable [5] [5] =
{
    { 0, 50, 100, 400, 400 },   /* Size A media */
    { 0, 34, 69, 275, 275 },    /* Size B media */
    { 0, 25, 50, 200, 200 },    /* Size C media */
    { 0, 17, 34, 137, 137 },    /* Size D media */
    { 0, 12, 25, 100, 100 }     /* Size E media */
};
/* *************************** Exported Data ******************************* */

DEFDEVICEINFO DefDeviceInfo [NUM_PLOTTERS] =
{
    {
    /* ColorPro */
        { 40, 10, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 }
    },
    {
    /* ColorPro with GEC */
        { 40, 10, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 }
    },
    {
    /* HP 7470A */
        { 38, 10, 0, 0, 15 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 }
    },
    /* HP 7475A */
    {
        { 38, 10, 0, 0, 15 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 }
    },
    /* HP 7550A */
    {
        { 50, 10, 60, 20, 15 },
        { 2, 2, 6, 2, 1 },
        { 6, 6, 6, 6, 6 }
    },
    /* HP 7580A */
    {
        { 40, 10, 0, 0, 0 },
        { 2, 0, 6, 2, 1 },
        { 0, 0, 0, 0, 0 }
    },
    /* HP 7585A */
    {
        { 40, 10, 0, 0, 0 },
        { 2, 0, 6, 2, 1 },
        { 0, 0, 0, 0, 0 }
    },
    /* HP 7580B */
    {
        { 50, 0, 60, 30, 15 },
        { 2, 0, 6, 2, 1 },
        { 4, 4, 4, 4, 4 }
    },
    /* HP 7585B */
    {
        { 50, 0, 60, 30, 15 },
        { 2, 0, 6, 2, 1 },
        { 4, 4, 4, 4, 4 }
    },
    /* HP 7586B */
    {
        { 50, 0, 60, 30, 15 },
        { 2, 0, 6, 2, 1 },
        { 4, 4, 4, 4, 4 }
    },
    /* DraftPro */
    {
        { 40, 0, 0, 20, 15 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 }
    },
    /* DraftPro DXL */
    {
        { 40, 0, 0, 20, 15 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 }
    },
    /* DraftPro EXL */
    {
        { 40, 0, 0, 20, 15 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 }
    },
    /* DraftMaster I */
    {
        { 50, 10, 60, 30, 30 },
        { 2, 2, 6, 3, 3 },
        { 4, 4, 4, 4, 4 }
    },
    /* DraftMaster II */
    {
        { 50, 10, 60, 30, 30 },
        { 2, 2, 6, 3, 3 },
        { 4, 4, 4, 4, 4 }
    }
};
/* *************************** Local Routines ****************************** */
LOCAL short NEAR PASCAL arc_to_polyline (LPPDEVICE,LPPOINT,LPPOINT,LPPOINT,int,LPPOINT);
LOCAL void NEAR PASCAL check_fill_style (LPPDEVICE,short,short,LPPOINT,short);
LOCAL void NEAR PASCAL check_pen_style (LPPDEVICE,short);
LOCAL short NEAR PASCAL get_angle (LPPOINT,LPPOINT);
LOCAL short NEAR PASCAL get_fill_angle (LPPOLYSET,short);
LOCAL void NEAR PASCAL make_float (LPPDEVICE,short,short);
LOCAL void NEAR PASCAL output_number (LPPDEVICE,int);
LOCAL void NEAR PASCAL perform_fill (LPPDEVICE,LPPBRUSH,LPPOLYSET,short);
LOCAL void NEAR PASCAL point_on_ellipse (LPPOINT,LPPOINT,short,short,short);
LOCAL void NEAR PASCAL rect_to_polyline (LPRECT,LPPOINT);
LOCAL short NEAR PASCAL rid_null_points (LPPOINT,short);
LOCAL void NEAR PASCAL set_bounds (LPPDEVICE,LPPOINT);
LOCAL void NEAR PASCAL thin_arc (LPPDEVICE,LPPPEN,LPPOINT,LPPOINT,short,short);
LOCAL void NEAR PASCAL thin_ellipse (LPPDEVICE,LPPPEN,LPPOINT,short);
LOCAL void NEAR PASCAL thin_pie (LPPDEVICE,short,short,short);
LOCAL void NEAR PASCAL wide_arc (LPPDEVICE,LPPPEN,LPPOINT,LPPOINT,LPPOINT);
LOCAL void NEAR PASCAL wide_ellipse (LPPDEVICE,LPPPEN,LPPOINT);
LOCAL void NEAR PASCAL wide_pie (LPPDEVICE,LPPPEN,LPPOINT,short,short,short);
LOCAL void NEAR PASCAL wide_polyline (LPPDEVICE,short,LPPOINT,LPPPEN);

LOCAL short NEAR PASCAL arc_to_polyline (lpPDevice,lpCenter,lpSize,lpAngles,
  Step,lpBuffer)
  /* This function will calculate points on an arc and place them into the
     buffer pointed to by lpBuffer.  The point structure pointed to by lpSize
     contains the horizontal (x) and vertical (y) radius of the elliptical arc,
     the point structure pointed to by lpSize contains the start (x) and
     stop (y) angles, and Step is a short value containing the step size for
     each point calculation. lpCenter points to the point structure containing
     the X and Y center of the elliptical arc. */
LPPDEVICE lpPDevice;
LPPOINT lpCenter,
        lpSize,
        lpAngles;
int Step;
LPPOINT lpBuffer;
{
    short Angle,
          EndAngle = lpAngles->y,
          Result = 0;

    if (Step > 0 && lpAngles->y < lpAngles->x)
        EndAngle += 3600;
    else if (Step < 0 && lpAngles->y > lpAngles->x)
        EndAngle -= 3600;
    for (Angle = lpAngles->x; Step > 0 ? Angle <= EndAngle : EndAngle <= Angle;
      Angle += Step)
    {
        short WorkAngle = (Angle + 3600) % 3600;

        lpBuffer->x = lpCenter->x + (short)(((long) Cosine (WorkAngle) * lpSize->x /
                      TRIG_SCALE));
        lpBuffer->y = lpCenter->y + (short)(((long) Sine (WorkAngle) * lpSize->y /
                      TRIG_SCALE));
        lpBuffer++;
        Result++;
    }
    if ((Angle - Step) != EndAngle)
    {
        lpBuffer->x = lpCenter->x + (short)(((long) Cosine (EndAngle) * lpSize->x /
                      TRIG_SCALE));
        lpBuffer->y = lpCenter->y + (short)(((long) Sine (EndAngle) * lpSize->y /
                      TRIG_SCALE));
        Result++;
    }
    return (Result);
}

LOCAL void NEAR PASCAL check_fill_style (lpPDevice,Style,Hatch,lpPoints,Count)
LPPDEVICE lpPDevice;
short Style,
      Hatch;
LPPOINT lpPoints;
short Count;
{
    check_pen_style (lpPDevice,PS_SOLID);
    if (Style == BS_SOLID)
    {
        POINT Bounds [2];

        get_bounds (lpPDevice,(LPPOINT) Bounds,lpPoints,Count);
        if (ABS (Bounds [0].x - Bounds [1].x) >= ABS (Bounds [0].y - Bounds
          [1].y) && lpPDevice->CurrentBrushAngle != 0)
        {
            PutJob (lpPDevice,lpPDevice->hJob,"FT1,0,0",7);
            lpPDevice->CurrentBrushAngle = 0;
        }
        else if (ABS (Bounds [0].x - Bounds [1].x) < ABS (Bounds [0].y - Bounds
          [1].y) && lpPDevice->CurrentBrushAngle != 90)
        {
            PutJob (lpPDevice,lpPDevice->hJob,"FT1,0,90",8);
            lpPDevice->CurrentBrushAngle = 90;
        }
        lpPDevice->CurrentBrushStyle = Style;
        lpPDevice->CurrentBrushHatch = -1;
    }
    else if (Style == BS_HATCHED && lpPDevice->CurrentBrushHatch != Hatch)
    {
        lpPDevice->CurrentBrushAngle = -1;
        lpPDevice->CurrentBrushStyle = Style;
        lpPDevice->CurrentBrushHatch = Hatch;
        if ((Hatch == HS_HORIZONTAL && lpPDevice->Setup.Orientation == 0) ||
          (Hatch == HS_VERTICAL && lpPDevice->Setup.Orientation == 1))
            PutJob (lpPDevice,lpPDevice->hJob,"FT3,120,90",10);
        else if ((Hatch == HS_VERTICAL && lpPDevice->Setup.Orientation == 0) ||
          (Hatch == HS_HORIZONTAL && lpPDevice->Setup.Orientation == 1))
            PutJob (lpPDevice,lpPDevice->hJob,"FT3,120,0",9);
        else if ((Hatch == HS_FDIAGONAL && lpPDevice->Setup.Orientation == 0) ||
          (Hatch == HS_BDIAGONAL && lpPDevice->Setup.Orientation == 1))
            PutJob (lpPDevice,lpPDevice->hJob,"FT3,90,45",9);
        else if ((Hatch == HS_BDIAGONAL && lpPDevice->Setup.Orientation == 0) ||
          (Hatch == HS_FDIAGONAL && lpPDevice->Setup.Orientation == 1))
            PutJob (lpPDevice,lpPDevice->hJob,"FT3,90,135",10);
        else if (Hatch == HS_CROSS)
            PutJob (lpPDevice,lpPDevice->hJob,"FT4,120,0",9);
        else
            PutJob (lpPDevice,lpPDevice->hJob,"FT4,90,45",9);
    }
}

LOCAL void NEAR PASCAL check_pen_style (lpPDevice,PenStyle)
LPPDEVICE lpPDevice;
short PenStyle;
{
    if (lpPDevice->CurrentPenStyle != PenStyle)
    {
        if (lpPDevice->LineInConstruction)
            lpPDevice->LineInConstruction = FALSE;
        PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "LT",2);
        if (PenStyle != 0)
        {
            char String [3];
            short MediaSize = lpPDevice->Setup.Size > 4 ?
              lpPDevice->Setup.Size - 5 : lpPDevice->Setup.Size;

            ltoa((LPSTR) String, StyleTable [PenStyle]);
            PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) String,1);
            PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) ",",1);
            construct_float (lpPDevice,lpPDevice->hJob,StyleLengthTable
              [MediaSize] [PenStyle],2);
        }
        lpPDevice->CurrentPenStyle = PenStyle;
    }
}

LOCAL short NEAR PASCAL get_angle (Center,Point)
LPPOINT Center,
        Point;
{
    return (Arctan (Point->x - Center->x,Point->y - Center->y,1,1));
}

LOCAL short NEAR PASCAL get_fill_angle (lpPolyset,nPolysets)
LPPOLYSET lpPolyset;
short nPolysets;
{
    RECT Bounds;
    LPPOINT lpPoints;
    short CurPolyset,
          nPoints,
          Index;

    for (CurPolyset = 0; CurPolyset < nPolysets; CurPolyset++,lpPolyset++)
    {
        lpPoints = lpPolyset->lpPoints;
        nPoints = lpPolyset->nPoints;
        if (CurPolyset == 0)
        {
            Bounds.right = Bounds.left = lpPoints->x;
            Bounds.top = Bounds.bottom = (lpPoints++)->y;
        }
        for (Index = 1; Index < nPoints; Index++,lpPoints++)
        {
            if (lpPoints->x < Bounds.left)
                Bounds.left = lpPoints->x;
            else if (lpPoints->x > Bounds.right)
                Bounds.right = lpPoints->x;
            if (lpPoints->y < Bounds.top)
                Bounds.top = lpPoints->y;
            else if (lpPoints->y > Bounds.bottom)
                Bounds.bottom = lpPoints->y;
        }
    }
    if ((Bounds.bottom - Bounds.top) > (Bounds.right - Bounds.left))
        return (900);
    else
        return (0);
}

LOCAL void NEAR PASCAL make_float (lpPDevice,hJob,Number)
LPPDEVICE lpPDevice;
short hJob,
      Number;
{
    char Coordinate [7];
    short Index = 0;

    ltoa((LPSTR) Coordinate, Number);
    while (Coordinate [Index] != 0)
        Index++;
    Coordinate [Index] = Coordinate [Index - 1];
    Coordinate [Index - 1] = '.';
    Coordinate [Index + 1] = '\0';
    PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) Coordinate,lstrlen ((LPSTR)
      Coordinate));
}

LOCAL void NEAR PASCAL output_number (lpPDevice,Number)
LPPDEVICE lpPDevice;
int Number;
{
    char Coordinate [7];

    ltoa((LPSTR) Coordinate, Number);
    PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) Coordinate,lstrlen ((LPSTR)
      Coordinate));
}

LOCAL void NEAR PASCAL perform_fill (lpPDevice,lpPBrush,lpPolysets,nPolysets)
LPPDEVICE lpPDevice;
LPPBRUSH lpPBrush;
LPPOLYSET lpPolysets;
short nPolysets;
{
    PPEN WorkPhysPen;

    WorkPhysPen.PhysicalColor = lpPBrush->PhysicalColor;
    WorkPhysPen.Style = PS_SOLID;
    WorkPhysPen.Width.x = 0;
    lpPDevice->CurPen = &WorkPhysPen;
    check_pen_style (lpPDevice,PS_SOLID);
    if (lpPBrush->Style == BS_SOLID)
        fill (lpPDevice,lpPolysets,nPolysets,lpPDevice->CurrentPenThickness,
          get_fill_angle ((LPPOLYSET) lpPolysets,nPolysets),TRUE,FALSE);
    else if (lpPBrush->Style == BS_HATCHED)
    {
        short Hatch = lpPBrush->Hatch;

        if (Hatch == HS_HORIZONTAL || Hatch == HS_CROSS)
            fill (lpPDevice,lpPolysets,nPolysets,24,0,TRUE,FALSE);
        if (Hatch == HS_VERTICAL || Hatch == HS_CROSS)
            fill (lpPDevice,lpPolysets,nPolysets,24,900,TRUE,FALSE);
        if (Hatch == HS_BDIAGONAL || Hatch == HS_DIAGCROSS)
            fill (lpPDevice,lpPolysets,nPolysets,18,1350,TRUE,FALSE);
        if (Hatch == HS_FDIAGONAL || Hatch == HS_DIAGCROSS)
            fill (lpPDevice,lpPolysets,nPolysets,18,450,TRUE,FALSE);
    }
    PutJob (lpPDevice,lpPDevice->hJob,"PUPA",4);
    lpPDevice->LineInConstruction = FALSE;
}

LOCAL void NEAR PASCAL point_on_ellipse (lpDest,lpCenter,Angle,Width,Height)
LPPOINT lpDest,
        lpCenter;
short   Angle,
        Width,
        Height;
{
    lpDest->x = lpCenter->x + (short)(((long) Cosine (Angle) * Width / 16384));
    lpDest->y = lpCenter->y + (short)(((long) Sine (Angle) * Height / 16384));
}

LOCAL void NEAR PASCAL rect_to_polyline (lpRect,lpPoints)
LPRECT lpRect;
LPPOINT lpPoints;
{
    lpPoints->x = lpRect->right;
    lpPoints->y = lpRect->top;
    (lpPoints + 1)->x = lpRect->right;
    (lpPoints + 1)->y = lpRect->bottom;
    (lpPoints + 2)->x = lpRect->left;
    (lpPoints + 2)->y = lpRect->bottom;
    (lpPoints + 3)->x = lpRect->left;
    (lpPoints + 3)->y = lpRect->top;
}

LOCAL short NEAR PASCAL rid_null_points (lpPoints,nPoints)
LPPOINT lpPoints;
short nPoints;
{
    short Index;

    for (Index = 1; Index < nPoints; Index++)
    {
        while ((lpPoints + Index)->x == (lpPoints + Index - 1)->x && (lpPoints +
          Index)->y == (lpPoints + Index - 1)->y && Index < nPoints)
        {
            if ((--nPoints - Index) > 0)
                _fmemcpy ((LPSTR) (lpPoints + Index),(LPSTR) (lpPoints + Index
                  + 1),sizeof (POINT) * (nPoints - Index));
        }
    }
    return (nPoints);
}

LOCAL void NEAR PASCAL set_bounds (lpPDevice,lpBounds)
LPPDEVICE lpPDevice;
LPPOINT lpBounds;
{
    PutJob (lpPDevice,lpPDevice->hJob,"IP",2);
    construct_point (lpPDevice,(LPPOINT) lpBounds++,FALSE);
    PutJob (lpPDevice,lpPDevice->hJob,",",1);
    construct_point (lpPDevice,(LPPOINT) lpBounds,FALSE);
}

LOCAL void NEAR PASCAL thin_arc (lpPDevice,lpPPen,lpStart,lpPoints,StartAngle,
  EndAngle)
LPPDEVICE lpPDevice;
LPPPEN lpPPen;
LPPOINT lpStart,
        lpPoints;
short StartAngle,
      EndAngle;
{
    short SweepAngle = (EndAngle - StartAngle + 3600) % 3600;

    check_pen_style (lpPDevice,lpPPen->Style);
    PutJob (lpPDevice,lpPDevice->hJob,"PU",2);
    construct_point (lpPDevice,(LPPOINT) lpStart,FALSE);
    if (lpPDevice->Setup.Plotter == COLORPRO + HP7475A && lpPDevice->Setup.
      Orientation == 1)
    {                           /* Firmware bug-fix */
        POINT Points [2];

        Points [0].x = lpPoints->x;
        Points [0].y = (lpPoints + 1)->y;
        Points [1].x = (lpPoints + 1)->x;
        Points [1].y = lpPoints->y;
        set_bounds (lpPDevice,(LPPOINT) Points);
        PutJob (lpPDevice,lpPDevice->hJob,"SC0,2,2,0",9);
    }
    else
    {
        set_bounds (lpPDevice,lpPoints);
        PutJob (lpPDevice,lpPDevice->hJob,"SC0,2,0,2",9);
    }
    PutJob (lpPDevice,lpPDevice->hJob,"PDAA1,1,",8);
    if (lpPDevice->Setup.Orientation == 1)      /* Landscape */
        SweepAngle = -SweepAngle;
    output_number (lpPDevice,SweepAngle / 10);
    PutJob (lpPDevice,lpPDevice->hJob,"SCPU",4);
    set_p1p2 (lpPDevice,(LPRECT) &Corner [lpPDevice->Setup.Plotter]
      [lpPDevice->Setup.Size]);
}

LOCAL void NEAR PASCAL thin_ellipse (lpPDevice,lpPPen,lpPoints,nPoints)
LPPDEVICE lpPDevice;
LPPPEN lpPPen;
LPPOINT lpPoints;
short nPoints;
{
    check_pen_style (lpPDevice,lpPPen->Style);
    if (MyPolygon [lpPDevice->Setup.Plotter])
        PutJob (lpPDevice,lpPDevice->hJob,"EP",2);
    else if (Circle [lpPDevice->Setup.Plotter] && lpPPen->Style == PS_SOLID)
        PutJob (lpPDevice,lpPDevice->hJob,"CI1SC",5);
    else
    {
        PPEN WorkPhysPen;

        WorkPhysPen.PhysicalColor = lpPPen->PhysicalColor;
        WorkPhysPen.Style = lpPPen->Style;
        WorkPhysPen.Width.x = 0;
        draw_polyline (lpPDevice,nPoints,lpPoints,(LPPPEN) &WorkPhysPen,
          (LPPBRUSH) NULL,(LPDRAWMODE) NULL,(LPRECT) NULL);
    }
}

LOCAL void NEAR PASCAL thin_pie (lpPDevice,Width,StartAngle,SweepAngle)
LPPDEVICE lpPDevice;
short Width,
      StartAngle,
      SweepAngle;
{
    PutJob (lpPDevice,lpPDevice->hJob,"EW1,",4);
    make_float (lpPDevice,lpPDevice->hJob,StartAngle);
    PutJob (lpPDevice,lpPDevice->hJob,",",1);
    make_float (lpPDevice,lpPDevice->hJob,SweepAngle);
}

LOCAL void NEAR PASCAL wide_arc (lpPDevice,lpPPen,lpStart,lpCenter,lpInfo)
LPPDEVICE lpPDevice;
LPPPEN lpPPen;
LPPOINT lpStart,
        lpCenter,
        lpInfo;
{
    POLYSET PolySet;
    PPEN WorkPhysPen;
    POINT Border [100],
          InnerSize,
          OuterSize,
          CapSize,
          InnerAngles,
          OuterAngles,
          CapAngles,
          Angles1,
          Angles2,
          End;
    short HalfWidth = lpPPen->Width.x / 2,
          StartAngle = lpInfo->x,
          EndAngle = lpInfo->y,
          Offset = 0;

    check_pen_style (lpPDevice,PS_SOLID);
    WorkPhysPen.PhysicalColor = lpPPen->PhysicalColor;
    WorkPhysPen.Style = PS_SOLID;
    WorkPhysPen.Width.x = 0;
    InnerSize.x = (lpInfo + 1)->x - HalfWidth;
    InnerSize.y = (lpInfo + 1)->y - HalfWidth;
    OuterSize.x = InnerSize.x + lpPPen->Width.x;
    OuterSize.y = InnerSize.y + lpPPen->Width.y;
    CapSize.x = CapSize.y = HalfWidth;
    InnerAngles.x = (lpInfo->x + 100) / 100 * 100;
    InnerAngles.y = lpInfo->y / 100 * 100;
    OuterAngles.x = InnerAngles.y;
    OuterAngles.y = InnerAngles.x;
    Angles1.x = Angles1.y = lpInfo->x;
    Angles2.x = Angles2.y = lpInfo->y;
    End.x = lpCenter->x + (short)(((long) Cosine (EndAngle) * (lpInfo + 1)->x / 16384));
    End.y = lpCenter->y + (short)(((long) Sine (EndAngle) * (lpInfo + 1)->y / 16384));
    Offset += arc_to_polyline (lpPDevice,lpCenter,(LPPOINT) &InnerSize,(LPPOINT)
      &InnerAngles,100,(LPPOINT) &Border [Offset]);
    CapAngles.x = (EndAngle + 1800) % 3600;
    CapAngles.y = EndAngle;
    Offset += arc_to_polyline (lpPDevice,(LPPOINT) &End,(LPPOINT) &CapSize,
      (LPPOINT) &CapAngles,-200,(LPPOINT) &Border [Offset]);
    Offset += arc_to_polyline (lpPDevice,lpCenter,(LPPOINT) &OuterSize,(LPPOINT)
      &OuterAngles,-100,(LPPOINT) &Border [Offset]);
    CapAngles.x = StartAngle;
    CapAngles.y = (StartAngle + 1800) % 3600;
    Offset += arc_to_polyline (lpPDevice,(LPPOINT) lpStart,(LPPOINT) &CapSize,
      (LPPOINT) &CapAngles,-200,(LPPOINT) &Border [Offset]);
    Border [Offset++] = Border [0];
    if (!MyPolygon [lpPDevice->Setup.Plotter])
    {
        PolySet.lpPoints = (LPPOINT) Border;
        PolySet.nPoints = Offset;
        lpPDevice->CurPen = &WorkPhysPen;
        fill (lpPDevice,(LPPOLYSET) &PolySet,1,lpPDevice->CurrentPenThickness,
          get_fill_angle ((LPPOLYSET) &PolySet,1),TRUE,FALSE);
        draw_polyline (lpPDevice,Offset,Border,(LPPPEN) &WorkPhysPen,
          (LPPBRUSH) NULL,(LPDRAWMODE) NULL,(LPRECT) NULL);
    }
    else
    {
        check_fill_style (lpPDevice,BS_SOLID,0,(LPPOINT) Border,Offset);
        PutJob (lpPDevice,lpPDevice->hJob,"PU",2);
        construct_point (lpPDevice,(LPPOINT) Border,FALSE);
        lpPDevice->PenPos.x = Border [0].x;
        lpPDevice->PenPos.y = Border [0].y;
        PutJob (lpPDevice,lpPDevice->hJob,"PM0",3);
        draw_polyline (lpPDevice,Offset,(LPPOINT) Border,(LPPPEN) &WorkPhysPen,
          (LPPBRUSH) NULL,(LPDRAWMODE) NULL,(LPRECT) NULL);
        PutJob (lpPDevice,lpPDevice->hJob,"PM2PAPUFPEP",11);
        lpPDevice->LineInConstruction = FALSE;
    }
}

LOCAL void NEAR PASCAL wide_ellipse (lpPDevice,lpPPen,lpPoints)
LPPDEVICE lpPDevice;
LPPPEN lpPPen;
LPPOINT lpPoints;
{
    short HalfWidth = lpPPen->Width.x / 2;
    PPEN WorkPhysPen;
    POLYSET Polysets [2];
    POINT Angles,
          Center,
          InnerSize,
          OuterSize,
          Ellipse [2] [37];

    check_pen_style (lpPDevice,PS_SOLID);
    WorkPhysPen.PhysicalColor = lpPPen->PhysicalColor;
    WorkPhysPen.Style = PS_SOLID;
    WorkPhysPen.Width.x = 0;
    if (lpPPen->Width.x > THRESHOLD && !lpPDevice->Setup.Draft)
    {
    Angles.x = 0;
    Angles.y = 3600;
    Center.x = (lpPoints->x + (lpPoints + 1)->x) / 2;
    Center.y = (lpPoints->y + (lpPoints + 1)->y) / 2;
    OuterSize.x = ABS(lpPoints->x - (lpPoints + 1)->x) / 2 + HalfWidth;
    OuterSize.y = ABS(lpPoints->y - (lpPoints + 1)->y) / 2 + HalfWidth;
    InnerSize.x = OuterSize.x - lpPPen->Width.x;
    InnerSize.y = OuterSize.y - lpPPen->Width.x;
    arc_to_polyline (lpPDevice,(LPPOINT) &Center,(LPPOINT) &OuterSize,(LPPOINT)
      &Angles,100,(LPPOINT) Ellipse [0]);
    check_fill_style (lpPDevice,BS_SOLID,0,(LPPOINT) Ellipse [0],37);
    arc_to_polyline (lpPDevice,(LPPOINT) &Center,(LPPOINT) &InnerSize,(LPPOINT)
      &Angles,100,(LPPOINT) Ellipse [1]);
    if (!MyPolygon [lpPDevice->Setup.Plotter])
    {
        Polysets [0].lpPoints = Ellipse [0];
        Polysets [1].lpPoints = Ellipse [1];
        Polysets [0].nPoints = Polysets [1].nPoints = 37;
        lpPDevice->CurPen = &WorkPhysPen;
        fill (lpPDevice,(LPPOLYSET) Polysets,2,lpPDevice->CurrentPenThickness,
          get_fill_angle ((LPPOLYSET) Polysets,2),TRUE,FALSE);
        draw_polyline (lpPDevice,37,Ellipse [0],(LPPPEN) &WorkPhysPen,
          (LPPBRUSH) NULL,(LPDRAWMODE) NULL,(LPRECT) NULL);
        draw_polyline (lpPDevice,37,Ellipse [1],(LPPPEN) &WorkPhysPen,
          (LPPBRUSH) NULL,(LPDRAWMODE) NULL,(LPRECT) NULL);
    }
    else
    {
        PutJob (lpPDevice,lpPDevice->hJob,"PU",2);
        construct_point (lpPDevice,(LPPOINT) Ellipse [0],FALSE);
        lpPDevice->PenPos.x = Ellipse [0] [0].x;
        lpPDevice->PenPos.y = Ellipse [0] [0].y;
        PutJob (lpPDevice,lpPDevice->hJob,"PM0",3);
        draw_polyline (lpPDevice,37,(LPPOINT) Ellipse [0],(LPPPEN) &WorkPhysPen,
          (LPPBRUSH) NULL,(LPDRAWMODE) NULL,(LPRECT) NULL);
        PutJob (lpPDevice,lpPDevice->hJob,"PM1PAPU",7);
        construct_point (lpPDevice,(LPPOINT) Ellipse [1],FALSE);
        lpPDevice->PenPos.x = Ellipse [1] [0].x;
        lpPDevice->PenPos.y = Ellipse [1] [0].y;
        lpPDevice->LineInConstruction = FALSE;
        draw_polyline (lpPDevice,37,(LPPOINT) Ellipse [1],(LPPPEN) &WorkPhysPen,
          (LPPBRUSH) NULL,(LPDRAWMODE) NULL,(LPRECT) NULL);
        PutJob (lpPDevice,lpPDevice->hJob,"PM2PAPUFPEP",11);
        lpPDevice->LineInConstruction = FALSE;
    }
    }
    else
    {                   /* Wide ellipse using around fill */
        PBRUSH WorkPhysBrush;
        RECT CurrentEllipse;
        short Index;

        WorkPhysBrush.Style = BS_HOLLOW;
        CurrentEllipse.left = lpPoints->x + HalfWidth;
        CurrentEllipse.top = lpPoints->y + HalfWidth;
        CurrentEllipse.right = (lpPoints + 1)->x - HalfWidth;
        CurrentEllipse.bottom = (lpPoints + 1)->y - HalfWidth;
        for (Index = 0; Index < lpPPen->Width.x; Index += lpPDevice->
          CurrentPenThickness)
        {
            draw_ellipse (lpPDevice,(LPPOINT) &CurrentEllipse,(LPPPEN)
              &WorkPhysPen,(LPPBRUSH) &WorkPhysBrush,(LPDRAWMODE) NULL,
              (LPRECT) NULL);
            CurrentEllipse.left -= lpPDevice->CurrentPenThickness;
            CurrentEllipse.right += lpPDevice->CurrentPenThickness;
            CurrentEllipse.top -= lpPDevice->CurrentPenThickness;
            CurrentEllipse.bottom += lpPDevice->CurrentPenThickness;
        }
    }
}

LOCAL void NEAR PASCAL wide_pie (lpPDevice,lpPPen,lpPoints,StartAngle,EndAngle,
  SweepAngle)
LPPDEVICE lpPDevice;
LPPPEN lpPPen;
LPPOINT lpPoints;
short StartAngle,
      EndAngle,
      SweepAngle;
  /* This function will draw a wide border pie with the given start and end
     angles using the width defined in lpPPen.  lpPoints points to a three
     necessary points: Center, start, and end. */
{
    LPPOINT lpSavePoints = lpPoints;
    short HalfWidth = lpPPen->Width.x / 2,
          CosStart90 = (short)(((long) Cosine ((StartAngle + 900) % 3600) * HalfWidth
            / TRIG_SCALE)),
          SinStart90 = (short)(((long) Sine ((StartAngle + 900) % 3600) * HalfWidth
            / TRIG_SCALE)),
          CosEnd270 = (short)(((long) Cosine ((EndAngle + 2700) % 3600) * HalfWidth
            / TRIG_SCALE)),
          SinEnd270 = (short)(((long) Sine ((EndAngle + 2700) % 3600) * HalfWidth
            / TRIG_SCALE)),
          Offset = 0;
    PPEN WorkPhysPen;
    POLYSET PolySets [2];
    POINT Border [2] [100],
          Angles,
          Size;

    check_pen_style (lpPDevice,PS_SOLID);
    WorkPhysPen.PhysicalColor = lpPPen->PhysicalColor;
    WorkPhysPen.Style = PS_SOLID;
    WorkPhysPen.Width.x = 0;
    lpPDevice->CurPen = &WorkPhysPen;
    if (lpPPen->Width.x > THRESHOLD && !lpPDevice->Setup.Draft)
    {
    Border [0] [Offset].x = lpPoints->x - CosStart90;
    Border [0] [Offset++].y = lpPoints->y - SinStart90;
    Border [0] [Offset].x = (lpPoints + 1)->x - CosStart90;
    Border [0] [Offset++].y = (lpPoints + 1)->y - SinStart90;
    {
        Angles.x = (StartAngle + 2700) % 3600;
        Angles.y = StartAngle;
        Size.x = Size.y = HalfWidth;
        Offset += arc_to_polyline (lpPDevice,(lpPoints + 1),&Size,&Angles,
          50,&Border [0] [Offset]);
        Angles.x = StartAngle;
        Angles.y = EndAngle;
        (lpPoints + 3)->x += HalfWidth;
        (lpPoints + 3)->y += HalfWidth;
        Offset += arc_to_polyline (lpPDevice,lpPoints,(lpPoints + 3),&Angles,
          100,&Border [0] [Offset]);
        Angles.x = EndAngle;
        Angles.y = (EndAngle + 900) % 3600;
        Offset += arc_to_polyline (lpPDevice,(lpPoints + 2),&Size,&Angles,
          50,&Border [0] [Offset]);
    }
    Border [0] [Offset].x = (lpPoints + 2)->x - CosEnd270;
    Border [0] [Offset++].y = (lpPoints + 2)->y - SinEnd270;
    Border [0] [Offset].x = lpPoints->x - CosEnd270;
    Border [0] [Offset++].y = lpPoints->y - SinEnd270;
    if (ABS(SweepAngle) < 1800)
    {
        Angles.x = (EndAngle + 900) % 3600;
        Angles.y = (StartAngle + 2700) % 3600;
        Offset += arc_to_polyline (lpPDevice,lpPoints,&Size,&Angles,50,&Border
          [0] [Offset]);
    }
    PolySets [0].nPoints = Offset;
    PolySets [0].lpPoints = Border [0];
    Border [1] [Offset = 0].x = lpPoints->x + CosEnd270;
    Border [1] [Offset++].y = lpPoints->y + SinEnd270;
    Border [1] [Offset].x = (lpPoints + 2)->x + CosEnd270;
    Border [1] [Offset++].y = (lpPoints + 2)->y + SinEnd270;
    {
        Angles.x = EndAngle;
        Angles.y = StartAngle;
        (lpPoints + 3)->x -= lpPPen->Width.x;
        (lpPoints + 3)->y -= lpPPen->Width.x;
        Offset += arc_to_polyline (lpPDevice,lpPoints,(lpPoints + 3),&Angles,
          -100,&Border [1] [Offset]);
    }
    Border [1] [Offset].x = (lpPoints + 1)->x + CosStart90;
    Border [1] [Offset++].y = (lpPoints + 1)->y + SinStart90;
    Border [1] [Offset].x = lpPoints->x + CosStart90;
    Border [1] [Offset++].y = lpPoints->y + SinStart90;
    if (ABS(SweepAngle) > 1800)
    {
        Angles.y = (EndAngle + 2700) % 3600;
        Angles.x = (StartAngle + 900) % 3600;
        Offset += arc_to_polyline (lpPDevice,lpPoints,&Size,&Angles,50,&Border
          [1] [Offset]);
    }
    PolySets [1].nPoints = Offset;
    PolySets [1].lpPoints = Border [1];
    fill (lpPDevice,(LPPOLYSET) PolySets,2,lpPDevice->CurrentPenThickness,
      get_fill_angle ((LPPOLYSET) PolySets,2),TRUE,TRUE);
    }
    else
    {                   /* Wide Pie with around fill */
    }
}

LOCAL void NEAR PASCAL wide_polyline (lpPDevice,nPoints,lpPoints,lpPPen)
LPPDEVICE lpPDevice;
short nPoints;
LPPOINT lpPoints;
LPPPEN lpPPen;
{

    HANDLE hRight,
           hLeft;
    LPPOINT lpRight,
            lpLeft;
    POINT Size;
    POLYSET Polyset;
    short HalfWidth = (lpPPen->Width.x / 2) + 1,
          AngleFrom,
          AngleTo,
          Index,
          nPointsRight = 100,
          nPointsLeft = 100,
          LeftOffset = 0,
          RightOffset = 0;

    check_pen_style (lpPDevice,PS_SOLID);
    Size.x = Size.y = HalfWidth;
    nPoints = rid_null_points (lpPoints,nPoints);
    if ((lpPPen->Width.x > THRESHOLD && !lpPDevice->Setup.Draft) ||
      nPoints == 2)
    {
    hRight = GlobalAlloc (GMEM_MOVEABLE,(long) nPointsRight * sizeof (POINT));
    lpRight = (LPPOINT) GlobalLock (hRight);
    hLeft = GlobalAlloc (GMEM_MOVEABLE,(long) nPointsLeft * sizeof (POINT));
    lpLeft = (LPPOINT) GlobalLock (hLeft);
    for (Index = 0; Index < nPoints; Index++,lpPoints++)
    {
        POINT Angles;

        if (nPointsLeft - LeftOffset < 15)
        {
            GlobalUnlock (hLeft);
            nPointsLeft += 100;
            hLeft  = GlobalReAlloc (hLeft ,(long) nPointsLeft * sizeof (POINT),
              GMEM_MOVEABLE);
            lpLeft = (LPPOINT) GlobalLock (hLeft);
        }
        if (nPointsRight - RightOffset < 15)
        {
            GlobalUnlock (hRight);
            nPointsRight += 100;
            hRight = GlobalReAlloc (hRight,(long) nPointsRight * sizeof (POINT),
              GMEM_MOVEABLE);
            lpRight = (LPPOINT) GlobalLock (hRight);
        }
        if (Index == 0 || Index == nPoints - 1)
        {
            if (Index == 0)
                AngleFrom = get_angle (lpPoints,(lpPoints + 1));
            Angles.x = (AngleFrom + 900) % 3600;
            Angles.y = (AngleFrom + 2700) % 3600;
            if (Index == 0)
            {
                (lpRight+RightOffset)->x = lpPoints->x + (int)(((long) Cosine
                  (Angles.x) * Size.x / TRIG_SCALE));
                (lpLeft+RightOffset++)->y = lpPoints->y + (int)(((long) Sine (Angles.x)
                  * Size.y / TRIG_SCALE));
                RightOffset += arc_to_polyline (lpPDevice,lpPoints,(LPPOINT)
                  &Size,(LPPOINT) &Angles,200,(lpRight + RightOffset));
            }
            if (Index == nPoints - 1)
                LeftOffset += arc_to_polyline (lpPDevice,lpPoints,(LPPOINT)
                  &Size,(LPPOINT) &Angles,-200,(lpLeft + LeftOffset));
        }
        else
        {
            AngleTo = get_angle (lpPoints,(lpPoints + 1));
            if ((AngleTo - ((AngleFrom + 1800) % 3600) + 3600) % 3600 <= 1800)
            {
                Angles.x = (AngleFrom + 900) % 3600;
                Angles.y = (AngleTo + 900) % 3600;
                LeftOffset += arc_to_polyline (lpPDevice,lpPoints,&Size,&Angles,
                  -200,(lpLeft + LeftOffset));
                (lpRight + RightOffset)->x = lpPoints->x + (int)(((long) Cosine
                  ((AngleFrom + 2700) % 3600) * HalfWidth / TRIG_SCALE));
                (lpRight + RightOffset++)->y = lpPoints->y + (int)(((long) Sine
                  ((AngleFrom + 2700) % 3600) * HalfWidth / TRIG_SCALE));
                (lpRight + RightOffset)->x = lpPoints->x + (int)(((long) Cosine
                  ((AngleTo + 2700) % 3600) * HalfWidth / TRIG_SCALE));
                (lpRight + RightOffset++)->y = lpPoints->y + (int)(((long) Sine
                  ((AngleTo + 2700) % 3600) * HalfWidth / TRIG_SCALE));
            }
            else
            {
                Angles.x = (AngleFrom + 2700) % 3600;
                Angles.y = (AngleTo + 2700) % 3600;
                RightOffset += arc_to_polyline (lpPDevice,lpPoints,&Size,
                  &Angles,200,(lpRight + RightOffset));
                (lpLeft + LeftOffset)->x = lpPoints->x + (int)(((long) Cosine
                  ((AngleFrom + 900) % 3600) * HalfWidth / TRIG_SCALE));
                (lpLeft + LeftOffset++)->y = lpPoints->y + (int)(((long) Sine
                  ((AngleFrom + 900) % 3600) * HalfWidth / TRIG_SCALE));
                (lpLeft + LeftOffset)->x = lpPoints->x + (int)(((long) Cosine
                  ((AngleTo + 900) % 3600) * HalfWidth / TRIG_SCALE));
                (lpLeft + LeftOffset++)->y = lpPoints->y + (int)(((long) Sine
                  ((AngleTo + 900) % 3600) * HalfWidth / TRIG_SCALE));
            }
            AngleFrom = AngleTo;
        }
    }
    GlobalUnlock (hLeft);
    hLeft = GlobalReAlloc (hLeft,(long) (nPointsLeft + nPointsRight) *
      sizeof (POINT), GMEM_FIXED);
    lpLeft = (LPPOINT) GlobalLock (hLeft);
    for (Index = 0; Index < RightOffset; Index++)
        *(lpLeft + LeftOffset + Index) = *(lpRight + RightOffset - Index - 1);
    GlobalUnlock (hRight);
    GlobalFree (hRight);
    Polyset.lpPoints = lpLeft;
    Polyset.nPoints = LeftOffset + RightOffset - 1;
    {
        PPEN WorkPhysPen;

        WorkPhysPen.PhysicalColor = lpPPen->PhysicalColor;
        WorkPhysPen.Style = PS_SOLID;
        WorkPhysPen.Width.x = 0;
        lpPDevice->CurPen = &WorkPhysPen;
        fill (lpPDevice,(LPPOLYSET) &Polyset,1,lpPDevice->CurrentPenThickness,
          get_fill_angle ((LPPOLYSET) &Polyset,1),TRUE,TRUE);
    }
    GlobalUnlock (hLeft);
    GlobalFree (hLeft);
    }
    else
    {                   /* Wide Polyline with around fill */
        short Index;
        POINT Points [2];

        Points [1] = *lpPoints++;
        for (Index = 0; Index < (nPoints - 1); Index++)
        {
            Points [0] = Points [1];
            Points [1] = *lpPoints++;
            wide_polyline (lpPDevice,2,(LPPOINT) Points,(LPPPEN) lpPPen);
        }
    }
}
/* ************************** Exported Routines **************************** */

void FAR PASCAL check_line_construction (lpPDevice)
LPPDEVICE lpPDevice;
{
    if (lpPDevice->LineInConstruction)
    {
        PutJob (lpPDevice,lpPDevice->hJob,"PAPU",4);
        lpPDevice->LineInConstruction = FALSE;
    }
}

void FAR PASCAL construct_float (lpPDevice,hJob,Number,DecimalPlace)
LPPDEVICE lpPDevice;
short hJob,
      Number,
      DecimalPlace;
{
    char NumText [10];
    short Index, sStep;

    // convert number to string
    ltoa(NumText,Number);

    // compute how many places to move each character
    sStep = (DecimalPlace+1) - lstrlen(NumText);

    // move number over in string
    for (Index=lstrlen(NumText); Index >= 0; Index--)
      NumText[Index+sStep] = NumText[Index];

   // pad beginning of string with zero's
   for (Index=0; sStep > Index; Index++)
      NumText[Index] = '0';

    if (NumText [0] == '0')
        NumText [0] = '.';
    else
    {
        for (Index = DecimalPlace; Index > 1; Index--);
            NumText [Index] = NumText [Index - 1];
        NumText [1] = '.';
        NumText [DecimalPlace + 1] = '\0';
    }
    for (Index = lstrlen ((LPSTR) NumText) - 1; NumText [Index] == '0' ||
      NumText [Index] == '.'; NumText [Index--] = '\0');
    PutJob (lpPDevice,hJob,(LPSTR) NumText,lstrlen ((LPSTR) NumText));
}

void FAR PASCAL construct_point (lpPDevice,lpPoints,Relative)
  /* This function is used to transform and output a single X,Y coordinate.
     The coordinate transformation process involves the following:
        - If the output is portrait, exchange X and Y.
        - If output is non-relative, adjust for center origin (if necessary)
            and adjust for landscape output (if necessary) by inverting the
            Y coordinate.
        - If output is relative and landscape, negate the Y coordinate.
     Finally, the coordinate is construct in ASCII and written to the current
     spool job. */
LPPDEVICE lpPDevice;
LPPOINT lpPoints;
BOOL Relative;
{
    long BandOffset = lpPDevice->BandOffset;
    long x = (Relative) ? (long) lpPoints->x - (lpPoints - 1)->x : (long)
           lpPoints->x,
         y = (Relative) ? (long) lpPoints->y - (lpPoints - 1)->y : (long)
           lpPoints->y;
    char Coordinate [12];

    if (lpPDevice->Setup.Orientation == 0)    /* Portrait */
    {
        long Temp = x;

        x = y;
        y = Temp;
    }
    if (!Relative)
    {
        if (lpPDevice->Setup.Plotter > 4)  /* Center Origin */
        {
            if (lpPDevice->Setup.Orientation == 0)    /* Portrait */
            {
                y -= (long) lpPDevice->HorzRes / 2;
		// the following commented lines may need to
		// be used instead of their counterparts
		// ie - switch HorzRes with VertRes in
		// this procedure.
		
		//    y-= (long) lpPDevice->VertRes / 2;
                if (lpPDevice->Setup.Size > 9)  /* Roll Feed */
                    x = (long) x + lpPDevice->BandOffset;
                else
                    x -= (long) lpPDevice->VertRes / 2;
		    // x -= (long) lpPDevice->HorzRes / 2;
            }
            else
            {
                y -= (long) lpPDevice->VertRes / 2;
		// y -= (long) lpPDevice->HorzRes / 2;
                if (lpPDevice->Setup.Size > 9) /* Roll Feed */
                    x = (long) x + lpPDevice->BandOffset;
                else
                    x -= (long) lpPDevice->HorzRes / 2;
	            // x -= (long) lpPDevice->VertRes / 2;
            }
        }
        if (lpPDevice->Setup.Orientation == 1)  /* Landscape */
        {
            if (lpPDevice->Setup.Plotter > 4)  /* Center Origin */
                y = (long) -y;
            else
                y = (long) lpPDevice->VertRes - y;
        }
    }
    else if (lpPDevice->Setup.Orientation == 1) /* Relative & Landscape */
        y = (long) -y;

    ltoa((LPSTR) Coordinate, (long) x * RESOLUTION);

    PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) Coordinate,lstrlen ((LPSTR)
      Coordinate));

    PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) ",",1);

    ltoa((LPSTR) Coordinate, (long) y * RESOLUTION);

    PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) Coordinate,lstrlen ((LPSTR)
      Coordinate));
}

#ifdef HPGL2
void FAR PASCAL get_construct_point (lpPDevice,lpPoints,Relative, x, y)
  /* This function is used to transform and output a single X,Y coordinate.
     The coordinate transformation process involves the following:
        - If the output is portrait, exchange X and Y.
        - If output is non-relative, adjust for center origin (if necessary)
            and adjust for landscape output (if necessary) by inverting the
            Y coordinate.
        - If output is relative and landscape, negate the Y coordinate.
     Finally, the coordinate is construct in ASCII and written to the current
     spool job. */
LPPDEVICE lpPDevice;
LPPOINT lpPoints;
BOOL Relative;
long
    *xnew, *ynew;
{
    long BandOffset = lpPDevice->BandOffset;
    long x = (Relative) ? (long) lpPoints->x - (lpPoints - 1)->x : (long)
           lpPoints->x,
         y = (Relative) ? (long) lpPoints->y - (lpPoints - 1)->y : (long)
           lpPoints->y;
    char Coordinate [12];

    if (lpPDevice->Setup.Orientation == 0)    /* Portrait */
    {
        long Temp = x;

        x = y;
        y = Temp;
    }
    if (!Relative)
    {
        if (lpPDevice->Setup.Plotter > 4)  /* Center Origin */
        {
            if (lpPDevice->Setup.Orientation == 0)    /* Portrait */
            {
                y -= (long) lpPDevice->HorzRes / 2;
                if (lpPDevice->Setup.Size > 9)  /* Roll Feed */
                    x = (long) x + lpPDevice->BandOffset;
                else
                    x -= (long) lpPDevice->VertRes / 2;
            }
            else
            {
                y -= (long) lpPDevice->VertRes / 2;
                if (lpPDevice->Setup.Size > 9) /* Roll Feed */
                    x = (long) x + lpPDevice->BandOffset;
                else
                    x -= (long) lpPDevice->HorzRes / 2;
            }
        }
        if (lpPDevice->Setup.Orientation == 1)  /* Landscape */
        {
            if (lpPDevice->Setup.Plotter > 4)  /* Center Origin */
                y = (long) -y;
            else
                y = (long) lpPDevice->VertRes - y;
        }
    }
    else if (lpPDevice->Setup.Orientation == 1) /* Relative & Landscape */
        y = (long) -y;
/*
    LongToStr ((long) x * RESOLUTION,(LPSTR) Coordinate,1);
    PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) Coordinate,lstrlen ((LPSTR)
      Coordinate));
    PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) ",",1);
    LongToStr ((long) y * RESOLUTION,(LPSTR) Coordinate,1);
    PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) Coordinate,lstrlen ((LPSTR)
      Coordinate));
*/
    newx = x;
    newy = y;
}
#endif

void FAR PASCAL draw_arc (lpPDevice,lpPoints,lpPPen,lpDrawMode,lpClipRect)
  /* Info [0] contains width and height, Info [1] contains start and end angle
  */
LPPDEVICE lpPDevice;
LPPOINT lpPoints;
LPPPEN lpPPen;
LPDRAWMODE lpDrawMode;
LPRECT lpClipRect;
{
    POINT Center,
          RelativeStart,
          RelativeEnd,
          Start,
          Info [2];
    
    check_line_construction (lpPDevice);
    Info [1].x = ABS(lpPoints->x - (lpPoints + 1)->x) / 2;
    Info [1].y = ABS(lpPoints->y - (lpPoints + 1)->y) / 2;
    Center.x = (lpPoints->x + (lpPoints + 1)->x) / 2;
    Center.y = (lpPoints->y + (lpPoints + 1)->y) / 2;
    RelativeStart.x = (lpPoints + 2)->x - Center.x;
    RelativeStart.y = (lpPoints + 2)->y - Center.y;
    RelativeEnd.x = (lpPoints + 3)->x - Center.x;
    RelativeEnd.y = (lpPoints + 3)->y - Center.y;
    Info [0].x = Arctan (RelativeStart.x,RelativeStart.y,Info [1].x,Info [1].y);
    Info [0].y = Arctan (RelativeEnd.x,RelativeEnd.y,Info [1].x,Info [1].y);
    point_on_ellipse (&Start,&Center,Info [0].x,Info [1].x,Info [1].y);
    if (lpPPen->Width.x <= lpPDevice->CurrentPenThickness)
    {
        if (!MyArc [lpPDevice->Setup.Plotter] || lpPPen->Style != PS_SOLID)
        {
            POINT ArcPolyline [70];
            short nPoints = arc_to_polyline (lpPDevice,&Center,&Info [1],Info,
              50,ArcPolyline);

            draw_polyline (lpPDevice,nPoints,ArcPolyline,lpPPen,(LPPBRUSH) NULL,
              (LPDRAWMODE) NULL,(LPRECT) NULL);
        }
        else
            thin_arc (lpPDevice,lpPPen,(LPPOINT) &Start,(LPPOINT) lpPoints,
              Info [0].x,Info [0].y);
    }
    else
        wide_arc (lpPDevice,lpPPen,(LPPOINT) &Start,(LPPOINT) &Center,(LPPOINT)
          Info);
}

void FAR PASCAL draw_bitmap (lpDstDev,DstXOrg,DstYOrg,lpSrcDev,SrcXOrg,SrcYOrg,
  Xext,Yext,Rop,lpPBrush,lpDrawMode)
LPPDEVICE lpDstDev;
int DstXOrg,
    DstYOrg;
LPPDEVICE lpSrcDev;
int SrcXOrg,
    SrcYOrg,
    Xext,
    Yext;
DWORD Rop;
LPPBRUSH lpPBrush;
LPDRAWMODE lpDrawMode;
{

}

void FAR PASCAL draw_chord (lpPDevice,lpPoints,lpPPen,lpPBrush,lpDrawMode,
  lpClipRect)
LPPDEVICE lpPDevice;
LPPOINT lpPoints;
LPPPEN lpPPen;
LPPBRUSH lpPBrush;
LPDRAWMODE lpDrawMode;
LPRECT lpClipRect;
{
}

void FAR PASCAL draw_ellipse (lpPDevice,lpPoints,lpPPen,lpPBrush,lpDrawMode,
  lpClipRect)
LPPDEVICE lpPDevice;
LPPOINT lpPoints;
LPPPEN lpPPen;
LPPBRUSH lpPBrush;
LPDRAWMODE lpDrawMode;
LPRECT lpClipRect;
{
    short HalfWidth = lpPPen->Width.x / 2,
          PolyPoints,
          Plotter = lpPDevice->Setup.Plotter;
    POINT Border [190],
          Bounds [2];

    check_line_construction (lpPDevice);
    if (lpPBrush->Style != BS_HOLLOW || (lpPPen->Style != PS_NULL && lpPPen->
      Width.x <= lpPDevice->CurrentPenThickness))
    {
        Bounds [0].x = lpPoints->x + HalfWidth;
        Bounds [0].y = lpPoints->y + HalfWidth;
        Bounds [1].x = (lpPoints + 1)->x - HalfWidth;
        Bounds [1].y = (lpPoints + 1)->y - HalfWidth;
        if (MyPolygon [Plotter])
        {
            set_bounds (lpPDevice,(LPPOINT) Bounds);
            PutJob (lpPDevice,lpPDevice->hJob,
              "SC0,2,0,2PU1,1PM0CI1PM2SC",25);
        }
        if ((!MyPolygon [Plotter] && lpPBrush->Style != BS_HOLLOW) || !Circle
          [Plotter] || (lpPPen->Style != PS_NULL && lpPPen->Style != PS_SOLID));
        {
            POINT Center,
                  Size,
                  Angles;

            Center.x = (Bounds [0].x + Bounds [1].x) / 2;
            Center.y = (Bounds [0].y + Bounds [1].y) / 2;
            Size.x = ABS(Bounds [0].x - Bounds [1].x) / 2;
            Size.y = ABS(Bounds [0].y - Bounds [1].y) / 2;
            Angles.x = 0;
            Angles.y = 3600;
            PolyPoints = arc_to_polyline (lpPDevice,&Center,&Size,&Angles,25,
              Border);
        }
    }
    if (lpPBrush->Style != BS_HOLLOW)
    {
        if (MyPolygon [lpPDevice->Setup.Plotter])
        {
            if (lpPBrush->Style == BS_HATCHED)
                lpPDevice->CurrentBrushHatch = -1;
            check_fill_style (lpPDevice,lpPBrush->Style,lpPBrush->Hatch,
              (LPPOINT) lpPoints,2);
            PutJob (lpPDevice,lpPDevice->hJob,"FP",2);
            lpPDevice->CurrentBrushStyle = BS_HOLLOW;
            lpPDevice->CurrentBrushHatch = -1;
        }
        else
        {
            POLYSET Polyset;

            Polyset.lpPoints = Border;
            Polyset.nPoints = PolyPoints;
            perform_fill (lpPDevice,lpPBrush,&Polyset,1);
        }
    }
    if (lpPPen->Style != PS_NULL)
    {
        if (lpPPen->Width.x <= lpPDevice->CurrentPenThickness)
        {
            if (!MyPolygon [Plotter] && Circle [Plotter] && lpPPen->Style ==
              PS_SOLID)
            {
                set_bounds (lpPDevice,(LPPOINT) Bounds);
                PutJob (lpPDevice,lpPDevice->hJob,"SC0,2,0,2PU1,1",14);
            }
            thin_ellipse (lpPDevice,lpPPen,Border,PolyPoints);
            if (Circle [lpPDevice->Setup.Plotter] && lpPPen->Style == PS_SOLID)
                set_p1p2 (lpPDevice,(LPRECT) &Corner [lpPDevice->Setup.Plotter]
                  [lpPDevice->Setup.Size]);
        }
        else
        {
            if (lpPBrush->Style != BS_HOLLOW && MyPolygon [lpPDevice->Setup.
              Plotter])
                set_p1p2 (lpPDevice,(LPRECT) &Corner [lpPDevice->Setup.Plotter]
                  [lpPDevice->Setup.Size]);
            wide_ellipse (lpPDevice,lpPPen,(LPPOINT) lpPoints);
        }
    }
}

void FAR PASCAL draw_pie (lpPDevice,lpPoints,lpPPen,lpPBrush,lpDrawMode,
  lpClipRect)
LPPDEVICE lpPDevice;
LPPOINT lpPoints;
LPPPEN lpPPen;
LPPBRUSH lpPBrush;
LPDRAWMODE lpDrawMode;
LPRECT lpClipRect;
{
    POINT Info [4],
          Border [170],
          RelativeStart,
          RelativeEnd;
    short StartAngle,
          EndAngle,
          TrueWidth = ABS(lpPoints->x - (lpPoints + 1)->x),
          TrueHeight = ABS(lpPoints->y - (lpPoints + 1)->y),
          Width = TrueWidth / 2,
          Height = TrueHeight / 2,
          MaxWidth = Width > Height ? Width * 2 : Height * 2,
          PolyPoints;
    int SweepAngle;

    check_line_construction (lpPDevice);
    Info [3].x = Width;
    Info [3].y = Height;
    Info [0].x = (lpPoints->x + (lpPoints + 1)->x) / 2;
    Info [0].y = (lpPoints->y + (lpPoints + 1)->y) / 2;
    RelativeStart.x = (lpPoints + 2)->x - Info [0].x;
    RelativeStart.y = (lpPoints + 2)->y - Info [0].y;
    RelativeEnd.x = (lpPoints + 3)->x - Info [0].x;
    RelativeEnd.y = (lpPoints + 3)->y - Info [0].y;
    if (lpPDevice->Setup.Orientation == 0)  /* Portrait */
    {
        StartAngle = Arctan (RelativeStart.x,RelativeStart.y,TrueWidth,
          TrueHeight);
        EndAngle = Arctan (RelativeEnd.x,RelativeEnd.y,TrueWidth,TrueHeight);
    }
    else
    {
        EndAngle = Arctan (RelativeStart.y,RelativeStart.x,TrueHeight,
          TrueWidth);
        StartAngle = Arctan (RelativeEnd.y,RelativeEnd.x,TrueHeight,TrueWidth);
    }

    if (EndAngle == StartAngle)		/* NF: 04DEC89 */
        EndAngle = EndAngle - 1;	/* Take care of 360 degree pie */

    Info [1].x = Info [0].x + (int)(((long) Cosine (StartAngle) * Width / 16384));
    Info [1].y = Info [0].y + (int)(((long) Sine (StartAngle) * Height / 16384));
    Info [2].x = Info [0].x + (int)(((long) Cosine (EndAngle) * Width / 16384));
    Info [2].y = Info [0].y + (int)(((long) Sine (EndAngle) * Height / 16384));
    SweepAngle = (EndAngle - StartAngle + 3600) % 3600;
    if (lpPBrush->Style != BS_HOLLOW || (lpPPen->Style != PS_NULL && lpPPen->
      Width.x <= lpPDevice->CurrentPenThickness))
    {
        if (Wedge [lpPDevice->Setup.Plotter] && lpPBrush->Style == BS_SOLID)
        {
            set_bounds (lpPDevice,lpPoints);
            PutJob (lpPDevice,lpPDevice->hJob,"SC0,2,0,2PU1,1",14);
            StartAngle = (StartAngle + 900) % 3600;
        }

       /* I changed the next two lines due to Powerpoint arrowhead/line */
       /* problems. -ssn 7/15/91 */
       /*if (!Wedge [lpPDevice->Setup.Plotter] || lpPPen->Style != PS_SOLID ||*/
       /* lpPBrush->Style == BS_HATCHED) */
        if (!Wedge [lpPDevice->Setup.Plotter] || (lpPPen->Style != PS_SOLID &&
	  lpPPen->Style != PS_INSIDEFRAME) || lpPBrush->Style == BS_HATCHED)
        {
            POINT Angles;
            short Step;

            if (lpPDevice->Setup.Orientation == 0)
            {
                Angles.x = StartAngle;
                Angles.y = EndAngle;
                Step = 25;
            }
            else
            {
                Angles.x = Arctan(RelativeStart.x,RelativeStart.y,Width,Height);
                Angles.y = Arctan(RelativeEnd.x,RelativeEnd.y,Width,Height);
                Step = 25;
            }
            PolyPoints = arc_to_polyline (lpPDevice,Info,&Info [3],&Angles,
              Step,Border);
            Border [PolyPoints++] = Info [0];
            Border [PolyPoints++] = Border [0];
        }
    }
    if (lpPBrush->Style != BS_HOLLOW)
    {
        if (Wedge [lpPDevice->Setup.Plotter] && lpPBrush->Style == BS_SOLID)
        {
            check_fill_style (lpPDevice,lpPBrush->Style,lpPBrush->Hatch,
              (LPPOINT) lpPoints,2);
            PutJob (lpPDevice,lpPDevice->hJob,"WG1,",4);
            make_float (lpPDevice,lpPDevice->hJob,StartAngle);
            PutJob (lpPDevice,lpPDevice->hJob,",",1);
            make_float (lpPDevice,lpPDevice->hJob,SweepAngle);
        }
        else
        {
            POLYSET Polyset;

            Polyset.lpPoints = Border;
            Polyset.nPoints = PolyPoints;
            perform_fill (lpPDevice,lpPBrush,&Polyset,1);
        }
    }
    if (lpPPen->Style != PS_NULL)
    {
        if (lpPPen->Width.x <= lpPDevice->CurrentPenThickness)
        {
            check_pen_style (lpPDevice,lpPPen->Style);

	  /* I changed the next line due to Powerpoint arrowhead/line*/
	  /* problems.  -ssn 7/15/91 */
          /*if (Wedge [lpPDevice->Setup.Plotter] && lpPPen->Style == PS_SOLID)*/

            if (Wedge [lpPDevice->Setup.Plotter] &&
	    (lpPPen->Style == PS_SOLID || lpPPen->Style == PS_INSIDEFRAME))
            {
                if (lpPBrush->Style != BS_SOLID)
                {
                    set_bounds (lpPDevice,lpPoints);
                    PutJob (lpPDevice,lpPDevice->hJob,"SC0,2,0,2PU1,1",14);
                    StartAngle = (StartAngle + 900) % 3600;
                }
                thin_pie (lpPDevice,MaxWidth,StartAngle,SweepAngle);
                PutJob (lpPDevice,lpPDevice->hJob,"SC",2);
                set_p1p2 (lpPDevice,(LPRECT) &Corner [lpPDevice->Setup.Plotter]
                  [lpPDevice->Setup.Size]);
            }
            else
            {
                PPEN WorkPhysPen;

                WorkPhysPen.PhysicalColor = lpPPen->PhysicalColor;
                WorkPhysPen.Style = lpPPen->Style;
                WorkPhysPen.Width.x = 0;
                draw_polyline (lpPDevice,PolyPoints,Border,(LPPPEN)&WorkPhysPen,
                  (LPPBRUSH) NULL,(LPDRAWMODE) NULL,(LPRECT) NULL);
            }
        }
        else
        {
            if (lpPBrush->Style == BS_SOLID && Wedge [lpPDevice->Setup.Plotter])
            {
                PutJob (lpPDevice,lpPDevice->hJob,"SC",2);
                set_p1p2 (lpPDevice,(LPRECT) &Corner [lpPDevice->Setup.Plotter]
                  [lpPDevice->Setup.Size]);
            }
            if (lpPDevice->Setup.Orientation == 1)
            {
                StartAngle = (3600 - Arctan (RelativeStart.y,RelativeStart.x,
                  Height,Width)) % 3600;
                EndAngle = (3600 - Arctan (RelativeEnd.y,RelativeEnd.x,Height,
                  Width)) % 3600;
                StartAngle = (StartAngle + 2700) % 3600;
                EndAngle = (EndAngle + 2700) % 3600;
                Info [1].x = Info [0].x + (int)(((long) Cosine (StartAngle) * Width /
                  16384));
                Info [1].y = Info [0].y + (int)(((long) Sine (StartAngle) * Height /
                  16384));
                Info [2].x = Info [0].x + (int)(((long) Cosine (EndAngle) * Width /
                  16384));
                Info [2].y = Info [0].y + (int)(((long) Sine (EndAngle) * Height /
                  16384));
            }
            wide_pie (lpPDevice,lpPPen,Info,StartAngle,EndAngle,SweepAngle);
        }
    }
    else
        PutJob (lpPDevice,lpPDevice->hJob,"SC",2);
}

void FAR PASCAL draw_polygon (lpPDevice,Style,Count,lpPoints,lpPPen,lpPBrush,
  lpDrawMode,lpClipRect)
LPPDEVICE lpPDevice;
short Style,
      Count;
LPPOINT lpPoints;
LPPPEN lpPPen;
LPPBRUSH lpPBrush;
LPDRAWMODE lpDrawMode;
LPRECT lpClipRect;
{
    short TempPoints = Count;
    LPPOINT lpSavePoints = lpPoints;

    check_line_construction (lpPDevice);
    if (lpPBrush->Style != BS_HOLLOW || (lpPPen->Style != PS_NULL && lpPPen->
      Width.x <= lpPDevice->CurrentPenThickness))
    {
        if (lpPDevice->PenPos.x != lpPoints->x || lpPDevice->PenPos.y !=
          lpPoints->y)
        {
            PutJob (lpPDevice,lpPDevice->hJob,"PU",2);
            lpPDevice->PenPos = *lpPoints;
            construct_point (lpPDevice,lpPoints,FALSE);
        }
        if (MyPolygon [lpPDevice->Setup.Plotter] && Count < MaxPolygonSize
          [lpPDevice->Setup.Plotter])
        {
            PutJob (lpPDevice,lpPDevice->hJob,"PM0PRPD",7);
            {
                short Index;
                POINT SubPolygonStart;
                BOOL FirstPoint = TRUE,
                     Done = FALSE;

                if (lpPPen->Style == PS_NULL)
                {
                SubPolygonStart = *lpPoints;
                for (Index = 1; Index < Count && !Done; Index++)
                {
                    if (!FirstPoint)
                        PutJob (lpPDevice,lpPDevice->hJob,",",1);
                    construct_point (lpPDevice,(lpPoints + Index),TRUE);
                    if (!FirstPoint && SubPolygonStart.x == (lpPoints +
                      Index)->x && SubPolygonStart.y == (lpPoints + Index)->y)
                    {
                        if ((Index + 2) >= Count)
                            Done = TRUE;
                        else
                        {
                            PutJob (lpPDevice,lpPDevice->hJob,"PM1PAPU",7);
                            construct_point (lpPDevice,(lpPoints + ++Index),
                              FALSE);
                            SubPolygonStart = *(lpPoints + Index);
                            PutJob (lpPDevice,lpPDevice->hJob,"PRPD",4);
                            FirstPoint = TRUE;
                        }
                    }
                    else
                        FirstPoint = FALSE;
                }
                }
                else
                {
                    for (Index = 1; Index < Count; Index++)
                    {
                        if (Index != 1)
                            PutJob (lpPDevice,lpPDevice->hJob,",",1);
                        construct_point (lpPDevice,(lpPoints + Index),TRUE);
                    }
                }
                PutJob (lpPDevice,lpPDevice->hJob,"PM2PAPU",7);
            }
        }
        if (lpPBrush->Style != BS_HOLLOW)
        {
            if (MyPolygon [lpPDevice->Setup.Plotter] && Count < MaxPolygonSize
              [lpPDevice->Setup.Plotter])
            {
                check_fill_style (lpPDevice,lpPBrush->Style,lpPBrush->Hatch,
                  (LPPOINT) lpPoints,Count);
                PutJob (lpPDevice,lpPDevice->hJob,"FP",2);
            }
            else
            {
                if (lpPPen->Style == PS_NULL)
                {
                    POLYSET Polyset [10];
                    short Index,
                          nPoints,
                          nPolysets = 0;
                    POINT SubPolygonStart;
                    BOOL FirstPoint = TRUE,
                         Done = FALSE;

                    Polyset [0].lpPoints = lpPoints;
                    SubPolygonStart = *lpPoints;
                    for (Index = 1; Index < Count && !Done; Index++)
                    {
                        if (FirstPoint)
                            nPoints = 1;
                        if (!FirstPoint && SubPolygonStart.x == (lpPoints +
                          Index)->x && SubPolygonStart.y == (lpPoints +
                          Index)->y)
                        {
                            if ((Index + 2) >= Count)
                            {
                                Polyset [nPolysets].nPoints = nPoints;
                                Done = TRUE;
                            }
                            else
                            {
                                Polyset [nPolysets].nPoints = nPoints;
                                nPolysets++;
                                Polyset [nPolysets].lpPoints = (lpPoints +
                                  ++Index);
                                SubPolygonStart = *(lpPoints + Index);
                                FirstPoint = TRUE;
                            }
                        }
                        else
                        {
                            nPoints++;
                            FirstPoint = FALSE;
                        }
                    }
                    Polyset [nPolysets].nPoints = nPoints;
                    nPolysets++;
                    perform_fill (lpPDevice,lpPBrush,Polyset,nPolysets);
                }
                else
                {
                    POLYSET Polyset;

                    Polyset.lpPoints = lpPoints;
                    Polyset.nPoints = Count;
                    perform_fill (lpPDevice,lpPBrush,&Polyset,1);
                }
            }
        }
    }
    if (lpPPen->Style != PS_NULL)
    {
        if (lpPPen->Width.x <= lpPDevice->CurrentPenThickness)
        {
            check_pen_style (lpPDevice,lpPPen->Style);
            if (MyPolygon [lpPDevice->Setup.Plotter] && Count < MaxPolygonSize
              [lpPDevice->Setup.Plotter])
                PutJob (lpPDevice,lpPDevice->hJob,"EP",2);
            else
            {
                PPEN WorkPhysPen;

                WorkPhysPen.PhysicalColor = lpPPen->PhysicalColor;
                WorkPhysPen.Style = lpPPen->Style;
                WorkPhysPen.Width.x = 0;
                lpPDevice->LineInConstruction = FALSE;
                draw_polyline (lpPDevice,Count,lpPoints,(LPPPEN) &WorkPhysPen,
                  (LPPBRUSH) NULL,(LPDRAWMODE) NULL,(LPRECT) NULL);
                check_line_construction (lpPDevice);
            }
        }
        else
        {
            lpPDevice->PenPos.x = -1;   /* Force line placement */
            wide_polyline (lpPDevice,Count,lpSavePoints,lpPPen);
        }
    }
}

void FAR PASCAL draw_polyline (lpPDevice,nPoints,lpPoints,lpPPen,lpPBrush,
  lpDrawMode,lpClipRect)
LPPDEVICE lpPDevice;
int nPoints;
LPPOINT lpPoints;
LPPPEN lpPPen;
LPPBRUSH lpPBrush;
LPDRAWMODE lpDrawMode;
LPRECT lpClipRect;
{
    int TempPoints = nPoints;
    LPPOINT lpWorkPoints = lpPoints;
    short Count;
    BOOL Relative = FALSE;

    if (lpPBrush)
        check_pen_style (lpPDevice,0);
    else
        check_pen_style (lpPDevice,lpPPen->Style);
    if (lpPDevice->LineInConstruction &&
       ((ABS (lpPDevice->PenPos.x - lpPoints->x) > lpPDevice->CurrentPenThickness ||
          ABS (lpPDevice->PenPos.y - lpPoints->y) > lpPDevice->CurrentPenThickness) ||
          lpPPen->Width.x > lpPDevice->CurrentPenThickness))
    {
        if (lpPPen->Width.x > lpPDevice->CurrentPenThickness)
            PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "PA",2);
        lpPDevice->LineInConstruction = FALSE;
    }
    if (lpPPen->Width.x <= lpPDevice->CurrentPenThickness)
    {
        if (!lpPDevice->LineInConstruction)
        {
            if (lpPoints->x != lpPDevice->PenPos.x || lpPoints->y != lpPDevice->
              PenPos.y)
            {
                PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "PAPU",4);
#ifdef HPGL2
		get_construct_point (lpPDevice, lpPoints, FALSE, &xnew, &ynew);
		gmove(xnew, ynew);
#endif /* ELSE */
                construct_point (lpPDevice,lpPoints,FALSE);
            }
            PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "PRPD",4);
        }
        else
        {
            if (lpPDevice->PenPos.x != lpPoints->x || lpPDevice->PenPos.y !=
              lpPoints->y)
            {
                POINT WorkPoint [2];

                PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) ",",1);
                WorkPoint [0].x = lpPDevice->PenPos.x;
                WorkPoint [0].y = lpPDevice->PenPos.y;
                WorkPoint [1].x = lpPoints->x;
                WorkPoint [1].y = lpPoints->y;
#ifdef HPGL2
		get_construct_point (lpPDevice, lpPoints, FALSE, &xnew, &ynew);
		gdraw(xnew, ynew);
#endif /* ELSE */
                construct_point (lpPDevice,(LPPOINT) &WorkPoint [1],TRUE);
            }
            PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) ",",1);
        }
        lpPoints++;
        for (Count = 1; Count < nPoints; Count++)
        {
            if (Count != 1)
                PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) ",",1);
#ifdef HPGL2
	    get_construct_point (lpPDevice, lpPoints, FALSE, &xnew, &ynew);
	    gdraw(xnew, ynew);
#endif /* ELSE */
            construct_point (lpPDevice,lpPoints++,TRUE);
        }
        lpPDevice->LineInConstruction = TRUE;
        lpPDevice->PenPos.x = (lpPoints - 1)->x;
        lpPDevice->PenPos.y = (lpPoints - 1)->y;
    }
    else
    {
        check_line_construction (lpPDevice);
        lpPDevice->PenPos.x = -1;
        wide_polyline (lpPDevice,nPoints,lpPoints,lpPPen);
    }
}

void FAR PASCAL draw_polymarker (lpPDevice,Count,lpPoints,lpPPen,lpDrawMode,
  lpClipRect)
LPPDEVICE lpPDevice;
short Count;
LPPOINT lpPoints;
LPPPEN lpPPen;
LPDRAWMODE lpDrawMode;
LPRECT lpClipRect;
{

}

void FAR PASCAL draw_rectangle (lpPDevice,lpPoints,lpPPen,lpPBrush,lpDrawMode,
  lpClipRect)
LPPDEVICE lpPDevice;
LPPOINT lpPoints;
LPPPEN lpPPen;
LPPBRUSH lpPBrush;
LPDRAWMODE lpDrawMode;
LPRECT lpClipRect;
{
    short HalfWidth = lpPPen->Width.x / 2;
    POINT WorkRect [2] [2],
          Border [2] [5];

    lpPDevice->PenPos.x = -1;
    check_line_construction (lpPDevice);
    WorkRect [1] [0] = WorkRect [0] [0] = *lpPoints;
    WorkRect [1] [1] = WorkRect [0] [1] = *(lpPoints + 1);
    WorkRect [0] [0].x -= HalfWidth;
    WorkRect [0] [0].y -= HalfWidth;
    WorkRect [0] [1].x += HalfWidth;
    WorkRect [0] [1].y += HalfWidth;
    rect_to_polyline ((LPRECT) WorkRect [0],(LPPOINT) Border [0]);
    Border [0] [4] = Border [0] [0];
    WorkRect [1] [0].x += HalfWidth;
    WorkRect [1] [0].y += HalfWidth;
    WorkRect [1] [1].x -= HalfWidth;
    WorkRect [1] [1].y -= HalfWidth;
    rect_to_polyline ((LPRECT) WorkRect [1],(LPPOINT) Border [1]);
    Border [1] [4] = Border [1] [0];
    if (lpPBrush->Style != BS_HOLLOW)
    {
        if (FillRectangle [lpPDevice->Setup.Plotter])
        {
            if (lpPDevice->PenPos.x != WorkRect [1] [0].x || lpPDevice->
              PenPos.y != WorkRect [1] [0].y)
            {
                PutJob (lpPDevice,lpPDevice->hJob,"PU",2);
                lpPDevice->PenPos = WorkRect [1] [0];
                construct_point (lpPDevice,&WorkRect [1] [0],FALSE);
            }
            check_fill_style (lpPDevice,lpPBrush->Style,lpPBrush->Hatch,
              (LPPOINT) lpPoints,2);
            PutJob (lpPDevice,lpPDevice->hJob,"RA",2);
            construct_point (lpPDevice,&WorkRect [1] [1],FALSE);
        }
        else
        {
            POLYSET Polyset;

            Polyset.lpPoints = Border [1];
            Polyset.nPoints = 5;
            perform_fill (lpPDevice,lpPBrush,&Polyset,1);
        }
    }
    if (lpPPen->Style != PS_NULL)
    {
        if (lpPPen->Width.x > lpPDevice->CurrentPenThickness)
        {
            if (lpPPen->Width.x > THRESHOLD && !lpPDevice->Setup.Draft)
            {
            if (!MyPolygon [lpPDevice->Setup.Plotter])
            {
                POLYSET Polyset [2];
                PBRUSH WorkPhysBrush;
                PPEN WorkPhysPen;

                Polyset [0].lpPoints = Border [0];
                Polyset [1].lpPoints = Border [1];
                Polyset [0].nPoints = Polyset [1].nPoints = 5;
                WorkPhysBrush.PhysicalColor = lpPPen->PhysicalColor;
                WorkPhysBrush.Style = BS_SOLID;
                perform_fill (lpPDevice,&WorkPhysBrush,Polyset,2);
                WorkPhysPen.PhysicalColor = lpPPen->PhysicalColor;
                WorkPhysPen.Style = lpPPen->Style;
                WorkPhysPen.Width.x = 0;
                draw_polyline (lpPDevice,5,Border [0],(LPPPEN) &WorkPhysPen,
                  (LPPBRUSH) NULL,(LPDRAWMODE) NULL,(LPRECT) NULL);
                draw_polyline (lpPDevice,5,Border [1],(LPPPEN) &WorkPhysPen,
                  (LPPBRUSH) NULL,(LPDRAWMODE) NULL,(LPRECT) NULL);
            }
            else
            {
                short Index;

                check_fill_style (lpPDevice,BS_SOLID,0,(LPPOINT) Border [0],
                  5);
                PutJob (lpPDevice,lpPDevice->hJob,"PU",2);
                construct_point (lpPDevice,&Border [0] [0],FALSE);
                PutJob (lpPDevice,lpPDevice->hJob,"PM0PD",5);
                for (Index = 1; Index < 5; Index++)
                {
                    if (Index != 1)
                        PutJob (lpPDevice,lpPDevice->hJob,",",1);
                    construct_point (lpPDevice,&Border [0] [Index],FALSE);
                }
                PutJob (lpPDevice,lpPDevice->hJob,"PM1PU",5);
                construct_point (lpPDevice,&Border [1] [0],FALSE);
                PutJob (lpPDevice,lpPDevice->hJob,"PD",2);
                for (Index = 1; Index < 5; Index++)
                {
                    if (Index != 1)
                        PutJob (lpPDevice,lpPDevice->hJob,",",1);
                    construct_point (lpPDevice,&Border [1] [Index],FALSE);
                }
                PutJob (lpPDevice,lpPDevice->hJob,"PM2FPEP",7);
            }
            }
            else
            {           /* Wide Rectangle with around fill */
                PPEN WorkPhysPen;
                PBRUSH WorkPhysBrush;
                RECT CurrentRect;
                short Index;

                WorkPhysBrush.Style = BS_HOLLOW;
                WorkPhysPen.Style = PS_SOLID;
                WorkPhysPen.PhysicalColor = lpPPen->PhysicalColor;
                WorkPhysPen.Width.x = 0;
                CurrentRect.left = lpPoints->x + HalfWidth;
                CurrentRect.top = lpPoints->y + HalfWidth;
                CurrentRect.right = (lpPoints + 1)->x - HalfWidth;
                CurrentRect.bottom = (lpPoints + 1)->y - HalfWidth;
                for (Index = 0; Index < lpPPen->Width.x; Index += lpPDevice->
                  CurrentPenThickness)
                {
                    draw_rectangle (lpPDevice,(LPPOINT) &CurrentRect,(LPPPEN)
                      &WorkPhysPen,(LPPBRUSH) &WorkPhysBrush,(LPDRAWMODE) NULL,
                      (LPRECT) NULL);
                    CurrentRect.left -= lpPDevice->CurrentPenThickness;
                    CurrentRect.right += lpPDevice->CurrentPenThickness;
                    CurrentRect.top -= lpPDevice->CurrentPenThickness;
                    CurrentRect.bottom += lpPDevice->CurrentPenThickness;
                }
            }
        }
        else
        {
            check_pen_style (lpPDevice,lpPPen->Style);
            if (lpPPen->Style == PS_SOLID && EdgeRectangle [lpPDevice->Setup.
              Plotter])
            {
                if (lpPDevice->PenPos.x != WorkRect [0] [0].x || lpPDevice->
                  PenPos.y != WorkRect [0] [0].y)
                {
                    PutJob (lpPDevice,lpPDevice->hJob,"PU",2);
                    lpPDevice->PenPos = WorkRect [0] [0];
                    construct_point (lpPDevice,&WorkRect [0] [0],FALSE);
                }
                PutJob (lpPDevice,lpPDevice->hJob,"EA",2);
                construct_point (lpPDevice,&WorkRect [0] [1],FALSE);
            }
            else
            {
                PPEN WorkPhysPen;

                WorkPhysPen.PhysicalColor = lpPPen->PhysicalColor;
                WorkPhysPen.Style = lpPPen->Style;
                WorkPhysPen.Width.x = 0;
                draw_polyline (lpPDevice,5,Border [0],(LPPPEN) &WorkPhysPen,
                  (LPPBRUSH) NULL,(LPDRAWMODE) NULL,(LPRECT) NULL);
            }
        }
    }
}

DWORD FAR PASCAL get_pixel (lpPDevice,X,Y)
LPPDEVICE lpPDevice;
int X,
    Y;
{
   return 0;
}

int FAR PASCAL scan_pixels (lpPDevice,X,Y,PhysColor,Style)
LPPDEVICE lpPDevice;
int X,
    Y;
DWORD PhysColor;
short Style;
{
   return 0;
}

void FAR PASCAL set_pixel (lpPDevice,X,Y,PhysColor,lpDrawMode)
LPPDEVICE lpPDevice;
int X,
    Y;
DWORD PhysColor;
LPDRAWMODE lpDrawMode;
{

}
