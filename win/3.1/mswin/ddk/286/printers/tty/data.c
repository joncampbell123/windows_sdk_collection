// DATA.C for TTY

// ------------------------------------------------------------------------
// Copyright (C) 1989-1990, Microsoft Corporation. All rights reserved.
// ------------------------------------------------------------------------
//	Microsoft History (latest first)
//	20 oct 89	peterbe		Checked in.
// ------------------------------------------------------------------------

#include "generic.h"

GDIINFO gBaseInfo = {
	0x30a	      ,   /* Version = 0300h			   */
	DT_CHARSTREAM ,   /* clasificacion de Device		 */
	MM_HSIZE_LET  ,	  /* taman~o Horizontal en milimetros    */
			  /* 8 pulgadas - "portrait"             */
	MM_VSIZE_LET  ,	  /* taman~o Vertical en milimetros      */
			  /* 11 pulgadas - "portrait"            */
	PG_ACROSS_LET ,	  /* ancho Horizontal en pixels          */
	PG_DOWN_LET   ,	  /* ancho Vertical en pixels            */
	1	      ,	  /* Numero de bits por pixel            */
	1	      ,	  /* Numero de planos                    */
	0	      ,	  /* Numero de brochas                   */
	0	      ,	  /* Numero de plumas                    */
	0	      ,	  /* Uso futuro                          */
	6	      ,	  /* Numero de fonts                     */
	2	      ,	  /* Numero de colores en la tabla de co.*/
	0	      ,	  /* Tam. requerido por device descriptor*/
	CC_NONE	      ,	  /* Capacidad de Curvas                 */
	LC_NONE	      ,	  /* Capacidad de Lineas                 */
	PC_NONE	      ,	  /* Capacidad "Polygonal"               */
	TC_EA_DOUBLE|
	TC_UA_ABLE    ,	  /* Raster font able                    */
	CP_NONE	      ,	  /* Capacidad de Clipping               */
	RC_GDI20_OUTPUT,	  /* Capacidad de Bitblt		 */
	XMAJOR	      ,	  /* Distance moving X only              */
	YMAJOR	      ,	  /* Distance moving Y only              */
	HYPOTENUSE    ,	  /* Distance moving X and Y             */
	MAXSTYLEERR   ,	  /* Longitud de segmento por estilos lin*/
	MM01	      ,	  /* HorzSize * 10                       */
	MM02	      ,	  /* VertSize * 10                       */
	MM03	      ,	  /* HorizRes                            */
	-MM04	      ,	  /* -VertRes                            */
	MM001	      ,	  /* HorzSize * 100                      */
	MM002	      ,	  /* VertSize * 100                      */
	MM003	      ,	  /* HorizRes                            */
	-MM004	      ,	  /* -VertRes                            */
	EnglishLo1    ,	  /* HorzSize * 1000                     */
	EnglishLo2    ,	  /* VertSize * 1000                     */
	EnglishLo3    ,	  /* HorizRes * 254                      */
	-EnglishLo4   ,	  /* -VertRes * 254                      */
	EnglishHi1    ,	  /* HorzSize * 10000                    */
	EnglishHi2    ,	  /* VertSize * 10000                    */
	EnglishHi3    ,	  /* HorizRes * 254                      */
	-EnglishHi4   ,	  /* -VertRes * 254                      */
	Twips1	      ,	  /* HorzSize * 14400                    */
	Twips2	      ,	  /* VertSize * 14400                    */
	Twips3	      ,	  /* HorizRes * 254                      */
	-Twips4	      ,	  /* -VertRes * 254                      */
	HDPI	      ,	  /* No. de logical pixels por pulgada H */
	VDPI	      ,	  /* No. de logical pixels por pulgada V */
	DC_SPDevice   ,	  /* especifica cuantos DContext por dev.*/
	0	      ,	  /* uso futuro 3                        */
	0	      ,	  /* uso futuro 4                        */
	0	      ,	  /* uso futuro 5                        */
	0	      ,	  /* uso futuro 6                        */
	0	      ,	  /* uso futuro 7                        */
	0	      ,	  /* No de regs de la paleta de Colores  */
	0	      ,	  /* No de regs de la paleta reservada   */
	0		  /* Resolucion de color                 */
};

// array of constants for different paper formats.

PAPERFORMAT PaperFormat[] = {
/* code,         X size,         Y size,        X size 10 mm,  Y size 10 mm,
	Form Length,    Physical Y offset*/
{DMPAPER_LETTER, PG_ACROSS_LET,	 PG_DOWN_LET   , MM_HSIZE_LET,	MM_VSIZE_LET,		PG_DOWN_LET,	0 },
{DMPAPER_LEGAL , PG_ACROSS_LEG,	 PG_DOWN_LEG   , MM_HSIZE_LEG,	MM_VSIZE_LEG,		PG_DOWN_LEG,	0 },
{DMPAPER_A4    , PG_ACROSS_A4 ,	 PG_DOWN_A4    , MM_HSIZE_A4 ,	MM_VSIZE_A4 ,		PG_DOWN_A4,	0 },
{DMPAPER_B5    , PG_ACROSS_B5 ,	 PG_DOWN_B5    , MM_HSIZE_B5 ,	MM_VSIZE_B5 ,		PG_DOWN_B5,	0 },
{DMPAPER_LETTER, PG_ACROSS_15 ,	 PG_DOWN_LET   , MM_HSIZE_15 ,	MM_VSIZE_LET,		PG_DOWN_LET,	0 },
{DMPAPER_LEGAL , PG_ACROSS_15 ,	 PG_DOWN_LEG   , MM_HSIZE_15 ,	MM_VSIZE_LEG,		PG_DOWN_LEG,	0 },
{DMPAPER_A4    , PG_ACROSS_15 ,	 PG_DOWN_A4    , MM_HSIZE_15 ,	MM_VSIZE_A4 ,		PG_DOWN_A4 ,	0 },
{DMPAPER_B5    , PG_ACROSS_15 ,	 PG_DOWN_B5    , MM_HSIZE_15 ,	MM_VSIZE_B5 ,		PG_DOWN_B5 ,	0 }
};

// ------------------------------------------------------------------------
// Translation table for characters >= A0H to ASCII substitutes.
// ------------------------------------------------------------------------
//	Microsoft History
//	22 oct 89	peterbe		Fixed thorn (DE, FE) = T, t.
//					extended defaultchars[] (need also
//					to define ANSISTART = 128.
//	20 oct 89	peterbe		Checked in.
//					D7, F7 multiply and divide updated.
// ------------------------------------------------------------------------

typedef unsigned char byte;

byte defaultchars[] = {

    (byte)'_', /* undefined      128  0x80 */
    (byte)'_', /* undefined      129  0x81 */
    (byte)'_', /* undefined      130  0x82 */
    (byte)'_', /* undefined      131  0x83 */
    (byte)'_', /* undefined      132  0x84 */
    (byte)'_', /* undefined      133  0x85 */
    (byte)'_', /* undefined      134  0x86 */
    (byte)'_', /* undefined      135  0x87 */
    (byte)'_', /* undefined      136  0x88 */
    (byte)'_', /* undefined      138  0x89 */
    (byte)'_', /* undefined      138  0x8A */
    (byte)'_', /* undefined      139  0x8B */
    (byte)'_', /* undefined      140  0x8C */
    (byte)'_', /* undefined      141  0x8D */
    (byte)'_', /* undefined      142  0x8E */
    (byte)'_', /* undefined      143  0x8F */

    (byte)'_', /* undefined      144  0x90 */
    (byte) 96, /* open sing. qt. 145  0x91 use grave accent*/
    (byte) 39, /* close sing qt. 146  0x92 use apostrophe */
    (byte)'_', /* undefined      147  0x93 */
    (byte)'_', /* undefined      148  0x94 */
    (byte)'_', /* undefined      149  0x95 */
    (byte)'_', /* undefined      150  0x96 */
    (byte)'_', /* undefined      151  0x97 */
    (byte)'_', /* undefined      152  0x98 */
    (byte)'_', /* undefined      153  0x99 */
    (byte)'_', /* undefined      154  0x9A */
    (byte)'_', /* undefined      155  0x9B */
    (byte)'_', /* undefined      156  0x9C */
    (byte)'_', /* undefined      157  0x9D */
    (byte)'_', /* undefined      158  0x9E */
    (byte)'_', /* undefined      159  0x9F */

    (byte)' ', /* ' '            160  0xA0 */
    (byte)'!', /* !              161  0xA1 */
    (byte)'c', /* c|             162  0xA2 */
    (byte)'#', /* #              163  0xA3 */
    (byte)'*', /*                164  0xA4 */
    (byte)'Y', /* Y=             165  0xA5 */
    (byte)'|', /* |              166  0xA6 */
    (byte)'S', /*                167  0xA7 */
    (byte)'"', /* dieresis       168  0xA8 */
    (byte)'c', /* (c)            169  0xA9 */
    (byte)'a', /* a_             170  0xAA */
    (byte)'<', /* <<             171  0xAB */
    (byte)'-', /* |-             172  0xAC */
    (byte)'-', /* -              173  0xAD */
    (byte)'R', /* (R)            174  0xAE */
    (byte)'-', /* upperline      175  0xAF */
    (byte)'.', /* bullet         176  0xB0 */
    (byte)'+', /* +-             177  0xB1 */
    (byte)'2', /* 2              178  0xB2 */
    (byte)'3', /* ^3             179  0xB3 */
    (byte)'\'',/* '		 180  0xB4 */
    (byte)'u', /* m              181  0xB5 */
    (byte)'q', /* q esp.         182  0xB6 */
    (byte)'.', /* .              183  0xB7 */
    (byte)',', /* ,              184  0xB8 */
    (byte)'1', /* ^1             185  0xB9 */
    (byte)'o', /* x_             186  0xBA */
    (byte)'>', /* >>             187  0xBB */
    (byte)'/', /* 1/4            188  0xBC */
    (byte)'/', /* 1/2            189  0xBD */
    (byte)'/', /* 3/4            190  0xBE */
    (byte)'?', /* ?              191  0xBF */
    (byte)'A', /* a mayuscula    192  0xC0 */
    (byte)'A', /* a' mayuscula   193  0xC1 */
    (byte)'A', /* a^ mayuscula   194  0xC2 */
    (byte)'A', /* A~mayuscula    195  0xC3 */
    (byte)'A', /* A mayuscula    196  0xC4 */
    (byte)'A', /* Aomayuscula    197  0xC5 */
    (byte)'A', /* AE mayuscula   198  0xC6 */
    (byte)'C', /* C mayuscula    199  0xC7 */
    (byte)'E', /* e` mayuscula   200  0xC8 */
    (byte)'E', /* e' mayuscula   201  0xC9 */
    (byte)'E', /* e^ mayuscula   202  0xCA */
    (byte)'E', /* e mayuscula    203  0xCB */
    (byte)'I', /* i` mayuscula   204  0xCC */
    (byte)'I', /* i' mayuscula   205  0xCD */
    (byte)'I', /* i^ mayuscula   206  0xCE */
    (byte)'I', /* i mayuscula    207  0xCF */
    (byte)'D', /* UC Eth         208  0xD0 */
    (byte)'N', /* N~ mayuscula   209  0xD1 */
    (byte)'O', /* o` mayuscula   210  0xD2 */
    (byte)'O', /* o' mayuscula   211  0xD3 */
    (byte)'O', /* o^ mayuscula   212  0xD4 */
    (byte)'O', /* O~mayuscula    213  0xD5 */
    (byte)'O', /* O mayuscula    214  0xD6 */
    (byte)'x', /* multiply       215  0xD7 */
    (byte)'0', /* O/             216  0xD8 */
    (byte)'U', /* u` mayuscula   217  0xD9 */
    (byte)'U', /* u' mayuscula   218  0xDA */
    (byte)'U', /* u^ mayuscula   219  0xDB */
    (byte)'U', /* u mayuscula    220  0xDC */
    (byte)'Y', /* Y'mayuscula    221  0xDD */
    (byte)'T', /* u.c. thorn     222  0xDE */
    (byte)'B', /* ss             223  0xDF */
    (byte)'a', /* a              224  0xE0 */
    (byte)'a', /* a'             225  0xE1 */
    (byte)'a', /* a^             226  0xE2 */
    (byte)'a', /* a~             227  0xE3 */
    (byte)'a', /* a              228  0xE4 */
    (byte)'a', /* ao             229  0xE5 */
    (byte)'a', /* ae             230  0xE6 */
    (byte)'c', /* c              231  0xE7 */
    (byte)'e', /* e`             232  0xE8 */
    (byte)'e', /* e'             233  0xE9 */
    (byte)'e', /* e^             234  0xEA */
    (byte)'e', /* e              235  0xEB */
    (byte)'i', /* i`             236  0xEC */
    (byte)'i', /* i'             237  0xED */
    (byte)'i', /* i^             238  0xEE */
    (byte)'i', /* i              239  0xEF */
    (byte)'d', /* lc Eth         240  0xF0 */
    (byte)'n', /* n~             241  0xF1 */
    (byte)'o', /* o`             242  0xF2 */
    (byte)'o', /* o'             243  0xF3 */
    (byte)'o', /* o^             244  0xF4 */
    (byte)'o', /* o~             245  0xF5 */
    (byte)'o', /* o              246  0xF6 */
    (byte)'/', /* divide         247  0xF7 */
    (byte)'0', /* ph             248  0xF8 */
    (byte)'u', /* u`             249  0xF9 */
    (byte)'u', /* u'             250  0xFA */
    (byte)'u', /* u^             251  0xFB */
    (byte)'u', /* u              252  0xFC */
    (byte)'y', /* y'             253  0xFD */
    (byte)'t', /* l.c. thorn     253  0xFE */
    (byte)'y', /* y              255  0xFF */
};

FONTINFO gFontInfo = {
    PF_BITS_IS_ADDRESS | PF_DEVICE_REALIZED,
    12,	    /* dfPoints (en 1/72") */
    6,	   /* dfVertRes */
    120,    /* dfHorizRes */
    1,	    /* dfAscent */
    0,	    /* dfInternalLeading */
    0,	    /* dfExternalLeading */
    0,	    /* dfItalic */
    0,	    /* dfUnderline */
    0,	    /* dfStrikeOut */
    400,    /* dfWeight */
    0,	    /* dfCharSet */
    12,	    /* dfPixWidth */
    1,	   /* dfPixHeight */
    0x30,   /* dfPitchAndFamily
             * el bit bajo es la bandera de variable "pitch"
             *    0 => FF_DONTCARE
             *    1 => FF_ROMAN
             *    2 => FF_SWISS
             *    3 => FF_MODERN
             *    4 => FF_SCRIPT
             *    5 => FF_DECORATIVE
             */
    12,	    /* dfAvgWidth */
    12,	    /* dfMaxWidth */
    0x20,   /* dfFirstChar */
    0xff,   /* dfLastChar */
    0x2e,   /* dfDefaultChar + dfFirstChar */
    0x20,   /* dfBreakChar + dfFirstChar */
    0,	    /* dfWidthBytes ( = 0 ) */
    0x41,   /* dfDevice - ver DeviceName */
    0x4d,   /* dfFace - ver FaceName */
    0x0,    /* dfBitsPointer ( = 0 ) */
    0x0,    /* dfBitsOffset ( = 0 ) */
};
