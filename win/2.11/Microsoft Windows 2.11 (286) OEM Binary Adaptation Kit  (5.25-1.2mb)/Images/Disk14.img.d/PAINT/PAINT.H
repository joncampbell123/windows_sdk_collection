/*
 *	(c) 1984 Microsoft Corp.
 *
 *	Microsoft Paint constants and types
 *
 *	Modification history:
 *		6/19/84   dhm	Created
 *		6/26/84   dhm	Moved constants from palette.h
 *		6/27/84   dhm	Added selection structures
 *		7/31/84   dhm	Went to single window format
 *		10/23/84  dhm	Changed mouse input to be above 256
 *
 *
 *
 *
 *
 *
 */

#define NOCOMM
#define NOKANJI
#define NOSOUND
#define NOWH
#define NOWINOFFSETS

#include "windows.h"

#define SPLINELIM

/*
 *	menu structure constants
 */
#define MENUBAS 		30
#define MENUMAX 		7
#define MENUENT 		12

/*  9/10/86 reduce to 16 */
#define FONTARRAYSIZE		16	    /* 25 */

#define TRAPMSG 		32

#define MOK			16
#define MCANCEL 		17
#define MNONE			18

 /*
  *	WARNING: since the menu constants are also used by rc,
  *	they cannot contain expressions.  Therefore, all expressions
  *	are pre-evaluated.  The formulae for the menu values is
  *	popup_value = MENUBAS + popup_index * MENUENT
  *	For each menu item in a popup, the indices increase sequentially.
  *	The values for the menuitems need to be updated if the menus change,
  *	or if MENUBAS or MENUENT change.
  */


/*
 *	file commands
 */
#define MFILE		(MENUBAS + 0 * MENUENT) /* == 30 */
#define FNEW		30
#define FOPEN		31
#define FSAVE		32
#define FSAVEAS 	33
#define FPRINT		34
#define FEXIT           35
#define FABOUT          36


/*
 *	edit commands
 */
#define MEDIT		(MENUBAS + 1 * MENUENT) /* == 42 */
#define EUNDO		42
#define EERASE		43
#define ECUT		44
#define ECOPY		45
#define EPASTE		46
#define ECLEAR		47
#define EINVERT 	48
#define ETRACE		49
#define FLIPHOR 	50
#define FLIPVER 	51

/*
 *	font and style commands
 *	Note that MFONT and MSIZE must each be less than 128
 */
#define MFONT		54 /* = (MENUBAS + 2 * MENUENT) */

#define MSIZE		66 /* = (MENUBAS + 3 * MENUENT) */

#define MSTYLE		(MENUBAS + 4 * MENUENT) /* == 78 */
#define STYLEP		78
#define STYLEB		79
#define STYLEI		80
#define STYLEU		81
#define STYLEO		82
#define STYLES		83
#define ALIGNL		84
#define ALIGNC		85
#define ALIGNR		86
#define SOPAQUE 	87
#define STRANSP 	88



/*
 *	options commands
 */
#define MPALETTE	(MENUBAS + 5 * MENUENT) /* == 90 */
#define PALTPATS	90
#define PALTSIZE	91
#define PALTSHAP	92
#define PALTTOOL	93




/*
 *	options commands  (note gap after grids and zoom)
 */
#define MOPTIONS	(MENUBAS + 6 * MENUENT) /* == 102 */
#define ZOOMIN		102
#define ZOOMOUT 	103

#define GRIDOFF 	105
#define GRIDFIN 	106
#define GRIDMED 	107
#define GRIDCRS 	108

#define PATEDIT 	110

#define FORPRINTER	112
#define FORSCREEN	113

#define SCREEN		1
#define PRINTER 	2

#define LT64KMODE	114
#define NOLIMIT 	115

/* Brush mirror bits. */
#define BM_RIGHT	1
#define BM_UPRIGHT	2
#define BM_UP		4
#define BM_UPLEFT	8
#define BM_RADIAL	16


/*
 *	Palette types
 */
#define PALMAX		4
#define PALNONE 	(-1)
#define PALICON 	0
#define PALSIZE 	1
#define PALSHAPE	2
#define PALPAT		3


/*
 *	Palette area bounds
 *
 *
 *	+-------------------------------+---------+-------+
 *	|	Icons			|	  |	  |
 *	+-------------------------------+ Shapes  | Sizes |
 *	|	Patterns		|	  |	  |
 *	+-------------------------------+---------+-------+
 */
#define PALTOP		5
#define PALLEFT 	10
#define PALBOT		(PATBOT + 5)
#define PALRT		(SIZERT + 10)


#define ICONMAX 	24
#define ICONROW 	2
#define ICONCOL 	((ICONMAX + ICONROW - 1) / ICONROW)

#define PATMAX		36
#define PATROW		4
#define PATCOL		((PATMAX + PATROW - 1) / PATROW)

#define SHAPEMAX	24
#define SHAPEROW	4
#define SHAPECOL	((SHAPEMAX + SHAPEROW - 1) / SHAPEROW)
#define SHAPEWD 	40
#define SHAPEHT 	20
#define SHAPETOP	PALTOP
#define SHAPELEFT	(ICONRT + 10)
#define SHAPEBOT	(SHAPETOP + SHAPEHT * SHAPEROW)
#define SHAPERT 	(SHAPELEFT + SHAPEWD * SHAPECOL)

#define SIZEMAX 	5
#define SIZEROW 	SIZEMAX
#define SIZECOL 	1
#define SIZEWD		40
#define SIZEHT		16
#define SIZETOP 	PALTOP
#define SIZELEFT	(SHAPERT + 10)
#define SIZEBOT 	(SIZETOP + SIZEHT * SIZEROW)
#define SIZERT		(SIZELEFT + SIZEWD * SIZECOL)
#define SIZEHSIZ	SIZEWD
#define SIZEVSIZ	0

#define ARROW		ICONMAX




/*
 *	Mouse button transitions
 */
#define 	DOUBLECLICK	256
#define 	MOUSEUP 	257
#define 	MOUSEDOWN	258
#define 	LFTSFTDN	259	       /* left shift down */
#define 	RGTSFTDN	260
#define 	CTRLDN		261
#define 	ALTDN		262
#define 	LFTSFTUP	263
#define 	RGTSFTUP	264
#define 	CTRLUP		265
#define 	ALTUP		266



/*
 *	Selection transfer modes
 *
 */
#define 	NOMODE		(-1)
#define 	MOVE		0
#define 	COPY		1
#define 	MASSCOPY	2
#define 	STRETCH 	3


/*
 *		Shift key codes
 */
#define 	LSHIFT		0x1
#define 	RSHIFT		0x2
#define 	ALT		0x4
#define 	CTRL		0x8
#define 	CTRL_ALT	(CTRL|ALT)
#define 	SHIFT		(RSHIFT|LSHIFT)




/*
 *	buffer sizes
 */
#define cbHeader	32
#define cbCache 	256


/*
 *	file signatures
 */
#define KEY1		0x6144
#define KEY2		0x4D6E

/* version 1 of paint file format */
#define KEY1V1		0x694c
#define KEY2V1		0x536e

#define CMDMARQE	 0
#define CMDLASSO	 1
#define CMDTEXT 	 2
#define CMDHAND 	 3
#define CMDPENCL	 4
#define CMDERASE	 5
#define CMDLINE 	 6
#define CMDBRUSH	 7
#define CMD3D		 8
#define CMDPAINT	 9
#define CMDSPLIN	10
#define CMDSPRAY	11
#define CMDHBOX 	12
#define CMDFBOX 	13
#define CMDHRRC 	14
#define CMDFRRC 	15
#define CMDHCIRC	16
#define CMDFCIRC	17
#define CMDHOVAL	18
#define CMDFOVAL	19
#define CMDHAREA	20
#define CMDFAREA	21
#define CMDHPOLY	22
#define CMDFPOLY	23



/*
 *	common macros
 */
#define abs(x)	(((x) >= 0) ? (x) : -(x))
#define swap(x,y)	(x^=y,y^=x,x^=y)


#define Printer_to_ScreenX(ScaleArg) \
	MulDiv((ScaleArg), XScaleDenominator, XScaleNumerator)

#define Screen_to_PrinterX(ScaleArg) \
	MulDiv((ScaleArg), XScaleNumerator, XScaleDenominator)

#define Printer_to_ScreenY(ScaleArg) \
	MulDiv((ScaleArg), YScaleDenominator, YScaleNumerator)

#define Screen_to_PrinterY(ScaleArg) \
	MulDiv((ScaleArg), YScaleNumerator, YScaleDenominator)


#define P_to_SX(ScaleArg) (ScaleArg)
#define S_to_PX(ScaleArg) (ScaleArg)
#define P_to_SY(ScaleArg) (ScaleArg)
#define S_to_PY(ScaleArg) (ScaleArg)


#if 0
#define ZFile_to_ZScreen(ScaleArg) \
    (LDiv(LMul((long)(ScaleArg),ZScaleDenominator)+LDiv(ZScaleNumerator,2),ZScaleNumerator))

#define ZScreen_to_ZFile(ScaleArg) \
    (LDiv(LMul((long)(ScaleArg),ZScaleNumerator)+LDiv(ZScaleDenominator,2),ZScaleDenominator))

#endif

NEAR PASCAL ZFile_to_ZScreen(short);
NEAR PASCAL ZScreen_to_ZFile(short);


#ifndef NOSTRUCTS

/*
 *	selection data structure
 */
typedef struct	{
	RECT	rectBounds;
	int	crowMac,
		ccolMac,
		cselMac,
		irowCur,
		icolCur,
		iselCur;
} SELECTION;

typedef struct {
	unsigned	message;
	WORD		wParam;
} PNTMSG;

/*
 *	call back data structure used in font initalization
 */
typedef struct {
	HDC	hdc;
	FARPROC pproc;
} CBDATA;

/*
 *  5/16/86 linsh, increase the number of caches to 3
 */
typedef struct
{
	short  used;
	short  handle;
} CACHE;

typedef struct
{
	short	    type;
	short	    handle;
	unsigned    size;	     /* Linsh - 3/19/87 - Changed from long */
	LPSTR	    adr;	     /* will never allocate > 64k*/
	unsigned    wfree1;	     /* a free field for any use */
	unsigned    wfree2;	     /* a free field for any use */
} MEMORY;


extern	short	FAR PASCAL  MulDiv(short, short, short);
long	FAR	PASCAL	LMul(long, long);
long	FAR	PASCAL	LDiv(long, long);


int	    far PASCAL OpenPathname( LPSTR, int );
int	    far PASCAL DeletePathname( LPSTR );
int	    far PASCAL _lopen( LPSTR, int );
void	    far PASCAL _lclose( int );
int	    far PASCAL _lcreat( LPSTR, int );
BOOL	    far PASCAL _ldelete( LPSTR );
WORD	    far PASCAL _ldup( int );
DWORD	    far PASCAL _llseek( int, long, int );
WORD	    far PASCAL _lread( int, LPSTR, int );
WORD	    far PASCAL _lwrite( int, LPSTR, int );
int	    far PASCAL _lrename( LPSTR, LPSTR );

#define READ	    0	/* Flags for _lopen */
#define WRITE	    1
#define READ_WRITE  2

int   FAR PASCAL lstrcmp(LPSTR, LPSTR);
LPSTR FAR PASCAL lstrcpy(LPSTR, LPSTR);
LPSTR FAR PASCAL lstrcat(LPSTR, LPSTR);
int   FAR PASCAL lstrlen(LPSTR);
LPSTR FAR PASCAL lstrbscan(LPSTR, LPSTR);
LPSTR FAR PASCAL lstrbskip(LPSTR, LPSTR);

#endif


int	NEAR	PASCAL	WinMain (HANDLE, HANDLE, LPSTR, int);
	NEAR	PASCAL	XlatePatEdit (LPMSG);
	NEAR	PASCAL	PaintCreate (HWND);
	NEAR	PASCAL	PaintClose (HWND);
	NEAR	PASCAL	PaintDestroy ();
	NEAR	PASCAL	PaintPaint (HWND, HDC);
	NEAR	PASCAL	PaintSize (HWND, int, int);
	NEAR	PASCAL	PaintRdMouse (HWND, int, LPPOINT);
	NEAR	PASCAL	PaintRdMenu (HWND, int);
	NEAR	PASCAL	PaintScreenMode(HWND, int);
	NEAR	PASCAL	CheckAlign (HWND, int);
	NEAR	PASCAL	CheckMode (HWND, int);
	NEAR	PASCAL	CheckGrid (HWND, int);
	FAR	PASCAL	EraseWnd (HWND);
	NEAR	PASCAL	EraseBM (HDC);
	FAR	PASCAL	MakeCanvas (HWND);
	NEAR	PASCAL	PaintHelp (HWND);
long	FAR	PASCAL	PaintWndProc (HWND, unsigned, WORD, LONG);
	FAR	PASCAL	FreePalette ();
	FAR	PASCAL	SetCmdCur (HWND, int);
int	FAR	PASCAL	GetCmdCur ();
HBRUSH	FAR		GetBrushCur ();
PPOINT	FAR		GetShapeCur ();
HPEN	FAR		GetSizeCur ();
int	FAR	PASCAL	PalFromPt (LPPOINT);
int	FAR	PASCAL	SelFromPt (LPPOINT, int);
	FAR	PASCAL	UpdateSel (LPPOINT, HDC);
	FAR	PASCAL	ChangeSel (HDC, int);
	FAR	PASCAL	HiliteSel (int, HDC);
	FAR	PASCAL	SetupSize ();
	FAR	PASCAL	SetupBrush ();
	FAR	PASCAL	SetupShape ();
	FAR	PASCAL	ShowPalette (HWND, int);
	NEAR	PASCAL	PaletteCreate (HWND);
	NEAR	PASCAL	PaletteRdMouse (HWND, int, LPPOINT);
	NEAR	PASCAL	DrawPatterns (HDC);
	NEAR	PASCAL	DrawShapes (HDC);
	NEAR	PASCAL	OffsetPoly (POINT *, int, int, int);
	NEAR	PASCAL	PalettePaint (HWND, HDC);
	NEAR	PASCAL	PaletteRdKB (HWND, WORD, LONG);
	NEAR	PASCAL	PaletteFocus (HWND, WORD);
long	FAR	PASCAL	PaletteWndProc (HWND, unsigned, WORD, LONG);
	FAR	PASCAL	ResetIcons (int, BOOL);
	FAR	PASCAL	ShowStatus (HWND, HDC, int);
	NEAR	PASCAL	CanvasCreate (HWND);
	NEAR	PASCAL	CanvasSize (HWND, int, int);
	NEAR	PASCAL	CanvasRdMouse (HWND, int, LPPOINT);
	NEAR	PASCAL	CanvasRdChar (HWND, int, int);
	NEAR	PASCAL	CanvasFocus (HWND, WORD);
	NEAR	PASCAL	CanvasPaint (HWND, HDC);
	NEAR	PASCAL	CanvasRdKB (HWND, WORD, LONG);
long	FAR	PASCAL	CanvasWndProc (HWND, unsigned, WORD, LONG);
	FAR	PASCAL	SyncFile ();
	FAR	PASCAL	ScrollFile (HDC, int, int);
	FAR	PASCAL	SyncWnd (HWND);
	FAR	PASCAL	ResizeWnd (HWND, int, int, BOOL);
	FAR	PASCAL	PatEdit (HWND);
	NEAR	PASCAL	PatEditCreate (HWND);
	NEAR	PASCAL	PatEditPaint (HWND, HDC);
	NEAR	PASCAL	PatEditRdMouse(HWND, unsigned, LPPOINT);
	NEAR	PASCAL	PatEditRdKB (HWND, WORD, LONG);
	NEAR	PASCAL	PatEditFocus (HWND, WORD);
	NEAR	PASCAL	PatEditRdMenu (HWND, int);
long	FAR	PASCAL	PatEditWndProc (HWND, unsigned, WORD, LONG);
BOOL	NEAR	PASCAL	TestPat (LPPOINT);
	NEAR	PASCAL	UpdatePat (HDC, LPPOINT);
BOOL	NEAR	PASCAL	InPatRect (LPPOINT);
	NEAR	PASCAL	GetPat ();
	NEAR	PASCAL	PutPat ();
	NEAR	PASCAL	CopyPat (int *, int *);
BOOL	FAR	PASCAL	InitClass (HANDLE);
BOOL	NEAR	PASCAL	RegisterPaint (HANDLE);
BOOL	NEAR	PASCAL	RegisterPalette (HANDLE);
BOOL	NEAR	PASCAL	RegisterCanvas (HANDLE);
BOOL	NEAR	PASCAL	RegisterZoomOut (HANDLE);
BOOL	NEAR	PASCAL	RegisterZoomIn (HANDLE);
BOOL	NEAR	PASCAL	RegisterPatEdit (HANDLE);
BOOL	FAR	PASCAL	InitPalette (HANDLE);
	NEAR	PASCAL	AllocPalette (HANDLE);
	NEAR	PASCAL	InitTools (HANDLE);
#ifdef	PATBM
	NEAR	PASCAL	InitPatterns ();
#endif
	NEAR	PASCAL	InitSizes ();
	NEAR	PASCAL	InitCursors (HANDLE);
#ifdef	SHAPEBM
	NEAR	PASCAL	InitShapes ();
#endif
	NEAR	PASCAL	InitSelect ();
	NEAR	PASCAL	SetSelect (int, int, int, int, int, int, int, int, int);
	NEAR	PASCAL	InitSpray ();
	NEAR	PASCAL	InitDiag ();
	FAR	PASCAL	InitStrings ();
BOOL	FAR	PASCAL	InitBM ();
	FAR	PASCAL	InitFont (HWND, HDC);
	FAR	PASCAL	ResetFont (HDC);
	FAR	PASCAL	PushFont (HDC);
	FAR	PASCAL	PopFont (HDC);
	FAR	PASCAL	FreeFont ();
int	FAR	PASCAL	FillFontMenues (HWND, HDC);
	FAR	PASCAL	SetStyleChecks (HWND);
	FAR	PASCAL	SetFontFace (HWND, WORD);
        FAR     PASCAL  EnableFontSizes (HWND, WORD);
	FAR	PASCAL	SetFontSize (HWND, WORD);
int	FAR	PASCAL	FontCallback (LOGFONT FAR *, TEXTMETRIC FAR *, WORD, HDC FAR *);
int	FAR	PASCAL	MenuCallback (LOGFONT FAR *, TEXTMETRIC FAR *, WORD, HMENU FAR *);
	NEAR	PASCAL	AddNewFont (LPSTR, int);
	NEAR	PASCAL	AddNewSize (int);
int	FAR	PASCAL	FaceCallback (LOGFONT FAR *, TEXTMETRIC FAR *, WORD, CBDATA FAR *);
	NEAR	PASCAL	EnumAllFonts (HDC);
	FAR	PASCAL	ZoomIn (HWND);
	FAR	PASCAL	ZoomOut (HWND);
	FAR	PASCAL	DisableZoom (HWND, int);
	FAR	PASCAL	EnableZoom (HWND, int);
	FAR	PASCAL	NormalFromIn (HWND, HDC);
	FAR	PASCAL	InFromNormal (HWND, HDC);
	NEAR	PASCAL	ZoomInSize (HWND, int, int);
	NEAR	PASCAL	ZoomInPaint (HWND, HDC);
	NEAR	PASCAL	ZoomInPencl (HWND, HDC, BOOL, BOOL);
	NEAR	PASCAL	ZoomInScroll (HWND, HDC, BOOL, BOOL);
	NEAR	PASCAL	ZoomInRdMouse (HWND, int, LPPOINT);
	NEAR	PASCAL	ZoomInRdKB (HWND, WORD, LONG);
	NEAR	PASCAL	ZoomInFocus (HWND, WORD);
long	FAR	PASCAL	ZoomInWndProc (HWND, unsigned, WORD, LONG);
	FAR	PASCAL	ZoomInErase (HDC);
	FAR	PASCAL	ZoomInUndo (HWND);
	FAR	PASCAL	ZoomInUndoSave ();
	FAR	PASCAL	OutFromNormal(HWND);
	FAR	PASCAL	NormalFromOut (HWND);
long	FAR	PASCAL	ZoomOutWndProc (HWND, unsigned, WORD, LONG);
	NEAR	PASCAL	ZoomOutRdMouse(HWND, unsigned, LPPOINT);
	NEAR	PASCAL	ZoomOutRdKB (HWND, WORD, LONG);
	NEAR	PASCAL	ZoomOutFocus (HWND, WORD);
	NEAR	PASCAL	HiliteZoom (HDC);
	NEAR	PASCAL	ZoomOutPaint (HWND, HDC);
	NEAR	PASCAL	ZoomOutSize (int, int);
BOOL	FAR	PASCAL	TrackFileOpen (HWND, unsigned, WORD, LONG);
BOOL	FAR	PASCAL	TrackFileSaveAs (HWND, unsigned, WORD, LONG);
BOOL	FAR	PASCAL	TrackFileChanged (HWND, unsigned, WORD, LONG);
BOOL	FAR	PASCAL	TrackHelp (HWND, unsigned, WORD, LONG);
	FAR	PASCAL	CenterPopup (HWND);
	FAR	PASCAL	GetScreenCenter(HWND, LPPOINT, int, int);
	FAR	PASCAL	EnableFrameSel (HWND);
	FAR	PASCAL	EnableLassoSel (HWND);
	FAR	PASCAL	DisableSel (HWND, HDC);
	FAR	PASCAL	ClearSel (HWND);
	FAR	PASCAL	CutSel (HWND);
	FAR	PASCAL	CopySel (HWND);
	FAR	PASCAL	PasteSel (HWND);
	NEAR	PASCAL	DiskPaste (HWND, HDC, int, int, int, int, int, int);
	NEAR	PASCAL	PtConvert (int, HDC, LPPOINT);
int	NEAR	PASCAL	PixConvert(int, int, int, int);
	NEAR	PASCAL	PasteErr (HWND, WORD);
int	FAR	PASCAL	AbortTest (HDC, int);
int	FAR	PASCAL	AbortPrint (HWND, WORD, WORD, LONG);
HDC	NEAR	PASCAL	GetDCFromDev (char *, BOOL);
HDC	FAR	PASCAL	CreatePrinterDC (BOOL);
	FAR	PASCAL	PrintFile (HWND);
	NEAR	PASCAL	PrintErr (HWND, WORD);
	NEAR	PASCAL	RdStretch (HDC, int, int, int, int, int, int, int, int, FARPROC);
	NEAR	PASCAL	NewFile (HWND);
	FAR	PASCAL	SetupFile (HWND);
	FAR	PASCAL	MakeFile (int, int);
int	FAR	PASCAL	ReopenFile ();
	FAR	PASCAL	CloseFile (HWND);
	FAR	PASCAL	SaveFile (HWND);
	FAR	PASCAL	RenameTmp ();
	FAR	PASCAL	FFileExists (char *);
	FAR	PASCAL	CalculateScaling (HDC);
int	FAR	PASCAL	CopyFile (int, int);
	FAR	PASCAL	WrHeader(int);
	FAR	PASCAL	WrBlt (int, int, int, int, HDC, int, int);
	FAR	PASCAL	RdBlt (HDC, int, int, int, int, int, int);
int	NEAR	PASCAL	RdB (int);
int	NEAR	PASCAL	RdW (int);
	NEAR	PASCAL	WrB (int, int);
	NEAR	PASCAL	WrW (int, int);
	FAR	PASCAL	NewCanvas (HWND);
	FAR	PASCAL	OpenCanvas (HWND);
	FAR	PASCAL	SaveAsCanvas (HWND);
	FAR	PASCAL	SaveNewCanvas (HWND);
	FAR	PASCAL	SaveCanvas (HWND, BOOL);
	FAR	PASCAL	DisableModes (HWND);
int	FAR	PASCAL	CopyFileName (LPSTR);
int	FAR	PASCAL	DupCallback (HWND, LPSTR);
	FAR	PASCAL	FDupFileName ();
BOOL	FAR	PASCAL	GetFileName (HWND);
BOOL	FAR	PASCAL	EquFileName (LPSTR, LPSTR);
	FAR	PASCAL	DefaultNames (char *);
	FAR	PASCAL	NameTmp (char *);
char *	FAR	PASCAL	SkipPath (char *);
	FAR	PASCAL	SetPaintCaption (char *, HWND);
	FAR	PASCAL	UndoSave ();
	FAR	PASCAL	Undo (HWND);
#ifdef	BRUSHMIR
			BrushMirrors (HWND);
			BrushMirrorsPaint (HWND, HDC);
			BrushMirrorsRdMouse(HWND, unsigned, LPPOINT);
			BrushMirrorsRdMenu (HWND, int);
long	FAR	PASCAL	BrushMirrorsWndProc(HWND, unsigned, WORD, LONG);
			UpdateMirroring(HDC);
#endif
int	FAR	PASCAL	Cmd3D (HWND, HDC, BOOL, BOOL);
	NEAR	PASCAL	DrawAxes (HDC, LPPOINT, LPRECT);
	NEAR	PASCAL	GetLeftUp (LPPOINT, LPPOINT, LPRECT);
	NEAR	PASCAL	GetLeftDown (LPPOINT, LPPOINT, LPRECT);
	NEAR	PASCAL	GetRightUp (LPPOINT, LPPOINT, LPRECT);
	NEAR	PASCAL	GetRightDown (LPPOINT, LPPOINT, LPRECT);
	NEAR	PASCAL	GetDown (LPPOINT, LPPOINT, LPRECT);
	NEAR	PASCAL	GetUp (LPPOINT, LPPOINT, LPRECT);
int	FAR	PASCAL	CmdFbox (HWND, HDC, BOOL, BOOL);
int	NEAR	PASCAL	CmdBoxGuts (HWND, HDC, BOOL, BOOL, BOOL);
int	FAR	PASCAL	CmdFrrc (HWND, HDC, BOOL, BOOL);
int	NEAR	PASCAL	CmdRrcGuts (HWND, HDC, BOOL, BOOL, BOOL);
int	FAR	PASCAL	CmdText (HWND, HDC, BOOL, BOOL);
	FAR	PASCAL	DisableText (HWND);
	FAR	PASCAL	UpdateTextCaret (HWND);
	FAR	PASCAL	RedoString (HWND, HDC);
	FAR	PASCAL	Text (HWND, HDC, int, LPSTR);
	NEAR	PASCAL	PrintLine (HWND, HDC, int, BOOL);
	NEAR	PASCAL	RefreshScreen (HWND, HDC);
	NEAR	PASCAL	RefreshLine (HDC, int, int);
	FAR	PASCAL	BldTextSel (HANDLE);
	FAR	PASCAL	RedoTextSel (HWND);
int	FAR	PASCAL	CmdHcirc (HWND, HDC, BOOL, BOOL);
int	FAR	PASCAL	CmdLasso (HWND, HDC, BOOL, BOOL);
	NEAR	PASCAL	MakeLassoSel (HWND, HDC, BOOL, BOOL);
	NEAR	PASCAL	CloseLassoSel (HWND, HDC);
	NEAR	PASCAL	GetBounds (HDC, RECT *, int, int);
	NEAR	PASCAL	SetupLassoSel (HDC);
	NEAR	PASCAL	MoveLassoSel (HDC);
	FAR	PASCAL	HiliteLasso (HDC);
	FAR	PASCAL	ResetLasso ();
	FAR	PASCAL	InvertLassoSel (HDC);
int	FAR	PASCAL	CmdPencl (HWND, HDC, BOOL, BOOL);
int	FAR	PASCAL	CmdBrush (HWND, HDC, BOOL, BOOL);
int	FAR	PASCAL	CmdFcirc (HWND, HDC, BOOL, BOOL);
int	NEAR	PASCAL	CmdCircGuts (HWND, HDC, BOOL, BOOL, BOOL);
int	FAR	PASCAL	CmdHand (HWND, HDC, BOOL, BOOL);
int	FAR	PASCAL	CmdHoval (HWND, HDC, BOOL, BOOL);
int	FAR	PASCAL	CmdLine (HWND, HDC, BOOL, BOOL);
int	FAR	PASCAL	CmdSplin (HWND, HDC, BOOL, BOOL);
#ifdef	SPLINELIM
	NEAR	PASCAL	Spline (PPOINT, PPOINT, PPOINT, int);
#else
	NEAR	PASCAL	Spline (PPOINT, PPOINT, PPOINT);
#endif
	NEAR	PASCAL	UpdateSpline (int, int);
	FAR	PASCAL	TermSpline (HDC);
	FAR	PASCAL	UndoSpline (HDC);
int	FAR	PASCAL	CmdErase (HWND, HDC, BOOL, BOOL);
int	FAR	PASCAL	CmdFoval (HWND, HDC, BOOL, BOOL);
int	NEAR	PASCAL	CmdOvalGuts (HWND, HDC, BOOL, BOOL, BOOL);
int	FAR	PASCAL	CmdHarea (HWND, HDC, BOOL, BOOL);
int	FAR	PASCAL	CmdHpoly (HWND, HDC, BOOL, BOOL);
int	FAR	PASCAL	CmdMarqe (HWND, HDC, BOOL, BOOL);
	NEAR	PASCAL	SetupFrameSel (HDC);
	NEAR	PASCAL	MakeFrameSel (HWND, HDC, BOOL, BOOL);
	FAR	PASCAL	HiliteMarque (HDC);
#ifdef	STRETCHSEL
			SetAnchors (LPPOINT);
#endif
	NEAR	PASCAL	MoveFrameSel (HDC);
#ifdef	STRETCHSEL
			StretchFrameSel (HDC);
#endif
	FAR	PASCAL	TraceSel (HDC);
	FAR	PASCAL	FlipSel (HDC, int);
#ifdef	NOROT
			RotateSel (HDC);
			RotBM (HDC, int);
#endif
	FAR	PASCAL	InvertMarqueSel (HDC);
	FAR	PASCAL	ResetMarqe ();
int	FAR	PASCAL	CmdSpray (HWND, HDC, BOOL, BOOL);
int	FAR	PASCAL	CmdFarea (HWND, HDC, BOOL, BOOL);
int	FAR	PASCAL	CmdFpoly (HWND, HDC, BOOL, BOOL);
int	FAR	PASCAL	CmdHbox (HWND, HDC, BOOL, BOOL);
int	FAR	PASCAL	CmdHrrc (HWND, HDC, BOOL, BOOL);
int	FAR	PASCAL	CmdPaint (HWND, HDC, BOOL, BOOL);

	FAR	PASCAL	PushPen (HDC);
	FAR	PASCAL	PopPen (HDC);
	FAR	PASCAL	PushBrush (HDC);
	FAR	PASCAL	PopBrush (HDC);
HDC	FAR	PASCAL	BindBM (HBITMAP);
	FAR	PASCAL	UpdatePoly (int, int);
	FAR	PASCAL	ResetPoly ();
	FAR	PASCAL	ClosePoly ();
	FAR	PASCAL	DrawPoly (HDC);
	FAR	PASCAL	TermPoly (HDC);
	FAR	PASCAL	TermHPoly (HDC);
int	FAR	PASCAL	PolyPath (int, int, int, int, int, PPOINT, PPOINT);
	FAR	PASCAL	SetPt (PPOINT, int, int);
	FAR	PASCAL	OffsetPt (PPOINT, int, int);
	FAR	PASCAL	DrawBrush (HDC, int, int, int, int);
	FAR	PASCAL	ResizeShapes ();
	FAR	PASCAL	RndRect (HDC, int, int, int, int);
int	FAR	PASCAL	length (int, int);
char *	FAR	PASCAL	ErrString (char *, int, int, char *);
	FAR	PASCAL	ShrinkBM (HDC, int, int, int, int, BOOL);
char	FAR	PASCAL	GetDisk();
int	NEAR	PASCAL	IsTmpOnSameDrive(PSTR);
char *		PASCAL	PFileInPath(PSTR);

/* 5/16/86 linsh */
	FAR	PASCAL	MemInit();
	FAR	PASCAL	MemAlloc(MEMORY FAR *, LONG);
	FAR	PASCAL	MemFree(MEMORY FAR *);
	FAR	PASCAL	MemDestroy();


/*
 *	string id's
 */
#define IDSCAPTION	1
#define IDSUNTITLED	2
#define IDSWARNING	3
#define IDSERROR	4
#define IDSOK		5
#define IDSCANCEL	6
#define IDSABOUT	7
#define IDSFONTSIZE	8
#define IDSFONT 	9

#define IDSDISKSPACE	10
#define IDSNOTEMP	11	/* 4/18/87 - Unable to create temp file */
#define IDSMEMSPACE	12

#define IDSCREATEFAIL	13
#define IDSOPENFAIL1	14
#define IDSOPENFAIL2	15
#define IDSSAVEFAIL1	16
#define IDSSAVEFAIL2	17
#define IDSEXISTS1	18
#define IDSEXISTS2	19
#define IDSINVALID1	20
#define IDSINVALID2	21
#define IDSSAVEQ1	22
#define IDSSAVEQ2	23
#define IDSDUPFILE1	24
#define IDSDUPFILE2	25
#define IDSBADNAME1	26
#define IDSBADNAME2	27
#define IDSDISKFULL	28

#define IDSPRINTERR	29
#define IDSPRINTDISK	30
#define IDSPRINTMEM	31

#define IDSPASTEERR	32
#define IDSCOPYERR	33
#define IDSCUTERR	34
#define IDSBIGPASTE	35
#define IDSPASTETEXT	36
#define IDSPASTEMEM	37
#define IDSSAVEDISK1	38
#define IDSSAVEDISK2	39

