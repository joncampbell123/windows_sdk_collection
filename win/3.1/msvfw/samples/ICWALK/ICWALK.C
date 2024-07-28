/*----------------------------------------------------------------------------*\
|   ICWalk.c - Walks all ICM compressors                                       |
\*----------------------------------------------------------------------------*/
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/

#include <windows.h>
#include <windowsx.h>

#define NOAVIFILE
#include <vfw.h>
#include "icwalk.dlg"

#define INDENT	5

static DWORD fccWalk = 0;
static int aiTabs[] = {40, 100, 220};

static ICINFO icinfo;
static nStuff = 0;
static char ach[512];

int CountStuff()
{
    int i;

    for (i=0; ICInfo(fccWalk, i, &icinfo); i++)
        ;

    return i;
}

void WalkStuff(HWND lb)
{
    char *pch;
    int i;
    HCURSOR hcur;

    hcur = SetCursor(LoadCursor(NULL, IDC_WAIT));

    SetWindowFont(lb, GetStockObject(ANSI_FIXED_FONT), FALSE);
    ListBox_ResetContent(lb);

    SendMessage(lb, LB_SETTABSTOPS, sizeof(aiTabs) / sizeof(int), (LPARAM)(LPINT)aiTabs);

    //
    // call ICInfo to enumerate compressors.
    //
    // we use 0 as the fccType to walk all compressors
    // of any type.
    //
    for (i=0; ICInfo(fccWalk, i, &icinfo); i++)
    {
        HIC hic;

        pch = ach;

        if (icinfo.fccHandler > 256)
            pch += wsprintf(pch, "%4.4s.%4.4s", (LPSTR)&icinfo.fccType, (LPSTR)&icinfo.fccHandler);
        else
            pch += wsprintf(pch, "%4.4s.%04d", (LPSTR)&icinfo.fccType, icinfo.fccHandler);

        hic = ICOpen(icinfo.fccType, icinfo.fccHandler, ICMODE_QUERY);

        //
        // find out more info (like the compressor name).
        //
        if (hic)
        {
            ICGetInfo(hic, &icinfo, sizeof(icinfo));
            ICClose(hic);
        }
	else
	{
	    lstrcpy(icinfo.szDescription, "<Can't Load>");
	}

        pch += wsprintf(pch, " %s\t%s\t%s",
            (LPSTR)icinfo.szName,
            (LPSTR)icinfo.szDescription,
            (LPSTR)icinfo.szDriver);

        ListBox_AddString(lb,ach);
    }

    nStuff = i;
}

BOOL CALLBACK ICWalkDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HWND lb;
    RECT rc;

    switch (msg)
    {
        case WM_INITDIALOG:
            //
            // if ICInfo does not support walking all types, then just
            // walk video types
            //
            if (!ICInfo(0, 0, &icinfo))
                fccWalk = ICTYPE_VIDEO;

            return TRUE;

        case WM_COMMAND:
            switch (wParam)
            {
                case ID_LISTBOX:
                    break;

                case IDOK:
                case IDCANCEL:
                    EndDialog(hwnd, 0);
                    break;
            }
            break;

        case WM_ACTIVATE:
            if (wParam && nStuff != CountStuff())
            {
                lb = GetDlgItem(hwnd, ID_LISTBOX);
                WalkStuff(lb);
            }

	case WM_SIZE:
            lb = GetDlgItem(hwnd, ID_LISTBOX);
	    GetClientRect(hwnd, &rc);
	    MoveWindow(lb, INDENT, INDENT, rc.right - 2 * INDENT,
		rc.bottom - 2 * INDENT, TRUE);
	    break;
    }

    return FALSE;
}

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int sw)
{
    DialogBox(hInstance,"ICWalkDialog",NULL,(DLGPROC)ICWalkDlgProc);
    return 0;
}
