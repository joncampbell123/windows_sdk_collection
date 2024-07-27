#include "printer.h"
#include "gdidefs.inc"
#include "epson.h"
#include "device.h"

char FntFile[] = "ps6.fnt";

char DeviceName[] = "Epson FX-80";	/* see dfDevice */

char FaceName[] = "Proportional Expanded";	/* see dfFace */

FONTINFO FontInfo = {
	0x80,	/* dfType (0x0080 is device font) */
	16,	/* dfPoints (in 1/72") */
	72,	/* dfVertRes */
	120,	/* dfHorizRes */
	7,	/* dfAscent */
	0,	/* dfInternalLeading */
	3,	/* dfExternalLeading */
	0,	/* dfItalic */
	0,	/* dfUnderline */
	0,	/* dfStrikeOut */
	400,	/* dfWeight */
	0,	/* dfCharSet */
	0,	/* dfPixWidth */
	9,	/* dfPixHeight */
	0x21,	/* dfPitchAndFamily
		 * low bit is variable pitch flag
		 *    0 => FF_DONTCARE
		 *    1 => FF_ROMAN
		 *    2 => FF_SWISS
		 *    3 => FF_MODERN
		 *    4 => FF_SCRIPT
		 *    5 => FF_DECORATIVE
		 */
	20,	/* dfAvgWidth */
	24,	/* dfMaxWidth */
	0x20,	/* dfFirstChar */
	0xff,	/* dfLastChar */
	0x2e,	/* dfDefaultChar + dfFirstChar */
	0x20,	/* dfBreakChar + dfFirstChar */
	0,	/* dfWidthBytes ( = 0 ) */
	0x41,	/* dfDevice - see DeviceName */
	0x4d,	/* dfFace - see FaceName */
	0x0,	/* dfBitsPointer ( = 0 ) */
	0x0,	/* dfBitsOffset ( = 0 ) */
};

PRDFONTINFO PrdFontInfo = {	/* Driver Specific Info */
	"",	/* dfFont */
	0,	/* wheel */
		/* bfont: */
	66,		/* offset */
	00,		/* mod */
	04,		/* length */
		/* efont: */
	70,		/* offset */
	00,		/* mod */
	04,		/* length */
	0x63,	/* widthtable - see WidthTable */
};

int bWidthTable = TRUE;
short WidthTable[] = {
	24,	/* 0x20 */
	10,	/* 0x21 */
	16,	/* 0x22 */
	24,	/* 0x23 */
	24,	/* 0x24 */
	24,	/* 0x25 */
	24,	/* 0x26 */
	10,	/* 0x27 */
	12,	/* 0x28 */
	12,	/* 0x29 */
	24,	/* 0x2a */
	24,	/* 0x2b */
	14,	/* 0x2c */
	24,	/* 0x2d */
	12,	/* 0x2e */
	20,	/* 0x2f */
	24,	/* 0x30 */
	16,	/* 0x31 */
	24,	/* 0x32 */
	24,	/* 0x33 */
	24,	/* 0x34 */
	24,	/* 0x35 */
	24,	/* 0x36 */
	24,	/* 0x37 */
	24,	/* 0x38 */
	24,	/* 0x39 */
	12,	/* 0x3a */
	12,	/* 0x3b */
	20,	/* 0x3c */
	24,	/* 0x3d */
	20,	/* 0x3e */
	24,	/* 0x3f */
	24,	/* 0x40 */
	24,	/* 0x41 */
	24,	/* 0x42 */
	24,	/* 0x43 */
	24,	/* 0x44 */
	24,	/* 0x45 */
	24,	/* 0x46 */
	24,	/* 0x47 */
	24,	/* 0x48 */
	16,	/* 0x49 */
	22,	/* 0x4a */
	24,	/* 0x4b */
	24,	/* 0x4c */
	24,	/* 0x4d */
	24,	/* 0x4e */
	24,	/* 0x4f */
	24,	/* 0x50 */
	24,	/* 0x51 */
	24,	/* 0x52 */
	24,	/* 0x53 */
	24,	/* 0x54 */
	24,	/* 0x55 */
	24,	/* 0x56 */
	24,	/* 0x57 */
	20,	/* 0x58 */
	24,	/* 0x59 */
	20,	/* 0x5a */
	16,	/* 0x5b */
	20,	/* 0x5c */
	16,	/* 0x5d */
	24,	/* 0x5e */
	24,	/* 0x5f */
	10,	/* 0x60 */
	24,	/* 0x61 */
	22,	/* 0x62 */
	22,	/* 0x63 */
	22,	/* 0x64 */
	24,	/* 0x65 */
	20,	/* 0x66 */
	22,	/* 0x67 */
	22,	/* 0x68 */
	16,	/* 0x69 */
	18,	/* 0x6a */
	20,	/* 0x6b */
	16,	/* 0x6c */
	24,	/* 0x6d */
	22,	/* 0x6e */
	24,	/* 0x6f */
	22,	/* 0x70 */
	22,	/* 0x71 */
	22,	/* 0x72 */
	24,	/* 0x73 */
	22,	/* 0x74 */
	24,	/* 0x75 */
	24,	/* 0x76 */
	24,	/* 0x77 */
	20,	/* 0x78 */
	24,	/* 0x79 */
	20,	/* 0x7a */
	18,	/* 0x7b */
	10,	/* 0x7c */
	18,	/* 0x7d */
	24,	/* 0x7e */
	0,	/* 0x7f */
};

unsigned char WidthFirstChar = 0x20;
unsigned char WidthLastChar = 0x7f;
