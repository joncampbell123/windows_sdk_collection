/*---------------------------------------------------------------------------*\
| TEST MODULE                                                                 |
|   This module is an object oriented library for common routines used by     |
|   testing applications.                                                     |
|                                                                             |
| OBJECT                                                                      |
|   TEST                                                                      |
|                                                                             |
| METHODS                                                                     |
|   InitTest()                                                                |
|   KillTest()                                                                |
|                                                                             |
| Copyright 1990-1992 by Microsoft Corporation                                |
| SEGMENT: _TEST                                                              |
\*---------------------------------------------------------------------------*/

#include <windows.h>
#include "isg_test.h"


/*---------------------------------------------------------------------------*\
| INIT TEST                                                                   |
|   This routine initializes the attributes associated with the test object.  |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   LPSTRT lpszModule - String representing the test library name.            |
|   HANDLE hInstance  - Instance handle of the testing library DS.            |
|   LPTEST lpTest     - Testing object.                                       |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - True indicates the test passed, otherwise failure.                 |
\*---------------------------------------------------------------------------*/
BOOL FAR PASCAL InitTest(LPSTR  lpszModule,
                         HANDLE hInstance,
                         LPTEST tTest)
{
  /*-----------------------------------------*\
  | Initialize STANDARD test settings.        |
  \*-----------------------------------------*/
  lstrcpy(tTest->lpszModule,lpszModule);
  tTest->hInstance       = hInstance;
  tTest->lpszFunction    = NULL;
  tTest->lpszDescription = NULL;
  tTest->wTestArea       = NULL;
  tTest->wStatus         = 0;
  tTest->wGranularity    = 1;
  tTest->wIterations     = 1;
  SetRect(&tTest->rTestRect,0,0,0,0);

  /*-----------------------------------------*\
  | Initialize GDI Flags/Values.              |
  \*-----------------------------------------*/
  tTest->gtTest.hPens         = GetDeviceObjects(NULL,DEV_INDEX,NULL);
  tTest->gtTest.hBrushes      = GetDeviceObjects(NULL,DEV_INDEX,NULL);
  tTest->gtTest.hFonts        = GetDeviceObjects(NULL,DEV_INDEX,NULL);
  tTest->gtTest.hRegion       = NULL;
  tTest->gtTest.nBkMode       = OPAQUE;
  tTest->gtTest.crBkColor     = RGB(255,255,255);
  tTest->gtTest.nPolyFillMode = ALTERNATE;
  tTest->gtTest.nROP2         = R2_COPYPEN;
  tTest->gtTest.nStretchMode  = BLACKONWHITE;
  tTest->gtTest.crTextColor   = RGB(0,0,0);
  tTest->gtTest.dwROP         = SRCCOPY;

  /*-----------------------------------------*\
  | Initialize USER Flags/Values.             |
  \*-----------------------------------------*/
  tTest->utTest.wDlgFlags   = 0x0000;
  tTest->utTest.dwDlgStyles = 0x00000000;
  tTest->utTest.wWinFlags   = 0x0000;
  tTest->utTest.dwWinStyles = 0x00000000;

  /*-----------------------------------------*\
  | Initialize KERNEL Flags/Values.           |
  \*-----------------------------------------*/
  tTest->ktTest.wKernelFlags = 0x0000;

  return(TRUE);
}


/*---------------------------------------------------------------------------*\
| KILL TEST                                                                   |
|   This routine cleans up the instances of the test object.                  |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   LPTEST lpTest - Testing object.                                           |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - True indicates the test passed, otherwise failure.                 |
|                                                                             |
|  05/29/31 set handles to NULL after freeing them                            |
\*---------------------------------------------------------------------------*/
BOOL FAR PASCAL KillTest(LPTEST tTest)
{
  if(tTest->gtTest.hPens)
  {
    FreeDeviceObjects(tTest->gtTest.hPens);
    tTest->gtTest.hPens=NULL;
  }
  if(tTest->gtTest.hBrushes)
  {
    FreeDeviceObjects(tTest->gtTest.hBrushes);
    tTest->gtTest.hBrushes=NULL;
  }
  if(tTest->gtTest.hFonts)
  {
    FreeDeviceObjects(tTest->gtTest.hFonts);
    tTest->gtTest.hFonts=NULL;
  }
  if(tTest->gtTest.hRegion)
  {
    DeleteObject(tTest->gtTest.hRegion);
    tTest->gtTest.hRegion=NULL;
  }

  return(TRUE);
}


