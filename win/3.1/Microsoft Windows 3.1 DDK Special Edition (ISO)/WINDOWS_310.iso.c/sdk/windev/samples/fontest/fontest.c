#include <windows.h>
#include <commdlg.h>
#include <drivinit.h>
#include <stdio.h>
#include "print.h"
#include "fontest.h"

char	szAppName[] = "TM";

#define EXPORT _export FAR PASCAL
#define PRIVATE NEAR PASCAL

long EXPORT MainWndProc(HWND, UINT, WPARAM, LPARAM);

void PRIVATE PaintMetrics(HDC hdc, PLOGFONT plf, int height);
void PRIVATE DrawMetrics(HDC hdc, int x, int y, HFONT hFont, TEXTMETRIC *tm, char *pch);
void FAR PASCAL ChangeOrient(HWND hWnd);
BOOL ShowLogFont(HWND hWnd, LOGFONT *plf);

HANDLE hInst;
char szFaceName[100] = "Times New Roman";
int iPoints = 10;
LOGFONT lf;
BOOL bPrinter = FALSE;
BOOL bScreen = TRUE;
BOOL bEffects = TRUE;
BOOL bAnsiOnly = FALSE;
BOOL bExistingFonts = FALSE;
BOOL bLimitSizes = FALSE;
BOOL bFixedOnly = FALSE;
BOOL bNoSimulations = FALSE;
BOOL bTTOnly = FALSE;
BOOL bNoWhirl = FALSE;

char szStyle[LF_FACESIZE];



#define PT_TO_PIXELS(hdc, pt)		MulDiv(-(pt), GetDeviceCaps(hdc, LOGPIXELSY), 72)
#define POINT_SIZE	72


int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	MSG		msg;
	WNDCLASS	wndclass;
	HWND		hwndMain;

	hInst = hInstance;

	if (!hPrevInstance) {
		wndclass.style = CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc = MainWndProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInstance;
		wndclass.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(FONTICON));
		wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndclass.hbrBackground = COLOR_WINDOW+1;
		wndclass.lpszMenuName = szAppName;
		wndclass.lpszClassName = szAppName;

		if (!RegisterClass(&wndclass))
			return FALSE;
	}

	if (lpszCmdLine) {
		lstrcpy(lf.lfFaceName, lpszCmdLine);
	}

	hwndMain = CreateWindow(szAppName, "Text Metrics",
            WS_OVERLAPPEDWINDOW | WS_SYSMENU,
	    CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
	    NULL, NULL, hInstance, NULL);

	ShowWindow(hwndMain, nCmdShow);
	UpdateWindow(hwndMain);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}


#define CommandMenu(id, bool) 	case id: bool = !bool; break;

#define CheckMenu(id, bool)	CheckMenuItem(wParam, id, bool ? MF_BYCOMMAND | MF_CHECKED : MF_BYCOMMAND | MF_UNCHECKED)



long EXPORT MainWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	HDC hdcDevice;
	PAINTSTRUCT ps;

	switch (iMessage) {

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		PaintMetrics(hdc, &lf, PT_TO_PIXELS(hdc, iPoints));
		EndPaint(hwnd, &ps);
		break;

	case WM_INITMENUPOPUP:
		CheckMenu(MENU_SCREEN, bScreen);
		CheckMenu(MENU_PRINTER, bPrinter);
		CheckMenu(MENU_EFFECTS, bEffects);
		CheckMenu(MENU_ANSIONLY, bAnsiOnly);
		CheckMenu(MENU_LIMITSIZE, bLimitSizes);
		CheckMenu(MENU_EXISTINGFONTS, bExistingFonts);
		CheckMenu(MENU_TTONLY, bTTOnly);
		CheckMenu(MENU_NOSIMS, bNoSimulations);
		CheckMenu(MENU_FIXEDONLY, bFixedOnly);
		CheckMenu(MENU_NOWHIRL, bNoWhirl);

		break;

	case WM_COMMAND:

		switch (wParam) {
		CommandMenu(MENU_SCREEN, bScreen);
		CommandMenu(MENU_PRINTER, bPrinter);
		CommandMenu(MENU_ANSIONLY, bAnsiOnly);
		CommandMenu(MENU_EFFECTS, bEffects);
		CommandMenu(MENU_EXISTINGFONTS, bExistingFonts);
		CommandMenu(MENU_LIMITSIZE, bLimitSizes);
		CommandMenu(MENU_FIXEDONLY, bFixedOnly);
		CommandMenu(MENU_NOSIMS, bNoSimulations);
		CommandMenu(MENU_TTONLY, bTTOnly);
		CommandMenu(MENU_NOWHIRL, bNoWhirl);

		case MENU_PRINT:

			if (hdcDevice = GetPrinterDC()) {

			    if (InitPrinting(hdcDevice, hwnd, szAppName)) {

				PaintMetrics(hdcDevice, &lf, PT_TO_PIXELS(hdcDevice, iPoints));
				Escape(hdcDevice, NEWFRAME, 0, NULL, NULL);

				TermPrinting(hdcDevice, hwnd);
			    }
			    DeleteDC(hdcDevice);
			}
			break;

		case MENU_FONT: {
			CHOOSEFONT cf;

			cf.lStructSize = sizeof(cf);
			cf.hwndOwner = hwnd;
			cf.lpLogFont = &lf;
			cf.lpszStyle = szStyle;
			cf.Flags = CF_INITTOLOGFONTSTRUCT; // | CF_USESTYLE;
			if (bScreen)
				cf.Flags |= CF_SCREENFONTS;
			if (bPrinter) {
				cf.hDC = GetPrinterDC();
				// cf.hDC = CreateCompatibleDC(NULL);
				cf.Flags |= CF_PRINTERFONTS;
			}
			if (bEffects)
				cf.Flags |= CF_EFFECTS;

			if (bAnsiOnly)
				cf.Flags |= CF_ANSIONLY;

			if (bExistingFonts)
				cf.Flags |= CF_FORCEFONTEXIST;

			if (bLimitSizes) {
				cf.Flags |= CF_LIMITSIZE;
				cf.nSizeMin = 10;
				cf.nSizeMax = 60;
			}

			if (bFixedOnly)
				cf.Flags |= CF_FIXEDPITCHONLY;

			if (bNoSimulations)
				cf.Flags |= CF_NOSIMULATIONS;

			if (bTTOnly)
				cf.Flags |= CF_WYSIWYG;

			if (ChooseFont(&cf)) {
				InvalidateRect(hwnd, NULL, TRUE);
				iPoints = cf.iPointSize / 10;
			}

			if (bPrinter && cf.hDC)
				DeleteDC(cf.hDC);
			break;
                        }


                case  MENU_LOGFONT:
                        if (ShowLogFont (hwnd, &lf))
                          InvalidateRect(hwnd, NULL, TRUE);
                        break;
		}
		break;


	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, iMessage, wParam, lParam);
	}

	return 0L;
}


BOOL EXPORT ShowLogFontProc(HWND hDlg, unsigned message, WORD wParam, LONG lParam)
// BOOL FAR PASCAL _loadds ShowLogFontProc(HWND hDlg, unsigned message, WORD wParam, LONG lParam)
{
  static LOGFONT  lfCurrent, FAR *plfSave;
  BOOL            bTrans;

  switch (message)
  {
    case WM_INITDIALOG:             /* message: initialize dialog box */
      plfSave = (LOGFONT FAR *) lParam;
      lfCurrent = * (LOGFONT FAR *) lParam;
      SetDlgItemInt (hDlg,ID_LOGHEIGHT      , lfCurrent.lfHeight, TRUE);
      SetDlgItemInt (hDlg,ID_LOGWIDTH       , lfCurrent.lfWidth, TRUE);
      SetDlgItemInt (hDlg,ID_LOGESCAPEMENT  , lfCurrent.lfEscapement, TRUE);
      SetDlgItemInt (hDlg,ID_LOGORIENTATION , lfCurrent.lfOrientation, TRUE);
      SetDlgItemInt (hDlg,ID_LOGWEIGHT      , lfCurrent.lfWeight, TRUE);
      SetDlgItemInt (hDlg,ID_UNDERLINE      , lfCurrent.lfItalic, TRUE);
      SetDlgItemInt (hDlg,ID_ITALIC         , lfCurrent.lfUnderline, TRUE);
      SetDlgItemInt (hDlg,ID_STRIKEOUT      , lfCurrent.lfStrikeOut, TRUE);
      SetDlgItemInt (hDlg,ID_CHARSET        , lfCurrent.lfCharSet, TRUE);
      SetDlgItemInt (hDlg,ID_PRECISION      , lfCurrent.lfOutPrecision, TRUE);
      SetDlgItemInt (hDlg,ID_CLIPPRECISION  , lfCurrent.lfClipPrecision, TRUE);
      SetDlgItemInt (hDlg,ID_QUALITY        , lfCurrent.lfQuality, TRUE);
      SetDlgItemInt (hDlg,ID_PITCH          , lfCurrent.lfPitchAndFamily, TRUE);
      SetDlgItemText(hDlg,ID_FACE           , lfCurrent.lfFaceName);
      return (TRUE);

    case WM_COMMAND:        /* message: received a command */
      switch (wParam)
      {
        case IDOK:
        lfCurrent.lfHeight         = GetDlgItemInt (hDlg,ID_LOGHEIGHT      , &bTrans, TRUE);
        lfCurrent.lfWidth          = GetDlgItemInt (hDlg,ID_LOGWIDTH       , &bTrans, TRUE);
        lfCurrent.lfEscapement     = GetDlgItemInt (hDlg,ID_LOGESCAPEMENT  , &bTrans, TRUE);
        lfCurrent.lfOrientation    = GetDlgItemInt (hDlg,ID_LOGORIENTATION , &bTrans, TRUE);
        lfCurrent.lfWeight         = GetDlgItemInt (hDlg,ID_LOGWEIGHT      , &bTrans, TRUE);
        lfCurrent.lfItalic         = (BYTE) GetDlgItemInt (hDlg,ID_UNDERLINE      , &bTrans, TRUE);
        lfCurrent.lfUnderline      = (BYTE) GetDlgItemInt (hDlg,ID_ITALIC         , &bTrans, TRUE);
        lfCurrent.lfStrikeOut      = (BYTE) GetDlgItemInt (hDlg,ID_STRIKEOUT      , &bTrans, TRUE);
        lfCurrent.lfCharSet        = (BYTE) GetDlgItemInt (hDlg,ID_CHARSET        , &bTrans, TRUE);
        lfCurrent.lfOutPrecision   = (BYTE) GetDlgItemInt (hDlg,ID_PRECISION      , &bTrans, TRUE);
        lfCurrent.lfClipPrecision  = (BYTE) GetDlgItemInt (hDlg,ID_CLIPPRECISION  , &bTrans, TRUE);
        lfCurrent.lfQuality        = (BYTE) GetDlgItemInt (hDlg,ID_QUALITY        , &bTrans, TRUE);
        lfCurrent.lfPitchAndFamily = (BYTE) GetDlgItemInt (hDlg,ID_PITCH          , &bTrans, TRUE);
        GetDlgItemText(hDlg, ID_FACE, lfCurrent.lfFaceName, sizeof (lfCurrent.lfFaceName));
        *plfSave = lfCurrent;

        case IDCANCEL:
          EndDialog(hDlg, wParam == IDOK);  /* Exits the dialog box */
          break;
      }
      break;
    default:
      return FALSE;
  }
  return TRUE;         /* Didn't process a message */
}


BOOL ShowLogFont (HWND hwnd, LOGFONT *plf)
{
  FARPROC lpShowLogFontProc;
  BOOL    bRet;

  lpShowLogFontProc = MakeProcInstance (ShowLogFontProc, hInst);
  bRet = (BOOL) DialogBoxParam(hInst, "LOGFONT", hwnd, lpShowLogFontProc, (LONG) (LOGFONT FAR *) plf);
  FreeProcInstance (lpShowLogFontProc);
  return bRet;
}

void NEAR cdecl printat(HDC hdc, int x, int y, LPSTR fmt, ...)
{
	char	buf[80];

	wvsprintf(buf, fmt, (LPSTR)(&fmt + 1));
	TextOut(hdc, x, y, buf, lstrlen(buf));
}

void printWidth (HDC hdc, int *pWidth, int x, int y, int dx, int dy)
{
  int line, col;
  char ach [10];

  for (line = 0; line < 16; line++)
    for (col = 0; col < 16; col++)
    {
      sprintf (ach, "%d", *(pWidth + line * 16 + col));
      TextOut(hdc, x + dx * col, y + line * dy, ach, lstrlen(ach));
    }

}

void NEAR PASCAL SpinFont(HDC hdc, int x, int y, PLOGFONT plf)
{
	LOGFONT lf;
	HFONT hFont, hOld;
	int modeOld;
	int theta;

	lf = *plf;
	lf.lfWidth = 0;

	modeOld = SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, 0x00000000);

	for (theta = 0; theta < 3600; theta += 200) {


		lf.lfOrientation = lf.lfEscapement = theta;

		hFont = CreateFontIndirect(&lf);

		if (hFont) {
			hOld = SelectObject(hdc, hFont);

			printat(hdc, x, y, "Wheee...");

			SelectObject(hdc, hOld);

			DeleteObject(hFont);
		}

	}
	SetBkMode(hdc, modeOld);
}




void PRIVATE PaintMetrics(HDC hdc, PLOGFONT plf, int height)
{
	char face_name[80];
        TEXTMETRIC tm, tmView;
        HFONT hFont, hOld, hFontView;
        int     x, y, dy, yMax;
        static LOGFONT lf = { 0, 0, 0, 0, 400, 0, 0, 0, 0, 1, 2, 1, 26, "Arial"};
        static int aWidths [256];

        lf.lfHeight = PT_TO_PIXELS(hdc, 8);
        hFontView = CreateFontIndirect(&lf);

	// plf->lfHeight = height;
	hFont = CreateFontIndirect(plf);

	if (hFont)
	    hOld = SelectObject(hdc, hFont);

	GetTextMetrics(hdc, &tm);
	GetTextFace(hdc, sizeof(face_name), face_name);
        GetCharWidth(hdc, 0, 0xff, aWidths);

	if (hOld)
	    SelectObject(hdc, hOld);

        hOld = SelectObject(hdc, hFontView);

        GetTextMetrics(hdc, &tmView);

	y = 0;
        x = GetDeviceCaps(hdc, LOGPIXELSX) / 4;
        dy = tmView.tmHeight - 2;

	printat(hdc, x, y += dy, "tmHeight %d", tm.tmHeight);
        printat(hdc, x, y += dy, "Ascent, Descent %d, %d", tm.tmAscent, tm.tmDescent);
	printat(hdc, x, y += dy, "tmInternalLeading %d", tm.tmInternalLeading);
	printat(hdc, x, y += dy, "tmExternalLeading %d", tm.tmExternalLeading);
	printat(hdc, x, y += dy, "tmAveCharWidth %d", tm.tmAveCharWidth);
	printat(hdc, x, y += dy, "tmMaxCharWidth %d", tm.tmMaxCharWidth);
	printat(hdc, x, y += dy, "tmWeight %d", tm.tmWeight);
        printat(hdc, x, y += dy, "Italic, Under, Strike %d, %d, %d", tm.tmItalic, tm.tmUnderlined, tm.tmStruckOut);
        printat(hdc, x, y += dy, "tmFirst, tmlast: %x - %x", tm.tmFirstChar, tm.tmLastChar);
	printat(hdc, x, y += dy, "tmDefaultChar %d", tm.tmDefaultChar);
	printat(hdc, x, y += dy, "tmBreakChar %d", tm.tmBreakChar);
	printat(hdc, x, y += dy, "tmPitchAndFamily %x", tm.tmPitchAndFamily);
	printat(hdc, x, y += dy, "tmCharSet %d", tm.tmCharSet);
	printat(hdc, x, y += dy, "tmOverhang %d", tm.tmOverhang);
        printat(hdc, x, y += dy, "Digitized Aspect X, Y: %d, %d", tm.tmDigitizedAspectX, tm.tmDigitizedAspectY);
        printat(hdc, x, y += dy, "Face Name: %s", (LPSTR)face_name);
        yMax = y + dy + dy;

	y = 0;
	x = 2 * GetDeviceCaps(hdc, LOGPIXELSX);
	printat(hdc, x, y += dy, "lfHeight %d", plf->lfHeight);
	printat(hdc, x, y += dy, "lfWidth %d", plf->lfWidth);
	printat(hdc, x, y += dy, "lfEscapement %d", plf->lfEscapement);
	printat(hdc, x, y += dy, "lfOrientation %d", plf->lfOrientation);
	printat(hdc, x, y += dy, "lfWeight %d", plf->lfWeight);
	printat(hdc, x, y += dy, "lfItalic %d", plf->lfItalic);
	printat(hdc, x, y += dy, "lfUnderline %d", plf->lfUnderline);
	printat(hdc, x, y += dy, "lfStrikeOut %d", plf->lfStrikeOut);
	printat(hdc, x, y += dy, "lfCharSet %d", plf->lfCharSet);
	printat(hdc, x, y += dy, "lfOutPrecision %x", plf->lfOutPrecision);
	printat(hdc, x, y += dy, "lfClipPrecision %x", plf->lfClipPrecision);
	printat(hdc, x, y += dy, "lfQuality %x", plf->lfQuality);
	printat(hdc, x, y += dy, "lfPitchAndFamily %x", plf->lfPitchAndFamily);
	printat(hdc, x, y += dy, "lfFaceName %s", (LPSTR)plf->lfFaceName);

        printWidth(hdc, aWidths, GetDeviceCaps(hdc, LOGPIXELSX) / 4, yMax, tmView.tmAveCharWidth * 4, dy);

	DrawMetrics(hdc, 4 * GetDeviceCaps(hdc, LOGPIXELSX), 1 * GetDeviceCaps(hdc, LOGPIXELSY), hFont, &tm, "Agm");

    if (!bNoWhirl)
	    SpinFont(hdc, 4 * GetDeviceCaps(hdc, LOGPIXELSX), 4 * GetDeviceCaps(hdc, LOGPIXELSY), plf);

        if (hOld)
	    SelectObject(hdc, hOld);

	if (hFont)
	    DeleteObject(hFont);

	if (hFontView)
            DeleteObject(hFontView);

}


void PRIVATE DrawMetrics(HDC hdc, int x, int y, HFONT hFont, TEXTMETRIC *tm, char *pch)
{
	RECT rc;
	HFONT hOld;
	int	modeOld;

	rc.top = y;
	rc.left = x;

	rc.right = x + tm->tmMaxCharWidth;


	rc.bottom = y + tm->tmHeight;

	FrameRect(hdc, &rc, GetStockObject(BLACK_BRUSH));

	rc.bottom = y + tm->tmAscent;

	FrameRect(hdc, &rc, GetStockObject(BLACK_BRUSH));

	rc.bottom = y + tm->tmInternalLeading;

	FrameRect(hdc, &rc, GetStockObject(BLACK_BRUSH));

	hOld = SelectObject(hdc, hFont);
	modeOld = SetBkMode(hdc, TRANSPARENT);

	SetTextColor(hdc, 0x00FF0000);
	SetBkColor(hdc, 0x0000FF00);

	while (*pch) {
		TextOut(hdc, x, y, pch, 1);
		pch++;
	}
	SetBkMode(hdc, modeOld);
	SelectObject(hdc, hOld);
}




#if 1
static char driver[] = {"PSCRIPT"};
static char printer[] = {"Apple LaserWriter Plus"};
static char port[] = {"output.prn"};
#else
static char driver[] = {"HPPCL"};
static char printer[] = {"PCL / HP LaserJet"};
static char port[] = {"LPT1"};
#endif

/**[r******************************************************************
 * ChangeOrient
 **[r******************************************************************/
void FAR PASCAL ChangeOrient(HWND hWnd)
  {
  LPDEVMODE lpDMOut;
  LPDEVMODE lpDMIn;
  LPFNDEVMODE lpfnDM;
  HDC hDC;
  HANDLE hMod;
  HANDLE hData;
  HFONT hFont;
  WORD wSize;
  char *pData;

  #define _EXTDEVICEMODE(a,b,c,d,e,f,g,h) \
    (*lpfnDM)((HWND)(a),(HANDLE)(b),(LPDEVMODE)(c),(LPSTR)(d), \
    (LPSTR)(e),(LPDEVMODE)(f),(LPSTR)(g),(WORD)(h))

  #define DM_SIZE 0

  if (hDC=CreateDC(driver,printer,port,(LPSTR)NULL))
    {
    if (!(hMod=GetModuleHandle(driver)))
      goto backout0;

    if (!(lpfnDM=(LPFNDEVMODE)GetProcAddress(hMod,PROC_EXTDEVICEMODE)))
      goto backout0;

    if (!(wSize=_EXTDEVICEMODE(hWnd, hMod, NULL,
      printer, port, NULL, NULL, DM_SIZE)))
      {
      goto backout0;
      }

    if (!(hData=LocalAlloc(LMEM_ZEROINIT,wSize * 2)))
      goto backout0;

    if (!(pData=LocalLock(hData)))
      goto backout1;

    lpDMIn = (LPDEVMODE)pData;
    lpDMOut = (LPDEVMODE)&pData[wSize];

    if (_EXTDEVICEMODE(hWnd, hMod, lpDMOut, printer,
      port, lpDMIn, NULL, DM_COPY) != IDOK)
      {
      goto backout2;
      }

    if (lpDMOut->dmFields & DM_ORIENTATION)
      {
      // dmcopy(lpDMIn, lpDMOut, wSize)
      *lpDMIn = *lpDMOut;	// doesn't copy the device dependant part!

      if (lpDMIn->dmOrientation == DMORIENT_PORTRAIT)
        lpDMIn->dmOrientation = DMORIENT_LANDSCAPE;
      else
        lpDMIn->dmOrientation = DMORIENT_PORTRAIT;

#if 0
      /* If you leave dmFields == the value returned from the
       * PostScript driver, then the call to ExtDeviceMode()
       * with (DM_MODIFY | DM_UPDATE) crashes the driver.
       */
      lpDMIn->dmFields = DM_ORIENTATION;
#endif

#if 1
      /* It appears that the DM_UPDATE flag has no effect.
       */
      _EXTDEVICEMODE(hWnd, hMod, lpDMOut,
        printer, port, lpDMIn,
        NULL, (DM_MODIFY | DM_UPDATE | DM_COPY));
#else
      /* This call succeeds, but the orientation is not changed
       * on the printed output.
       */
      _EXTDEVICEMODE(hWnd, hMod, lpDMOut,
        printer, port, lpDMIn,
        NULL, DM_MODIFY);
#endif

#if 1
      /* If you use DM_UPDATE, the CreateDC() returns zero.
       * If you don't use it, then you can't print with
       * a different DC because the orientation change was
       * not permanent.
       */
      DeleteDC(hDC);
      hDC = 0;

      if (!(hDC=CreateDC(driver,printer,port,(LPSTR)NULL)))
        goto backout2;
#endif

      Escape(hDC, STARTDOC, 10, "Test Bench", NULL);

      if ((hFont=CreateFont(-300, 0, 0, 0, FW_NORMAL, 0, 0, 0,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, (VARIABLE_PITCH | FF_ROMAN),
        (LPSTR)"Tms Rmn")) && SelectObject(hDC,hFont))
        {
        ExtTextOut(hDC, 100, 100, 0, 0L, printer, sizeof(printer)-1, NULL);
        }

      Escape(hDC, NEWFRAME, NULL, NULL, NULL);
      Escape(hDC, ENDDOC, NULL, NULL, NULL);
      }

backout2:
    LocalUnlock(hData);
    pData = NULL;
backout1:
    LocalFree(hData);
    hData = 0;
backout0:
    if (hDC)
      DeleteDC(hDC);
    }
  }
