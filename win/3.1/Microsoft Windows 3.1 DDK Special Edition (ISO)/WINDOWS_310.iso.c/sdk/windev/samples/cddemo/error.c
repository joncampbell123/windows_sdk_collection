/**************************************************************************
*                                                                         *
*      File:  ERROR.C                                                     *
*                                                                         *
*   Purpose:  Contains the error routines for this program                *
*                                                                         *
* Functions:  void FAR ProcessCDError(DWORD)                              *
*             void FAR ReportError(WORD)                                  *
*                                                                         *
*  Comments:                                                              *
*                                                                         *
*   History:  Date      Reason                                            *
*             --------  -----------------------------------               *
*                                                                         *
*             10/01/91  Created                                           *
*                                                                         *
**************************************************************************/

#define WIN31

#include "windows.h"  
#include "commdlg.h"
#include "cderr.h"
#include "cddemo.h"  

/**************************************************************************
*                                                                         *
*  Function:  ProcessCDError(DWORD)                                       *
*                                                                         *
*   Purpose:  To report an error that has occurred during the last        *
*             call to a CD routine.                                       *
*                                                                         *
*   Returns:  void                                                        *
*                                                                         *
*  Comments:                                                              *
*                                                                         *
*   History:  Date      Reason                                            *
*             --------  -----------------------------------               *
*                                                                         *
*             10/01/91  Created                                           *
*                                                                         *
**************************************************************************/
void FAR ProcessCDError(DWORD dwErrorCode)
{
   WORD  wStringID;

   switch(dwErrorCode)
      {
         case CDERR_DIALOGFAILURE:   wStringID=IDS_DIALOGFAILURE;   break;
         case CDERR_STRUCTSIZE:      wStringID=IDS_STRUCTSIZE;      break;
         case CDERR_INITIALIZATION:  wStringID=IDS_INITIALIZATION;  break;
         case CDERR_NOTEMPLATE:      wStringID=IDS_NOTEMPLATE;      break;
         case CDERR_NOHINSTANCE:     wStringID=IDS_NOHINSTANCE;     break;
         case CDERR_LOADSTRFAILURE:  wStringID=IDS_LOADSTRFAILURE;  break;
         case CDERR_FINDRESFAILURE:  wStringID=IDS_FINDRESFAILURE;  break;
         case CDERR_LOADRESFAILURE:  wStringID=IDS_LOADRESFAILURE;  break;
         case CDERR_LOCKRESFAILURE:  wStringID=IDS_LOCKRESFAILURE;  break;
         case CDERR_MEMALLOCFAILURE: wStringID=IDS_MEMALLOCFAILURE; break;
         case CDERR_MEMLOCKFAILURE:  wStringID=IDS_MEMLOCKFAILURE;  break;
         case CDERR_NOHOOK:          wStringID=IDS_NOHOOK;          break;
         case PDERR_SETUPFAILURE:    wStringID=IDS_SETUPFAILURE;    break;
         case PDERR_PARSEFAILURE:    wStringID=IDS_PARSEFAILURE;    break;
         case PDERR_RETDEFFAILURE:   wStringID=IDS_RETDEFFAILURE;   break;
         case PDERR_LOADDRVFAILURE:  wStringID=IDS_LOADDRVFAILURE;  break;
         case PDERR_GETDEVMODEFAIL:  wStringID=IDS_GETDEVMODEFAIL;  break;
         case PDERR_INITFAILURE:     wStringID=IDS_INITFAILURE;     break;
         case PDERR_NODEVICES:       wStringID=IDS_NODEVICES;       break;
         case PDERR_NODEFAULTPRN:    wStringID=IDS_NODEFAULTPRN;    break;
         case PDERR_DNDMMISMATCH:    wStringID=IDS_DNDMMISMATCH;    break;
         case PDERR_CREATEICFAILURE: wStringID=IDS_CREATEICFAILURE; break;
         case PDERR_PRINTERNOTFOUND: wStringID=IDS_PRINTERNOTFOUND; break;
         case CFERR_NOFONTS:         wStringID=IDS_NOFONTS;         break;
         case FNERR_SUBCLASSFAILURE: wStringID=IDS_SUBCLASSFAILURE; break;
         case FNERR_INVALIDFILENAME: wStringID=IDS_INVALIDFILENAME; break;
         case FNERR_BUFFERTOOSMALL:  wStringID=IDS_BUFFERTOOSMALL;  break;

         case 0:   //User may have hit CANCEL or we got a *very* random error
         default:
            return; 
      }

   if (!LoadString(ghInst, wStringID, gszBuffer, sizeof(gszBuffer)))
      {
         ReportError(IDC_LOADSTRINGFAIL);
         return;
      }

   MessageBox(ghWnd, gszBuffer, gszAppName, MB_OK);
   return;
}


/**************************************************************************
*                                                                         *
*  Function:  ReportError(WORD)                                           *
*                                                                         *
*   Purpose:  To report an error that has occurred while allocating       *
*             memory for the CD struct, locking the memory or while       *
*             trying to load a resource string.                           *
*                                                                         *
*   Returns:  void                                                        *
*                                                                         *
*  Comments:                                                              *
*                                                                         *
*   History:  Date      Reason                                            *
*             --------  -----------------------------------               *
*                                                                         *
*             10/01/91  Created                                           *
*                                                                         *
**************************************************************************/
void FAR ReportError(WORD wErrorType)
{
   LPSTR lpszErrorMsg;

   switch( wErrorType )
      {
         case IDC_ALLOCFAIL:

            lpszErrorMsg=gszAllocErrorMsg;
            break;

         case IDC_LOCKFAIL:

            lpszErrorMsg=gszLockErrorMsg;
            break;

         case IDC_LOADSTRINGFAIL:

            lpszErrorMsg=gszLoadStrFail;
            break;

         default:    //let's hope we never get here!
            return;
      }

   MessageBox(ghWnd, (LPSTR)lpszErrorMsg, gszAppName, MB_OK);

   return;
}
