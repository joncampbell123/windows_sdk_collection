/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  DefWindowProc() -                                                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/

LRESULT API IDefWindowProc(HWND hwnd, WORD message, WPARAM wParam, LPARAM lParam)
{
    int     i;
    HBRUSH  hbr;
    HWND    hwndT;

    switch (message)
    {
    case WM_NCCREATE:
	// If WS_HSCROLL or WS_VSCROLL, initialize storage for scroll positions.
	//
	// NOTE: Scroll bar storage and text storage will be freed automatically
	// by Windows during CreateWindow()
	//
	if (TestWF(hwnd, (WFHSCROLL | WFVSCROLL)))
	{
	    // Initialize extra storage for

	    if (InitPwSB(hwnd) == NULL)
		return((LONG)FALSE);
	}
	// Store window text if present.
	//
	return((LRESULT)(LONG)DefSetText(hwnd, ((LPCREATESTRUCT)lParam)->lpszName));

    case WM_NCCALCSIZE:
	//
	// wParam = fCalcValidRects
	// lParam = LPRECT rgrc[3]:
	//        lprc[0] = rcWindowNew = New window rectangle
	//    if fCalcValidRects:
	//        lprc[1] = rcWindowOld = Old window rectangle
	//        lprc[2] = rcClientOld = Old client rectangle
	//
	// On return:
	//        rgrc[0] = rcClientNew = New client rect
	//    if fCalcValidRects:
	//        rgrc[1] = rcValidDst  = Destination valid rectangle
	//        rgrc[2] = rcValidSrc  = Source valid rectangle
	//
	CalcClientRect(hwnd, (LPRECT)lParam);
	break;

    case WM_NCHITTEST:
	//
	// Determine what area the passed coordinate is in.
	//
	return((LRESULT)(DWORD)FindNCHit(hwnd, (LONG)lParam));

    case WM_NCPAINT:
	//
	// Do non-client area drawing.
	//
	DWP_DoNCPaint(hwnd, (HRGN)wParam);
	break;

    case WM_NCACTIVATE:
	//
	// Do non-client drawing in response to
	// activation or deactivation.
	//
	DWP_DoNCActivate(hwnd, (BOOL)wParam);
	return (LRESULT)(DWORD)TRUE;

    case WM_CANCELMODE:
	//
	// Terminate any modes the system might
	// be in, such as scrollbar tracking, menu mode,
	// button capture, etc.
	//
	DWP_DoCancelMode(hwnd);
	break;

    case WM_SETTEXT:
	// Set the new text and redraw the caption or icon title window.
	//
	DefSetText(hwnd, (LPCSTR)lParam);
	DWP_RedrawTitle(hwnd);
	break;

    case WM_GETTEXT:
	//
	// If the buffer size is > 0, copy as much of the window text as
	// will fit (making sure to zero terminate the result).
	//
	if (wParam)
	{
	    if (hwnd->hName)
		return (LRESULT)(LONG)TextCopy(hwnd->hName, (LPSTR)lParam, (int)wParam);

	    // No string: make sure we return an empty string.
	    //
	    ((LPSTR)lParam)[0] = 0;
	}
	return (0L);

    case WM_GETTEXTLENGTH:
	//
	// Just return the length of the window text (excluding 0 terminator)
	//
	if (hwnd->hName)
	    return((LRESULT)(LONG)lstrlen(TextPointer(hwnd->hName)));
	return(0L);

    case WM_PAINT:
    case WM_PAINTICON:
	DWP_Paint(message, hwnd);
	break;

    case WM_ERASEBKGND:
    case WM_ICONERASEBKGND:
	return (LRESULT)(LONG)DWP_EraseBkgnd(hwnd, message, (HDC)wParam);

    case WM_SYNCPAINT:
	//
	// This message is sent when SetWindowPos() is trying
	// to get the screen looking nice after window rearrangement,
	// and one of the windows involved is of another task.
	// This message avoids lots of inter-app message traffic
	// by switching to the other task and continuing the
	// recursion there.
	//
	// wParam         = flags
	// LOWORD(lParam) = hrgnClip
	// HIWORD(lParam) = hwndSkip  (not used; always NULL)
	//
	// hwndSkip is now always NULL.
	//
	// NOTE: THIS MESSAGE IS FOR INTERNAL USE ONLY! ITS BEHAVIOR
	// IS DIFFERENT IN 3.1 THAN IN 3.0!!
	//
	DoSyncPaint(hwnd, NULL, ((WORD)wParam | DSP_WM_SYNCPAINT));
	break;

    case WM_SYSCOMMAND:
	SysCommand(hwnd, (int)wParam, lParam);
	break;

    case WM_ACTIVATE:
	//
	// By default, windows set the focus to themselves when activated.
	//
	if ((BOOL)wParam)
	    SetFocus(hwnd);
	break;

    case WM_SETREDRAW:
	//
	// Set or clear the WS_VISIBLE bit, without invalidating the window.
	// (Also performs some internal housekeeping to ensure that window
	// DC clip regions are computed correctly).
	//
	DWP_SetRedraw(hwnd, (BOOL)wParam);
	break;

    case WM_WINDOWPOSCHANGING:
	//
	// If the window's size is changing, and the window has
	// a size border (WS_THICKFRAME) or is a main window (WS_OVERLAPPED),
	// then adjust the new width and height by sending a WM_MINMAXINFO message.
	//
	#define ppos ((WINDOWPOS FAR *)lParam)
	if (!(ppos->flags & SWP_NOSIZE))
	    AdjustSize(hwnd, &ppos->cx, &ppos->cy);
	#undef ppos
	break;

    case WM_WINDOWPOSCHANGED:
	//
	// If (!(lpswp->flags & SWP_NOCLIENTMOVE)
	//    send WM_MOVE message
	//
	// If (!(lpswp->flags & SWP_NOCLIENTSIZE)
	//    send WM_SIZE message with wParam set based on
	//    current WS_MINIMIZED/WS_MAXIMIZED style.
	//
	// If DefWindowProc() is not called, WM_MOVE and WM_SIZE messages
	// will not be sent to the window.
	//
	HandleWindowPosChanged(hwnd, (WINDOWPOS FAR *)lParam);
	break;

    case WM_CTLCOLOR:
	//
	// Set up the supplied DC with the foreground and background
	// colors we want to use in the control, and return a brush
	// to use for filling.
	//
	switch (HIWORD(lParam))
	{
	case CTLCOLOR_SCROLLBAR:
	    //
	    // Background = white
	    // Foreground = black
	    // brush = COLOR_SCROLLBAR.
	    //
	    SetBkColor((HDC)wParam, RGB(255, 255, 255));
	    SetTextColor((HDC)wParam, RGB(0, 0, 0));
	    hbr = sysClrObjects.hbrScrollbar;

	    // The scroll bar color may be dithered, so unrealize it.
	    //
	    UnrealizeObject(hbr);
	    break;

	default:
	    //
	    // Background = COLOR_WINDOW
	    // Foreground = COLOR_WINDOWTEXT
	    // Brush = COLOR_WINDOW
	    //
	    SetBkColor((HDC)wParam, sysColors.clrWindow);
	    SetTextColor((HDC)wParam, sysColors.clrWindowText);
	    hbr = sysClrObjects.hbrWindow;
	}
	return((LRESULT)(DWORD)(WORD)hbr);

    case WM_SETCURSOR:
	//
	// wParam  == hwndHit == hwnd that cursor is over
	// lParamL == codeHT  == Hit test area code (result of WM_NCHITTEST)
	// lParamH == msg     == Mouse message number (may be 0)
	//
	// Strategy: First forward WM_SETCURSOR message to parent.  If it
	// returns TRUE (i.e., it set the cursor), just return.  Otherwise,
	// set the cursor based on codeHT and msg.
	//
	return (LRESULT)(LONG)DWP_SetCursor(hwnd, (HWND)wParam,
		LOWORD(lParam), HIWORD(lParam));

    case WM_MOUSEACTIVATE:
	//
	// First give the parent a chance to process the message.
	//
	hwndT = GetChildParent(hwnd);
	if (hwndT)
	{
	    i = (int)(DWORD)SendMessage(hwndT, WM_MOUSEACTIVATE,
		wParam, lParam);

	    if (i != 0)
		return (LRESULT)(LONG)i;
	}

	// If the user clicked in the title bar, don't activate now:
	// the activation will take place later when the move or size
	// occurs.
	//
	if (LOWORD(lParam) == HTCAPTION)
	    return((LRESULT)(LONG)MA_NOACTIVATE);

	return((LRESULT)(LONG)MA_ACTIVATE);

    case WM_SHOWWINDOW:
	//
	// If we are being called because our owner window is being shown,
	// hidden, minimized, or un-minimized, then we must hide or show
	// show ourself as appropriate.
	//
	// This behavior occurs for popup windows or owned windows only.
	// It's not designed for use with child windows.
	//
	if (LOWORD(lParam) != 0 && (TestwndPopup(hwnd) || hwnd->hwndOwner))
	{
	    // The WFHIDDENPOPUP flag is an internal flag that indicates
	    // that the window was hidden because its owner was hidden.
	    // This way we only show windows that were hidden by this code,
	    // not intentionally by the application.
	    //
	    // Go ahead and hide or show this window, but only if:
	    //
	    // a) we need to be hidden, or
	    // b) we need to be shown, and we were hidden by
	    //    an earlier WM_SHOWWINDOW message
	    //
	    if ((!wParam && TestWF(hwnd, WFVISIBLE)) ||
	       (wParam && !TestWF(hwnd, WFVISIBLE) && TestWF(hwnd, WFHIDDENPOPUP)))
	    {
		// Remember that we were hidden by WM_SHOWWINDOW processing
		//
		ClrWF(hwnd, WFHIDDENPOPUP);
		if (!wParam)
		    SetWF(hwnd, WFHIDDENPOPUP);

		ShowWindow(hwnd, (wParam ? SW_SHOWNOACTIVATE : SW_HIDE));
	    }
	}
	break;

    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONUP:
    case WM_NCLBUTTONDBLCLK:
    case WM_NCMOUSEMOVE:
	//
	// Deal with mouse messages in the non-client area.
	//
	DWP_NCMouse(hwnd, message, wParam, lParam);
	break;

    case WM_KEYDOWN:
	// Windows 2.0 backward compatibility:
	// Alias F10 to the menu key
	// (only for apps that don't handle WM_KEY* messages themselves)
	//
	if ((WORD)wParam == VK_F10)
	    fF10Status = TRUE;
	break;

    case WM_SYSKEYDOWN:
	// Is the ALT key down?
	if (HIWORD(lParam) & SYS_ALTERNATE)
	{
	    // Toggle only if this is not an autorepeat key
	    //
	    if ((HIWORD(lParam) & SYS_PREVKEYSTATE) == 0)
	    {
		if (((WORD)wParam == VK_MENU) && (!fMenuStatus))
		    fMenuStatus = TRUE;
		else
		    fMenuStatus = FALSE;
	    }

	    fF10Status = FALSE;

	    DWP_ProcessVirtKey((WORD)wParam);
	}
	else
	{
	    if ((WORD)wParam == VK_F10)
	    {
		fF10Status = TRUE;
	    }
	    else
	    {
		if ((WORD)wParam == VK_ESCAPE)
		{
		    if (GetKeyState(VK_SHIFT) < 0)
		    {
			SendMessage(hwnd, WM_SYSCOMMAND,
				(WPARAM)SC_KEYMENU, (LPARAM)(DWORD)MENUSYSMENU);
		    }
		}
	    }
	}
	break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
	// Press and release F10 or ALT. Send this only to top-level
	// windows, otherwise MDI gets confused.  The fix in which
	// DefMDIChildProc() passed up the message was insufficient in the
	// case a child window of the MDI child had the focus.
	//
	if ( ((WORD)wParam == VK_MENU && (fMenuStatus == TRUE)) ||
		((WORD)wParam == VK_F10 && fF10Status) )
	{
	    SendMessage(GetTopLevelWindow(hwnd), WM_SYSCOMMAND,
		    (WPARAM)SC_KEYMENU, 0L);
	}
	fF10Status = fMenuStatus = FALSE;
	break;

    case WM_SYSCHAR:
	// If syskey is down and we have a char... */
	fMenuStatus = FALSE;

	if ((WORD)wParam == VK_RETURN && TestWF(hwnd, WFMINIMIZED))
	{
	    // If the window is iconic and user hits RETURN, we want to
	    // restore this window.
	    //
	    PostMessage(hwnd, WM_SYSCOMMAND, (WPARAM)SC_RESTORE, 0L);
	    break;
	}

	if ((HIWORD(lParam) & SYS_ALTERNATE) && wParam)
	{
	    if ((WORD)wParam == VK_TAB || (WORD)wParam == VK_ESCAPE)
		break;

	    // Send ALT-SPACE only to top-level windows.
	    if (((WORD)wParam == MENUSYSMENU) && (TestwndChild(hwnd)))
		SendMessage(hwnd->hwndParent, message, wParam, lParam);
	    else
		SendMessage(hwnd, WM_SYSCOMMAND, (WPARAM)SC_KEYMENU, (LPARAM)(DWORD)(WORD)wParam);
	}
	else
	{
	    // Ctrl-Esc produces a WM_SYSCHAR, but should not beep
	    if ((WORD)wParam != VK_ESCAPE)
		MessageBeep(0);
	}
	break;

    case WM_CLOSE:
	//
	// Default WM_CLOSE handling is to destroy the window.
	//
	DestroyWindow(hwnd);
	break;

    case WM_QUERYOPEN:
    case WM_QUERYENDSESSION:
	return((LRESULT)(LONG)TRUE);

    case WM_ISACTIVEICON:
	return ((LRESULT)(DWORD)(BOOL)(TestWF(hwnd, WFFRAMEON) != 0));

    case WM_CHARTOITEM:
    case WM_VKEYTOITEM:
	//
	// Return -1 to cause default processing
	//
	return((LRESULT)-1L);

    case WM_DRAWITEM:
	#define lpdis   ((LPDRAWITEMSTRUCT)lParam)
	if (lpdis->CtlType == ODT_LISTBOX)
	    LBDefaultListboxDrawItem(lpdis);
	#undef  lpdis
	break;

    case WM_GETHOTKEY:
	return((LRESULT)(LONG)DWP_GetHotKey(hwnd));
	break;

    case WM_SETHOTKEY:
	return((LRESULT)(LONG)SetHotKey(hwnd, (WORD)wParam));
	break;

    case WM_QUERYDRAGICON:
	return((LRESULT)(DWORD)(WORD)DWP_QueryDragIcon(hwnd));
	break;

    case WM_QUERYDROPOBJECT:
	//
	// If the application is WS_EX_ACCEPTFILES, return TRUE.
	//
	if (TestWF(hwnd, WEFACCEPTFILES))
	    return (LRESULT)(DWORD)TRUE;

	return (LRESULT)(DWORD)FALSE;

    case WM_DROPOBJECT:
	return (LRESULT)(DWORD)DO_DROPFILE;

    }   // end switch
    return(0L);
}

