/*---------------------------------------------------------------------------*\
| MISC ROUTINES                                                               |
|   This module contains miscellaneous routines which can be used throughout  |
|   the entire application.  These functions are more than not, common to     |
|   more than one segment.  It is desirable to place these routines in a      |
|   segment which is always loaded.                                           |
|                                                                             |
| DATE   : June 22, 1989                                                      |
|   Copyright   1989-1992 by Microsoft Corporation                            |
\*---------------------------------------------------------------------------*/

#include    <Windows.H>
#include    "PrntTest.H"

/*---------------------------------------------------------------------------*\
| PRINT TEST DESCRIPTION                                                      |
|   This routine uses DrawText to output the test description to the printer  |
|   device.                                                                   |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   LPRECT lprc       - points to bounding rectangle                          |
|   WORD   wLength    - length of string to be output                         |
|   LPSTR  lpText     - Pointer to string                                     |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   HDC    hdcPrinter - hDC of PRINTER, not Metafile (DrawText isn't          |
|                       supported to a metafile                               |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if successful.                                                |
/*---------------------------------------------------------------------------*/
BOOL DoSomeText(LPRECT lprc,
                WORD   wLength,
                LPSTR  lpText)
{
  /*-----------------------------------------*\
  | Output the text to the printer DC.        |
  \*-----------------------------------------*/
  return (BOOL)DrawText(hdcPrinter, lpText, wLength, lprc,
                        DT_LEFT | DT_WORDBREAK | DT_EXTERNALLEADING);
}

/*---------------------------------------------------------------------------*\
| PRINT TEST DESCRIPTION                                                      |
|   This routine uses DrawText to output the test description to the printer  |
|   device.                                                                   |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   WORD   wTestString - Identifies resource to be printed                    |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   HANDLE hInst       - Handle to application instance.                      |
|   HDC    hdcPrinter  - DC of Printer, not MetaFile!                         |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if successful.                                                |
/*---------------------------------------------------------------------------*/
int PrintTestDescription(WORD wTestString)
{
  extern HANDLE hInst;

  char    szTextRes[20];                      /* Temporary buffer for res */
  HANDLE  hResource;                          /* Handle of text resource  */
  LPSTR   lpText,lpEnd;                       /* Pointers to text         */
  RECT    rc;                                 /* Outputting rectangle     */
  int     iHeight;                            /* Height of text outputted */

  /*-----------------------------------------*\
  | Get text resource for the description.    |
  \*-----------------------------------------*/
  LoadString(hInst, wTestString, (LPSTR)szTextRes, sizeof(szTextRes));
  if    (!(hResource = LoadResource(hInst, FindResource(hInst, szTextRes,
          "TEXT"))))
       return(FALSE);
  if    (!(lpText = LockResource(hResource)))
    {
      FreeResource(hResource);
      return(FALSE);
    }

  /*-----------------------------------------*\
  | NULL terminate the text resource.         |
  \*-----------------------------------------*/
  lpEnd = lpText;
  while ((*lpEnd != '\0') && (*lpEnd != '\x1A'))
    lpEnd++;

  *lpEnd = '\0';

  /*-----------------------------------------*\
  | Output the text to the printer DC.        |
  \*-----------------------------------------*/
  SetRect(&rc,0,0,iPageWidth,iPageHeight);

  iHeight = DrawText(hdcPrinter, lpText, -1, &rc,
        DT_LEFT | DT_WORDBREAK | DT_EXTERNALLEADING | DT_CALCRECT);

  AddDrawObject(&rc, DoSomeText, TEXT_OBJECT, lstrlen(lpText), lpText);

  /*-----------------------------------------*\
  | Free up structures and resources.         |
  \*-----------------------------------------*/
  UnlockResource(hResource);
  FreeResource(hResource);

  return(iHeight);
}


/******************************************************************************

    Public Function: GetFontName

    Purpose:         Builds a font name string, based on a LPLOGFONT

    Change History:  Moved from setup.c and fonts.c

******************************************************************************/
int GetFontName(LPLOGFONT lplf,
                LPSTR     lpszBuffer)
{
  char szTemp[15];

  lstrcpy(lpszBuffer,lplf->lfFaceName);
  lstrcat(lpszBuffer," ");
  lstrcat(lpszBuffer,
          litoa(DEVICE_TO_POINTSIZE(lplf->lfHeight),szTemp,10));
  
  if(lplf->lfWeight > 550)
    lstrcat(lpszBuffer," Bold");
  if(lplf->lfItalic)
    lstrcat(lpszBuffer," Italic");
  if(lplf->lfStrikeOut)
    lstrcat(lpszBuffer," StrikeOut");
  if(lplf->lfUnderline)
    lstrcat(lpszBuffer," Underlined");
  
  return lstrlen(lpszBuffer);
}

/*****************************************************************************

    Public Function: Find31GDICall

    Purpose:         Does GetModuleHandle/GetProcAddress for 3.1-specific
                     GDI calls.  Write it once.

    Change History:  Why am I inside when it's beautiful outside?

******************************************************************************/
FARPROC Find31GDICall(LPCSTR lpName)
{
  HANDLE  hGDI;
  FARPROC lpFn;

  hGDI=GetModuleHandle("GDI.EXE");

  if(lpFn=GetProcAddress(hGDI,lpName))
    return lpFn;
  else
  {
    BYTE szTemp[20];

    if(!HIWORD(lpName))
    {
      wsprintf(szTemp,"Ordinal: %u",LOWORD(lpName));
      lpName=szTemp;
    }
  
    MessageBox(GetFocus(),lpName,"Can't Find Function!",MB_ICONEXCLAMATION);
  }
  return NULL;
}
