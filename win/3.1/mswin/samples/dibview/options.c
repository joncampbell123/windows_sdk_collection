/*************************************************************************

      File:  OPTIONS.C

   Purpose:  Routines to implement the display/printing options dialog
             box.

 Functions:  ShowOptions
             StretchDlg
             OptionsInit
             EnableXYAxisWindows
             SetHelpText
             GetDlgItemIntOrDef

  Comments:

   History:   Date     Reason

             6/1/91    Created

*************************************************************************/

#include <windows.h>
#include <string.h>
#include "dibview.h"
#include "options.h"



// Magic numbers used within this modules.

#define MAX_HELP_LINE   80


// Locally used variables -- globals for this module only.

static char          szStretchDlgName[] = "OptionsStretch";
static BOOL          bTmpStretch;         // New stretch value.
static BOOL          bTmpUse31PrnAPIs;    // New 3.1 Print APIs value. 
static BOOL          bTmpUseBanding;      // New banding flag value.
static LPOPTIONSINFO lpInfo;
static int           idDispOption;        // Current display option.
static int           idPrintOption;       // Current print option.



// Locally used function prototypes.

BOOL FAR PASCAL StretchDlg(HWND, unsigned, WORD, LONG);
void EnableXYAxisWindows (HWND hDlg, BOOL bEnable);
BOOL OptionsInit (HWND hDlg, LPOPTIONSINFO lpOptions);
void SetHelpText (HWND hDlg, WORD wCtrlID);
int  GetDlgItemIntOrDef (HWND hDlg, int nCtrlID, int nDefault);




//---------------------------------------------------------------------
//
// Function:   ShowOptions
//
// Purpose:    Brings up options dialog box which modifies the OPTIONSINFO
//             structure passed to this routine.
//
// Parms:      hWnd    == Handle to options dialog box's parent.
//             lpInfo  == Far pointer to OPTIONSINFO structure the options
//                         dialog box should edit.
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------

void ShowOptions (HWND hWnd, LPOPTIONSINFO lpInfo)
{
   FARPROC lpProcStretch;

   lpProcStretch = MakeProcInstance(StretchDlg, hInst);

   DialogBoxParam(hInst,
                  szStretchDlgName,
                  hWnd,
                  lpProcStretch,
                  (LONG) (LPOPTIONSINFO) lpInfo);

   FreeProcInstance(lpProcStretch);
}






//---------------------------------------------------------------------
//
// Function:   StretchDlg
//
// Purpose:    Window procedure for the options dialog box.
//
// Parms:      hWnd    == Handle to options dialog box's parent.
//             message == Message being sent to dialog box.
//             wParam  == Depends on message.
//             lParam  == Depends on message.
//
//
// History:   Date      Reason
//             6/01/91  Created
//             7/01/91  Major restructuring.
//             
//---------------------------------------------------------------------

BOOL FAR PASCAL StretchDlg (HWND hDlg, 
                        unsigned message, 
                            WORD wParam, 
                            LONG lParam)
{
   switch (message)
      {
      case WM_INITDIALOG:
         return OptionsInit (hDlg, (LPOPTIONSINFO) lParam);



      case WM_COMMAND:
         if (wParam != IDRB_STRETCHWINDOW)
            SetHelpText (hDlg, wParam);

         switch (wParam)
            {
               // If the stretch checkbox is hit, toggle the current
               //  stretch mode, and update the help window.

            case IDRB_STRETCHWINDOW:
               bTmpStretch = !bTmpStretch;
               SetHelpText (hDlg, wParam + bTmpStretch);
               return TRUE;


               // If the banding checkbox is hit, toggle the current
               //  banding mode, and update the help window.

            case IDRB_USEBANDING:
               bTmpUseBanding = !bTmpUseBanding;
               SetHelpText (hDlg, wParam + bTmpUseBanding);
               return TRUE;


               // If the "Use 3.1 APIs" checkbox is hit, toggle the current
               //  mode, and update the help window.

            case IDRB_USE31APIS:
               bTmpUse31PrnAPIs = !bTmpUse31PrnAPIs;
               SetHelpText (hDlg, wParam + bTmpUse31PrnAPIs);
               return TRUE;


            
               // If one of the printer option buttons is pressed,
               //  we may want to enable or gray the X/Y Axis
               //  edit controls.  We only perform this action if
               //  it is necessary.  Also, remember which button
               //  is selected.

            case IDRB_SCALE:
            case IDRB_STRETCH:
            case IDRB_BESTFIT:
               if (wParam == IDRB_SCALE && idPrintOption != IDRB_SCALE)
                  EnableXYAxisWindows (hDlg, TRUE);
               else if (wParam != IDRB_SCALE && idPrintOption == IDRB_SCALE)
                  EnableXYAxisWindows (hDlg, FALSE);

               idPrintOption = wParam;
               return TRUE;


               // If one of the display option buttons is pressed,
               //  remember it.

            case IDRB_USEDIBS:
            case IDRB_USEDDBS:
            case IDRB_USESETDIBITS:
               idDispOption = wParam;
               return TRUE;



//            case IDEF_XAXIS:
//               return TRUE;
//
//            case IDEF_YAXIS:
//               return TRUE;


            case IDOK:
               lpInfo->bStretch        = bTmpStretch;
               lpInfo->bPrinterBand    = bTmpUseBanding;
               lpInfo->bUse31PrintAPIs = bTmpUse31PrnAPIs;
               lpInfo->wDispOption     = idDispOption;
               lpInfo->wPrintOption    = idPrintOption;
               lpInfo->wXScale         = GetDlgItemIntOrDef (hDlg, 
                                                             IDEF_XAXIS, 
                                                             lpInfo->wXScale);
               lpInfo->wYScale         = GetDlgItemIntOrDef (hDlg,
                                                             IDEF_YAXIS,
                                                             lpInfo->wYScale);

               // Fall through (end this dialog box).

            case IDCANCEL:
               EndDialog(hDlg, TRUE);
               return TRUE;


            default:
               return FALSE;
            }
      }
   return FALSE;
}





//---------------------------------------------------------------------
//
// Function:   OptionsInit
//
// Purpose:    Called by options dialog for WM_INITDIALOG.  Keeps a pointer
//             to the OPTIONSINFO structure we're modifying.  Sets up
//             globals for current state of certain buttons.  Sets correct
//             buttons.  Grays controls that should be gray.  Initializes
//             edit fields.
//
//             Sets focus to radio button who is "selected" in the
//             first group of radio buttons (i.e. the display options
//             radio button group).  Strange side-effects can happen
//             if this isn't done (e.g. tabbing through the controls
//             will stop on the first control in the dialog box -- even
//             if it isn't checked).  This leads to some subtle bugs...
//
// Parms:      hDlg      == Handle to options dialog box's window.
//             lpOptions == Far pointer to OPTIONSINFO structure passed
//                          for our dialog box to change.  All starting
//                          info is saved from this.
//
// History:   Date      Reason
//             9/01/91  Cut out of dialog procedure.
//            11/08/91  Added SetFocus, and return FALSE.
//             
//---------------------------------------------------------------------

BOOL OptionsInit (HWND hDlg, 
         LPOPTIONSINFO lpOptions)
{
   lpInfo           = lpOptions;
   bTmpStretch      = lpOptions->bStretch;
   bTmpUseBanding   = lpOptions->bPrinterBand;
   bTmpUse31PrnAPIs = lpOptions->bUse31PrintAPIs;


      // Set up the correct buttons in the box.  Disable controls
      //  which should be disabled.

   SendDlgItemMessage (hDlg,
                       IDRB_STRETCHWINDOW,
                       BM_SETCHECK,
                       bTmpStretch,
                       NULL);

   SendDlgItemMessage (hDlg,
                       IDRB_USEBANDING,
                       BM_SETCHECK,
                       bTmpUseBanding,
                       NULL);

   SendDlgItemMessage (hDlg,
                       IDRB_USE31APIS,
                       BM_SETCHECK,
                       bTmpUse31PrnAPIs,
                       NULL);

   idDispOption  = lpOptions->wDispOption;
   idPrintOption = lpOptions->wPrintOption;


      // Note the use of CheckRadioButton (as opposed to
      //  SendDlgItemMessage (... BM_SETCHECK...) -- this is used so
      //  that tabbing occurs between _groups_ correctly.  When
      //  a radio button is pressed, tabbing to the group of
      //  radio buttons should set the focus to *that* button.

   CheckRadioButton (hDlg, IDRB_USEDIBS, IDRB_USESETDIBITS, idDispOption);
   CheckRadioButton (hDlg, IDRB_BESTFIT, IDRB_SCALE, idPrintOption);

   SetDlgItemInt (hDlg, IDEF_XAXIS, lpOptions->wXScale, FALSE);
   SetDlgItemInt (hDlg, IDEF_YAXIS, lpOptions->wYScale, FALSE);

   EnableXYAxisWindows (hDlg, lpOptions->wPrintOption == PRINT_SCALE);

   return TRUE;
}




//---------------------------------------------------------------------
//
// Function:   EnableXYAxisWindows
//
// Purpose:    Enables or gray's the X/Y axis edit controls and their
//             labes.
//
// Parms:      hDlg    == Handle to options dialog box's parent.
//             bEnable == TRUE = Enable them, FALSE = Gray them.
//
// History:   Date      Reason
//             7/01/91  Cut out of options dialog.
//             
//---------------------------------------------------------------------

void EnableXYAxisWindows (HWND hDlg, BOOL bEnable)
{
   EnableWindow (GetDlgItem (hDlg, IDEF_XAXIS),  bEnable);
   EnableWindow (GetDlgItem (hDlg, IDEF_YAXIS),  bEnable);
   EnableWindow (GetDlgItem (hDlg, IDC_XLABEL),  bEnable);
   EnableWindow (GetDlgItem (hDlg, IDC_YLABEL),  bEnable);
}




//---------------------------------------------------------------------
//
// Function:   SetHelpText        
//
// Purpose:    Sets the help static edit control to the appropriate
//             string.  Strings are kept in a string table in the
//             resources (see DIBVIEW.RC).
//
// Parms:      hDlg    == Handle to options dialog box's parent.
//             wCtrlID == ID of control we're setting help for.
//
// History:   Date      Reason
//             7/01/91  Cut out of options dialog.
//             
//---------------------------------------------------------------------

void SetHelpText (HWND hDlg, WORD wCtrlID)
{
   char  szHelpBuffer [MAX_HELP_LINE];

   LoadString (hInst, wCtrlID, szHelpBuffer, sizeof (szHelpBuffer));
   SetDlgItemText (hDlg, IDEF_HELP, szHelpBuffer);
}






//---------------------------------------------------------------------
//
// Function:   GetDlgItemIntOrDef
//
// Purpose:    Returns an integer stored in an edit control of a
//             dialog box.  If the edit control doesn't contain
//             a valid integer value, returns a default value.
//
// Parms:      hDlg     == Handle to dialog box.
//             wCtrlID  == ID of control we're getting the int from.
//             nDefault == Value to return if edit control doesn't have
//                          a valid integer in it.
//
// History:   Date      Reason
//            11/13/91  Created
//            11/14/91  Use string table for error string.
//             
//---------------------------------------------------------------------

int GetDlgItemIntOrDef (HWND hDlg, int nCtrlID, int nDefault)
{
   int  nVal;
   BOOL bTrans;
   char szErr[MAX_HELP_LINE];

   nVal = GetDlgItemInt (hDlg, nCtrlID, &bTrans, FALSE);

   if (bTrans)
      return nVal;
   else
      if (LoadString (hInst, IDS_ERRXYSCALE, szErr, MAX_HELP_LINE))
         MessageBox (hDlg, szErr, NULL, MB_OK);
      else
         MessageBeep (0);

   return nDefault;
}

