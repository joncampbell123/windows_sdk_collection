/* Declarations common to the hp747xA plotter drivers. */

#define RESOLUTION	    10

#define STEPSPERMM	    40


#ifdef	STYLED_LINES
#define NUMPENSTYLES	    6
#else
#define NUMPENSTYLES	    1
#endif


#define FONTWEIGHT	    FW_NORMAL
#define CHARSET 	    0

/* CHARSET7 is used to get the foreign characters reqired for the hp7475a */


#define CHARSET0	    '0'
#define CHARSET1	    '1'
#define CHARSET2	    '2'
#define CHARSET3	    '3'
#define CHARSET4	    '4'
#define CHARSET7	    '7'
#define BACKSP		    '\x08'

#define TRANS_MIN   ((BYTE) 0xA0)
#define ANSI_MAX   ((BYTE) 0x7e)

#define FIRSTCHAR	    32
#define LASTCHAR	    255
#define DEFAULTCHAR	    ('.'-FIRSTCHAR)
#define BREAKCHAR	    (32-FIRSTCHAR)


#define SETCURPOS(xc, yc) \
	{ \
	(lpPDevice->CurPosX) = (xc); \
	(lpPDevice->CurPosY) = (yc); \
	}

#define LIFTPEN() \
	{ \
	if (lpPDevice->PenDown) { \
		SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"PU;", 3); \
		(lpPDevice->PenDown) = FALSE; \
		} \
	}

#define DROPPEN() \
	{ \
	if (!(lpPDevice->PenDown)) { \
		SendString((LPPDEVICEHEADER)lpPDevice, (LPSTR)"PD;", 3); \
		(lpPDevice->PenDown) = TRUE; \
		} \
	}


#define BANDING /* ????????????????????????????????????????????????????? */

#ifdef	BANDING
#define WRONGBAND(lpPDev, Color)    (lpPDev->Header.BandNumber != (Color))
#endif


typedef struct {
	PDEVICEHEADER	Header;     /* This MUST be the first item in the structure. */
	int		NumColors;
	int		CurrentPen;
	BOOL		PenDown;
	BOOL		Clipped;
	int		CurPosX;
	int		CurPosY;
	int		FontHeight;
	int		FontWidth;
	int		FontItalic;
	int		HeightNumerator;
	int		HeightDenominator;
	int		WidthNumerator;
	int		WidthDenominator;
	DWORD		Colors[NUMPENCOLORS];
	char		OutputFile[1];	/* This MUST be the last item in the structure. */
	} PDEVICE;
typedef PDEVICE     *PPDEVICE;
typedef PDEVICE     FAR *LPPDEVICE;
