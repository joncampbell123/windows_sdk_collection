/*---------------------------------------------------------------------------*\
| PRINTER TEST ABOUT BOX                                                      |
|   This module contains the routine(s) necessary to handle the application   |
|   AboutBox.  It is usually a simple routine to display the author, date     |
|   and description of the application.                                       |
|                                                                             |
| DATE   : June 05, 1989                                                      |
| Copyright 1989-1992 by Microsoft Corporation                                |
\*---------------------------------------------------------------------------*/

#include <windows.h>
#include "PrntTest.h"

/*---------------------------------------------------------------------------*\
| APPLICATION ABOUT BOX ROUTINE                                               |
|   This routine handles the inputs supplied to the AboutBox dialog window.   |
|   It displays minor information concernig the application and waits for the |
|   OK button to be hit.  Apon which it returns to the application.           |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HWND     hWnd     - Handle to the dialogbox window.                       |
|   unsigned iMessage - Message to be processed.                              |
|   WORD     wParam   - Information associated with message.                  |
|   LONG     lParam   - Information associated with message.                  |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE indicates message has been processed.  FALSE specifies        |
|          otherwise.                                                         |
\*---------------------------------------------------------------------------*/
BOOL FAR PASCAL AboutDlg(HWND     hDlg,
                         unsigned iMessage,
                         WORD     wParam,
                         LONG     lParam)
{
  switch(iMessage)
  {
    /*------------------------------------*\
    | Return to application procedure once |
    | a key is hit.                        |
    \*------------------------------------*/
    case WM_COMMAND:
      if(wParam == IDOK)
        EndDialog(hDlg,TRUE);
      else
        return(FALSE);
      break;

    /*------------------------------------*\
    | Message wasn't processed.            |
    \*------------------------------------*/
    default:
      return(FALSE);
  }

  return(TRUE);
}
