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

                                     build

*******************************************************************************
*******************************************************************************
*/

#define PRINTDRIVER
#define NOPQ
#define NOINSTALLFILES
#include <print.h>
#include <gdidefs.inc>

#include "device.h"
#include "fill.h"
#include "build.h"

/* ******************************* History ********************************* */
/* 10/31/87 (RWM) - signoff                                                  */
/* ****************************** Constants ******************************** */

#define LOCAL static

/* ***************************** Local Data ******************************** */

LOCAL HANDLE  hPairs;      /* Handle to vertex pairs */

LOCAL LPPOINT lpPoints;    /* Pointer to current set of points */

LOCAL short   CurIndex,
              nAllocated,
              nPairs,
              nPoints;

/* **************************** External Data ****************************** */

RECT Bounds;  /* Used to determine boundary of polygon */

/* **************************** Local Routines ***************************** */

LOCAL short NEAR PASCAL get_first_sign (void);
LOCAL short NEAR PASCAL get_sign (short);
LOCAL BOOL  NEAR PASCAL get_vertex_pairs (void);
LOCAL short NEAR PASCAL skip_horizontal_lines (short);


LOCAL BOOL NEAR PASCAL add_pair_index (Index)
  /* This function adds a new pair index to the current set of indices. Memory
     will be allocated if neccessary to accomodate the new pair. If successful,
     TRUE is returned as a result, otherwise, FALSE is returned. */
short Index;
{
    if (nPairs >= nAllocated)
    {
        nAllocated += 16;
        hPairs = GlobalReAlloc (hPairs,(DWORD) nAllocated * sizeof (int),
          GMEM_MOVEABLE);
    }
    if (hPairs)
    {
        LPINT lpPairs = (LPINT) GlobalLock (hPairs) + nPairs;

                if (Index >= nPoints)
                        Index -= nPoints;
        *lpPairs = Index;
        ++nPairs;
        GlobalUnlock (hPairs);
    }
    return (hPairs != NULL);
}


LOCAL short NEAR PASCAL get_first_sign ()
  /* This function returns the first sign change in the list of points. If no
     sign change occurs, 0 is returned as a result, otherwise, the sign is
     returned. A 0 return value indicates that list of points form a horizontal
     line. */
{
    short Result = 0;

    CurIndex = 0;
    while (CurIndex < nPoints)
    {
        if (Result = get_sign (CurIndex))
            break;
        ++CurIndex;
    }
    return (Result);
}


LOCAL short NEAR PASCAL get_sign (Index)
  /* This function returns the sign of the difference between the point at
     'Index' and the point at 'Index + 1'. It is also used to find the
     rectangular boundaries of the data set. */
short Index;
{
    short Result;

    Index = Index % nPoints;
    Result = lpPoints [Index].y - lpPoints [(Index + 1) % nPoints].y;
    if (lpPoints [Index].y < Bounds.top)
        Bounds.top = lpPoints [Index].y;
    if (lpPoints [Index].y > Bounds.bottom)
        Bounds.bottom = lpPoints [Index].y;
    if (lpPoints [Index].x < Bounds.left)
        Bounds.left = lpPoints [Index].x;
    if (lpPoints [Index].x > Bounds.right)
        Bounds.right = lpPoints [Index].x;
    if (Result)
        Result = (Result < 0) ? -1 : 1;
    return (Result);
}


LOCAL BOOL NEAR PASCAL get_vertex_pairs ()
  /* This function builds a list of vertex pairs to be used in the filling
     process. A vertex pair is two indices that index maximum and minimum
     point values with no maximum or minimum values in between. Using vertex
     pairs is a method for speeding up the filling process in a polygon with a
     large number of points. In the following diagram the vertex pairs are:

     (B,C) (C,D) (D,E) (F,A)        A----------B
                                   /            \  D
                                  G              \/ \
                                   \              C   \
                                     \                  \
                                       \                  \
                                        F------------------E

     The number of vertex pairs is assumed to be small in relation to the
     number of points. If the number of pairs should become large, they may
     be sorted to prevent searching the entire list for each scan line. If
     successful, the local variable 'nPairs' contains the number of index pairs
     and TRUE is returned as a result, otherwise, FALSE is returned. Note that
     if nPairs is equal to zero the list of points forms a horizonal line. */
{
    BOOL  Result = TRUE;
    short Last,
          Sign,
          NewSign;

    if (Sign = get_first_sign ())
    {
        ++CurIndex;
        while (Result && (CurIndex <= nPoints))
        {
            short Prev = CurIndex;

            CurIndex = skip_horizontal_lines (CurIndex);
            NewSign = get_sign (CurIndex);
            if (Sign != NewSign)
            {
                if (nPairs == 0)
                    Last = Prev;
                else
                    Result = add_pair_index (Prev);
                if (Result)
                    Result = add_pair_index (CurIndex);
                Sign = NewSign;
            }
            ++CurIndex;
        }
        if (Result)
            Result = add_pair_index (Last);
    }
    nPairs /= 2;
    return (Result);
}


LOCAL short NEAR PASCAL skip_horizontal_lines (Index)
  /* This function is used to skip over horizontal lines in the list of points.
     An index to the first non horzontal line past the given index is returned
     as a result. */
short Index;
{
    while (!get_sign (Index))
        ++Index;
    return (Index);
}


/******************************** Test Routines ******************************/

BOOL NEAR PASCAL build (lpPolyset)
  /* This function is used to build a list of vertex pairs for the given
     polyset. */
LPPOLYSET lpPolyset;
{
    BOOL Result;

    nPairs = 0;
    lpPoints = lpPolyset->lpPoints;
    nPoints = lpPolyset->nPoints;
    nAllocated = 16;
    hPairs = GlobalAlloc (GMEM_MOVEABLE,(long) nAllocated * sizeof (int));
    if (Result = (hPairs != NULL))
    {
        if (Result = get_vertex_pairs ())
        {
                lpPolyset->hIndex = hPairs;
            lpPolyset->nPairs = nPairs;
        }
    }
    return (Result);
}
