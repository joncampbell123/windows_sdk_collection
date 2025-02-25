#include "printer.h"
#include "gdidefs.inc"
#include "epson.h"
#include "device.h"

char FntFile[] = "ocr_b5.fnt";

char DeviceName[] = "IBM Color Printer (B/W)";	/* see dfDevice */

char FaceName[] = "OCR-B double-width";	/* see dfFace */

FONTINFO FontInfo = {
	0x80,	/* dfType (0x0080 is device font) */
	12,	/* dfPoints (in 1/72") */
	84,	/* dfVertRes */
	140,	/* dfHorizRes */
	8,	/* dfAscent */
	0,	/* dfInternalLeading */
	4,	/* dfExternalLeading */
	0,	/* dfItalic */
	0,	/* dfUnderline */
	0,	/* dfStrikeOut */
	400,	/* dfWeight */
	0,	/* dfCharSet */
	28,	/* dfPixWidth */
	10,	/* dfPixHeight */
	0x30,	/* dfPitchAndFamily
		 * low bit is variable pitch flag
		 *    0 => FF_DONTCARE
		 *    1 => FF_ROMAN
		 *    2 => FF_SWISS
		 *    3 => FF_MODERN
		 *    4 => FF_SCRIPT
		 *    5 => FF_DECORATIVE
		 */
	28,	/* dfAvgWidth */
	28,	/* dfMaxWidth */
	0x20,	/* dfFirstChar */
	0xff,	/* dfLastChar */
	0x2e,	/* dfDefaultChar + dfFirstChar */
	0x20,	/* dfBreakChar + dfFirstChar */
	0,	/* dfWidthBytes ( = 0 ) */
	0x41,	/* dfDevice - see DeviceName */
	0x59,	/* dfFace - see FaceName */
	0x0,	/* dfBitsPointer ( = 0 ) */
	0x0,	/* dfBitsOffset ( = 0 ) */
};

PRDFONTINFO PrdFontInfo = {	/* Driver Specific Info */
	"",	/* dfFont */
	0,	/* wheel */
		/* bfont: */
	0,		/* offset */
	00,		/* mod */
	03,		/* length */
		/* efont: */
	3,		/* offset */
	00,		/* mod */
	03,		/* length */
	0x0,	/* widthtable - see WidthTable */
};

/* No Width Table */
int bWidthTable = FALSE;
short WidthTable[] = {
	0,
};
unsigned char WidthFirstChar = 0;
unsigned char WidthLastChar = 0;
