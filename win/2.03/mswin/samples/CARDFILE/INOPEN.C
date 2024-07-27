#include "index.h"

/****************************************************************/
/*								*/
/*  Windows Cardfile - Written by Mark Cliggett 		*/
/*  (c) Copyright Microsoft Corp. 1985 - All Rights Reserved	*/
/*								*/
/****************************************************************/

extern HWND hCardWnd;
extern HANDLE hEditCurs;
extern CATCHBUF CatchBuf;

DoMerge()
    {
    char *pchBuf;
    int t;

    if(!fReadOnly && (pchBuf = (char *)PutUpDB(DTMERGE)))
	{
	{
	OFSTRUCT ofStruct;

	t = OpenFile((LPSTR)pchBuf, (LPOFSTRUCT)&ofStruct, OF_PARSE);
	}
	if (t)
	    {
	    IndexOkError(EINVALIDFILE);
	    LocalFree((HANDLE)pchBuf);
	    return;
	    }
	SetCursor(hWaitCurs);
	if (CardPhone == PHONEBOOK || SaveCurrentCard(iFirstCard))
	    {
	    if(MergeCardFile(pchBuf))
		{
		iTopCard = iFirstCard = 0;
		SetScrRangeAndPos();
		if (CardPhone == CCARDFILE)
		    SetCurCard(iFirstCard);
		InvalidateRect(hIndexWnd, (LPRECT)NULL, TRUE);
		}
	    }
	LocalFree((HANDLE)pchBuf);
	SetCursor(hArrowCurs);
	}
    }

FAR DoOpen(pchBuf)
char *pchBuf;
    {
    int result = FALSE;
    int t;

    {
    OFSTRUCT ofStruct;

    t = OpenFile((LPSTR)pchBuf, (LPOFSTRUCT)&ofStruct, OF_PARSE);
    }
    if (t)
	{
	IndexOkError(EINVALIDFILE);
#if 0
/* this will be freed under OPEN in ininput.c!! */
	LocalFree((HANDLE)pchBuf);
#endif
	return(0);
	}
    SetCursor(hWaitCurs);
    if(ReadCardFile(pchBuf))
	{
	SetCaption();
        Fdelete(TempFile);
        MakeTempFile(hIndexInstance);
	iTopCard = iFirstCard = 0;
	SetScrRangeAndPos();
	if (CardPhone == CCARDFILE);
	    SetCurCard(iFirstCard);
	CurCardHead.flags = 0;
	InvalidateRect(hIndexWnd, (LPRECT)NULL, TRUE);
	result = TRUE;
	}
    SetCursor(hArrowCurs);
    return(result);
    }

MaybeSaveFile(fSystemModal)
int fSystemModal;
    {
    char buf[60];
    char buf2[128];
    char rgchAnsi[128];
    char *pchBuf;
    char *pchFile;
    int result;
    char *pch;
    char *pch2;
    int fh;
    int t;
    OFSTRUCT ofStruct;

/* put up a message box that says "Do you wish to save your edits?" */
/* if so, save 'em */
/* if returns FALSE, means it couldn't save, and whatever is happening */
/* should not continue */
    if (fFileDirty || CurCardHead.flags & FDIRTY || SendMessage(hCardWnd, EM_GETMODIFY, 0, 0L))
	{
	LoadString(hIndexInstance, IOKTOSAVE, (LPSTR)buf, 60);

        /* must have current file in ansi char set. 28-Oct-1987. */
        OemToAnsi((LPSTR)CurIFile, (LPSTR)rgchAnsi);

        if (rgchAnsi[0])
	    {
            for (pch = rgchAnsi ; *pch; ++pch)
		;
            while (pch > rgchAnsi && *pch != '\\')
		pch--;
	    if (*pch == '\\')
		pch++;

	    AnsiUpper((LPSTR)pch);
	    }
	else
	    pch = rgchUntitled;
	MergeStrings((LPSTR)buf, (LPSTR)pch, (LPSTR)buf2);

	result = MessageBox(hIndexWnd, (LPSTR)buf2, (LPSTR)rgchNote, MB_YESNOCANCEL | MB_ICONQUESTION | MB_APPLMODAL);

	if (result == IDYES)
	    {
	    if (SaveCurrentCard(iFirstCard))
		{
		if (!CurIFile[0])
		    {
SaveGetName:
		    if(pchBuf = PutUpDB(DTSAVE))
			{
			t = OpenFile((LPSTR)pchBuf, (LPOFSTRUCT)&ofStruct, OF_PARSE);
			pchFile = ofStruct.szPathName;
			if (t)
			    {
			    IndexOkError(EINVALIDFILE);
			    LocalFree((HANDLE)pchBuf);
			    goto SaveGetName;
			    }
                        if ((fh = _lopen((LPSTR)pchFile, READ)) > -1)
			    {
			    _lclose(fh);
			    LoadString(hIndexInstance, EFILEEXISTS, (LPSTR)buf, 60);
			    AnsiUpper((LPSTR)pchBuf);
			    MergeStrings((LPSTR)buf, (LPSTR)pchBuf, (LPSTR)buf2);
			    if (MessageBox(hIndexWnd, (LPSTR)buf2, (LPSTR)rgchWarning, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDNO)
				{
				LocalFree((HANDLE)pchBuf);
				goto SaveGetName;
				}
			    }
			LocalFree((HANDLE)pchBuf);
			}
		    else
			{
			SetCurCard(iFirstCard);
			return(FALSE);	    /* cancelled */
			}
		    }
		else
                    {
                    lstrcpy((LPSTR)buf2, (LPSTR)CurIFile);
                    pchFile = buf2;
                    }

		/* save file, if can't save don't continue */
		if (!WriteCardFile(pchFile))
                    goto SaveGetName;
		}
	    else
		return(FALSE);
	    }
	else if (result == IDCANCEL)
	    return(FALSE);
	else if (CurCard.hBitmap)
	    {
	    DeleteObject(CurCard.hBitmap);
	    CurCard.hBitmap = 0;
	    }
	}
    else if (CurCard.hBitmap)
	{
	DeleteObject(CurCard.hBitmap);
	CurCard.hBitmap = 0;
	}

    return(TRUE);
    }


IndexConfirm(strid, pchCaption)
int strid;
char *pchCaption;
    {
    char buf[128];

    LoadString(hIndexInstance, strid, (LPSTR)buf, 128);
    return(MessageBox(hIndexWnd, (LPSTR)buf, (LPSTR)pchCaption, MB_OKCANCEL | MB_ICONQUESTION) == IDOK);
    }
