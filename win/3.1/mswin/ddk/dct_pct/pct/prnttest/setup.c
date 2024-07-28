/*---------------------------------------------------------------------------*\
| PRINTER TEST SETUP DIALOG                                                   |
|   This module contains the Printer Test Setup Dialog procedure for setting  |
|   up the test structure.                                                    |
|                                                                             |
| DATE   : June 05, 1989                                                      |
|   Copyright   1989-1992 by Microsoft Corporation                            |
\*---------------------------------------------------------------------------*/

#include    <Windows.H>
#include    "PrntTest.H"

#define RGB_SELECT   ((bColor)?RGB(0,0,255):RGB(0,0,0))
#define RGB_DESELECT ((bColor)?RGB(0,128,0):RGB(255,255,255))

/*---------------------------------------------------------------------------*\
| SETUP PRINT HEADER CHARACTERISTICS                                          |
|   This is the dialog box procedure which prompts for the Print Header       |
|   information.  The user has a choice between Expanded, Condensed options   |
|   for the information to be displayed.  Expanded being more detailed in its |
|   information.  Information can be excluded from the print by checking the  |
|   box.                                                                      |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HWND     hDlg     - DialogBox window Handle.                              |
|   unsigned iMessage - Message to be processed.                              |
|   WORD     wParam   - Information associated with message.                  |
|   LONG     lParam   - Information associated with message.                  |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   WORD wHeaderSet - Each bit represents an option to include or exclude.    |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if message was processed, or FALSE if it was not.  When the   |
|          user hits the OK button, then control goes back to the application.|
\*---------------------------------------------------------------------------*/
BOOL FAR PASCAL SetupHeaderDlg(HWND     hDlg,
                               unsigned iMessage,
                               WORD     wParam,
                               LONG     lParam)
{
  int           nIdx;
  char          acBuffer[10];
  static WORD   wFlags[] = {IDD_HEAD_TITLEPAGE ,IDD_HEAD_ENTRY, IDD_HEAD_GRAY,
                          IDD_HEAD_BOUNDS, IDD_HEAD_CAPS, IDD_HEAD_BRSH,
                          IDD_HEAD_PEN, IDD_HEAD_FONT, IDD_HEAD_EXP,
                          IDD_HEAD_CON};

  switch    (iMessage)
  {
    case  WM_INITDIALOG:

      /*
          Set up the various check boxes to reflect the current settings.
      */
      for (nIdx = 0; nIdx <  sizeof(wFlags) / sizeof(wFlags[0]); nIdx++)
        SendDlgItemMessage(hDlg, wFlags[nIdx], BM_SETCHECK,
              !!(wHeaderSet & wFlags[nIdx]), 0l);

      /*
          Disable the Expanded/Compressed font info buttons if we're not
          printing the font summary.
      */

      EnableWindow(GetDlgItem(hDlg, IDD_HEAD_CON),
                   (wHeaderSet & IDD_HEAD_FONT)?TRUE:FALSE);

      EnableWindow(GetDlgItem(hDlg, IDD_HEAD_EXP),
                   (wHeaderSet & IDD_HEAD_FONT)?TRUE:FALSE);

      return  FALSE;

    case  WM_COMMAND:
      /*
          Process button messages.  We only Handle OK, Cancel, and the FONTS
          button.  Everything else's default actions are fine.
      */

      switch(wParam)
      {
        case IDD_HEAD_FONT:

          /*
            Enable/Disable the Expanded/Compressed font info buttons as
            needed.
          */

          EnableWindow(GetDlgItem(hDlg, IDD_HEAD_CON),
                       IsDlgButtonChecked(hDlg, IDD_HEAD_FONT));
          EnableWindow(GetDlgItem(hDlg, IDD_HEAD_EXP),
                       IsDlgButtonChecked(hDlg, IDD_HEAD_FONT));
          return  FALSE;

        case IDOK:

          wHeaderSet=0;
          for (nIdx = 0;
               nIdx <  sizeof(wFlags) / sizeof(wFlags[0]);
               nIdx++)
            if (IsDlgButtonChecked(hDlg,wFlags[nIdx]))
              wHeaderSet |= wFlags[nIdx];

          wHeaderSet &= FLAGMASK;         // Clear High Byte

          wsprintf(acBuffer,"%u",wHeaderSet);
          WritePrivateProfileString("StartUp", "HeaderTest", acBuffer,
                                    strApplicationProfile);

        case IDCANCEL:   /*  Or fall through from OK!    */

          EndDialog(hDlg, TRUE);
          break;

        default:
          return    TRUE;
      }
      break;

    /*------------------------------------*\
    | No message to process.               |
    \*------------------------------------*/
    default:
      return  FALSE;
  }

  return    TRUE;
}


/*---------------------------------------------------------------------------*\
| SETUP PRINT TEST CHARACTERISTICS                                            |
|   This is the dialog box procedure which prompts for the Print Test         |
|   information.  The user has a choice of which tests to execute.  The bit   |
|   is set in the wTestsSet WORD for which tests to run.                      |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HWND     hDlg     - DialogBox window Handle.                              |
|   unsigned iMessage - Message to be processed.                              |
|   WORD     wParam   - Information associated with message.                  |
|   LONG     lParam   - Information associated with message.                  |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   WORD wTestsSet - Each bit represents an option to include or exclude.     |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if message was processed, or FALSE if it was not.  When the   |
|          user hits the OK button, then control goes back to the application.|
|                                                                             |
|   Change History:                                                           |
|   02-04-1991          Altered to pick up ResetDC test.  Also changed it     |
|                       so changes made to the test set are only recorded     |
|                       if "OK" is pushed, instead of as items are checked.   |
\*---------------------------------------------------------------------------*/
BOOL FAR PASCAL SetupTestsDlg(HWND     hDlg,
                              unsigned iMessage,
                              WORD     wParam,
                              LONG     lParam)
{
  int           nIdx;
  char          acBuffer[10];
  static WORD   wFlags[] = {IDD_TEST_TEXT, IDD_TEST_BITMAPS,
                            IDD_TEST_POLYGONS, IDD_TEST_CHARWIDTH,
                            IDD_TEST_CURVES, IDD_TEST_LINES,
                            IDD_TEST_RESETDC, IDD_TEST_ABORTDOC};

  switch    (iMessage)
  {
    /*
      Initialization:

      Check the tests which are currently selected.  If Win 3.0 style is
      selected, deselect the ResetDC test, disable, and hide its checkbox.
    */

    case  WM_INITDIALOG:
      if  (bMetafile || wStyleSelected == 30)
      {
        wTestsSet &= ~(IDD_TEST_RESETDC);
        EnableWindow(GetDlgItem(hDlg, IDD_TEST_RESETDC), FALSE);
        ShowWindow(GetDlgItem(hDlg, IDD_TEST_RESETDC), SW_HIDE);
      }

      for (nIdx=0; nIdx < sizeof(wFlags)/sizeof(WORD); nIdx++)
        SendDlgItemMessage(hDlg, wFlags[nIdx], BM_SETCHECK,
              (wTestsSet & wFlags[nIdx])?TRUE:FALSE, 0L);
      break;

    /*
      Commands:

      If OK is pressed, record the current settings, and write them to
      the application profile.  If Cancel is pressed, just go.  Let Windows
      take care of the rest.
    */

    case WM_COMMAND:
      switch  (wParam)
      {
        case IDOK:
          wTestsSet = 0;

          for (nIdx=0; nIdx < sizeof(wFlags)/sizeof(WORD); nIdx++)
            if  (IsDlgButtonChecked(hDlg, wFlags[nIdx]))
              wTestsSet |= wFlags[nIdx];
            else
              wTestsSet &= ~wFlags[nIdx];

          wTestsSet &= FLAGMASK;              // clear high byte

          wsprintf(acBuffer,"%u",wTestsSet);
          WritePrivateProfileString("StartUp", "TestsTest", acBuffer,
                                    strApplicationProfile);

        case IDCANCEL:
          EndDialog(hDlg,TRUE);
          break;

        default:
          return    FALSE;
      }
      break;

    /*
      Let Windows handle anything else
    */

    default:
      return  FALSE;
  }

  return    TRUE;
}

BOOL FAR PASCAL SetupObjectsDlg(HWND     hDlg,
                                unsigned iMessage,
                                WORD     wParam,
                                LONG     lParam)
{
  HDC              hDC;
  TEXTMETRIC       tm;
  RECT             rRect;
  int              nIdx, y;
  HPEN             hPen, hOldPen;
  HBRUSH           hBrush, hOldBrush, hBkBrush;
  LPDRAWITEMSTRUCT lpdis;
  int              nOldRop;
  static BOOL      bColor;

  switch(iMessage)
  {
    case  WM_MEASUREITEM:
      hDC = GetDC(hDlg);
      GetTextMetrics(hDC,&tm);

      ((LPMEASUREITEMSTRUCT)lParam) -> itemHeight =
            tm.tmHeight+tm.tmExternalLeading;
      ((LPMEASUREITEMSTRUCT)lParam) -> itemWidth = 25;

      ReleaseDC(hDlg,hDC);
      break;

    case WM_DRAWITEM:
      lpdis=(LPDRAWITEMSTRUCT)lParam;

      y = lpdis->rcItem.top + ((lpdis->rcItem.bottom - lpdis->rcItem.top)/2);
      hDC = GetDC(hDlg);
      GetTextMetrics(hDC,&tm);
      ReleaseDC(hDlg,hDC);
      hDC  = lpdis->hDC;
      nIdx = LOWORD(lpdis->itemData);

      /*-------------------------------*\
      |                                 |
      \*-------------------------------*/
      if  (lpdis->itemAction & (ODA_DRAWENTIRE | ODA_SELECT))
      {
        SetBkMode(hDC,TRANSPARENT);
        nOldRop=SetROP2(hDC,((lpdis->itemState&ODS_SELECTED)&&!bColor)?
                              R2_NOT:R2_COPYPEN);

        switch  (lpdis->CtlID)
        {
          case    IDD_OBJT_PENLIST:
            SetCurrentObject(hPens,nIdx);
            hPen = CreateDeviceObject(hPens);
            hOldPen = SelectObject(hDC,hPen);
            CopyRect(&rRect, &(lpdis->rcItem));

            hBkBrush=CreateSolidBrush((lpdis->itemState&ODS_SELECTED)?
                                      RGB_SELECT:RGB_DESELECT);
            FillRect(hDC,&rRect,hBkBrush);
            DeleteObject(hBkBrush);

            InflateRect(&rRect, -(2*tm.tmAveCharWidth),-2);
            MoveTo(hDC,tm.tmAveCharWidth*2,y);
            LineTo(hDC,lpdis->rcItem.right - (tm.tmAveCharWidth*2),y);
            DeleteObject(SelectObject(hDC,hOldPen));
            break;

          case    IDD_OBJT_BRSHLIST:
            SetCurrentObject(hBrushes,nIdx);
            hBrush = CreateDeviceObject(hBrushes);
            hOldBrush = SelectObject(hDC,hBrush);
            CopyRect(&rRect, &(lpdis->rcItem));

            hBkBrush=CreateSolidBrush((lpdis->itemState&ODS_SELECTED)?
                                      RGB_SELECT:RGB_DESELECT);
            FillRect(hDC,&rRect,hBkBrush);
            DeleteObject(hBkBrush);

            InflateRect(&rRect, -(2 * tm.tmAveCharWidth), -2);
            Rectangle(hDC,rRect.left,rRect.top,rRect.right,rRect.bottom);
            DeleteObject(SelectObject(hDC,hOldBrush));
            break;
        }

        SetBkMode(hDC,OPAQUE);
        SetROP2(hDC,nOldRop);
      }

      if (((LPDRAWITEMSTRUCT)lParam)->itemAction & ODA_FOCUS)
        DrawFocusRect(hDC, &((LPDRAWITEMSTRUCT)lParam)->rcItem);
      break;

    case  WM_INITDIALOG:
      {
        char  acBuffer[128];

        wsprintf(acBuffer, "%s on %s",(LPSTR)pPrinter.szName,
                (LPSTR)pPrinter.szPort);
        SetWindowText(hDlg,acBuffer);

        wsprintf(acBuffer, "%d &Pens", GetObjectCount(hPens));
        SetDlgItemText(hDlg, IDD_OBJT_PENTEXT, acBuffer);

        wsprintf(acBuffer, "%d &Brushes", GetObjectCount(hBrushes));
        SetDlgItemText(hDlg, IDD_OBJT_BRUSHTXT, acBuffer);

        wsprintf(acBuffer, "%d &Fonts", GetObjectCount(hFonts));
        SetDlgItemText(hDlg, IDD_OBJT_FONTTEXT, acBuffer);
      }

      for (nIdx=0; nIdx < GetObjectCount(hPens); nIdx++)
        SendDlgItemMessage(hDlg, IDD_OBJT_PENLIST, LB_ADDSTRING, NULL,
                           MAKELONG(nIdx, 0));

      for (nIdx=0; nIdx < GetObjectCount(hBrushes); nIdx++)
        SendDlgItemMessage(hDlg, IDD_OBJT_BRSHLIST, LB_ADDSTRING, NULL,
                           MAKELONG(nIdx, 0));

      InitObjectsFontList(hDlg);

      if (bMetafile || wStyleSelected == 31 ||
         !(dcDevCaps.wRasterCaps & RC_BANDING))
      {
        EnableWindow(GetDlgItem(hDlg, IDD_OBJT_BANDING), FALSE);
        ShowWindow(GetDlgItem(hDlg, IDD_OBJT_BANDING), SW_HIDE);
      }
      else
        CheckDlgButton(hDlg, IDD_OBJT_BANDING, bUseBanding);

      KillTest(&tlTest);
      InitTest("\0", hInst, &tlTest);

      bColor=TRUE;
      hDC=GetDC(hDlg);
      if(RGB_SELECT != GetNearestColor(hDC,RGB_SELECT))
        bColor=FALSE;
      ReleaseDC(hDlg,hDC);
      break;

    case  WM_COMMAND:

      switch(wParam)
      {
        case IDOK:
          for   (nIdx=0; nIdx < GetObjectCount(hPens); nIdx++)
          {
            if(SendDlgItemMessage(hDlg,IDD_OBJT_PENLIST,LB_GETSEL,nIdx,0L))
              AddObject(tlTest.gtTest.hPens, (LPSTR)&nIdx);
          }

          for   (nIdx=0; nIdx < GetObjectCount(hBrushes); nIdx++)
          {
            if(SendDlgItemMessage(hDlg,IDD_OBJT_BRSHLIST,LB_GETSEL,nIdx,0L))
              AddObject(tlTest.gtTest.hBrushes, (LPSTR)&nIdx);
          }

          for (nIdx=0; nIdx < GetObjectCount(hFonts); nIdx++)
          {
            if (SendDlgItemMessage(hDlg,IDD_OBJT_FONTLIST,LB_GETSEL,nIdx,0L))
            {
              int nFontIndex;

              nFontIndex=(int)SendDlgItemMessage(hDlg,IDD_OBJT_FONTLIST,
                                                 LB_GETITEMDATA,nIdx,NULL);
              AddObject(tlTest.gtTest.hFonts,(LPSTR)&nFontIndex);
            }
          }

          bUseBanding = !bMetafile && wStyleSelected == 30   &&
                        (dcDevCaps.wRasterCaps & RC_BANDING) &&
                        IsDlgButtonChecked(hDlg, IDD_OBJT_BANDING);

          EndDialog(hDlg,TRUE);
          break;

        case IDCANCEL:
          KillTest(&tlTest);
          EndDialog(hDlg, FALSE);
          break;

        case IDD_OBJT_PENALL:
        case IDD_OBJT_BRSHALL:
        case IDD_OBJT_FONTALL:
          SendDlgItemMessage(hDlg, wParam - 3, LB_SETSEL,
                             IsDlgButtonChecked(hDlg,wParam), -1L);
          break;

        default:
          return FALSE;
      }
      break;

    default:
      return FALSE;
  }
  return TRUE;
}


BOOL InitObjectsFontList(HWND hDlg)
{
  int    nIdx;
  char   szBuffer[80];
  FONT   fFont;

  for(nIdx=0; nIdx < GetObjectCount(hFonts); nIdx++)
  {
    SetCurrentObject(hFonts,nIdx);
    CopyDeviceObject((LPSTR)&fFont,hFonts);

    GetFontName(&fFont.lf,szBuffer);

    SendDlgItemMessage(hDlg,IDD_OBJT_FONTLIST,LB_SETITEMDATA,
                      (WORD)SendDlgItemMessage(hDlg,IDD_OBJT_FONTLIST,
                                               LB_ADDSTRING,NULL,
                                              (LONG)(LPSTR)szBuffer),
                       MAKELONG(nIdx,NULL));
  }
  return(TRUE);
}

/******************************************************************************

    Public Dialog Function: SelectStyleDlg

    Purpose:

    This dialog function allows the user to manipulate the printer using
    Escape calls (as was done prior to Win 3.0) or the new Win 3.1 GDI calls.
    This affects the operation of the Test Selection dialog above, in that
    banding is only supported if Escape calls are used, whereas RestDC is
    supported only if the new calls are.

  CALLED ROUTINES
    -none-

  PARAMETERS
    HWND     hDlg     - DialogBox window Handle.
    unsigned iMessage - Message to be processed.
    WORD     wParam   - Information associated with message.
    LONG     lParam   - Information associated with message.

  GLOBAL VARIABLES
    WORD     wStyleSet- Set to windows printing style to emulate (30=3.0)

  RETURNS
    BOOL - TRUE for WM_COMMAND messages, FALSE for the others.  Control
           returns to the caller when the OK or Cancel buttons get pushed.
           These correspond to Enter and Esc.

    Change History:

    02-01-1991  Coded it
    02-12-1991  Added Metafile controls

******************************************************************************/
BOOL FAR PASCAL SelectStyleDlg(HWND     hwndDialog,
                               WORD     wMsg,
                               WORD     wParam,
                               LONG     lParam)
{
  switch    (wMsg)
  {
    /*
    Initialization:

    Set the radio button corresponding to the Style read from the
    application profile.  Give it the focus, and we be done.
    */

    case WM_INITDIALOG:
    {
      if(0x0003 == LOWORD(GetVersion()))     // Disable Win31 option
      {
        HWND hWin31;

        hWin31=GetDlgItem(hwndDialog,IDD_STYLE_WIN31);
        EnableWindow(hWin31,FALSE);
        ShowWindow(hWin31,SW_HIDE);
        wStyleSelected=30;
      }

      CheckDlgButton(hwndDialog, IDD_STYLE_WIN30, wStyleSelected == 30);
      CheckDlgButton(hwndDialog, IDD_STYLE_WIN31, wStyleSelected == 31);
      CheckDlgButton(hwndDialog, IDD_STYLE_METAFILE, bMetafile);

      SetFocus(GetDlgItem(hwndDialog, wStyleSelected == 31 ?
               IDD_STYLE_WIN31 : IDD_STYLE_WIN30));

      return  FALSE;
    }

    /*
      Mouse and Keyboard Commands:

      If the OK button is pressed, note which style was selected.  If
      Cancel was pressed, just quit.  Otherwise, let Windows handle it.
    */

    case WM_COMMAND:
      switch  (wParam)
      {
        char    acBuffer[10];

        /*
            Please note that we let the IDOK case fall through into the
            IDCANCEL code, thus terminating the dialog.
        */

        case IDOK:
          wStyleSelected = IsDlgButtonChecked(hwndDialog, IDD_STYLE_WIN31) ?
                              31 : IsDlgButtonChecked(hwndDialog,
                              IDD_STYLE_WIN30) ? 30 : wStyleSelected;

          wsprintf(acBuffer, "%u", wStyleSelected);
          WritePrivateProfileString("StartUp", "Print Style", acBuffer,
                                    strApplicationProfile);

          bMetafile = IsDlgButtonChecked(hwndDialog, IDD_STYLE_METAFILE);
          wsprintf(acBuffer, "%u", bMetafile);
          WritePrivateProfileString("StartUp", "Use Metafiles", acBuffer,
                                    strApplicationProfile);


        case IDCANCEL:
          EndDialog(hwndDialog,TRUE);
          return    TRUE;

        default:
          return    FALSE;
      }
      break;

    /*
      Anything else we let Windows handle- it's doing just fine!
    */

    default:
      return  FALSE;
  }
}



/******************************************************************************

    Public Dialog Function: FontOptionsDlg

    Purpose:

    This dialog allows the user to specify device fonts only as an
    option.  This could easily be enhanced to include options concerning
    true type fonts, gdi fonts, etc.

  CALLED ROUTINES
    -none-

  PARAMETERS
    HWND     hDlg     - DialogBox window Handle.
    WORD     iMessage - Message to be processed.
    WORD     wParam   - Information associated with message.
    LONG     lParam   - Information associated with message.

  GLOBAL VARIABLES
    BOOL     bDeviceFontsOnly - if TRUE, use device fonts only

  RETURNS
    BOOL - TRUE for WM_COMMAND messages, FALSE for the others.  Control
           returns to the caller when the OK or Cancel buttons get pushed.
           These correspond to Enter and Esc.

    Change History:

    08-27-1991  Adapted from SelectStyleDlg

******************************************************************************/
BOOL FAR PASCAL FontOptionsDlg(HWND     hwndDialog,
                               WORD     wMsg,
                               WORD     wParam,
                               LONG     lParam)
{
  switch    (wMsg)
  {
    /*
    Initialization:

    Set the checkbox to the current state.  Get the explanation and put
    it into the text window.
    */

    case WM_INITDIALOG:
      SendDlgItemMessage(hwndDialog,IDD_TT_FONTS,BM_SETCHECK,
                        (wFontOptions&TT_FONTS),NULL);

      SendDlgItemMessage(hwndDialog,IDD_DEV_FONTS,BM_SETCHECK,
                        (wFontOptions&NON_TT_DEVICE_FONTS),NULL);

      SendDlgItemMessage(hwndDialog,IDD_NON_DEV_FONTS,BM_SETCHECK,
                        (wFontOptions&NON_TT_NON_DEVICE_FONTS),NULL);

      return  FALSE;


    /*
      Mouse and Keyboard Commands:

      If the OK button is pressed, note which style was selected.  If
      Cancel was pressed, just quit.  Otherwise, let Windows handle it.
    */

    case WM_COMMAND:
      switch  (wParam)
      {
        /*
            Please note that we let the IDOK case fall through into the
            IDCANCEL code, thus terminating the dialog.
        */

        case IDOK:
        {
          char    acBuffer[10];

          // Get the font options and save them.
          wFontOptions=0;

          if(IsDlgButtonChecked(hwndDialog,IDD_TT_FONTS))
            wFontOptions |= TT_FONTS;
          if(IsDlgButtonChecked(hwndDialog,IDD_DEV_FONTS))
            wFontOptions |= NON_TT_DEVICE_FONTS;
          if(IsDlgButtonChecked(hwndDialog,IDD_NON_DEV_FONTS))
            wFontOptions |= NON_TT_NON_DEVICE_FONTS;

          wsprintf(acBuffer, "%u", wFontOptions);
          WritePrivateProfileString("StartUp", "Font Options", acBuffer,
                                    strApplicationProfile);
        }


        case IDCANCEL:
          EndDialog(hwndDialog,TRUE);
          return    TRUE;

        default:
          return    FALSE;
      }
      break;

    /*
      Anything else we let Windows handle- it's doing just fine!
    */

    default:
      return  FALSE;
  }
}
