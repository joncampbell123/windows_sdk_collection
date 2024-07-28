/* Printer capability structure--used to consolidate the individual
 * printer's capabilities and requirements.
 *
 * the size of this structure depends on the defs in drvinint.h
 *
 * msd 08-Mar-91	Added iCapsBits field to PRINTER struct, bumped
 *			version number, and renamed Reserved to iDeviceRes,
 *			which is what it was being used for anyways.
 *			Added DUPLEX and TRUETYPE caps bits.
 */

#ifndef CAPS_SIMPLEX

#define NUMFEEDS (DMBIN_LAST - DMBIN_FIRST + 2)
//      have to keep size of array compatible with previous
//      wpd  files.
#define NUMPAPERS (DMPAPER_LAST - DMPAPER_FIRST + 1)
#define NUMDEVICERES 4
#define NUMDUPLEX 3

#define CAPS_SIMPLEX      0x0001
#define CAPS_DUPLEX_VERT  0x0002
#define CAPS_DUPLEX_HORZ  0x0004
#define CAPS_TRUETYPE     0x0008
#define CAPS_TRUEIMAGE    0x0010
#define CAPS_SETPAGE      0x0020

/*	Additional duplex constant.
 */
#define CAPS_DUPLEX       (CAPS_DUPLEX_VERT | CAPS_DUPLEX_HORZ)

/*	Macro for detecting duplex.
 */
#define IS_DUPLEX(pPrinter) \
	(((pPrinter)->version >= 0x0002) && ((pPrinter)->iCapsBits & CAPS_DUPLEX))

/*	Macro for detecting truetype.
 */
#define ACCEPTS_TRUETYPE(pPrinter) \
	(((pPrinter)->version >= 0x0002) && ((pPrinter)->iCapsBits & CAPS_TRUETYPE))

/*	Macro for detecting trueimage devices.
 */
#define IS_TRUEIMAGE(pPrinter) \
	(((pPrinter)->version >= 0x0002) && ((pPrinter)->iCapsBits & CAPS_TRUEIMAGE))

/* Macro for detecting devices supporting setpage to implement custom sizes
 */
#define IS_SETPAGE(pPrinter) \
   (((pPrinter)->version >= 0x0002) && ((pPrinter)->iCapsBits & CAPS_SETPAGE))

typedef struct {
	int iPaperType;
	RECT rcImage;
} PAPER_REC;


typedef struct {
	int	version;		// version number
	char	Name[32];		/* the printer name */
	int	defFeed;		/* default feeder (DMBIN_*) */
	BOOL	feed[NUMFEEDS];
	int	defRes;
	int	defJobTimeout;
	BOOL	fEOF;
	BOOL	fColor;
	int	ScreenFreq;	// in 10ths of an inch
	int	ScreenAngle;	// in 10ths of a degree
        int     iMaxVM;         // amount of printer VM in K
	int	iDeviceRes[NUMDEVICERES]; // device resolutions
	int	iCapsBits;	// various capabilities bits
	int	iNumPapers;	/* # of paper sizes supported */
	PAPER_REC Paper[1];	/* variable size array of papers supported */
} PRINTER, FAR *LPPRINTER, *PPRINTER;


typedef struct { 
	long	cap_loc;
	int	cap_len;
	long	dir_loc;
	int	dir_len;
	long	pss_loc;
	int	pss_len;
} PS_RES_HEADER;

#endif
