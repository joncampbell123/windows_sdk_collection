/*---------------------------------------------------------------------------*\
| PRINTER SPECIFIC MODULE                                                     |
|                                                                             |
| STRUCTURE (----)                                                            |
|                                                                             |
| FUNCTION EXPORTS                                                            |
|   GetPrinterDC()                                                            |
|                                                                             |
| DATE   : January 08, 1990                                                   |
| SEGMENT:_TEXT                                                               |
|                                                                             |
| Copyright, 1990-1992 by Microsoft Corporation                               |
\*---------------------------------------------------------------------------*/

#include <windows.h>
#include <print.h>
#include "isg_test.h"

HDC PASCAL GetPrinterDC(HWND     hWnd,
                        LPSTR    lpProfile,
                        LPHANDLE lphDevMode)
{
  char        szPrinterLine[80];
  char        szModule[80];
  LPSTR       szType,szDriver,szPort,szTemp;
  LPDEVMODE   lpDevMode;
  HANDLE      hLibrary;
  DWORD       dwSize;
  FARPROC     lpProc;
  HDC         hDC;

  /*-----------------------------------------*\
  | Initialize pointer variables for the      |
  | processing of the printer strings.        |
  \*-----------------------------------------*/
  *lphDevMode = (HANDLE) NULL;
  GetPrivateProfileString("Windows", "Device", "", szPrinterLine,
                          sizeof(szPrinterLine), lpProfile);
  szTemp = szType = szPrinterLine;
  szDriver = szPort = NULL;

  /*-----------------------------------------*\
  | Get printer device strings.  This will    |
  | parse the device string so that the NAME  |
  | DRIVER and PORT strings can be identified |
  | separately.                               |
  \*-----------------------------------------*/
  while (*szTemp)
  {
    if    (*szTemp == ',')
    {
      *szTemp++ = 0;
      while(*szTemp == ' ')
        szTemp++;

      if    (!szDriver)
        szDriver = szTemp;
      else
      {
        szPort = szTemp;
        break;
      }
    }
    else
      szTemp++;
  }

  /*-----------------------------------------*\
  | Modify the driver string to include the   |
  | .drv extension for the LoadLibrary().     |
  \*-----------------------------------------*/
  if (!szDriver || !szPort)
    return  NULL;

  lstrcpy(szModule, szDriver);
  lstrcat(szModule, ".DRV");
  if ((hLibrary = LoadLibrary(szModule)) < 32)
    return  NULL;

  /*-----------------------------------------*\
  | Attempt to get ProcAddress of ExtDevMode. |
  | If it doesn't exist, then we must use the |
  | call to the DevMode to create the DC.     |
  \*-----------------------------------------*/
  if (!(lpProc = GetProcAddress(hLibrary, "ExtDeviceMode")))
  {

    if (!(lpProc = GetProcAddress(hLibrary, "DeviceMode")))
    {
      FreeLibrary(hLibrary);
      return NULL;
    }
    (*lpProc)(hWnd, hLibrary, szType, szPort);
    FreeLibrary(hLibrary);

    return CreateDC(szDriver, szType, szPort, NULL);
  }

  /*-----------------------------------------*\
  | Get the size of the DevMode Structure.    |
  \*-----------------------------------------*/
  dwSize = (DWORD) (*lpProc)(hWnd, hLibrary, (LPDEVMODE) NULL, szType,
                             szPort, (LPDEVMODE) NULL, lpProfile, 0);

  if (!dwSize)
    return NULL;

  /*-----------------------------------------*\
  | Allocate space for the devicemode.  Call  |
  | the ExtDeviceMode() for settings.         |
  \*-----------------------------------------*/
  *lphDevMode = GlobalAlloc(GHND, (DWORD) dwSize);
  if (!*lphDevMode)
    return NULL;

  lpDevMode = (LPDEVMODE) GlobalLock(*lphDevMode);

  if    (!lpDevMode)
    return  NULL;

  (*lpProc)(hWnd, hLibrary, lpDevMode, szType, szPort, (LPDEVMODE) NULL,
            lpProfile, DM_OUT_BUFFER);
  FreeLibrary(hLibrary);

  hDC = CreateDC(szDriver, szType, szPort, (LPSTR)lpDevMode);
  GlobalUnlock(*lphDevMode);

  return    hDC;
}
