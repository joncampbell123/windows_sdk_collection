/*************************************************************************

      File:  ABOUT.C

   Purpose:  Contains the about box's window procedure.

 Functions:  AboutDlg

  Comments:  Nothing really special here...Put in a separate module
             so it doesn't eat memory when no about box is around.

   History:   Date     Reason

             6/1/91    Created

*************************************************************************/

#include <windows.h>
#include "about.h"

//---------------------------------------------------------------------
//
// Function:   AboutDlg
//
// Purpose:    About Dialog box message handler.  Does nothing special,
//             except close down when needed.
//
// Parms:      hDlg    == Handle to About dialog box.
//             message == Message to handle.
//             wParam  == Depends on message.
//             lParam  == Depends on message.
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------

BOOL FAR PASCAL AboutDlg(HWND hDlg, 
                     unsigned message, 
                         WORD wParam, 
                         LONG lParam)
{
   switch (message)
   {
      case WM_INITDIALOG:
         return (TRUE);

      case WM_COMMAND:                 
         if ((wParam == IDOK) ||       // "OK" box selected?        
             (wParam == IDCANCEL))     // System menu close command?
         {
            EndDialog(hDlg, TRUE);     // Exits the dialog box 
            return (TRUE);
         }
         break;
    }
    return (FALSE);                    // Didn't process a message
}




