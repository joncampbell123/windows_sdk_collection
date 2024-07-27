#include "printer.h"
#include "gdidefs.inc"
#include "epson.h"
#include "device.h"

char FntFile[] = "ps12it.fnt";

char DeviceName[] = "Epson Fx-80";	/* see dfDevice */

char FaceName[] = "Proportional";	/* see dfFace */

FONTINFO FontInfo = {
	0x80,	/* dfType (0x0080 is device font) */
	12,	/* dfPoints (in 1/72") */
	72,	/* dfVertRes */
	120,	/* dfHorizRes */
	7,	/* dfAscent */
	0,	/* dfInternalLeading */
	3,	/* dfExternalLeading */
	1,	/* dfItalic */
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
	10,	/* dfAvgWidth */
	10,	/* dfMaxWidth */
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
	60,		/* offset */
	00,		/* mod */
	03,		/* length */
		/* efont: */
	63,		/* offset */
	00,		/* mod */
	03,		/* length */
	0x5a,	/* widthtable - see WidthTable */
};

int bWidthTable = TRUE;
short WidthTable[] = {
	12,	/* 0x20 */
	10,	/* 0x21 */
	10,	/* 0x22 */
	12,	/* 0x23 */
	11,	/* 0x24 */
	12,	/* 0x25 */
	12,	/* 0x26 */
	5,	/* 0x27 */
	8,	/* 0x28 */
	8,	/* 0x29 */
	12,	/* 0x2a */
	12,	/* 0x2b */
	8,	/* 0x2c */
	12,	/* 0x2d */
	7,	/* 0x2e */
	10,	/* 0x2f */
	12,	/* 0x30 */
	9,	/* 0x31 */
	12,	/* 0x32 */
	12,	/* 0x33 */
	12,	/* 0x34 */
	12,	/* 0x35 */
	11,	/* 0x36 */
	12,	/* 0x37 */
	12,	/* 0x38 */
	11,	/* 0x39 */
	8,	/* 0x3a */
	9,	/* 0x3b */
	10,	/* 0x3c */
	11,	/* 0x3d */
	9,	/* 0x3e */
	11,	/* 0x3f */
	12,	/* 0x40 */
	12,	/* 0x41 */
	12,	/* 0x42 */
	12,	/* 0x43 */
	12,	/* 0x44 */
	12,	/* 0x45 */
	12,	/* 0x46 */
	12,	/* 0x47 */
	12,	/* 0x48 */
	10,	/* 0x49 */
	12,	/* 0x4a */
	12,	/* 0x4b */
	10,	/* 0x4c */
	12,	/* 0x4d */
	12,	/* 0x4e */
	12,	/* 0x4f */
	12,	/* 0x50 */
	12,	/* 0x51 */
	12,	/* 0x52 */
	12,	/* 0x53 */
	12,	/* 0x54 */
	12,	/* 0x55 */
	11,	/* 0x56 */
	12,	/* 0x57 */
	12,	/* 0x58 */
	12,	/* 0x59 */
	12,	/* 0x5a */
	11,	/* 0x5b */
	7,	/* 0x5c */
	11,	/* 0x5d */
	10,	/* 0x5e */
	12,	/* 0x5f */
	5,	/* 0x60 */
	11,	/* 0x61 */
	11,	/* 0x62 */
	11,	/* 0x63 */
	12,	/* 0x64 */
	11,	/* 0x65 */
	12,	/* 0x66 */
	11,	/* 0x67 */
	11,	/* 0x68 */
	9,	/* 0x69 */
	10,	/* 0x6a */
	11,	/* 0x6b */
	9,	/* 0x6c */
	11,	/* 0x6d */
	10,	/* 0x6e */
	11,	/* 0x6f */
	11,	/* 0x70 */
	11,	/* 0x71 */
	10,	/* 0x72 */
	11,	/* 0x73 */
	10,	/* 0x74 */
	11,	/* 0x75 */
	10,	/* 0x76 */
	12,	/* 0x77 */
	12,	/* 0x78 */
	11,	/* 0x79 */
	12,	/* 0x7a */
	10,	/* 0x7b */
	9,	/* 0x7c */
	10,	/* 0x7d */
	12,	/* 0x7e */
	0,	/* 0x7f */
	0,	/* 0x80 */
	0,	/* 0x81 */
	1,	/* 0x82 */
	0,	/* 0x83 */
	0,	/* 0x84 */
	0,	/* 0x85 */
	0,	/* 0x86 */
	0,	/* 0x87 */
	0,	/* 0x88 */
	0,	/* 0x89 */
	1,	/* 0x8a */
	0,	/* 0x8b */
	1,	/* 0x8c */
	0,	/* 0x8d */
	0,	/* 0x8e */
	0,	/* 0x8f */
	1,	/* 0x90 */
	0,	/* 0x91 */
	1,	/* 0x92 */
	1,	/* 0x93 */
	1,	/* 0x94 */
	0,	/* 0x95 */
	1,	/* 0x96 */
	0,	/* 0x97 */
	0,	/* 0x98 */
	0,	/* 0x99 */
	0,	/* 0x9a */
	1,	/* 0x9b */
	0,	/* 0x9c */
};

unsigned char WidthFirstChar = 0x20;
unsigned char WidthLastChar = 0x9c;
