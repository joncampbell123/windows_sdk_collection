/*/  DIALS.C for TTY
 +-----------------------------------------+
 | Fralc Consultores (c), Mexico 1986-1989 |
 +-----------------------------------------+
/*/

// ------------------------------------------------------------------------
// Copyright (C) 1989-1990, Microsoft Corporation. All rights reserved.
// ------------------------------------------------------------------------
//	Microsoft History (latest first)
//	18 jan 90	peterbe		Added bHelpWasCalled, changed/added
//					WinHelp() calls.
//	29 nov 89	peterbe		Use MB_ICONEXCLAMATION instead of
//					MB_ICONQUESTION.
//	22 oct 89	peterbe		Changed casting AGAIN around line
//					940
//					Changed param of LB_SETCURSEL since
//					ANSISTART is now 128.
//					Change init. of edit control.
//	20 oct 89	peterbe		Checked in.
//					Cast second param of SetEditBuf()
//					call to WORD to elim. annoying mesg.
// ------------------------------------------------------------------------

#include	"generic.h"
#include	<ctype.h>
#include	"stdlib.h"
#include	"ttyhelp.h"

BOOL bHelpWasCalled = FALSE;

int FAR PASCAL DialogFn ( HWND, unsigned, WORD, DWORD);
int FAR PASCAL AboutDialog (HWND, unsigned, WORD, DWORD);
int FAR PASCAL AddDialog (  HWND, unsigned, WORD, DWORD);
int FAR PASCAL ModifyDialog(HWND, unsigned, WORD, DWORD);
int FAR PASCAL CharDialog(  HWND, unsigned, WORD, DWORD);

int near CodigoComun(	    HWND, unsigned, WORD, DWORD);
void CentraDlg(HWND, BOOL);

void near ecInsert(char *s,char c);
void near ecDelete(char *s);
void near ecReplace(unsigned char *s,unsigned char *i);
int near Printable(unsigned char *s,unsigned char *p);
int near RealPos(HWND wnd, int p,char *buff,char *pePos);
void near PutCaret(unsigned short wnd,int x);
void near ecCopy(unsigned short wnd);
void near ecClear(int wnd);
void near ecCut(int wnd);
void near ecPaste(unsigned short wnd);
void near ecSysVirtual(unsigned short wnd,unsigned short wp);
void near ecVirtual(unsigned short wnd,unsigned short wp);
void near ecInput(unsigned short wnd,unsigned int c);
BOOL FAR PASCAL PseudoInit( HANDLE );
LONG FAR PASCAL PseudoEdit( HWND, unsigned, WORD, DWORD);
#if 0
long FAR PASCAL	    SendDlgItemMessage(HWND, int, unsigned, WORD, LONG);
void FAR PASCAL	    CheckRadioButton(HWND, int, int, int);
HWND FAR PASCAL	    SetFocus(HWND);
HWND FAR PASCAL	    GetDlgItem(HWND, int);
void FAR PASCAL	    EndDialog(HWND, int);
BOOL FAR PASCAL EnableWindow(HWND,BOOL);
int  FAR PASCAL	    DialogBox(HANDLE, LPSTR, HWND, FARPROC);
void FAR PASCAL	    CheckRadioButton(HWND, int, int, int);
long FAR PASCAL	    SendMessage(HWND, unsigned, WORD, LONG);
int FAR PASCAL LoadString( HANDLE, unsigned, LPSTR, int );
void FAR PASCAL	    SetDlgItemText(HWND, int, LPSTR);
int  FAR PASCAL	    GetDlgItemText(HWND, int, LPSTR, int);
int  FAR PASCAL MessageBox(HWND, LPSTR, LPSTR, WORD);
int FAR cdecl wsprintf(LPSTR,LPSTR,...);
int FAR PASCAL GetSystemMetrics(int);
void FAR PASCAL GetWindowRect(HWND, LPRECT);
void FAR PASCAL MoveWindow(HWND, int, int, int, int, BOOL);
HDC   FAR PASCAL GetDC(HWND);
int   FAR PASCAL ReleaseDC(HWND, HDC);
DWORD FAR PASCAL GetTextExtent(HDC, LPSTR, int);
void FAR PASCAL	  GetClientRect(HWND, LPRECT);
WORD FAR PASCAL GetWindowWord(HWND, int);
WORD FAR PASCAL SetWindowWord(HWND, int, WORD);
LONG FAR PASCAL GetWindowLong(HWND, int);
LONG FAR PASCAL SetWindowLong(HWND, int, LONG);
void FAR PASCAL	  GetClientRect(HWND, LPRECT);
void FAR PASCAL InvalidateRect(HWND, LPRECT, BOOL);
void FAR PASCAL SetCaretPos(int, int);
void FAR PASCAL GetCaretPos(LPPOINT);
BOOL   FAR PASCAL OpenClipboard(HWND);
BOOL   FAR PASCAL CloseClipboard(void);
HANDLE FAR PASCAL SetClipboardData(WORD, HANDLE);
HANDLE FAR PASCAL GetClipboardData(WORD);
BOOL   FAR PASCAL EmptyClipboard(void);
void FAR PASCAL UpdateWindow(HWND);
HWND FAR PASCAL GetParent(HWND);
HWND FAR PASCAL GetNextDlgTabItem(HWND, HWND, BOOL);
HANDLE	FAR PASCAL LocalAlloc( WORD, WORD );
HANDLE	FAR PASCAL LocalFree( HANDLE );
HWND FAR PASCAL	    GetFocus(void);
BOOL  FAR PASCAL GetTextMetrics(HDC, LPTEXTMETRIC );
void FAR PASCAL CreateCaret(HWND, HBITMAP, int, int);
void FAR PASCAL DestroyCaret(void);
void FAR PASCAL HideCaret(HWND);
void FAR PASCAL ShowCaret(HWND);
void FAR PASCAL SetCaretPos(int, int);
void FAR PASCAL GetCaretPos(LPPOINT);
HDC FAR PASCAL	  BeginPaint(HWND, LPPAINTSTRUCT);
void FAR PASCAL	  EndPaint(HWND, LPPAINTSTRUCT);
long FAR PASCAL DefWindowProc(HWND, unsigned, WORD, LONG);
void FAR PASCAL CheckDlgButton(HWND, int, WORD);
void FAR PASCAL CheckRadioButton(HWND, int, int, int);
WORD FAR PASCAL IsDlgButtonChecked(HWND, int);
#endif
BOOL  FAR PASCAL GetTextMetrics(HDC, LPTEXTMETRIC );


int far PASCAL DeletePrinter(int number);

int FAR PASCAL DialogFn( hDlg, uMessage, wp, lp)
HWND	hDlg;
unsigned uMessage;
WORD	wp;
DWORD	lp;
{
    static int iNum, i;
    short	temp;
    short  n;

    switch( uMessage ){
	case WM_INITDIALOG:

	    if(!GetPrinterFileName())
		{
		EndDialog(hDlg, FALSE);
		break;
		}
	    iNum = GetNumPrinters();
	    for( i = 1 ; i <= iNum ; i++){
		GetPrinter(i);
		SendDlgItemMessage( hDlg, MD_PBOX, CB_ADDSTRING,
			0 ,(LONG)(LPSTR) Printer. piPrinterName);
	    }
	    SendDlgItemMessage( hDlg, MD_PBOX, CB_SETCURSEL,
		GetCurPrinter() - 1, 0L);
	    GetPrinter(GetCurPrinter());

	    if(lpDevmode->feed == CUT){
		lpDevmode->page_break = TRUE;
		EnableWindow(GetDlgItem( hDlg, MD_NOPAGEBREAK), FALSE);
	    }else
		EnableWindow(GetDlgItem( hDlg, MD_NOPAGEBREAK), TRUE);

	    CheckRadioButton( hDlg, MD_LETTER,MD_B5,
		lpDevmode->paper+MD_LETTER);

	    CheckRadioButton( hDlg, MD_CONT, MD_CUT,
		lpDevmode->feed-CONTINUOUS+MD_CONT);

	    CheckDlgButton(hDlg, MD_2ANCHO, lpDevmode->use_wide_carriage);
	    CheckDlgButton(hDlg, MD_NOPAGEBREAK, !lpDevmode->page_break);

	    SetFocus(GetDlgItem(hDlg, MD_PBOX));
	    CentraDlg(hDlg, TRUE);   /* TRUE -> Es el main DLG */
	    break;


	case WM_COMMAND:
	    switch(wp)
		{
		case IDOK:
		    // End Help if it was called.
		    if (bHelpWasCalled)
			WinHelp(hDlg, (LPSTR) "TTY.hlp",
				    (WORD) HELP_QUIT, (DWORD) NULL);

		    // i is enumerated type {LETTER, LEGAL, A4, B5}

		    for (i = 0; i < MAXPAPER; i++)
			if (IsDlgButtonChecked(hDlg, MD_LETTER + i))
			    {
			    lpDevmode->paper = i;
			    lpDevmode->dm.dmPaperSize = PaperFormat[i].code;
			    break;
			    }
#if 0
		    if( IsDlgButtonChecked(hDlg,MD_OFICIO) )
			{
			lpDevmode->paper = LEGAL;
			lpDevmode->dm.dmPaperSize = DMPAPER_LEGAL;
			}
		    else if( IsDlgButtonChecked(hDlg,MD_A4) )
			{
			lpDevmode->paper = A4;
			lpDevmode->dm.dmPaperSize = DMPAPER_A4;
			}
		    else if( IsDlgButtonChecked(hDlg,MD_B5) )
			{
			lpDevmode->paper = B5;
			lpDevmode->dm.dmPaperSize = DMPAPER_B5;
			}
		    else
			{
			lpDevmode->paper = LETTER;
			lpDevmode->dm.dmPaperSize = DMPAPER_LETTER;
			}
#endif

		    if( IsDlgButtonChecked(hDlg,MD_CUT) )
			{
			lpDevmode->feed = CUT;
			lpDevmode->dm.dmDefaultSource = DMBIN_MANUAL;
			}
		    else
			{
			lpDevmode->feed = CONTINUOUS;
			lpDevmode->dm.dmDefaultSource = DMBIN_TRACTOR;
			}

		    lpDevmode->use_wide_carriage =
			IsDlgButtonChecked(hDlg,MD_2ANCHO);
		    lpDevmode->page_break =
			!IsDlgButtonChecked(hDlg,MD_NOPAGEBREAK);
		    temp = (int)SendMessage(GetDlgItem(hDlg, MD_PBOX),
			    CB_GETCURSEL, 0, 0L);
		    SetCurPrinter( temp + 1 );
		    EndDialog(hDlg, IDOK);
		    break;

		case IDCANCEL:
		    // End Help if it was called.
		    if (bHelpWasCalled)
			WinHelp(hDlg, (LPSTR) "TTY.hlp",
				    (WORD) HELP_QUIT, (DWORD) NULL);
		    EndDialog(hDlg, IDCANCEL);
		    break;

		case MD_LETTER:
		case MD_LEGAL:
		case MD_A4:
		case MD_B5:
		    CheckRadioButton( hDlg, MD_LETTER, MD_B5, wp);
		    break;

		case MD_CONT:
		case MD_CUT:
		    CheckRadioButton( hDlg, MD_CONT, MD_CUT, wp);
		    if ( wp == MD_CUT ){
			CheckDlgButton(hDlg, MD_NOPAGEBREAK, FALSE);
			EnableWindow(GetDlgItem( hDlg, MD_NOPAGEBREAK), FALSE);
		    }else
			EnableWindow(GetDlgItem( hDlg, MD_NOPAGEBREAK), TRUE);
		    break;

		case MD_2ANCHO:
		case MD_NOPAGEBREAK:
		    SendDlgItemMessage( hDlg, wp, BM_SETCHECK,
			SendDlgItemMessage( hDlg, wp, BM_GETCHECK,0,0L) ? 0 : 1,
			0L);
		    break;


		case MD_ADD:
		    if( DialogBox( hInst,
				MAKEINTRESOURCE(ADD_DLG), hDlg, AddDialog)) {
			SendDlgItemMessage( hDlg, MD_PBOX, CB_ADDSTRING, 0,
				(LONG)(LPSTR) Printer. piPrinterName);
			SetPrinter(n = GetNumPrinters()+1);
			SetCurPrinter(n);
			SendDlgItemMessage( hDlg, MD_PBOX, CB_SETCURSEL,
				n-1, 0L);
		    } else
			GetPrinter(GetCurPrinter());
		    break;

		case MD_MOD:
		    temp = (int)SendMessage(GetDlgItem(hDlg, MD_PBOX),
			CB_GETCURSEL, 0, 0L);
		    GetPrinter( temp + 1 );
		    if((iNum = DialogBox( hInst,
		    		MAKEINTRESOURCE(MOD_DLG), hDlg,
				ModifyDialog)) == AD_BORRA) {
			DeletePrinter(GetCurPrinter());
			SetCurPrinter(1);
			GetPrinter(1);
			iNum = (int)SendDlgItemMessage( hDlg, MD_PBOX,
				CB_GETCURSEL, 0, 0L);
			SendDlgItemMessage( hDlg, MD_PBOX, CB_DELETESTRING,
				iNum, 0L);
			SendDlgItemMessage( hDlg, MD_PBOX, CB_SETCURSEL, 0, 0L);
			break;
		    }
		    if( iNum != IDCANCEL ) {
			SetPrinter(GetCurPrinter());
			iNum = (int)SendDlgItemMessage( hDlg, MD_PBOX,
				CB_GETCURSEL, 0, 0L);
			SendDlgItemMessage(hDlg,MD_PBOX, CB_DELETESTRING,
				iNum, 0L);
			SendDlgItemMessage(hDlg,MD_PBOX, CB_INSERTSTRING,
				iNum, (LONG)(LPSTR) Printer. piPrinterName);
		    }
		    SendDlgItemMessage( hDlg, MD_PBOX, CB_SETCURSEL,
			GetCurPrinter() - 1, 0L);
		    SetFocus(GetDlgItem(hDlg, MD_PBOX));
		    break;

		case MD_CHAR:
		    temp = (int)SendMessage(GetDlgItem(hDlg, MD_PBOX),
			CB_GETCURSEL, 0, 0L);
		    GetPrinter( temp + 1 );
		    if(DialogBox( hInst, MAKEINTRESOURCE(CHAR_DLG),
			hDlg, CharDialog) != IDCANCEL)
			SetPrinter( temp + 1 );
		    break;

		case MD_HELP:
		    bHelpWasCalled = WinHelp(
			hDlg, (LPSTR)"TTY.HLP", HELP_INDEX, (DWORD)0L);
		    break;

		case MD_ABOUT:
		    DialogBox( hInst, MAKEINTRESOURCE(ABOUT_DLG),
					hDlg, AboutDialog);
		    break;

		case MD_PBOX:
		    if( HIWORD(lp) == CBN_SELCHANGE ) {
			temp = (int)SendMessage(GetDlgItem(hDlg, MD_PBOX),
					CB_GETCURSEL, 0, 0L);
			GetPrinter( temp + 1 );
			SetCurPrinter( temp + 1 );
		    }
		    break;

	    }
	    break;

	case WM_SYSCOMMAND:
	    if ( wp == SC_CLOSE ) {
	    EndDialog(hDlg,IDCANCEL);
	    break;
	    }

	    /* Fall through */

	default:
	    return FALSE;
    }
    return TRUE;
}


int FAR PASCAL AddDialog(HWND hDlg, unsigned mes, WORD wp, DWORD lp)
{
    int	   i;

    if(mes == WM_INITDIALOG){
	Printer.piPrinterData.pcPageWidth   = 8;
	Printer.piPrinterData.pcPageHeight  = 66;
	Printer.piPrinterData.pcReset[0]    = '\0';
	Printer.piPrinterData.pc10cpi[0]    = '\0';
	Printer.piPrinterData.pc12cpi[0]    = '\0';
	Printer.piPrinterData.pc16cpi[0]    = '\0';
	Printer.piPrinterData.pcDoubleOn[0]  = '\0';
	Printer.piPrinterData.pcDoubleOff[0] = '\0';
	for(i = 0; i<=ANSIEND-ANSISTART; i++){
	    Printer.piPrinterTable[i][0] = defaultchars[i];
	    Printer.piPrinterTable[i][1] = '\0';
	}
	LoadString(hInst, STR_NEWID, Printer.piPrinterName, 15);
    }
    return CodigoComun(hDlg,mes,wp,lp);
}



int FAR PASCAL ModifyDialog(HWND hDlg, unsigned mes, WORD wp, DWORD lp)
{
    if( mes == WM_INITDIALOG && GetCurPrinter() == 1)
	EnableWindow(GetDlgItem( hDlg, AD_BORRA), FALSE);

    return CodigoComun(hDlg,mes,wp,lp);
}

int near CodigoComun(HWND hDlg, unsigned mes, WORD wp, DWORD lp)
{
    char    msgBuffer[80],
	    msgBuf[80],
	    capBuffer[20];

    switch(mes) {
	case WM_INITDIALOG:
	    SetDlgItemText(hDlg,AD_NAME,Printer.piPrinterName);
	    SendDlgItemMessage(hDlg, AD_NAME, EM_LIMITTEXT, 15, 0L);
	    SetDlgItemText(hDlg,AD_RESET,Printer.piPrinterData.pcReset);
	    SetDlgItemText(hDlg,AD_10CPI,Printer.piPrinterData.pc10cpi);
	    SetDlgItemText(hDlg,AD_12CPI,Printer.piPrinterData.pc12cpi);
	    SetDlgItemText(hDlg,AD_16CPI,Printer.piPrinterData.pc16cpi);
	    SetDlgItemText(hDlg,AD_BDW,Printer.piPrinterData.pcDoubleOn);
	    SetDlgItemText(hDlg,AD_EDW,Printer.piPrinterData.pcDoubleOff);
	    SetFocus(GetDlgItem(hDlg, AD_NAME));
	    SendDlgItemMessage(hDlg, AD_NAME, EM_SETSEL, 0, 0x7fff0000L);
	    CentraDlg(hDlg,FALSE);     /* FALSE -> Escalonalo */
	    if(!lstrlen(Printer.piPrinterName))
		EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
	    return FALSE;

	case WM_COMMAND:
	    switch( wp ) {
		case IDOK:
		    GetDlgItemText(hDlg, AD_NAME,
			Printer.piPrinterName, PRINTERLEN+1);
		    GetDlgItemText(hDlg, AD_RESET,
			Printer.piPrinterData.pcReset, ESCAPELEN+1);
		    GetDlgItemText(hDlg, AD_10CPI,
			Printer.piPrinterData.pc10cpi, ESCAPELEN+1);
		    GetDlgItemText(hDlg, AD_12CPI,
			Printer.piPrinterData.pc12cpi, ESCAPELEN+1);
		    GetDlgItemText(hDlg, AD_16CPI,
			Printer.piPrinterData.pc16cpi, ESCAPELEN+1);
		    GetDlgItemText(hDlg, AD_BDW,
			Printer.piPrinterData.pcDoubleOn, ESCAPELEN+1);
		    GetDlgItemText(hDlg, AD_EDW,
			Printer.piPrinterData.pcDoubleOff, ESCAPELEN+1);
		    EndDialog(hDlg,IDOK);
		    break;

		case AD_CANCEL:
		    EndDialog( hDlg, FALSE);
		    break;

		case AD_NAME:
		    if(SendDlgItemMessage(hDlg, AD_NAME, WM_GETTEXTLENGTH,
				0, 0L))
			EnableWindow(GetDlgItem(hDlg, IDOK), TRUE);
		    else
			EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
		    break;

		case AD_BORRA:

		    /*  Hay que asegurarse que sea el de Modificar ??? */

		    LoadString(hInst, IDS_MSG_DEL, msgBuf, 40);
		    LoadString(hInst, IDS_MSG_CAPTION, capBuffer, 20);
		    wsprintf((LPSTR)msgBuffer,(LPSTR)msgBuf,
			(LPSTR)Printer.piPrinterName);
		    if( MessageBox(hDlg,(LPSTR)msgBuffer,(LPSTR)capBuffer,
			MB_ICONEXCLAMATION|MB_OKCANCEL|MB_DEFBUTTON2) == IDOK) {
			EndDialog(hDlg,AD_BORRA);
		    }
		    break;

	    }
	    break;

	case WM_SYSCOMMAND:
	    if ( wp == SC_CLOSE ) {
		EndDialog(hDlg,FALSE);
		break;
	    }

	    /* Fall Through */

	default:
	    return FALSE;
    }
    return TRUE;
}


int FAR PASCAL CharDialog(HWND hDlg, unsigned mes, WORD wp, DWORD lp)
{
    short i;
    static int ii;
    static char	 cBuffer[10];

    switch(mes){
	case WM_INITDIALOG:
	    SendDlgItemMessage(hDlg,CD_CARANSI, WM_SETREDRAW, FALSE, 0L);
	    for(ii = ANSISTART; ii <= ANSIEND; ii++) {
		wsprintf(cBuffer, " %3d = %c", ii, ii);
		SendDlgItemMessage(hDlg,CD_CARANSI,LB_ADDSTRING,0,
			(LONG)(LPSTR) cBuffer);
	    }
	    SendDlgItemMessage(hDlg,CD_CARANSI, WM_SETREDRAW, TRUE, 0L);
	    SendDlgItemMessage(hDlg,CD_CODIMPRESORA,
		EM_LIMITTEXT,ESCAPELEN+1,0L);
	    // since ANSISTART < 160, we do this... we end up with 160
	    // highlighted in the middle of the listbox
	    SendDlgItemMessage(hDlg,CD_CARANSI, LB_SETCURSEL,
			164-ANSISTART, 0L);
	    SendDlgItemMessage(hDlg,CD_CARANSI, LB_SETCURSEL,
			160-ANSISTART, 0L);
	    // put the initially-selected string in the edit control
	    SetDlgItemText(hDlg,CD_CODIMPRESORA,
		Printer.piPrinterTable[160-ANSISTART]);
	    CentraDlg(hDlg,TRUE);
	    SetFocus(GetDlgItem(hDlg, CD_CODIMPRESORA));
	    return FALSE;

	case WM_COMMAND:
	    switch(wp){
		case IDOK:
		EndDialog(hDlg,IDOK);
		break;

		case CD_CODIMPRESORA:
		    if(HIWORD(lp)==EN_KILLFOCUS)
			GetDlgItemText(hDlg,CD_CODIMPRESORA,
			    Printer.piPrinterTable[
				SendDlgItemMessage(hDlg, CD_CARANSI,
					LB_GETCURSEL, 0, 0L)],
			    ESCAPELEN+1);
		    break;

		case IDCANCEL:
		    EndDialog(hDlg,IDCANCEL);
		    break;

		case CD_CARANSI:
		    switch(HIWORD(lp)){
			case LBN_SELCHANGE:
			    i = (int)SendDlgItemMessage(hDlg,
				CD_CARANSI, LB_GETCURSEL, 0, 0L);
			    if(i >= 0){
				EnableWindow(GetDlgItem(
					hDlg,CD_CODIMPRESORA), TRUE);
				SetDlgItemText(hDlg, CD_CODIMPRESORA,
					Printer.piPrinterTable[i]);
			    }else
				EnableWindow(GetDlgItem(hDlg,CD_CODIMPRESORA),
					FALSE);
			    break;
		    }
		    break;

		default:
		    break;
	    }
	    break;

	default:
	    return FALSE;
    }
    return TRUE;
}

int FAR PASCAL AboutDialog(HWND hDlg, unsigned mes, WORD wp, DWORD lp)
{
    if (mes == WM_COMMAND) {
    EndDialog( hDlg, TRUE );
    return TRUE;
    }
    else if (mes == WM_INITDIALOG){
    CentraDlg(hDlg,TRUE);
    return TRUE;
    }
    else return FALSE;
}

void CentraDlg(HWND hDlg, BOOL bEscalona)
{
    int	   x = GetSystemMetrics( SM_CXSCREEN ),
       y = GetSystemMetrics( SM_CYSCREEN );
    RECT   DlgRect;

    GetWindowRect( hDlg, &DlgRect );
    MoveWindow(hDlg,(x + DlgRect.left - DlgRect.right)/2 + 20 * !bEscalona -10,
	    (y + DlgRect.top  - DlgRect.bottom)/2 + 20 * !bEscalona -10,
	    DlgRect.right  - DlgRect.left,
	    DlgRect.bottom - DlgRect.top,
	    TRUE);
}

/*************************************************************************/


void near ecInsert(s,c)
char *s,c;
{
    char *a;

    for(a=s;*a;a++);
    a++;
    for(;a>s;a--)
	*a = *(a-1);
    *s = c;
}

void near ecDelete(s)
char *s;
{
    for(;*s;s++)
	*s = *(s+1);
}

void near ecReplace(s,i)
unsigned char *s;
unsigned char *i;
{
    int a,d,l;

    for(l=0;i[l];l++);
    for(a=0;s[a];a++);
    /*a--;*/
    for(d=a+l-1;a>0;a--,d--)
	 s[d]=s[a];
    for(;*i;)
	*s++=*i++;
}

char asc[8];

int near Printable(s,p)
unsigned char  *s;
unsigned char  *p;
{
    int h,q,l;
    char c;

    *p = '\0';
    l = 0;
    for(h=0;s[h];){
	l++;
	*p++ = (char)h;
	if(s[h] == '\x1b'){
	    ecReplace(s+h,"<ESC>");
	    h += 5;
	}else if( s[h] == (unsigned char)'\xFF'){
	    ecReplace(s+h,"<0>");
	    h += 3;
	}else if( iscntrl(s[h])){
	    c = s[h];
	  /*ecReplace(s+h,"<CTRL> ");*/
	    ecReplace(s+h,"^ ");
	    h += 2;
	    s[h-1] = (char)'@' + c;
	}else if( isprint(s[h]))
	    h++;
	else{
	    for(c=3,q=(unsigned char)s[h];c;c--,q /= 10)
		asc[c] = (char)(q%10)+(char)'0';
	    asc[0] = '<';
	    asc[4] = '>';
	    asc[5] = '\0';
	    ecReplace(s+h,asc);
	    h+=5;
	}
    }
    *p = (char)h;
    return l;
}

int near RealPos(wnd, p, buff, pePos)
HWND wnd;
int p;
char *buff;
char pePos[];
{
    int h;
    HDC hDc;

    hDc = GetDC(wnd);
    for(h=0; p > LOWORD(GetTextExtent(hDc, buff, pePos[h])); h++)
	if( *(buff+pePos[h]) == '\0'){
	    h++;
	    break;
	}
    h--;
    ReleaseDC(wnd, hDc);
    return h;
}

// some static data
PAINTSTRUCT ps;
TEXTMETRIC  tm;

char		peText[5*ESCAPELEN+2];
char		pePos[ESCAPELEN+2];
int		ecShift;
int		ecControl;
int		ecReturn;
int		ecTab;
int		ecBackSpace;
int		inNumberCapture;
unsigned int	NumberCaptured;

#define TEXTBUFF	0
#define CHARPOS		2
#define FIRSTCHAR	4

#define SetEditBuff(wnd,bf)	SetWindowWord(wnd,TEXTBUFF, (WORD)(bf))
#define SetEditPos(wnd,h)	SetWindowWord(wnd,CHARPOS, (WORD)(h))
#define SetEditLeft(wnd,f)	SetWindowWord(wnd,FIRSTCHAR, (WORD)(f))

#define EditBuff(wnd)	  (char near *) GetWindowWord(wnd, TEXTBUFF)
#define EditPos(wnd)		GetWindowWord(wnd, CHARPOS)
#define EditLeft(wnd)		GetWindowWord(wnd, FIRSTCHAR)

void near PutCaret(wnd,x)
HWND wnd;
int x; /* numero de caracter */
{
    RECT    rect;
    int	    pos, relPos;  //   position of caret relative to EditLeft.
    HDC	    hDc;

    GetClientRect(wnd,(LPRECT)&rect);
    hDc = GetDC(wnd);
    lstrcpy( (LPSTR)peText,(LPSTR)EditBuff(wnd));
    Printable(peText,pePos);

    while(1)
    {
        if((int)(relPos = x-EditLeft(wnd)) < 0)
    	    SetEditLeft(wnd, EditLeft(wnd)-1);
        else 
        {
        	pos = LOWORD(GetTextExtent(hDc, peText+EditLeft(wnd), relPos))
                            + 1;
        	if(pos >= rect.right - rect.left)
	            SetEditLeft(wnd, EditLeft(wnd)+1);
            else
                break;
        }
	    InvalidateRect(wnd,NULL,TRUE);
    }

    ReleaseDC(wnd, hDc);
    SetCaretPos(pos, 1);
}

void near ecCopy(wnd)
HWND wnd;
{
    HANDLE clp;

    OpenClipboard(wnd);
    EmptyClipboard();
    clp = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,(DWORD)(ESCAPELEN+2));
    lstrcpy(GlobalLock(clp),(LPSTR)EditBuff(wnd));
    GlobalUnlock(clp);
    SetClipboardData(CF_TEXT,clp);
    CloseClipboard();
}

void near ecClear(wnd)
HWND wnd;
{
    *EditBuff(wnd) = '\0';
    SetEditPos(wnd,0);
    PutCaret(wnd, 0);
    InvalidateRect(wnd,NULL,TRUE);
}

void near ecCut(wnd)
HWND wnd;
{
    ecCopy(wnd);
    ecClear(wnd);
}

void near ecPaste(wnd)
HWND wnd;
{
    HANDLE  clp;
    LPSTR   aux;
    register int     i;

    OpenClipboard(wnd);
    if(clp = GetClipboardData(CF_TEXT)){
	aux = GlobalLock(clp);
	if(lstrlen(aux)+lstrlen(EditBuff(wnd))>ESCAPELEN)
	    MessageBeep(MB_OK);
	else{
	    for(i = 0; *(aux+i);i++){
		ecInsert(EditBuff(wnd)+EditPos(wnd),*(aux+i));
		SetEditPos(wnd, EditPos(wnd)+1);
	    }
	    lstrcpy(peText,EditBuff(wnd));
	    Printable(peText,pePos);
	    PutCaret(wnd, pePos[EditPos(wnd)]);
	    InvalidateRect(wnd,NULL,TRUE);
	    UpdateWindow(wnd);
	}
	GlobalUnlock(clp);
    }
    CloseClipboard();
}

void near ecSysVirtual(wnd,wp)
HWND wnd;
WORD wp;
{
    int p;
    static WORD SysKeys[] = {VK_INSERT,VK_END,VK_DOWN,VK_NEXT,
	VK_LEFT,VK_CLEAR,VK_RIGHT,VK_HOME,VK_UP,VK_PRIOR};

    for(p=0;p<=9;p++)
	if(SysKeys[p] == wp)
	    break;
    if(p == 10)
	return;
    inNumberCapture = TRUE;
    if(NumberCaptured < 0xff)
	NumberCaptured = NumberCaptured * 10 + p;
    if(NumberCaptured > 0xff)
	NumberCaptured = NumberCaptured % 0xFF;
}

void near ecVirtual(wnd,wp)
HWND wnd;
WORD wp;
{
    int l;
    HWND foco;

    switch(wp){

	case VK_SHIFT:
	    ecShift = 1;
	    break;

	case VK_CONTROL:
	    ecControl = 1;
	    break;

	case VK_LEFT:
	    if(EditPos(wnd) <=0){
		SetEditPos(wnd, 0);
		PutCaret(wnd, 0);
		MessageBeep(MB_OK);
		break;
	    }
	    lstrcpy((LPSTR)peText,(LPSTR)EditBuff(wnd));
	    Printable(peText,pePos);
	    SetEditPos(wnd, EditPos(wnd)-1);
	    PutCaret(wnd, pePos[EditPos(wnd)]);
	    break;

	case VK_RIGHT:
	    lstrcpy((LPSTR)peText,(LPSTR)EditBuff(wnd));
	    if(EditPos(wnd) >= (l=Printable(peText,pePos))){
		SetEditPos(wnd,l);
		PutCaret(wnd,pePos[l]);
		MessageBeep(MB_OK);
		break;
	    }
	    SetEditPos(wnd, EditPos(wnd)+1);
	    PutCaret(wnd, pePos[EditPos(wnd)]);
	    break;

	case VK_HOME:
	    lstrcpy((LPSTR)peText,(LPSTR)EditBuff(wnd));
	    Printable(peText,pePos);
	    SetEditPos(wnd, 0);
	    PutCaret(wnd, 0);
	    break;

	case VK_END:
	    lstrcpy((LPSTR)peText,(LPSTR)EditBuff(wnd));
	    l = Printable(peText,pePos);
	    SetEditPos(wnd, l);
	    PutCaret(wnd, pePos[l]);
	    break;

	case VK_TAB:
	    foco = GetNextDlgTabItem(GetParent(wnd), wnd, ecShift);
	    ecTab = 1;
	    if(foco)
		SetFocus(foco);
	    break;

	case VK_BACK:
	    ecBackSpace = 1;
	    break;

	case VK_RETURN:
	    ecReturn = 1;
	    SendMessage(GetParent(wnd),WM_COMMAND,IDOK,0L);
	    break;

	case VK_INSERT:
	    if(ecShift)
		ecPaste(wnd);
	    else if(ecControl)
		ecCopy(wnd);
	    break;

	case VK_DELETE:
	    if(ecShift)
		ecCut(wnd);
	    else
		ecClear(wnd);
	    break;
    }
}

void near ecInput(wnd, c)
HWND wnd;
unsigned int c;
{
    switch(c){
	case '\x08':	/* backspace */
	    if(ecBackSpace){
		if(EditPos(wnd) <= 0)
		    MessageBeep(MB_OK);
		else{
		    ecDelete(EditBuff(wnd)+EditPos(wnd)-1);
		    SetEditPos(wnd,EditPos(wnd)-1);
		    lstrcpy((LPSTR)peText,(LPSTR)EditBuff(wnd));
		    Printable(peText,pePos);
		    PutCaret(wnd,pePos[EditPos(wnd)]);
		}
		InvalidateRect(wnd,NULL,TRUE);
		ecBackSpace = 0;
		break;
	    }

	default:
	    if(c == '\011' && ecTab){
		ecTab = 0;
		break;
	    }
	    if(c == '\015' && ecReturn){
		ecReturn = 0;
	    }
	    if(lstrlen((LPSTR)EditBuff(wnd)) >= ESCAPELEN){
		MessageBeep(MB_OK);
		break;
	    }
	    if(inNumberCapture){
		inNumberCapture = FALSE;
		c = NumberCaptured;
	    }
	    if(c == 0)
		c = (unsigned char)'\xFF';
	    ecInsert(EditBuff(wnd)+EditPos(wnd),(char)c);
	    SetEditPos(wnd,EditPos(wnd)+1);
	    lstrcpy((LPSTR)peText,(LPSTR)EditBuff(wnd));
	    Printable(peText,pePos);
	    PutCaret(wnd,pePos[EditPos(wnd)]);
	    InvalidateRect(wnd,NULL,TRUE);
	    break;
    }
}

LONG FAR PASCAL PseudoEdit(wnd, mess, wp, lp)
HWND wnd;
unsigned mess;
WORD wp;
DWORD lp;
{
    LPSTR buff;
    HDC dc;
    int h;

    switch(mess){

	case WM_KEYDOWN:
	    ecVirtual(wnd, wp);
	    break;

	case WM_KEYUP:
	    switch(wp){
		case VK_SHIFT:
		    ecShift = 0;
		    break;
	    }
	    break;

	case WM_SYSKEYDOWN:
	    if(wp == VK_MENU){
		NumberCaptured = 0;
		inNumberCapture = 0;
	    }else
		ecSysVirtual(wnd,wp);
	    break;

	case WM_SYSKEYUP:
	    if( (wp == VK_MENU) && inNumberCapture && !NumberCaptured){
		ecInput(wnd, wp);
	    }
	    break;

	case WM_CHAR:
	    ecInput(wnd, wp);
	    break;

	case WM_CREATE:
	    buff = (char near *)LocalAlloc(LMEM_FIXED|LMEM_ZEROINIT, 
                                                ESCAPELEN+1);
	    // change casting so compliler won't complain.  We're
	    // truncating the buffer pointer on the assumption the
	    // selector will be OK!
	    SetEditBuff(wnd, (WORD)(LONG)(LPSTR)buff);
	    SetEditPos(wnd,0);
	    SetEditLeft(wnd,0);
	    ecShift = 0;
	    ecTab = 0;
	    ecReturn = 0;
	    ecBackSpace = 0;
	    break;

	case WM_DESTROY:
	    LocalFree((HANDLE)EditBuff(wnd));
	    break;

	case WM_LBUTTONDOWN:
	    if(GetFocus() != wnd)
		SetFocus(wnd);

	    dc = GetDC(wnd);
	    GetTextMetrics(dc,(LPTEXTMETRIC)&tm);
	    ReleaseDC(wnd,dc);
	    lstrcpy((LPSTR)peText,(LPSTR)EditBuff(wnd));
	    Printable(peText, pePos);
	    h = RealPos(wnd, LOWORD(lp), peText, pePos);
	    PutCaret(wnd, pePos[h]);
	    SetEditPos(wnd, h);
	    break;

	case WM_SETFOCUS:
	    dc = GetDC(wnd);
	    GetTextMetrics(dc,(LPTEXTMETRIC)&tm);
	    ReleaseDC(wnd,dc);
	    h = tm.tmHeight + tm.tmExternalLeading;
	    CreateCaret(wnd, NULL, 1, h);
	    lstrcpy((LPSTR)peText,(LPSTR)EditBuff(wnd));
	    Printable(peText,pePos);
	    PutCaret(wnd, pePos[EditPos(wnd)]);
	    ShowCaret(wnd);
	    SendMessage(GetParent(wnd),WM_COMMAND,
		GetWindowWord(wnd,GWW_ID),MAKELONG(wnd, EN_SETFOCUS));
	    break;

	case WM_KILLFOCUS:
	    HideCaret(wnd);
	    DestroyCaret();
	    ecBackSpace = 0;
	    SendMessage(GetParent(wnd), WM_COMMAND,
		GetWindowWord(wnd,GWW_ID), MAKELONG(wnd, EN_KILLFOCUS));
	    break;

	case WM_PAINT:
	    BeginPaint(wnd ,(LPPAINTSTRUCT)&ps);
	    lstrcpy((LPSTR)peText,(LPSTR)EditBuff(wnd));
	    Printable(peText,pePos);
	    TextOut(ps.hdc, 1, 1, (LPSTR)peText+EditLeft(wnd),
		lstrlen((LPSTR)(peText+EditLeft(wnd))));
	    EndPaint(wnd ,(LPPAINTSTRUCT)&ps);
	    break;

	case WM_GETDLGCODE:
	    return DLGC_WANTALLKEYS;

	case WM_GETTEXT:
	    for(h=0;*(EditBuff(wnd)+h);h++){
		if(h >= (wp-1)){
		    *((LPSTR)lp+wp) = '\0';
		    return h;
		}
		*((LPSTR)lp+h) = *(EditBuff(wnd)+h);
	    }
	    *((LPSTR)lp+h) = '\0';
	    return h;

	case WM_SETTEXT:
	    for(h=0;h<ESCAPELEN && *((LPSTR)lp+h);h++)
		*(EditBuff(wnd)+h) = *((LPSTR)lp+h);
	    *(EditBuff(wnd)+h) = '\0';
	    if(EditPos(wnd) > lstrlen((LPSTR)EditBuff(wnd)))
		SetEditPos(wnd,lstrlen((LPSTR)EditBuff(wnd)));
	    InvalidateRect(wnd,NULL,TRUE);
	    return h;

	default:
	    return DefWindowProc(wnd,mess,wp,lp);
    }
    return 0;
}
