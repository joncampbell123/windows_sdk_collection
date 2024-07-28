/*---------------------------------------------------------------------------*\
| DEVICE CAPABILTITIES MODULE                                                 |
|                                                                             |
| STRUCTURE (DEVINFO)                                                         |
|                                                                             |
| FUNCTION EXPORTS                                                            |
|   GetDeviceInfo()                                                           |
|                                                                             |
| Copyright 1990-1992 by Microsoft Corporation                                |
| SEGMENT: _TEXT                                                              |
\*---------------------------------------------------------------------------*/

#include <windows.h>
#include "isg_test.h"

/*---------------------------------------------------------------------------*\
| GET DEVICE CAPABILITIES                                                     |
|   This routine retrieves the device capabilities information and stores it  |
|   in a globally defined structure (dcDevCaps).                              |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HDC       hDC       - Handle to a printer device context.                 |
|   LPDEVINFO lpDevInfo - Long pointer to the dcDevCaps structure             |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if successful.                                                |
\*---------------------------------------------------------------------------*/
BOOL FAR PASCAL GetDeviceInfo(HDC       hDC,
                              LPDEVINFO lpDevInfo)
{
  lpDevInfo->nDriverVersion = GetDeviceCaps(hDC,DRIVERVERSION);
  lpDevInfo->nTechnology    = GetDeviceCaps(hDC,TECHNOLOGY);
  lpDevInfo->nHorzSizeMM    = GetDeviceCaps(hDC,HORZSIZE);
  lpDevInfo->nVertSizeMM    = GetDeviceCaps(hDC,VERTSIZE);
  lpDevInfo->nHorzRes       = GetDeviceCaps(hDC,HORZRES);
  lpDevInfo->nVertRes       = GetDeviceCaps(hDC,VERTRES);
  lpDevInfo->nLogPixelsX    = GetDeviceCaps(hDC,LOGPIXELSX);
  lpDevInfo->nLogPixelsY    = GetDeviceCaps(hDC,LOGPIXELSY);
  lpDevInfo->nBitsPixel     = GetDeviceCaps(hDC,BITSPIXEL);
  lpDevInfo->nPlanes        = GetDeviceCaps(hDC,PLANES);
  lpDevInfo->nBrushes       = GetDeviceCaps(hDC,NUMBRUSHES);
  lpDevInfo->nPens          = GetDeviceCaps(hDC,NUMPENS);
  lpDevInfo->nFonts         = GetDeviceCaps(hDC,NUMFONTS);
  lpDevInfo->nColors        = GetDeviceCaps(hDC,NUMCOLORS);
  lpDevInfo->nAspectX       = GetDeviceCaps(hDC,ASPECTX);
  lpDevInfo->nAspectY       = GetDeviceCaps(hDC,ASPECTY);
  lpDevInfo->nAspectXY      = GetDeviceCaps(hDC,ASPECTXY);
  lpDevInfo->nPDeviceSize   = GetDeviceCaps(hDC,PDEVICESIZE);
  lpDevInfo->wClipCaps      = GetDeviceCaps(hDC,CLIPCAPS);
  lpDevInfo->wRasterCaps    = GetDeviceCaps(hDC,RASTERCAPS);
  lpDevInfo->wCurveCaps     = GetDeviceCaps(hDC,CURVECAPS);
  lpDevInfo->wLineCaps      = GetDeviceCaps(hDC,LINECAPS);
  lpDevInfo->wPolygonCaps   = GetDeviceCaps(hDC,POLYGONALCAPS);
  lpDevInfo->wTextCaps      = GetDeviceCaps(hDC,TEXTCAPS);

  return(TRUE);
}
