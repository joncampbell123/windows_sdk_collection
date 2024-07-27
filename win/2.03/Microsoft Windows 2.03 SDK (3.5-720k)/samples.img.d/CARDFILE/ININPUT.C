#define NORASTEROPS
#define NOSYSCOMMANDS
#include "index.h"

/****************************************************************/
/*								*/
/*  Windows Cardfile - Written by Mark Cliggett 		*/
/*  (c) Copyright Microsoft Corp. 1985 - All Rights Reserved	*/
/*								*/
/****************************************************************/

extern HWND hCardWnd;
HANDLE hEditCurs;
CATCHBUF CatchBuf;
int fCanPrint = FALSE;

IndexInput(hWindow, event)
HWND hWindow;
int event;
    {
    char *pchBuf;
    char *pchFile;
    LPCARDHEADER lpCards;
    int i;
    int fh;
    char buf[50];
    char buf2[150];
    int CatchVal;
    RECT rect;
    long ltmp;
    OFSTRUCT ofStruct;
    unsigned long cBytes;

    extern long FAR mylmul();

    if (CatchVal = Catch((LPCATCHBUF)CatchBuf))
	{
	switch(CatchVal)
	    {
	    case EINSMEMORY:
                MessageBox(hWindow, (LPSTR)NotEnoughMem, (LPSTR)rgchCardfile,  MB_OK | MB_ICONHAND | MB_SYSTEMMODAL);
		break;
	    default:
		break;
	    }
	return;
	}

    switch(event)
	{
	case ABOUT:
	    PutUpDB(DTABOUT);
	    break;
        case EXIT:
            PostMessage(hWindow, WM_CLOSE, 0, 0L);
            break;
	case NEW:
	    if (fReadOnly)
		break;
	    if (!MaybeSaveFile(FALSE))
		break;
	    SetCursor(hWaitCurs);
	    CurIFile[0] = 0;
	    CurIFind[0] = 0;
	    SetCaption();
            Fdelete(TempFile);
            MakeTempFile(hIndexInstance);
	    /* shrinking or leaving the same size, so this should always work */
	    GlobalReAlloc(hCards, (long)sizeof(CARDHEADER),GMEM_MOVEABLE);
	    cCards = 1;
	    iFirstCard = 0;
	    iTopCard = 0;
	    SetScrRangeAndPos();
	    MakeBlankCard();
	    lpCards = (LPCARDHEADER) GlobalLock(hCards);
	    *lpCards = CurCardHead;
	    GlobalUnlock(hCards);
	    if (CardPhone == PHONEBOOK)
		SaveCurrentCard(iFirstCard);
	    InvalidateRect(hWindow, (LPRECT)NULL, TRUE);
	    SetCursor(hArrowCurs);
	    fFileDirty = FALSE;
	    if (CardPhone == CCARDFILE)
		{
		SetFocus(NULL);
		SetFocus(hCardWnd);
		}
	    break;
	case OPEN:
	    if(MaybeSaveFile(FALSE))
		{
		if (pchBuf = PutUpDB(DTOPEN))
		    {
		    if (!DoOpen(pchBuf))
			SetCurCard(iFirstCard);
		    LocalFree((HANDLE)pchBuf);
		    }
		else
		    SetCurCard(iFirstCard);
		}
	    break;
	case MERGE:		/* merge in another file */
	    DoMerge();
	    break;
	case PRINT:
	    PrintCards(1);
	    break;
	case PRINTALL:
	    if (CardPhone == CCARDFILE)
		PrintCards(cCards);
	    else
		PrintList();
	    break;
	case SAVE:
	    if (CurIFile[0])
		{
                /* make copy of filename, since it will be translated
                 * in writecardfile(). 04-Nov-1987. davidhab.
                 */
                lstrcpy((LPSTR)buf2, (LPSTR)CurIFile);
                pchFile = buf2;
		goto Save;
		}
	case SAVEAS:
SaveAs:
	    if(pchBuf = PutUpDB(DTSAVE))
		{
		i = OpenFile((LPSTR)pchBuf, (LPOFSTRUCT)&ofStruct, OF_PARSE);
		pchFile = ofStruct.szPathName;
		if (i)
		    {
		    IndexOkError(EINVALIDFILE);
		    LocalFree((HANDLE)pchBuf);
		    goto SaveAs;
		    }
                if ((fh = _lopen((LPSTR)pchFile, READ)) > -1)
		    {
		    _lclose(fh);
		    LoadString(hIndexInstance, EFILEEXISTS, (LPSTR)buf, 50);
		    AnsiUpper((LPSTR)pchBuf);
		    MergeStrings((LPSTR)buf, (LPSTR)pchBuf, (LPSTR)buf2);
		    if (MessageBox(hIndexWnd, (LPSTR)buf2, (LPSTR)rgchWarning, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDNO)
			{
			LocalFree((HANDLE)pchBuf);
			goto SaveAs;
			}
		    UpdateWindow(hIndexWnd);
		    }
		LocalFree((HANDLE)pchBuf);
Save:
		SetCursor(hWaitCurs);
		if (CardPhone == CCARDFILE)
		    ltmp = SendMessage(hCardWnd, EM_GETSEL, 0, 0L);
		if (CardPhone == PHONEBOOK || SaveCurrentCard(iFirstCard))
		    {
		    if (WriteCardFile(pchFile))
			{
			SetCaption();
                        Fdelete(TempFile);
                        MakeTempFile(hIndexInstance);
			}
                    /* if write fails, due to can't create or memfull,
                     * return to saveas dialog box.
                     * 20-Jul-1987.
                     */
                    else
                        goto SaveAs;

		    if (CardPhone == CCARDFILE)
			{
			SetCurCard(iFirstCard);
			SendMessage(hCardWnd, EM_SETSEL, 0, ltmp);
			}
		    }
		SetCursor(hArrowCurs);
		}
	    break;
	case CCARDFILE:
	case PHONEBOOK:
	    if (event != CardPhone)
		{
		if (event == PHONEBOOK)
		    {
		    if (!SaveCurrentCard(iFirstCard))
			break;
		    }
		else
		    {
		    SetCurCard(iFirstCard);
		    }
		CardPhone = event;
		SetScrollRange(hWindow, event == PHONEBOOK ? SB_HORZ : SB_VERT, 0, 0, FALSE);

		ShowWindow(hCardWnd, event == PHONEBOOK ? HIDE_WINDOW : SHOW_OPENWINDOW);
		SendMessage(hCardWnd, WM_SETREDRAW, event == CCARDFILE, 0L);

		if (event == PHONEBOOK)
		    {
		    GetClientRect(hIndexWnd, (LPRECT)&rect);
		    i = rect.bottom / CharFixHeight;
		    if (!i)
			i = 1;
		    iTopCard = min(iFirstCard-(i-1)/2, (cCards - i));
		    if (iTopCard < 0)
			iTopCard = 0;
		    }
		SetScrRangeAndPos();
		SetFocus(event == PHONEBOOK ? hIndexWnd : hCardWnd);
		InvalidateRect(hWindow, (LPRECT)NULL, TRUE);
		}
	    break;
	case UNDO:
	    if (CardPhone == CCARDFILE)
		SendMessage(hCardWnd, EM_UNDO, 0, 0L);
	    break;
	case HEADER:
	    if (CardPhone == PHONEBOOK)
		{
		SetCurCard(iFirstCard);
		SaveCurrentCard(iFirstCard);
		}
	    if(pchBuf = PutUpDB(DTHEADER))
		{
		lstrcpy((LPSTR)CurCardHead.line, (LPSTR)pchBuf);
		DeleteCard(iFirstCard);     /* take it out of it's current position */
		iFirstCard = AddCurCard();  /* and put it back in the right place */
		SetScrRangeAndPos();	    /* set position */
		fFileDirty = TRUE;
		InvalidateRect(hWindow, (LPRECT)NULL, TRUE);
		LocalFree((HANDLE)pchBuf);
		}
	    break;
	case RESTORE:
	    if (CurCard.hBitmap)
		{
		DeleteObject(CurCard.hBitmap);
		CurCard.hBitmap = 0;
		}
	    SetCurCard(iFirstCard);
	    InvalidateRect(hCardWnd, (LPRECT)NULL, TRUE);
	    break;
	case CUT:
	case COPY:
	    DoCutCopy(event);
	    break;
	case PASTE:
	    DoPaste();
	    break;
	case I_TEXT:
	case I_BITMAP:
	    if (event != EditMode)
		{
		if (event == I_BITMAP)
		    hEditCurs = SetCursor(hArrowCurs);
		else
		    SetCursor(hEditCurs);
		SetFocus(NULL);     /* make sure the old caret is off */
		EditMode = event;
		SetFocus(hCardWnd);
		}
	    break;
	case ADD:
	    if(pchBuf = PutUpDB(DTADD))
		{
                /* limit data to <64k. 26-Jun-1987. */
                cBytes = mylmul((cCards+1),sizeof(CARDHEADER));
                if (cBytes >= 0x0000FFFF)
                    goto Fail;
                if (!GlobalReAlloc(hCards, cBytes, GMEM_MOVEABLE))
Fail:
		    Throw((LPCATCHBUF)CatchBuf, EINSMEMORY);
		else
		    {
		    if (CardPhone == PHONEBOOK || SaveCurrentCard(iFirstCard))
			{
			MakeBlankCard();
			lstrcpy((LPSTR)CurCardHead.line, (LPSTR)pchBuf);
			CurCardHead.flags |= (FDIRTY + FNEW);
			iFirstCard = AddCurCard();
			if (CardPhone == PHONEBOOK)
			    {
			    GetClientRect(hIndexWnd, (LPRECT)&rect);
			    i = rect.bottom / CharFixHeight;
			    if (!i)
				i = 1;
			    iTopCard = min(iFirstCard-(i-1)/2, (cCards - i));
			    if (iTopCard < 0)
				iTopCard = 0;
			    }
			SetScrRangeAndPos();
			if (CardPhone == PHONEBOOK)
			    SaveCurrentCard(iFirstCard);
			else
			    {
			    SetFocus(NULL);
			    SetFocus(hCardWnd);
			    }
			InvalidateRect(hWindow, (LPRECT)NULL, TRUE);
			}
		    }
		LocalFree((HANDLE)pchBuf);
		}
	    break;
	case DELETE:
	    if (CardPhone == PHONEBOOK)
		{
		SetCurCard(iFirstCard);
		SaveCurrentCard(iFirstCard);
		}
	    LoadString(hIndexInstance, IDELCURCARD, (LPSTR)buf, 50);
	    MergeStrings((LPSTR)buf, (LPSTR)CurCardHead.line, (LPSTR)buf2);
	    if (MessageBox(hIndexWnd, (LPSTR)buf2, (LPSTR)rgchWarning, MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2) == IDOK)
		{
		if (CurCard.hBitmap)
		    {
		    DeleteObject(CurCard.hBitmap);
		    CurCard.hBitmap = 0;
		    }
		if (cCards > 1)
		    {
		    DeleteCard(iFirstCard);
		    if (iFirstCard == cCards)
			iFirstCard = (CardPhone == CCARDFILE ? 0 : cCards-1);
		    SetScrRangeAndPos();
		    if (CardPhone == CCARDFILE)
			SetCurCard(iFirstCard);
		    /* shrinking, so don't have to check */
                    GlobalReAlloc(hCards, (unsigned long)((unsigned)cCards*(unsigned)sizeof(CARDHEADER)),GMEM_MOVEABLE);
		    }
		else
		    {
		    MakeBlankCard();
		    lpCards = (LPCARDHEADER) GlobalLock(hCards);
		    *lpCards = CurCardHead;
		    GlobalUnlock(hCards);
		    if (CardPhone == PHONEBOOK)
			SaveCurrentCard(iFirstCard);
		    }
		fFileDirty = TRUE;
		InvalidateRect(hWindow, (LPRECT)NULL, TRUE);
		}
	    break;
	case DUPLICATE:
            /* limit data to <64k. 26-Jun-1987. */
            cBytes = mylmul((cCards+1),sizeof(CARDHEADER));
            if (cBytes >= 0x0000FFFF)
                goto Fail2;
            if (!GlobalReAlloc(hCards, cBytes, GMEM_MOVEABLE))
Fail2:
		Throw((LPCATCHBUF)CatchBuf, EINSMEMORY);
	    else
		{
		if (CardPhone == PHONEBOOK || SaveCurrentCard(iFirstCard))
		    {
		    SetCurCard(iFirstCard);
		    CurCardHead.flags |= (FDIRTY + FNEW);
		    iFirstCard = AddCurCard();
		    if (CardPhone == PHONEBOOK)
			{
			SaveCurrentCard(iFirstCard);
			GetClientRect(hIndexWnd, (LPRECT)&rect);
			i = rect.bottom / CharFixHeight;
			if (!i)
			    i = 1;
			iTopCard = min(iFirstCard-(i-1)/2, (cCards - i));
			if (iTopCard < 0)
			    iTopCard = 0;
			}
		    SetScrRangeAndPos();
		    InvalidateRect(hWindow, (LPRECT)NULL, TRUE);
		    fFileDirty = TRUE;
		    }
		}
	    break;
	case DIAL:
	    if (CardPhone == PHONEBOOK)
		{
		SetCurCard(iFirstCard);
		SaveCurrentCard(iFirstCard);
		}
	    if (pchBuf = PutUpDB(DTDIAL))
		{
		DoDial(pchBuf);
		LocalFree((HANDLE)pchBuf);
		}
	    break;
	case GOTO:
	    if (pchBuf = PutUpDB(DTGOTO))
		{
		DoGoto(pchBuf);
		LocalFree((HANDLE)pchBuf);
		}
	    break;
	case FINDNEXT:
	    if (CurIFind[0])
		{
		FindStrCard();
		break;
		}
	case FIND:
	    if(pchBuf = PutUpDB(DTFIND))
		{
		lstrcpy((LPSTR)CurIFind, (LPSTR)pchBuf);
		FindStrCard();
		LocalFree((HANDLE)pchBuf);
		}
	    break;
	default:
	    break;
	}
    }

char * PutUpDB(idb)
int idb;
    {
    FARPROC lpdbProc;
    extern int DBcmd;
    int wResult;

    DBcmd = idb;
    switch(idb)
	{
	case DTMERGE:
	case DTOPEN:
	    lpdbProc = lpfnOpen;
	    break;
	case DTSAVE:
	    lpdbProc = lpfnSave;
	    break;
	case DTABOUT:
	    lpdbProc = lpfnAbout;
	    break;
	case DTDIAL:
	    lpdbProc = lpfnDial;
	    break;
	default:
	    lpdbProc = lpDlgProc;
	    break;
	}

    wResult = DialogBox(hIndexInstance, (LPSTR)idb, hIndexWnd, lpdbProc);

    if (wResult == -1) {
        MessageBox(NULL, (LPSTR)NotEnoughMem, (LPSTR)rgchCardfile, MB_OK | MB_ICONHAND | MB_SYSTEMMODAL);
        wResult = 0;
    }

    return((char *)wResult);
    }

UpdateMenu()
    {
    HMENU hMenu;
    long lSelection;
    int wFmt;
    int mfPaste;

    hMenu = GetMenu(hIndexWnd);
    CheckMenuItem(hMenu, CCARDFILE, CardPhone == CCARDFILE ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(hMenu, PHONEBOOK, CardPhone == PHONEBOOK ? MF_CHECKED : MF_UNCHECKED);

    CheckMenuItem(hMenu, I_TEXT, EditMode == I_TEXT ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(hMenu, I_BITMAP, EditMode == I_BITMAP ? MF_CHECKED : MF_UNCHECKED);
    if (CardPhone == CCARDFILE)
	{
	if (EditMode == I_TEXT && SendMessage(hCardWnd, EM_CANUNDO, 0, 0L))
	    EnableMenuItem(hMenu, UNDO, MF_ENABLED);
	else
	    EnableMenuItem(hMenu, UNDO, MF_GRAYED);

	mfPaste = MF_GRAYED;
	if (OpenClipboard(hIndexWnd))
	    {
	    wFmt = 0;

	    while (wFmt = EnumClipboardFormats(wFmt))
		{
		if (wFmt == (EditMode == I_BITMAP ? CF_BITMAP : CF_TEXT))
		    {
		    mfPaste = MF_ENABLED;
		    break;
		    }
		}

	    CloseClipboard();
	    }
	EnableMenuItem(hMenu, PASTE, mfPaste);

	EnableMenuItem(hMenu, I_TEXT, MF_ENABLED);
	EnableMenuItem(hMenu, I_BITMAP, MF_ENABLED);
	EnableMenuItem(hMenu, RESTORE, MF_ENABLED);
	EnableMenuItem(hMenu, FIND, MF_ENABLED);
	EnableMenuItem(hMenu, FINDNEXT, MF_ENABLED);
	EnableMenuItem(hMenu, PRINT, fCanPrint ? MF_ENABLED : MF_GRAYED);
	if (EditMode == I_TEXT)
	    {
	    lSelection = SendMessage(hCardWnd, EM_GETSEL, 0, 0L);
	    if (HIWORD(lSelection) == LOWORD(lSelection))
		goto Disabled;
	    else
		goto Enabled;
	    }
	else
	    if (CurCard.hBitmap)
		{
Enabled:
		EnableMenuItem(hMenu, CUT, MF_ENABLED);
		EnableMenuItem(hMenu, COPY, MF_ENABLED);
		}
	    else
		{
Disabled:
		EnableMenuItem(hMenu, CUT, MF_GRAYED);
		EnableMenuItem(hMenu, COPY, MF_GRAYED);
		}
	}
    else
	{
	EnableMenuItem(hMenu, UNDO, MF_GRAYED);
	EnableMenuItem(hMenu, I_TEXT, MF_GRAYED);
	EnableMenuItem(hMenu, I_BITMAP, MF_GRAYED);
	EnableMenuItem(hMenu, RESTORE, MF_GRAYED);
	EnableMenuItem(hMenu, CUT, MF_GRAYED);
	EnableMenuItem(hMenu, COPY, MF_GRAYED);
	EnableMenuItem(hMenu, PASTE, MF_GRAYED);
	EnableMenuItem(hMenu, FIND, MF_GRAYED);
	EnableMenuItem(hMenu, FINDNEXT, MF_GRAYED);
	EnableMenuItem(hMenu, PRINT, MF_GRAYED);
	}
    }

SetEditText(lpText)
LPSTR lpText;
    {
    fSettingText = TRUE;
    SetWindowText(hCardWnd, lpText);
    }

IndexMouse(hWindow, message, wParam, pt)
HWND hWindow;
int message;
WORD wParam;
POINT pt;
    {
    RECT rect;
    int iCard;
    HDC hDC;
    int y;
    MSG msg;

    if (CardPhone == CCARDFILE)
	{
	if ((iCard = MapPtToCard(pt)) > -1)  /* see if click on a card or background */
	    {
	    if (iCard != iFirstCard)		    /* if on another card */
		{				    /* bring it to front */
		IndexScroll(hIndexWnd, SB_THUMBTRACK, iCard);
		IndexScroll(hIndexWnd, SB_THUMBPOSITION, iCard);
		}
	    else if (message == WM_LBUTTONDBLCLK)	/* if double click on first */
		{					/* bring up index box */
		SetCapture(hIndexWnd);
		while(GetKeyState(VK_LBUTTON) < 0)
		    {
		    PeekMessage((LPMSG)&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, TRUE);
		    PeekMessage((LPMSG)&msg, NULL, WM_KEYFIRST, WM_KEYLAST, TRUE);
		    }
		ReleaseCapture();
		IndexInput(hWindow, HEADER);
		}
	    }
	}
    else
	{
	iCard = iTopCard + (pt.y / CharFixHeight);
	if (message == WM_LBUTTONDOWN || iCard != iFirstCard)
	    {
	    if (iCard < cCards)
		{
		y = (iFirstCard - iTopCard) * CharFixHeight;
		hDC = GetDC(hWindow);
		SetRect((LPRECT)&rect, 0, y, (LINELENGTH+2)*CharFixWidth, y+CharFixHeight);
		InvertRect(hDC, (LPRECT)&rect);
		iFirstCard = iCard;
		y = (iFirstCard - iTopCard) * CharFixHeight;
		SetRect((LPRECT)&rect, 0, y, (LINELENGTH+2)*CharFixWidth, y+CharFixHeight);
		InvertRect(hDC, (LPRECT)&rect);
		ReleaseDC(hWindow, hDC);
		}
	    }
	else /* double click on cur card */
	    {
	    if (iCard < cCards)
		{
		while(!PeekMessage((LPMSG)&msg, NULL, WM_LBUTTONUP, WM_LBUTTONUP, TRUE))
		    ;
		IndexInput(hWindow, HEADER);
		}
	    }
	}
    }

FAR GetNewCard(iOldCard, iNewCard)
int iOldCard;
int iNewCard;
    {
    HDC hDC;
    RECT rect;
    int y;

    if (CardPhone == PHONEBOOK)
	{
	y = (iOldCard - iTopCard) * CharFixHeight;
	hDC = GetDC(hIndexWnd);
	SetRect((LPRECT)&rect, 0, y, (LINELENGTH+2)*CharFixWidth, y+CharFixHeight);
	InvertRect(hDC, (LPRECT)&rect);
	ReleaseDC(hIndexWnd, hDC);
	BringCardOnScreen(iFirstCard = iNewCard);
	}
    else
	{
	IndexScroll(hIndexWnd, SB_THUMBTRACK, iNewCard);
	return(IndexScroll(hIndexWnd, SB_THUMBPOSITION, iNewCard));
	}
    return(TRUE);
    }

MapPtToCard(pt)
POINT pt;
    {
    int idCard;
    int xCur;
    int yCur;
    int i;
    RECT rect;

    yCur = yFirstCard - (cScreenCards - 1)*ySpacing;
    xCur = xFirstCard + (cScreenCards - 1)* (2 * CharFixWidth);
    idCard = (iFirstCard + cScreenCards-1) % cCards;

    for (i = 0; i < cScreenCards; ++i)
	{
	SetRect((LPRECT)&rect, xCur+1, yCur+1, xCur+CardWidth-1, yCur+CharFixHeight+1);
	if (PtInRect((LPRECT)&rect, pt))
	    return(idCard);
	SetRect((LPRECT)&rect, rect.right - 2*CharFixWidth + 2, rect.top,rect.right,rect.top+CardHeight-2);
	if (PtInRect((LPRECT)&rect, pt))
	    return(idCard);
	xCur -= (2*CharFixWidth);
	yCur += ySpacing;
	idCard--;
	if (idCard < 0)
	    idCard = cCards - 1;
	}
    return(-1);
    }

FAR IndexOkError(strid)
int strid;
    {
    char buf[128];

    if (strid == EINSMEMORY)
	lstrcpy((LPSTR)buf, (LPSTR)NotEnoughMem);
    else
	LoadString(hIndexInstance, strid, (LPSTR)buf, 128);
    MessageBox(hIndexWnd, (LPSTR)buf, (LPSTR)rgchWarning, MB_OK | MB_ICONEXCLAMATION);
    }

/* ** Scan sz1 for merge spec.	If found, insert string sz2 at that point.
      Then append rest of sz1 NOTE! Merge spec guaranteed to be two chars.
      returns TRUE if it does a merge, false otherwise. */
BOOL  FAR MergeStrings(lpszSrc, lpszMerge, lpszDst)
LPSTR	lpszSrc;
LPSTR	lpszMerge;
LPSTR	lpszDst;
{
    LPSTR lpchSrc;
    LPSTR lpchDst;

    lpchSrc = lpszSrc;
    lpchDst = lpszDst;

    /* Find merge spec if there is one. */
    while (*(unsigned far *)lpchSrc != wMerge) {
	*lpchDst++ = *lpchSrc;

	/* If we reach end of string before merge spec, just return. */
	if (!*lpchSrc++)
	    return FALSE;

    }
    /* If merge spec found, insert sz2 there. (check for null merge string */
    if (lpszMerge) {
	while (*lpszMerge)
	    *lpchDst++ = *lpszMerge++;

    }

    /* Jump over merge spec */
    lpchSrc++,lpchSrc++;


    /* Now append rest of Src String */
    while (*lpchDst++ = *lpchSrc++);
    return TRUE;

}



MakeBlankCard()
    {
    CurCardHead.line[0] = 0;
    CurCard.hBitmap = 0;
    SetEditText((LPSTR)"");
    CurCardHead.flags = FNEW;
    }

SetCaption()
    {
    char buf[40];
    BuildCaption(buf);
    SetWindowText(hIndexWnd, (LPSTR)buf);
    }

FAR BuildCaption(pchBuf)
char *pchBuf;
    {
    char *pch;
    char *pch2;
    char *pch3;
    char *pchtemp;

    lstrcpy((LPSTR)pchBuf, (LPSTR)rgchCardfile);
    lstrcat((LPSTR)pchBuf, (LPSTR)" - ");
    if (CurIFile[0])
	{
	pchtemp = pchBuf + lstrlen((LPSTR)pchBuf);
	pch = CurIFile;
	pch3 = pch;
	for ( ; *pch; ++pch)
	    ;
	while (pch > pch3 && *pch != '\\')
	    pch--;
	if (*pch == '\\')
	    pch++;

	lstrcat((LPSTR)pchBuf, (LPSTR)pch);

        /* filename is still in oem char set. 23-Oct-1987. davidhab. */
	/* only convert the filename portion. (Fri 11-Dec-1987 : bobgu) */
	OemToAnsi((LPSTR)pchtemp, (LPSTR)pchtemp);
	}
    else
	lstrcat((LPSTR)pchBuf, (LPSTR)rgchUntitled);
    }

FAR IndexWinIniChange()
    {
    char buf[3];
    HMENU hMenu;

    hMenu = GetMenu(hIndexWnd);
    if (!GetProfileString((LPSTR)rgchWindows, (LPSTR)rgchDevice, (LPSTR)"", buf, 2))
	{
	fCanPrint = FALSE;
	EnableMenuItem(hMenu, PRINT, MF_GRAYED);
	EnableMenuItem(hMenu, PRINTALL, MF_GRAYED);
	}
    else
	{
	fCanPrint = TRUE;
	EnableMenuItem(hMenu, PRINT, MF_ENABLED);
	EnableMenuItem(hMenu, PRINTALL, MF_ENABLED);
	}
    }
