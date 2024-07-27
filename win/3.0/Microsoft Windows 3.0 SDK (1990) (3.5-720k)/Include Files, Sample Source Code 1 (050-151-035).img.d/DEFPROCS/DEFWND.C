/*--------------------------------------------------------------------------*/
/*									    */
/*  DefWindowProc() -							    */
/*									    */
/*--------------------------------------------------------------------------*/

LONG FAR PASCAL DefWindowProc(hwnd, message, wParam, lParam)

register HWND hwnd;
WORD	      message;
register WORD wParam;
LONG	      lParam;

{
  int	      i;
  HDC	      hdc;
  PAINTSTRUCT ps;
  HICON	      hIcon;
  RECT	      rc;
  HANDLE      hCurs;
  HBRUSH      hbr;
  HWND	      hwndT;

  if (!CheckHwnd(hwnd))
      return((DWORD)FALSE);

  switch (message)
    {
      case WM_NCACTIVATE:
	  if (wParam != 0)
	      SetWF(hwnd, WFFRAMEON);
	  else
	      ClrWF(hwnd, WFFRAMEON);

	  if (TestWF(hwnd, WFVISIBLE) && !TestWF(hwnd, WFNONCPAINT))
	    {
	      hdc = GetWindowDC(hwnd);
	      DrawCaption(hwnd, hdc, TRUE, TestWF(hwnd, WFFRAMEON));
	      InternalReleaseDC(hdc);
	      if (TestWF(hwnd,WFMINIMIZED))
		  RedrawIconTitle(hwnd);
	    }
	  return(TRUE);

      case WM_NCHITTEST:
	  return(FindNCHit(hwnd, lParam));

      case WM_NCCALCSIZE:
	  CalcClientRect(hwnd, (LPRECT)lParam);
	  break;

      case WM_NCLBUTTONDOWN:
	{
	  WORD	     cmd;
	  RECT	     rcWindow;
	  RECT	     rcCapt;
	  RECT	     rcInvert;
	  RECT	     rcWindowSave;

	  cmd = 0;

	  switch(wParam)
	    {
	      case HTZOOM:
	      case HTREDUCE:
		  GetWindowRect(hwnd, (LPRECT)&rcWindow);
		  CopyRect((LPRECT)&rcWindowSave, (LPRECT)&rcWindow);

		  if (TestWF(hwnd, WFSIZEBOX))
		      InflateRect((LPRECT)&rcWindow, 
			          -cxSzBorderPlus1, -cySzBorderPlus1);
		  else
		      InflateRect((LPRECT)&rcWindow, -cxBorder, -cyBorder);

		  rcCapt.right = rcWindow.right + cxBorder;
                  rcCapt.left = rcWindow.right - oemInfo.bmReduce.cx-cxBorder;

		  if (wParam == HTREDUCE)
		      cmd = SC_MINIMIZE;
		  else if (TestWF(hwnd, WFMAXIMIZED))
		      cmd = SC_RESTORE;
		  else
		      cmd = SC_MAXIMIZE;

		  if (wParam == HTREDUCE && TestWF(hwnd, WFMAXBOX))
                      OffsetRect((LPRECT)&rcCapt, -oemInfo.bmReduce.cx, 0);

		  rcCapt.top = rcWindow.top;
		  rcCapt.bottom = rcCapt.top + cyCaption;

		  CopyRect((LPRECT)&rcInvert, (LPRECT)&rcCapt);
                  InflateRect((LPRECT)&rcInvert, -cxBorder, -cyBorder);

                  rcInvert.right += cxBorder;
		  rcInvert.left += cxBorder;

		  /* Converting to window coordinates. */
		  OffsetRect((LPRECT)&rcInvert, 
			     -(rcWindowSave.left + cxBorder),
			     -(rcWindowSave.top + cyBorder));
		  /* Wait for the BUTTONUP message and see if cursor is still
		   * in the Minimize or Maximize box.
		   *
		   * NOTE: rcInvert is in window coords, rcCapt is in screen 
		   * coords
		   */
                  if (!DepressTitleButton(hwnd, rcCapt, rcInvert, wParam))
		      cmd = 0;

		  break;

	      default:
		  if (wParam >= HTSIZEFIRST && wParam <= HTSIZELAST)
		      /* Change HT into a MV command. */
		      cmd = SC_SIZE + (wParam - HTSIZEFIRST + MVSIZEFIRST);
	    }

	  if (cmd != 0)
	    {
	      /* For SysCommands on system menu, don't do if
	       * menu item is disabled.
	       */
	      if (TestWF(hwnd, WFSYSMENU))
		{
                  /* don't check old app child windows
		   */
		  if (LOWORD(GetExpWinVer(hwnd->hInstance)) >= VER || 
		      !TestwndChild(hwnd))
		    {
		      SetSysMenu(hwnd);
		      if (GetMenuState(GetSysMenuHandle(hwnd), cmd & 0xFFF0,
				MF_BYCOMMAND) & (MF_DISABLED | MF_GRAYED))
			  break;
		    }
		}
	      SendMessage(hwnd, WM_SYSCOMMAND, cmd, lParam);
	      break;
	    }
	  /*** FALL THRU ***/
	}

      case WM_NCMOUSEMOVE:
      case WM_NCLBUTTONUP:
      case WM_NCLBUTTONDBLCLK:
	  HandleNCMouseGuys(hwnd, message, wParam, lParam);
	  break;

      case WM_CANCELMODE:
	  if (hwndCapture == hwnd && pfnSB != NULL)
	      EndScroll(hwnd, TRUE);

          if (fMenu && hwndMenu == hwnd)
              EndMenu();

	  /* If the capture is still set, just release at this point.
	   * Can put other End* functions in later.
	   */
	  if (hwnd == hwndCapture)
	      ReleaseCapture();
	  break;

      case WM_NCCREATE:
	  if (TestWF(hwnd, (WFHSCROLL | WFVSCROLL)))
	      if (InitPwSB(hwnd) == NULL)
		  return((LONG)FALSE);

	  return((LONG)DefSetText(hwnd, ((LPCREATESTRUCT)lParam)->lpszName));

      case WM_NCDESTROY:
	  if (hwnd->hName)
	      hwnd->hName = TextFree(hwnd->hName);
	  break;

      case WM_NCPAINT:
	  /* Force the drawing of the menu. */
	  SetWF(hwnd, WFMENUDRAW);
	  DrawWindowFrame(hwnd, (HRGN)wParam);
	  ClrWF(hwnd, WFMENUDRAW);
	  break;

      case WM_SETTEXT:
	  DefSetText(hwnd, (LPSTR)lParam);
	  if (TestWF(hwnd, WFVISIBLE))
            {
	      if (TestWF(hwnd,WFMINIMIZED))
		{
		  ShowIconTitle(hwnd,FALSE);
		  ShowIconTitle(hwnd,TRUE);
		}
	      else if (TestWF(hwnd, WFBORDERMASK) == (BYTE)LOBYTE(WFCAPTION))
		{
		  hdc = GetWindowDC(hwnd);
		  DrawCaption(hwnd, hdc, FALSE, TestWF(hwnd, WFFRAMEON));
		  InternalReleaseDC(hdc);
		}
            }
	  break;

      case WM_GETTEXT:
	  if (wParam)
            {
              if (hwnd->hName)
	          return (DWORD)TextCopy(hwnd->hName,(LPSTR)lParam,wParam);

              /* else Null terminate the text buffer since there is no text.
	       */
	      ((LPSTR)lParam)[0] = NULL;
            }
	  return (0L);


      case WM_GETTEXTLENGTH:
	  if (hwnd->hName)
	      return(lstrlen(TextPointer(hwnd->hName)));

          /* else */
          return(0L);

      case WM_CLOSE:
	  DestroyWindow(hwnd);
	  break;

      case WM_PAINT:
	  BeginPaint(hwnd, (LPPAINTSTRUCT)&ps);
	  EndPaint(hwnd, (LPPAINTSTRUCT)&ps);
	  break;

      case WM_PAINTICON:
          /* Draw the icon through the window DC if app used own DC. If own DC
	   * is used the mapping mode may not be MM_TEXT.  
	   */
          BeginPaint(hwnd, (LPPAINTSTRUCT)&ps);
          if (TestCF(hwnd, CFOWNDC) || TestCF(hwnd, CFCLASSDC))
            {
              /* If owndc, do the end paint now so that the
	       * erasebackgrounds/validate regions go through properly. Then
	       * we get a clean window dc to draw the icon into. 
	       */
              InternalEndPaint(hwnd, (LPPAINTSTRUCT)&ps, TRUE);
              hdc = GetWindowDC(hwnd);
            }
          else
            {
              hdc = ps.hdc;
            }

	  /* wParam is TRUE to draw icon, FALSE to ignore paint. */
	  if (wParam)
	    {
              hIcon = (HICON)(PCLS)(hwnd->pcls)->hIcon;
              GetClientRect(hwnd, (LPRECT)&rc);

              rc.left = (rc.right - rgwSysMet[SM_CXICON]) >> 1;
	      rc.top = (rc.bottom - rgwSysMet[SM_CYICON]) >> 1;

	      DrawIcon(hdc, rc.left, rc.top, hIcon);
	    }

	  /* Delete the update region. */
          if (TestCF(hwnd, CFOWNDC) || TestCF(hwnd, CFCLASSDC))
            {
              InternalReleaseDC(hdc);
              /* ValidateRect(hwnd, NULL); */
            }
          else
              InternalEndPaint(hwnd, (LPPAINTSTRUCT)&ps, TRUE);
	  break;

      case WM_ICONERASEBKGND:
          /* Erase the icon through the window DC if app used own DC. If own
	   * DC is used the mapping mode may not be MM_TEXT. 
	   */
          if (TestCF(hwnd, CFOWNDC) || TestCF(hwnd, CFCLASSDC))
  	      hdc = GetWindowDC(hwnd);
          else
   	      hdc = (HDC)wParam;

          if (TestWF(hwnd, WFCHILD))    /* for MDI child icons */
            {
              if ((hbr = GetBackBrush(hwnd->hwndParent)) == NULL)
		{
                  /* No brush, punt. */
                  goto AbortIconEraseBkGnd;
                }
              else
                  goto ICantBelieveIUsedAGoToStatement;
	    }

	  if (hbmWallpaper)
            {
              /* Since desktop bitmaps are done on a wm_paint message (and not
	       * erasebkgnd), we need to call the paint proc with our dc 
	       */
              PaintDesktop(hdc);
              /* SendMessage(hwndDesktop, WM_ERASEBKGND, hdc, 0L);*/
            }
	  else
	    {
	      hbr = sysClrObjects.hbrDesktop;
ICantBelieveIUsedAGoToStatement:
	      FillWindow(hwnd->hwndParent,hwnd,hdc,hbr);
	    }

AbortIconEraseBkGnd:
          if (TestCF(hwnd, CFOWNDC) || TestCF(hwnd, CFCLASSDC))
  	      InternalReleaseDC(hdc);

	  return((LONG)TRUE);

      case WM_ERASEBKGND:
	  if ((hbr = GetBackBrush(hwnd)) != NULL)
	    {
	      FillWindow(hwnd, hwnd, (HDC)wParam, hbr);
	      return((LONG)TRUE);
	    }
	  break;

      case WM_QUERYOPEN:
      case WM_QUERYENDSESSION:
	  return((LONG)TRUE);

      case WM_SYSCOMMAND:
	  SysCommand(hwnd, wParam, lParam);
	  break;

      case WM_KEYDOWN:
	  if (wParam == VK_F10)
	      fF10Status = TRUE;
	  break;

      case WM_SYSKEYDOWN:
	  /* Is the ALT key down? */
	  if (HIWORD(lParam) & SYS_ALTERNATE)
	    {
	      /* Toggle the fMenuStatus iff this is NOT a repeat KEYDOWN
	       * message; Only if the prev key state was 0, then this is the
	       * first KEYDOWN message and then we consider toggling menu 
               * status;
	       */
	      if((HIWORD(lParam) & SYS_PREVKEYSTATE) == 0)
	        {
	          /* Don't have to lock hwndActive because it's processing this 
		     key. */
	          if ((wParam == VK_MENU) && (!fMenuStatus))
		      fMenuStatus = TRUE;
	          else
		      fMenuStatus = FALSE;
		}

	      fF10Status = FALSE;

	      DWP_ProcessVirtKey(wParam);
	    }
	  else
	    {
	      if (wParam == VK_F10)
		  fF10Status = TRUE;
	      else
		{
		  if (wParam == VK_ESCAPE)
		    {
		      if(GetKeyState(VK_SHIFT) < 0)
		           SendMessage(hwnd, WM_SYSCOMMAND, 
				       SC_KEYMENU, (DWORD)MENUSYSMENU);
		    }
		}
	    }
	  break;

      case WM_KEYUP:
      case WM_SYSKEYUP:
	  /* press and release F10 or ALT.Send this only to top-level windows,
	   * otherwise MDI gets confused.  The fix in which DefMDIChildProc()
	   * passed up the message was insufficient in the case a child window
	   * of the MDI child had the focus.
	   */
	  if ((wParam == VK_MENU && (fMenuStatus == TRUE)) ||
	      (wParam == VK_F10 && fF10Status) )
	      SendMessage(GetTopLevelWindow(hwnd), WM_SYSCOMMAND, SC_KEYMENU,
		  (DWORD)0);

	  fF10Status = fMenuStatus = FALSE;
	  break;

      case WM_SYSCHAR:
	  /* If syskey is down and we have a char... */
	  fMenuStatus = FALSE;
	  if ((HIWORD(lParam) & SYS_ALTERNATE) && wParam)
	    {
	      if (wParam == VK_TAB || wParam == VK_ESCAPE)
		  break;

	      /* Send ALT-SPACE only to top-level windows. */
	      if ((wParam == MENUSYSMENU) && (TestwndChild(hwnd)))
		  SendMessage(hwnd->hwndParent, message, wParam, lParam);
	      else
		  SendMessage(hwnd, WM_SYSCOMMAND, SC_KEYMENU, (DWORD)wParam);
	    }
	  else
	      /* Ctrl-Esc produces a WM_SYSCHAR, But should not beep; */
	      if(wParam != VK_ESCAPE)
	          MessageBeep(0);
	  break;

      case WM_CHARTOITEM:
      case WM_VKEYTOITEM:
          /* Do default processing for keystrokes into owner draw 
             listboxes. */
          return(-1);


      case WM_ACTIVATE:
	  if (wParam)
	      SetFocus(hwnd);
	  break;

      case WM_SETREDRAW:
	  DWP_SetRedraw(hwnd, wParam);
	  break;

      case WM_SHOWWINDOW:
	  /* Non null descriptor implies popup hide or show. */
	  /* We should check whether it is a popup window or Owned window */
	  if (LOWORD(lParam) != 0 && (TestwndPopup(hwnd) || hwnd -> hwndOwner))
	    {
	      /* IF NOT(showing, invisible, and not set as hidden) AND
	       *   NOT(hiding and not visible)
	       */
	      if (!(wParam != 0 && !TestWF(hwnd, WFVISIBLE) && 
		  !TestWF(hwnd, WFHIDDENPOPUP)) &&
		  !(wParam == 0 && !TestWF(hwnd, WFVISIBLE)))
		{
		  /* Are we showing? */
		  if (wParam)
		      /* Yes, clear the hidden popup flag. */
		      ClrWF(hwnd, WFHIDDENPOPUP);
		  else
		      /* No, Set the hidden popup flag. */
		      SetWF(hwnd, WFHIDDENPOPUP);

		  ShowWindow(hwnd, 
			     (wParam ? SHOW_OPENNOACTIVATE : HIDE_WINDOW));
		}
	    }
	  break;

      case WM_CTLCOLOR:
	  if (HIWORD(lParam) != CTLCOLOR_SCROLLBAR)
	    {
	      SetBkColor((HDC)wParam, sysColors.clrWindow);
	      SetTextColor((HDC)wParam, sysColors.clrWindowText);
	      hbr = sysClrObjects.hbrWindow;
	    }
	  else
	    {
              SetBkColor((HDC)wParam, 0x00ffffff);
              SetTextColor((HDC)wParam, (LONG)0x00000000);
	      hbr = sysClrObjects.hbrScrollbar;
	      UnrealizeObject(hbr);
	    }

	  return((DWORD)hbr);

      case WM_SETCURSOR:
	  /* wParam  == hwnd that cursor is over
	   * lParamL == Hit test area code (result of WM_NCHITTEST)
	   * lParamH == Mouse message number
	   */
	  if (HIWORD(lParam) != 0 &&
	      LOWORD(lParam) >= HTSIZEFIRST &&
	      LOWORD(lParam) <= HTSIZELAST)
	    {
	      SetCursor(rghCursor[LOWORD(lParam) - HTSIZEFIRST + MVSIZEFIRST]);
	      break;
	    }

	  if ((hwndT = GetChildParent(hwnd)) != NULL &&
		       (BOOL)SendMessage(hwndT, WM_SETCURSOR, wParam, lParam))
	      return((LONG)TRUE);

	  if (HIWORD(lParam) == 0)
	    {
	      hCurs = hCursNormal;
	      SetCursor(hCurs);
	    }
	  else
	    {
	      switch (LOWORD(lParam))
		{
		  case HTCLIENT:
		      if (((HWND)wParam)->pcls->hCursor != NULL)
			  SetCursor(((HWND)wParam)->pcls->hCursor);
		      break;

		  case HTERROR:
		      switch (HIWORD(lParam))
			{
			  case WM_LBUTTONDOWN:
			      if ((hwndT = DWP_GetEnabledPopup(hwnd)) != NULL)
				{
				  if (hwndT != hwndDesktop->hwndChild)
				    {
				      SetWindowPos(hwnd, NULL,
						   0, 0, 0, 0,
						   SWP_NOMOVE | SWP_NOSIZE | 
						   SWP_NOACTIVATE);
				      SetActiveWindow(hwndT);
				      break;
				    }
				}

			      /*** FALL THRU ***/

			  case WM_RBUTTONDOWN:
			  case WM_MBUTTONDOWN:
                              MessageBeep(0);
			      break;
			}
		      /*** FALL THRU ***/

		  default:
		      SetCursor(hCursNormal);
		}
	    }

	  return((LONG)FALSE);

      case WM_MOUSEACTIVATE:
	  if ((hwndT = GetChildParent(hwnd)) != NULL &&
	      (i = (int)SendMessage(hwndT, WM_MOUSEACTIVATE, wParam, lParam))
		      != 0)
	      return((LONG)i);

	  /* Moving, sizing or minimizing? Activate AFTER we take action. */
	  if (LOWORD(lParam) == HTCAPTION)
	      return((LONG)MA_NOACTIVATE);
	  else
	      return((LONG)MA_ACTIVATE);

      case WM_DRAWITEM:
          if (((LPDRAWITEMSTRUCT)lParam)->CtlType == ODT_LISTBOX)
              LBDefaultListboxDrawItem((LPDRAWITEMSTRUCT)lParam);
          break;

    }

  return(0L);
}
