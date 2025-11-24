//////////////////////////////////////////////////////////////////////////
//	CONFIG.C							//
//									//
//	This module contains the configuration dialog box routines.	//
//									//
//	For the AuraVision video capture driver AVCAPT.DRV.		//
//									//
//////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1994 - 1995 Microsoft Corporation.	All Rights Reserved.
//
//---------------------------------------------------------------------------

#include <windows.h>
#include <mmsystem.h>
#include <stdlib.h>
#include <msvideo.h>
#include <msviddrv.h>
#include "avcapt.h"
#include "avdev.h"
#include "config.h"
#include "ver.h"
#include "debug.h"
#include "mmdebug.h"
#include <avwin.h>


#define WIDTHBYTES(i)	((unsigned)((i+31)&(~31))/8)
#define WM_UPDATEDIALOG	(WM_USER+200)

typedef struct tagVS_VERSION
	{
	WORD			wTotLen;
	WORD			wValLen;
	char			szSig[16];
	VS_FIXEDFILEINFO	vffInfo;
	} VS_VERSION;

typedef struct tagLANGANDCP
	{
	WORD	wLanguage;
	WORD	wCodePage;
	} LANGANDCP;

extern DEVICE_INIT devInit;
extern    WORD    wDebugLevel;       // debug level

static WORD	wTmpVideoFormat;
static WORD	wTmpSize40;	// Legal range is 1 to 40 (= 40 to 640).

static WORD	wTmpSourceConnector;	// 0-3
static WORD	wTmpVideoStandard;	// 0 = NTSC, 1 = PAL
static WORD	wTmpVideoCableFormat;	// 0 = Composite, 1 = SVideo

static int	gwVideoStandard;
static WORD	gwVideoCableFormat;	// 0 = Composite, 1 = SVideo

static WORD	wTmpBrightness;
static WORD	wTmpContrast;
static WORD	wTmpSaturation;
static WORD	wTmpHue;
static WORD	wTmpXPosition;
static WORD	wTmpYPosition;

static char     szHelpFileName[] = "mmdrv.hlp";  //Replace this line!!!

static COMBOBOX_ENTRY gwSizeOptions[] = 
	{
	1,	"40 x 30",
	2,	"80 x 60",
	3,	"120 x 90",
	4,	"160 x 120",
	5,	"200 x 150",
	6,	"240 x 180",
	7,	"280 x 210",
	8,	"320 x 240",
	9,	"360 x 270",
	10,	"400 x 300",
	11,	"440 x 330",
	12,	"480 x 360",
	13,	"520 x 390",
	14,	"560 x 420",
	15,	"600 x 450",
	16,	"640 x 480"
	};

#define N_SIZEOPTIONS (sizeof (gwSizeOptions) / sizeof (COMBOBOX_ENTRY))

static COMBOBOX_ENTRY gwFormatOptions[] = 
	{
	IMAGE_FORMAT_YUV411COMPRESSED,	"Aura 1 Compressed",
	IMAGE_FORMAT_PAL8,    		"8 bit Palettized",
	IMAGE_FORMAT_RGB16,		"16 bit RGB",
	IMAGE_FORMAT_RGB24,		"24 bit RGB"
	};

#define N_FORMATOPTIONS (sizeof (gwFormatOptions) / sizeof (COMBOBOX_ENTRY))

int	iMin, iMax;

static DWORD aConfigDlgHelpIDs[] = {
        ID_UNDER1MEG    , IDH_ORCHID_VIDEO_CARD,
        ID_OVER1MEG     , IDH_ORCHID_VIDEO_CARD,
        ID_PH9051411    , IDH_ORCHID_VIDEO_DECODE,
        ID_PH7191422    , IDH_ORCHID_VIDEO_DECODE,
        ID_PH7110411    , IDH_ORCHID_VIDEO_DECODE,
        ID_PH7110422    , IDH_ORCHID_VIDEO_DECODE,
        0,0
} ;

static DWORD aSourceDlgHelpIDs[] = {
        ID_PBSOURCE0            , IDH_ORCHID_VIDEO_CONNECT,
        ID_PBSOURCE1            , IDH_ORCHID_VIDEO_CONNECT,
        ID_PBSOURCE2            , IDH_ORCHID_VIDEO_CONNECT,
        ID_PBNTSC               , IDH_ORCHID_VIDEO_STANDARD,
        ID_PBPAL                , IDH_ORCHID_VIDEO_STANDARD,
        ID_PBDEFAULT            , IDH_ORCHID_DEFAULT,
        0,0
} ;

static DWORD aFormatDlgHelpIDs[] = {
        ID_LBIMAGEFORMAT        , IDH_ORCHID_IMAGE_FORMAT,
        ID_PBSIZEFULL           , IDH_ORCHID_SIZE,
        ID_PBSIZEHALF           , IDH_ORCHID_SIZE,
        ID_PBSIZEQUARTER        , IDH_ORCHID_SIZE,
        ID_PBSIZEEIGHTH         , IDH_ORCHID_SIZE,
        ID_LBSIZE               , IDH_ORCHID_SIZE,
        0,0
} ;

static DWORD aDisplayDlgHelpIDs[] = {
        ID_SBHUE                , IDH_ORCHID_COLOR_CONTROLS,
        ID_SBSATURATION         , IDH_ORCHID_COLOR_CONTROLS,
        ID_SBBRIGHTNESS         , IDH_ORCHID_COLOR_CONTROLS,
        ID_SBCONTRAST           , IDH_ORCHID_COLOR_CONTROLS,
        ID_SBXPOSITION          , IDH_ORCHID_POSITION,
        ID_SBYPOSITION          , IDH_ORCHID_POSITION,
        ID_EBHUE                , IDH_ORCHID_COLOR_CONTROLS,
        ID_EBSATURATION         , IDH_ORCHID_COLOR_CONTROLS,
        ID_EBBRIGHTNESS         , IDH_ORCHID_COLOR_CONTROLS,
        ID_EBCONTRAST           , IDH_ORCHID_COLOR_CONTROLS,
        ID_EBXPOSITION          , IDH_ORCHID_POSITION,
        ID_EBYPOSITION          , IDH_ORCHID_POSITION,
        ID_PBDEFAULT            , IDH_ORCHID_DEFAULT,
        0,0
} ;

//
// This initalizes the destination BitmapInfo header
//

void FAR PASCAL InitDestBIHeader(
	LPBITMAPINFOHEADER	lpbi,
	WORD			wEnumFormat,
	WORD			wWidth)
	{
	lpbi->biSize		= sizeof (BITMAPINFOHEADER);
	lpbi->biWidth		= wWidth;
	lpbi->biHeight		= (lpbi->biWidth * 3) / 4;
	lpbi->biPlanes		= 1;
	lpbi->biXPelsPerMeter	= 0L;
	lpbi->biYPelsPerMeter	= 0L;
	lpbi->biClrUsed		= 0L;
	lpbi->biClrImportant	= 0L;

	switch (wEnumFormat)
		{
		case IMAGE_FORMAT_PAL8:  
		lpbi->biBitCount = 8; 
		lpbi->biCompression = BI_RGB;
		break;

		case IMAGE_FORMAT_RGB16:
		lpbi->biBitCount = 16; 
		lpbi->biCompression = BI_RGB;
		break;

		case IMAGE_FORMAT_RGB24:
		lpbi->biBitCount = 24; 
		lpbi->biCompression = BI_RGB;
		break;

		case IMAGE_FORMAT_YUV411COMPRESSED:
		lpbi->biBitCount = 16;
		lpbi->biCompression = ckidYUV411Compressed;
		break;
		}

	if (lpbi->biCompression == BI_RGB)
		lpbi->biSizeImage     = (DWORD)biDest.biHeight * 
			(DWORD)WIDTHBYTES(biDest.biWidth * biDest.biBitCount);
	else
		lpbi->biSizeImage     = (DWORD)biDest.biHeight * 
			(DWORD)WIDTHBYTES(biDest.biWidth * biDest.biBitCount);
	}


//
// Initalizes all state variables.  Called once at startup.
//

BOOL FAR PASCAL ConfigInit(LPDEVICE_INIT lpDI)
	{
	D1("CONFIGINIT");
	
	gfEurope = gwVideoStandard != 0;

	// The following constant is never changed.
	gwWidthBytes = 2048;	// Width of a line in the frame buffer.
        
	// New way.
	biSource.biSize		= sizeof (BITMAPINFOHEADER);
	biSource.biWidth	= 720;
	biSource.biHeight	= 480;
	biSource.biPlanes	= 1;
	biSource.biBitCount	= 8;
	biSource.biCompression	= BI_RGB;
	biSource.biSizeImage	=
		WIDTHBYTES(biSource.biWidth * biSource.biBitCount) 
		* biSource.biHeight;
	biSource.biXPelsPerMeter= 0L;
	biSource.biYPelsPerMeter= 0L;
	biSource.biClrUsed	= 0L;
	biSource.biClrImportant	= 0L;

	SetSourceFormat(&biSource, sizeof (biSource));

	InitDestBIHeader(&biDest, gwDestFormat, gwSize40 * 40);

	// Try to set the format using the last saved configuration.
	if (SetDestFormat(&biDest, sizeof (biDest)))
		{
		// Initialization failed, try again using minimum sizes.
		InitDestBIHeader (&biDest, IMAGE_FORMAT_PAL8, 1 * 40);
		if (SetDestFormat (&biDest, sizeof (biDest)))
			return FALSE;
		}
	return TRUE;
	}

//
// Get a profile hex value.
// Returns the profile value or the default if none is present.
//

WORD FAR PASCAL GetProfileHex(LPSTR szApp, LPSTR szEntry, WORD wDef)
	{
	BYTE	buf[20];
	WORD	n;
	BYTE	b;
	int	i;

	n = GetPrivateProfileString(szApp, szEntry, gszNULL, buf,
		sizeof(buf), gszIniFile);
	if (n < 1) return wDef;

	for (n = 0, i = 0; b = buf[i]; i++)
		{
		if (b > 'Z') b -= 'a' - 'A';
		b -= '0';
		if (b > 9) b -= 7;
		if (b > 15) break;
		n = n * 16 + b;
		}
	return n;
	}

//
// Retrieves settings from INI for everything except 
// those from the config dialog.
// Fills globals with settings under [<drivename>.drv].
//

WORD FAR PASCAL ConfigGetSettings(void)
	{
	WORD	wTemp;

	wTemp = HW_GetVideoSource();
	gwSourceConnector = LimitRange (wTemp, 0, HW_SOURCE2);

	wTemp = HW_GetVideoStandard();
	gwVideoStandard = LimitRange (wTemp, 0, 1);

	wTemp = 0;
	gwVideoCableFormat = LimitRange(wTemp, 0, 1); // 0=composite, 1=SVideo

	wTemp = GetPrivateProfileInt(gszDriverName, gszSize40Key, 
		4, gszIniFile);
	gwSize40 = LimitRange (wTemp, 1, MAX_SIZE_X_NTSC / 40);

	wTemp = GetPrivateProfileInt(gszDriverName, gszVideoFormatKey, 
		IMAGE_FORMAT_PAL8, gszIniFile);
	gwDestFormat = LimitRange (wTemp, 
		IMAGE_FORMAT_YUV411COMPRESSED, IMAGE_FORMAT_RGB24);

	return TRUE;
	}

//
// Saves all settings to the INI except those controlled by the config dialog
// Saves globals settings under [<drivename>.drv]
//

WORD FAR PASCAL ConfigPutSettings(void)
	{
	char	buf[40];

	wsprintf(buf, gszIntFormat, gwSize40);
	WritePrivateProfileString(gszDriverName, gszSize40Key, buf, gszIniFile);

	wsprintf(buf, gszIntFormat, gwDestFormat);
	WritePrivateProfileString(gszDriverName, gszVideoFormatKey, buf, gszIniFile);

	HW_SaveConfiguration(NULL);

	return TRUE;
	}


static void WINAPI ConfigErrorMsgBox(HWND hDlg, WORD wStringId)
	{
	char	szPname[MAXPNAMELEN];
	char	szErrorBuffer[MAX_ERR_STRING];	// Buffer for error messages.

	LoadString(ghModule, IDS_VCAPPRODUCT, szPname, MAXPNAMELEN);
	LoadString(ghModule, wStringId, szErrorBuffer, sizeof(szErrorBuffer));
	MessageBox(hDlg, szErrorBuffer, szPname, MB_OK|MB_ICONEXCLAMATION);
	}


void WINAPI ConfigRemove(void)
	{
	WritePrivateProfileString(gszDriverName, NULL, NULL, gszIniFile);
	}

//
// Loads entries into a combobox, and selects the current index
// Parmameters:
//      hWnd of the parent dialog box
//      ID of the combobox
//      array of text and values
//      Count of entries in the COMBOBOX_ENTRY array
//      Initial value which should match the wValue field of the 
//              COMBOBOX_ENTRY.  If no values match, the selection
//              defaults to the first entry in the combobox.
// Returns:
//      Returns the index of the selected item in the combobox.
//

static int WINAPI ConfigLoadComboBox(
	HWND			hDlg,
	int			wID,
	COMBOBOX_ENTRY *	pCBE,
	int			nEntries,
	WORD			wInitialValue)
	{
	int	j;
	int	nIndex = 0;	// Zero entry should be blank, None, etc.
	HWND hWndCB = GetDlgItem(hDlg, wID);

	for (j = 0; j < nEntries; j++)
		{
		SendMessage (hWndCB, CB_ADDSTRING, 0,
			(LONG)(LPSTR)((pCBE+j)->szText));
		if (pCBE[j].wValue == wInitialValue)
			nIndex = j;
		}
	SendMessage (hWndCB, CB_SETCURSEL, nIndex, 0L);
	return nIndex;
	}

//
// Returns the value associated with the selected ComboBox text string.
// Parameters:
//      hWnd of the parent dialog box
//      ID of the ComboBox
//      array of text and values
//  Returns:
//      Returns the value of the selected item in list.
//

static WORD WINAPI ConfigGetComboBoxValue(
	HWND			hDlg,
	int			wID,
	COMBOBOX_ENTRY *	pCBE)
	{
	int	nIndex;
	HWND	hWndCB = GetDlgItem(hDlg, wID);

	nIndex = (int) SendMessage (hWndCB, CB_GETCURSEL, 0, 0L);
	nIndex = max (0, nIndex);   // LB_ERR is negative
	return pCBE[nIndex].wValue;
	}



//
// Puts up the configuration dialog box.
// Returns whatever was returned from the dialog box procedure.
//

int WINAPI Config(HWND hWnd, HANDLE hModule)
	{
	return DialogBox(hModule, MAKEINTATOM(DLG_VIDEOCONFIG), hWnd,
		(FARPROC)ConfigDlgProc);
	}

//
// Configuration dialog displayed by the drivers applet
//     This dialog only handles non-PnP related settings which cannot be 
//     autodetected, such as the amount of memory installed on the card, 
//     and the video decoder used.
//
// Returns DRVCNF_RESTART if the user has changed settings, which will
//     cause the drivers applet which launched this to give the user a
//     message about having to restart Windows for the changes to take
//     effect.  If the user clicks on "Cancel" or if no settings have changed,
//     DRVCNF_CANCEL is returned.
//
//

int FAR PASCAL _loadds ConfigDlgProc(
	HWND	hDlg,
	WORD	msg,
	WORD	wParam,
	LONG	lParam)
	{
        char    szIniFile [MAX_PATH];
        char    buf [80];
        int     Interleave;
        int     QFactor;
        BOOL    fIs422;
        BOOL    fUnder1Meg;
        LPSTR   cp;
        static  char szPH9051411[] = "PH9051411";
        static  char szPH7110411[] = "PH7110411";
        static  char szPH7191422[] = "PH7191422";
        static  char szPH7110422[] = "PH7110422";

	switch (msg)
		{
		case WM_INITDIALOG:
                AV_GetIniFilename (szIniFile, sizeof (szIniFile) - 1);
                Interleave = GetPrivateProfileInt("AVSettings", "Interleave", 2, 
                        szIniFile);
                QFactor = GetPrivateProfileInt("AVSettings", "QFactor", 100, 
                        szIniFile);
                GetPrivateProfileString("EXInit", "Decoder", "PH9051411", 
                        buf, sizeof(buf), szIniFile);
                AuxDebugEx (3, DEBUGLINE "IniFile is %s\r\n", szIniFile);
                AuxDebugEx (3, DEBUGLINE "Interleave is %d\r\n", Interleave);
                AuxDebugEx (3, DEBUGLINE "QFactor is %d\r\n", QFactor);

                // What kind of decoder is in use?
                fIs422 = FALSE;

                if (lstrcmpi(szPH9051411, buf) == 0 ) 
                        CheckDlgButton (hDlg, ID_PH9051411, TRUE);

                else if (lstrcmpi(szPH7110411, buf) == 0 )  
                        CheckDlgButton (hDlg, ID_PH7110411, TRUE);

                else if (lstrcmpi(szPH7191422, buf ) == 0 ) {
                        CheckDlgButton (hDlg, ID_PH7191422, TRUE);
                        fIs422 = TRUE;
                }
                else if (lstrcmpi(szPH7110422, buf ) == 0 ) {
                        CheckDlgButton (hDlg, ID_PH7110422, TRUE);
                        fIs422 = TRUE;
                }
                else
                        CheckDlgButton (hDlg, ID_PH9051411, TRUE);

                // Guess at memory setting given decoder and Interleave
                if (fIs422 || Interleave > 2)
                        CheckDlgButton (hDlg, ID_OVER1MEG, TRUE);
                else        
                        CheckDlgButton (hDlg, ID_UNDER1MEG, TRUE);

		break;

		// User will unload this module at exit time, so make
		// sure that the dialog is gone if the user just quits 
		// Windows with the dialog box up.
		case WM_ENDSESSION:     
		if (wParam)
			EndDialog(hDlg, DRV_CANCEL);
		break;

		case WM_COMMAND:
		switch (wParam)
			{
			case IDOK:
                        // If < 1Meg, Interleave is 2
                        // If > 1Meg and 422, Interleave is 3
                        // If > 1Meg and 411, Interleave is 4
                        fIs422 = FALSE;
                        if (fUnder1Meg = IsDlgButtonChecked (hDlg, ID_UNDER1MEG))
                                Interleave = 2;
                        else
                                Interleave = 4;

                        cp = szPH9051411;
                        if (IsDlgButtonChecked(hDlg, ID_PH9051411))
                                cp = szPH9051411;
                        else if (IsDlgButtonChecked(hDlg, ID_PH7110411))
                                cp = szPH7110411;
                        else if (IsDlgButtonChecked(hDlg, ID_PH7191422)) {
                                cp = szPH7191422;
                                fIs422 = TRUE;
                        }
                        else if (IsDlgButtonChecked(hDlg, ID_PH7110422)) {
                                cp = szPH7110422;
                                fIs422 = TRUE;
                        }
                        if (fIs422)
                                Interleave = 3;

                        AuxDebugEx (3, DEBUGLINE "Interleave is %d\r\n", Interleave);
                        AuxDebugEx (3, DEBUGLINE "Decoder is %s\r\n", cp);

                        AV_GetIniFilename (szIniFile, sizeof (szIniFile) - 1);
                        wsprintf (buf, "%d", Interleave);
                        WritePrivateProfileString("AVSettings", "Interleave", 
                                (LPSTR) buf, szIniFile);
                        WritePrivateProfileString("EXInit", "Decoder", 
                                (LPSTR) cp, szIniFile);

			EndDialog(hDlg, DRV_CANCEL);
			break;

			case IDCANCEL:
			EndDialog(hDlg, DRV_CANCEL);
			break;

			default:
			break;
			}
		break;
                
                case WM_HELP:
                        WinHelp (((LPHELPINFO) lParam)->hItemHandle, 
                                szHelpFileName, HELP_WM_HELP,
                                (DWORD) (LPSTR) aConfigDlgHelpIDs);
                        break;

                case WM_CONTEXTMENU:
                        WinHelp ((HWND) wParam, 
                                szHelpFileName, HELP_CONTEXTMENU,
                                (DWORD) (LPSTR) aConfigDlgHelpIDs);
                        break;

		default:
		return FALSE;
		}
	return TRUE;
	}


VOID UpdateSizeDisplay(HWND hDlg)
	{
	SendDlgItemMessage (hDlg, ID_LBSIZE, CB_SETCURSEL, wTmpSize40 - 1, 0L);
	}

//
// Dialog proc for the video format dialog box (VIDEO_IN channel)
//

int FAR PASCAL _loadds VideoFormatDlgProc(
	HWND	hDlg,
	WORD	msg,
	WORD	wParam,
	LONG	lParam)
	{
	int			j;
	BITMAPINFOHEADER	bi;

	switch (msg)
		{
		case WM_INITDIALOG:
		wTmpVideoFormat = gwDestFormat;
		wTmpSize40 = gwSize40;
		ConfigLoadComboBox(hDlg, ID_LBSIZE, gwSizeOptions,
			N_SIZEOPTIONS, wTmpSize40);
		ConfigLoadComboBox(hDlg, ID_LBIMAGEFORMAT, gwFormatOptions,
			N_FORMATOPTIONS, gwDestFormat);
		UpdateSizeDisplay(hDlg);
		break;

		// User will unload this module at exit time, so make
		// sure that the dialog is gone if the user just quits 
		// Windows with the dialog box up.
		case WM_ENDSESSION:     
		if (wParam)
			EndDialog (hDlg, IDCANCEL);
		break;

		case WM_COMMAND:
		switch (wParam)
			{
			case IDOK:
			gwSize40 = wTmpSize40;
			j = ConfigGetComboBoxValue(hDlg,
				ID_LBIMAGEFORMAT, gwFormatOptions);

			InitDestBIHeader(&bi, j, gwSize40 * 40);
			SetDestFormat(&bi, sizeof (bi));

			ConfigPutSettings();	// Save settings.
			EndDialog(hDlg, IDOK);
			break;

			case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			break;

			case ID_PBSIZEFULL:
			wTmpSize40 = (MAX_SIZE_X_NTSC / 40);
			UpdateSizeDisplay(hDlg);
			break;

			case ID_PBSIZEHALF:
			wTmpSize40 = (MAX_SIZE_X_NTSC / 40) / 2;
			UpdateSizeDisplay(hDlg);
			break;

			case ID_PBSIZEQUARTER:
			wTmpSize40 = (MAX_SIZE_X_NTSC / 40) / 4;
			UpdateSizeDisplay(hDlg);
			break;

			case ID_PBSIZEEIGHTH:
			wTmpSize40 = (MAX_SIZE_X_NTSC / 40) / 8;
			UpdateSizeDisplay(hDlg);
			break;

			case ID_LBSIZE:
			if (HIWORD(lParam) == CBN_KILLFOCUS)
				{
				wTmpSize40 = (WORD) SendDlgItemMessage(hDlg,
					ID_LBSIZE, CB_GETCURSEL, 0, 0L) + 1;
				}
			break;

			default:
			break;
			}
		break;

                case WM_HELP:
                        WinHelp (((LPHELPINFO) lParam)->hItemHandle, 
                                szHelpFileName, HELP_WM_HELP,
                                (DWORD) (LPSTR) aFormatDlgHelpIDs);
                        break;

                case WM_CONTEXTMENU:
                        WinHelp ((HWND) wParam, 
                                szHelpFileName, HELP_CONTEXTMENU,
                                (DWORD) (LPSTR) aFormatDlgHelpIDs);
                        break;

		default:
		return FALSE;
		}
	return TRUE;
	}


int ColorProcessScroll(
	HWND	hDlg,
	HWND	hCtl,
	WORD	wParam,
	DWORD	lParam,
 	int	iVal,
	WORD	wIDEditBox)
	{
	int	iPageSize;

	GetScrollRange(hCtl, SB_CTL, &iMin, &iMax);
	iPageSize = 1 + ((iMax - iMin) / 10);

	switch (wParam)
		{
		case SB_LINEDOWN:
		iVal++;
		break;

		case SB_LINEUP: 
		iVal--;
		break;

		case SB_PAGEDOWN:
		iVal += iPageSize;
		break;

		case SB_PAGEUP: 
		iVal -= iPageSize;
		break;

		case SB_THUMBTRACK:
		iVal = LOWORD (lParam);
		break;

		default:
		break;
		}
	iVal = LimitRange(iVal, iMin, iMax);
	SetScrollPos(hCtl, SB_CTL, iVal, TRUE);
	SetDlgItemInt(hDlg, wIDEditBox, iVal, TRUE);
	return iVal;
	}


//
// Dialog proc for the video display dialog box
//

int FAR PASCAL _loadds VideoDisplayDlgProc(
	HWND	hDlg,
	WORD	msg,
	WORD	wParam,
	LONG	lParam)
	{
	WORD	wID;
	HWND	hCtl;

	switch (msg)
		{
		case WM_INITDIALOG:
		AV_LoadConfiguration();
		wTmpBrightness = AV_GetParameter(AVBRIGHTNESS);
		wTmpContrast   = AV_GetParameter(AVCONTRAST);
		wTmpSaturation = AV_GetParameter(AVSATURATION);
		wTmpHue        = AV_GetParameter(AVHUE);
		wTmpXPosition  = AV_GetParameter(AVXPOSITION);
		wTmpYPosition  = AV_GetParameter(AVYPOSITION);

		// Intentional fall through.
		case WM_UPDATEDIALOG:
		wTmpBrightness = LimitRange(wTmpBrightness, 0, 255);
		AV_SetParameter(AVBRIGHTNESS, wTmpBrightness);
		SetDlgItemInt(hDlg, ID_EBBRIGHTNESS, wTmpBrightness, FALSE);
		hCtl = GetDlgItem(hDlg, ID_SBBRIGHTNESS);
		SetScrollRange(hCtl, SB_CTL ,0 ,255 ,FALSE);
		SetScrollPos(hCtl, SB_CTL, wTmpBrightness, TRUE);

		wTmpContrast = LimitRange(wTmpContrast, 0, 15);
		AV_SetParameter(AVCONTRAST, wTmpContrast);
		SetDlgItemInt(hDlg, ID_EBCONTRAST, wTmpContrast, FALSE);
		hCtl = GetDlgItem(hDlg, ID_SBCONTRAST);
		SetScrollRange(hCtl, SB_CTL, 0, 15, FALSE);
		SetScrollPos(hCtl, SB_CTL, wTmpContrast, TRUE);
 
		wTmpSaturation = LimitRange(wTmpSaturation, 0, 15);
		AV_SetParameter(AVSATURATION, wTmpSaturation);
		SetDlgItemInt(hDlg, ID_EBSATURATION, wTmpSaturation, FALSE);
		hCtl = GetDlgItem(hDlg, ID_SBSATURATION);
		SetScrollRange(hCtl, SB_CTL, 0, 15, FALSE);
		SetScrollPos(hCtl, SB_CTL, wTmpSaturation, TRUE);

		wTmpHue = LimitRange(wTmpHue, 0, 255);
		AV_SetParameter(AVHUE, wTmpHue);
		SetDlgItemInt(hDlg, ID_EBHUE, wTmpHue, FALSE);
		hCtl = GetDlgItem(hDlg, ID_SBHUE);
		SetScrollRange(hCtl, SB_CTL, 0, 255, FALSE);
		SetScrollPos(hCtl, SB_CTL, wTmpHue, TRUE);

		wTmpXPosition = LimitRange(wTmpXPosition, 0, 1023);
		AV_SetParameter(AVXPOSITION, wTmpXPosition);
		SetDlgItemInt(hDlg, ID_EBXPOSITION,  wTmpXPosition,  FALSE);
		hCtl = GetDlgItem(hDlg, ID_SBXPOSITION);
		SetScrollRange(hCtl, SB_CTL, 0, 1023, FALSE);
		SetScrollPos(hCtl, SB_CTL, wTmpXPosition, TRUE);

		wTmpYPosition = LimitRange(wTmpYPosition, 0, 1023);
		AV_SetParameter(AVYPOSITION, wTmpYPosition);
		SetDlgItemInt(hDlg, ID_EBYPOSITION,  wTmpYPosition,  FALSE);
		hCtl = GetDlgItem(hDlg, ID_SBYPOSITION);
		SetScrollRange(hCtl, SB_CTL, 0, 1023, FALSE);
		SetScrollPos(hCtl, SB_CTL, wTmpYPosition, TRUE);

		AV_UpdateVideo();
		break;

		case WM_HSCROLL:
		hCtl = HIWORD (lParam);
		wID = GetWindowWord (hCtl, GWW_ID);
		switch (wID)
			{
			case ID_SBBRIGHTNESS:
			wTmpBrightness = ColorProcessScroll (hDlg, hCtl,
				wParam, lParam, wTmpBrightness, ID_EBBRIGHTNESS);
			AV_SetParameter(AVBRIGHTNESS, wTmpBrightness);
			break;

			case ID_SBCONTRAST:
			wTmpContrast = ColorProcessScroll (hDlg, hCtl,
				wParam, lParam, wTmpContrast, ID_EBCONTRAST);
			AV_SetParameter(AVCONTRAST, wTmpContrast);
			break;

			case ID_SBSATURATION:
			wTmpSaturation = ColorProcessScroll (hDlg, hCtl, wParam,
				lParam, wTmpSaturation, ID_EBSATURATION);
			AV_SetParameter(AVSATURATION, wTmpSaturation);
			break;

			case ID_SBHUE:
			wTmpHue = ColorProcessScroll (hDlg, hCtl, wParam,
				lParam, wTmpHue, ID_EBHUE);
			AV_SetParameter(AVHUE, wTmpHue);
			break;

			case ID_SBXPOSITION:
			wTmpXPosition = ColorProcessScroll (hDlg, hCtl, wParam,
				lParam, wTmpXPosition, ID_EBXPOSITION);
			AV_SetParameter(AVXPOSITION, wTmpXPosition);
			break;

			case ID_SBYPOSITION:
			wTmpYPosition = ColorProcessScroll (hDlg, hCtl, wParam,
				lParam, wTmpYPosition, ID_EBYPOSITION);
			AV_SetParameter(AVYPOSITION, wTmpYPosition);
			break;
			}
		AV_UpdateVideo();
		break;

		case WM_ENDSESSION:     
		if (wParam)
			EndDialog (hDlg, IDCANCEL);
		break;

		case WM_COMMAND:
		switch (wParam)
			{
			// OK button saves configuration.
			case ID_PBOK:
			AV_SaveConfiguration();
			EndDialog(hDlg, IDOK);
			break;

			// Cancel button restores old configuration.
			case IDCANCEL:
			AV_LoadConfiguration();
			AV_UpdateVideo();
			EndDialog(hDlg, IDCANCEL);
			break;

			// Default button resets some parameters.
			case ID_PBDEFAULT:
			wTmpBrightness = 128;
			wTmpContrast   = 8;
			wTmpSaturation = 8;
			wTmpHue        = 128;
			SendMessage (hDlg, WM_UPDATEDIALOG, 0, 0L);
			break;

			// Leaving an edit box should cause all edit boxes
			// to be reread.
			case ID_EBBRIGHTNESS:
			case ID_EBCONTRAST:
			case ID_EBSATURATION:
			case ID_EBHUE:
			case ID_EBXPOSITION:
			case ID_EBYPOSITION:
			if (HIWORD(lParam) != EN_KILLFOCUS)
				return FALSE;

			// Intentional fallthrough.
			// Enter key should reread edit boxes.
			case IDOK:
			wTmpBrightness = GetDlgItemInt (hDlg,
				ID_EBBRIGHTNESS, NULL, FALSE);
			wTmpContrast = GetDlgItemInt (hDlg,
				ID_EBCONTRAST, NULL, FALSE);
			wTmpSaturation = GetDlgItemInt (hDlg,
				ID_EBSATURATION, NULL, FALSE);
			wTmpHue = GetDlgItemInt (hDlg,
				ID_EBHUE, NULL, FALSE);
			wTmpXPosition = GetDlgItemInt (hDlg,
				ID_EBXPOSITION, NULL, FALSE);
			wTmpYPosition = GetDlgItemInt (hDlg,
				ID_EBYPOSITION, NULL, FALSE);
			SendMessage (hDlg, WM_UPDATEDIALOG, 0, 0L);
			break;

			default:
			break;
			}
		break;

                case WM_HELP:
                        WinHelp (((LPHELPINFO) lParam)->hItemHandle, 
                                szHelpFileName, HELP_WM_HELP,
                                (DWORD) (LPSTR) aDisplayDlgHelpIDs);
                        break;

                case WM_CONTEXTMENU:
                        WinHelp ((HWND) wParam, 
                                szHelpFileName, HELP_CONTEXTMENU,
                                (DWORD) (LPSTR) aDisplayDlgHelpIDs);
                        break;

		default:
		return FALSE;
		}
	return TRUE;
	}


//
// Dialog proc for the video source dialog box (VIDEO_EXTERNALIN channel)
//

int FAR PASCAL _loadds VideoSourceDlgProc(
	HWND	hDlg,
	WORD	msg,
	WORD	wParam,
	LONG	lParam)
	{
	int	j;

	switch (msg)
		{
		case WM_INITDIALOG:
		wTmpSourceConnector = gwSourceConnector;
		wTmpVideoStandard = gwVideoStandard;
		wTmpVideoCableFormat = gwVideoCableFormat;

		// Disable unused inputs.
		for (j = HW_GetVideoChannelCount(); j < 3; j++)
			{
			EnableWindow(GetDlgItem(hDlg,ID_PBSOURCE0+j),FALSE);
			}
#if 0
		EnableWindow(GetDlgItem(hDlg,ID_PBSVIDEO),HW_HasSVideo());
#endif
		// Intentional fall through...

		case WM_UPDATEDIALOG:
		HW_SetVideoSource (wTmpSourceConnector);
		CheckRadioButton( hDlg, ID_PBSOURCE0, ID_PBSOURCE2, 
			ID_PBSOURCE0 + wTmpSourceConnector);

		HW_SetVideoStandard (wTmpVideoStandard);
		CheckRadioButton( hDlg, ID_PBNTSC, ID_PBPAL, 
			ID_PBNTSC + wTmpVideoStandard);

#if 0
		HW_SetVideoCableFormat (wTmpVideoCableFormat);
		CheckRadioButton( hDlg, ID_PBCOMPOSITE, ID_PBSVIDEO, 
			ID_PBCOMPOSITE + wTmpVideoCableFormat);
#endif
		break;


		// User will unload this module at exit time, so make
		// sure that the dialog is gone if the user just quits 
		// Windows with the dialog box up.
		case WM_ENDSESSION:     
		if (wParam)
			EndDialog (hDlg, IDCANCEL);
		break;

		case WM_COMMAND:
		switch (wParam)
			{
			case IDOK:
			gwSourceConnector = wTmpSourceConnector;
			gwVideoStandard = wTmpVideoStandard;
			gwVideoCableFormat = wTmpVideoCableFormat;
			ConfigPutSettings();	// Save settings
			EndDialog(hDlg, IDOK);
			break;

			case IDCANCEL:
			// Fix, restore all values
			HW_SetVideoSource (gwSourceConnector);
			HW_SetVideoStandard (gwVideoStandard);
			EndDialog(hDlg, IDCANCEL);
			break;

			case ID_PBDEFAULT:
			wTmpSourceConnector = HW_DEFAULT_INPUT;
			wTmpVideoStandard = HW_DEFAULT_STANDARD;
			wTmpVideoCableFormat = 0;
			HW_SetVideoSource (wTmpSourceConnector);
			HW_SetVideoStandard (wTmpVideoStandard);
			HW_SetVideoCableFormat (wTmpVideoCableFormat);
			SendMessage (hDlg, WM_UPDATEDIALOG, 0, 0L);
			break;

			case ID_PBSOURCE0:
			case ID_PBSOURCE1:
			case ID_PBSOURCE2:
			wTmpSourceConnector = wParam - ID_PBSOURCE0;
			SendMessage (hDlg, WM_UPDATEDIALOG, 0, 0L);
			break;

			case ID_PBNTSC:
			case ID_PBPAL:
			wTmpVideoStandard = wParam - ID_PBNTSC;
			SendMessage (hDlg, WM_UPDATEDIALOG, 0, 0L);
			break;
#if 0
			case ID_PBCOMPOSITE:
			case ID_PBSVIDEO:
			wTmpVideoCableFormat = wParam - ID_PBCOMPOSITE;
			SendMessage (hDlg, WM_UPDATEDIALOG, 0, 0L);
			break;
#endif

			default:
			break;
			}
		break;
	
                case WM_HELP:
                        WinHelp (((LPHELPINFO) lParam)->hItemHandle, 
                                szHelpFileName, HELP_WM_HELP,
                                (DWORD) (LPSTR) aSourceDlgHelpIDs);
                        break;

                case WM_CONTEXTMENU:
                        WinHelp ((HWND) wParam, 
                                szHelpFileName, HELP_CONTEXTMENU,
                                (DWORD) (LPSTR) aSourceDlgHelpIDs);
                        break;

		default:
		return FALSE;
		}

	return TRUE;
	}
