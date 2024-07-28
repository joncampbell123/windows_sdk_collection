#define PRINTDRIVER
#include "print.h"
#include "gdidefs.inc"

// Actual definition for LPDV is in ..\genlib\device.h.
// Actual definition for LPDM is in drivinit.h and ..\genlib\resource.h

typedef     LPSTR   LPDV;
typedef     LPSTR   LPDM;

#include "unidrv.h"

extern char *rgchModuleName;	// global module name


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
LPDV	    lpdv;	    // --> to the destination
short	    style;	    // Output operation
short	    count;	    // # of points
LPPOINT     lpPoints;	    // --> to a set of points
long	    lpPPen;	    // --> to physical pen
long	    lpPBrush;	    // --> to physical brush
LPDRAWMODE  lpDrawMode;     // --> to a Drawing mode
LPRECT	    lpCR;     // --> to a clipping rectange if <> 0
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

void FAR PASCAL DeviceMode(hWnd, hInst, lpDevName, lpPort)
HANDLE	hWnd;
HANDLE	hInst;
LPSTR	lpDevName;
LPSTR	lpPort;
{
    GlDeviceMode(hWnd, hInst, lpDevName, lpPort);
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

#ifndef NOENABLE
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
#endif

#ifndef NODISABLE
short far PASCAL Disable(lpdv)
LPDV lpdv;
{
    return GlDisable(lpdv);
}
#endif

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

#ifndef NODEVMODE
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
    return GlExtDeviceMode(hWnd, hInst, lpdmOut, lpDevName, lpPort, lpdmIn,
			   lpProfile, wMode);
}
#endif

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

#ifndef NODEVINSTALL
int FAR PASCAL DevInstall(hWnd, lpDevName, lpOldPort, lpNewPort)
HWND	hWnd;
LPSTR	lpDevName;
LPSTR	lpOldPort, lpNewPort;
{
    return GlDevInstall(hWnd, lpDevName, lpOldPort, lpNewPort);
}
#endif

VOID FAR PASCAL WEP(fExitWindows)
short fExitWindows;
{
}



