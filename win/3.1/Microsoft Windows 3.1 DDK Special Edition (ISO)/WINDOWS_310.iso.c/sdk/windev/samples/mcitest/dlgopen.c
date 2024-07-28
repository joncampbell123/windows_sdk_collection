/*************************************************************************
 *
 *  DLGOPEN.C
 *
 *  Code implementing Dialog Open box.  Static link version!
 *  The application MUST EXPORT DlgfnOpen() in order to work.
 *
 *************************************************************************/

#include <windows.h>
#include <bios.h>
#include "dlgopen.h"

/*
 *  DLGOPEN private definitions
 */
#define ATTRFILELIST    0x0000  /* include files only */
#define ATTRDIRLIST 0xC010  /* directories and drives ONLY */
#define _MAX_PATH   128

static  char NEAR aszPropInfo[] = "dlgopen";
static  char NEAR aszPeriod[] = ".";
static  char NEAR aszNull[] = "";
static  char NEAR aszExt[] = "*.*";

typedef struct tagDLGOPENSTRUCT {
    char    aszExt[_MAX_PATH];      /* default extension to use */
    LPSTR   lszTitle;           /* Dialog box caption/title */
    OFSTRUCT    rOF;            /* Storage for OpenFile */
    DWORD   dwFlags;
    LPSTR   lszBuffer;          /* output file name */
    LPSTR   lszStatic;          /* Optional static text */
    WORD    cbBufLen;
}   DLGOPENSTRUCT,
    NEAR *NPDLGOPEN;

//  -   -   -   -   -   -   -   -   -

static  BOOL PASCAL NEAR DosChangeDir(
    LPSTR   lszDir)
{
        BOOL    fReturn;

    _asm {
        push    ds
        lds     dx,lszDir
        mov     bx,dx

        cmp     BYTE PTR ds:[bx+1],':'
        jnz     chdnod                      ; No drive
        mov     dl,ds:[bx]
        or      dl,20h
        sub     dl,'a'

        mov     ah,0eh                      ; set current drive
        int     21h

        mov     ah,19h                      ; get current drive
        int     21h

        cmp     al,dl
        jne     chderror

        lds     dx,lszDir
        add     dx,2
        mov     bx,dx
        cmp     BYTE PTR ds:[bx],0          ; path name is ""
        jz      chdok
chdnod:
        mov     ah,3bh
        int     21h
        jc      chderror
chdok:
        mov     ax, 1
        jmp     short chdexit
chderror:
        xor     ax, ax
chdexit:
        pop     ds
                mov             fReturn, ax
    }
        return fReturn;
}
//  -   -   -   -   -   -   -   -   -

static  BOOL PASCAL NEAR FGetEnviron(
    BYTE    bSearchSysDrive,
    LPSTR   lszPath)
{
        BOOL    fReturn;

    _asm {
        push    ds
        push    si
        lds     si, lszPath
        mov     ah, 19h         ; Get current drive.
        int     21h
        add     al, 'A'
        mov     BYTE PTR [si], al
        mov     dl, bSearchSysDrive
        mov     BYTE PTR [si + 1], dl
        mov     BYTE PTR [si + 2], ':'
        mov     BYTE PTR [si + 3], '\\'
        add     si, 4
        sub     dl, '@'
        mov     ah, 47h
        int     21h             ; Get current directory.
        mov     ax,0
        adc     ax,ax
        dec     ax
        pop     si
        pop     ds
                mov             fReturn, ax
    }
        return fReturn;
}

//  -   -   -   -   -   -   -   -   -

static  void PASCAL NEAR VSetEnviron(
    LPSTR   lszPath)
{
    _asm {
        push    ds
        push    si
        lds     si, lszPath
        mov     dl, BYTE PTR [si]
        sub     dl, 'A'
        mov     ah, 0Eh                 ; Set current drive.
        int     21h
        lea     dx, [si + 1]
        mov     ah, 3Bh                 ; Set current directory.
        int     21h
        pop     si
        pop     ds
    }
}

//  -   -   -   -   -   -   -   -   -

extern  int FAR PASCAL OpenFileDialog(
    HWND    hwndParent,
    LPSTR   lszTitle,
    LPSTR   lszExtension,
    DWORD   dwFlags,
    LPSTR   lszStatic,
    LPSTR   lszPath,
    WORD    cbBufLen)
{
    NPDLGOPEN   pdlg;
    FARPROC lpfnDlgProc;
    HINSTANCE hInst;
    int nResult;

    pdlg = (NPDLGOPEN)LocalAlloc(LPTR, sizeof(DLGOPENSTRUCT));
    pdlg->dwFlags = dwFlags;
    pdlg->cbBufLen = cbBufLen;
    pdlg->lszStatic = lszStatic;
    pdlg->lszBuffer = lszPath;
    lstrcpy(pdlg->aszExt, (lszExtension && *lszExtension) ? lszExtension : aszExt);
    pdlg->lszTitle = lszTitle;
    hInst = (HINSTANCE)GetWindowWord(hwndParent, GWW_HINSTANCE);
    lpfnDlgProc = (FARPROC)MakeProcInstance((FARPROC)DlgfnOpen, hInst);
    nResult = DialogBoxParam(hInst, "DlgOpenBox", hwndParent, (DLGPROC)lpfnDlgProc, (LONG)(WORD)pdlg);
    FreeProcInstance(lpfnDlgProc);
    LocalFree((HANDLE)pdlg);
    return nResult;
}

//  -   -   -   -   -   -   -   -   -

static BOOL NEAR PASCAL FSearchSpec(
    LPSTR   lsz)
{
    for (; *lsz; lsz++)
        if (*lsz == '*' || *lsz == '?')
            return TRUE;
    return FALSE;
}

//  -   -   -   -   -   -   -   -   -

static  void NEAR PASCAL DlgCheckOkEnable(
    HWND    hwnd,
    int     idEdit,
    WORD    wMsg)
{
    BYTE    aszBuf[80];
    HWND    hwndEdit;
    HWND    hwndOk;

    if (wMsg == EN_CHANGE) {
        hwndEdit = GetDlgItem(hwnd, idEdit);
        hwndOk = GetDlgItem(hwnd, IDOK);
        EnableWindow(hwndOk, (BOOL)SendMessage(hwndEdit, WM_GETTEXTLENGTH, 0, 0L));
        GetWindowText(hwndEdit, aszBuf, sizeof(aszBuf));
	SendDlgItemMessage(hwnd, (int)DLGOPEN_FILE_LISTBOX, (UINT)LB_SELECTSTRING, (WPARAM)-1, (LONG)(LPSTR)aszBuf);
    }
}

//  -   -   -   -   -   -   -   -   -

static LPSTR NEAR PASCAL LszFillListBox(
    HWND    hwnd,
    LPSTR   lszFile)
{
    BYTE    asz[20];
    LPSTR   lsz;
    LPSTR   lszDir;   // Directory name or path */

    lsz = lszFile;
    lszDir = asz;
    while (*lsz && *lsz != ';')
        lsz++;
    while (lsz > lszFile && *lsz != '\\')
        lsz--;
    if (lsz > lszFile) {
        *lsz = 0;
        lstrcpy(lszDir, lszFile);
        lszFile = lsz+1;
    } else
        lstrcpy(lszDir, aszPeriod);
    DlgDirList(hwnd, lszDir, DLGOPEN_DIR_LISTBOX, DLGOPEN_PATH, ATTRDIRLIST);
    SendDlgItemMessage(hwnd, DLGOPEN_FILE_LISTBOX, LB_RESETCONTENT, 0, 0L);
    SendDlgItemMessage(hwnd, DLGOPEN_FILE_LISTBOX, WM_SETREDRAW, FALSE, 0L);
    lszDir = lszFile;        // save lszFile to return */
    while (*lszFile) {
        lsz = asz;
        while (*lszFile == ' ')
            lszFile++;
        while (*lszFile && *lszFile != ';')
            *lsz++ = *lszFile++;
        *lsz = 0;
        if (*lszFile)
            lszFile++;
        SendDlgItemMessage(hwnd, DLGOPEN_FILE_LISTBOX, LB_DIR, ATTRFILELIST, (LONG)(LPSTR)asz);
    }
    SendDlgItemMessage(hwnd, DLGOPEN_FILE_LISTBOX, WM_SETREDRAW, TRUE, 0L);
    InvalidateRect(GetDlgItem(hwnd, DLGOPEN_FILE_LISTBOX), NULL, TRUE);
    return lszDir;
}

//  -   -   -   -   -   -   -   -   -

static  void NEAR PASCAL VInitDialog(
    HWND    hwnd,
    LONG    lParam)
{
    NPDLGOPEN   pdlg;

    SetProp(hwnd, aszPropInfo, (HANDLE)(WORD)lParam);
    pdlg = (NPDLGOPEN)(WORD)lParam;
    if (pdlg->lszTitle)
        SetWindowText(hwnd, pdlg->lszTitle);
    if (pdlg->lszStatic)
        SetDlgItemText(hwnd, DLGOPEN_STATIC, pdlg->lszStatic);
    SendDlgItemMessage(hwnd, DLGOPEN_EDIT, EM_LIMITTEXT, pdlg->cbBufLen, 0L);
    if (pdlg->dwFlags & DLGOPEN_NOSHOWSPEC)
        SetDlgItemText(hwnd, DLGOPEN_EDIT, aszNull);
    else
        SetDlgItemText(hwnd, DLGOPEN_EDIT, pdlg->aszExt);
    if ((pdlg->dwFlags & DLGOPEN_SAVE) && *pdlg->lszBuffer)
        SetDlgItemText(hwnd, DLGOPEN_EDIT, pdlg->lszBuffer);
    LszFillListBox(hwnd, pdlg->aszExt);
    SendDlgItemMessage(hwnd, DLGOPEN_EDIT, EM_SETSEL, 0, 0x7FFF0000L);
}

//  -   -   -   -   -   -   -   -   -

static  void NEAR PASCAL VComIdOk(
    HWND    hwnd)
{
    NPDLGOPEN   pdlg;
    LPSTR   lszFile;
    HFILE nResult;
    WORD    w;

    if (!IsWindowEnabled(GetDlgItem(hwnd, IDOK)))
        return;
    pdlg = (NPDLGOPEN)GetProp(hwnd, aszPropInfo);
    GetDlgItemText(hwnd, DLGOPEN_EDIT, pdlg->lszBuffer, pdlg->cbBufLen);
    lszFile = pdlg->lszBuffer;
    w = lstrlen(lszFile);
    if (w && (lszFile[w - 1] == '\\'))
        lszFile[w] = '\0';
    if (DosChangeDir(lszFile))
        lstrcpy(lszFile, pdlg->aszExt);
    if (FSearchSpec(lszFile)) {
        lstrcpy(pdlg->aszExt, LszFillListBox(hwnd, lszFile));
        SetDlgItemText(hwnd, DLGOPEN_EDIT, (pdlg->dwFlags & DLGOPEN_NOSHOWSPEC) ? aszNull : pdlg->aszExt);
        return;
    }
    nResult = OpenFile(lszFile, &pdlg->rOF, LOWORD(pdlg->dwFlags));
    if ((nResult == HFILE_ERROR) && (HIWORD(pdlg->dwFlags) & DLGOPEN_MUSTEXIST))
        MessageBeep(0);
    else {
        OemToAnsi(pdlg->rOF.szPathName, pdlg->lszBuffer);
        EndDialog(hwnd, nResult == HFILE_ERROR ? DLGOPEN_NOTFOUND : (int)nResult);
    }
}

//  -   -   -   -   -   -   -   -   -

extern  BOOL FAR PASCAL DlgfnOpen(
    HWND    hwnd,
    WORD    wMsg,
    WORD    wParam,
    LONG    lParam)
{
    WORD    w;
    NPDLGOPEN   pdlg;
    LPSTR   lszFile;

    pdlg = (NPDLGOPEN)GetProp(hwnd, aszPropInfo);
    switch (wMsg) {
    case WM_INITDIALOG:
        VInitDialog(hwnd, lParam);
        return TRUE;
    case WM_DESTROY:
        RemoveProp(hwnd, aszPropInfo);
        return TRUE;
    case WM_CLOSE:
        EndDialog(hwnd, DLGOPEN_CANCEL);
        return TRUE;
    case WM_COMMAND:
        switch (wParam) {
        case IDOK:
            VComIdOk(hwnd);
            return TRUE;
        case IDCANCEL:
            *pdlg->lszBuffer = '\0';
            *pdlg->rOF.szPathName = '\0';
            EndDialog(hwnd, DLGOPEN_CANCEL);
            return TRUE;
        case DLGOPEN_FILE_LISTBOX:
        case DLGOPEN_DIR_LISTBOX:
            switch (HIWORD(lParam)) {
            case 1:
                lszFile = pdlg->lszBuffer;
                DlgDirSelect(hwnd, lszFile, wParam);
                w = lstrlen(lszFile) - 1;
                switch (lszFile[w]) {
                case ':':
                    lstrcat(lszFile, aszPeriod);
                    break;
                case '\\':
                    lszFile[w] = 0;
                    break;
                }
                SetDlgItemText(hwnd, DLGOPEN_EDIT, lszFile);
                return TRUE;
            case 2:
                PostMessage(hwnd, WM_COMMAND, IDOK, 0L);
                return TRUE;
            }
            break;
        case DLGOPEN_EDIT:
            DlgCheckOkEnable(hwnd, DLGOPEN_EDIT, HIWORD(lParam));
            return TRUE;
        }
        break;
    }
    return FALSE;
}

//  -   -   -   -   -   -   -   -   -
