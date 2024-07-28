#include "dbwin.h"

// DbgOptDlg functions

int     DbgOptDlg_Do(HWND hwndOwner);

BOOL CALLBACK _export DbgOptDlg_OldDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT DbgOptDlg_DefProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT DbgOptDlg_DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL    DbgOptDlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
VOID    DbgOptDlg_OnCommand(HWND hwnd, UINT id, HWND hwndCtl, UINT codeNotify);

void    DbgOptDlg_OutputOptions(HWND hwndDlg, UINT option, UINT filter, LPCSTR lpszTaskFilter);
void    DbgOptDlg_InputOptions(HWND hwndDlg, UINT FAR* poptions, UINT FAR* pfilter, LPSTR lpszTaskFilter);

//---------------------------------------------------------------------------

BOOL fOptionsSaved = FALSE;
UINT DebugOptionsSave = 0;
UINT DebugFilterSave = 0;
char szTaskFilter[MAX_MODULE_NAME];

static WINDEBUGINFO wdi;

typedef struct
{
    UINT idCtl;
    UINT options;
    UINT filter;
} DBOITEM;

//---------------------------------------------------------------------------

void DoDebugOptions(HWND hwndOwner)
{
    DLGPROC pfndp;
    extern HINSTANCE hinstDBWin;

    pfndp = (DLGPROC)MakeProcInstance((FARPROC)DbgOptDlg_OldDlgProc, hinstDBWin);

    DialogBoxParam(hinstDBWin, MAKEINTRESOURCE(IDR_SETDBDLG), hwndOwner, pfndp, 0L);

    FreeProcInstance((FARPROC)pfndp);
}

void DoSaveOptions(HWND hwndOwner)
{
    char ach[10];

    wsprintf(ach, "0x%04x", (UINT)wdi.dwOptions);
    WriteProfileString("Windows", "DebugOptions", (LPSTR)ach);

    wsprintf(ach, "0x%04x", (UINT)wdi.dwFilter);
    WriteProfileString("Windows", "DebugFilter", (LPSTR)ach);

    // Save out the task filter now (even though the system doesn't
    // read this option)
    //
    WriteProfileString("Windows", "DebugTaskFilter", (LPSTR)szTaskFilter);
}

static BOOL fDefDlgEx = FALSE;

BOOL CALLBACK _export DbgOptDlg_OldDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    CheckDefDlgRecursion(&fDefDlgEx);

    return SetDlgMsgResult(hwndDlg, msg, DbgOptDlg_DlgProc(hwndDlg, msg, wParam, lParam));
}

LRESULT DbgOptDlg_DefProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return DefDlgProcEx(hwndDlg, msg, wParam, lParam, &fDefDlgEx);
}

LRESULT DbgOptDlg_DlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        HANDLE_MSG(hwndDlg, WM_INITDIALOG, DbgOptDlg_OnInitDialog);
        HANDLE_MSG(hwndDlg, WM_COMMAND, DbgOptDlg_OnCommand);
    default:
        return DbgOptDlg_DefProc(hwndDlg, msg, wParam, lParam);
    }
}

BOOL DbgOptDlg_OnInitDialog(HWND hwndDlg, HWND hwndFocus, LPARAM lParam)
{
    if (!GetWinDebugInfo(&wdi, WDI_OPTIONS | WDI_FILTER))
        return FALSE;

    if (wdi.flags != (WDI_OPTIONS | WDI_FILTER))
        return FALSE;

    // Initialize the saved options/filter the first time the dialog
    // is brought up: these are used for restore later.
    //
    if (!fOptionsSaved)
    {
        fOptionsSaved = TRUE;
        DebugOptionsSave = (UINT)wdi.dwOptions;
        DebugFilterSave = (UINT)wdi.dwFilter;

        // Read the DebugTaskFilter() from win.ini
        // NOTE: Windows doesn't pay attention to this key -- only dbwin does.
        //
        GetProfileString("Windows", "DebugTaskFilter", "", szTaskFilter, sizeof(szTaskFilter));
        SetTaskFilter(szTaskFilter);
    }

    GetTaskFilter(szTaskFilter, sizeof(szTaskFilter));

    // Set up dialog to reflect current options
    //
    DbgOptDlg_OutputOptions(hwndDlg, (UINT)wdi.dwOptions, (UINT)wdi.dwFilter, szTaskFilter);

    return TRUE;
}

VOID DbgOptDlg_OnCommand(HWND hwndDlg, UINT id, HWND hwndCtl, UINT code)
{
    switch (id)
    {
    case IDOK:	// CTL_OK
        {
            UINT options;
            UINT filter;

            DbgOptDlg_InputOptions(hwndDlg, &options, &filter, szTaskFilter);

            SetTaskFilter(szTaskFilter);

            wdi.dwOptions = options;
            wdi.dwFilter = filter;
            wdi.flags = WDI_OPTIONS | WDI_FILTER;
            SetWinDebugInfo(&wdi);
        }

	// fall through...

    case IDCANCEL:	// CTL_CANCEL
        EndDialog(hwndDlg, id);
	break;

    case CTL_DEFAULTS:
        if (code == BN_CLICKED)
        {
            wdi.dwOptions = DebugOptionsSave;
            wdi.dwFilter = DebugFilterSave;
            wdi.flags = WDI_OPTIONS | WDI_FILTER;
            SetWinDebugInfo(&wdi);

            SetTaskFilter("");

            DbgOptDlg_OutputOptions(hwndDlg, (UINT)wdi.dwOptions, (UINT)wdi.dwFilter, szTaskFilter);
	}
	break;
    }
}

DBOITEM adbo[] =
{
    { CTL_CHECKHEAP, DBO_CHECKHEAP, 0 },
    { CTL_CHECKFREE, DBO_CHECKFREE, 0 },
    { CTL_FILLBUFFER, DBO_BUFFERFILL, 0 },
    { CTL_DISABLEGPTRAPPING, DBO_DISABLEGPTRAPPING, 0 },

    { CTL_SILENT, DBO_SILENT, 0 },

    { CTL_TRACEBREAK, DBO_TRACEBREAK, 0 },
    { CTL_WARNINGBREAK, DBO_WARNINGBREAK, 0 },
    { CTL_NOERRORBREAK, DBO_NOERRORBREAK, 0 },
    { CTL_NOFATALBREAK, DBO_NOFATALBREAK, 0 },
    { CTL_INT3, DBO_INT3BREAK, 0 },

    { CTL_KERNEL, 0, DBF_KERNEL },
    { CTL_MEMMAN, 0, DBF_KRN_MEMMAN },
    { CTL_LOADMODULE, 0, DBF_KRN_LOADMODULE },
    { CTL_LOADSEGMENT, 0, DBF_KRN_SEGMENTLOAD },
    { CTL_USER, 0, DBF_USER },
    { CTL_GDI, 0, DBF_GDI },
    { CTL_MMSYSTEM, 0, DBF_MMSYSTEM },
    { CTL_PENWIN, 0, DBF_PENWIN },
    { CTL_DRIVER, 0, DBF_DRIVER },
    { CTL_APPLICATION, 0, DBF_APPLICATION }
};

void DbgOptDlg_InputOptions(HWND hwndDlg, UINT FAR* poptions, UINT FAR* pfilter, LPSTR lpszFilter)
{
    DBOITEM* pdbo;
    UINT options = 0;
    UINT filter = 0;

    // Get task filter
    //
    Edit_GetText(GetDlgItem(hwndDlg, CTL_APPNAME), lpszFilter, MAX_MODULE_NAME);

    for (pdbo = adbo; pdbo != &adbo[sizeof(adbo) / sizeof(DBOITEM)]; pdbo++)
    {
        HWND hwndButton = GetDlgItem(hwndDlg, pdbo->idCtl);

        if (Button_GetCheck(hwndButton))
        {
            options |= pdbo->options;
            filter |= pdbo->filter;
        }
        else
        {
            options &= ~pdbo->options;
            filter &= ~pdbo->filter;
        }
    }
    *poptions = options;
    *pfilter = filter;
}

void DbgOptDlg_OutputOptions(HWND hwndDlg, UINT option, UINT filter, LPCSTR lpszFilter)
{
    DBOITEM* pdbo;

    Edit_SetText(GetDlgItem(hwndDlg, CTL_APPNAME), lpszFilter);

    for (pdbo = adbo; pdbo != &adbo[sizeof(adbo) / sizeof(DBOITEM)]; pdbo++)
    {
        HWND hwndButton = GetDlgItem(hwndDlg, pdbo->idCtl);

        Button_SetCheck(hwndButton,
                ((pdbo->options & option) || (pdbo->filter & filter)) );
    }
}
