/* (c) Copyright 1987 MICROGRAFX, Inc., All Rights Reserved. This material is
   confidential and a trade secret. Permission to use this work for any purpose
   must be obtained in writing from MICROGRAFX, 1820 N. Greenville Ave.,
   Richardson, Tx.  75081.

   This file is Micrografx proprietary information and permission for
   Hewlett-Packard to distribute any part of this file outside its peripheral
   division must obtained in writing from MICROGRAFX, 1820 N. Greenville Ave.,
   Richardson, Tx.  75081.

*******************************************************************************
*******************************************************************************

                                      fill

*******************************************************************************
*******************************************************************************
*/

#define PRINTDRIVER
#define NOPQ
#define NOINSTALLFILES
#include <print.h>
#include <gdidefs.inc>


#include "glib.h"
#include "utils.h"
#include "device.h"
#include "driver.h"
#include "fill.h"
#include "build.h"
#include <memory.h>

/* ******************************* History ********************************* */
/* 10/31/87 (RWM) - signoff                                                  */
/* ****************************** Constants ******************************** */

#define LOCAL static

#define MAX_INT   ((int) (((unsigned) -1) >> 1))
#define MIN_INT   ((int) (((unsigned) MAX_INT) + 1))

/********************************* Typedefs **********************************/

typedef struct
        {
            BYTE  Dir;
            short Lo,
                  Hi;
        } RANGE;
typedef RANGE FAR *LPRANGE;

/********************************* Local Data ********************************/

LOCAL BOOL Direction,      /* Used as direction indicator for intersection */
           OddScan,
           Vertex;         /* TRUE if intersection occurs at vertex */

LOCAL HANDLE  hRanges;     /* Handle to scanline range values */

LOCAL LPPOINT lpPoints;    /* Pointer to current list of points */

LOCAL LPINT   lpIndex;       /* Pointer to current pair index */

LOCAL LPRANGE lpRanges;    /* Pointer to range values */

LOCAL short CurScan,  /* The current scanline */
            I1,       /* Used as indices into the current list of points */
            I2,
            nPairs,   /* Number of vertex pairs in current polygon */
            nPoints,  /* Number of points in current polygon */
            nRanges,  /* Number of ranges values allocated */
            X1,       /* Used to describe end points of current line */
            Y1,
            X2,
            Y2,
            xLo,      /* The low x value for the current scanline */
            xHi,      /* The high x value for the current scanline */
            xCount;   /* The number of x range values for current scanline. */

/******************************** External Data ******************************/
/******************************* Local Routines ******************************/

LOCAL BOOL  NEAR PASCAL add_range (void);
LOCAL void  NEAR PASCAL get_extent (short);
LOCAL BOOL  NEAR PASCAL get_line (short);
LOCAL short NEAR PASCAL get_range (short,BOOL);
LOCAL void  NEAR PASCAL output_ranges (LPPDEVICE,short,BOOL,BOOL);
LOCAL void  NEAR PASCAL process_vertex (short,short);
LOCAL void  NEAR PASCAL scanline (LPPDEVICE,short,short,short,short);
LOCAL BOOL  NEAR PASCAL setup (LPPOLYSET,short,short);
LOCAL void  NEAR PASCAL rotate_points (LPPOINT,short,short);


LOCAL BOOL NEAR PASCAL add_range ()
  /* This function adds a new range to the list of ranges maintained for the
     current scanline. The range is given by the variables 'xLo' and 'xHi'.
     The direction indicated by the variable 'Dir' is also associated with the
     intersection range. The direction indicates if the line was directed
     upwards or downwards. This information is used for winding fills. */
{
    BOOL  Result = TRUE;
    short I, J, K;

    if (xCount >= nRanges)
    {
        GlobalUnlock (hRanges);
        nRanges += 16;
        Result = (hRanges = GlobalReAlloc (hRanges,(DWORD) nRanges *
          sizeof (RANGE),GMEM_MOVEABLE)) != NULL;
        if (Result)
            lpRanges = (LPRANGE) GlobalLock (hRanges);
    }
    if (Result)
    {
        for (I = 0; I < xCount; ++I)
            if (xLo <= lpRanges [I].Lo)
                break;
        K = xCount - 1;
        J = xCount++;
        while (J > I)
        {
            lpRanges [J].Lo = lpRanges [K].Lo;
            lpRanges [J].Hi = lpRanges [K].Hi;
            lpRanges [J].Dir = lpRanges [K].Dir;
            --K;
            --J;
        }
        lpRanges [I].Lo = xLo;
        lpRanges [I].Hi = xHi;
        lpRanges [I].Dir = Direction;
    }
    return (Result);
}


LOCAL void NEAR PASCAL get_extent (Angle)
  /* This function computes the low and high intersection values for the line
     specified by (X1,Y1) & (X2,Y2) with the current scanline specified by
     'CurScan'. Low and high intersection values are used instead of the
     mathematical intersection so that the extreme outside edges of the
     polygon will be filled. If the mathematical intersection was used instead,
     the extreme edges of the polygon would not filled in some cases.

     For example:

     The intersection of scanline 'S'                   M   B
     with the line (A,B) produces the                   |  /
     intersection values (L,H). The             S     L---H
     mathematical intersection is shown              /
     by the letter M.                               A

     On return, the range will be contained in the local variables 'xLo' and
     'xHi'.

     Note that horizontal optimization only occurs if 'Angle' is zero.
         The mathematical solution is given by:
            Y = Y2 - Y1;
        xLo = xHi = ((long) (CurScan - Y1) * (X2 - X1) + Y / 2) / Y + X1;
  */
short Angle;
{
    short X, Y, N;

    Y = Y2 - Y1;
    if (Y < 0)
        Y = -Y;
    X = X2 - X1;
    if (X < 0)
        X = -X;
    N = (Y1 > Y2)
      ? (CurScan - (X1 < X2 ? Y1 : Y2))
      : (CurScan - (X1 > X2 ? Y1 : Y2));
    if (N < 0)
        N = -N;
    if (Angle || Y >= X)
        xLo = xHi = (short)(((long) N * X + Y / 2) / Y);
    else
    {
        ++Y;
        ++X;
        xLo = (short)(((long) N * X ) / Y);
        xHi = (short)(((long) (N + 1) * X ) / Y - 1);
    }
    if (X1 > X2)
    {
        short Temp = X1;

        X1 = X2;
        X2 = Temp;
    }
    if (Y1 > Y2)
    {
        xLo += X1;
        xHi += X1;
    }
    else
    {
        short Temp = xLo;

        xLo = X2 - xHi;
        xHi = X2 - Temp;
    }
}


LOCAL BOOL NEAR PASCAL get_line (Index)
  /* This function determines a line for intersection with the current
     scanline. If a line is determined, the local variables X1,Y1 and X2,Y2
     will contain the end points of the line and TRUE will be returned as a
     result, otherwise, FALSE is returned. */
short Index;
{
    BOOL  Result = FALSE;
    short Hi,
          K;

    I1 = lpIndex [Index * 2];
    I2 = lpIndex [Index * 2 + 1];
    if (I1 != I2)
    {
        Direction = Vertex = FALSE;
        Y1 = lpPoints [I1].y;
        Y2 = lpPoints [I2].y;
        if ((CurScan >= Y1 && CurScan <= Y2) ||
          (CurScan >= Y2 && CurScan <= Y1))
        {
            X1 = lpPoints [I1].x;
            X2 = lpPoints [I2].x;
            if (Y2 < Y1)
            {
                K = I2 - 1;
                if (K < 0)
                    K += nPoints;
                X1 = lpPoints [K].x;
                Y1 = lpPoints [K].y;
                Hi = Y1;
                Direction = TRUE;
            }
            else
            {
                K = I1 + 1;
                if (K >= nPoints)
                    K -= nPoints;
                X2 = lpPoints [K].x;
                Y2 = lpPoints [K].y;
                Hi = Y2;
            }
            if (Hi == CurScan)
                Vertex = TRUE;
            Result = TRUE;
        }
    }
    return (Result);
}


LOCAL short NEAR PASCAL get_range (Index,Winding)
  /* This function determines the next range to be used in the fill for the
     current scanline. On return the local variables, 'xLo' and 'xHi' are set
     to the proper range and an index to the next range is returned as a
     result. */
short Index;
BOOL  Winding;
{
    short Count = 1;

    if (Winding)
    {
        ++Index;
        while (Count)
        {
            if (lpRanges [Index].Dir == Direction)
                ++Count;
            else
                --Count;
            ++Index;
        }
        xHi = lpRanges [Index - 1].Hi;
    }
    else
    {
        xLo = lpRanges [Index].Lo;
        xHi = lpRanges [Index].Hi;
        ++Index;
        while (Count % 2 || (Index < xCount && lpRanges [Index].Lo <= xHi))
        {
            if (lpRanges [Index].Hi > xHi)
                xHi = lpRanges [Index].Hi;
            ++Count;
            ++Index;
        }
    }
    return (Index);
}


LOCAL void NEAR PASCAL output_ranges (lpPDevice,Angle,Alternate,Winding)
  /* This function is used to output the range values for the current scanline.
     'Angle' specifies the angle of rotation. If the variable 'Winding' is
     TRUE then a winding fill will be used, otherwise, an alternate fill
     will be used. */
LPPDEVICE lpPDevice;
short     Angle;
BOOL      Alternate,
          Winding;
{
    short Index = 0,
          nRanges = 0;

    while (Index < xCount)
    {
        xLo = lpRanges [Index].Lo;
        Direction = lpRanges [Index].Dir;
        Index = get_range (Index,Winding);
        lpRanges [nRanges].Lo = xLo;
        lpRanges [nRanges].Hi = xHi;
        ++nRanges;
    }
        if (Alternate && OddScan)
                while (nRanges--)
                scanline (lpPDevice,lpRanges [nRanges].Hi,lpRanges [nRanges].Lo,
                  CurScan,Angle);
        else
                for (Index = 0; Index < nRanges; ++Index)
                scanline (lpPDevice,lpRanges [Index].Lo,lpRanges [Index].Hi,
                  CurScan,Angle);
    OddScan = !OddScan;
}


LOCAL void NEAR PASCAL process_vertex (Index,Angle)
  /* This function provides special handling for vertex points. When a vertex
     point is reached during the processing of scanlines, two things must be
     done. The vertex pair table must be adjusted to eliminate the previous
     line and the intersection range must be detemined for the next line as
     this may add to the current range given by 'xLo' and 'xHi'. */
short Index,
      Angle;
{
    short xLo2 = xLo,
          xHi2 = xHi;

    if (Direction)
    {
        --I2;
        if (I2 < 0)
            I2 += nPoints;
        while (I2 != I1)
        {
            --I2;
            if (I2 < 0)
                I2 += nPoints;
            if (Y1 != lpPoints [I2].y)
            {
                ++I2;
                if (I2 >= nPoints)
                    I2 -= nPoints;
                break;
            }
        }
        lpIndex [Index * 2 + 1] = I2;
    }
    else
    {
        ++I1;
        if (I1 >= nPoints)
            I1 -= nPoints;
        while (I1 != I2)
        {
            ++I1;
            if (I1 >= nPoints)
                I1 -= nPoints;
            if (Y2 != lpPoints [I1].y)
            {
                --I1;
                if (I1 < 0)
                    I1 += nPoints;
                break;
            }
        }
        lpIndex [Index * 2] = I1;
    }
    if (get_line (Index))
    {
        get_extent (Angle);
        if (xLo2 < xLo)
            xLo = xLo2;
        if (xHi2 > xHi)
            xHi = xHi2;
    }
}


LOCAL void NEAR PASCAL rotate_points (lpPoints,nPoints,Angle)
LPPOINT lpPoints;
short   nPoints,
        Angle;
{
    while (nPoints--)
    {
        long X = lpPoints->x,
             Y = lpPoints->y,
             Xt,
             Yt;

        Xt = X * Cosine (Angle) - Y * Sine (Angle);
        Yt = X * Sine (Angle) + Y * Cosine (Angle);
        lpPoints->x = (int)((Xt < 0)
          ? (Xt - TRIG_SCALE / 2) / TRIG_SCALE
          : (Xt + TRIG_SCALE / 2) / TRIG_SCALE);
        lpPoints->y = (int)((Yt < 0)
          ? (Yt - TRIG_SCALE / 2) / TRIG_SCALE
          : (Yt + TRIG_SCALE / 2) / TRIG_SCALE);
        ++lpPoints;
    }
}


LOCAL void NEAR PASCAL scanline (lpPDevice,Lo,Hi,Scan,Angle)
LPPDEVICE lpPDevice;
short     Lo,
          Hi,
          Scan,
          Angle;
{
    POINT Line [2];

    if (Angle)
    {
        Line [0].x = Lo;
        Line [1].x = Hi;
        Line [0].y = Line [1].y = Scan;
        rotate_points (Line,2,-Angle);
        Output (lpPDevice,OS_POLYLINE,2,(LPPOINT) Line,(LPSTR) lpPDevice->
          CurPen,(LPSTR) NULL,(LPDRAWMODE) NULL,(LPRECT) NULL);
    }
    else
    {
        Line [0].y = Scan;
        Line [1].x = Lo;
        Line [1].y = Hi;
        Output (lpPDevice,OS_SCANLINES,2,(LPPOINT) Line,(LPSTR) lpPDevice->
          CurPen,(LPSTR) NULL,(LPDRAWMODE) NULL,(LPRECT) NULL);
    }
}


LOCAL BOOL NEAR PASCAL setup (lpPolyset,nSets,Angle)
LPPOLYSET lpPolyset;
short     nSets,
          Angle;
{
    BOOL  Result = TRUE;
    short Index;

    Bounds.left = Bounds.top = MAX_INT;
    Bounds.right = Bounds.bottom = MIN_INT;
    for (Index = 0; Result && Index < nSets; ++Index)
    {
        lpPolyset [Index].lpOldPoints = lpPolyset [Index].lpPoints;
        lpPolyset [Index].hIndex = lpPolyset [Index].hMem = NULL;
        if (Angle)
        {
            if (Result = (lpPolyset [Index].hMem = GlobalAlloc (GMEM_MOVEABLE,
              (long) lpPolyset [Index].nPoints * sizeof (POINT))) != NULL)
            {
                lpPolyset [Index].lpPoints =
                  (LPPOINT) GlobalLock (lpPolyset [Index].hMem);
                _fmemcpy ((LPSTR) lpPolyset [Index].lpPoints,
                  (LPSTR) lpPolyset [Index].lpOldPoints,
                  lpPolyset [Index].nPoints * sizeof (POINT));
                rotate_points (lpPolyset [Index].lpPoints,
                  lpPolyset [Index].nPoints,Angle);
            }
        }
        if (Result)
            Result = build (&lpPolyset [Index]);
    }
    hRanges = NULL;
    if (Result)
    {
        nRanges = 16;
        hRanges = GlobalAlloc (GMEM_MOVEABLE,(long) nRanges * sizeof (RANGE));
        if (Result = (hRanges != NULL))
            lpRanges = (LPRANGE) GlobalLock (hRanges);
    }
    if (!Result)
    {
        while (Index--)
        {
            if (lpPolyset [Index].hMem)
            {
                GlobalUnlock (lpPolyset [Index].hMem);
                GlobalFree (lpPolyset [Index].hMem);
            }
            if (lpPolyset [Index].hIndex)
                GlobalFree (lpPolyset [Index].hIndex);
        }
        if (hRanges)
        {
            GlobalUnlock (hRanges);
            GlobalFree (hRanges);
        }
    }
    return (Result);
}


/****************************** External Routines ****************************/

BOOL NEAR PASCAL fill (lpPDevice,lpPolyset,nSets,Spacing,Angle,Alternate,
                        Winding)
LPPDEVICE lpPDevice;
LPPOLYSET lpPolyset;
short     nSets,
          Spacing,
          Angle;
BOOL      Alternate,
          Winding;
{
    BOOL  Result;
    short Index,
          I;

    if (Result = setup (lpPolyset,nSets,Angle))
    {
        if (Spacing <= 0)
            Spacing = 1;
        CurScan = Bounds.top;
        do
        {
            xCount = 0;
            for (Index = 0; Result && Index < nSets; ++Index)
            {
                lpIndex = (LPINT) GlobalLock (lpPolyset [Index].hIndex);
                lpPoints = lpPolyset [Index].lpPoints;
                nPairs = lpPolyset [Index].nPairs;
                nPoints = lpPolyset [Index].nPoints;
                if (!nPairs)
                    scanline (lpPDevice,Bounds.left,Bounds.right,Bounds.top,
                      Angle);
                for (I = 0; I < nPairs; ++I)
                {
                    if (get_line (I))
                    {
                        get_extent (Angle);
                        if (Vertex)
                            process_vertex (I,Angle);
                        if (!(Result = add_range ()))
                            break;
                    }
                }
                GlobalUnlock (lpPolyset [Index].hIndex);
            }
            if (Result && (CurScan % Spacing == 0))
                output_ranges (lpPDevice,Angle,Alternate,Winding);
            ++CurScan;
        } while (Result && xCount);
        for (Index = 0; Index < nSets; ++Index)
        {
            lpPolyset [Index].lpPoints = lpPolyset [Index].lpOldPoints;
            if (Angle)
            {
                GlobalUnlock (lpPolyset [Index].hMem);
                GlobalFree (lpPolyset [Index].hMem);
            }
            GlobalFree (lpPolyset [Index].hIndex);
        }
        if (hRanges)
        {
            GlobalUnlock (hRanges);
            GlobalFree (hRanges);
        }
    }
    return (Result);
}

