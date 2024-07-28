/*************************************************************************

      File:  ERRORS.C

   Purpose:  Contains the error message box handler.

 Functions:  DIBError()

  Comments:  Should use a string table here...We're unnecessarily
             eating up DS, and make it harder to localize for international
             markets...  Maybe next time...

   History:   Date     Reason

             6/1/91    Created

*************************************************************************/


#include <windows.h>
#include "errors.h"


static char *szErrors[] = {"Not a DIB file!",
                           "Couldn't allocate memory!",
                           "Error reading file!",
                           "Error locking memory!",
                           "Error opening file!",
                           "Error creating palette!",
                           "Error getting a DC!",
                           "Error creating MDI Child!",
                           "Error creating Device Dependent Bitmap",
                           "StretchBlt() failed!",
                           "StretchDIBits() failed!",
                           "Paint requires both DDB and DIB!",
                           "SetDIBitsToDevice() failed!",
                           "Printer: StartDoc failed!",
                           "Printing: GetModuleHandle() couldn't find GDI!",
                           "Printer: SetAbortProc failed!",
                           "Printer: StartPage failed!",
                           "Printer: NEWFRAME failed!",
                           "Printer: EndPage failed!",
                           "Printer: EndDoc failed!",
                           "Only one DIB can be animated at a time!",
                           "No timers available for animation!",
                           "Can't copy to clipboard -- no current DIB!",
                           "Clipboard is busy -- operation aborted!",
                           "Can't paste -- no DIBs or DDBs in clipboard!",
                           "SetDIBits() failed!",
                           "File Not Found!",
                           "Error writing DIB file!"
                          };

void DIBError (int ErrNo)
{
   if ((ErrNo < ERR_MIN) || (ErrNo >= ERR_MAX))
   {
      MessageBox (GetFocus (), "Undefined Error!", NULL, MB_OK | MB_ICONHAND);
   }
   else
   {
      MessageBox (GetFocus (), szErrors[ErrNo], NULL, MB_OK | MB_ICONHAND);
   }
}
