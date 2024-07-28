#include "dbwin.h"

// AllocBrkDlg functions

int     AllocBrkDlg_Do(HWND hwndOwner);

BOOL CALLBACK _export AllocBrkDlg_OldDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT AllocBrkDlg_DefProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT AllocBrkDlg_DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL    AllocBrkDlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
VOID    AllocBrkDlg_OnCommand(HWND hwnd, UINT id, HWND hwndCtl, UINT codeNotify);

void    AllocBrkDlg_WriteOptions(HWND hwndDlg, UINT option, UINT filter);
void    AllocBrkDlg_ReadOptions(HWND hwndDlg, UINT FAR* poptions, UINT FAR* pfilter);

//---------------------------------------------------------------------------

static WINDEBUGINFO wdi;

//---------------------------------------------------------------------------

void DoAllocBrk(HWND hwndOwner)
{
    DLGPROC pfndp;
    extern HINSTANCE hinstDBWin;

    pfndp = (DLGPROC)MakeProcInstance((FARPROC)AllocBrkDlg_OldDlgProc, hinstDBWin);

    DialogBoxParam(hinstDBWin, MAKEINTRESOURCE(IDR_ALLOCBRKDLG), hwndOwner, pfndp, 0L);

    FreeProcInstance((FARPROC)pfndp);
}

static BOOL fDefDlgEx = FALSE;

BOOL CALLBACK _export AllocBrkDlg_OldDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    CheckDefDlgRecursion(&fDefDlgEx);

    return SetDlgMsgResult(hwndDlg, msg, AllocBrkDlg_DlgProc(hwndDlg, msg, wParam, lParam));
}

LRESULT AllocBrkDlg_DefProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return DefDlgProcEx(hwndDlg, msg, wParam, lParam, &fDefDlgEx);
}

LRESULT AllocBrkDlg_DlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        HANDLE_MSG(hwndDlg, WM_INITDIALOG, AllocBrkDlg_OnInitDialog);
        HANDLE_MSG(hwndDlg, WM_COMMAND, AllocBrkDlg_OnCommand);
    default:
        return AllocBrkDlg_DefProc(hwndDlg, msg, wParam, lParam);
    }
}

BOOL AllocBrkDlg_OnInitDialog(HWND hwndDlg, HWND hwndFocus, LPARAM lParam)
{
    HWND hwndModule = GetDlgItem(hwndDlg, CTL_MODULE);
    HWND hwndBrkCount = GetDlgItem(hwndDlg, CTL_BRKCOUNT);
    HWND hwndAllocCount = GetDlgItem(hwndDlg, CTL_ALLOCCOUNT);
    char ach[32];
    int i;

    GetWinDebugInfo(&wdi, WDI_ALLOCBREAK);
    for (i = 0; i != 8; i++)
    {
        ach[i] = wdi.achAllocModule[i];
        if (ach[i] == 0)
            break;
    }
    ach[8] = 0;
    Edit_SetText(hwndModule, ach);

    wsprintf(ach, "%ld", wdi.dwAllocBreak);
    Edit_SetText(hwndBrkCount, ach);

    wsprintf(ach, "%ld", wdi.dwAllocCount);
    Edit_SetText(hwndAllocCount, ach);

    return TRUE;
}

DWORD DwordFromString(LPSTR lpsz);
void AllocBrkDlg_Set(HWND hwndDlg);

void AllocBrkDlg_OnCommand(HWND hwndDlg, UINT id, HWND hwndCtl, UINT code)
{
    HWND hwndModule = GetDlgItem(hwndDlg, CTL_MODULE);
    HWND hwndBrkCount = GetDlgItem(hwndDlg, CTL_BRKCOUNT);
    DWORD c;
    char ach[32];

    switch (id)
    {
    case CTL_CLEAR:
        Edit_SetText(hwndModule, NULL);
        Edit_SetText(hwndBrkCount, NULL);
        AllocBrkDlg_Set(hwndDlg);
        break;

    case CTL_INCSET:
        Edit_GetText(hwndBrkCount, ach, sizeof(ach));
        c = DwordFromString(ach) + 1;
        wsprintf(ach, "%ld", c);
        Edit_SetText(hwndBrkCount, ach);
        AllocBrkDlg_Set(hwndDlg);
        break;

    case CTL_OK:
        AllocBrkDlg_Set(hwndDlg);
        break;

    case CTL_CANCEL:
        EndDialog(hwndDlg, 0);
	break;

    case CTL_SHOWCOUNT:
        // Update the alloc break count
        //
        GetWinDebugInfo(&wdi, WDI_ALLOCBREAK);
        wsprintf(ach, "%ld", wdi.dwAllocCount);
        Edit_SetText(GetDlgItem(hwndDlg, CTL_ALLOCCOUNT), ach);
        break;
    }
}

void AllocBrkDlg_Set(HWND hwndDlg)
{
    HWND hwndModule = GetDlgItem(hwndDlg, CTL_MODULE);
    HWND hwndBrkCount = GetDlgItem(hwndDlg, CTL_BRKCOUNT);
    HWND hwndAllocCount = GetDlgItem(hwndDlg, CTL_ALLOCCOUNT);
    DWORD c;
    int i;
    char ach[32];

    Edit_GetText(hwndBrkCount, ach, sizeof(ach));
    c = DwordFromString(ach);

    Edit_GetText(hwndModule, ach, sizeof(ach));

    for (i = 0; i != 8; i++)
    {
        wdi.achAllocModule[i] = ach[i];
        if (ach[i] == 0)
            break;
    }

    wdi.flags = WDI_ALLOCBREAK;
    wdi.dwAllocBreak = c;
    wdi.dwAllocCount = 0;
    SetWinDebugInfo(&wdi);

    Edit_SetText(hwndAllocCount, "0");

    Edit_SetSel(hwndBrkCount, 0, 32767);
    SetFocus(hwndBrkCount);
}

DWORD DwordFromString(LPSTR lpsz)
{
    DWORD l = 0;
    char ch;

    while (ch = *lpsz++)
    {
        if (ch < '0' || ch > '9')
            break;
        l = (l * 10) + (ch - '0');
    }
    return l;
}
