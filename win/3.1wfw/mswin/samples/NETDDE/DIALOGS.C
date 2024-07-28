/*****************************************************************************\
*                                                                             *
* dialogs.c -  WFWRK Network DDE Share Manager dialog routines                *
*                                                                             *
*               Version 3.1                                                   *
*                                                                             *
*        Copyright (c) 1992-1993, Microsoft Corp.  All rights reserved.       *
*                                                                             *
\*****************************************************************************/

#define  STRICT
#include "windows.h"
#include "ddeshare.h"
#include "dialogs.h"
#include "nddeapi.h"

extern VOID ShowErrMsg ( HWND hwnd, char * s, UINT code );

/* these are just convenient abbreviations for our 2 classes of access */
#define        PERM_READ    (NDDEACCESS_REQUEST|NDDEACCESS_ADVISE)
#define        PERM_WRITE    (NDDEACCESS_REQUEST|NDDEACCESS_ADVISE|\
            NDDEACCESS_POKE|NDDEACCESS_EXECUTE)

/* this buffer size big enough unless there are additional items, in which
   case we will re-allocate */
#define        APIBUFSIZ    (sizeof(NDDESHAREINFO)+MAX_APPNAME+1+MAX_TOPICNAME+1+\
                        MAX_ITEMNAME+1+MAX_PASSWORD*2)

HANDLE hAPIbuf;                            /* our buffer for api calls */
LPNDDESHAREINFO lpDdeI;                    /* share info pointer to buffer */
char AppNameBuf[MAX_APPNAME+1];            /* app name buffer for new shares */
char TopicNameBuf[MAX_TOPICNAME+1];        /* topic name buffer for new shares */
char ItemNameBuf[MAX_ITEMNAME+1];        /* item name buffer for new shares */
char Password1[MAX_PASSWORD+1];            /* password 1 buffer for new shares */
char Password2[MAX_PASSWORD+1];            /* password 2 buffer for new shares */
DWORD Perm1;                            /* temp variable for permissions 1 */
DWORD Perm2;                            /* temp variable for permissions 2 */

/* add share / share properties dialog proc */

BOOL FAR PASCAL AddShareDlg(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    UINT ret;
    static BOOL fPropertiesCalled;        /* this variable remembers whether the
                                           dialog was invoked to create a new
                                           share or modify an existing one
                                           for the duration the dialog is up */

    switch (message) {

    case WM_INITDIALOG:
        /* limit our text windows to appropriate values */
        SendDlgItemMessage ( hDlg, IDC_SHARENAME, EM_LIMITTEXT,
            MAX_NDDESHARENAME, 0L );
        SendDlgItemMessage ( hDlg, IDC_APPNAME, EM_LIMITTEXT,
            MAX_APPNAME, 0L );
        SendDlgItemMessage ( hDlg, IDC_TOPICNAME, EM_LIMITTEXT,
            MAX_TOPICNAME, 0L );
        SendDlgItemMessage ( hDlg, IDC_ITEMNAME, EM_LIMITTEXT,
            MAX_ITEMNAME, 0L );
        SendDlgItemMessage ( hDlg, IDC_PASSWORD1, EM_LIMITTEXT,
            MAX_PASSWORD - 1, 0L );
        SendDlgItemMessage ( hDlg, IDC_PASSWORD2, EM_LIMITTEXT,
            MAX_PASSWORD - 1, 0L );

        /* initialize radio buttons (mainly to clear any tabstops) */
        SendDlgItemMessage (hDlg, IDC_DEPENDSON, BM_SETCHECK, FALSE, 0L);
        SendDlgItemMessage (hDlg, IDC_READONLY, BM_SETCHECK, FALSE, 0L);
        SendDlgItemMessage (hDlg, IDC_FULLACCESS, BM_SETCHECK, FALSE, 0L);

        /* allocate our inital best-guess size buffer */
        if ( !( hAPIbuf = GlobalAlloc ( GMEM_ZEROINIT, APIBUFSIZ )))
            return FALSE;

        if ( !( lpDdeI = (LPNDDESHAREINFO)GlobalLock ( hAPIbuf ))) {
            GlobalFree( hAPIbuf );
            return FALSE;
        }

        /* NON-NULL lParam means this is a "Properties" invocation of the
            dialog (LPSTR)lParam points to the share name */

        if ( lParam ) {
            UINT ret;
            DWORD avail;
            WORD items;

            fPropertiesCalled = TRUE;

            SetWindowText ( hDlg, "Share Properties" );

            ret = NDdeShareGetInfo ( NULL, (LPSTR)lParam, 2, (LPBYTE)lpDdeI,
                APIBUFSIZ, &avail, &items );

            /* if the buffer was too small we try to increase it and try
               again */

            if ( ret == NDDE_BUF_TOO_SMALL ) {
                GlobalUnlock ( hAPIbuf );
                if ( GlobalReAlloc ( hAPIbuf, avail, GMEM_ZEROINIT ))
                    if ( lpDdeI = (LPNDDESHAREINFO)GlobalLock ( hAPIbuf ))
                        ret = NDdeShareGetInfo ( NULL, (LPSTR)lParam, 2,
                            (LPBYTE)lpDdeI, GlobalSize(hAPIbuf),
                            &avail, &items );
            }

            if ( ret != NDDE_NO_ERROR ) {
                ShowErrMsg ( hDlg, "Getting Share Properties", ret );
                if ( lpDdeI )
                    GlobalUnlock ( hAPIbuf );
                if ( hAPIbuf )
                    GlobalFree ( hAPIbuf );
                EndDialog ( hDlg, FALSE );
                return TRUE;
            }

            /* make sure the share name can't be edited... NDdeShareSetInfo
               can't change the name of a share */

            SendDlgItemMessage ( hDlg, IDC_SHARENAME, EM_SETREADONLY, TRUE, 0L);
            EnableWindow ( GetDlgItem ( hDlg, IDC_SHARENAME  ), FALSE );

            /* initialize dialog controls with values from GetInfo call */

            SetDlgItemText ( hDlg, IDC_SHARENAME, lpDdeI->szShareName );
            SetDlgItemText ( hDlg, IDC_TOPICNAME, lpDdeI->lpszTargetTopic );
            SetDlgItemText ( hDlg, IDC_APPNAME, lpDdeI->lpszTargetApp );
            SetDlgItemText ( hDlg, IDC_ITEMNAME, lpDdeI->lpszItem );

            Perm1 = lpDdeI->dwPermissions1;
            Perm2 = lpDdeI->dwPermissions2;

            /* this bit will be same for password 1 and 2 */
            SendDlgItemMessage ( hDlg, IDC_STARTAPP, BM_SETCHECK,
                !!(lpDdeI->dwPermissions1 & NDDEACCESS_START_APP), 0L );

            /* both passwords enabled? */
            if ( Perm1 & Perm2 & NDDEACCESS_REQUEST ) {
                SendDlgItemMessage(hDlg, IDC_DEPENDSON, BM_SETCHECK, TRUE, 0L);
                SetDlgItemText (hDlg, IDC_PASSWORD1, lpDdeI->lpbPassword1 );
                SetDlgItemText (hDlg, IDC_PASSWORD2, lpDdeI->lpbPassword2 );
            }

            /* full access share? check poke bit */
            /* NOTE that if the share is created with a permission mask
               that doesn't match either of our 2 read/write permission
               classes, changing the share with this utility will force it into
               one of those 2 forms according to the condition of the poke bit
            */
            else if ( Perm1 & NDDEACCESS_POKE ) {
                SendDlgItemMessage(hDlg, IDC_FULLACCESS, BM_SETCHECK, TRUE, 0L);
                EnableWindow ( GetDlgItem( hDlg, IDC_RDONLYTEXT ), FALSE );
                EnableWindow ( GetDlgItem( hDlg, IDC_PASSWORD1 ), FALSE );
                SetDlgItemText (hDlg, IDC_PASSWORD2, lpDdeI->lpbPassword1 );
            }
            /* else assumed read-only */
            else {
                SendDlgItemMessage (hDlg, IDC_READONLY, BM_SETCHECK, TRUE, 0L );
                EnableWindow ( GetDlgItem( hDlg, IDC_FULLACCESSTEXT ), FALSE );
                EnableWindow ( GetDlgItem( hDlg, IDC_PASSWORD2 ), FALSE );
                SetDlgItemText (hDlg, IDC_PASSWORD1, lpDdeI->lpbPassword1 );
            }
        }
        else {
            /* just put dialog into inital state - default to full access */
            SetWindowText ( hDlg, "New Share" );
            SendDlgItemMessage (hDlg, IDC_FULLACCESS, BM_SETCHECK, TRUE, 0L );
            EnableWindow ( GetDlgItem( hDlg, IDC_RDONLYTEXT ), FALSE );
            EnableWindow ( GetDlgItem( hDlg, IDC_PASSWORD1 ), FALSE );
            fPropertiesCalled = FALSE;
        }
        return (TRUE);

    case WM_COMMAND:
        switch ( wParam ) {

            case IDC_READONLY:
            case IDC_FULLACCESS:
            case IDC_DEPENDSON:
                switch ( HIWORD(lParam) ) {
                    case BN_CLICKED:
                    /* gray appropriate text */
                    EnableWindow ( GetDlgItem ( hDlg, IDC_FULLACCESSTEXT ),
                        SendDlgItemMessage ( hDlg, IDC_FULLACCESS,
                            BM_GETCHECK, 0, 0L ) ||
                        SendDlgItemMessage ( hDlg, IDC_DEPENDSON,
                            BM_GETCHECK, 0, 0L ));

                    EnableWindow ( GetDlgItem ( hDlg, IDC_PASSWORD2 ),
                        SendDlgItemMessage ( hDlg, IDC_FULLACCESS,
                            BM_GETCHECK, 0, 0L ) ||
                        SendDlgItemMessage ( hDlg, IDC_DEPENDSON,
                            BM_GETCHECK, 0, 0L ));

                    EnableWindow ( GetDlgItem ( hDlg, IDC_RDONLYTEXT ),
                        SendDlgItemMessage ( hDlg, IDC_READONLY,
                            BM_GETCHECK, 0, 0L ) ||
                        SendDlgItemMessage ( hDlg, IDC_DEPENDSON,
                            BM_GETCHECK, 0, 0L ));

                    EnableWindow ( GetDlgItem ( hDlg, IDC_PASSWORD1 ),
                        SendDlgItemMessage ( hDlg, IDC_READONLY,
                            BM_GETCHECK, 0, 0L ) ||
                        SendDlgItemMessage ( hDlg, IDC_DEPENDSON,
                            BM_GETCHECK, 0, 0L ));
                    break;
                }
                break;

            case IDOK:

                /* for both new shares and modifications the contents of
                   the dialog controls are retrieved into out buffers */

                SendDlgItemMessage ( hDlg, IDC_SHARENAME, WM_GETTEXT,
                    MAX_NDDESHARENAME+1, (LPARAM)(LPSTR)lpDdeI->szShareName );
                SendDlgItemMessage ( hDlg, IDC_APPNAME, WM_GETTEXT,
                    MAX_APPNAME+1, (LPARAM)(LPSTR)AppNameBuf );
                SendDlgItemMessage ( hDlg, IDC_TOPICNAME, WM_GETTEXT,
                    MAX_TOPICNAME+1, (LPARAM)(LPSTR)TopicNameBuf );
                SendDlgItemMessage ( hDlg, IDC_ITEMNAME, WM_GETTEXT,
                    MAX_ITEMNAME+1, (LPARAM)(LPSTR)ItemNameBuf );
                SendDlgItemMessage ( hDlg, IDC_PASSWORD1, WM_GETTEXT,
                    MAX_PASSWORD, (LPARAM)(LPSTR)Password1 );
                SendDlgItemMessage ( hDlg, IDC_PASSWORD2, WM_GETTEXT,
                    MAX_PASSWORD, (LPARAM)(LPSTR)Password2 );

                lpDdeI->lpszItem = ItemNameBuf;
                lpDdeI->lpszTargetApp = AppNameBuf;
                lpDdeI->lpszTargetTopic = TopicNameBuf;
                lpDdeI->lpbPassword1 = Password1;
                lpDdeI->lpbPassword2 = Password2;
                lpDdeI->lpszItem = ItemNameBuf;

                /* set up password(s) according to access type */

                if (SendDlgItemMessage (hDlg,IDC_FULLACCESS,BM_GETCHECK,0,0L)) {
                    Perm1 = PERM_WRITE;
                    lpDdeI->cbPassword1 = GetDlgItemText ( hDlg,
                        IDC_PASSWORD2, lpDdeI->lpbPassword1, MAX_PASSWORD );
                    lpDdeI->lpbPassword2[0] = '\0';
                    lpDdeI->cbPassword2 = 0;
                    Perm2 = 0L;
                    if (SendDlgItemMessage (hDlg,IDC_STARTAPP,BM_GETCHECK,0,0L))
                        Perm1 |= NDDEACCESS_START_APP;
                }
                if (SendDlgItemMessage(hDlg,IDC_READONLY,BM_GETCHECK,0,0L) ) {
                    lpDdeI->cbPassword1 = GetDlgItemText ( hDlg,
                        IDC_PASSWORD1, lpDdeI->lpbPassword1, MAX_PASSWORD );
                    Perm1 = PERM_READ;
                    lpDdeI->lpbPassword2[0] = '\0';
                    lpDdeI->cbPassword2 = 0;
                    Perm2 = 0L;
                    if (SendDlgItemMessage(hDlg,IDC_STARTAPP,BM_GETCHECK,0,0L)){
                        Perm1 |= NDDEACCESS_START_APP;
                    }
                }
                if (SendDlgItemMessage(hDlg,IDC_DEPENDSON,BM_GETCHECK,0,0L)) {
                    Perm2 = PERM_WRITE;
                    Perm1 = PERM_READ;
                        
                    lpDdeI->cbPassword1 = GetDlgItemText ( hDlg,
                        IDC_PASSWORD1, lpDdeI->lpbPassword1, MAX_PASSWORD );
                    lpDdeI->cbPassword2 = GetDlgItemText ( hDlg,
                        IDC_PASSWORD2, lpDdeI->lpbPassword2, MAX_PASSWORD );
                    if (SendDlgItemMessage(hDlg,IDC_STARTAPP,BM_GETCHECK,0,0L)){
                        Perm1 |= NDDEACCESS_START_APP;
                        Perm2 |= NDDEACCESS_START_APP;
                    }
                }

                lpDdeI->dwPermissions1 = Perm1;
                lpDdeI->dwPermissions2 = Perm2;
                lpDdeI->cAddItems = 0;
                lpDdeI->lpNDdeShareItemInfo = NULL;

                /* info is complete, now call share add or setinfo as
                   appropriate */

                if ( !fPropertiesCalled ) {
                    ret = NDdeShareAdd (NULL,2,(LPBYTE)lpDdeI,
                        sizeof(NDDESHAREINFO));
                    ShowErrMsg ( hDlg, "Adding Share", ret );
                }
                else {
                    ret = NDdeShareSetInfo ( NULL, lpDdeI->szShareName,
                        2, (LPBYTE)lpDdeI, sizeof(NDDESHAREINFO), 0 );
                    ShowErrMsg ( hDlg, "Properties", ret );
                }

                if ( ret == NDDE_NO_ERROR ) {
                    if ( lpDdeI )
                        GlobalUnlock ( hAPIbuf );
                    if ( hAPIbuf )
                        GlobalFree ( hAPIbuf );
                    EndDialog(hDlg, TRUE);
                }
                /* else give the user a change to correct or cancel */
                return (TRUE);

            case IDCANCEL:
                if ( lpDdeI )
                    GlobalUnlock ( hAPIbuf );
                if ( hAPIbuf )
                    GlobalFree ( hAPIbuf );
                EndDialog(hDlg, FALSE );
                return (TRUE);
        }
        break;
    }
    return (FALSE);
}

/* About box dialog proc */

BOOL FAR PASCAL AboutDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_COMMAND:
        if (wParam == IDOK || wParam == IDCANCEL) {
            EndDialog(hDlg, TRUE);
            return (TRUE);
        }
        else
            return( FALSE );
        break;
    }
    return (FALSE);
}
