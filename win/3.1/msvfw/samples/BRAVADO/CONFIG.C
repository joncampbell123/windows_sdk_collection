/****************************************************************************
 *
 *   config.c
 * 
 *   Dialog Box processing
 *
 *   Microsoft Video for Windows Sample Capture Driver
 *   Chips & Technologies 9001 based frame grabbers.
 *
 ***************************************************************************/
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
#include <mmsystem.h>
#include <stdlib.h>
#include <msvideo.h>
#include <msviddrv.h>
#include "ct.h"
#include "ctdev.h"      // Device specific include
#include "config.h"
#include "ver.h"

#define WIDTHBYTES(i)     ((unsigned)((i+31)&(~31))/8)  /* ULONG aligned ! */
#define WM_UPDATEDIALOG         (WM_USER+200)

typedef struct tagVS_VERSION
{
    WORD wTotLen;
    WORD wValLen;
    char szSig[16];
    VS_FIXEDFILEINFO vffInfo;
} VS_VERSION;

typedef struct tagLANGANDCP
{
    WORD wLanguage;
    WORD wCodePage;
} LANGANDCP;

static WORD wTmpVideoFormat;
static WORD wTmpSize40;         // legal range is 1 to 40 (= 40 to 640)

static int iTmpHue;             // might be decremented to -1
static int iTmpSat;
static int iTmpContrast;
static int iTmpBrightness;
static int iTmpRed;
static int iTmpGreen;
static int iTmpBlue;

static WORD wTmpSourceConnector;        // 0-3
static WORD wTmpVideoStandard;          // 0 = NTSC, 1 = PAL
static WORD wTmpVideoCableFormat;       // 0 = Composite, 1 = SVideo

static int  gwHue;
static int  gwSat;
static int  gwContrast;
static int  gwBrightness;
static int  gwRed;
static int  gwGreen;
static int  gwBlue;
static int  gwVideoStandard;
static WORD gwVideoCableFormat;         // 0 = Composite, 1 = SVideo

/***************************************************************************
    The following are used both to load the config dialog list boxes,
    but also to confirm INI values.
***************************************************************************/

#ifdef _VBLASTER
// This will be replaced with the values returned by vbcGetPortAddress
COMBOBOX_ENTRY BCODE gwPortBaseOptions [] = 
{
       0x2AD6, "0x2AD6        "         // Leave the spaces for wsprintf!
};
#define N_PORTBASEOPTIONS (sizeof (gwPortBaseOptions) / sizeof (COMBOBOX_ENTRY))

COMBOBOX_ENTRY  gwInterruptOptions [] = 
{
       5, "5",
       10, "10",
       11, "11",
       12, "12"
};
#define N_INTERRUPTOPTIONS (sizeof (gwInterruptOptions) / sizeof (COMBOBOX_ENTRY))
#endif // _VBLASTER specific

/* ------------------------------------------------------- */
#ifdef _BRAVADO

COMBOBOX_ENTRY BCODE gwPortBaseOptions [] = 
{
       0x204, "0x204",
       0x214, "0x214",
       0x224, "0x224 (Default)",
       0x234, "0x234",
       0x304, "0x304",
       0x314, "0x314",
       0x3A4, "0x3A4"
};
#define N_PORTBASEOPTIONS (sizeof (gwPortBaseOptions) / sizeof (COMBOBOX_ENTRY))

COMBOBOX_ENTRY  gwInterruptOptions [] = 
{
       9, "9"
};
#define N_INTERRUPTOPTIONS (sizeof (gwInterruptOptions) / sizeof (COMBOBOX_ENTRY))
#endif // BRAVADO specific

COMBOBOX_ENTRY  gwMemoryBaseOptions [] = 
{
       0x02, "2 Meg  (0x200000)",
       0x03, "3 Meg  (0x300000)",
       0x04, "4 Meg  (0x400000)",
       0x05, "5 Meg  (0x500000)",
       0x06, "6 Meg  (0x600000)",
       0x07, "7 Meg  (0x700000)",
       0x08, "8 Meg  (0x800000)",
       0x09, "9 Meg  (0x900000)",
       0x0a, "10 Meg (0xA00000)",
       0x0b, "11 Meg (0xB00000)",
       0x0c, "12 Meg (0xC00000)",
       0x0d, "13 Meg (0xD00000)",
       0x0e, "14 Meg (0xE00000)",
       0x0f, "15 Meg (0xF00000)",
       0x1f, "31 Meg (some motherboards)",
       0x2f, "47 Meg (some motherboards)"
};
#define N_MEMORYBASEOPTIONS (sizeof (gwMemoryBaseOptions) / sizeof (COMBOBOX_ENTRY))


static COMBOBOX_ENTRY  gwSizeOptions [] = 
{
       1,		"40 x 30",
       2,		"80 x 60",
       3,		"120 x 90",
       4,		"160 x 120",
       5,		"200 x 150",
       6,		"240 x 180",
       7,		"280 x 210",
       8,		"320 x 240",
       9,		"360 x 270",
       10,		"400 x 300",
       11,		"440 x 330",
       12,		"480 x 360",
       13,		"520 x 390",
       14,		"560 x 420",
       15,		"600 x 450",
       16,		"640 x 480"
};                      
#define N_SIZEOPTIONS (sizeof (gwSizeOptions) / sizeof (COMBOBOX_ENTRY))

static COMBOBOX_ENTRY  gwFormatOptions [] = 
{
       IMAGE_FORMAT_PAL8,    		"8 bit Palettized",
       IMAGE_FORMAT_RGB16,		"16 bit RGB",
       IMAGE_FORMAT_RGB24,		"24 bit RGB"
#if DEBUG
       , IMAGE_FORMAT_YUV411UNPACKED,	"YUV 411 (Unpacked)"
#endif
};
#define N_FORMATOPTIONS (sizeof (gwFormatOptions) / sizeof (COMBOBOX_ENTRY))


//
// This initalizes the destination BitmapInfo header
//
void FAR PASCAL InitDestBIHeader (LPBITMAPINFOHEADER lpbi, 
                WORD wEnumFormat,  WORD wWidth)
{
    lpbi->biSize          = sizeof (BITMAPINFOHEADER);
    lpbi->biWidth         = wWidth;
    lpbi->biHeight        = (lpbi->biWidth * 3) / 4;
    lpbi->biPlanes        = 1;
    lpbi->biXPelsPerMeter = 0L;
    lpbi->biYPelsPerMeter = 0L;
    lpbi->biClrUsed       = 0L;
    lpbi->biClrImportant  = 0L;

    switch (wEnumFormat) {
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
        case IMAGE_FORMAT_YUV411PACKED:
            lpbi->biBitCount = 16;      
            lpbi->biCompression = ckidYUV411Packed;  
            break;
        case IMAGE_FORMAT_YUV411UNPACKED:
            lpbi->biBitCount = 16; 
            lpbi->biCompression = ckidYUV411Unpacked;
            break;
    }
    if (lpbi->biCompression == BI_RGB)
        lpbi->biSizeImage     = (DWORD)biDest.biHeight * 
                (DWORD)WIDTHBYTES(biDest.biWidth * biDest.biBitCount);
    else {
        lpbi->biSizeImage     = (DWORD)biDest.biHeight * 
                (DWORD)WIDTHBYTES(biDest.biWidth * biDest.biBitCount);
    }
}

//
// Initalizes all state variables.  Called once at startup.
//
BOOL FAR PASCAL ConfigInit (LPDEVICE_INIT lpDI)
{
    // Set Hue, Contrast, Zoom, etc.
    CT_SetColor (CT_COLOR_HUE, gwHue);
    CT_SetColor (CT_COLOR_SAT, gwSat);
    CT_SetColor (CT_COLOR_BRIGHTNESS, gwBrightness);
    CT_SetColor (CT_COLOR_CONTRAST, gwContrast);
    CT_SetColor (CT_COLOR_RED, gwRed);
    CT_SetColor (CT_COLOR_GREEN, gwGreen);
    CT_SetColor (CT_COLOR_BLUE, gwBlue);
    CT_SetVideoSource (gwSourceConnector);
    CT_SetVideoStandard (gwVideoStandard);
    CT_SetVideoCableFormat (gwVideoCableFormat);

    gfEurope = gwVideoStandard != 0;

    // The following constant is never changed
    gwWidthBytes = 2048;        // Width of a line in the frame buffer
        
    // New way
    biSource.biSize          = sizeof (BITMAPINFOHEADER);
    biSource.biWidth         = 720;
    biSource.biHeight        = 480;
    biSource.biPlanes        = 1;
    biSource.biBitCount      = 8;
    biSource.biCompression   = BI_RGB;
    biSource.biSizeImage     = 
        WIDTHBYTES(biSource.biWidth * biSource.biBitCount) 
        * biSource.biHeight;
    biSource.biXPelsPerMeter = 0L;
    biSource.biYPelsPerMeter = 0L;
    biSource.biClrUsed       = 0L;
    biSource.biClrImportant  = 0L;

    SetSourceFormat(&biSource, sizeof (biSource));

    InitDestBIHeader (&biDest, gwDestFormat, gwSize40 * 40);

    // Try to set the format using the last saved configuration
    if (SetDestFormat (&biDest, sizeof (biDest))) {
        // Initialization failed, try again using minimum sizes
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
    BYTE  buf[20];
    WORD  n;
    BYTE  b;
    int   i;

    n = GetPrivateProfileString(szApp, szEntry, gszNULL, buf, sizeof(buf), gszIniFile);
    if (n < 1) return wDef;

    for (n=0,i=0; b=buf[i]; i++) {
        if (b > 'Z') b -= 'a' - 'A';
        b -= '0';
        if (b > 9) b -= 7;
        if (b > 15)
            break;
        n = n * 16 + b;
    }
    return n;
}

//
// Returns values set by config dialog from SYSTEM.INI
//
int FAR PASCAL GetHardwareSettingsFromINI( LPDEVICE_INIT lpDI )
{
    lpDI->bInterrupt = (BYTE)GetPrivateProfileInt(gszDriverName, gszIntKey, 10, gszIniFile);
    lpDI->wSegment   = GetProfileHex(gszDriverName, gszMemoryKey, 0x0e );

    // The BasePort is trouble.  For the Bravado, we can set it anytime.
    // So it is stored in SYSTEM.INI
    // For the VBlaster, we ask the driver how it was previously setup,
    // (by VBSETENV.EXE) which was saved in AUTOEXEC.BAT
#ifdef _BRAVADO
    lpDI->wIOBase    = GetProfileHex(gszDriverName, gszPortKey, CT_DEFAULT_IOBASE );
#endif
#ifdef _VBLASTER
    lpDI->wIOBase    = CT_GetPortAddress ();
#endif
    return 1;
}

//
// Saves settings from Config dialog into INI file
//
int FAR PASCAL PutHardwareSettingsToINI( LPDEVICE_INIT lpDI )
{
    char buf[64];

    wsprintf(buf, gszIntFormat, lpDI->bInterrupt);
    WritePrivateProfileString(gszDriverName, gszIntKey, buf, gszIniFile);
    wsprintf(buf, gszHexFormat, lpDI->wSegment);
    WritePrivateProfileString(gszDriverName, gszMemoryKey, buf, gszIniFile);
    wsprintf(buf, gszHexFormat, lpDI->wIOBase);
    WritePrivateProfileString(gszDriverName, gszPortKey, buf, gszIniFile);
    return 1;
}


//
// Retrieves settings from INI for everything except 
// those from the config dialog
// Fills globals with settings under [<drivename>.drv], HUE, SAT, INPUT
//
WORD FAR PASCAL ConfigGetSettings( void )
{
    WORD wTemp;

    wTemp = GetPrivateProfileInt(gszDriverName, gszHueKey, 
                CT_DEFAULT_HUE, gszIniFile);
    gwHue = LimitRange (wTemp, 0, CT_MAX_HUE);
    
    wTemp = GetPrivateProfileInt(gszDriverName, gszSatKey, 
                CT_DEFAULT_SAT, gszIniFile);
    gwSat = LimitRange (wTemp, 0, CT_MAX_SAT);

    wTemp = GetPrivateProfileInt(gszDriverName, gszBrightnessKey, 
                CT_DEFAULT_BRIGHTNESS, gszIniFile);
    gwBrightness = LimitRange (wTemp, 0, CT_MAX_BRIGHTNESS);

    wTemp = GetPrivateProfileInt(gszDriverName, gszContrastKey, 
                CT_DEFAULT_CONTRAST, gszIniFile);
    gwContrast = LimitRange (wTemp, 0, CT_MAX_CONTRAST);

    wTemp = GetPrivateProfileInt(gszDriverName, gszRedKey, 
                CT_DEFAULT_RED, gszIniFile);
    gwRed = LimitRange (wTemp, 0, CT_MAX_RED);

    wTemp = GetPrivateProfileInt(gszDriverName, gszGreenKey, 
                CT_DEFAULT_GREEN, gszIniFile);
    gwGreen = LimitRange (wTemp, 0, CT_MAX_GREEN);

    wTemp = GetPrivateProfileInt(gszDriverName, gszBlueKey, 
                CT_DEFAULT_BLUE, gszIniFile);
    gwBlue = LimitRange (wTemp, 0, CT_MAX_BLUE);

    wTemp = GetPrivateProfileInt(gszDriverName, gszInputChannelKey, 
                CT_DEFAULT_INPUT, gszIniFile);
    gwSourceConnector = LimitRange (wTemp, 0, CT_SOURCE2);

    wTemp = GetPrivateProfileInt(gszDriverName, gszVideoStandardKey, 
                CT_DEFAULT_STANDARD, gszIniFile);
    gwVideoStandard = LimitRange (wTemp, 0, 1);

    wTemp = GetPrivateProfileInt(gszDriverName, gszVideoCableKey, 
                0, gszIniFile);
    gwVideoCableFormat = LimitRange (wTemp, 0, 1); // 0=composite, 1=SVideo

    wTemp = GetPrivateProfileInt(gszDriverName, gszSize40Key, 
                4, gszIniFile);
    gwSize40 = LimitRange (wTemp, 1, MAX_SIZE_X_NTSC / 40);

    wTemp = GetPrivateProfileInt(gszDriverName, gszVideoFormatKey, 
                IMAGE_FORMAT_PAL8, gszIniFile);
    gwDestFormat = LimitRange (wTemp, 
                IMAGE_FORMAT_PAL8, IMAGE_FORMAT_YUV411UNPACKED);

    return TRUE;
}

//
// Saves all settings to the INI except those controlled by the config dialog
// Saves globals settings under [<drivename>.drv], HUE, SAT, INPUT
//
WORD FAR PASCAL ConfigPutSettings( void )
{
    char buf[40];

    wsprintf(buf, gszIntFormat, gwHue);
    WritePrivateProfileString(gszDriverName, gszHueKey, buf, gszIniFile);

    wsprintf(buf, gszIntFormat, gwSat);
    WritePrivateProfileString(gszDriverName, gszSatKey, buf, gszIniFile);

    wsprintf(buf, gszIntFormat, gwBrightness);
    WritePrivateProfileString(gszDriverName, gszBrightnessKey, buf, gszIniFile);

    wsprintf(buf, gszIntFormat, gwContrast);
    WritePrivateProfileString(gszDriverName, gszContrastKey, buf, gszIniFile);

    wsprintf(buf, gszIntFormat, gwRed);
    WritePrivateProfileString(gszDriverName, gszRedKey, buf, gszIniFile);

    wsprintf(buf, gszIntFormat, gwGreen);
    WritePrivateProfileString(gszDriverName, gszGreenKey, buf, gszIniFile);

    wsprintf(buf, gszIntFormat, gwBlue);
    WritePrivateProfileString(gszDriverName, gszBlueKey, buf, gszIniFile);

    wsprintf(buf, gszIntFormat, gwSourceConnector);
    WritePrivateProfileString(gszDriverName, gszInputChannelKey, buf, gszIniFile);

    wsprintf(buf, gszIntFormat, gwVideoStandard);
    WritePrivateProfileString(gszDriverName, gszVideoStandardKey, buf, gszIniFile);

    wsprintf(buf, gszIntFormat, gwVideoCableFormat);
    WritePrivateProfileString(gszDriverName, gszVideoCableKey, buf, gszIniFile);

    wsprintf(buf, gszIntFormat, gwSize40);
    WritePrivateProfileString(gszDriverName, gszSize40Key, buf, gszIniFile);

    wsprintf(buf, gszIntFormat, gwDestFormat);
    WritePrivateProfileString(gszDriverName, gszVideoFormatKey, buf, gszIniFile);

    return TRUE;
}


static void WINAPI ConfigErrorMsgBox( HWND hDlg, WORD wStringId )
{
    char    szPname[MAXPNAMELEN];
    char    szErrorBuffer[MAX_ERR_STRING]; // buffer for error messages

    LoadString(ghModule, IDS_VCAPPRODUCT, szPname, MAXPNAMELEN);
    LoadString(ghModule, wStringId, szErrorBuffer, sizeof(szErrorBuffer));
    MessageBox(hDlg, szErrorBuffer, szPname,
#ifdef BIDI
		MB_RTL_READING |
#endif
MB_OK|MB_ICONEXCLAMATION);
}


void WINAPI ConfigRemove( void )
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
static int WINAPI ConfigLoadComboBox(HWND hDlg, int wID,
       COMBOBOX_ENTRY  * pCBE, int nEntries, WORD wInitialValue)
{
    int j;
    int nIndex = 0;     // Zeroeth entry should be blank, None, etc.
    HWND hWndCB = GetDlgItem (hDlg, wID);

    for (j = 0; j < nEntries; j++) {
        SendMessage (hWndCB, CB_ADDSTRING, 0, (LONG) (LPSTR) ((pCBE+j)->szText));
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
static WORD WINAPI ConfigGetComboBoxValue (HWND hDlg, int wID,
       COMBOBOX_ENTRY  * pCBE)
{
    int nIndex;
    HWND hWndCB = GetDlgItem (hDlg, wID);

    nIndex = (int) SendMessage (hWndCB, CB_GETCURSEL, 0, 0L);
    nIndex = max (0, nIndex);   // LB_ERR is negative
    return pCBE[nIndex].wValue;
}


//
// Checks that a value passed matches an entry in a COMBOBOX_ENTRY list.
// Parameters:
//      array of text and values
//      Count of entries in the COMBOBOX_ENTRY array
//      Value to confirm matches an entry in the
//              value field of the COMBOBOX_ENTRY.  
// Returns:
//      Returns wValueToTest if a match is found, otherwise -1.
//
static WORD WINAPI ConfigConfirmLegalValue (COMBOBOX_ENTRY  * pCBE,
        int nEntries, WORD wValueToTest)
{
    int j;

    for (j = 0; j < nEntries; j++) 
        if (wValueToTest == pCBE[j].wValue)
            return wValueToTest;
    return -1;
}

//
// Checks that all values passed in the DEVICE_INIT structure are legal.
// Returns TRUE if all are OK, FALSE if any are out of range.
//
BOOL FAR PASCAL ConfigCheckAllDeviceInitParms (LPDEVICE_INIT lpDI)
{
    if (ConfigConfirmLegalValue (gwInterruptOptions, N_INTERRUPTOPTIONS,
                lpDI->bInterrupt) == -1)
        return FALSE;
    if (ConfigConfirmLegalValue (gwMemoryBaseOptions, N_MEMORYBASEOPTIONS,
                lpDI->wSegment) == -1)
        return FALSE;

    return TRUE;     
}

//
// Puts up the configuration dialog box.
// Returns whatever was returned from the dialog box procedure.
//
int WINAPI Config(HWND hWnd, HANDLE hModule)
{
    return DialogBox(hModule, MAKEINTATOM(DLG_VIDEOCONFIG), hWnd, (FARPROC)ConfigDlgProc);
}

//
// Configuration dialog displayed by the drivers applet
//
// Returns DRVCNF_RESTART if the user has changed settings, which will
//     cause the drivers applet which launched this to give the user a
//     message about having to restart Windows for the changes to take
//     effect.  If the user clicks on "Cancel" or if no settings have changed,
//     DRVCNF_CANCEL is returned.
//
int FAR PASCAL _loadds ConfigDlgProc(HWND hDlg, WORD msg, WORD wParam, LONG lParam)
{
    DEVICE_INIT devInit;         // structure to hold IO Addx, Interrupt, MemBase
    DEVICE_INIT devInit2;

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            LPSTR   lpVersion;       
            WORD    wVersionLen;
            BOOL    bRetCode;
            char    szGetName[_MAX_PATH];
            DWORD dwVerInfoSize;
            DWORD dwVerHnd;
            char szBuf[_MAX_PATH];

            // find the version number
            GetModuleFileName (ghModule, szBuf, sizeof (szBuf));

            // You must fine the file size first before getting any file info
            dwVerInfoSize =
                GetFileVersionInfoSize(szBuf, &dwVerHnd);

            if (dwVerInfoSize) {
                LPSTR   lpstrVffInfo;             // Pointer to block to hold info
                HANDLE  hMem;                     // handle to mem alloc'ed

                // Get a block big enough to hold version info
                hMem          = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
                lpstrVffInfo  = GlobalLock(hMem);

                // Get the File Version first
                if(GetFileVersionInfo(szBuf, 0L, dwVerInfoSize, lpstrVffInfo)) {
                     VS_VERSION FAR *pVerInfo = (VS_VERSION FAR *) lpstrVffInfo;

                     // fill in the file version                                    
                     wsprintf(szBuf,
                              "Version:  %d.%d.%d.%d", 
                              HIWORD(pVerInfo->vffInfo.dwFileVersionMS),
                              LOWORD(pVerInfo->vffInfo.dwFileVersionMS),
                              HIWORD(pVerInfo->vffInfo.dwFileVersionLS),
                              LOWORD(pVerInfo->vffInfo.dwFileVersionLS));
                     SetDlgItemText(hDlg, ID_DRIVERVERSION, szBuf);
                }

                // Now try to get the FileDescription
                // Do this the American english translation be default.  
                // Keep track of the string length for easy updating.  
                // 040904E4 represents the language ID and the four 
                // least significant digits represent the codepage for 
                // which the data is formatted.  The language ID is 
                // composed of two parts: the low ten bits represent 
                // the major language and the high six bits represent 
                // the sub language.

                lstrcpy(szGetName, "\\StringFileInfo\\040904E4\\FileDescription");

                wVersionLen   = 0;
                lpVersion     = NULL;

                // Look for the corresponding string. 
                bRetCode      =  VerQueryValue((LPVOID)lpstrVffInfo,
			                (LPSTR)szGetName,
			                (void FAR* FAR*)&lpVersion,
			                (UINT FAR *) &wVersionLen);

                if ( bRetCode && wVersionLen && lpVersion)
                   SetDlgItemText(hDlg, ID_FILEDESCRIPTION, lpVersion);

                // Let go of the memory
                GlobalUnlock(hMem);
                GlobalFree(hMem);
            }

            GetHardwareSettingsFromINI (&devInit);

            ConfigLoadComboBox (hDlg, ID_LBINTERRUPTNUMBER,
                gwInterruptOptions, N_INTERRUPTOPTIONS,
                (WORD) devInit.bInterrupt);
            ConfigLoadComboBox (hDlg, ID_LBMEMORYBASEADDRESS,
                gwMemoryBaseOptions, N_MEMORYBASEOPTIONS,
                devInit.wSegment);
            // Total VBlaster hack.  Stuff the current values into array.
#if _VBLASTER
            gwPortBaseOptions[0].wValue = devInit.wIOBase;
            wsprintf (gwPortBaseOptions[0].szText, "0x%X", devInit.wIOBase);
#endif
            ConfigLoadComboBox (hDlg, ID_LBPORTBASEADDRESS,
                gwPortBaseOptions, N_PORTBASEOPTIONS, 
                devInit.wIOBase);
        }
        break;

        // User will unload this module at exit time, so make
        // sure that the dialog is gone if the user just quits 
        // Windows with the dialog box up.
        case WM_ENDSESSION:     
            if (wParam)
                EndDialog (hDlg, DRV_CANCEL);
            break;

        case WM_COMMAND:
            switch (wParam)
            {
                case IDOK:
                    devInit.wIOBase = ConfigGetComboBoxValue (hDlg, ID_LBPORTBASEADDRESS,
                        gwPortBaseOptions);
                    devInit.bInterrupt = (BYTE) ConfigGetComboBoxValue (hDlg, ID_LBINTERRUPTNUMBER,
                        gwInterruptOptions);
                    devInit.wSegment = ConfigGetComboBoxValue (hDlg, ID_LBMEMORYBASEADDRESS,
                        gwMemoryBaseOptions);

                    if (!(InitVerifyConfiguration( &devInit)))
                    {
                        ConfigErrorMsgBox(hDlg, IDS_ERRBADCONFIG);
                        break;
                    }
                    // Get the original again to see if anything changed
                    GetHardwareSettingsFromINI (&devInit2);
                    PutHardwareSettingsToINI( &devInit);

                    // Ask for a restart if anything serious changed
                    // and the driver was already in use...
                    // Note that an open for config only doesn't bump
                    // the usage count
                    if ((gwDriverUsage != 0) && 
                        ((devInit.wIOBase != devInit2.wIOBase) ||
                         (devInit.bInterrupt != devInit2.bInterrupt) ||
                         (devInit.wSegment != devInit2.wSegment) 
                        ))
                        EndDialog(hDlg, DRV_RESTART);
                    else
                        EndDialog(hDlg, DRV_CANCEL);
                    break;

                case IDCANCEL:
                    EndDialog(hDlg, DRV_CANCEL);
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


VOID UpdateSizeDisplay (HWND hDlg)
{
    SendDlgItemMessage (hDlg, ID_LBSIZE, CB_SETCURSEL, wTmpSize40 - 1, 0L);
}

//
// Dialog proc for the video format dialog box (VIDEO_IN channel)
//
int FAR PASCAL _loadds VideoFormatDlgProc(HWND hDlg, WORD msg, WORD wParam, LONG lParam)
{
    int j;
    BITMAPINFOHEADER bi;

    switch (msg)
    {
        case WM_INITDIALOG:
	    wTmpVideoFormat = gwDestFormat;
            wTmpSize40 = gwSize40;
            ConfigLoadComboBox( hDlg, ID_LBSIZE,
                gwSizeOptions, N_SIZEOPTIONS, 
                wTmpSize40);

            ConfigLoadComboBox( hDlg, ID_LBIMAGEFORMAT,
                gwFormatOptions, N_FORMATOPTIONS, 
                gwDestFormat);
            UpdateSizeDisplay (hDlg);
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
			    ID_LBIMAGEFORMAT,
			    gwFormatOptions);

                    InitDestBIHeader (&bi, j,  gwSize40 * 40);
                    SetDestFormat (&bi, sizeof (bi));

                    ConfigPutSettings();    // Save settings
                    EndDialog(hDlg, IDOK);
                    break;

                case IDCANCEL:
                    EndDialog(hDlg, IDCANCEL);
                    break;

                case ID_PBSIZEFULL:
		    wTmpSize40 = (MAX_SIZE_X_NTSC / 40);
                    UpdateSizeDisplay (hDlg);
                    break;

                case ID_PBSIZEHALF:
		    wTmpSize40 = (MAX_SIZE_X_NTSC / 40) / 2;
                    UpdateSizeDisplay (hDlg);
                    break;

                case ID_PBSIZEQUARTER:
		    wTmpSize40 = (MAX_SIZE_X_NTSC / 40) / 4;
                    UpdateSizeDisplay (hDlg);
                    break;

                case ID_PBSIZEEIGHTH:
		    wTmpSize40 = (MAX_SIZE_X_NTSC / 40) / 8;
                    UpdateSizeDisplay (hDlg);
                    break;

                case ID_LBSIZE:
                    if (HIWORD (lParam) == CBN_KILLFOCUS) {
                        wTmpSize40 = (WORD) SendDlgItemMessage (hDlg, 
                                ID_LBSIZE, CB_GETCURSEL, 0, 0L) + 1;
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


int ColorProcessScroll (HWND hDlg, HWND hCtl, WORD wParam, DWORD lParam, 
        int iVal, WORD wIDEditBox)
{
    switch (wParam) {
        case SB_LINEDOWN:
           iVal++;
           break;
        case SB_LINEUP: 
           iVal--;
           break;
        case SB_PAGEDOWN:
           iVal += MAX_COLOR_VALUE / 8;
           break;
        case SB_PAGEUP: 
           iVal -= MAX_COLOR_VALUE / 8;
           break;
        case SB_THUMBTRACK:
            iVal = LOWORD (lParam);
            break;
        default:
            break;
     }
     iVal = LimitRange (iVal, 0, MAX_COLOR_VALUE);
     SetScrollPos   ( hCtl, SB_CTL, iVal, TRUE);
     SetDlgItemInt (hDlg, wIDEditBox, iVal, TRUE);
     return iVal;
}


//
// Dialog proc for the video source dialog box (VIDEO_EXTERNALIN channel)
//
int FAR PASCAL _loadds VideoSourceDlgProc(HWND hDlg, WORD msg, WORD wParam, LONG lParam)
{
    HWND hCtl;
    WORD wID;
    int  j;

    switch (msg)
    {
        case WM_INITDIALOG:
	    wTmpSourceConnector = gwSourceConnector;
            wTmpVideoStandard = gwVideoStandard;
            wTmpVideoCableFormat = gwVideoCableFormat;

	    iTmpHue = gwHue;
            // Disable unused inputs
            for (j = CT_GetVideoChannelCount(); j < 3; j++) {
                EnableWindow (GetDlgItem (hDlg, 
                        ID_PBSOURCE0 + j), FALSE);
            }

            EnableWindow (GetDlgItem (hDlg, ID_PBSVIDEO), 
                CT_HasSVideo () );

            // Hue
            hCtl = GetDlgItem (hDlg, ID_SBHUE);
            SetScrollRange ( hCtl, SB_CTL, 0, CT_MAX_HUE, FALSE);

            // Intentional fall through

        case WM_UPDATEDIALOG:
            CT_SetVideoSource (wTmpSourceConnector);
            CheckRadioButton( hDlg, ID_PBSOURCE0, ID_PBSOURCE2, 
                ID_PBSOURCE0 + wTmpSourceConnector);

            CT_SetVideoStandard (wTmpVideoStandard);
            CheckRadioButton( hDlg, ID_PBNTSC, ID_PBPAL, 
                ID_PBNTSC + wTmpVideoStandard);

            CT_SetVideoCableFormat (wTmpVideoCableFormat);
            CheckRadioButton( hDlg, ID_PBCOMPOSITE, ID_PBSVIDEO, 
                ID_PBCOMPOSITE + wTmpVideoCableFormat);

            // Hue
            hCtl = GetDlgItem (hDlg, ID_SBHUE);
            SetScrollPos   ( hCtl, SB_CTL, iTmpHue, TRUE);
            SetDlgItemInt (hDlg, ID_EBHUE, iTmpHue, FALSE);
            break;


        // User will unload this module at exit time, so make
        // sure that the dialog is gone if the user just quits 
        // Windows with the dialog box up.
        case WM_ENDSESSION:     
            if (wParam)
                EndDialog (hDlg, IDCANCEL);
            break;

        case WM_HSCROLL:
            hCtl = HIWORD (lParam);
            wID = GetWindowWord (hCtl, GWW_ID);
            switch (wID) {
                case ID_SBHUE:
                    iTmpHue = ColorProcessScroll (hDlg, hCtl, wParam, lParam, 
                                iTmpHue, ID_EBHUE);
                    CT_SetColor (CT_COLOR_HUE, iTmpHue);
                    break;
                default:
                    break;
            }
            break;
	    
        case WM_COMMAND:
            switch (wParam)
            {
                case IDOK:
		    gwHue = iTmpHue;
		    gwSourceConnector = wTmpSourceConnector;
		    gwVideoStandard = wTmpVideoStandard;
		    gwVideoCableFormat = wTmpVideoCableFormat;
                    ConfigPutSettings();    // Save settings
                    EndDialog(hDlg, IDOK);
                    break;

                case IDCANCEL:
                    // Fix, restore all values
                    CT_SetColor (CT_COLOR_HUE, gwHue);
                    CT_SetVideoSource (gwSourceConnector);
                    CT_SetVideoStandard (gwVideoStandard);
                    EndDialog(hDlg, IDCANCEL);
                    break;

                case ID_PBDEFAULT:
	            iTmpHue = CT_DEFAULT_HUE;
	            wTmpSourceConnector = CT_DEFAULT_INPUT;
                    wTmpVideoStandard = CT_DEFAULT_STANDARD;
                    wTmpVideoCableFormat = 0;

                    CT_SetColor (CT_COLOR_HUE, iTmpHue);
                    CT_SetVideoSource (wTmpSourceConnector);
                    CT_SetVideoStandard (wTmpVideoStandard);
                    CT_SetVideoCableFormat (wTmpVideoCableFormat);

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

                case ID_PBCOMPOSITE:
                case ID_PBSVIDEO:
		    wTmpVideoCableFormat = wParam - ID_PBCOMPOSITE;
                    SendMessage (hDlg, WM_UPDATEDIALOG, 0, 0L);
                    break;

                case ID_EBHUE:
                    if (HIWORD (lParam) == EN_KILLFOCUS) {
                        iTmpHue = GetDlgItemInt (hDlg, ID_EBHUE, NULL, FALSE);
                        iTmpHue = LimitRange (iTmpHue, 0, CT_MAX_HUE);
                        SendMessage (hDlg, WM_UPDATEDIALOG, 0, 0L);
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

//
// Dialog proc for the video monitor (VIDEO_EXTERNALOUT channel)
//
int FAR PASCAL _loadds VideoMonitorDlgProc(HWND hDlg, WORD msg, WORD wParam, LONG lParam)
{
    HWND hCtl;
    WORD wID;

    switch (msg)
    {
        case WM_INITDIALOG:
	    iTmpSat = gwSat;
            iTmpBrightness = gwBrightness;
	    iTmpContrast = gwContrast;
            iTmpRed = gwRed;
            iTmpGreen = gwGreen;
            iTmpBlue = gwBlue;
            // Intentional fall through

        case WM_UPDATEDIALOG:

            // Sat
            hCtl = GetDlgItem (hDlg, ID_SBSAT);
            SetScrollRange ( hCtl, SB_CTL, 0, CT_MAX_SAT, FALSE);
            SetScrollPos   ( hCtl, SB_CTL, iTmpSat, TRUE);
            SetDlgItemInt (hDlg, ID_EBSAT, iTmpSat, FALSE);
            CT_SetColor (CT_COLOR_SAT, iTmpSat);

            // Brightness
            hCtl = GetDlgItem (hDlg, ID_SBBRIGHTNESS);
            SetScrollRange ( hCtl, SB_CTL, 0, CT_MAX_BRIGHTNESS, FALSE);
            SetScrollPos   ( hCtl, SB_CTL, iTmpBrightness, TRUE);
            SetDlgItemInt (hDlg, ID_EBBRIGHTNESS, iTmpBrightness, FALSE);
            CT_SetColor (CT_COLOR_BRIGHTNESS, iTmpBrightness);

            // Contrast
            hCtl = GetDlgItem (hDlg, ID_SBCONTRAST);
            SetScrollRange ( hCtl, SB_CTL, 0, CT_MAX_CONTRAST, FALSE);
            SetScrollPos   ( hCtl, SB_CTL, iTmpContrast, TRUE);
            SetDlgItemInt (hDlg, ID_EBCONTRAST, iTmpContrast, FALSE);
            CT_SetColor (CT_COLOR_CONTRAST, iTmpContrast);

            // Red
            hCtl = GetDlgItem (hDlg, ID_SBRED);
            SetScrollRange ( hCtl, SB_CTL, 0, CT_MAX_RED, FALSE);
            SetScrollPos   ( hCtl, SB_CTL, iTmpRed, TRUE);
            SetDlgItemInt (hDlg, ID_EBRED, iTmpRed, FALSE);
            CT_SetColor (CT_COLOR_RED, iTmpRed);

            // Green
            hCtl = GetDlgItem (hDlg, ID_SBGREEN);
            SetScrollRange ( hCtl, SB_CTL, 0, CT_MAX_GREEN, FALSE);
            SetScrollPos   ( hCtl, SB_CTL, iTmpGreen, TRUE);
            SetDlgItemInt (hDlg, ID_EBGREEN, iTmpGreen, FALSE);
            CT_SetColor (CT_COLOR_GREEN, iTmpGreen);

            // Blue
            hCtl = GetDlgItem (hDlg, ID_SBBLUE);
            SetScrollRange ( hCtl, SB_CTL, 0, CT_MAX_BLUE, FALSE);
            SetScrollPos   ( hCtl, SB_CTL, iTmpBlue, TRUE);
            SetDlgItemInt (hDlg, ID_EBBLUE, iTmpBlue, FALSE);
            CT_SetColor (CT_COLOR_BLUE, iTmpBlue);
            break;


        case WM_HSCROLL:
            hCtl = HIWORD (lParam);
            wID = GetWindowWord (hCtl, GWW_ID);
            switch (wID) {
                case ID_SBSAT:
                    iTmpSat = ColorProcessScroll (hDlg, hCtl, wParam, lParam, 
                                iTmpSat, ID_EBSAT);
                    CT_SetColor (CT_COLOR_SAT, iTmpSat);
                    break;
                case ID_SBBRIGHTNESS:
                    iTmpBrightness = ColorProcessScroll (hDlg, hCtl, wParam, lParam, 
                                iTmpBrightness, ID_EBBRIGHTNESS);
                    CT_SetColor (CT_COLOR_BRIGHTNESS, iTmpBrightness);
                    break;
                case ID_SBCONTRAST:
                    iTmpContrast = ColorProcessScroll (hDlg, hCtl, wParam, lParam, 
                                iTmpContrast, ID_EBCONTRAST);
                    CT_SetColor (CT_COLOR_CONTRAST, iTmpContrast);
                    break;
                case ID_SBRED:
                    iTmpRed = ColorProcessScroll (hDlg, hCtl, wParam, lParam, 
                                iTmpRed, ID_EBRED);
                    CT_SetColor (CT_COLOR_RED, iTmpRed);
                    break;
                case ID_SBGREEN:
                    iTmpGreen = ColorProcessScroll (hDlg, hCtl, wParam, lParam, 
                                iTmpGreen, ID_EBGREEN);
                    CT_SetColor (CT_COLOR_GREEN, iTmpGreen);
                    break;
                case ID_SBBLUE:
                    iTmpBlue = ColorProcessScroll (hDlg, hCtl, wParam, lParam, 
                                iTmpBlue, ID_EBBLUE);
                    CT_SetColor (CT_COLOR_BLUE, iTmpBlue);
                    break;
            }
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
		    gwSat = iTmpSat;
		    gwContrast = iTmpContrast;
                    gwBrightness = iTmpBrightness;
                    gwRed = iTmpRed;
                    gwGreen = iTmpGreen;
                    gwBlue = iTmpBlue;
                    ConfigPutSettings();    // Save settings
                    EndDialog(hDlg, IDOK);
                    break;

                case IDCANCEL:
                    // Fix, restore all values
                    CT_SetColor (CT_COLOR_SAT, gwSat);
                    CT_SetColor (CT_COLOR_BRIGHTNESS, gwBrightness);
                    CT_SetColor (CT_COLOR_CONTRAST, gwContrast);
                    CT_SetColor (CT_COLOR_RED, gwRed);
                    CT_SetColor (CT_COLOR_GREEN, gwGreen);
                    CT_SetColor (CT_COLOR_BLUE, gwBlue);
                    EndDialog(hDlg, IDCANCEL);
                    break;

                case ID_PBDEFAULT:
	            iTmpSat = CT_DEFAULT_SAT;
	            iTmpContrast = CT_DEFAULT_CONTRAST;
	            iTmpBrightness = CT_DEFAULT_BRIGHTNESS;
                    iTmpRed = CT_DEFAULT_RED;
                    iTmpGreen = CT_DEFAULT_GREEN;
                    iTmpBlue = CT_DEFAULT_BLUE;

                    SendMessage (hDlg, WM_UPDATEDIALOG, 0, 0L);
                    break;


                case ID_EBSAT:
                    if (HIWORD(lParam) == EN_KILLFOCUS) {
                        iTmpSat = GetDlgItemInt (hDlg, ID_EBSAT, NULL, FALSE);
                        iTmpSat = LimitRange (iTmpSat, 0, CT_MAX_SAT);
                        SendMessage (hDlg, WM_UPDATEDIALOG, 0, 0L);
                    }    
                    break;

                case ID_EBBRIGHTNESS:
                    if (HIWORD(lParam) == EN_KILLFOCUS) {
                        iTmpBrightness = GetDlgItemInt (hDlg, ID_EBBRIGHTNESS, NULL, FALSE);
                        iTmpBrightness = LimitRange (iTmpBrightness, 0, CT_MAX_BRIGHTNESS);
                        SendMessage (hDlg, WM_UPDATEDIALOG, 0, 0L);
                    }    
                    break;

                case ID_EBCONTRAST:
                    if (HIWORD(lParam) == EN_KILLFOCUS) {
                        iTmpContrast = GetDlgItemInt (hDlg, ID_EBCONTRAST, NULL, FALSE);
                        iTmpContrast = LimitRange (iTmpContrast, 0, CT_MAX_CONTRAST);
                        SendMessage (hDlg, WM_UPDATEDIALOG, 0, 0L);
                    }    
                    break;

                case ID_EBRED:
                    if (HIWORD(lParam) == EN_KILLFOCUS) {
                        iTmpRed = GetDlgItemInt (hDlg, ID_EBRED, NULL, FALSE);
                        iTmpRed = LimitRange (iTmpRed, 0, CT_MAX_RED);
                        SendMessage (hDlg, WM_UPDATEDIALOG, 0, 0L);
                    }    
                    break;

                case ID_EBGREEN:
                    if (HIWORD(lParam) == EN_KILLFOCUS) {
                        iTmpGreen = GetDlgItemInt (hDlg, ID_EBGREEN, NULL, FALSE);
                        iTmpGreen = LimitRange (iTmpGreen, 0, CT_MAX_GREEN);
                        SendMessage (hDlg, WM_UPDATEDIALOG, 0, 0L);
                    }    
                    break;

                case ID_EBBLUE:
                    if (HIWORD(lParam) == EN_KILLFOCUS) {
                        iTmpBlue = GetDlgItemInt (hDlg, ID_EBBLUE, NULL, FALSE);
                        iTmpBlue = LimitRange (iTmpBlue, 0, CT_MAX_BLUE);
                        SendMessage (hDlg, WM_UPDATEDIALOG, 0, 0L);
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
