/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dinstall.c
 *  Content:    Game SDK sample setup program
 ***************************************************************************/
 
#include <windows.h>
#include <shellapi.h>   // for SHFileOperation
#include <shlobj.h>     // for SHBroweForFolder
#include "dsetup.h"
#include "dinstall.h"

/*
 * list of files that will be copied from the source directory to
 * to the directory the game is created in
 */
static char* copy_list [] =
{
    "FOXBEAR.EXE",
    "FOXBEAR.ART"
};

/*
 * default directory to install game in
 */
static char DefaultGameDirectory [] = "C:\\DirectX Games\\FoxBear";

/*
 * title of game installation
 */
static char title [] = "The Fox and the Bear Setup";

/*
 * title of program group
 */
static char group_name[] = "DirectX Games";
static char link_name[]  = "FoxBear";

/*
 * prototypes
 */
BOOL FAR PASCAL masterDlgProc( HWND hdlg,DWORD message,DWORD wparam,DWORD lparam );

/*
 * globals
 */
static HANDLE   hinst;
static char     GameDirectory[MAX_PATH];    // where the user wants the game
static char     SetupDirectory[MAX_PATH];     // where the user ran setup from

/*
 * support functions
 */
void catpath(char *dst, char *src)
{
    int len = lstrlen(dst);
    if (len > 0 && (dst[len-1] != '\\' && dst[len-1] != '/'))
        lstrcat(dst,"\\");
    lstrcat(dst,src);

    // SHFileOperation needs a double null string.
    len = lstrlen(dst);
    dst[len+1] = 0;
}

/*
 * set a bitmap into a static control
 */
void SetBitmap(HWND hDlg, int id, char *szBitmap, int w, int h)
{
    HBITMAP hbm;
    HWND hwnd;

    hwnd = GetDlgItem(hDlg, id);

    if (hwnd == NULL)
        return;

    hbm = (HBITMAP)LoadImage(hinst, szBitmap, IMAGE_BITMAP, w, h,
        LR_LOADTRANSPARENT | LR_LOADMAP3DCOLORS | LR_CREATEDIBSECTION);

    if (hbm)
        hbm = (HBITMAP)SendMessage(hwnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbm);

    if (hbm)
        DeleteObject(hbm);
}

void SetInfoText(HWND hDlg, char *sz, ...)
{
    char ach[128];
    wvsprintf(ach, sz, (void*)(&sz+1));
    SetDlgItemText(hDlg, IDC_INFO, ach);
}

void _SHFree(void *p)
{
    IMalloc *pm;
    SHGetMalloc(&pm);
    if (pm)
    {
        pm->lpVtbl->Free(pm,p);
        pm->lpVtbl->Release(pm);
    }
}

/*
 * build a shortcut in the start menu
 */
void MakeShortcut()
{
    char buf[512];
    char szSetupIni[MAX_PATH];
    char szExeFile[MAX_PATH];
    int len;
    int fh;

    static char setup_ini[] =
        "[progman.groups]\r\n"
        "groupX=%s\r\n"
        "[groupX]\r\n"
        "\"%s\",\"%s\",,,,\"%s\"\r\n";

    GetWindowsDirectory(szSetupIni, sizeof(szSetupIni));
    catpath(szSetupIni, "SETUP.INI");

    lstrcpy(buf, GameDirectory);
    catpath(buf, copy_list[0]);
    GetShortPathName(buf, szExeFile, sizeof(szExeFile));

//  lstrcpy(buf, GameDirectory);
//  GetShortPathName(buf, szWork, sizeof(szWork));

    len = wsprintf(buf, setup_ini, group_name, link_name, szExeFile, GameDirectory);

    fh = _lcreat(szSetupIni, 0);

    if (fh != -1)
    {
        _lwrite(fh, buf, len);
        _lclose(fh);
        WinExec("grpconv -o", SW_HIDE);
    }
}

/*
 * dlg proc for wizard dialog box, the setup is controlled from here.
 */
BOOL FAR PASCAL masterDlgProc(HWND hDlg,DWORD dwMessage,DWORD wParam,DWORD lParam)
{
    int         result;
    static int  system_restart;
    static int  current_dialog;
    static int  busy;

    char        src[MAX_PATH];
    char        dst[MAX_PATH];
    SHFILEOPSTRUCT fileop;

    switch(dwMessage)
    {
    case WM_INITDIALOG:
        busy = 0;
        current_dialog = 0;
        SetWindowText(hDlg, title);
        EnableWindow( GetDlgItem(hDlg, IDC_B), FALSE );
        EnableWindow( GetDlgItem(hDlg, IDC_H), FALSE );

        /*
         * set the fox bitmap into our static control
         */
        SetBitmap(hDlg, IDC_STATIC, "fox", 175, 195);

        /*
         * limit the size of the input of this text field to the length of a path
         * put the default directory to install the game into in it
         * select the whole thing to make it easy for people to replace it
         * set the focus to it
         */
        SendDlgItemMessage( hDlg, IDC_EDIT, EM_LIMITTEXT, MAX_PATH, 0L);
        SetDlgItemText( hDlg, IDC_EDIT, DefaultGameDirectory );
        SendDlgItemMessage( hDlg, IDC_EDIT, EM_SETSEL, 0, MAKELONG(256, 256) );
        SetFocus( GetDlgItem(hDlg, IDC_EDIT) );
        /*
         * return 0 here indicating we have set the focus for the dialog box
         * and it doesn't need to help us
         */
        return 0;

    case WM_SETCURSOR:
        if (busy)
        {
            SetCursor(LoadCursor(NULL, IDC_WAIT));
            return TRUE;
        }
        break;

    case WM_COMMAND:
        switch(wParam)
        {
        case IDOK:
            if( busy > 0 )
            {
                /*
                 * busy bit keeps us from taking input while we are off doing
                 * things that can create other dialog boxes and end up causing
                 * us to be reentered.
                 */
                break;
            }
            else if( current_dialog == 0 )
            {
                int     i;

                busy++;
                EnableWindow(GetDlgItem(hDlg,IDOK), FALSE);
                EnableWindow(GetDlgItem(hDlg,IDCANCEL), FALSE);
                SetCursor(LoadCursor(NULL, IDC_WAIT));

                /*
                 * get the directory the user typed
                 */
                GetWindowText( GetDlgItem( hDlg,IDC_EDIT ), GameDirectory, sizeof(GameDirectory));

                /*
                 * verify that the typed in directory is valid
                 * by having the SHELL copy WIN.INI to this directory
                 * it will also create the directory for us.
                 */
                SetInfoText(hDlg, "Creating directory..");

                GetWindowsDirectory(src, sizeof(src));
                catpath(src,"WIN.INI");

                lstrcpy(dst,GameDirectory);
                catpath(dst,"SMAG.INI");

                fileop.hwnd     = hDlg;
                fileop.wFunc    = FO_COPY;
                fileop.pFrom    = src;
                fileop.pTo      = dst;
                fileop.fFlags   = FOF_SILENT | FOF_NOCONFIRMATION;

                if (SHFileOperation(&fileop) != 0)
                {
                    // failed, the shell gave the user a error.
                    SetInfoText(hDlg, "");
                    EnableWindow(GetDlgItem(hDlg,IDOK), TRUE);
                    EnableWindow(GetDlgItem(hDlg,IDCANCEL), TRUE);
                    busy--;
                    break;
                }

                /*
                 * the directory is valid now delete the bogus file
                 */
                fileop.hwnd     = hDlg;
                fileop.wFunc    = FO_DELETE;
                fileop.pFrom    = dst;
                fileop.pTo      = NULL;
                fileop.fFlags   = FOF_SILENT | FOF_NOCONFIRMATION;

                SHFileOperation(&fileop);
                SetInfoText(hDlg, "");

                /*
                 * check if there is enough space to install the game
                 * NOTE: there is always enough space at the moment :-)
                 */
                SetInfoText(hDlg, "Checking disk space.");
                if( 0 )
                {
                }
                SetInfoText(hDlg, "");

                /*
                 * now setup DirectX
                 */
                SetInfoText(hDlg, "Installing DirectX.");
                result = DirectXSetup( hDlg, NULL, DSETUP_DIRECTX );
                SetInfoText(hDlg, "");

                if( result < 0 )
                {
                    MessageBox( hDlg, "DirectX failed to install. The game was not installed.", title, 0 );
                    EndDialog(hDlg, result);
                    break;
                }

                /*
                 * check if there is enough space to install the game
                 * NOTE: there is always enough space at the moment :-)
                 */
                SetInfoText(hDlg, "Checking disk space.");
                if( 0 )
                {
                }
                SetInfoText(hDlg, "");

                /*
                 * now copy the files.
                 */
                system_restart = result;

                SetInfoText(hDlg, "Copying files.");
                for( i = 0; i < sizeof( copy_list )/sizeof( copy_list[0] ); i++ )
                {
                    lstrcpy( src, SetupDirectory );
                    catpath( src, copy_list[i] );

                    lstrcpy( dst, GameDirectory );
                    catpath( dst, copy_list[i] );

                    SetInfoText(hDlg, "Copying %s", copy_list[i]);

                    fileop.hwnd     = hDlg;
                    fileop.wFunc    = FO_COPY;
                    fileop.pFrom    = src;
                    fileop.pTo      = dst;
                    fileop.fFlags   = FOF_SILENT | FOF_NOCONFIRMATION;

                    while (result = SHFileOperation(&fileop))
                    {
                        char errorTemplate[] = "Setup Failure: %s could not be copied.";
                        char errorText[MAX_PATH+sizeof(errorTemplate)];

                        wsprintf(errorText, errorTemplate, copy_list[i] );
                        result = MessageBox( hDlg, errorText, title, MB_RETRYCANCEL );

                        if( result == IDCANCEL )
                        {
                            result = -1;
                            break;
                        }
                    }

                    if( result == 0 )
                    {
                        SetFileAttributes( dst, FILE_ATTRIBUTE_NORMAL );
                    }
                }
                SetInfoText(hDlg, "");

                SetInfoText(hDlg, "Creating StartMenu shortcut");
                MakeShortcut();
                SetInfoText(hDlg, "");

                if( result >= 0 )
                {
                    /*
                     * hide current controls
                     */
                    ShowWindow( GetDlgItem(hDlg, IDC_EDIT), SW_HIDE );
                    ShowWindow( GetDlgItem(hDlg, IDC_DIRECTIONS1), SW_HIDE );
                    ShowWindow( GetDlgItem(hDlg, IDC_DIRECTIONS2), SW_HIDE );
                    ShowWindow( GetDlgItem(hDlg, IDC_EDITTEXT), SW_HIDE );
                    ShowWindow( GetDlgItem(hDlg, IDC_INFO), SW_HIDE );
                    ShowWindow( GetDlgItem(hDlg, IDC_BROWSE), SW_HIDE );

                    if( system_restart )
                    {
                        /*
                         * show new dialogs
                         */
                        ShowWindow( GetDlgItem(hDlg, IDC_REBOOT1), SW_SHOW );
                        ShowWindow( GetDlgItem(hDlg, IDC_REBOOT2), SW_SHOW );
                        SetWindowText(GetDlgItem(hDlg, IDOK), "&Reboot");

                        /*
                         * set the bear bitmap into our static control
                         */
                        SetBitmap(hDlg, IDC_STATIC, "bear", 270, 195);
                        current_dialog++;
                    }
                    else
                    {
                        ShowWindow( GetDlgItem(hDlg, IDC_SUCCESS), SW_SHOW );
                        SetWindowText( GetDlgItem(hDlg, IDOK), "&Finish" );
                        current_dialog++;
                        EnableWindow(GetDlgItem(hDlg,IDOK), TRUE);
                        busy--;
                        break;
                    }
                }

                EnableWindow(GetDlgItem(hDlg,IDOK), TRUE);
                EnableWindow(GetDlgItem(hDlg,IDCANCEL), TRUE);
                busy--;

                if( result < 0 )
                {
                    EndDialog( hDlg, result );
                }
            }
            else if (current_dialog == 1)
            {
                /*
                 * restart windows, kill apps that aren't responding, reboot
                 */
                if( system_restart )
                {
                    ExitWindowsEx( EWX_REBOOT, 0 );
                }
                else
                {
                    EndDialog( hDlg, 0 );
                }
            }
            break;

        case IDCANCEL:
            if( !busy )
            {
                /*
                 * only allow cancel if we aren't doing anything else
                 */
                EndDialog( hDlg, -1 );
            }
            break;

        case IDC_BROWSE:
            if( current_dialog == 0 )
            {
                BROWSEINFO bi;
                LPITEMIDLIST pidl;
                char ach[MAX_PATH];

                bi.hwndOwner      = hDlg;
                bi.pidlRoot       = NULL;
                bi.pszDisplayName = ach;
                bi.lpszTitle      = NULL;
                bi.ulFlags        = BIF_RETURNONLYFSDIRS;
                bi.lpfn           = NULL;
                bi.lParam         = 0;
                bi.iImage         = 0;

                pidl = SHBrowseForFolder(&bi);

                if (pidl)
                {
                    SHGetPathFromIDList(pidl, ach);
                    SetDlgItemText(hDlg, IDC_EDIT, ach);
                    _SHFree(pidl);
                }
            }
            break;
        }
    }
    return 0;
}

/* **************************************************************** */
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
    TCHAR * p;
    TCHAR * x;
    hinst = hInstance;

    /*
     * get our fullpath name and strip the file name
     */
    GetModuleFileName(hInstance, SetupDirectory, sizeof(SetupDirectory));

    for (x=p=SetupDirectory; *p; p=AnsiNext(p))
    {
        if (*p == '\\' || *p == '/')
            x = p;
    }
    *x = 0;

    /*
     * do the setup thing, it is all one big dialog box that you show
     * and hide things from depending on the screen
     * we just sign on, ask where to install, and install
     */
    DialogBox( hInstance, "DLG_MASTER", NULL, masterDlgProc );

    return 0;
} /* WinMain */
