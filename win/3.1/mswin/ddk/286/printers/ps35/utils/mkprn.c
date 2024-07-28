 /***************************************************************************
 * PPD compiler
 *
 * read adobe standard printer descriptions and produce resorces
 * for the windows pscript driver.
 *
 * note, this thing is totally hacked.  some things depend on the order
 * in which they appear.  for example the DefaultFont must appear first.
 * PageSizes must appear before ImageableAreas and such. 
 *
 * in:
 *	file.PPD
 *
 * out:
 *	file.DIR
 *	file.PSS
 *	file.CAP
 *
 * usage:
 *	mkprn [-v] [-s] file		(no extension, .PPD assumed)
 *
 * Note: case is significant in PPD keywords.
 *
 * extensions to the adobe PPD standard:
 *
 *	*EndOfFile: True/False
 *	*AcceptsTrueType: True/False
 *	*TrueImageDevice: True/False
 *
 ***************************************************************************/

#define NOMINMAX 
#define NOCOMM

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include "drivinit.h"
#include "..\printers.h"
#include "..\pss.h"
#include "..\psver.h"
#include "..\resource.h"
#include "fontinfo.h"

#define DOS

#ifdef DOS
/* stdio stuff */
#include <stdio.h>

#undef READ
#undef WRITE

#define OPENR(f)		fopen(f, "r")
#define OPENW(f)		fopen(f, "w")
#define OPENRB(f)		fopen(f, "rb")
#define OPENWB(f)		fopen(f, "wb")
#define CLOSE(f)		fclose(f)
#define READ(buf, count, file)	fread(buf, 1, count, file)
#define WRITE(buf, count, file)	fwrite(buf, 1, count, file)
#define SEEK(f, x, y)		fseek(f, x, y)
#define TELL(f)			ftell(f)

#define STRLEN(s)		strlen(s)
#define STRCAT(s1, s2)		strcat(s1, s2)
#define STRCPY(s1, s2)		strcpy(s1, s2)

#else		/* WINDOWS */

#define OPENR(f)	_lopen(f, "r")
#define OPENW(f)	_lopen(f, "w")
#define OPENRB(f)	_lopen(f, "rb")
#define OPENWB(f)	_lopen(f, "wb")
#define CLOSE(f)	_lclose(f)
#define READ(buf, count, file)	fread(a, 1, b, f)
#define WRITE(buf, count, file)	fwrite(buf, 1, count, file)
#define SEEK(f, x, y)		_lseek(f, x, y)

#define STRLEN(s)		lstrlen(s)
#define STRCAT(s1, s2)		lstrcat(s1, s2)
#define STRCPY(s1, s2)		lstrcpy(s1, s2)

#endif

char szDefTransfer[] = "{}";

#define MAKEFILENAME(f, base, ext)	STRCPY(f, base); STRCAT(f, ext)

typedef struct {
	char *str;
	int value;
} TABLE_ENTRY;
 
TABLE_ENTRY paper_table[] = {
	"Letter:",	DMPAPER_LETTER,
	"LetterSmall:",	DMPAPER_LETTERSMALL,
	"Tabloid:",	DMPAPER_TABLOID,
	"Ledger:",	DMPAPER_LEDGER,
	"Legal:",	DMPAPER_LEGAL,
	"Statement:",	DMPAPER_STATEMENT,
	"Executive:",	DMPAPER_EXECUTIVE,
	"A3:",		DMPAPER_A3,
	"A4:",		DMPAPER_A4,
	"a4small:",	DMPAPER_A4SMALL,
	"A4Small:",	DMPAPER_A4SMALL,
	"A5:",		DMPAPER_A5,
	"B4:",		DMPAPER_B4,
	"B5:",		DMPAPER_B5,
	"Folio:", 	DMPAPER_FOLIO,
	"Quarto:",	DMPAPER_QUARTO,
	"10x14:",	DMPAPER_10X14,
	"11x17:",	DMPAPER_11X17,
	"Note:",	DMPAPER_NOTE,
	"Envelope.279.639",		DMPAPER_ENV_9,
	"Envelope.297.684",		DMPAPER_ENV_10,
	"Envelope.324.747",		DMPAPER_ENV_11,
	"Envelope.342.792",		DMPAPER_ENV_12,
	"Envelope.360.828",		DMPAPER_ENV_14,
        "Envelope.312.624",             DMPAPER_ENV_DL,
        "Envelope.459.649",             DMPAPER_ENV_C5,
        "Envelope.279.540", DMPAPER_ENV_MONARCH,

	/* extra sizes */

	"LetterExtra:",			DMPAPER_LETTER_EXTRA,
	"LegalExtra:",			DMPAPER_LEGAL_EXTRA,
	"TabloidExtra:",		DMPAPER_TABLOID_EXTRA,
	"A4Extra:",                     DMPAPER_A4_EXTRA,

	/* transverse sizes */

	"Letter.Transverse",            DMPAPER_LETTER_TRANSVERSE,
	"A4.Transverse",		DMPAPER_A4_TRANSVERSE,
	"LetterExtra.Transverse",	DMPAPER_LETTER_EXTRA_TRANSVERSE,

	/* plus sizes */
	"SuperA/SuperA/A4:",		 DMPAPER_A_PLUS,
	"SuperB/SuperB/A3:",		 DMPAPER_B_PLUS,
	"LetterPlus:",			 DMPAPER_LETTER_PLUS,
	"A4Plus:",			 DMPAPER_A4_PLUS,

	NULL, 0
};


typedef struct tagSUBTABLE {
    char *szNickname;
    char *szWPDname;
} SUBTABLE;


/* 
 * Table organization: Each line represents a different printer model.  The
 * first field is compared against the initial portion of the *NickName field
 * of the PPD.  If the first field matches then the second field is used as
 * the printer name.  If no match is found, the original *NickName is used as
 * the printer name.
 * 
 * NOTE: Searching terminates on the first entry that is a valid prefix of 
 *       the *NickName.  Model names that are the prefix of another model
 *       name should be listed from the longest name to the shortest name.
 *       (i.e. List "Apple LaserWriter Plus" before "Apple LaserWriter".)
 *
 * NOTE: The version numbers have been dropped from most of the prefixes.
 *       This is done to avoid a table entry for every revision of the PPD
 *       file.
 */

SUBTABLE NickNames[] = {
    "Agfa-Compugraphic 9400P","Agfa-Compugraphic 9400P",
    "Apple LaserWriter II NTX","Apple LaserWriter II NTX",
    "Apple LaserWriter II NT","Apple LaserWriter II NT",
    "Apple LaserWriter Plus","Apple LaserWriter Plus",
    "Apple LaserWriter","Apple LaserWriter",
    "AST TurboLaser/PS","AST TurboLaser/PS",
    "Dataproducts LZR-2665","Dataproducts LZR-2665",
    "EPSON EPL-7500","EPSON EPL-7500",
    "HP LaserJet IID PostScript","HP LaserJet IID PostScript",
    "HP LaserJet IIP PostScript","HP LaserJet IIP PostScript",
    "HP LaserJet III PostScript","HP LaserJet III PostScript",
    "HP LaserJet IIID PostScript","HP LaserJet IIID PostScript",
    "HP LaserJet IIIP PostScript","HP LaserJet IIIP PostScript",
    "IBM 4019 v52.1 (17 Fonts)","IBM LaserPrinter 4019 PS17",
    "IBM 4019 v52.1 (39 Fonts)","IBM LaserPrinter 4019 PS39",
    "IBM 4216-020","IBM 4216-020",
    "IBM 4216-030","IBM 4216-030",
    "Linotronic 100","Linotronic 100",
    "Linotronic 300","Linotronic 300",
    "Linotronic 500","Linotronic 500",
    "NEC Colormate PS/40","NEC Colormate PS/40",
    "NEC Colormate PS/80","NEC Colormate PS/80",
    "NEC Silentwriter LC890XL","NEC Silentwriter LC890XL",
    "NEC Silentwriter LC890","NEC Silentwriter LC890",
    "NEC Silentwriter2 90","NEC Silentwriter2 90",
    "NEC Silentwriter2 290","NEC Silentwriter2 290",
    "NEC Silentwriter2 990","NEC Silentwriter2 990",
    "Oki OL840/PS","Oki OL840/PS",
    "QMS ColorScript 100","QMS ColorScript 100",
    "QMS-PS 800 Plus","QMS-PS 800 Plus",
    "QMS-PS 800","QMS-PS 800",
    "QMS-PS 810","QMS-PS 810",
    "QMS-PS 820","QMS-PS 820",
    "QMS-PS 2200","QMS-PS 2200",
    "TI microLaser PS17","TI microLaser PS17",
    "TI microLaser PS35","TI microLaser PS35",
    "TI OmniLaser 2108","TI OmniLaser 2108",
    "TI Omnilaser 2115","TI Omnilaser 2115",
    "Unisys AP9415","Unisys AP9415",
    NULL, NULL
};

#define PRODUCT			1
#define NICKNAME		2
#define COLORDEVICE		3
#define FILESYSTEM		4
#define DEFAULTRESOLUTION	5
#define DEFAULTTRANSFER		6
#define TRANSFER_NULL		7
#define TRANSFER_NORMALIZED	8
#define DEFAULTPAGESIZE		9
#define PAGESIZE		10
#define PAGEREGION		11
#define DUPLEX			12
#define ACCEPTSTRUETYPE		13
#define TRUEIMAGEDEVICE		14

#define DEFAULTIMAGEABLEAREA	31
#define IMAGEABLEAREA		32

#define DEFAULTMANUALFEED	50
#define MANUALFEED_TRUE		51
#define MANUALFEED_FALSE	52
#define DEFAULTFONT		53
#define FONT			54
#define INPUTSLOT		55
#define PAPERDIMENSION		56
#define DEFAULTINPUTSLOT	57
#define ENDOFFILE		58
#define SCREENFREQ		59
#define SCREENANGLE		60
#define PRTVM                   61
#define SETRESOLUTION           62
#define SETPAGE   63

TABLE_ENTRY parse_table[] = {
	"*Product:",			PRODUCT,
	"*NickName:",			NICKNAME,
	"*ColorDevice:",		COLORDEVICE,
	"*FileSystem:", 		FILESYSTEM,
	"*DefaultResolution:", 		DEFAULTRESOLUTION,
	"*DefaultTransfer:", 		DEFAULTTRANSFER,
	"*Transfer Normalized:", 	TRANSFER_NORMALIZED,
	"*Defaultpagesize:", 		DEFAULTPAGESIZE,
	"*PageSize ", 			PAGESIZE,
	"*PageRegion ",			PAGEREGION,
	"*Duplex ",			DUPLEX,
	"*AcceptsTrueType:",		ACCEPTSTRUETYPE,
	"*TrueImageDevice:",		TRUEIMAGEDEVICE,
	"*DefaultImageableArea:", 	DEFAULTIMAGEABLEAREA,
	"*ImageableArea ", 		IMAGEABLEAREA,
	"*ManualFeed True:",		MANUALFEED_TRUE,
	"*ManualFeed False:",		MANUALFEED_FALSE,
	"*DefaultFont:",		DEFAULTFONT,
	"*Font ", 			FONT,
	"*InputSlot ",			INPUTSLOT,
	"*PaperDimension ",		PAPERDIMENSION,
	"*DefaultInputSlot:",		DEFAULTINPUTSLOT,
	"*ScreenFreq:",			SCREENFREQ,
	"*ScreenAngle:",		SCREENANGLE,
        "*FreeVM:",                     PRTVM,
        "*SetResolution ",              SETRESOLUTION,

	"*EndOfFile:",			ENDOFFILE,	/* extended PPD stuff */
   "*SetPage:", SETPAGE,

	NULL, 0
};

TABLE_ENTRY slot_table[] = {
	"Upper",		DMBIN_UPPER,		/* standard Adobe */
	"OnlyOne",		DMBIN_ONLYONE,		/* slot types */
	"Lower",		DMBIN_LOWER,
	"Middle",		DMBIN_MIDDLE,
	"LargeCapacity",	DMBIN_LARGECAPACITY,

	"AutoSelect",		DMBIN_AUTO,		/* extended tray types */
	"EnvelopeManual",	DMBIN_ENVMANUAL,
	"Envelope",		DMBIN_ENVELOPE,
	"None",			DMBIN_UPPER,		/* map None into only one */

	NULL, 0
};

TABLE_ENTRY duplex_table[] = {
	"DuplexTumble",		DMDUP_HORIZONTAL,
	"DuplexNoTumble",	DMDUP_VERTICAL,
/*	"SimplexTumble",	DMDUP_SIMPLEX, -- ignore this one */
	"None",			DMDUP_SIMPLEX,
	NULL, 0
};

typedef struct {
	DWORD x, y;
} DPOINT;

#define MAX_PAGE 40

/*---------------------------------- data -------------------------------*/

FILE *ppd_file;		/* input */
FILE *dir_file;		/* output */
FILE *cap_file;		/* output */
FILE *pss_file;		/* output */

//#define MAX_PAPERS 50
#define MAX_PAPERS 55

#define PSS_IND_SIMPLEX      (DMDUP_SIMPLEX - DMDUP_SIMPLEX)
#define PSS_IND_DUPLEX_HORZ  (DMDUP_HORIZONTAL - DMDUP_SIMPLEX)
#define PSS_IND_DUPLEX_VERT  (DMDUP_VERTICAL - DMDUP_SIMPLEX)

int	NumFonts, NumRes;
PRINTER printer;
PAPER_REC PaperRec[MAX_PAPERS];	/* all the paper types */
DWORD	PSSDir[NUM_PSS_ENTRIES];
DWORD	NormalPage[MAX_PAPERS];
DWORD	ManualPage[MAX_PAPERS];
DWORD	InputSlot[NUMFEEDS];
DWORD	ManualSwitch[2];
DPOINT	PaperSizes[DMPAPER_USER_LAST+1];
char	DefaultFont[50];
DWORD   Duplex[NUMDUPLEX];

BOOL	Verbose = FALSE;
BOOL	Combine = TRUE;

char DeviceName[] = "PostScript";

/*---------------------------------- defs -------------------------------*/

BOOL	BuildDirEntry(FILE *dir_file, char *long_name);
char	*GetValue(char *token, int *value, TABLE_ENTRY *parse_table);
void	InitPrintCaps(PRINTER *printer);
void	GetString(char *dst, char *src, FILE *ppd_file);
BOOL	GetBool(char *ptr);
int	GetInt(char *ptr);
void	GetImageableArea(RECT *rect, char *ptr);
int	BuildPrinter(char *base_name);
BOOL	GetShortName(char *ShortName, char *LongName);
BOOL	SpecialLookup(char *ShortName, char *LongName);
BOOL	BuildPSSFile(FILE *pss_file);
DWORD	WritePSSStr(FILE *pss_file, char *str);
void	GetDimension(DPOINT *pt, char *ptr);
void	AdjustImageableArea(int i);
BOOL    PatchName(char *pszPrtName);



/*---------------------------------- main -------------------------------*/


int main(int argc, char **argv) 
{
	if (argc < 2) {
		printf("usage: mkprn [-v] [-s] <file>\n");
		printf("\t-v	verbose\n");
		printf("\t-s	separate files (don't combine)\n");
		return 1;
	}
	argv++;

	if (**argv == '-') {
		switch ((*argv)[1]) {
		case 'v':
		case 'V':
			Verbose = TRUE;
			break;

		case 's':
		case 'S':
			Combine = FALSE;
			break;

		default:
			printf("bad switch %s\n", *argv);
			break;
		}
		argv++;
	}

	if (BuildPrinter(*argv))
		printf("Printer build successful\n\n");
	else
		printf("Printer build failed\n\n");

	return 0;
}


/* given a string (token) and a table, lookup token in the table
 * and return the associated value.  also return
 * a pointer just past the token. 
 *
 **/

char *GetValue(char *token, int *value, TABLE_ENTRY *parse_table)
{
	*value = -1;	/* not found by default */

	while (parse_table->str) {

		if (!strncmp(token, parse_table->str, STRLEN(parse_table->str))) {
			*value = parse_table->value;
			break;
		}
		parse_table++;
	}

	return token + STRLEN(parse_table->str);	/* not found */
}


void InitPrintCaps(PRINTER *printer)
{
	int i;

	for (i = 0; i < NUMFEEDS; i++) {
		printer->feed[i] = FALSE;
	}
	printer->version = PRINT_CAP_VERSION;
	printer->defFeed = 0;
	printer->defRes = 300;
	printer->defJobTimeout = 0;
	printer->fEOF = TRUE;
	printer->fColor = FALSE;
	for (i = 0; i < NUMDEVICERES; i++) {
		printer->iDeviceRes[i] = 0;
	}
	printer->iCapsBits = 0;
	printer->iNumPapers = 0;
}

/* get a " bracketed string from the ppd_file */

void GetString(char *dst, char *src, FILE *ppd_file)
{
	char *ptr;
	char ch;

	ptr = src;

	while (*ptr != '"')	/* advance up to first " */
		ptr++;
	ptr++;			/* and past it */

	while (*ptr && *ptr != '"')
		*dst++ = *ptr++;

	*dst = 0;

	if (*ptr != '"') {	/* does this go down to next line */

		ch = 0;
		while (!feof(ppd_file) && ch != '"') {
			READ(&ch, sizeof(ch), ppd_file);
                        if (ch == '\012')
                            ch = ' ';
			*dst++ = ch;
		}
		dst--;
		*dst = 0;
	}
}



BOOL GetBool(char *ptr)
{
	return (*ptr == 'T' || *ptr == 't');
}


int GetInt(char *ptr)
{
	int val;

	sscanf(ptr, "%d", &val);

	return val;
}


long GetLong(char *ptr)
{
        long val;

        sscanf(ptr, "%ld", &val);

        return val;
}

void GetDimension(DPOINT *pt, char *ptr)
{
	char buf[80];
	int x, y;

	GetString(buf, ptr, ppd_file);

	sscanf(buf, "%d %d", &x, &y);

	pt->x = x;
	pt->y = y;
}


void GetImageableArea(RECT *rect, char *ptr)
{
	char buf[80];

	GetString(buf, ptr, ppd_file);

	sscanf(buf, "%d %d %d %d",
		&rect->left,
		&rect->top,
		&rect->right,
		&rect->bottom); 
}


/*
 * convert the PPD user units (1/72 inch) into (1/100th inch units) */

void AdjustImageableArea(int i)
{
	DWORD x0, y0, x1, y1;
	int iPaperType;

	x0 = PaperRec[i].rcImage.left;
	y0 = PaperRec[i].rcImage.top;
	x1 = PaperRec[i].rcImage.right;
	y1 = PaperRec[i].rcImage.bottom;

	iPaperType = PaperRec[i].iPaperType;

	PaperRec[i].rcImage.left   = (short)(x0 * 100 / 72);
	PaperRec[i].rcImage.top    = (PaperSizes[iPaperType].y > y1) ?
                                    (short) (((PaperSizes[iPaperType].y - y1) * 100) / 72) : 0;
	PaperRec[i].rcImage.right  = (short)(x1 * 100 / 72);
	PaperRec[i].rcImage.bottom = (short) (((PaperSizes[iPaperType].y - y0) * 100) / 72);

	if (Verbose) {
		printf("iPaperType:%d\n", iPaperType);

		printf("ImageAdjust: %d %d %d %d\n",
			PaperRec[i].rcImage.left,
			PaperRec[i].rcImage.top,
			PaperRec[i].rcImage.right,
			PaperRec[i].rcImage.bottom);
	}

}


/* write a string to the pss file and return the offset into the
 * file of the beginning of the string.  note, pps strings are
 * preceded by a word that has the length of the string and then
 * the string. */

DWORD WritePSSStr(FILE *pss_file, char *str)
{
	short len;
	DWORD loc;

	loc = TELL(pss_file);

	len = STRLEN(str);

	WRITE(&len, sizeof(short), pss_file);
	WRITE(str, len, pss_file);

	return loc;
}


/* write out the PSS jump tables and then rewrite the master header
 * with the proper locations of the jump files */

BOOL BuildPSSFile(FILE *pss_file)
{

	if (PSSDir[PSS_NORMALIMAGE]) {
		PSSDir[PSS_NORMALIMAGE] = TELL(pss_file);
		WRITE(NormalPage, sizeof(DWORD) * printer.iNumPapers, pss_file);
	}

	if (PSSDir[PSS_MANUALIMAGE]) {
		PSSDir[PSS_MANUALIMAGE] = TELL(pss_file);
		WRITE(ManualPage, sizeof(DWORD) * printer.iNumPapers, pss_file);
	}

	if (PSSDir[PSS_MANUALSWITCH]) {
		PSSDir[PSS_MANUALSWITCH] = TELL(pss_file);
		WRITE(ManualSwitch, sizeof(DWORD) * 2, pss_file);
	}

	if (PSSDir[PSS_INPUTSLOT]) {
		PSSDir[PSS_INPUTSLOT] = TELL(pss_file);
		WRITE(InputSlot, sizeof(DWORD) * NUMFEEDS, pss_file);
	}

	if (PSSDir[PSS_DUPLEX]) {
		PSSDir[PSS_DUPLEX] = TELL(pss_file);
		WRITE(Duplex, sizeof(DWORD) * NUMDUPLEX, pss_file);
	}

	/* if they forgot the transfer function */


	if (!PSSDir[PSS_TRANSFER]) {
		PSSDir[PSS_TRANSFER] = WritePSSStr(pss_file, szDefTransfer);
	}

	SEEK(pss_file, 0L, SEEK_SET);
	WRITE(PSSDir, sizeof(PSSDir), pss_file);

	return TRUE;
}


int FileLen(FILE *f)
{
	int len;

	SEEK(f, 0L, SEEK_END);

	len = (int)TELL(f);

	SEEK(f, 0L, 0);	// return to the start

	printf("file len %d\n", len);

	return len;
}



BOOL CombineFiles(char *base_name)
{
	char file_name[80];
	FILE *wpd_file;
	PS_RES_HEADER header;
	int i;

	MAKEFILENAME(file_name, base_name, ".WPD");
	if (!(wpd_file = OPENWB(file_name)))
		return FALSE;

	MAKEFILENAME(file_name, base_name, ".CAP");
	if (!(cap_file = OPENRB(file_name)))
		return FALSE;

	MAKEFILENAME(file_name, base_name, ".DIR");
	if (!(dir_file = OPENRB(file_name)))
		return FALSE;

	MAKEFILENAME(file_name, base_name, ".PSS");
	if (!(pss_file = OPENRB(file_name)))
		return FALSE;

	header.cap_len = FileLen(cap_file);
	header.dir_len = FileLen(dir_file);
	header.pss_len = FileLen(pss_file);

	WRITE(&header, sizeof header, wpd_file);	// position the file ponter

	header.cap_loc = TELL(wpd_file);

	for (i = 0; i < header.cap_len; i++) {
		fputc(fgetc(cap_file), wpd_file);
	}

	header.dir_loc = TELL(wpd_file);

	for (i = 0; i < header.dir_len; i++) {
		fputc(fgetc(dir_file), wpd_file);
	}

	header.pss_loc = TELL(wpd_file);

	for (i = 0; i < header.pss_len; i++) {
		fputc(fgetc(pss_file), wpd_file);
	}

	SEEK(wpd_file, 0L, 0);

	WRITE(&header, sizeof header, wpd_file);

	CLOSE(wpd_file);
	CLOSE(cap_file);
	CLOSE(dir_file);
	CLOSE(pss_file);

	return TRUE;			// success
}


/* this does all the work */


int BuildPrinter(char *base_name)
{
	char file_name[80];
	char buf[200];
	char buf2[200];
	int value;
	char *ptr;
	char *font;
	int i, j;

	MAKEFILENAME(file_name, base_name, ".ppd");

	if (!(ppd_file = OPENR(file_name))) {
		perror(file_name);
		return FALSE;
	}

	InitPrintCaps(&printer);

	/* CAP file */

	MAKEFILENAME(file_name, base_name, ".cap");

	if (!(cap_file = OPENWB(file_name)))
		return FALSE;

	/* PSS file */

	MAKEFILENAME(file_name, base_name, ".pss");

	if (!(pss_file = OPENWB(file_name)))
		return FALSE;

	for (i = 0; i < NUM_PSS_ENTRIES; i++)
		PSSDir[i] = 0L;

	for (i = 0; i < MAX_PAPERS; i++) {
		NormalPage[i] = ManualPage[i] =	InputSlot[i] = 0L;
	}

	for (i = 0; i < NUMFEEDS; i++) {
		InputSlot[i] = 0L;
	}


	ManualSwitch[0] = ManualSwitch[1] = 0L;

	WRITE(PSSDir, sizeof(PSSDir), pss_file);	/* reserve this space */

	/* DIR file */

	MAKEFILENAME(file_name, base_name, ".dir");

	if (!(dir_file = OPENWB(file_name)))
		return FALSE;

	NumFonts = 0;
	WRITE(&NumFonts, sizeof(short), dir_file);	/* place holder */

        NumRes = 0;

	/********************* pass 1 *********************/

	while (!feof(ppd_file)) {

		fgets(buf, sizeof(buf), ppd_file);

		ptr = GetValue(buf, &value, parse_table);

		if (value == -1)
			continue;

		while (*ptr == ' ')	/* advance to parameters */
			ptr++;

		switch (value) {

		case IMAGEABLEAREA:

			GetValue(ptr, &i, paper_table);

			if (i == -1) {
				printf("warning: unknown paper type %s ", ptr);
				break;
			}

			PaperRec[printer.iNumPapers].iPaperType = i;

			GetImageableArea(&(PaperRec[printer.iNumPapers].rcImage), ptr);

			if (Verbose) {
				printf("IMAGEABLEAREA: [%d] %d %d %d %d",
				PaperRec[printer.iNumPapers].iPaperType,
				PaperRec[printer.iNumPapers].rcImage.left,
				PaperRec[printer.iNumPapers].rcImage.top,
				PaperRec[printer.iNumPapers].rcImage.right,
				PaperRec[printer.iNumPapers].rcImage.bottom);

				printf("  PaperType:%d\n", PaperRec[printer.iNumPapers].iPaperType);
			}

			printer.iNumPapers++;
			break;
		}
	}

	CLOSE(ppd_file);


	/********************* pass 2 *********************/

	MAKEFILENAME(file_name, base_name, ".ppd");

	if (!(ppd_file = OPENR(file_name))) {
		perror(file_name);
		return FALSE;
	}

	while (!feof(ppd_file)) {

		fgets(buf, sizeof(buf), ppd_file);

		ptr = GetValue(buf, &value, parse_table);

		if (value == -1)
			continue;

		while (*ptr == ' ')	/* advance to parameters */
			ptr++;

		switch (value) {

		case ENDOFFILE:

			printer.fEOF = GetBool(ptr);

			if (Verbose)
				printf("ENDOFFILE: %d\n", printer.fEOF);
			break;

		case SCREENFREQ:
			while (*ptr != '"')	/* advance up to first " */
				ptr++;
			ptr++;			/* and past it */

			printer.ScreenFreq = GetInt(ptr) * 10;

			if (Verbose)
				printf("SCREENFREQ: %d\n", printer.ScreenFreq);
			break;

		case SCREENANGLE:
			while (*ptr != '"')	/* advance up to first " */
				ptr++;
			ptr++;			/* and past it */

			printer.ScreenAngle = GetInt(ptr) * 10;

			if (Verbose)
				printf("SCREENANGLE: %d\n", printer.ScreenAngle);
			break;

		case DEFAULTINPUTSLOT:

			/* this sets defFeed.  this value is a DMBIN_* value
			 * and does not need to be adjusted to index into
			 * feed */

			GetValue(ptr, &i, slot_table);

			if (i == -1) {
				printf("warning: unknown InputSlot type %s\n", ptr);
				break;
			}

			if (i >= (NUMFEEDS + DMBIN_FIRST))
				printf("slot out of range %d\n", i);
			else {
				printer.defFeed = i;
				printer.feed[i - DMBIN_FIRST] = TRUE;
			}

			if (Verbose)
				printf("DEFAULTINPUTSLOT: [%d] %s\n", i - DMBIN_FIRST, ptr);
			break;

		case INPUTSLOT:

			PSSDir[PSS_INPUTSLOT] = 1;

			GetValue(ptr, &i, slot_table);

			if (i == -1) {
				printf("warning: unknown InputSlot type %s\n", ptr);
				break;
			}

			i -= DMBIN_FIRST;


			GetString(buf2, ptr, ppd_file);

			if (i >= NUMFEEDS)
				printf("slot out of range %d\n", i);
			else {
				InputSlot[i] = WritePSSStr(pss_file, buf2);
				printer.feed[i] = TRUE;
			}

			if (Verbose)
				printf("INPUTSLOT: [%d] %s\n", i, buf2);
			break;

		/* NOTE: the "*DefaultFont:" entry must come before the
		 * "*Font" entries. 
		 *
		 * here we collect the font information.  the default
		 * font is always the first font.  note, we save the
		 * name of the default font so that it does not appear
		 * twice. */

		case FONT:
		case DEFAULTFONT:

			font = ptr;
			while (*ptr && *ptr != ':' && *ptr != '\n')
				ptr++;
			*ptr = 0;	/* toast the : or \n */

			if (Verbose) {
				if (value == FONT)
					printf("FONT: %s\n", font);
				else
					printf("DEFAULTFONT: %s\n", font);
			}

			if (value == DEFAULTFONT) {

				/* save the default font */

				STRCPY(DefaultFont, font);
			} else {
				/* don't output the default font
				 * twice */

				if (!strcmp(DefaultFont, font))
					break;
			}

			BuildDirEntry(dir_file, font);

			break;

		case NICKNAME:

			GetString(printer.Name, ptr, ppd_file);
                        PatchName(printer.Name);
                        
			if (Verbose)
				printf("NICKNAME: |%s|\n", printer.Name);
			break;

		case COLORDEVICE:

			printer.fColor = GetBool(ptr);

			if (Verbose)
				printf("COLORDEVICE: %d\n", printer.fColor);
			break;

		case DEFAULTRESOLUTION:
			printer.defRes = GetInt(ptr);

			if (Verbose)
				printf("DEFAULTRESOLUTION: %d\n", printer.defRes);
			break;

                case SETRESOLUTION:
                        /* !!! Assumes DEFAULTRESOLUTION already processed */
                        i = GetInt(ptr);
                        if (i == printer.defRes)
                            break;

                        if (NumRes >= NUMDEVICERES) {
                            printf("Warning: Not enough room in .WPD for resolution list\n");
                            break;
                        }

                        if (Verbose)
                            printf("SETRESOLUTION: %d\n", i);

                        printer.iDeviceRes[NumRes++] = i;
                        break;

		case PAPERDIMENSION:

			GetValue(ptr, &i, paper_table);

			if (i == -1) {
				printf("warning: unknown paper type %s ", ptr);
				break;
			}

			/* PaperSizes are indexed by DMPAPER_* numbers! */

			GetDimension(&PaperSizes[i], ptr);

			if (Verbose)
				printf("PAPERDIMENSION: [%d] %d %d\n", i,
					(short)PaperSizes[i].x,
					(short)PaperSizes[i].y);

			break;

		case TRANSFER_NORMALIZED:

			GetString(buf2, ptr, ppd_file);

			PSSDir[PSS_TRANSFER] = WritePSSStr(pss_file, buf2);

			if (Verbose)
				printf("TRANSFER_NORMALIZED: %s\n", buf2);
			break;


		case MANUALFEED_TRUE:

			PSSDir[PSS_MANUALSWITCH] = 1;

			GetString(buf2, ptr, ppd_file);

			ManualSwitch[1] = WritePSSStr(pss_file, buf2);

			printer.feed[DMBIN_MANUAL - DMBIN_FIRST] = TRUE;

			if (Verbose)
				printf("MANUALFEED_TRUE: %s\n", buf2);
			break;

		case MANUALFEED_FALSE:

			PSSDir[PSS_MANUALSWITCH] = 1;

			GetString(buf2, ptr, ppd_file);

			ManualSwitch[0] = WritePSSStr(pss_file, buf2);

			printer.feed[DMBIN_MANUAL - DMBIN_FIRST] = TRUE;

			if (Verbose)
				printf("MANUALFEED_FALSE: %s\n", buf2);
			break;

		/* PS stuff to set page size for regular operation */

		case PAGESIZE:

			PSSDir[PSS_NORMALIMAGE] = 1;

			GetValue(ptr, &i, paper_table);

			if (i == -1) {
				printf("warning: unknown paper type %s ", ptr);
				break;
			}

			for (j = 0; j < printer.iNumPapers; j++)
				if (PaperRec[j].iPaperType == i)
					break;

			GetString(buf2, ptr, ppd_file);

			NormalPage[j] = WritePSSStr(pss_file, buf2);

			if (Verbose)
				printf("PAGESIZE: [%d] %s\n", i, buf2);
			break;

		/* PS stuff to set page region for manual feed operation */

		case PAGEREGION:

			PSSDir[PSS_MANUALIMAGE] = 1;

			GetValue(ptr, &i, paper_table);

			if (i == -1) {
				printf("warning: unknown paper type %s ", ptr);
				break;
			}

			for (j = 0; j < printer.iNumPapers; j++)
				if (PaperRec[j].iPaperType == i)
					break;

			GetString(buf2, ptr, ppd_file);

			ManualPage[j] = WritePSSStr(pss_file, buf2);

			if (Verbose)
				printf("PAGEREGION: [%d] %s\n", i, buf2);
			break;

                case PRTVM:
			while (*ptr != '"')	/* advance up to first " */
				ptr++;
			ptr++;			/* and past it */

			printer.iMaxVM = (int)(GetLong(ptr) >> 10);

			if (Verbose)
				printf("PRTVM (in K): %d\n", printer.iMaxVM);
			break;

		case DUPLEX:
			GetValue(ptr, &i, duplex_table);

			if (i == -1) {
				printf("warning: unknown Duplex type %s\n", ptr);
				break;
			}

			PSSDir[PSS_DUPLEX] = 1;

			i -= DMDUP_SIMPLEX;

			GetString(buf2, ptr, ppd_file);

			if (i >= NUMDUPLEX)
				printf("duplex value out of range %d\n", i);
			else {
				Duplex[i] = WritePSSStr(pss_file, buf2);

				switch (i) {
					case PSS_IND_SIMPLEX:
						printer.iCapsBits |= CAPS_SIMPLEX;
						break;
					case PSS_IND_DUPLEX_HORZ:
						printer.iCapsBits |= CAPS_DUPLEX_HORZ;
						break;
					case PSS_IND_DUPLEX_VERT:
						printer.iCapsBits |= CAPS_DUPLEX_VERT;
						break;
				}
			}

			if (Verbose)
				printf("DUPLEX: [%d] bits x%x %s\n", i, (printer.iCapsBits & (CAPS_DUPLEX | CAPS_SIMPLEX)), buf2);
			break;

		case ACCEPTSTRUETYPE:
			if (GetBool(ptr))
				printer.iCapsBits |= CAPS_TRUETYPE;

			if (Verbose)
				printf("ACCEPTSTRUETYPE: bit x%x\n", (printer.iCapsBits & CAPS_TRUETYPE));
			break;

		case SETPAGE:
			if (GetBool(ptr))
				printer.iCapsBits |= CAPS_SETPAGE;

			if (Verbose)
				printf("SETPAGE: bit x%x\n", (printer.iCapsBits & CAPS_SETPAGE));
			break;

		case TRUEIMAGEDEVICE:
			if (GetBool(ptr))
				printer.iCapsBits |= CAPS_TRUEIMAGE;

			if (Verbose)
				printf("TRUEIMAGEDEVICE: bit x%x\n", (printer.iCapsBits & CAPS_TRUEIMAGE));
			break;

		}
	}

	CLOSE(ppd_file);


	for (i = 0; i < printer.iNumPapers; i++) {
		 AdjustImageableArea(i);
	}

	/* wrap up stuff */

	/* write # of fonts in font dir */

	SEEK(dir_file, 0L, 0);
	WRITE(&NumFonts, sizeof(short), dir_file);

	/* caps file */

	WRITE(&printer, sizeof(PRINTER) - sizeof(PAPER_REC), cap_file);
	WRITE(PaperRec, sizeof(PAPER_REC) * printer.iNumPapers, cap_file);


	/* pss file */

	BuildPSSFile(pss_file);

	CLOSE(dir_file);
	CLOSE(cap_file);
	CLOSE(pss_file);

	if (Combine) {
		if (!CombineFiles(base_name))
			printf("error, combine failed\n");
	}


	return TRUE;	/* success */
}



typedef struct {
	char *key;
	char *value;
} KEY_VAL_PAIR;

/* this table maps full postscript face names into the pfm file names
 * and resource names used by the postscript driver. all other names
 * are constructed by taking all capital letters from the full adobe
 * name and cating them together. */

KEY_VAL_PAIR font_xlate[] = {
	"Arial",			"ar",
	"Arial-Bold",			"arb",
	"Arial-BoldOblique",		"arbo",
	"Arial-Oblique",		"aro",
	"Arial-Narrow",			"arn",
	"Arial-Narrow-Bold",		"arnb",
	"Arial-Narrow-BoldOblique", 	"arnbo",
	"Arial-Narrow-Oblique",		"arno",
	"Courier", 			"cm",
	"Helvetica",			"hm",
	"Helvetica-Black",		"hvbl",
	"Helvetica-BlackOblique",	"hvblo",
	"Helvetica-Light",		"hvl",
	"Helvetica-LightOblique",	"hvlo",
	"Helvetica-Narrow",		"hvn",
	"Helvetica-Narrow-Bold",	"hvnb",
	"Helvetica-Narrow-BoldOblique", "hvnbo",
	"Helvetica-Narrow-Oblique",	"hvno",
	"NewCenturySchlbk-Bold",	"ncb",
	"NewCenturySchlbk-BoldItalic",	"ncbi",
	"NewCenturySchlbk-Italic",	"nci",
	"NewCenturySchlbk-Roman",	"ncr",
	"Korinna-Bold",			"krb",
	"Korinna-Regular",		"krrg",
	"Korinna-KursivRegular",	"krkx",
	"Korinna-KursivBold",		"krkb",
	"LubalinGraph-Book",		"lb",
	"LubalinGraph-BookOblique",	"lbo",
	"LubalinGraph-Demi",		"ld",
	"LubalinGraph-DemiOblique",	"ldo",
	"Symbol",			"sm",
	"Times-New-Roman",		"ntr",
	"Times-New-Roman-Bold",		"ntb",
	"Times-New-Roman-BoldItalic",	"ntbi",
	"Times-New-Roman-Italic",	"nti",
	"Varitimes#Bold",		"vtb",
	"Varitimes#BoldItalic",		"vtbi",
	"Varitimes#Italic",		"vti",
	"Varitimes#Roman",		"vtr",
	"ZapfCalligraphic-Roman",	"zca",
	"ZapfCalligraphic-Bold",	"zcab",
	"ZapfCalligraphic-BoldItalic",	"zcabi",
	"ZapfCalligraphic-Italic",	"zcai",
	"ZapfChancery-MediumItalic",	"zc",
	NULL, 0
};


BOOL SpecialLookup(char *ShortName, char *LongName)
{
	KEY_VAL_PAIR *ptr = font_xlate;

	while (ptr->key) {
		if (!strcmp(LongName, ptr->key)) {
			STRCPY(ShortName, ptr->value);
			return TRUE;
		}
		ptr++;
	}

	return FALSE;	/* not found */
}

/* convert a standard adobe long face name into a short abreviated
 * name by extracting all upper case letters from the name.  this works
 * for most face names.  use SpecialLookup() for those that it doesn't
 * work on */

BOOL GetShortName(char *ShortName, char *LongName)
{
	char *src;
	char *dst;

	src = LongName;
	dst = ShortName;

	if (SpecialLookup(ShortName, LongName))
		return TRUE;

	while (*src) {
		if (*src >= 'A' && *src <= 'Z')
			*dst++ = *src;
		src++;
	}

	*dst = 0;	/* null terminate */

	return TRUE;

	printf("GetShortName() long:%s  %s\n", LongName, ShortName);
}



/*
 * directory entrys look like this:
 *
 *   short size
 *   short font_name_offset	(this is the short resource name)
 *   FONTINFO df
 *   char device_name[]		(pointed to by dfDevice)
 *   char face_name[]		(pointed to by dfFace)
 *   char font_name[]		(short font resource name)
 *
 *
 * in:
 *	dir_file	font directory file being written to
 *	long_name	short font name (resource and pfm file base name)
 */


BOOL BuildDirEntry(FILE *dir_file, char *long_name)
{
	PFM pfm;
	char file_name[80];
	char font_name[20];	/* build short name here */

	int entry_size;
	int font_name_offset;
	char face_name[80];

	FILE *pfm_file;

	GetShortName(font_name, long_name);

	MAKEFILENAME(file_name, font_name, ".pfm");

	if (!(pfm_file = OPENRB(file_name))) {
		printf("can not open %s.  font not included.\n", file_name);
		return FALSE;
	}

	NumFonts++;

	if (READ(&pfm, sizeof(PFM), pfm_file) != sizeof(PFM)) {
		printf("reading of pfm file failed\n");
		CLOSE(pfm_file);
		return FALSE;
	}

	/* note, we probally won't read in all of sizeof(face_name) */

	SEEK(pfm_file, pfm.df.dfFace, SEEK_SET);

	READ(face_name, sizeof(face_name), pfm_file);

	if (Verbose)
		printf("PFM face name: %s\n", face_name);

	pfm.df.dfDevice = sizeof(FONTINFO);	/* device follows imedeatly */

	pfm.df.dfFace = pfm.df.dfDevice + STRLEN(DeviceName) + 1;

	font_name_offset = (int)pfm.df.dfFace + STRLEN(face_name) + 1;

	entry_size = 2 * sizeof(short) + 
		     font_name_offset + STRLEN(font_name) + 1;

	WRITE(&entry_size, sizeof(short), dir_file);
	WRITE(&font_name_offset, sizeof(short), dir_file);

	WRITE(&pfm.df, sizeof(FONTINFO), dir_file);
	WRITE(DeviceName, STRLEN(DeviceName) + 1, dir_file);
	WRITE(face_name,  STRLEN(face_name)  + 1, dir_file);
	WRITE(font_name,  STRLEN(font_name)  + 1, dir_file);


#if 0	/* DEBUG */

	printf("dfFontName %04x\n", font_name_offset);
	printf("dfDevice   %08lx\n", pfm.df.dfDevice);
	printf("dfFace     %08lx\n", pfm.df.dfFace);
#endif

	CLOSE(pfm_file);

	return TRUE;
}

/* PatchName: Searches NickNames table for pszPrtName, and replaces 
 * pszPrtName with substituted name if found.  The return value says
 * whether the printer name was substituted.
 */
BOOL PatchName(char *pszPrtName)
{
    char *pSrc, *pDst;
    int i;

    for (i = 0; NickNames[i].szNickname; ++i) {
        pSrc = NickNames[i].szNickname;
        pDst = pszPrtName;

        /* find first non-matching character or EOS */
        while (*pSrc == *pDst && *pSrc) {
            ++pSrc;
            ++pDst;
        }

        /* if we hit end of source string then the prefix matched */
        if (!*pSrc) {
            if (Verbose)
                printf("PatchName: %s translates to %s\n", pszPrtName,
                       NickNames[i].szWPDname);
            strcpy(pszPrtName, NickNames[i].szWPDname);
            return TRUE;
        }
    }

    return FALSE;
}
