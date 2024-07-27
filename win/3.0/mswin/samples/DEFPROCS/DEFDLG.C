
/*--------------------------------------------------------------------------*/
/*									    */
/*  DefDlgProc() -							    */
/*									    */
/*--------------------------------------------------------------------------*/

LONG FAR PASCAL DefDlgProc(hwnd, message, wParam, lParam)

register HWND hwnd;
register WORD message;
WORD	      wParam;
LONG	      lParam;

{
  HWND	      hwndT1;
  HWND	      hwndT2;
  int	      result;

  if (!CheckHwnd(hwnd))
      return(NULL);

  ((PDLG)hwnd)->resultWP = 0L;

  if (((PDLG)hwnd)->lpfnDlg == NULL || 
      !(result = (*((PDLG)hwnd)->lpfnDlg)(hwnd, message, wParam, lParam)))
    {
      switch (message)
	{
	  case WM_ERASEBKGND:
	      FillWindow(hwnd, hwnd, (HDC)wParam, (HBRUSH)CTLCOLOR_DLG);
	      return((LONG)TRUE);

	  case WM_SHOWWINDOW:
	      /* If hiding the window, save the focus. If showing the window
	       * by means of a SW_* command and the fEnd bit is set, do not
	       * pass to DWP so it won't get shown.
	       */
	      if (!wParam && ((PDLG)hwnd)->hwndFocusSave == NULL)
		  ((PDLG)hwnd)->hwndFocusSave = hwndFocus;
	      else if (LOWORD(lParam) != 0 && ((PDLG)hwnd)->fEnd)
		  break;
	      goto CallDWP;

	  case WM_ACTIVATE:
	      fDialog = FALSE;
	      if (wParam)
		{
		  fDialog = TRUE;
		  if (((PDLG)hwnd)->hwndFocusSave)
		    {
		      SetFocus(((PDLG)hwnd)->hwndFocusSave);

		      /* Set to NULL so we don't reset if we get more than
			 one activate message. */
		      ((PDLG)hwnd)->hwndFocusSave = NULL;
		    }
		}
	      else if (hwndFocus && IsChild(hwnd, hwndFocus) &&
		       ((PDLG)hwnd)->hwndFocusSave == NULL)
		{
		  /* Must remember focus if deactivated */
		  ((PDLG)hwnd)->hwndFocusSave = hwndFocus;
		}
	      break;

	  case WM_SETFOCUS:
              if (!((PDLG)hwnd)->fEnd)
                  /* Don't set the focus if we are ending this dialog box 
		   */
  	          DlgSetFocus(GetFirstTab(hwnd));
	      break;

	  case WM_CLOSE:
	      /* Make sure cancel button is not disabled before sending the
	       * IDCANCEL.  Note that we need to do this as a message instead
	       * of directly calling the dlg proc so that any dialog box
	       * filters get this. 
	       */
	      hwndT1 = GetDlgItem(hwnd, IDCANCEL);
	      if (hwndT1 && TestWF(hwndT1, WFDISABLED))
		  MessageBeep(0);
	      else
		  PostMessage(hwnd, WM_COMMAND, IDCANCEL, 0L);
	      break;

	  case WM_NCDESTROY:
              fDialog = FALSE;      /* clear this flag */
	      if (!(hwnd->style & DS_LOCALEDIT))
		{
		  if (((PDLG)hwnd)->hData)
		    {
		      GlobalUnWire(((PDLG)hwnd)->hData);
		      ReleaseEditDS(((PDLG)hwnd)->hData);
		    }
		}
	      /* Delete the user defined font if any */
	      if (((PDLG)hwnd)->hUserFont)
		  DeleteObject(((PDLG)hwnd)->hUserFont);

	      /* Gotta let DefWindowProc do its thing here or we won't
	       * get all of the little chunks of memory freed for this
	       * window (szName and rgwScroll).
	       */
	      DefWindowProc(hwnd, message, wParam, lParam);
	      break;

	  case DM_SETDEFID:
              if (!(((PDLG)hwnd)->fEnd) && (((PDLG)hwnd)->result != wParam))
                {
                  /* Make sure that the new default button has the highlight.
		   * We need to blow this off if we are ending the dialog box
		   * because hwnd->result is no longer a default window id but
		   * rather the return value of the dialog box.  
		   */
                  /* Catch the case of setting the defid to null or setting
		   * the defid to something else when it was initially null. 
		   */
                  CheckDefPushButton(hwnd, 
                       (((PDLG)hwnd)->result ? 
                                     GetDlgItem(hwnd, ((PDLG)hwnd)->result) :
                                     NULL),
                       (wParam ? GetDlgItem(hwnd, wParam) : NULL));
   	          ((PDLG)hwnd)->result = wParam;
                }
	      return(TRUE);

	  case DM_GETDEFID:
	      return(MAKELONG(((PDLG)hwnd)->result, DC_HASDEFID));

	  /* This message was added so that user defined controls that want 
	   * tab keys can pass the tab off to the next/previous control in the
	   * dialog box.  Without this, all they could do was set the focus
	   * which didn't do the default button stuff.
	   */
	  case WM_NEXTDLGCTL:
	      hwndT2 = hwndFocus;
	      if (LOWORD(lParam))
		{
		  if (hwndT2 == NULL)
		      hwndT2 = hwnd;

		  /* wParam contains the hwnd of the ctl to set focus to. */
		  hwndT1 = (HWND)wParam;
		}
	      else
		{
		  if (hwndT2 == NULL)
		    {
		      /* Set focus to the first tab item. */
		      hwndT1 = GetFirstTab(hwnd);
		      hwndT2 = hwnd;
		    }
		  else
		    {
		      /* If window with focus not a dlg ctl, ignore message. */
		      if (!IsChild(hwnd, hwndT2))
			  return(TRUE);

		      /* wParam = TRUE for previous, FALSE for next */
		      hwndT1 = GetNextDlgTabItem(hwnd, hwndT2, wParam);
		    }
		}

	      DlgSetFocus(hwndT1);
	      CheckDefPushButton(hwnd, hwndT2, hwndT1);
	      return(TRUE);

	  case WM_LBUTTONDOWN:
	  case WM_NCLBUTTONDOWN:
	      hwndT1 = hwndFocus;
	      if (hwndT1->pcls->lpfnWndProc == ComboBoxCtlWndProc)
		  /* If user clicks anywhere in dialog box and a combo box (or
		   * the editcontrol of a combo box) has the focus, then hide
		   * it's listbox. 
		   */		      
		  SendMessage(hwndT1,CB_SHOWDROPDOWN, FALSE, 0L);
	      else
	      if (hwndT1->pcls->lpfnWndProc == EditWndProc &&
		  hwndT1->hwndParent->pcls->lpfnWndProc==ComboBoxCtlWndProc)
		{
		  SendMessage(hwndT1->hwndParent,CB_SHOWDROPDOWN, FALSE, 0L);
		}
	      /* Always send the message off to DefWndProc */
	      goto CallDWP;

	  case WM_GETFONT:
	      return(((PDLG)hwnd)->hUserFont);

	  case WM_VKEYTOITEM:
	  case WM_COMPAREITEM:
	  case WM_CHARTOITEM:
	      /* We need to return the 0 the app may have returned for these
	       * items instead of calling defwindow proc. 
	       */
	      return(result);

	  default:
CallDWP:
	      return(DefWindowProc(hwnd, message, wParam, lParam));
	}
    }

  /* Must return brush which apps dlgfn returns. */
  if (message == WM_CTLCOLOR ||
      message == WM_COMPAREITEM ||
      message == WM_VKEYTOITEM ||
      message == WM_CHARTOITEM)
      return(result);

  return(((PDLG)hwnd)->resultWP);
}
