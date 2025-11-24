/* (c) Copyright 1987 MICROGRAFX, Inc., All Rights Reserved.  Permission to
   use this work for any purpose must be obtained in writing from MICROGRAFX,
   1820 N. Greenville Ave., Richardson, Tx.  75081.

*******************************************************************************
*******************************************************************************

                                  color

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
#include "color.h"
#include "object.h"
/* ****************************** History ********************************** */
/* 10/31/87 (RWM) - signoff                                                  */
/* ***************************** Constants ********************************* */
/* **************************** Local Data ********************************* */
/* *************************** Exported Data ******************************* */
/* *************************** Local Routines ****************************** */
/* ************************** Exported Routines **************************** */

DWORD FAR PASCAL get_nearest_rgb (lpPDevice,Color,lpPColor)
/* This function determines the best pen color for the given RGB color value.
   All requests for white are immediatly returned so the driver shell can
   isolate and prevent output. */
LPPDEVICE lpPDevice;
DWORD Color;
LPPCOLOR lpPColor;
{
    DWORD SaveColor = Color;
    short Index,
          MinDistance = 32760,
          SelColor = 9,
          Distance;

    if (Color == 0x00FFFFFF)
    {
        *lpPColor = Color;
        return (Color);
    }
    for (Index = 0; Index < MAX_COLORS; Index++)
    {
        if (lpPDevice->ColorsAvailable [Index].Available)
        {
            if ((Distance = ColorDistance (lpPDevice->ColorsAvailable
              [Index].Color,Color)) < MinDistance)
            {
                MinDistance = Distance;
                SelColor = Index;
            }
        }
    }
    *lpPColor = rgb_to_pen (lpPDevice,lpPDevice->ColorsAvailable [SelColor].
      Color,OBJ_PEN);
    return (lpPDevice->ColorsAvailable [SelColor].Color);
}

DWORD FAR PASCAL physical_to_rgb (lpPDevice,PhysicalColor)
LPPDEVICE lpPDevice;
DWORD PhysicalColor;
{
    if (PhysicalColor == 0x00FFFFFF)
        return (PhysicalColor);
    else
        return (lpPDevice->ColorsAvailable [PhysicalColor].Color);
}
