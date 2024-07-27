#include "printer.h"
#include "gdidefs.inc"
#include "epson.h"
#include "device.h"

char FntFile[] = "ps6it.fnt";

char DeviceName[] = "Epson Fx-80";	/* see dfDevice */

char FaceName[] = "Proportional Expanded";	/* see dfFace */

FONTINFO FontInfo = {
	0x80,	/* dfType (0x0080 is device font) */
	16,	/* dfPoints (in 1/72") */
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
	20,	/* dfAvgWidth */
	20,	/* dfMaxWidth */
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
	20,	/* 0x21 */
	20,	/* 0x22 */
	24,	/* 0x23 */
	22,	/* 0x24 */
	24,	/* 0x25 */
	24,	/* 0x26 */
	10,	/* 0x27 */
	16,	/* 0x28 */
	16,	/* 0x29 */
	24,	/* 0x2a */
	24,	/* 0x2b */
	16,	/* 0x2c */
	24,	/* 0x2d */
	14,	/* 0x2e */
	20,	/* 0x2f */
	24,	/* 0x30 */
	18,	/* 0x31 */
	24,	/* 0x32 */
	24,	/* 0x33 */
	24,	/* 0x34 */
	24,	/* 0x35 */
	22,	/* 0x36 */
	24,	/* 0x37 */
	24,	/* 0x38 */
	22,	/* 0x39 */
	16,	/* 0x3a */
	18,	/* 0x3b */
	20,	/* 0x3c */
	22,	/* 0x3d */
	18,	/* 0x3e */
	22,	/* 0x3f */
	24,	/* 0x40 */
	24,	/* 0x41 */
	24,	/* 0x42 */
	24,	/* 0x43 */
	24,	/* 0x44 */
	24,	/* 0x45 */
	24,	/* 0x46 */
	24,	/* 0x47 */
	24,	/* 0x48 */
	20,	/* 0x49 */
	24,	/* 0x4a */
	24,	/* 0x4b */
	20,	/* 0x4c */
	24,	/* 0x4d */
	24,	/* 0x4e */
	24,	/* 0x4f */
	24,	/* 0x50 */
	24,	/* 0x51 */
	24,	/* 0x52 */
	24,	/* 0x53 */
	24,	/* 0x54 */
	24,	/* 0x55 */
	22,	/* 0x56 */
	24,	/* 0x57 */
	24,	/* 0x58 */
	24,	/* 0x59 */
	24,	/* 0x5a */
	22,	/* 0x5b */
	14,	/* 0x5c */
	22,	/* 0x5d */
	20,	/* 0x5e */
	24,	/* 0x5f */
	10,	/* 0x60 */
	22,	/* 0x61 */
	22,	/* 0x62 */
	22,	/* 0x63 */
	24,	/* 0x64 */
	22,	/* 0x65 */
	24,	/* 0x66 */
	22,	/* 0x67 */
	22,	/* 0x68 */
	18,	/* 0x69 */
	20,	/* 0x6a */
	22,	/* 0x6b */
	18,	/* 0x6c */
	22,	/* 0x6d */
	20,	/* 0x6e */
	22,	/* 0x6f */
	22,	/* 0x70 */
	22,	/* 0x71 */
	20,	/* 0x72 */
	22,	/* 0x73 */
	20,	/* 0x74 */
	22,	/* 0x75 */
	20,	/* 0x76 */
	24,	/* 0x77 */
	24,	/* 0x78 */
	22,	/* 0x79 */
	24,	/* 0x7a */
	20,	/* 0x7b */
	18,	/* 0x7c */
	20,	/* 0x7d */
	24,	/* 0x7e */
	0,	/* 0x7f */
};

unsigned char WidthFirstChar = 0x20;
unsigned char WidthLastChar = 0x7f;
