/* (c) Copyright 1987 MICROGRAFX, Inc., All Rights Reserved.  Permission to
   use this work for any purpose must be obtained in writing from MICROGRAFX,
   1820 N. Greenville Ave., Richardson, Tx.  75081.

*******************************************************************************
*******************************************************************************

				  dialog

*******************************************************************************
*******************************************************************************

The driver supports page sizes A thru E, A4 thru A0, 24 and 36 inch wide rolls.
Only the HP-7550A and the plotters with roll-feed capability can select the
automatic page feed selection.  The preloaded option is allowed only under
manual feed and allows the user to load the initial page without being prompted
by the spooler.  Page orientation of landscape and portrait defaults to
portrait.  The carousel in the plotter is known as the current carousel and
its configuration is shown in the carousel configuration list box.  All
carousels that can be used for a drawing are the active carousels. */


#define PRINTDRIVER
#define NOPQ
#define NOINSTALLFILES

#define HELP_FILE	"hpplot.hlp"

#include <print.h>
#include <gdidefs.inc>
#include <windowsx.h>
#include "device.h"
#include "driver.h"
#include "dialog.h"
#include "initlib.h"
#include "output.h"
#include "profile.h"
#include "utils.h"
#include "devmode.h"
#include "data.h"
#include "hpplthlp.h"	// Help IDs


/* **************************** Global Data ********************************* */

// This table contains the device specific capabilities per plotter. 
DEVICEINFO DeviceInfo [NUM_PLOTTERS] =
{
    // ColorPro 
    {
	{ 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // Pen type support 
	{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },          // Media size support 
	{ 1, 0, 0, 1 },                                  // Options support
	0,                                               // Paper feed support? 
	8,                                               // Pens per carousel
	{ 1, 40, 1 },
	{ 0, 0, 0 },
	{ 0, 0, 0 }
    },
    // ColorPro with GEC 
    {
	{ 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // Pen type support 
	{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },          // Media size support 
	{ 1, 0, 0, 1 },                                  // Options support
	0,                                               // Paper feed support?
	8,                                               // Pens per carousel
	{ 1, 40, 1 },                                    // Speed (min, max, interval)
	{ 0, 0, 0 },                                     // Force (min, max, interval)
	{ 0, 0, 0 }                                      // Accel (min, max, interval)
    },
    // HP 7470A 
    {
	{ 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // Pen type support 
	{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },          // Media size support 
	{ 1, 0, 0, 1 },                                  // Options support
	0,                                               // Paper feed support?
	2,                                               // Pens per carousel
	{ 4, 38, 1 },                                    // Speed (min, max, interval)
	{ 0, 0, 0 },                                     // Force (min, max, interval)
	{ 0, 0, 0 }                                      // Accel (min, max, interval)
    },
    // HP 7475A 
    {
	{ 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 }, // Pen type support 
	{ 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0 },          // Media size support 
	{ 1, 0, 0, 1 },                                  // Options support
	0,                                               // Paper feed support?
	6,                                               // Pens per carousel
	{ 1, 38, 1 },                                    // Speed (min, max, interval)
	{ 0, 0, 0 },                                     // Force (min, max, interval)
	{ 0, 0, 0 }                                      // Accel (min, max, interval)
    },
    // HP 7550A 
    {
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // Pen type support 
	{ 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0 },          // Media size support 
	{ 1, 1, 1, 1 },                                  // Options support
	1,                                               // Paper feed support?
	8,                                               // Pens per carousel
	{ 1, 80, 1 },                                    // Speed (min, max, interval)
	{ 0, 7, 1 },                                     // Force (min, max, interval)
	{ 1, 6, 1 }                                      // Accel (min, max, interval)
    },
    // HP 7580A 
    {
	{ 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // Pen type support 
	{ 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0 },          // Media size support 
	{ 1, 1, 1, 1 },                                  // Options support
	0,                                               // Paper feed support?
	8,                                               // Pens per carousel
	{ 1, 60, 1 },                                    // Speed (min, max, interval)
	{ 0, 7, 1 },                                     // Force (min, max, interval)
	{ 1, 4, 1 }                                      // Accel (min, max, interval)
    },
    // HP 7585A 
    {
	{ 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // Pen type support 
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },          // Media size support 
	{ 1, 1, 1, 1 },                                  // Options support
	0,                                               // Paper feed support?
	8,                                               // Pens per carousel
	{ 1, 60, 1 },                                    // Speed (min, max, interval)
	{ 0, 7, 1 },                                     // Force (min, max, interval)
	{ 1, 4, 1 }                                      // Accel (min, max, interval)
    },
    // HP 7580B 
    {
	{ 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // Pen type support 
	{ 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0 },          // Media size support 
	{ 1, 1, 1, 1 },                                  // Options support
	0,                                               // Paper feed support?
	8,                                               // Pens per carousel
	{ 1, 60, 1 },                                    // Speed (min, max, interval)
	{ 0, 7, 1 },                                     // Force (min, max, interval)
	{ 1, 4, 1 }                                      // Accel (min, max, interval)
    },
    // HP 7585B 
    {
	{ 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // Pen type support 
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },          // Media size support 
	{ 1, 1, 1, 1 },                                  // Options support
	0,                                               // Paper feed support?
	8,                                               // Pens per carousel
	{ 1, 60, 1 },                                    // Speed (min, max, interval)
	{ 0, 7, 1 },                                     // Force (min, max, interval)
	{ 1, 4, 1 }                                      // Accel (min, max, interval)
    },
    // HP 7586B 
    {
	{ 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // Pen type support 
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },          // Media size support 
	{ 1, 1, 1, 1 },                                  // Options support
	1,                                               // Paper feed support?
	8,                                               // Pens per carousel
	{ 1, 60, 1 },                                    // Speed (min, max, interval)
	{ 0, 7, 1 },                                     // Force (min, max, interval)
	{ 1, 4, 1 }                                      // Accel (min, max, interval)
    },
    // DraftPro 
    {
	{ 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1 }, // Pen type support 
	{ 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0 },          // Media size support 
	{ 1, 0, 0, 1 },                                  // Options support
	0,                                               // Paper feed support?
	8,                                               // Pens per carousel
	{ 1, 40, 1 },                                    // Speed (min, max, interval)
	{ 0, 0, 0 },                                     // Force (min, max, interval)
	{ 0, 0, 0 }                                      // Accel (min, max, interval)
    },
    // DRAFTPRO DXL 
    {
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // Pen type support 
	{ 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0 },          // Media size support 
	{ 1, 0, 0, 1 },                                  // Options support
	0,                                               // Paper feed support?
	8,                                               // Pens per carousel
	{ 1, 60, 1 },                                    // Speed (min, max, interval)
	{ 0, 0, 0 },                                     // Force (min, max, interval)
	{ 0, 0, 0 }                                      // Accel (min, max, interval)
    },
    // DRAFTPRO EXL  
    {
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // Pen type support 
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },          // Media size support 
	{ 1, 0, 0, 1 },                                  // Options support
	0,                                               // Paper feed support?
	8,                                               // Pens per carousel
	{ 1, 60, 1 },                                    // Speed (min, max, interval)
	{ 0, 0, 0 },                                     // Force (min, max, interval)
	{ 0, 0, 0 }                                      // Accel (min, max, interval)
    },
    // DraftMaster I
    {
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // Pen type support 
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },          // Media size support 
	{ 1, 1, 1, 1 },                                  // Options support
	0,                                               // Paper feed support?
	8,                                               // Pens per carousel
	{ 1, 60, 1 },                                    // Speed (min, max, interval)
	{ 0, 7, 1 },                                     // Force (min, max, interval)
	{ 1, 4, 1 }                                      // Accel (min, max, interval)
    },
    // DraftMaster II 
    {
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // Pen type support 
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },          // Media size support 
	{ 1, 1, 1, 1 },                                  // Options support
	1,                                               // Paper feed support?
	8,                                               // Pens per carousel
	{ 1, 60, 1 },                                    // Speed (min, max, interval)
	{ 0, 7, 1 },                                     // Force (min, max, interval)
	{ 1, 4, 1 }                                      // Accel (min, max, interval)
    }
};

// ************************** Exported Routines **************************** 

//*************************************************************
//
//  about
//
//  Purpose: Dialog proc for About dialog.
//              
//
//
//  Parameters:
//      hWnd
//      Message
//      wParam
//      lParam
//      
//
//  Return: (BOOL FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

BOOL FAR PASCAL about (hWnd,Message,wParam,lParam)
HANDLE   hWnd;
unsigned Message;
WORD     wParam;
DWORD    lParam;
{
   switch (Message)
   {
      case WM_COMMAND:
	 switch (wParam)
	 {
	    case IDOK: 
	       EndDialog (hWnd,wParam);
	       return TRUE;

	    default:
	       break;
	 }
	 break;

      default:
	break;
   }
   return FALSE;

} //*** about

//*************************************************************
//
//  get_group
//
//  Purpose: This function returns the group in which a pen is 
//           classified from its given type.  (Example: P3 and 
//           P7 are paper pens, T3 and T6 are transparency 
//           pens, etc.
//
//  Parameters:
//      short Type
//      LPENVIRONMENT lpSetup
//      
//
//  Return: (short FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

short FAR PASCAL get_group (short Type, LPENVIRONMENT lpSetup)
{
    short Result;

    Type += P3;
    if (WITHIN (Type,P3,P7))
	Result = PAPER;
    else if (WITHIN (Type,T3,T6))
	Result = TRANSPARENCY;
    else if (Type == R3)
	Result = ROLLERBALL;
    else if (WITHIN (Type,V25,V70))
	Result = DISPOSABLE;
    else
	Result = REFILLABLE;
    return (Result);

} //*** get_group

//*************************************************************
//
//  PaperDlgProc
//
//  Purpose: This is the dialog proc for the Paper property 
//           sheet
//
//
//  Parameters:
//      hDlg
//      Message
//      wParam
//      lParam
//      
//
//  Return: (BOOL FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

#pragma data_seg(".text")
const static DWORD aPaperDlgHelpIDs[] = {  // Context Help IDs
    IDD_PAPERSIZE_LABEL,     IDH_HPPLOT_PAPERSIZE,
    IDD_PAPERSIZE,           IDH_HPPLOT_PAPERSIZE,
    IDD_LENGTH_LABEL,        IDH_HPPLOT_PAPERSIZE,
    IDD_LENGTH,              IDH_HPPLOT_PAPERSIZE,
    IDD_LENGTH_LABEL2,       IDH_HPPLOT_PAPERSIZE,
    IDD_PAPERSOURCE_LABEL,   IDH_HPPLOT_PAPERSOURCE,
    IDD_PAPERSOURCE,         IDH_HPPLOT_PAPERSOURCE,
    IDD_ORIENT_GROUP,        IDH_HPPLOT_ORIENT,
    IDD_ORIENT_ICON,         IDH_HPPLOT_ORIENT,
    IDD_ORIENT_PORTRAIT,     IDH_HPPLOT_ORIENT,
    IDD_ORIENT_LANDSCAPE,    IDH_HPPLOT_ORIENT,
    IDD_PROMPT,              IDH_HPPLOT_PROMPT,
    IDD_DEFAULT,             IDH_HPPLOT_DEFAULT,

    0, 0
};
#pragma data_seg()

BOOL FAR PASCAL PaperDlgProc (hDlg,Message,wParam,lParam)
HWND   hDlg;
UINT   Message;
WPARAM wParam;
LPARAM lParam;
{
   LPDI lpdi;

   switch (Message)
   {
      case WM_INITDIALOG:
	 lpdi = (LPDI)(((LPPROPSHEETPAGE)lParam)->lParam);
	 SetWindowLong(hDlg, DWL_USER, (LONG)lpdi);
	 InitPapersDlg(hDlg, lpdi);
	 break;

      case WM_COMMAND:
	 lpdi = (LPDI)GetWindowLong(hDlg, DWL_USER);
	 
	 switch (wParam)
	 {
	    // prompt for paper checkbox
	    case IDD_PROMPT:
	       lpdi->lpDM->Preloaded = !lpdi->lpDM->Preloaded;
	       SendDlgItemMessage(hDlg, wParam, BM_SETCHECK, lpdi->lpDM->Preloaded, 0);
	       break;

	    // orientation buttons 
	    case IDD_ORIENT_PORTRAIT:
	    case IDD_ORIENT_LANDSCAPE:
	       lpdi->lpDM->Orientation = wParam - IDD_ORIENT_PORTRAIT;
	       lpdi->lpDM->dm.dmOrientation = (lpdi->lpDM->Orientation == 0) ? 
			     DMORIENT_PORTRAIT : DMORIENT_LANDSCAPE;

	       // set the new button and use the appropriate icon
	       SetOrientation(hDlg, lpdi->lpDM->dm.dmOrientation, lpdi);
	       break;

	    // paper size and paper source comboboxes
	    case IDD_PAPERSIZE:
	    case IDD_PAPERSOURCE:
	       if(HIWORD(lParam) == CBN_SELENDOK)
	       {
		  int iCurrSel = (int)SendDlgItemMessage(hDlg, wParam, CB_GETCURSEL,0,0);
		  int iCurrVal = (int)SendDlgItemMessage(hDlg, wParam, CB_GETITEMDATA,iCurrSel,0);

		  if (wParam == IDD_PAPERSOURCE)
		  {
		     lpdi->lpDM->PaperFeed = iCurrVal;
		     lpdi->lpDM->dm.dmDefaultSource = MANUAL + iCurrVal;

		    // check to see if paper feed is Automatic.  If so, disable
		    // prompt for page checkbox
		    CheckPaperFeed(hDlg);
		  }
		  else
		  {
		     lpdi->lpDM->Size = iCurrVal;
		     lpdi->lpDM->dm.dmPaperSize = SIZE_A + iCurrVal;

		     // check to see if paper size is a roll length paper.  If so,
		     // display the length edit control
		     CheckPaperSize(hDlg);
		  }
	       }
	       break;

	    // set defaults button
	    case IDD_DEFAULT:
	       SetPaperDefaults(hDlg);
	       break;

	    default:
	       return (FALSE);
	 }
	 break;

      // property sheet notification messages
      case WM_NOTIFY:
      {
	  NMHDR FAR *lpnmhdr=(NMHDR FAR *)lParam;

	  lpdi=(LPDI)GetWindowLong(hDlg,DWL_USER);
	  switch(lpnmhdr->code)
	  {
	      case PSN_KILLACTIVE:
		  // if roll size, must validate length
		  // before allowing the dialog to close
		  if(!ValidateRollLength(hDlg))
		  {
		      SetWindowLong(hDlg,DWL_MSGRESULT,TRUE);
		      return TRUE;
		  }
		  return FALSE;
		  break;

	      case PSN_APPLY:
		  lpdi->bOK=TRUE;  // Fall through...

	      case PSN_RESET:
		  return FALSE;
		  break;

	      case PSN_HELP:
		  break;
	  }
      }
      break;

      case WM_HELP:
          WinHelp((HWND)((LPHELPINFO) lParam)->hItemHandle, HELP_FILE,
              HELP_WM_HELP, (DWORD)(LPSTR) aPaperDlgHelpIDs);
          break;

      case WM_CONTEXTMENU:
          WinHelp((HWND) wParam, HELP_FILE, HELP_CONTEXTMENU,
              (DWORD)(LPVOID) aPaperDlgHelpIDs);
          break;

      default:
	 return FALSE;
   }

   return TRUE;

} //*** PaperDlgProc

//*************************************************************
//
//  DeviceOptionsDlgProc
//
//  Purpose: This is the dialog proc for the Device Options
//           property sheet
//
//
//  Parameters:
//      hDlg
//      Message
//      wParam
//      lParam
//      
//
//  Return: (BOOL FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

#pragma data_seg(".text")
const static DWORD aDevOptsDlgHelpIDs[] = {  // Context Help IDs
    IDD_LABEL_1,       IDH_HPPLOT_CURCARSEL,
    IDD_LABEL_2,       IDH_HPPLOT_CURCARSEL,
    IDD_CURCARSEL,     IDH_HPPLOT_CURCARSEL,
    IDD_LABEL_3,       IDH_HPPLOT_ACTCARSEL,
    IDD_LABEL_4,       IDH_HPPLOT_ACTCARSEL,
    IDD_ACTCARSEL,     IDH_HPPLOT_ACTCARSEL,
    IDD_LABEL_5,       IDH_HPPLOT_COLORS,
    IDD_COLORS,        IDH_HPPLOT_COLORS,
    IDD_LABEL_6,       IDH_HPPLOT_TYPES,
    IDD_TYPES,         IDH_HPPLOT_TYPES,
    IDD_LABEL_7,       IDH_HPPLOT_PRIORITY,
    IDD_PRIORITY,      IDH_HPPLOT_PRIORITY,
    IDD_LABEL_8,       IDH_HPPLOT_SPEEDBOX,
    IDD_SPEEDFRAME,    IDH_HPPLOT_SPEEDBOX,
    IDD_SPEEDBOX,      IDH_HPPLOT_SPEEDBOX,
    IDD_SPEEDSB,       IDH_HPPLOT_SPEEDBOX,
    IDD_LABEL_9,       IDH_HPPLOT_FORCEBOX,
    IDD_FORCEFRAME,    IDH_HPPLOT_FORCEBOX,
    IDD_FORCEBOX,      IDH_HPPLOT_FORCEBOX,
    IDD_FORCESB,       IDH_HPPLOT_FORCEBOX,
    IDD_LABEL_10,      IDH_HPPLOT_ACCELBOX,
    IDD_ACCELFRAME,    IDH_HPPLOT_ACCELBOX,
    IDD_ACCELBOX,      IDH_HPPLOT_ACCELBOX,
    IDD_ACCELSB,       IDH_HPPLOT_ACCELBOX,
    IDD_CARSELGRP,     IDH_HPPLOT_CARSELLB,
    IDD_LABEL_11,      IDH_HPPLOT_CARSELLB,
    IDD_LABEL_12,      IDH_HPPLOT_CARSELLB,
    IDD_LABEL_13,      IDH_HPPLOT_CARSELLB,
    IDD_LABEL_14,      IDH_HPPLOT_CARSELLB,
    IDD_LABEL_15,      IDH_HPPLOT_CARSELLB,
    IDD_LABEL_16,      IDH_HPPLOT_CARSELLB,
    IDD_LABEL_17,      IDH_HPPLOT_CARSELLB,
    IDD_CARSELLB,      IDH_HPPLOT_CARSELLB,
    IDD_CARSELLB,      IDH_HPPLOT_CARSELLB,
    IDD_DRAFT,         IDH_HPPLOT_DRAFT,
    IDD_PENRESET,      IDH_HPPLOT_PENRESET,
		
    0, 0
};
#pragma data_seg()

BOOL FAR PASCAL DeviceOptionsDlgProc (hDlg,Message,wParam,lParam)
HWND   hDlg;
UINT   Message;
WPARAM wParam;
LPARAM lParam;
{
   LPDI lpdi;

   switch (Message)
   {
      case WM_INITDIALOG:
	 lpdi = (LPDI)(((LPPROPSHEETPAGE)lParam)->lParam);
	 SetWindowLong(hDlg, DWL_USER, (LONG)lpdi);
	 InitDevOptsDlg(hDlg, lpdi);
	 break;

      // handle spin controls
      case WM_VSCROLL:
	 UpdateSpinCtrl(hDlg, wParam, lParam);
	 break;

      case WM_COMMAND:
	 lpdi = (LPDI)GetWindowLong(hDlg, DWL_USER);
	 
	 switch (wParam)
	 {
	    // device options set default button
	    case IDD_PENRESET:
	       SetDevOptsDefaults(hDlg, lpdi);
	       break;

	    // draft check box
	    case IDD_DRAFT:
	       lpdi->lpDM->Draft = !lpdi->lpDM->Draft;
	       SendDlgItemMessage(hDlg, wParam, BM_SETCHECK, lpdi->lpDM->Draft, 0);
	       break;

	    // current carousel listbox
	    case IDD_CURCARSEL:
	       if (HIWORD(lParam) == LBN_SELCHANGE)
	       {

		  // update current carousel based on current selected carousel choice
		  lpdi->lpDM->CurrentCarousel = (int)SendDlgItemMessage(hDlg, IDD_CURCARSEL,
						    LB_GETCURSEL, 0, 0);

		  // update other controls to reflect the corousel change   
		  display_colors(hDlg, lpdi);
		  display_priorities(hDlg, lpdi);
		  display_pens(hDlg, lpdi);
		  display_SFA(hDlg, 0, lpdi);
	       }
	       break;

	    // active carousels listbox
	    case IDD_ACTCARSEL:
	       if (HIWORD(lParam) == LBN_SELCHANGE)
	       {
		  int iaActiveCarsels[MAX_CAROUSELS];
		  int iNumActiveCarsels, i;

		  // set all entries in  old list to FALSE
		  for (i=0; i < MAX_CAROUSELS; i++)
		     lpdi->lpDM->ActiveCarousel[i] = FALSE;

		  // get the currently selected items
		  iNumActiveCarsels = (int)SendDlgItemMessage(hDlg, IDD_ACTCARSEL,
							      LB_GETSELITEMS,
							      (WPARAM)MAX_CAROUSELS,
							      (LPARAM)(int FAR*)iaActiveCarsels);

		  // save which carousels are active
		  for (i=0; i < iNumActiveCarsels; i++)
		     lpdi->lpDM->ActiveCarousel[iaActiveCarsels[i]] = TRUE;
	       }
	       break;

	    // pen carousel list box
	    case IDD_CARSELLB:
	       if (HIWORD(lParam) == LBN_SELCHANGE)
	       {
		  int iPen = (int)SendDlgItemMessage(hDlg, wParam, LB_GETCURSEL, 0, 0);

		  // change apporpriate controls to reflect selected pen
		  display_colors(hDlg, lpdi);
		  display_priorities(hDlg, lpdi);
		  display_SFA(hDlg, iPen, lpdi);
	       }
	       break;

	    // pen colors combobox
	    case IDD_COLORS:
	       if(HIWORD(lParam) == CBN_SELENDOK)
	       {
		  int iCurrColorIndex, iCurrColor, iCurrPen, iCurrCarsel;

		  // get curr color selection
		  iCurrColorIndex = (int)SendDlgItemMessage(hDlg, IDD_COLORS, 
							    CB_GETCURSEL, 0, 0);
		  iCurrColor = (int)SendDlgItemMessage(hDlg, IDD_COLORS,
						       CB_GETITEMDATA,
						       iCurrColorIndex, 0);

		  // get current pen selection
		  iCurrPen = (int)SendDlgItemMessage(hDlg, IDD_CARSELLB, LB_GETCURSEL, 0, 0);
   
		  // get current carousel
		  iCurrCarsel = lpdi->lpDM->CurrentCarousel;

		  // change color of current carousel's current pen
		  lpdi->lpDM->Carousel[iCurrCarsel].Pen[iCurrPen].Color = iCurrColor;

		  // display pen with new color
		  display_pen(hDlg, iCurrPen, lpdi);
	       }
	       break;

	    // pen types combobox
	    case IDD_TYPES:
	       if(HIWORD(lParam) == CBN_SELENDOK)
	       {
		  int iCurrTypeIndex, iCurrType, iCurrPen, iCurrCarsel;

		  // get current type selection
		  iCurrTypeIndex = (int)SendDlgItemMessage(hDlg, IDD_TYPES, 
							   CB_GETCURSEL, 0, 0);
		  iCurrType = (int)SendDlgItemMessage(hDlg, IDD_COLORS, 
						      CB_GETITEMDATA,
						      iCurrTypeIndex, 0);

		  // get current pen selection
		  iCurrPen = (int)SendDlgItemMessage(hDlg, IDD_CARSELLB, 
						     LB_GETCURSEL, 0, 0);
   
		  // get current carousel
		  iCurrCarsel = lpdi->lpDM->CurrentCarousel;

		  // change type of current carousel's current pen
		  lpdi->lpDM->Carousel[iCurrCarsel].Pen[iCurrPen].Type = iCurrType;

		  // since type changed, color list can change
		  display_colors(hDlg, lpdi);

		  // display pen with new type
		  display_pen(hDlg, iCurrPen, lpdi);
	       }
	       break;

	    // pen priorities combobox
	    case IDD_PRIORITY:
	       if(HIWORD(lParam) == CBN_SELENDOK)
	       {
		  int iCurrPriority, iCurrPen, iCurrCarsel;

		  // get curr priority selection
		  iCurrPriority = (int)SendDlgItemMessage(hDlg, IDD_PRIORITY, CB_GETCURSEL, 0, 0);

		  // get current pen selection
		  iCurrPen = (int)SendDlgItemMessage(hDlg, IDD_CARSELLB, LB_GETCURSEL, 0, 0);
   
		  // get current carousel
		  iCurrCarsel = lpdi->lpDM->CurrentCarousel;

		  // change priority of current carousel's current pen
		  lpdi->lpDM->Carousel[iCurrCarsel].Pen[iCurrPen].Usage = iCurrPriority;

		  // display pen with new priority
		  display_pen(hDlg, iCurrPen, lpdi);
	       }
	       break;

	    default:
	       return (FALSE);
	 }
	 break;

      // property sheet notification messages
      case WM_NOTIFY:
      {
	  NMHDR FAR *lpnmhdr=(NMHDR FAR *)lParam;

	  lpdi=(LPDI)GetWindowLong(hDlg,DWL_USER);
	  switch(lpnmhdr->code)
	  {
	      case PSN_KILLACTIVE:
		  return FALSE;
		  break;

	      case PSN_APPLY:
		  lpdi->bOK=TRUE;  // Fall through...

	      case PSN_RESET:
		  return FALSE;
		  break;

	      case PSN_HELP:
		  break;
	  }
      }
      break;

      case WM_HELP:
          WinHelp((HWND)((LPHELPINFO) lParam)->hItemHandle, HELP_FILE,
              HELP_WM_HELP, (DWORD)(LPSTR) aDevOptsDlgHelpIDs);
          break;

      case WM_CONTEXTMENU:
          WinHelp((HWND) wParam, HELP_FILE, HELP_CONTEXTMENU,
              (DWORD)(LPVOID) aDevOptsDlgHelpIDs);
          break;

      default:
	 return FALSE;
   }

   return TRUE;

} //*** DeviceOptionsDlgProc

// *************** Beg of PaperDlg supporting code *************

//*************************************************************
//
//  InitPapersDlg
//
//  Purpose: Initializes Papers property sheet
//              
//
//
//  Parameters:
//      HWND hDlg
//      LPDI lpdi
//      
//
//  Return: (void)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

void InitPapersDlg(HWND hDlg, LPDI lpdi)
{
   int i, iCount;
   WORD wCurSel;
   HWND hPromptBox = GetDlgItem(hDlg, IDD_PROMPT);

   SendDlgItemMessage(hDlg, IDD_PAPERSIZE, CB_RESETCONTENT, 0, 0);

   // fill paper size combobox with supported paper sizes
   for (i=0, iCount=0; i < MAX_MEDIA_SIZES; i++)
   {
      // check to see if paper size is supported
      if (DeviceInfo[lpdi->lpDM->Plotter].MediaSizeSupport[i])
      {
	 // save index of current paper size
	 if (lpdi->lpDM->Size == i)
	    wCurSel = iCount;

	 SendDlgItemMessage(hDlg, IDD_PAPERSIZE, CB_INSERTSTRING, (WPARAM)iCount, 
			    (LPARAM)(LPSTR)GetString(SIZE_A + i));

	 // store paper id
	 SendDlgItemMessage(hDlg, IDD_PAPERSIZE, CB_SETITEMDATA, (WPARAM)iCount,
			    (LPARAM)(DWORD)i);

	 iCount++;
      }
   }
   SendDlgItemMessage(hDlg, IDD_PAPERSIZE, CB_SETCURSEL, wCurSel, 0);

   // check for roll paper and show roll length window if necessary
   CheckPaperSize(hDlg);

   SendDlgItemMessage(hDlg, IDD_PAPERSOURCE, CB_RESETCONTENT, 0, 0);

   // fill paper source combobox with supported paper feeds
   for (i=0, iCount=0; i < MAX_FEEDS; i++)
   {
      // manual (i == 0) is always supported
      if ((i == 0) || (i == DeviceInfo[lpdi->lpDM->Plotter].PaperFeedSupport))
      {
	 // save index of current paper source
	 if (lpdi->lpDM->PaperFeed == i)
	    wCurSel = iCount;

	 SendDlgItemMessage(hDlg, IDD_PAPERSOURCE, CB_INSERTSTRING, (WPARAM)iCount, 
			    (LPARAM)(LPSTR)GetString(MANUAL + i));

	 SendDlgItemMessage(hDlg, IDD_PAPERSOURCE, CB_SETITEMDATA, (WPARAM)iCount, 
			    (LPARAM)(DWORD)i);
	 iCount++;
      }
   }
   SendDlgItemMessage(hDlg, IDD_PAPERSOURCE, CB_SETCURSEL, wCurSel, 0);

   // set orientation button and icon
   SetOrientation(hDlg, lpdi->lpDM->Orientation ? DMORIENT_LANDSCAPE :
		  DMORIENT_PORTRAIT,lpdi);

   // check to see if automatic feed is the current choice.  if so, then
   // disable page prompt box
   EnableWindow(hPromptBox, !lpdi->lpDM->PaperFeed);

   // do only if paper feed is manual
   if (!lpdi->lpDM->PaperFeed)
      CheckDlgButton(hDlg, IDD_PROMPT, !lpdi->lpDM->Preloaded);

} //*** InitPapersDlg

//*************************************************************
//
//  SetOrientation
//
//  Purpose: Set the current orientation button and display
//           the appropriate icon
//
//
//  Parameters:
//      HWND hDlg
//      WORD wOrientation
//      LPDI lpdi
//      
//
//  Return: (void)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

void SetOrientation(HWND hDlg, WORD wOrientation, LPDI lpdi)
{
    HICON hIcon;
    BOOL  bPortrait=(DMORIENT_PORTRAIT==wOrientation);

    CheckRadioButton(hDlg, IDD_ORIENT_PORTRAIT, IDD_ORIENT_LANDSCAPE,
		     bPortrait?IDD_ORIENT_PORTRAIT:IDD_ORIENT_LANDSCAPE);

    // display the corresponding icon.
    hIcon = LoadIcon(lpdi->hInst,
		     bPortrait?
		     MAKEINTRESOURCE(ICO_PORTRAIT):
		     MAKEINTRESOURCE(ICO_LANDSCAPE));

    SendDlgItemMessage(hDlg, IDD_ORIENT_ICON, STM_SETICON, (WORD)hIcon, 0L);

} //*** SetOrientation

//*************************************************************
//
//  CheckPaperFeed
//
//  Purpose: 
//              
//
//
//  Parameters:
//      HWND hDlg
//      
//
//  Return: (void)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

void CheckPaperFeed(HWND hDlg)
{
   HWND hPromptBox;
   LPDI lpdi = (LPDI)GetWindowLong(hDlg,DWL_USER);

   // enable or disable page prompt checkbox
   hPromptBox = GetDlgItem(hDlg, IDD_PROMPT);

   if (hPromptBox)
      EnableWindow(hPromptBox, !lpdi->lpDM->PaperFeed);

} //*** CheckPaperFeed

//*************************************************************
//
//  SetPaperDefaults
//
//  Purpose: Set Papers property sheet back to its default
//           values
//
//
//  Parameters:
//      HWND hDlg
//      
//
//  Return: (void)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

void SetPaperDefaults(HWND hDlg)
{
   LPDI lpdi = (LPDI)GetWindowLong(hDlg,DWL_USER);

   // get default settings for current model
   get_defaults(lpdi->lpDM,TRUE);

   InitPapersDlg(hDlg,lpdi);

} //*** SetPaperDefaults

//*************************************************************
//
//  ValidateRollLength
//
//  Purpose: Make sure that if a roll paper is selected that the
//           typed in length is in the 1 <= length <= 161 inches 
//           range.  Cannot exit dialog unit in this range.  A 
//           message box will pop up and tell the user what the
//           range is if he is out of range.
//
//
//  Parameters:
//      HWND hDlg
//      
//
//  Return: (BOOL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

BOOL ValidateRollLength(HWND hDlg)
{
   WORD wLength;
   BOOL bSuccess;
   LPDI lpdi = (LPDI)GetWindowLong(hDlg,DWL_USER);

   // if paper is a roll size, get length and make sure it is a
   // valid value.  If not a valid value, pop a message box up
   // showing the correct values and return FALSE.  Set focus to
   // the Length edit control.
   if (lpdi->lpDM->Size > 9)
   {
      wLength = GetDlgItemInt(hDlg, IDD_LENGTH, (BOOL FAR *)&bSuccess, FALSE);      

      if (((wLength < 1) || (wLength > 161)) || (!bSuccess))
      {
	 MessageBox(hDlg, "Roll Length must be between 1 or 161 inches", 
		    "Invalid Roll Length Value", MB_ICONSTOP|MB_OK);

	 SetFocus(GetDlgItem(hDlg, IDD_LENGTH));
	 return FALSE;
      }
      lpdi->lpDM->Length = (short)wLength;
   }
   else
      lpdi->lpDM->Length = 0;

   return TRUE;

} //*** ValidateRollLength

//*************************************************************
//
//  CheckPaperSize
//
//  Purpose: Check to see if paper size is a roll length.  If so,
//           display roll length edit control.  If size is not
//           a roll length, the hide the roll length window, if
//           showing.
//
//
//  Parameters:
//      HWND hDlg
//      
//
//  Return: (void)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

void CheckPaperSize(HWND hDlg)
{
   LPDI lpdi = (LPDI)GetWindowLong(hDlg,DWL_USER);

   // If the size is not a roll size, then hide length window and text.
   if (lpdi->lpDM->Size < 9) 
      DisplayRollWindows(hDlg, FALSE);

   else
   {
      // make sure current roll length is in valid range.  If not, set to
      // default value
      if ((lpdi->lpDM->Length < 1) || (lpdi->lpDM->Length > 161))
      {
	 // 24 in wide roll
	 if (lpdi->lpDM->Size == 10)
	    lpdi->lpDM->Length = 36;

	 // // 36 in wide roll
	 else if (lpdi->lpDM->Size == 11)
	    lpdi->lpDM->Length = 48;

	 else
	    lpdi->lpDM->Length = 0;
      }
      
      DisplayRollWindows(hDlg, TRUE);
      SetDlgItemInt(hDlg,IDD_LENGTH,lpdi->lpDM->Length,FALSE);
   }

} //*** CheckPaperSize

//*************************************************************
//
//  DisplayRollWindows
//
//  Purpose: Hide/unhide roll length window
//              
//
//
//  Parameters:
//      HWND hDlg
//      BOOL bShow
//      
//
//  Return: (void)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

void DisplayRollWindows(HWND hDlg, BOOL bShow)
{
   int i;

   for (i=IDD_LENGTH_LABEL; i <= IDD_LENGTH_LABEL2; i++)
      ShowWindow(GetDlgItem(hDlg, i), bShow ? SW_SHOW: SW_HIDE);

} //*** DisplayRollWindows

// ************** End of PaperDlg supporting code *************

// ************** Beg of DeviceOptionsDlg supporting code *************

//*************************************************************
//
//  InitDevOptsDlg
//
//  Purpose: Initialize the Device Options property sheet
//              
//
//
//  Parameters:
//      HWND hDlg
//      LPDI lpdi
//      
//
//  Return: (void)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

void InitDevOptsDlg(HWND hDlg, LPDI lpdi)
{
   int i;
   char szStr[2];

   // init current and active carousel listboxes
   SendDlgItemMessage(hDlg, IDD_CURCARSEL, LB_RESETCONTENT, 0, 0);
   SendDlgItemMessage(hDlg, IDD_ACTCARSEL, LB_RESETCONTENT, 0, 0);

   for (i=0; i < MAX_CAROUSELS; i++)
   {
      szStr[0] = (char)('1' + i);
      szStr[1] = '\0';
      SendDlgItemMessage(hDlg, IDD_CURCARSEL, LB_ADDSTRING, 0, (LPARAM)(LPSTR)szStr);
      SendDlgItemMessage(hDlg, IDD_ACTCARSEL, LB_ADDSTRING, 0, (LPARAM)(LPSTR)szStr);

      if (lpdi->lpDM->ActiveCarousel[i])
	 SendDlgItemMessage(hDlg, IDD_ACTCARSEL, LB_SETSEL, TRUE,
			    MAKELPARAM(i,0));
   }
   SendDlgItemMessage(hDlg, IDD_CURCARSEL, LB_SETCURSEL, lpdi->lpDM->CurrentCarousel, 0);

   display_pens(hDlg,lpdi);

   display_colors(hDlg,lpdi);

   display_priorities(hDlg,lpdi);

   // check to see what options (speed, force, acceleration) the device 
   // supports.  Disable any option windows the device doesn't support and
   // init any it does support
   for (i=0; i < 3; i++)
   {
     if (!DeviceInfo[lpdi->lpDM->Plotter].OptionsSupport[i])
     {
	 int j;
	 HWND hCtrl; 

	 // disable unsupported windows
	 for (j=0; j < 2; j++)
	 {
	    hCtrl = GetDlgItem(hDlg, IDD_SPEEDBOX + (2*i) + j);        
	    EnableWindow(hCtrl, FALSE);
	 }
     }
     else
     {
      int iPen = 0;
      int iCurCarsel = lpdi->lpDM->CurrentCarousel;
      int iVal;
      
      if (i == 0)
	 iVal = lpdi->lpDM->Carousel[iCurCarsel].Pen [iPen].Speed;

      else if (i == 1)
      {
	 int iForceIndex = lpdi->lpDM->Carousel[iCurCarsel].Pen [iPen].Force;
	 iVal = ForceValue[lpdi->lpDM->Plotter][iForceIndex];
      }
      else
	 iVal = lpdi->lpDM->Carousel[iCurCarsel].Pen [iPen].Acceleration;

      SetDlgItemInt(hDlg, IDD_SPEEDBOX + (i*2), iVal, FALSE);
     }
   }
   // init draft checkbox
   CheckDlgButton(hDlg, IDD_DRAFT, lpdi->lpDM->Draft);

} //*** InitDevOptsDlg

//*************************************************************
//
//  SetDevOptsDefaults
//
//  Purpose: Restore Device options property sheet to its
//           default values
//
//
//  Parameters:
//      HWND hDlg
//      LPDI lpdi
//      
//
//  Return: (void)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

void SetDevOptsDefaults(HWND hDlg, LPDI lpdi)
{
   int i;

   // blank active carousel string because get_defaults doesn't do it.
   // should be moved to get_defaults
   for (i=0; i < MAX_CAROUSELS; i++) 
      lpdi->lpDM->ActiveCarousel[i] = FALSE;

   // get defaults for pen info
   get_defaults(lpdi->lpDM,FALSE);

   InitDevOptsDlg(hDlg,lpdi);

} //*** SetDevOptsDefaults

//*************************************************************
//
//  display_pen
//
//  Purpose: Builds pen description line.
//              
//
//
//  Parameters:
//      HWND hDlg
//      int iPen
//      LPDI lpdi
//      
//
//  Return: (void)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

void display_pen(HWND hDlg, int iPen, LPDI lpdi)
{
   LPSTR  lpBuf;
   int iColor;
   int iType;
   int iCurCarsel;
   LPSTR lpStr;
   HWND  hCarselLB = GetDlgItem (hDlg, IDD_CARSELLB);

   // allocate and lock string buffer
   lpBuf = GlobalAllocPtr(GHND,256);

   if(!lpBuf)
      return;

   // get current carousel as selected in current carousel lb
   iCurCarsel = lpdi->lpDM->CurrentCarousel;

   // put pen number into string
   lpStr = lpBuf;
   lpStr += wsprintf(lpBuf,"%d",iPen+1);

   // get values to insert into string.  if no color then only display
   // pen number
   if ((iColor = lpdi->lpDM->Carousel[iCurCarsel].Pen[iPen].Color) != 0)
   {
      // put color into string
      lpStr += wsprintf(lpStr, "\t%s\t", (LPSTR)GetString(YELLOW + (iColor-1)));

      // put type into string
      iType = lpdi->lpDM->Carousel[iCurCarsel].Pen[iPen].Type;
      lpStr += wsprintf(lpStr, "%s\t", (LPSTR)GetString(P3 + iType));

      // if priority is supported, put priority into string
      if (DeviceInfo[lpdi->lpDM->Plotter].OptionsSupport[3])
      {
	 LPINT lpPriority = &lpdi->lpDM->Carousel[iCurCarsel].Pen[iPen].Usage;

	 // if priority is not in valid range set to 0 (NONE)
	 if ((*lpPriority < 0) || (*lpPriority > 2))
	    *lpPriority = 0;   

	 lpStr += wsprintf(lpStr, "%s\t", (LPSTR) GetString( NONE + *lpPriority));
      }
      else
	 lpStr += wsprintf(lpStr, "\t");

      // if speed is supported, put speed into string
      if (DeviceInfo[lpdi->lpDM->Plotter].OptionsSupport[0])
      {
	 LPINT lpSpeed = &lpdi->lpDM->Carousel[iCurCarsel].Pen [iPen].Speed;

	 // if speed is in not in valid range then set speed to max
	 if ((*lpSpeed < DeviceInfo[lpdi->lpDM->Plotter].Speed.Min) ||
	    (*lpSpeed > DeviceInfo[lpdi->lpDM->Plotter].Speed.Max))
	    *lpSpeed = DeviceInfo[lpdi->lpDM->Plotter].Speed.Max;

	 lpStr += wsprintf(lpStr, "%d\t", *lpSpeed);
      }
      else
	 lpStr += wsprintf(lpStr, "\t");

      // if force is supported, put force into string
      if (DeviceInfo[lpdi->lpDM->Plotter].OptionsSupport[1])
      {
	 LPINT lpForce = &lpdi->lpDM->Carousel[iCurCarsel].Pen [iPen].Force;

	 // if force is in not in valid range then set force to min
	 if ((*lpForce < DeviceInfo[lpdi->lpDM->Plotter].Force.Min) ||
	    (*lpForce > DeviceInfo[lpdi->lpDM->Plotter].Force.Max))
	    *lpForce = DeviceInfo[lpdi->lpDM->Plotter].Force.Min;

	 lpStr += wsprintf(lpStr, "%d\t", ForceValue[lpdi->lpDM->Plotter][*lpForce]);
      }
      else
	 lpStr += wsprintf(lpStr, "\t");

      // if acceleration is supported, put accel into string
      if (DeviceInfo[lpdi->lpDM->Plotter].OptionsSupport[2])
      {
	 LPINT lpAccel = &lpdi->lpDM->Carousel[iCurCarsel].Pen [iPen].Acceleration;

	 // if accel is in not in valid range then set accel to max
	 if ((*lpAccel < DeviceInfo[lpdi->lpDM->Plotter].Acceleration.Min) ||
	    (*lpAccel > DeviceInfo[lpdi->lpDM->Plotter].Acceleration.Max))
	    *lpAccel = DeviceInfo[lpdi->lpDM->Plotter].Acceleration.Max;

	 lpStr += wsprintf(lpStr, "%d", *lpAccel);
      }
   }
   // delete old pen description string
   SendDlgItemMessage(hDlg,IDD_CARSELLB,LB_DELETESTRING,(WPARAM)iPen,0);

   // insert new pen description string
   SendDlgItemMessage(hDlg,IDD_CARSELLB,LB_INSERTSTRING,(WPARAM)iPen,(LPARAM)lpBuf);

   // select new string
   SendDlgItemMessage(hDlg,IDD_CARSELLB,LB_SETCURSEL,(WPARAM)iPen,0);

   GlobalFreePtr(lpBuf);

} //*** display_pen

//*************************************************************
//
//  display_pens
//
//  Purpose: Display current carousel's pens
//              
//
//
//  Parameters:
//      HWND hDlg
//      LPDI lpdi
//      
//
//  Return: (void)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

void display_pens(HWND hDlg, LPDI lpdi)
{
   short Index;
   int   iaTabStops[] = {23,63,83,117,140,163};
   HWND  hCarselLB = GetDlgItem (hDlg, IDD_CARSELLB);
   char szBuf[16];

   SendMessage (hCarselLB, WM_SETREDRAW, FALSE, 0);
   SendDlgItemMessage (hDlg, IDD_CARSELLB, LB_RESETCONTENT, 0, 0);

   // set group box name to include carousel number
   wsprintf((LPSTR)szBuf, "%s %d", (LPSTR)"Carousel",
	    lpdi->lpDM->CurrentCarousel+1);
   SetDlgItemText(hDlg, IDD_CARSELGRP, (LPSTR)szBuf);

   SendDlgItemMessage(hDlg, IDD_CARSELLB, LB_SETTABSTOPS, 6, (LPARAM)(int FAR*)iaTabStops);

   // display pens
   for (Index = 0; Index < DeviceInfo[lpdi->lpDM->Plotter].PensInCarousel; Index++)
       display_pen (hDlg,Index,lpdi);

   SendDlgItemMessage (hDlg,IDD_CARSELLB,LB_SETCURSEL,0,0);
   SendMessage (hCarselLB,WM_SETREDRAW,TRUE,0);

   InvalidateRect(hCarselLB,NULL,TRUE);

} //*** display_pens

//*************************************************************
//
//  display_colors
//
//  Purpose: Display colors into colors combobox.  Colors 
//           supported depend on current pen type.
//              
//
//
//  Parameters:
//      HWND hDlg
//      LPDI lpdi
//      
//
//  Return: (void)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

void display_colors(HWND hDlg, LPDI lpdi)
{
   BOOL NoPen = FALSE;
   int iCurrType, iCurrTypeIndex, iCurrColor, iCurrColorIndex, 
       iCurCarsel, iPenSelected,
       Index, iCurIndex;

    HWND hTypesCB  = GetDlgItem(hDlg, IDD_TYPES);
    HWND hColorsCB = GetDlgItem(hDlg, IDD_COLORS);

   // get current carousel as selected in current carousel lb
   iCurCarsel = lpdi->lpDM->CurrentCarousel;

   // get index of selected pen
   iPenSelected = (int)SendDlgItemMessage(hDlg, IDD_CARSELLB, LB_GETCURSEL,0,0);

   iCurrType  = lpdi->lpDM->Carousel[iCurCarsel].Pen[iPenSelected].Type;
   iCurrColor = lpdi->lpDM->Carousel[iCurCarsel].Pen[iPenSelected].Color;

   // if iCurrColor is zero, this means no color.  But if non zero, we have to 
   // decrement by one so it becomes the right color index
   if (iCurrColor)
      iCurrColor--;
   else
      NoPen = TRUE;

   SendMessage(hTypesCB, WM_SETREDRAW, FALSE, 0);
   SendMessage(hColorsCB, WM_SETREDRAW, FALSE, 0);

   // reset types and colors combobox
   SendDlgItemMessage(hDlg, IDD_TYPES, CB_RESETCONTENT, 0, 0);
   SendDlgItemMessage(hDlg, IDD_COLORS, CB_RESETCONTENT, 0, 0);

   // display types into type combobox
   for (Index = 0, iCurIndex=0; Index < MAX_PEN_TYPES; Index++)
   {
      // put types into types lb
      if (DeviceInfo[lpdi->lpDM->Plotter].PenTypeSupport[Index])
      {
	 if (iCurrType == Index)
	    iCurrTypeIndex = iCurIndex;

	 SendDlgItemMessage(hDlg, IDD_TYPES, CB_ADDSTRING, 0,
			    (LPARAM)(LPSTR)GetString(P3+Index));

	 SendDlgItemMessage(hDlg, IDD_TYPES, CB_SETITEMDATA, iCurIndex,
			    (LPARAM)(DWORD)Index);

	 iCurIndex++;
      }
   }
   SendDlgItemMessage(hDlg, IDD_TYPES, CB_SETCURSEL, iCurrTypeIndex, 0); 

   SendDlgItemMessage(hDlg, IDD_COLORS, CB_ADDSTRING, 0,
		      (LPARAM)(LPSTR)GetString(NONE));

   iCurrColorIndex = -1;
   // based on pen type put colors in colors lb
   for (Index = 0, iCurIndex=1; Index < MAX_COLORS; Index++)
   {
      // put colors into types lb
      if (ColorSupport[iCurrType][Index])
      {
	 if (iCurrColor == Index)
	    iCurrColorIndex = iCurIndex;

	 SendDlgItemMessage(hDlg, IDD_COLORS, CB_ADDSTRING, 0,
			    (LPARAM)(LPSTR)GetString(YELLOW+Index));

	 SendDlgItemMessage(hDlg, IDD_COLORS, CB_SETITEMDATA, iCurIndex,
			    (LPARAM)(DWORD)Index+1);

	 iCurIndex++;
      }
   }
   // check to make sure current color is supported for selected pen type
   // if it isn't supported, then change current color to be first color
   // selection in list

   // no color match found in current color list for current color
   if ((iCurrColorIndex == -1) && (!NoPen))
   {
      LPINT lpColor = (LPINT)&lpdi->lpDM->Carousel[iCurCarsel].Pen[iPenSelected].Color;

      // set color to 1st color in list (index 1, not 0 since 0 is None)
      *lpColor = (int)SendDlgItemMessage(hDlg, IDD_COLORS, CB_GETITEMDATA,
					 1, 0);
      iCurrColorIndex = 1;
   }
   SendDlgItemMessage(hDlg, IDD_COLORS, CB_SETCURSEL,
		      NoPen ? iCurrColor : iCurrColorIndex, 0); 

   // redraw type and colors comboxes
   SendMessage(hTypesCB, WM_SETREDRAW, TRUE, 0);
   SendMessage(hColorsCB, WM_SETREDRAW, TRUE, 0);

   InvalidateRect(hTypesCB, NULL, TRUE);
   InvalidateRect(hColorsCB, NULL, TRUE);

} //*** display_colors

//*************************************************************
//
//  display_priorities
//
//  Purpose: Display supported priorities.  All pens support
//           the same priorities
//
//
//  Parameters:
//      HWND hDlg
//      LPDI lpdi
//      
//
//  Return: (void)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

void display_priorities(HWND hDlg, LPDI lpdi)
{
   int Index, iCurCarsel, iCurrPriority, iPenSelected;

   SendDlgItemMessage(hDlg, IDD_PRIORITY, CB_RESETCONTENT, 0, 0);

   // get current carousel as selected in current carousel lb
   iCurCarsel = (int)SendDlgItemMessage(hDlg, IDD_CURCARSEL, LB_GETCURSEL, 0, 0);

   // get selected pen
   iPenSelected = (int)SendDlgItemMessage(hDlg, IDD_CARSELLB, LB_GETCURSEL,0,0);
       
   iCurrPriority = lpdi->lpDM->Carousel[iCurCarsel].Pen[iPenSelected].Usage;

   // diplay priorities
   for (Index = 0; Index < 3; Index++)
   {
      // put priorities into priority lb
      SendDlgItemMessage(hDlg, IDD_PRIORITY, CB_ADDSTRING, 0,
			(LPARAM)(LPSTR)GetString(NONE+Index));
   }
   SendDlgItemMessage(hDlg, IDD_PRIORITY, CB_SETCURSEL, iCurrPriority, 0); 

} //*** display_priorities

//*************************************************************
//
//  display_SFA
//
//  Purpose: Display the Speed, Force and Acceleration of the
//           selected pen.
//              
//
//  Parameters:
//      HWND hDlg
//      int iPen
//      LPDI lpdi
//      
//
//  Return: (void)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

void display_SFA(HWND hDlg, int iPen, LPDI lpdi)
{
   int iCurCarsel = lpdi->lpDM->CurrentCarousel;

   if (DeviceInfo[lpdi->lpDM->Plotter].OptionsSupport[0])
      SetDlgItemInt(hDlg, IDD_SPEEDBOX, lpdi->lpDM->Carousel[iCurCarsel].Pen [iPen].Speed, FALSE);

   if (DeviceInfo[lpdi->lpDM->Plotter].OptionsSupport[1])
   {
      int iForceIndex = lpdi->lpDM->Carousel[iCurCarsel].Pen [iPen].Force;
      SetDlgItemInt(hDlg, IDD_FORCEBOX, ForceValue[lpdi->lpDM->Plotter][iForceIndex], FALSE);
   }

   if (DeviceInfo[lpdi->lpDM->Plotter].OptionsSupport[2])
      SetDlgItemInt(hDlg, IDD_ACCELBOX, lpdi->lpDM->Carousel[iCurCarsel].Pen [iPen].Acceleration, FALSE);

} //*** display_SFA

//*************************************************************
//
//  UpdateSpinCtrl
//
//  Purpose: Handles the scrolling of the Speed, Force and
//           Acceleration spin controls.               
//
//
//  Parameters:
//      HWND hDlg
//      WPARAM wParam
//      LPARAM lParam
//      
//
//  Return: (void)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

void UpdateSpinCtrl(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
   LPINT lpCurrVal;
   OPTIONINFO FAR* lpOI;
   int  iCurCarsel;
   int  iCtrlID = GetDlgCtrlID((HWND)HIWORD(lParam));
   int  iPen = (int)SendDlgItemMessage(hDlg, IDD_CARSELLB, LB_GETCURSEL, 0, 0);
   LPDI lpdi = (LPDI)GetWindowLong(hDlg, DWL_USER);

   iCurCarsel = lpdi->lpDM->CurrentCarousel;

   switch (iCtrlID)
   {
      case IDD_SPEEDSB:
	 lpOI = &DeviceInfo[lpdi->lpDM->Plotter].Speed;
	 lpCurrVal = &lpdi->lpDM->Carousel[iCurCarsel].Pen [iPen].Speed;
	 break;

      case IDD_FORCESB:
	 lpOI = &DeviceInfo[lpdi->lpDM->Plotter].Force;
	 lpCurrVal = &lpdi->lpDM->Carousel[iCurCarsel].Pen [iPen].Force;
	 break;

      case IDD_ACCELSB:
	 lpOI = &DeviceInfo[lpdi->lpDM->Plotter].Acceleration;
	 lpCurrVal = &lpdi->lpDM->Carousel[iCurCarsel].Pen [iPen].Acceleration;
	 break;

      default:
	 return;
   }

   if ((wParam == SB_LINEUP && *lpCurrVal < lpOI->Max) ||
       (wParam == SB_LINEDOWN && *lpCurrVal > lpOI->Min))
   {
      int iValue;

      if (wParam == SB_LINEUP)
	  *lpCurrVal += lpOI->Increment;
      else
	  *lpCurrVal -= lpOI->Increment;

      if (iCtrlID == IDD_FORCESB)
	  iValue = ForceValue[lpdi->lpDM->Plotter][*lpCurrVal];
      else
	  iValue = *lpCurrVal;

      // display new value
      SetDlgItemInt (hDlg, iCtrlID-1, iValue, 0);
      display_pen(hDlg, iPen, lpdi);
   }

} //*** UpdateSpinCtrl

/*************** End of DeviceOptionsDlg supporting code *************/
