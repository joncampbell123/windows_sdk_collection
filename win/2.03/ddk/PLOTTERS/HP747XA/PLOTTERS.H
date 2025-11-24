/* Constants, types, externs needed for the plotters driver. */


/* The number of devices in the driver. */
#define NUMDEVICES	2


#define APAPER		    10
#define BPAPER		    11
#define A3PAPER 	    12
#define A4PAPER 	    13


#ifdef SPOOLING
#define SETSPOOLING	    14
#define RESETSPOOLING	    15
#endif

#define SLOWSPEED	    16
#define FASTSPEED	    17

#define PORTRAIT	    18
#define LANDSCAPE	    19


/* From windows.h. */
#define Abs(X)	((X) >= 0 ? (X) : (-(X)))

#define PASCAL	pascal
#define FALSE	0
#define TRUE	1
#define NULL	0
#define FAR	far
#define NEAR	near
#define VOID	void
#define REGISTER register
typedef unsigned char	BYTE;
typedef unsigned short	WORD;
typedef short  INTEGER;
typedef unsigned long  DWORD;
typedef int	  BOOL;
typedef BOOL FAR *LPBOOL;
typedef int  FAR *LPINT;
typedef char	 *PSTR;
typedef char FAR *LPSTR;
typedef char FAR * FAR *LPLPSTR;
typedef char *HANDLE;
typedef HANDLE HWND;
typedef HANDLE GLOBALHANDLE;
typedef HANDLE LOCALHANDLE;
typedef HANDLE HDC;
typedef int   (FAR PASCAL *FARPROC)();
typedef DWORD (FAR PASCAL *DFARPROC)();
typedef int   (NEAR PASCAL *NEARPROC)();
typedef DWORD (NEAR PASCAL *DNEARPROC)();


INTEGER       FAR PASCAL MulDiv(INTEGER, INTEGER, INTEGER);
BOOL	    FAR PASCAL Yield( void );

#define LockData()     LockSegment( 0xFFFF )
#define UnlockData()   UnlockSegment( 0xFFFF )

HANDLE	    FAR PASCAL LockSegment( WORD );
HANDLE	    FAR PASCAL UnlockSegment( WORD );

INTEGER 	FAR PASCAL lstrcmp( LPSTR, LPSTR );
INTEGER 	FAR PASCAL OpenPathname( LPSTR, INTEGER );
INTEGER 	FAR PASCAL DeletePathname( LPSTR );
INTEGER 	FAR PASCAL _lopen( LPSTR, INTEGER );
INTEGER 	FAR PASCAL _lclose( INTEGER );
INTEGER 	FAR PASCAL _lcreat( LPSTR, INTEGER );
INTEGER 	FAR PASCAL _ldelete( LPSTR );
INTEGER 	FAR PASCAL _ldup( INTEGER );
long	    FAR PASCAL _llseek( INTEGER, long, INTEGER );
INTEGER 	FAR PASCAL _lread( INTEGER, LPSTR, INTEGER );
INTEGER 	FAR PASCAL _lwrite( INTEGER, LPSTR, INTEGER );

/* Flags for _lopen */
#define READ	    0
#define WRITE	    1
#define READ_WRITE  2

/* MessageBox type flags */
#define MB_OK			0x00
#define MB_OKCANCEL		0x01
#define MB_ABORTRETRYIGNORE	0x02
#define MB_NOFOCUS		0x8000

INTEGER 	FAR PASCAL MessageBox(HWND, LPSTR, LPSTR, WORD);
DWORD	    FAR PASCAL SendMessage(HWND, unsigned, WORD, DWORD);

/* Dialog style bits */
#define DS_ABSALIGN   0x000000001L

/* Conventional dialog box command IDs */
#define IDOK	    1
#define IDCANCEL    2

#define WM_DEVMODECHANGE    0x001b
#define WM_INITDIALOG	    0x0110
#define WM_COMMAND	    0x0111

#define WS_GROUP	0x00020000L
#define WS_TABSTOP	0x00010000L
#define WS_POPUP	0x80000000L
#define WS_DLGFRAME	0x00400000L


INTEGER     FAR PASCAL DialogBox(HANDLE, LPSTR, HWND, FARPROC);
void	    FAR PASCAL EndDialog(HWND, INTEGER);
void	    FAR PASCAL CheckDlgButton(HWND, INTEGER, BOOL);
BOOL	    FAR PASCAL IsDlgButtonChecked(HWND, INTEGER);
void	    FAR PASCAL SetDlgItemText(HWND, INTEGER, LPSTR);
INTEGER     FAR PASCAL GetDlgItemText(HWND, INTEGER, LPSTR, INTEGER);
HWND	    FAR PASCAL SetFocus(HWND);
HWND	    FAR PASCAL GetDlgItem(HWND, INTEGER);


INTEGER 	FAR PASCAL SetEnvironment(LPSTR, LPSTR, INTEGER);
INTEGER 	FAR PASCAL GetEnvironment(LPSTR, LPSTR, INTEGER);
INTEGER 	FAR PASCAL QueryAbort(HDC, INTEGER);


#ifndef     NOGDIINCLUDES

/* Get all the GDI stuff. */
#include  "gdidefs.inc"

#endif


typedef GDIINFO     *PGDIINFO;
typedef GDIINFO     FAR *LPGDIINFO;


#ifndef BUFFERLENGTH
#define BUFFERLENGTH	512
#endif


/* A buffer. */
typedef struct {
	INTEGER 	    Length;
	INTEGER 	    Count;
	BYTE		Buffer[BUFFERLENGTH];
	} BUFFERSTRUCT;
typedef BUFFERSTRUCT	*PBUFFERSTRUCT;
typedef BUFFERSTRUCT	FAR *LPBUFFERSTRUCT;

/* The shared part of a PDEVICE. */
typedef struct {
	BYTE		Type;		/* Bitmap or device. */
	INTEGER 	DeviceNumber;	/* Our internal index for this device. */
	INTEGER 	HorzRes;
	INTEGER 	VertRes;
	HDC		hDC;		/* The DC which brought this all about. */
	INTEGER 	ChannelNumber;	/* The I/O channel number. */
					/*  -1 ==> not open. */
					/*  0  ==> open, but error on output. */
	BOOL		OutputInit;	/* TRUE if output has been initialized. */
	WORD		OutFileOffset;	/* Offset down into the PDevice to */
					/*  the output filename. */
#ifdef SPOOLING
	BOOL		Spooling;	/* Signals spooling vs direct output. */
#endif
	INTEGER 	PenSpeed;	/* Signals slow speed pens for tranparencies. */
	INTEGER 	Orientation;	/* Landscape vs portrait. */
	INTEGER 	BandNumber;	/* The number of the current band. */
	BUFFERSTRUCT	Buffer; 	/* The output buffer. */
	POINT		PageSize;	/* The physical paper size in GDI
					   device units. */
	POINT		LandPageOffset; /* The offset from the corner of the
					   medium to point (0, 0) in GDI
					   device units in landscape mode. */
	POINT		PortPageOffset; /* The offset from the corner of the
					   medium to point (0, 0) in GDI
					   device units in portrait mode. */
	} PDEVICEHEADER;
typedef PDEVICEHEADER	*PPDEVICEHEADER;
typedef PDEVICEHEADER	FAR *LPPDEVICEHEADER;


/* A color in HLS form. */
typedef struct {
	INTEGER 	Hue;
	INTEGER 	Lightness;
	INTEGER 	Saturation;
	} HLSBLOCK;
typedef HLSBLOCK    *PHLSBLOCK;
typedef HLSBLOCK    FAR *LPHLSBLOCK;


/* The various utilities. */
extern BOOL FAR PASCAL StripColon(LPSTR, LPSTR);
extern BOOL FAR PASCAL EqualNames(LPSTR, LPSTR);
extern INTEGER	FAR PASCAL FindIndex(LPSTR, LPLPSTR, INTEGER);
extern INTEGER	FAR PASCAL IntToAscii(INTEGER, LPSTR, INTEGER);
extern INTEGER	FAR PASCAL OpenChannel(LPPDEVICEHEADER, LPSTR, LPSTR);
extern INTEGER	FAR PASCAL SendString(LPPDEVICEHEADER, LPSTR, INTEGER);
extern INTEGER	FAR PASCAL FlushBuffer(LPPDEVICEHEADER);
extern INTEGER	FAR PASCAL CloseChannel(LPPDEVICEHEADER);
extern INTEGER	FAR PASCAL HPSendCommand(LPPDEVICEHEADER, LPSTR, LPINT,
				     INTEGER,  INTEGER, INTEGER);
extern INTEGER	FAR PASCAL HPChangePen(LPPDEVICEHEADER, INTEGER, LPINT);
extern INTEGER	FAR PASCAL HPSetClipRect(LPPDEVICEHEADER, LPRECT, INTEGER, INTEGER);
extern INTEGER	FAR PASCAL HPRestoreClipRect(LPPDEVICEHEADER, INTEGER, INTEGER, INTEGER);
extern INTEGER	FAR PASCAL HPInsertTextPixels(LPPDEVICEHEADER, INTEGER);
extern INTEGER	FAR PASCAL HPRotatePoints(LPPDEVICEHEADER, LPINT, LPINT, INTEGER, INTEGER, INTEGER);
extern INTEGER	FAR PASCAL RGB_to_HLS(DWORD, LPHLSBLOCK);
extern INTEGER	FAR PASCAL ColorDistance(DWORD, DWORD);
extern INTEGER	FAR PASCAL ConstrainHLS(LPHLSBLOCK);
extern INTEGER	FAR PASCAL RotateGDIInfo(PGDIINFO);
extern INTEGER	FAR PASCAL ScaleGDIInfo(PGDIINFO);
extern INTEGER	FAR PASCAL NormalizeGDIInfoPoint(long, long, PPOINT,
					     long, long, PPOINT);
extern INTEGER	FAR PASCAL PlotterMessageBox(LPPDEVICEHEADER,
					 HWND, LPSTR, LPSTR, WORD);
extern INTEGER	FAR PASCAL CheckOutputState(LPPDEVICEHEADER);


/* The stuff we need to use the spooler. */

INTEGER  FAR PASCAL OpenJob(LPSTR, LPSTR, HDC);
INTEGER  FAR PASCAL StartSpoolPage(INTEGER);
INTEGER  FAR PASCAL EndSpoolPage(INTEGER);
INTEGER  FAR PASCAL WriteSpool(INTEGER, LPSTR, INTEGER);
INTEGER  FAR PASCAL CloseJob(INTEGER);
INTEGER  FAR PASCAL WriteDialog(INTEGER, LPSTR, INTEGER);
void	 FAR PASCAL Copy(LPSTR, LPSTR, INTEGER);
INTEGER  FAR PASCAL DeleteJob(INTEGER, INTEGER);



/* The names of the devices. */
/* Note that these names must also occur in the DESCRIPTION section
   of the .DEF file. */
#ifndef ALLOCATE
extern
#endif
LPSTR Devices[NUMDEVICES]
#ifdef	ALLOCATE
		= {
		"HP 7470A",
		"HP 7475A"
		}
#endif
;


/* The device entry points. */


/* ColorInfo functions. */
extern DWORD FAR PASCAL hp7470AColorInfo();
#ifndef ALLOCATE
extern
#endif
DFARPROC ColorInfoFuncs[NUMDEVICES]
#ifdef	ALLOCATE
		= {
		hp7470AColorInfo,
		hp7470AColorInfo
		}
#endif
;


/* Control functions. */
extern INTEGER	 FAR PASCAL hp7470AControl();
#ifndef ALLOCATE
extern
#endif
FARPROC ControlFuncs[NUMDEVICES]
#ifdef	ALLOCATE
		= {
		hp7470AControl,
		hp7470AControl
		}
#endif
;


/* Disable functions. */
extern INTEGER	 FAR PASCAL hp7470ADisable();
#ifndef ALLOCATE
extern
#endif
FARPROC DisableFuncs[NUMDEVICES]
#ifdef	ALLOCATE
		= {
		hp7470ADisable,
		hp7470ADisable
		}
#endif
;


/* Enable functions. */
extern INTEGER	 FAR PASCAL hp7470AEnable();
#ifndef ALLOCATE
extern
#endif
FARPROC EnableFuncs[NUMDEVICES]
#ifdef	ALLOCATE
		= {
		hp7470AEnable,
		hp7470AEnable
		}
#endif
;


/* EnumDFonts functions. */
extern INTEGER	 FAR PASCAL hp7470AEnumDFonts();
#ifndef ALLOCATE
extern
#endif
FARPROC EnumDFontsFuncs[NUMDEVICES]
#ifdef	ALLOCATE
		= {
		hp7470AEnumDFonts,
		hp7470AEnumDFonts
		}
#endif
;


/* EnumObj functions. */
extern INTEGER	 FAR PASCAL hp7470AEnumObj();
#ifndef ALLOCATE
extern
#endif
FARPROC EnumObjFuncs[NUMDEVICES]
#ifdef	ALLOCATE
		= {
		hp7470AEnumObj,
		hp7470AEnumObj
		}
#endif
;


/* Output functions. */
extern INTEGER	 FAR PASCAL hp7470AOutput();
#ifndef ALLOCATE
extern
#endif
FARPROC OutputFuncs[NUMDEVICES]
#ifdef	ALLOCATE
		= {
		hp7470AOutput,
		hp7470AOutput
		}
#endif
;


/* Pixel functions. */
extern DWORD FAR PASCAL hp7470APixel();
#ifndef ALLOCATE
extern
#endif
DFARPROC PixelFuncs[NUMDEVICES]
#ifdef	ALLOCATE
		= {
		hp7470APixel,
		hp7470APixel
		}
#endif
;


/* RealizeObject functions. */
extern INTEGER	 FAR PASCAL hp7470ARealizeObject();
#ifndef ALLOCATE
extern
#endif
FARPROC RealizeObjectFuncs[NUMDEVICES]
#ifdef	ALLOCATE
		= {
		hp7470ARealizeObject,
		hp7470ARealizeObject
		}
#endif
;


/* StrBlt functions. */
extern DWORD FAR PASCAL hp7470AStrBlt();
#ifndef ALLOCATE
extern
#endif
DFARPROC StrBltFuncs[NUMDEVICES]
#ifdef	ALLOCATE
		= {
		hp7470AStrBlt,
		hp7470AStrBlt
		}
#endif
;


/* ScanLR functions. */
extern INTEGER	 FAR PASCAL hp7470AScanLR();
#ifndef ALLOCATE
extern
#endif
FARPROC ScanLRFuncs[NUMDEVICES]
#ifdef	ALLOCATE
		= {
		hp7470AScanLR,
		hp7470AScanLR
		}
#endif
;


/* DeviceMode functions. */
extern INTEGER	 FAR PASCAL hp7470ADeviceMode();
extern INTEGER	 FAR PASCAL hp7475ADeviceMode();
#ifndef ALLOCATE
extern
#endif
FARPROC DeviceModeFuncs[NUMDEVICES]
#ifdef	ALLOCATE
		= {
		hp7470ADeviceMode,
		hp7475ADeviceMode
		}
#endif
;


/* Setup functions. */
extern INTEGER	 FAR PASCAL hp7470ASetupEnable();
extern INTEGER	 FAR PASCAL hp7475ASetupEnable();
#ifndef ALLOCATE
extern
#endif
FARPROC SetupEnableFuncs[NUMDEVICES]
#ifdef	ALLOCATE
		= {
		hp7470ASetupEnable,
		hp7475ASetupEnable
		}
#endif
;


/* Initialization functions. */
extern INTEGER	 FAR PASCAL hp7470AInitDevice();
extern INTEGER	 FAR PASCAL hp7475AInitDevice();
#ifndef ALLOCATE
extern
#endif
FARPROC InitDeviceFuncs[NUMDEVICES]
#ifdef	ALLOCATE
		= {
		hp7470AInitDevice,
		hp7475AInitDevice
		}
#endif
;


/* Load paper message. */
#ifndef ALLOCATE
extern
#endif
PSTR	LoadPaperMess
#ifdef	ALLOCATE
		= "Please load a new sheet of paper."
#endif
;
