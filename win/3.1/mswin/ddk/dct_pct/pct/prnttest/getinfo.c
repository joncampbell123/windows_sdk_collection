/*---------------------------------------------------------------------------*\
| GET DEVICE INFORMATION                                                      |
|   This module contains the routines necessary to retrieve the device        |
|   capabilities for a particular driver.  It will retrieve the following     |
|   device capabilities:                                                      |
|                                                                             |
| DATE   : June 05, 1989                                                      |
| Copyright 1989-1992 by Microsoft Corporation                                |
\*---------------------------------------------------------------------------*/

#include <windows.h>
#include "PrntTest.h"

/*---------------------------------------------------------------------------*\
| GET PRINTER INFORMATION                                                     |
|   This routine retrieves the printer information concerning the device.     |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HDC       hDC       - Handle to a printer device context.                 |
|   LPPRINTER lpPrinter - Long pointer to printer information structure.      |
|   LPSTR     szProfile - Pointer to Profile Name                             |
|   LPSTR     szString  -                                                     |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if successful.                                                |
\*---------------------------------------------------------------------------*/
BOOL GetPrinterInformation(HDC       hDC,
                           LPPRINTER pPrinter,
                           LPSTR     szProfile,
                           LPSTR     szString)
{
  LPSTR lpBuffer;

  lstrcpy(pPrinter->szProfile,szProfile);

  lpBuffer = szString;

  while(*lpBuffer++ != ',');
  *(lpBuffer-1) = 0;
  lstrcpy(pPrinter->szName,szString);
  szString = lpBuffer;

  while(*lpBuffer++ != ',');
  *(lpBuffer-1) = 0;
  lstrcpy(pPrinter->szDriver,szString);

  lstrcpy(pPrinter->szPort,lpBuffer);
  wsprintf(pPrinter->szDriverVer,"%#X",GetDeviceCaps(hDC,DRIVERVERSION));
  wsprintf(pPrinter->szSystemVer, "%d:%d", LOBYTE(LOWORD(GetVersion())),
           HIBYTE(LOWORD(GetVersion())));

  return FALSE;
}
