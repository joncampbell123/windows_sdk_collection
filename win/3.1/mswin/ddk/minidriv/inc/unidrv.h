//-----------------------------------------------------------------------------
// This file contains function declaration
//
// Created:  2/6/90
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// general typedefs
//-----------------------------------------------------------------------------

typedef short	    FAR *LPSHORT;

//-----------------------------------------------------------------------------
// Imported routines from GDI
//-----------------------------------------------------------------------------

short	FAR PASCAL MulDiv(short, short, short);

// This structure is the last parameter in GlEnable.
typedef struct
{
    short   cbSize;	    // size of this structure, contains 8 for v 1.0
    HANDLE  hMd;	    // handle to mini-driver
    short (FAR PASCAL *fnOEMDump)(LPDV, LPPOINT, WORD);
} CUSTOMDATA, FAR * LPCUSTOMDATA;

// flags defined for the last parameter (WORD) of DUMP call-back
#define CB_LANDSCAPE	    0x0001	// indicate the current orientation

//-----------------------------------------------------------------------------
// Exported routines to mini driver
//-----------------------------------------------------------------------------

short	FAR PASCAL GlControl(LPDV, short, LPSTR, LPSTR);
short	FAR PASCAL GlBitBlt(LPDV, short, short, LPBITMAP, short, short,
			    short, short, long, long, LPDRAWMODE);
short	FAR PASCAL GlPixel(LPDV, short, short, long, long);
short	FAR PASCAL GlOutput(LPDV, short, short, LPPOINT, long, long,
			    LPDRAWMODE, LPRECT);
long	FAR PASCAL GlStrBlt(LPDV, short, short, LPRECT, LPSTR, short,
			    LPFONTINFO, LPDRAWMODE, LPTEXTXFORM);
short	FAR PASCAL GlScanLR(LPDV, short, short, long, short);
short	FAR PASCAL GlEnumObj(LPDV, short, long, long);
long	FAR PASCAL GlColorInfo(LPDV, long, long);
short	FAR PASCAL GlDeviceMode(HANDLE, HANDLE, LPSTR, LPSTR);
short	FAR PASCAL GlRealizeObject(LPDV, short, LPSTR, LPSTR, LPTEXTXFORM);
short	FAR PASCAL GlEnumDFonts(LPDV, LPSTR, FARPROC, long);
short	FAR PASCAL GlEnable(LPDV, short, LPSTR, LPSTR, LPDM, LPCUSTOMDATA);
short	FAR PASCAL GlDisable(LPDV);
long	FAR PASCAL GlExtTextOut(LPDV, short, short, LPRECT, LPSTR, short,
			    LPFONTINFO, LPDRAWMODE, LPTEXTXFORM, LPSHORT,
			    LPRECT, WORD);
short	FAR PASCAL GlGetCharWidth(LPDV, LPSHORT, BYTE, BYTE, LPFONTINFO,
			    LPDRAWMODE, LPTEXTXFORM);
short	FAR PASCAL GlDeviceBitmapBits(LPDV, WORD, WORD, WORD, LPSTR,
			    LPBITMAPINFOHEADER, LPDRAWMODE, LPSTR);
short	FAR PASCAL GlCreateDIBitmap(VOID);
short	FAR PASCAL GlSetDIBitsToDevice(LPDV, WORD, WORD, WORD, WORD, LPRECT,
			    LPDRAWMODE, LPSTR, LPBITMAPINFOHEADER, LPSTR);
int FAR PASCAL GlStretchDIB(LPDV, WORD, WORD, WORD, WORD, short,
		WORD, WORD, WORD, WORD, LPSTR, LPBITMAPINFOHEADER,
		LPSTR, DWORD, LPSTR, LPDRAWMODE, LPRECT);
short	FAR PASCAL GlExtDeviceMode(HANDLE, HANDLE, LPDM, LPSTR, LPSTR, LPDM,
			    LPSTR, WORD);
DWORD	FAR PASCAL GlDeviceCapabilities(LPSTR, LPSTR, WORD, LPSTR, LPDM, HANDLE);
LONG	FAR PASCAL GlQueryDeviceNames(HANDLE, LPSTR);
LONG	FAR PASCAL GlAdvancedSetUpDialog(HWND, HANDLE, LPDM, LPDM);

int	FAR PASCAL GlDevInstall(HWND, LPSTR, LPSTR, LPSTR);
int	FAR PASCAL WriteSpoolBuf(LPDV, LPSTR, short);

