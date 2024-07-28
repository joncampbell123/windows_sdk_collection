//-----------------------------------------------------------------------------
// This files contains the module name for this mini driver.  Each mini driver
// must have a unique module name.  The module name is used to obtain the
// module handle of this Mini Driver.  The module handle is used by the
// generic library to load in tables from the Mini Driver.
// It also contains Install() for upgrading 3.0 driver to 3.1.
//
//
//-----------------------------------------------------------------------------


char *rgchModuleName = "HPPCL";

#define PRINTDRIVER
#include "print.h"
#include "gdidefs.inc"
#define DELETE_OLD

HDC   FAR PASCAL CreateIC(LPCSTR, LPCSTR, LPCSTR, CONST VOID FAR*);
BOOL  FAR PASCAL DeleteDC(HDC);

// Actual definition for LPDV is in ..\genlib\device.h.
// Actual definition for LPDM is in print.h and ..\genlib\resource.h

typedef     LPSTR   LPDV;
typedef     LPSTR   LPDM;

short FAR PASCAL MakeAppName(LPSTR, LPSTR, short);
short FAR PASCAL itoa(LPSTR, short);
short FAR PASCAL ExtDeviceMode(HANDLE, HANDLE, LPDM, LPSTR, LPSTR, LPDM, LPSTR, WORD);

#include "unidrv.h"

extern char *rgchModuleName;	// global module name

#define SOFT_FONT_THRES 25	    // build font summary, if over this limit
#define MAX_CART_INDEX	33
#define MAX_OLD_CART	24
#define MAX_NUM_MODELS	24
#define MAX_MODEL_NAME	29
typedef struct
    {
    char szModel[MAX_MODEL_NAME];
    int  rgIndexLimit[2];
    char szPrtCaps[7];	    // keep as a string instead of integer to avoid
			    // conversion because "itoa" doesn't work here.
    } MODELMAP, FAR * LPMODELMAP;

// map old HPPCL's cartindex to unidrv's FONTCART index for newer cartridges.
// This mapping table is created based on the old HPPCL .rc file.
int rgNewCartMap[9] = {8, 7, 2, 3, 5, 6, 1, 4, 0};

//--------------------------*MakeAppName*---------------------------------------
// Action:  compose the <printer,port> name for reading the profile data
//	Return the length of the actual application name. Return -1 if fails.
//
//------------------------------------------------------------------------------

short FAR PASCAL MakeAppName(lpAppName, lpPortName, max)
LPSTR	lpPortName, lpAppName;
short	max;	   // max length for 'lpAppName'.
{
    short   length, count;
    LPSTR   lpTmp;
    LPSTR   lpLastColon = NULL;

    length = lstrlen(lpAppName);

    if (!lpPortName)
	return length;

    if (length == 0 || length > max - lstrlen(lpPortName))
	return -1;

    // insert the comma
    lpAppName[length++] = ',';

    // append the port name but do not want the last ':', if any.
    for (lpTmp = lpPortName ; *lpTmp; lpTmp++)
	if (*lpTmp == ':')
	    lpLastColon = lpTmp;
    if (lpLastColon && lpLastColon == lpTmp - 1)
	count = lpLastColon - lpPortName;
    else
	count = lpTmp - lpPortName;

    //Copy((LPSTR)&lpAppName[length], lpPortName, count);
    lpPortName[count] = '\0';
    lstrcpy((LPSTR)&lpAppName[length], lpPortName);
    length += count;

    return length;
}

//------------------------------------------------------------------------------
// Function: itoa
//
// Action:  This function converts the given integer into an ASCII string.
//
// return:  The length of the string.
//-----------------------------------------------------------------------------

short FAR PASCAL itoa(buf, n)
LPSTR buf;
short n;
{
    short   fNeg;
    short   i, j;

    if (fNeg = (n < 0))
	n = -n;

    for (i = 0; n; i++)
	{
	buf[i] = (char)(n % 10 + '0');
	n /= 10;
	}

    // n was zero
    if (i == 0)
	buf[i++] = '0';

    if (fNeg)
	buf[i++] = '-';

    for (j = 0; j < i / 2; j++)
	{
	short tmp;

	tmp = buf[j];
	buf[j] = buf[i - j - 1];
	buf[i - j - 1] = (char)tmp;
	}

    buf[i] = 0;

    return i;
}

//-------------------------*DevInstall*---------------------------------------
// Action: De-install, upgrade or install a device.
//
//----------------------------------------------------------------------------

int FAR PASCAL DevInstall(hWnd, lpDevName, lpOldPort, lpNewPort)
HWND	hWnd;
LPSTR	lpDevName;
LPSTR	lpOldPort, lpNewPort;
{
    char szOldSec[64];
    char szNewSec[64];

    if (!lpDevName)
	return -1;

    if (!lpOldPort)
	{
	char szBuf[32];
	int  tmp;
	int  i, index;
	HANDLE	hMd;
	HANDLE	hResMap;
	LPMODELMAP  lpModelMap;

	if (!lpNewPort)
	    return 0;

	// install a device for the first time. Convert old HPPCL settings,
	// which are still under [<driver>,<port>], into equivalent new
	// UNIDRV settings under [<device>,<port>], if applicable.
	// All soft fonts are left under the section [<driver>,<port>].
	lstrcpy(szOldSec, "HPPCL");
	MakeAppName((LPSTR)szOldSec, lpNewPort, sizeof(szOldSec));

	// if old section exists at all
	if (!GetProfileString(szOldSec, NULL, NULL, szBuf, sizeof(szBuf)))
	    return 1;

	// make sure the old device settings are for this device.
	// If not, there is nothing to do here. Simply return 1.
	tmp = GetProfileInt(szOldSec, "prtindex", 0);
	hMd = GetModuleHandle((LPSTR)rgchModuleName);
	hResMap = LoadResource(hMd,
			  FindResource(hMd, MAKEINTRESOURCE(1), RT_RCDATA));
	lpModelMap = (LPMODELMAP)LockResource(hResMap);
	for (i = 0; i < MAX_NUM_MODELS; i++)
	    if (!lstrcmp(lpDevName, (LPSTR)lpModelMap[i].szModel))
		{
		if ((tmp < lpModelMap[i].rgIndexLimit[0]) ||
		    (tmp > lpModelMap[i].rgIndexLimit[1]) )
		    i = MAX_NUM_MODELS;        // not this model. No conversion.
		break;
		}

	UnlockResource(hResMap);
	FreeResource(hResMap);

	if (i >= MAX_NUM_MODELS)
	    // this model is not even listed in the old HPPCL driver.
	    return 1;

	if (GetProfileInt(szOldSec, "winver", 0) == 310)
	    return 1;

	WriteProfileString(szOldSec, "winver", "310");
#ifdef DELETE_OLD
	WriteProfileString(szOldSec, "prtindex", NULL);
#endif

	lstrcpy(szNewSec, lpDevName);
	MakeAppName((LPSTR)szNewSec, lpNewPort, sizeof(szNewSec));

	// convertable device settings include: "copies", "duplex", "orient",
	// "paper", "prtresfac", "tray", and cartidges.

	if (GetProfileString(szOldSec, "orient", "", szBuf, sizeof(szBuf)) > 0)
	    {
	    WriteProfileString(szNewSec, "Orientation", szBuf);
#ifdef DELETE_OLD
	    WriteProfileString(szOldSec, "orient", NULL);
#endif
	    }
	if (GetProfileString(szOldSec, "paper", "", szBuf, sizeof(szBuf)) > 0)
	    {
	    WriteProfileString(szNewSec, "Paper Size", szBuf);
#ifdef DELETE_OLD
	    WriteProfileString(szOldSec, "paper", NULL);
#endif
	    }

	// default to 2 if cannot find it
	tmp = GetProfileInt(szOldSec, "prtresfac", 2);

	if (tmp == 1)
	    WriteProfileString(szNewSec, "Print Quality", "150");
	else if (tmp == 2)
	    WriteProfileString(szNewSec, "Print Quality", "75");

#ifdef DELETE_OLD
	WriteProfileString(szOldSec, "prtresfac", NULL);
#endif

	if (GetProfileString(szOldSec, "tray", "", szBuf, sizeof(szBuf)) > 0)
	    {
	    WriteProfileString(szNewSec, "Default Source", szBuf);
#ifdef DELETE_OLD
	    WriteProfileString(szOldSec, "tray", NULL);
#endif
	    }

	// try to convert the cartridge information.

	if ((tmp = GetProfileInt(szOldSec, "numcart", 0)) == 0)
	    tmp = 1;

	// this is executed at least once
	{
	char szOldCartKey[16];
	char szNewCartKey[16];
	char nCart = 0;

	lstrcpy(szOldCartKey, "cartindex");

	for (i = 0; i < tmp; i++)
	    {
	    if (i > 0)
		itoa((LPSTR)&szOldCartKey[9], i);
	    // compose cartridge keyname under UNIDRV.
	    lstrcpy(szNewCartKey, "Cartridge ");
	    itoa((LPSTR)&szNewCartKey[10], i + 1);

	    if ((index = GetProfileInt(szOldSec, szOldCartKey, 0)) > 0)
		{
		WriteProfileString(szOldSec, szOldCartKey, NULL);
		nCart++;
		if (index <= MAX_OLD_CART)
		    {
		    itoa((LPSTR)szBuf, index + 8);
		    WriteProfileString(szNewSec, szNewCartKey, szBuf);
		    }
		else if (index <= MAX_CART_INDEX)
		    {
		    itoa((LPSTR)szBuf, rgNewCartMap[index - MAX_OLD_CART - 1]);
		    WriteProfileString(szNewSec, szNewCartKey, szBuf);
		    }
		else
		    {
		    // external cartridges. Simply copy the id over.
		    itoa((LPSTR)szBuf, index);
		    WriteProfileString(szNewSec, szNewCartKey, szBuf);
		    }
		}
	    }

	// integer to ASCII string conversion.
	itoa((LPSTR)szBuf, nCart);
	WriteProfileString(szNewSec, "Number of Cartridges", szBuf);
	}

	// delete the old font summary file
	WriteProfileString(szOldSec, "fsvers", NULL);
	if (GetProfileString(szOldSec, "FontSummary", "", szBuf, sizeof(szBuf)) > 0)
	    {
	    int hFS;

	    // truncate the old font summary file to zero size.
	    if ((hFS = _lcreat(szBuf, 0)) >= 0)
		_lclose(hFS);
	    WriteProfileString(szOldSec, "FontSummary", NULL);
	    }
	// create UNIDRV's font summary file, if there are many soft fonts.
	if (GetProfileInt(szOldSec, "SoftFonts", 0) > SOFT_FONT_THRES)
	    {
	    HDC hIC;

       //   if (hIC = CreateIC("HPPCL", lpDevName, "LPT1:", NULL))
	    // LinS 12/3/91, why lpt1?	should be the current port
	    if (hIC = CreateIC("HPPCL", lpDevName, lpNewPort, NULL))
		DeleteDC(hIC);
	    }
	return 1;
	}
    else
	{
	// move device settings from the old port to the new port, or
	// de-install a device, i.e. remove its device setttings in order
	// to compress the profile.

	// First, check if there is any  soft font installed under the
	// old port. If so, warn the user to copy them over.
	lstrcpy(szOldSec, "HPPCL");
	MakeAppName((LPSTR)szOldSec, lpOldPort, sizeof(szOldSec));
	if (GetProfileInt(szOldSec, "SoftFonts", 0) > 0 && lpNewPort)
	    MessageBox(0,
	       "If soft fonts and/or external font catridges are installed for the printer, you need to copy them to the new port via the Fonts button in the printer Setup dialog box.",
	       lpOldPort, MB_OK);

	return GlDevInstall(hWnd, lpDevName, lpOldPort, lpNewPort);
	}

}


// the following 3 definitions MUST be compatible with the
// HPPCL font installer
#define CLASS_LASERJET	    0
#define CLASS_DESKJET	    1
#define CLASS_DESKJET_PLUS  2

//---------------------------*InstallExtFonts*---------------------------------
// Action: call the specific font installer to add/delete/modify soft fonts
//	    and/or external cartridges.
//
// Parameters:
//	HWND	hWnd;		handle to the parent windows.
//	LPSTR	lpDeviceName;	long pointer to the printer name.
//	LPSTR	lpPortName;	long pointer to the associated port name.
//	BOOL	bSoftFonts;	flag if supporting soft fonts or not.
//
//  Return Value:
//	> 0   :  if the font information has changed;
//	== 0  :  if nothing has changed;
//	== -1 :  if intending to use the universal font installer
//		 (not available now).
//-------------------------------------------------------------------------

int FAR PASCAL InstallExtFonts(hWnd, lpDeviceName, lpPortName, bSoftFonts)
HWND	hWnd;
LPSTR	lpDeviceName;
LPSTR	lpPortName;
BOOL	bSoftFonts;
{
    int     fsVers;
    HANDLE  hFIlib;
    // declare far ptr to InstallSoftFont()
    int (FAR * PASCAL lpFIns)(HWND,LPSTR,LPSTR,BOOL,int,int);

    if ((hFIlib = LoadLibrary((LPSTR)"FINSTALL.DLL")) < 32 ||
	!(lpFIns = GetProcAddress(hFIlib,"InstallSoftFont")))
	{
	if (hFIlib >= 32)
	    FreeLibrary(hFIlib);
#ifdef DEBUG
	MessageBox(0,
	    "Can't load FINSTAL.DLL or can't get InstallSoftFont",
	    NULL, MB_OK);
#endif
	return TRUE;
	}

    // FINSTALL.DLL was loaded properly. Now call InstallSoftFont().
    // We choose to ignore the returned "fvers". No use of it.
    fsVers = (*lpFIns)(hWnd, rgchModuleName, lpPortName,
		(GetKeyState(VK_SHIFT) < 0 && GetKeyState(VK_CONTROL) < 0),
		1,	  // dummy value for "fvers".
		bSoftFonts ? CLASS_LASERJET : 256
		);
    FreeLibrary(hFIlib);
    return fsVers;
}


short FAR PASCAL ExtDeviceMode(hWnd, hInst, lpdmOut, lpDevName, lpPort,
			      lpdmIn, lpProfile, wMode)
HANDLE	hWnd;		// parent for DM_PROMPT dialog box
HANDLE	hInst;		// handle from LoadLibrary()
LPDM	lpdmOut;	// output DEVMODE for DM_COPY
LPSTR	lpDevName;	// device name
LPSTR	lpPort; 	// port name
LPDM	lpdmIn; 	// input DEVMODE for DM_MODIFY
LPSTR	lpProfile;	// alternate .INI file
WORD	wMode;		// operation(s) to carry out
{
    char szOldSec[64];
    int  i;
    HANDLE  hMd;
    HANDLE  hResMap;
    LPMODELMAP	lpModelMap;

    if (wMode & DM_UPDATE)
	{
	// write out PRTCAPS under [HPPCL,port] section in "win.ini"
	// in order to be backward-compatible with existing font
	// packages. Note that if there are multiple printers connected
	// to the same port, the PRTCAPS under [HPPCL,port] will be that
	// of the last printer updated.

	lstrcpy(szOldSec, "HPPCL");
	MakeAppName((LPSTR)szOldSec, lpPort, sizeof(szOldSec));

	hMd = GetModuleHandle((LPSTR)rgchModuleName);
	hResMap = LoadResource(hMd,
			  FindResource(hMd, MAKEINTRESOURCE(1), RT_RCDATA));
	lpModelMap = (LPMODELMAP)LockResource(hResMap);
	for (i = 0; i < MAX_NUM_MODELS; i++)
	    if (!lstrcmp(lpDevName, (LPSTR)lpModelMap[i].szModel))
		{
		WriteProfileString((LPSTR)szOldSec, "prtcaps",
				   (LPSTR)lpModelMap[i].szPrtCaps);
		WriteProfileString(szOldSec, "winver", "310");
		break;
		}
	UnlockResource(hResMap);
	FreeResource(hResMap);
	}
    return GlExtDeviceMode(hWnd, hInst, lpdmOut, lpDevName, lpPort, lpdmIn,
			   lpProfile, wMode);
}

#if 0

int FAR PASCAL Control(lpdv, function, lpInData, lpOutData)
LPDV	lpdv;
short   function;
LPSTR   lpInData;
LPSTR	lpOutData;
{
    return GlControl(lpdv, function, lpInData, lpOutData);
}

short far PASCAL DevBitBlt(lpdv, DstxOrg, DstyOrg, lpSrcDev, SrcxOrg, SrcyOrg, xExt,
		  yExt, lRop, lpBrush, lpDrawmode)
LPDV	    lpdv;	    // --> to destination bitmap descriptor
short	    DstxOrg;	    // Destination origin - x coordinate
short	    DstyOrg;	    // Destination origin - y coordinate
LPBITMAP    lpSrcDev;	    // --> to source bitmap descriptor
short	    SrcxOrg;	    // Source origin - x coordinate
short	    SrcyOrg;	    // Source origin - y coordinate
short	    xExt;	    // x extent of the BLT
short	    yExt;	    // x extent of the BLT
long	    lRop;	    // Raster operation descriptor
long	    lpBrush;	    // --> to a physical brush (pattern)
LPDRAWMODE  lpDrawmode;
{
    return GlBitBlt(lpdv, DstxOrg, DstyOrg, lpSrcDev, SrcxOrg, SrcyOrg,
		    xExt, yExt, lRop, lpBrush, lpDrawmode);
}

short far PASCAL Pixel(lpdv, x, y, Color, lpDrawMode)
LPDV	lpdv;
short   x;
short   y;
long    Color;
long    lpDrawMode;
{
    return GlPixel(lpdv, x, y, Color, lpDrawMode);
}

short far PASCAL Output(lpdv, style, count, lpPoints, lpPPen, lpPBrush, lpDrawMode, lpCR)
LPDV	lpdv;		// --> to the destination
short	style;		// Output operation
short	count;		// # of points
LPPOINT lpPoints;	// --> to a set of points
long	lpPPen; 	// --> to physical pen
long	lpPBrush;	// --> to physical brush
long	lpDrawMode;	// --> to a Drawing mode
LPRECT	lpCR;	  // --> to a clipping rectange if <> 0
{
    return GlOutput(lpdv, style, count, lpPoints, lpPPen, lpPBrush, lpDrawMode, lpCR);
}

long far PASCAL StrBlt(lpdv, x, y, lpCR, lpStr, count, lpFont, lpDrawMode, lpXform)
LPDV	    lpdv;
short	    x;
short	    y;
LPRECT	    lpCR;
LPSTR	    lpStr;
short	    count;
LPFONTINFO  lpFont;
LPDRAWMODE  lpDrawMode; 	  // includes background mode and bkColor
LPTEXTXFORM lpXform;
{
    return GlStrBlt(lpdv, x, y, lpCR, lpStr, count, lpFont, lpDrawMode, lpXform);
}

short far PASCAL ScanLR(lpdv, x, y, Color, DirStyle)
LPDV	lpdv;
short	x;
short	y;
long	Color;
short   DirStyle;
{
    return GlScanLR(lpdv, x, y, Color, DirStyle);
}

short far PASCAL EnumObj(lpdv, style, lpCallbackFunc, lpClientData)
LPDV	lpdv;
short   style;
long    lpCallbackFunc;
long    lpClientData;
{
    return GlEnumObj(lpdv, style, lpCallbackFunc, lpClientData);
}

long far PASCAL ColorInfo(lpdv, ColorIn, lpPhysBits)
LPDV	lpdv;
long    ColorIn;
long    lpPhysBits;
{

    return GlColorInfo(lpdv, ColorIn, lpPhysBits);
}

short far PASCAL RealizeObject(lpdv, Style, lpInObj, lpOutObj, lpTextXForm)
LPDV	    lpdv;
short	    Style;
LPSTR	    lpInObj;
LPSTR	    lpOutObj;
LPTEXTXFORM lpTextXForm;
{
    return GlRealizeObject(lpdv, Style, lpInObj, lpOutObj, lpTextXForm);
}

short far PASCAL EnumDFonts(lpdv, lpFaceName, lpCallbackFunc, lpClientData)
LPDV	lpdv;
LPSTR   lpFaceName;
FARPROC lpCallbackFunc;
long    lpClientData;
{
    return GlEnumDFonts(lpdv, lpFaceName, lpCallbackFunc, lpClientData);
}

short FAR PASCAL Enable(lpdv, style, lpModel, lpPort, lpStuff)
LPDV	lpdv;
short	style;
LPSTR	lpModel;
LPSTR	lpPort;
LPDM	lpStuff;
{
    CUSTOMDATA	cd;

    cd.cbSize = sizeof(CUSTOMDATA);
    cd.hMd = GetModuleHandle((LPSTR)rgchModuleName);
    cd.fnOEMDump = NULL;
    return GlEnable(lpdv, style, lpModel, lpPort, lpStuff, &cd);
}

short far PASCAL Disable(lpdv)
LPDV lpdv;
{
    return GlDisable(lpdv);
}

long far PASCAL DevExtTextOut(lpdv, x, y, lpCR, lpStr, count, lpFont,
			lpDrawMode, lpXform, lpWidths, lpOpaqRect, options)
LPDV	    lpdv;
short	    x;
short	    y;
LPRECT	    lpCR;
LPSTR	    lpStr;
short	    count;
LPFONTINFO  lpFont;
LPDRAWMODE  lpDrawMode;
LPTEXTXFORM lpXform;
LPSHORT     lpWidths;
LPRECT	    lpOpaqRect;
WORD	    options;
{
    return(GlExtTextOut(lpdv, x, y, lpCR, lpStr, count, lpFont,
			lpDrawMode, lpXform, lpWidths, lpOpaqRect, options));
}

short far PASCAL DevGetCharWidth(lpdv, lpBuf, chFirst, chLast, lpFont, lpDrawMode,
			lpXForm)
LPDV	    lpdv;
LPSHORT     lpBuf;
BYTE	    chFirst;
BYTE	    chLast;
LPFONTINFO  lpFont;
LPDRAWMODE  lpDrawMode;
LPTEXTXFORM lpXForm;
{
    return(GlGetCharWidth(lpdv, lpBuf, chFirst, chLast, lpFont,lpDrawMode,
			  lpXForm));
}

short FAR PASCAL DeviceBitmap(lpdv, command, lpBitMap, lpStr)
LPDV	lpdv;
short	command;
LPSTR	lpBitMap;
LPSTR	lpStr;
{
    return 0;
}

short FAR PASCAL FastBorder(lpRect, width, depth, lRop, lpdv, lpPBrush,
					  lpDrawmode, lpCR)
LPRECT	lpRect;
short	width;
short	depth;
long	lRop;
LPDV	lpdv;
long	lpPBrush;
long	lpDrawmode;
LPRECT	lpCR;
{
    return 0;
}

short FAR PASCAL SetAttribute(lpdv, statenum, index, attribute)
LPDV	lpdv;
short	statenum;
short	index;
short	attribute;
{
    return 0;
}

void FAR PASCAL DeviceMode(hWnd, hInst, lpDevName, lpPort)
HANDLE	hWnd;
HANDLE	hInst;
LPSTR	lpDevName;
LPSTR	lpPort;
{
    // call ExtDeviceMode in order to share post-processing code.
    ExtDeviceMode(hWnd, hInst, (LPDM)NULL, lpDevName, lpPort,
		   (LPDM)NULL, (LPSTR)NULL, DM_PROMPT | DM_UPDATE);
}

DWORD FAR PASCAL DeviceCapabilities(lpDevName, lpPort, wIndex, lpOutput, lpdm)
LPSTR	lpDevName;
LPSTR	lpPort;
WORD	wIndex;
LPSTR	lpOutput;
LPDM	lpdm;
{
    return(GlDeviceCapabilities(lpDevName, lpPort, wIndex, lpOutput, lpdm,
		    GetModuleHandle((LPSTR)rgchModuleName)));
}


LONG FAR PASCAL AdvancedSetUpDialog(hWnd, hInstMiniDrv, lpdmIn, lpdmOut)
HWND	hWnd;
HANDLE	hInstMiniDrv;	// handle of the driver module
LPDM	lpdmIn; 	// initial device settings
LPDM	lpdmOut;	// final device settings
{
    return(GlAdvancedSetUpDialog(hWnd, hInstMiniDrv, lpdmIn, lpdmOut));
}


short FAR PASCAL DeviceBitmapBits(lpdv, style, iStart, sScans, lpDIBits,
				    lpDIBHdr, lpDrawMode, lpConvInfo)
LPDV		    lpdv;
WORD		    style;
WORD		    iStart;
WORD		    sScans;
LPSTR		    lpDIBits;
LPBITMAPINFOHEADER  lpDIBHdr;
LPDRAWMODE	    lpDrawMode;
LPSTR		    lpConvInfo;
{
    return(GlDeviceBitmapBits(lpdv, style, iStart, sScans, lpDIBits,
			 lpDIBHdr, lpDrawMode, lpConvInfo));
}


short FAR PASCAL CreateDIBitmap()
{
    return(GlCreateDIBitmap());
}


short FAR PASCAL SetDIBitsToDevice(lpdv, DstXOrg, DstYOrg, StartScan, NumScans,
			 lpCR, lpDrawMode, lpDIBits, lpDIBHdr, lpConvInfo)
LPDV		    lpdv;
WORD		    DstXOrg;
WORD		    DstYOrg;
WORD		    StartScan;
WORD		    NumScans;
LPRECT		    lpCR;
LPDRAWMODE	    lpDrawMode;
LPSTR		    lpDIBits;
LPBITMAPINFOHEADER  lpDIBHdr;
LPSTR		    lpConvInfo;
{
    return(GlSetDIBitsToDevice(lpdv, DstXOrg, DstYOrg, StartScan, NumScans,
			 lpCR, lpDrawMode, lpDIBits, lpDIBHdr, lpConvInfo));
}


int FAR PASCAL StretchDIB(lpdv, wMode, DstX, DstY, DstXE, DstYE,
		SrcX, SrcY, SrcXE, SrcYE, lpBits, lpDIBHdr,
		lpConvInfo, dwRop, lpbr, lpdm, lpClip)
LPDV		    lpdv;
WORD		    wMode;
WORD		    DstX, DstY, DstXE;
short		    DstYE;
WORD		    SrcX, SrcY, SrcXE, SrcYE;
LPSTR		    lpBits;		/* pointer to DIBitmap Bits */
LPBITMAPINFOHEADER  lpDIBHdr;		/* pointer to DIBitmap info Block */
LPSTR		    lpConvInfo; 	/* not used */
DWORD		    dwRop;
LPSTR		    lpbr;
LPDRAWMODE	    lpdm;
LPRECT		    lpClip;
{
    return(GlStretchDIB(lpdv, wMode, DstX, DstY, DstXE, DstYE,
		SrcX, SrcY, SrcXE, SrcYE, lpBits, lpDIBHdr,
		lpConvInfo, dwRop, lpbr, lpdm, lpClip));
}

long FAR PASCAL QueryDeviceNames(lprgDeviceNames)
LPSTR	lprgDeviceNames;
{
    return GlQueryDeviceNames(GetModuleHandle(rgchModuleName),
			      lprgDeviceNames);
}

VOID FAR PASCAL WEP(fExitWindows)
short fExitWindows;
{
}

#endif
