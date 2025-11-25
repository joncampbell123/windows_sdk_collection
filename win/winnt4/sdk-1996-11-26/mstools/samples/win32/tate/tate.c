/****************************************************************************

    PROGRAM: Tate.c

    目的: 縦書き用フォントのためのサンプル

    関数: WinMain()         - 初期化関数の呼び出しと、メッセージループの処理を
                              行います。
          InitApplication() - ウィンドウデータの初期化と、ウィンドウの登録を行
                              います。
          InitInstance()    - インスタンスハンドルを保存して、メインウィンドウ
                              を作成します。
          MainWndProc()     - メインメッセージを処理します。
          AbortProc()       - 印刷中止のためのメッセージを処理します。
          AbortDlg()        - 印刷中止ダイアログボックスのためのメッセージを処
                              理します。
          OpenDlg()         - ユーザーがファイルを選択できるようにします。
          About()           - [バージョン情報]ダイアログボックスのメッセージを
                              処理します。
          GetPrinterDC()    - プリンタのデバイスコンテキストを取得します。
          GetUseFont()      - 使用するフォントを取得します。
          StringOut()       - 文字列を表示します。
          RedrawText()      - 文字列を再表示します。
          RefreshText()     - 文字列の調整を行います。
          PrintOut()        - 文字列を印刷します。
          SetInitPosition() - 表示位置やバッファを初期化します。
          SetTitle()        - ウィンドウのタイトルを設定します。
          f_gets()          - ファイルの読み取りを行います。

****************************************************************************/

#include "windows.h"
#include "windowsx.h"
#include "tate.h"
#include "string.h"
#include "ime.h"
#include "commdlg.h"

HINSTANCE hInst;			/* アプリケーションのインスタンスハンドル */
HANDLE hHourGlass;			/* 砂時計カーソルのハンドル               */
HFONT hCurrentFont;			/* 使用するフォントのハンドル             */
BYTE szMsgBuf[MAXBUF];		/* メッセージボックスのメッセージ         */
BYTE szCapString[30] = "縦書きサンプル - ";/* ウィンドウのタイトル        */
BYTE aszString[LINENUM][BUFSIZE];	   /* 入力文字列格納用バッファ    */
WORD wCurrentAngle = 2700;	/* 文字の回転角度                         */
UINT wScrnOutX = 10;		/* 水平方向の文字表示位置                 */
UINT wScrnOutY = 10;		/* 垂直方向の文字表示位置                 */
WORD wCurrentLine = 0;		/* 入力文字列の行                         */
WORD wCurrentPos = 0;		/* 入力文字列の桁                         */
WORD wWndWidth;				/* クライアント領域の幅                   */
BOOL bVerticalFlag = FALSE;	/* 縦書きを示すフラグ                     */
BOOL bAbort;				/* 印刷中止フラグ                         */
BOOL bPrintable = FALSE;	/* プリンタフォントフラグ                 */
BOOL bTrueType = FALSE; 	/* TrueType選択フラグ                     */
BOOL bSystemFont = TRUE;	/* システムフォント選択フラグ             */

BYTE szOpenName[256] = {'\0'};	/* 開いているファイルの名前 */
BYTE szDefPath[256] = {'\0'};	/* パス名 */
BYTE szFileTitle[128];
BYTE szFilter[256];

OFSTRUCT OfStruct;	    	/* オープンファイル構造体                      */
HWND  hAbortDlgWnd;	    	/* 印刷中止ダイアログボックスのハンドル        */
DLGPROC lpAbortDlg;	    	/* AbortDlgプロシージャインスタンスのアドレス  */
FARPROC lpAbortProc;		/* AbortProcプロシージャインスタンスのアドレス */
HANDLE	hIME;		    	/* IMEインターフェイス用構造体のメモリハンドル */
LOGFONT LogFont;	    	/* フォント属性構造体                          */
CHOOSEFONT cf;		    	/* フォント選択構造体                          */
OPENFILENAME ofn;	    	/* コモンダイアログ用構造体                    */
COLORREF rgbColors = RGB(0, 0, 0);
                              /* テキストカラー */

/****************************************************************************

    関数: WinMain(HANDLE, HANDLE, LPSTR, int)

    目的: 初期化関数の呼び出しと、メッセージ ループの処理を行います。

****************************************************************************/

int PASCAL WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	LPSTR		lpCmdLine,
	int			nCmdShow)
{
    MSG msg;

    if (!hPrevInstance)			 		/* ほかのインスタンスが実行中か?    */
		if (!InitApplication(hInstance)) /* 共通の初期化処理                 */
	    	return (FALSE);				/* 初期化に失敗した場合は終了       */

    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);	   /* 仮想キーコードの変換		     */
		DispatchMessage(&msg);	   /* ウィンドウにメッセージをディスパッチ   */
    }
    return (msg.wParam);	   /* PostQuitMessageの戻り値を返す          */
}


/****************************************************************************

    関数: InitApplication(HANDLE)

    目的: ウィンドウデータの初期化と、ウィンドウクラスの登録を行います。

****************************************************************************/

BOOL InitApplication(
	HINSTANCE hInstance)
{
    WNDCLASS  wc;

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH); 
    wc.lpszMenuName =  "TateMenu";
    wc.lpszClassName = "TateWClass";

    return (RegisterClass(&wc));
}


/****************************************************************************

    関数: InitInstance(HANDLE, int)

    目的: インスタンスハンドルを保存して、メインウィンドウを作成します。

****************************************************************************/

BOOL InitInstance(
    HINSTANCE       hInstance,       /* 現在のインスタンス識別子            */
    int             nCmdShow)        /* 最初のShowWindow()呼び出しの引数    */
{
    HWND            hWnd;            /* メインウィンドウのハンドル          */

    hInst = hInstance;

    hWnd = CreateWindow(
        "TateWClass",                  /* RegisterClass()呼び出しを参照     */
        szCapString,                   /* ウィンドウタイトル バーのテキスト */
        WS_OVERLAPPEDWINDOW,           /* ウィンドウスタイル                */
        CW_USEDEFAULT,                 /* デフォルトの水平位置              */
        CW_USEDEFAULT,                 /* デフォルトの垂直位置              */
        CW_USEDEFAULT,                 /* デフォルトの幅                    */
        CW_USEDEFAULT,                 /* デフォルトの高さ                  */
        0,                             /* オーバーラップウィンドウは        */
                                       /* 親ウィンドウを持たない            */
        0,                             /* ウィンドウクラスメニューを使用    */
        hInstance,                     /* このインスタンスが                */
                                       /* このウィンドウを使用              */
        0                              /* ポインタは不要                    */
    );

    if (!hWnd)
        return (FALSE);

    hHourGlass = LoadCursor(NULL, IDC_WAIT);

    ShowWindow(hWnd, nCmdShow);  /* ウィンドウ表示                         */
    UpdateWindow(hWnd);          /* WM_PAINTメッセージを送る               */
    return (TRUE);               /* PostQuitMessageの戻り値を返す          */
}


/****************************************************************************

    関数: GetPrinterDC(HWND, int)

    目的: プリンタのデバイスコンテキストを取得します。

****************************************************************************/

HANDLE GetPrinterDC(
	HWND hWnd,
	int flg)
{
    CHAR	pPrintInfo[80];
    LPSTR	lpTemp;
    LPSTR	lpPrintType;
    LPSTR	lpPrintDriver;
    LPSTR	lpPrintPort;
    HANDLE	hDriver;
    FARPROC lpDeviceMode;

    /* GetProfileString関数を使って現在のプリンタの情報を取得 */
    if (!GetProfileString("windows", "device", (LPSTR)NULL, pPrintInfo, 80)) {
		LoadString(hInst, STR_NOPRNTDRV, szMsgBuf, MAXBUF);
		MessageBox(hWnd, szMsgBuf, NULL, MB_OK | MB_ICONQUESTION);
		return (NULL);
    }
    /* 取得した[windows]セクションのdevice=フィールド */
    /* の文字列を各情報に分割                         */
    lpTemp = lpPrintType = pPrintInfo;
    lpPrintDriver = lpPrintPort = NULL;
    while (*lpTemp) {
		if (*lpTemp == ',') {
	    	*lpTemp++ = '\0';
	    	while (*lpTemp == ' ')
				lpTemp = CharNext(lpTemp);
	    	if (!lpPrintDriver)
				lpPrintDriver = lpTemp;
	    	else {
				lpPrintPort = lpTemp;
				break;
	    	}
		}
		else
			lpTemp = CharNext(lpTemp);
    }
    /* プリンタデバイスコンテキストの作成 */
    if (flg == 0) {
		return (CreateDC(lpPrintDriver, lpPrintType, lpPrintPort, 0));
    }
	else {
        /* プリンタの設定 */
		lstrcat(lpPrintDriver, ".DRV");
		if (!(hDriver = LoadLibrary(lpPrintDriver))) {
			LoadString(hInst, STR_NOTLOADPRNTDRV, szMsgBuf, MAXBUF);
			MessageBox(hWnd, szMsgBuf, NULL, MB_OK | MB_ICONQUESTION);
			return (NULL);
		}	
		if (!(lpDeviceMode = GetProcAddress(hDriver, "DEVICEMODE"))) {
			LoadString(hInst, STR_NOTGETADDRESS, szMsgBuf, MAXBUF);
			MessageBox(hWnd, szMsgBuf, NULL, MB_OK | MB_ICONQUESTION);
			FreeLibrary(hDriver);
			return (NULL);
		}	
		if (lpDeviceMode != NULL) {
	    	(*lpDeviceMode)(hWnd, hDriver, lpPrintType, lpPrintPort);
		}
		FreeLibrary(hDriver);
    }
}


/****************************************************************************

    関数: GetUseFont(HWND)

    目的: 使用するフォントを取得します。

****************************************************************************/

VOID GetUseFont(
	HWND hWnd)
{
    if (!bSystemFont)
		DeleteObject(hCurrentFont);
    /* フォントの作成 */
    hCurrentFont = CreateFont(
		LogFont.lfHeight,			/* フォントのサイズ           */
		0,
		0,							/* エスケープメント(印刷方向) */
		0,							/* 文字の向き                 */
		LogFont.lfWeight,
		LogFont.lfItalic,
		LogFont.lfUnderline,
		LogFont.lfStrikeOut,
		LogFont.lfCharSet,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		LogFont.lfPitchAndFamily,
		LogFont.lfFaceName);		/* フォントフェース名 */

    /* LogFont構造体にフォントの情報をセット */
    GetObject(hCurrentFont, sizeof(LOGFONT), &LogFont);

    /* 縦書き用フォントを選択したときはエスケープメントと */
    /* 文字の向きを270度に設定                            */
    DeleteObject(hCurrentFont);
	if(LogFont.lfFaceName[0] == '@' && wCurrentAngle == 2700) {
		LogFont.lfEscapement = wCurrentAngle;
		LogFont.lfOrientation = wCurrentAngle;
		hCurrentFont = CreateFontIndirect(&LogFont);
		bVerticalFlag = TRUE;
    }
	else {
    	/* ラインプリンタ縦書き */
		LogFont.lfEscapement = 0;
		LogFont.lfOrientation = 0;
		hCurrentFont = CreateFontIndirect(&LogFont);
		bVerticalFlag = FALSE;
    }
}


/****************************************************************************

    関数: SetInitPosition( HWND )

    目的: 表示位置やバッファを初期化します。

****************************************************************************/

VOID SetInitPosition(
	HWND hWnd)
{
    INT	i;

    wCurrentLine = 0;
    wCurrentPos = 0;
    for (i = 0; i < LINENUM; ++i)
		aszString[i][0] = '\0';

    if (!bVerticalFlag)	/* 10は余白分 */
		wScrnOutX = 10;
    else
		wScrnOutX = wWndWidth - 10;
    wScrnOutY = 10;
    ImeMoveConvertWin( hWnd, wScrnOutX, wScrnOutY );
}


/****************************************************************************

    関数: SetTitle(HWND, PSTR)

    目的: ウィンドウのタイトルを設定します。

****************************************************************************/

VOID SetTitle(
	HWND hWnd,
	PSTR FontName)
{
    static BYTE szTitleString[128];		/* 表示用タイトル */

    lstrcpy(szTitleString, szCapString);
    lstrcat(szTitleString, FontName);
    SetWindowText(hWnd, szTitleString);
}


/****************************************************************************

    関数: f_gets(PSTR, int, int)

    目的: ファイルを1行(CR/LFまで)読み取ります。

*****************************************************************************/

INT f_gets(
	CHAR TmpStr[],
	INT Length,
	HFILE hFile)
{
	INT i, ret;

    /* ファイルから1文字ずつ読み取り、行末またはEOFに */
    /* 達したら文字数を戻り値として上位関数へ返す     */
    for (i = 0; i < Length-1; ++i) {
		ret = _lread(hFile, &TmpStr[i], 1);
		if (ret <= 0)
			return ret;
		if (TmpStr[i] == 0x0d)
			TmpStr[i] = '\0';
		if (TmpStr[i] == 0x0a || TmpStr[i] == 0x1a)
	    	break;
    }

    TmpStr[i] = '\0';
    return (i);
}


/****************************************************************************

    関数: StringOut(HWND, PSTR)

    目的: 文字列を表示します。

****************************************************************************/

void StringOut(
	HWND hWnd,
	PSTR pStr)
{
    HDC			hDC;
    TEXTMETRIC	tm;
    HFONT		hOldFont;		/* 元のフォントのハンドル */
    SIZE		size;			/* 文字列の表示幅         */

    hDC = GetDC(hWnd);

    /* テキストカラーを設定 */
    SetTextColor(hDC, rgbColors);
    /* 現在のフォントを選択 */
    hOldFont = SelectObject(hDC, hCurrentFont);
    if (hOldFont != NULL) {
		/* フォントのサイズを取得 */
		GetTextMetrics(hDC, &tm);
		bTrueType = tm.tmPitchAndFamily & TMPF_TRUETYPE;
		/* 改行が押された場合、または行末に達した場合は改行する */
		if (*pStr == 0x0d || wCurrentPos + lstrlen(pStr) + 1 >= BUFSIZE) {
	    	wCurrentPos = 0;
	    	++wCurrentLine;
	    	/* 表示開始位置を次の行の先頭に進める */
	    	if (wCurrentLine < LINENUM) {
				if (!bVerticalFlag) {
		    		wScrnOutX  = 10;
		    		wScrnOutY += tm.tmHeight;
				}
				else {
		    		wScrnOutX -= tm.tmHeight;
		    		wScrnOutY  = 10;
				}
				ImeMoveConvertWin( hWnd, wScrnOutX, wScrnOutY );
	    	}
			else {
				*pStr = 0x0d; /* 文字列の終わり */
			}
		}

		if (*pStr != 0x0d) {
	    	/* 出力する文字列をバッファに格納 */
	    	lstrcat(aszString[wCurrentLine], pStr);
	    	wCurrentPos += lstrlen(pStr);
	    	/* 文字列の実際の表示幅と高さを取得 */
			GetTextExtentPoint(hDC, pStr, lstrlen(pStr), &size);
	    	/* 文字列を表示 */
	    	TextOut(hDC, wScrnOutX, wScrnOutY, pStr, lstrlen(pStr));
	    	/* 表示開始位置を次の文字の先頭に進める */
	    	if (!bVerticalFlag) {
				wScrnOutX += size.cx;
	    	}
			else {
				if (bTrueType)
		    		/* TrueTypeでは表示の幅と高さは用紙の向きに対応する */
		    		wScrnOutY += size.cx;
				else
		    		wScrnOutY += size.cy;
	    	}
			/* IME変換ウィンドウの位置も進める */
	    	ImeMoveConvertWin( hWnd, wScrnOutX, wScrnOutY );
		}
		/* 元のフォントに戻す */
		SelectObject(hDC, hOldFont);
    }
    ReleaseDC(hWnd, hDC);
}


/****************************************************************************

    関数: RedrawText(HWND, HDC)

    目的: 文字列を再表示します。

****************************************************************************/

VOID RedrawText(
	HWND hWnd,
	HDC hDC)
{
    TEXTMETRIC	tm;
    INT			i;
    UINT		x, y;
    HFONT		hOldFont;			/* 元のフォントのハンドル */

    /* テキストカラーを設定 */
    SetTextColor(hDC, rgbColors);
    /* 現在のフォントを選択 */
    hOldFont = SelectObject(hDC, hCurrentFont);
    if (hOldFont != NULL) {
		/* フォントのサイズを取得 */
		GetTextMetrics(hDC, &tm);
		/* 行ごとにバッファに格納された文字列を表示 */
		if (!bVerticalFlag)
	    	x = 10;
		else
	    	x = wWndWidth - 10;
		y = 10;
		for (i = 0; i <= wCurrentLine; ++i) {
	    	TextOut(hDC, x, y, aszString[i], lstrlen(aszString[i]));
	    	if (!bVerticalFlag)
				y += tm.tmHeight;
	    	else
				x -= tm.tmHeight;
		}
		/* 縦書きの場合入力位置を再設定 */
    	if(bVerticalFlag) 
    		wScrnOutX = x + tm.tmHeight;
		/* 元のフォントに戻す */
		SelectObject(hDC, hOldFont);
    }
}


/****************************************************************************

    関数: RefreshText(HWND hWnd)

    目的: フォントを変更したときにこの関数を呼び出して、画面を整えます。

****************************************************************************/

VOID RefreshText(
	HWND hWnd)
{
    HDC 		hDC;
    TEXTMETRIC	tm;
    HFONT		hOldFont;		/* 元のフォントのハンドル */
    SIZE		size;			/* 文字列の表示幅         */

    hDC = GetDC(hWnd);
    /* 現在のフォントを選択 */
    hOldFont = SelectObject(hDC, hCurrentFont);
    if (hOldFont != NULL) {
		/* フォントのサイズを取得 */
		GetTextMetrics(hDC, &tm);
		bTrueType = tm.tmPitchAndFamily & TMPF_TRUETYPE;
		/* 文字列の実際の表示幅と高さを取得 */
		GetTextExtentPoint(hDC, aszString[wCurrentLine],
						lstrlen(aszString[wCurrentLine]), &size);
		/* 次の表示開始位置を計算して求める */
		if (!bVerticalFlag) {
	    	wScrnOutX = 10 + size.cx;
	    	wScrnOutY = 10 + tm.tmHeight * wCurrentLine;
		}
		else {
	    	wScrnOutX = wWndWidth - 10 - tm.tmHeight * wCurrentLine;
	    	if (bTrueType)
				/* TrueTypeでは表示の幅と高さは用紙の向きに対応する */
				wScrnOutY = 10 + size.cx;
	    	else
				wScrnOutY = 10 + size.cy;
		}
		/* IME変換ウィンドウの位置も進める */
		ImeMoveConvertWin( hWnd, wScrnOutX, wScrnOutY );
		/* 元のフォントに戻す */
    	SelectObject(hDC, hOldFont);
    }
    ReleaseDC(hWnd, hDC);
    /* ウィンドウ全体を再描画 */
    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);
}


/****************************************************************************

    関数: PrintOut(HDC, HWND)

    目的: 文字列を印刷します。

****************************************************************************/

void PrintOut(hPrDC, hWnd)
HDC hPrDC;
HWND hWnd;
{
    INT		Px;
    INT 	Py;
    HFILE 	hFile;
    INT 	LineSpace;			/* 行間隔        */
    INT 	LinesPerPage;		/* 1ページの行数 */
    INT 	CurrentLine;		/* 現在行        */
    INT 	i;
    POINT	Point;				/* 印字サイズ    */
    HANDLE	hSaveCursor;		/* 現在のカーソルを保存するためのハンドル */
    HFONT	hOldFont;			/* 現在のフォントのハンドル               */
    static BYTE abPrintBuffer[MAXBUF];	/* ファイル印刷用バッファ */
    TEXTMETRIC tm;
	DOCINFO	diPrint;

    hSaveCursor = SetCursor(hHourGlass);

    /* 印刷中止プロシージャとダイアログプロシージャ */
    /* のインスタンスアドレスを作成                 */
    lpAbortDlg  = MakeProcInstance(AbortDlg,  hInst);
    lpAbortProc = MakeProcInstance(AbortProc, hInst);

    if (!bSystemFont)
		hOldFont = SelectObject(hPrDC, hCurrentFont);

	diPrint.cbSize = sizeof(DOCINFO);
	diPrint.lpszDocName = "Tate";
	diPrint.lpszOutput = NULL;
	
	/* 印刷中止プロシージャの設定、印刷要求 */
	if (SetAbortProc(hPrDC, (PROC)lpAbortProc) < 0
	||  StartDoc(hPrDC, &diPrint) == SP_ERROR || StartPage (hPrDC) < 0) {
		/* エラー時の処理 */
		SetCursor(hSaveCursor); 	 /* 砂時計カーソルを元のカーソルに戻す */
		LoadString(hInst, STR_CANNOTPRINT, szMsgBuf, MAXBUF);
		MessageBox(hWnd, szMsgBuf, NULL, MB_OK | MB_ICONINFORMATION);
		FreeProcInstance(AbortDlg);
		FreeProcInstance(AbortProc);
		return;
	}

	bAbort = FALSE; 					/* 印刷中止フラグをクリア */
	/* 印刷中止ダイアログボックスを作成 */
	hAbortDlgWnd = CreateDialog(hInst, "AbortDlg", hWnd, lpAbortDlg);
	ShowWindow (hAbortDlgWnd, SW_NORMAL);
    UpdateWindow (hAbortDlgWnd);
	/* メインウィンドウを無効化 */
	EnableWindow(hWnd, FALSE);

    /* 構造体tmにフォントの情報を格納 */
    GetTextMetrics(hPrDC, &tm);

    /* 行間隔はtmHeightメンバとtmExternalLeadingメンバの和 */
    LineSpace = tm.tmHeight + tm.tmExternalLeading;

    /* GetDeviceCaps関数により印字範囲のサイズを取得 */
    Point.x = GetDeviceCaps(hPrDC, HORZRES) - 20;  /* 20は右端の余白分 */
    Point.y = GetDeviceCaps(hPrDC, VERTRES);
    
	/* 1ページの行数を計算 */
    if (!bVerticalFlag)
		LinesPerPage = Point.y / LineSpace;
    else
		LinesPerPage = Point.x / LineSpace;

	/* 印字開始位置を設定 */
    if (!bVerticalFlag)
		Px = 0;
    else
		Px = Point.x;
    Py = 0;

    SetCursor(hSaveCursor);          /* 砂時計カーソルを元のカーソルに戻す */

    if (hOldFont != NULL) {
	/* 現在行を1に設定 */
	CurrentLine = 1;
	/* 開いているファイルを印刷する場合 */
	if (szOpenName[0] != '\0') {
	    /* ファイルを再び開く */
	    hFile = OpenFile((LPSTR) NULL, &OfStruct, OF_REOPEN | OF_READ);
	    for ( ; ; ) {
			if (f_gets(abPrintBuffer, MAXBUF, hFile) <= 0)
				break;
			if (abPrintBuffer[0] != '\0')
		    	TextOut(hPrDC, Px, Py, abPrintBuffer, lstrlen(abPrintBuffer));
			/* 次の印字位置を設定 */
			if (!bVerticalFlag)
		    	Py += LineSpace;
			else
		    	Px -= LineSpace;
			/* 行が1ページ分を超えた場合には改ページ */
			if (++CurrentLine > LinesPerPage) {
			    if (!EndPage (hPrDC) || StartPage (hPrDC) < 0 || bAbort)
					break;
		    	if (!bSystemFont)
					SelectObject(hPrDC, hCurrentFont);
		    	CurrentLine = 1;
		    	if (!bVerticalFlag)
					Px = 0;
		    	else
					Px = Point.x;
		    	Py = 0;
			}
	    }
	}
	else {
		/* ユーザーが入力したテキストを印刷する場合 */
	    for (i = 0; i <= wCurrentLine; ++i) {
			if (aszString[i][0] != '\0')
		    	TextOut(hPrDC, Px, Py, aszString[i], lstrlen(aszString[i]));
			if (!bVerticalFlag)
		    	Py += LineSpace;
			else
		    	Px -= LineSpace;
			if (++CurrentLine   +  1   > LinesPerPage) {
			    if (!EndPage (hPrDC) || StartPage (hPrDC) < 0 || bAbort)
					break;
		    	if (!bSystemFont)
					SelectObject(hPrDC, hCurrentFont);
		    	CurrentLine = 1;
		    	if (!bVerticalFlag)
					Px = 0;
		    	else
					Px = Point.x;
		    	Py = 0;
			}
	    }
	}
		if (!bSystemFont)
			SelectObject(hPrDC, hOldFont);
    }
    
	if (!bAbort) {
		Escape(hPrDC, NEWFRAME, 0, 0L, 0L);
		Escape(hPrDC, ENDDOC, 0, 0L, 0L);
    }
	
    /* メインウィンドウを有効化して印刷中止ダイアログを破棄 */
    EnableWindow(hWnd, TRUE);
    DestroyWindow(hAbortDlgWnd);
    FreeProcInstance(AbortDlg);
    FreeProcInstance(AbortProc);
}


/****************************************************************************

    関数: AbortProc(HDC, int)

    目的: 印刷中止のためのメッセージを処理します。

****************************************************************************/

int FAR PASCAL AbortProc(
	HDC hPr,                        /* 複数のプリンタディスプレイ */
                                    /* コンテキストに対応         */
	INT Code)                       /* 印刷のステータス           */
{
    MSG msg;

    /* 印刷中止ダイアログボックスのメッセージ処理 */
    while (!bAbort && PeekMessage(&msg, 0, 0, 0, TRUE)) {
        if (!IsDialogMessage(hAbortDlgWnd, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
	}	
    /* ユーザーが中止を選択した場合には、bAbortはTRUE */
    return (!bAbort);
}


/****************************************************************************

    関数:       AbortDlg(HWND, unsigned, WORD, LONG)

    目的:       印刷中止ダイアログボックスのためのメッセージを処理します。

    メッセージ: WM_INITDIALOG - ダイアログボックスの初期化
                WM_COMMAND    - 入力を受け取ったとき

    コメント:

        このダイアログボックスは印刷中に作成され、これによりユーザーは印刷
        処理を途中で中止させることができます。

****************************************************************************/

int FAR PASCAL AbortDlg(
	HWND	hDlg,
	UINT	msg,
	WPARAM	wParam,
	LPARAM	lParam)
{
    switch(msg) {

        /* 印刷中止のメッセージ([ｷｬﾝｾﾙ]ボタン)により */
        /* bAbortをTRUEにセット                      */
        case WM_COMMAND:
            return (bAbort = TRUE);

        case WM_INITDIALOG:
            /* ダイアログの[ｷｬﾝｾﾙ]ボタンにフォーカスを設定 */
            SetFocus(GetDlgItem(hDlg, IDCANCEL));
	    	if (szOpenName[0] != '\0')
				SetDlgItemText(hDlg, IDC_FILENAME, (LPSTR)szOpenName);
	    	else
				SetDlgItemText(hDlg, IDC_FILENAME, (LPSTR)"テキストを");
            return (TRUE);
    }
    return (FALSE);
}


/****************************************************************************

    関数:       MainWndProc(HWND, unsigned, WORD, LONG)

    目的:       メインメッセージを処理します。

    メッセージ: WM_CREATE     - メインウィンドウを開くときの設定
                WM_PAINT      - ウィンドウの再描画
                WM_COMMAND    - アプリケーションメニュー
                WM_CHAR       - 文字入力
                WM_DESTROY    - ウィンドウの破棄

****************************************************************************/

LRESULT FAR PASCAL MainWndProc(
	HWND hWnd,							/* ウィンドウハンドル 			 */
	UINT message,						/* メッセージのタイプ 			 */
	WPARAM wParam,						/* 付加情報						 */
	LPARAM lParam)						/* 付加情報						 */
{
    DLGPROC lpProcAbout;		/* About関数を指すポインタ	     */
    HDC		hDC;
    HDC 	hPrDC;
    PAINTSTRUCT ps;
    HFILE 	hFile;
    INT 	len;
    HANDLE hSaveCursor;         /* 現在のカーソルを保存するためのハンドル */

    switch (message) {
	case WM_CREATE:		/* メインウィンドウを開くときの設定 */
	    hCurrentFont = GetStockObject(SYSTEM_FONT);

	    SetTitle(hWnd, "システム");

	    EnableMenuItem(GetMenu(hWnd), 2,MF_BYPOSITION | MF_GRAYED);
	    DrawMenuBar(hWnd);

	    hIME = GlobalAlloc( GHND, (LONG)sizeof(IMESTRUCT) );
	    break;

	case WM_SIZE:		/* ウィンドウのサイズが変更されたときの処理 */
	    wWndWidth  = LOWORD(lParam);
	    ImeMoveConvertWin( hWnd, wScrnOutX, wScrnOutY );
	    break;

	case WM_MOVE:		/* ウィンドウが移動したときの処理 */
	    ImeMoveConvertWin( hWnd, wScrnOutX, wScrnOutY );
	    break;
	    
	case WM_PAINT:		/* ウィンドウの再描画 */
	    hDC = BeginPaint(hWnd, &ps);
	    RedrawText(hWnd, hDC);
	    EndPaint(hWnd, &ps);
	    ImeMoveConvertWin( hWnd, wScrnOutX, wScrnOutY );
	    break;

	case WM_COMMAND:
		switch(GET_WM_COMMAND_ID(wParam, lParam)) {
		case IDM_ABOUT:      /* [バージョン情報]ダイアログ   */
                                     /* ボックスを開く               */
		    lpProcAbout = MakeProcInstance(About, hInst);
		    DialogBox(hInst, "AboutBox", hWnd, lpProcAbout);
		    FreeProcInstance(lpProcAbout);
		    break;

		case IDM_PRINT:      /* 印刷項目の選択 */
		    hPrDC = GetPrinterDC(hWnd, 0);
		    if (!hPrDC)
				return (0);
		    if (bPrintable || bSystemFont) {
				PrintOut(hPrDC, hWnd);
		    }
			else {
				LoadString(hInst, STR_SCREENONLY, szMsgBuf, MAXBUF);
				MessageBox(hWnd, szMsgBuf, NULL, MB_OK | MB_ICONINFORMATION);
		    }
		    DeleteDC(hPrDC);
		    break;

		case IDM_PRNTSET:    /* プリンタの設定 */
		    GetPrinterDC(hWnd, 1);
		    break;

		case IDM_OPEN:       /* ファイルを開く */
		    hFile = OpenDlg(hWnd);
		    /* ファイルを開いたらその内容をバッファに読み込む */
		    if (hFile != 0) {
				hSaveCursor = SetCursor(hHourGlass);
				SetInitPosition( hWnd );
				for (wCurrentLine = 0; wCurrentLine < 50; ++wCurrentLine) {
			    	if ((f_gets(aszString[wCurrentLine],BUFSIZE, hFile)) <= 0)
						break;
				}
				_lclose(hFile);
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
				SetCursor(hSaveCursor);
		    }
		    break;

		case IDM_SELECT:     /* フォントの選択 */
		    {
			BOOL fNoErr;
			HDC  hPrDC;
			
			memset(&cf, 0, sizeof(CHOOSEFONT));
			cf.lStructSize = sizeof(CHOOSEFONT);
			cf.hwndOwner = hWnd;
			cf.lpLogFont = &LogFont;
			
			if (hPrDC = GetPrinterDC(hWnd, 0)) {
			    cf.Flags = CF_EFFECTS | CF_BOTH | CF_INITTOLOGFONTSTRUCT;
			    cf.hDC = hPrDC;
			}
			else {
 			    cf.Flags = CF_EFFECTS | CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;			}
			
			fNoErr = ChooseFont(&cf);
			
			if (hPrDC)
				DeleteDC(hPrDC);
			
			if (!fNoErr)
			    break;
			
			rgbColors = cf.rgbColors;
			bPrintable = cf.nFontType & PRINTER_FONTTYPE ? TRUE : FALSE;
			bSystemFont = FALSE;

		    /* 使用するフォントを作成 */
		    GetUseFont(hWnd);
		    SetTitle(hWnd, LogFont.lfFaceName);
		    
			/* 縦書き用フォントの場合はモードを選択できるようにする */
		    if (LogFont.lfFaceName[0] == '@')
				EnableMenuItem(GetMenu(hWnd), 2, MF_BYPOSITION | MF_ENABLED);
			else
				EnableMenuItem(GetMenu(hWnd), 2, MF_BYPOSITION | MF_GRAYED);
		    DrawMenuBar(hWnd);

		    RefreshText(hWnd);
		    
			break;
			}
		case IDM_SYSTEM:     /* システムフォントを選択 */
		    if (!bSystemFont)
				DeleteObject(hCurrentFont);
		    hCurrentFont = GetStockObject(SYSTEM_FONT);
		    GetObject(hCurrentFont, sizeof(LOGFONT), (LPSTR) &LogFont);
		    SetTitle(hWnd, "システム");
		    /* システムフォントの場合には縦書きは不可 */
		    EnableMenuItem(GetMenu(hWnd), 2, MF_BYPOSITION | MF_GRAYED);
		    DrawMenuBar(hWnd);
		    bVerticalFlag = FALSE;
		    bPrintable = FALSE;
		    bSystemFont = TRUE;

		    RefreshText(hWnd);
		    break;

		case IDM_SYSTEMFIXED:/* システム固定フォントを選択 */
		    if (!bSystemFont)
				DeleteObject(hCurrentFont);
		    hCurrentFont = GetStockObject(SYSTEM_FIXED_FONT);
		    GetObject(hCurrentFont, sizeof(LOGFONT), (LPSTR) &LogFont);
		    SetTitle(hWnd, "システム固定");
		    /* システム固定フォントの場合には縦書きは不可 */
		    EnableMenuItem(GetMenu(hWnd), 2, MF_BYPOSITION | MF_GRAYED);
		    DrawMenuBar(hWnd);
		    bVerticalFlag = FALSE;
		    bPrintable    = FALSE;
		    bSystemFont = TRUE;

		    RefreshText(hWnd);
		    break;

		case IDM_LINEPRNT:   /* ラインプリンタの方向で表示または印刷 */
		    CheckMenuItem(GetMenu(hWnd), IDM_LINEPRNT, MF_CHECKED);
		    CheckMenuItem(GetMenu(hWnd), IDM_NVERTIC, MF_UNCHECKED);
		    wCurrentAngle = 0;
		    GetUseFont(hWnd);
		    RefreshText(hWnd);
		    break;

		case IDM_NVERTIC:   /* 通常の縦書き方向で表示または印刷 */
		    CheckMenuItem(GetMenu(hWnd), IDM_NVERTIC, MF_CHECKED);
		    CheckMenuItem(GetMenu(hWnd), IDM_LINEPRNT, MF_UNCHECKED);
		    wCurrentAngle = 2700;
		    GetUseFont(hWnd);
		    RefreshText(hWnd);
		    break;

		case IDM_CLEAR:     /* 画面をクリア */
		    SetInitPosition( hWnd );
		    szOpenName[0] = '\0';
		    InvalidateRect(hWnd, 0, TRUE);
		    UpdateWindow(hWnd);
		    break;

		case IDM_QUIT:      /* 終了 */
		    if (!bSystemFont)
				DeleteObject(hCurrentFont);
		    PostQuitMessage(0);
		    break;

		default:		       /* Windowsがメッセージを処理 */
		    return (DefWindowProc(hWnd, message, wParam, lParam));
	    }
	    break;

	case WM_CHAR:		/* 文字が入力されたら1文字ずつ処理 */
	{
	    static BOOL KanjiFlag = FALSE;   /* 入力文字が漢字の場合のフラグ */
	    static BYTE szOutStr[3] = {'\0'}; /* 出力文字用バッファ */
	
	    /* 入力文字列用バッファに格納 */
	    if (wCurrentLine >= LINENUM)
			break;

	    /* 漢字の第1バイトの場合は第2バイト目まで */
	    /* 待ってからStringOut関数に送る          */
	    if (!KanjiFlag && IsDBCSLeadByte((BYTE)wParam)) {
			szOutStr[0] = (CHAR) wParam;
			szOutStr[1] = '\0';
			KanjiFlag = TRUE;
	    }
		else {
            if(!KanjiFlag && !isprint(wParam))
                break;
			len = lstrlen(szOutStr);
			szOutStr[len] = (char) wParam;
			szOutStr[len + 1] = '\0';

			StringOut(hWnd, szOutStr);

			szOutStr[0] = '\0';
			KanjiFlag = FALSE;
	    }
	    break;
	}

	case WM_KILLFOCUS:
	    ImeMoveConvertWin( hWnd, -1, -1 );
	    break;

	case WM_DESTROY:	/* ウィンドウを廃棄 */
	    if (!bSystemFont)
			DeleteObject(hCurrentFont);
	    PostQuitMessage(0);
	    GlobalFree( hIME );
	    break;

	default:			  /* 未処理メッセージはそのまま渡す */
	    return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (0);
}


/****************************************************************************

    関数: OpenDlg(HWND)

    目的: ユーザーがファイルを選択できるようにします。

****************************************************************************/

HFILE OpenDlg(
	HWND hWnd)
{
    WORD	wCnt;
    BYTE	chReplace;
    HFILE	hFile;

    if ((wCnt = LoadString(hInst, STR_FILTERSTRING,
							szFilter, sizeof(szFilter))) == 0) {
		return 0;
    }
    chReplace = szFilter[wCnt - 1];
    for (wCnt = 0; szFilter[wCnt]; wCnt++) {
		if (szFilter[wCnt] == chReplace)
	    	szFilter[wCnt] = '\0';
    }
    memset(&ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize     = sizeof(OPENFILENAME);
    ofn.hwndOwner       = hWnd;
    ofn.lpstrFilter     = szFilter;
    ofn.nFilterIndex    = 1;
    ofn.lpstrFile       = szOpenName;
    ofn.nMaxFile        = sizeof(szOpenName);
    ofn.lpstrFileTitle  = szFileTitle;
    ofn.nMaxFileTitle   = sizeof(szFileTitle);
    ofn.lpstrInitialDir = szDefPath;
    ofn.Flags           = OFN_SHOWHELP | OFN_PATHMUSTEXIST |
						  OFN_FILEMUSTEXIST;
    if (!GetOpenFileName(&ofn))
		return 0;

    hFile = OpenFile(szOpenName, &OfStruct, OF_READ);
    if (hFile == 0) {
		LoadString(hInst, STR_CANNOTOPEN, szMsgBuf, MAXBUF);
		MessageBox(hWnd, szMsgBuf, NULL, MB_OK | MB_ICONHAND);
		return 0;
    }

    /* ファイルを開けたらファイルハンドルを返す */
    return hFile;
}


/****************************************************************************

    関数:       About(HWND, unsigned, WORD, LONG)

    目的:       [バージョン情報]ダイアログボックスのメッセージ処理をします。

    メッセージ: WM_INITDIALOG - ダイアログボックスの初期化
                WM_COMMAND    - 入力を受け取ったとき

****************************************************************************/

BOOL FAR PASCAL About(
	HWND	hDlg,			   /* ダイアログボックスウィンドウのハンドル */
	UINT	message,		   /* メッセージのタイプ					 */
	WPARAM	wParam,			   /* メッセージ固有の情報					 */
	LPARAM	lParam)
{
    switch (message) {
	case WM_INITDIALOG:	/* ダイアログボックスを初期化 */
	    return (TRUE);

	case WM_COMMAND:	/* コマンドを受け取った */
		switch(GET_WM_COMMAND_ID(wParam, lParam)) {
		case IDOK:						  /* [OK]ボタンを選択 */
		case IDCANCEL:
			EndDialog(hDlg, TRUE);		  /* ダイアログボックスを閉じる */
			return (TRUE);
		}
	    break;
    }
    return (FALSE);		/* メッセージを処理しなかった場合 */
}


/****************************************************************************

    関数: ImeMoveConvertWin(HWND, int, int )

    目的: 日本語変換バッファの表示位置を変更します。

****************************************************************************/

VOID ImeMoveConvertWin(
	HWND hWnd,
	INT x,
	INT y)
{
    LPIMESTRUCT lpIme;

    if( lpIme = (LPIMESTRUCT)GlobalLock( hIME ) ){
		/* サブファンクション番号IME_SETCONVERSIONWINDOWを設定 */
		lpIme->fnc = IME_SETCONVERSIONWINDOW;
		if( x == -1 && y == -1 ) {
	    	lpIme->wParam = MCW_DEFAULT;
		}
		else {
	    	if (!bVerticalFlag)
				lpIme->wParam = MCW_WINDOW;
	    	else
				lpIme->wParam = MCW_WINDOW | MCW_VERTICAL;
		}
		lpIme->lParam1 = MAKELONG( x, y );
		GlobalUnlock( hIME );
		SendIMEMessageEx( hWnd, (LPARAM)hIME);
    }
}


/****************************************************************************

	関数: ImeSetFont(HWND, HFONT)

	目的:  ＩＭＥ変換ウィンドウで使用するフォントを指定します。


****************************************************************************/

VOID ImeSetFont(
	HWND  hWnd,
	HANDLE hFontLF)
{
	LPIMESTRUCT lpIme;

	if (lpIme = (LPIMESTRUCT)GlobalLock(hIME)) {
		/* サブファンクション番号IME_SETCONVERSIONFONTEXを設定する */
		lpIme->fnc = IME_SETCONVERSIONFONTEX;
		/* 論理フォント構造体のハンドルをIME構造体に設定する */
		lpIme->lParam1 = (LPARAM)hFontLF;
		GlobalUnlock(hIME);
		/* 通常のIMEインターフェイス関数を呼び出す */
		SendIMEMessageEx(hWnd, (LPARAM)hIME);
		/* 論理フォント構造体のメモリを開放する */
		if (hFontLF) {
			GlobalFree(hFontLF);
		}	
	}
}
