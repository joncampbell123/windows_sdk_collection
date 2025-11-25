/******************************Module*Header*******************************\
* Module Name: mfedit.c
*
* Main module for the Enhanced Metafile Editor
*       contains everything
*
* Created: 28-May-1992 14:24:00
* Author: Petrus Wong
*
* Copyright (c) 1992 Microsoft Corporation
*
* The Enhanced Metafile Editor serves to demonstrate the enhanced metafile
* APIs in Windows NT.
*
* The Editor provides the following functions:
*       1.  Playback and recording of GDI calls
*       2.  Embedding bitmap and enhanced metafile into another enhanced
*           metafile with transformation
*       3.  Hit-testing against enhanced metafile records
*       4.  Random access playback
*       5.  Playback metafile records one-by-one
*       6.  Selective recording of existing enhanced metafile records into
*           a new enhanced metafile
*       7.  drawing with pen, text, bezier, line, ellipse, rectangle and
*           embedding bitmap and enhanced metafile tools
*
* Dependencies:
*
*   metadef.h   - contains definition for enhanced metafile records
*
\**************************************************************************/
#include <stdlib.h>
#include "mfedit.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <commdlg.h>
#include <shellapi.h>
#include <math.h>


#if 0
//
// TEST!!!
//
HBITMAP hbmp;


typedef struct tagLOGPAL {
    WORD        palVersion;
    WORD        palNumEntries;
    PALETTEENTRY        palPalEntry[48];
} LOGPAL;

#define exp 17		// explicit entries on end
#define pb 6		// palette base

HPALETTE hPal, hOldPal;
LOGPAL LogPal =
{
	0x300, pb+16+9+exp,
	{
#if pb
           { 0,    0,    0,    PC_RESERVED  },
           { 0,    0,    0,    PC_RESERVED  },
           { 0,    0,    0,    PC_RESERVED  },
           { 0,    0,    0,    PC_RESERVED  },
           { 0,    0,    0,    PC_RESERVED  },
           { 0,    0,    0,    PC_RESERVED  },
#endif
           { 0,    0,    0,    PC_RESERVED  }, // black
           { 0,    0,    255,  PC_RESERVED  }, // blue
           { 255,  0,    0,    PC_RESERVED  }, // red
           { 0,    255,  0,    PC_RESERVED  }, // green
           { 255,  0,    255,  PC_RESERVED  }, // magenta
           { 0,    255,  255,  PC_RESERVED  }, // cyan
           { 255,  255,  0,    PC_RESERVED  }, // yellow
           { 192,  192,  192,  PC_RESERVED  }, // white
           { 96,   96,   96,   PC_RESERVED  }, // dark grey
           { 0,    0,    185,  PC_RESERVED  }, // blue
           { 185,  0,    0,    PC_RESERVED  }, // red
           { 0,    185,  0,    PC_RESERVED  }, // green
           { 185,  0,    185,  PC_RESERVED  }, // magenta
           { 0,    185,  185,  PC_RESERVED  }, // cyan
           { 185,  185,  0,    PC_RESERVED  }, // yellow
           { 255,  255,  255,  PC_RESERVED  }, // white
           { 0,    0,    0,    PC_RESERVED  },
           { 0,    0,    0,    PC_RESERVED  },
           { 0,    0,    0,    PC_RESERVED  },
           { 0,    0,    0,    PC_RESERVED  },
           { 0,    0,    0,    PC_RESERVED  },
           { 0,    0,    0,    PC_RESERVED  },
           { 0,    0,    0,    PC_RESERVED  },
           { 0,    0,    0,    PC_RESERVED  },
           { 0,    0,    0,    PC_RESERVED  },
           { 0,    0,    0,    PC_EXPLICIT  }, // physical palette index 0 (Windows black)
           { 1,    0,    0,    PC_EXPLICIT  }, // physical palette index 1
           { 2,    0,    0,    PC_EXPLICIT  }, // physical palette index 2
           { 3,    0,    0,    PC_EXPLICIT  }, // physical palette index 3
           { 4,    0,    0,    PC_EXPLICIT  }, // physical palette index 4
           { 5,    0,    0,    PC_EXPLICIT  }, // physical palette index 5
           { 6,    0,    0,    PC_EXPLICIT  }, // physical palette index 6
           { 7,    0,    0,    PC_EXPLICIT  }, // physical palette index 7
           { 8,    0,    0,    PC_EXPLICIT  }, // physical palette index 8
           { 9,    0,    0,    PC_EXPLICIT  }, // physical palette index 9
           { 10,   0,    0,    PC_EXPLICIT  }, // physical palette index 10
           { 11,   0,    0,    PC_EXPLICIT  }, // physical palette index 11
           { 12,   0,    0,    PC_EXPLICIT  }, // physical palette index 12
           { 13,   0,    0,    PC_EXPLICIT  }, // physical palette index 13
           { 14,   0,    0,    PC_EXPLICIT  }, // physical palette index 14
           { 15,   0,    0,    PC_EXPLICIT  }, // physical palette index 15
           { 255,  0,    0,    PC_EXPLICIT  }  // Windows white
	}
};

typedef struct BMAPINFO
{
	BITMAPINFOHEADER	bmiHeader;
	union
	{
		WORD			awColors[16];
		RGBQUAD			aRGBQuad[16];
	} u;
} BMAPINFO;

BMAPINFO DIBHdr =
{
	{
		sizeof(BITMAPINFOHEADER),	// biSize
		64,				// biWidth
		64,				// biHeight
		1,				// biPlanes
		4,				// biBitCount
		BI_RGB,				// biCompression
		2048,				// biSizeImage
		0,				// biXPelsPerMeter
		0,				// biYPelsPerMeter
		0,				// biClrUsed
		0				// biClrImportant
	},
	// Here the indices map 1 to 1 with our logical palette, they may change
	// depending on the index of the first color but they always follow in order:
	{{0+pb,1+pb,2+pb,3+pb,4+pb,5+pb,6+pb,7+pb,8+pb,9+pb,10+pb,11+pb,12+pb,13+pb,14+pb,15+pb}}
};

static BYTE dib[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
	0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
	0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
	0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
	0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
	0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
	0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
	0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
	0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
	0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
	0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
	0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
	0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
	0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
	0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
	0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
	0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
	0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
	0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
	0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
	0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
	0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
	0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
	0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB,
	0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB,
	0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB,
	0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB,
	0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB,
	0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB,
	0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB,
	0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB,
	0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
	0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
	0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
	0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
	0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
	0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
	0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
	0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
	0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
	0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
	0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
	0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
	0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
	0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
	0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
	0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
	0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
	0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
	0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
	0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
	0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
	0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
	0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
	0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};
#endif


//
// Forward declarations.
//
BOOL InitializeApp   (void);
LONG APIENTRY MainWndProc     (HWND, UINT, DWORD, LONG);
LONG APIENTRY DrawSurfWndProc (HWND, UINT, DWORD, LONG);
BOOL CALLBACK About           (HWND, UINT, DWORD, LONG);
LONG APIENTRY TextWndProc     (HWND, UINT, DWORD, LONG);
LONG APIENTRY CtrlPanelDlgProc(HWND, UINT, DWORD, LONG);
BOOL bDrawStuff      (HDC, INT, INT, INT, INT, BOOL, BOOL, BOOL, LPSTR);
HENHMETAFILE hemfLoadMetafile(HWND);
HDC  hDCRecordMetafileAs(HWND, LPSTR);
BOOL APIENTRY bPlayRecord(HDC, LPHANDLETABLE, LPENHMETARECORD, UINT, LPVOID);
BOOL APIENTRY bDoHitTest(HDC, LPHANDLETABLE, LPENHMETARECORD, UINT, LPVOID);
BOOL bHitTest(HDC, INT, INT);
HBITMAP hBmpLoadBitmapFile(HDC, PSTR);
BOOL bGetBMP(HWND, BOOL);
BOOL bChooseNewFont(HWND, PLOGFONT, COLORREF * );
BOOL bChooseNewColor(HWND, LPDWORD);
BOOL bPrintMf(PPRTDATA);
HBRUSH hBrCreateBrush(HDC, DWORD);
BOOL bSelectDIBPal(HDC, LPBITMAPINFO, BOOL);
BOOL bFreeDibFile(PDIBDATA);
BOOL bPlgBlt(HDC, LPPOINT);
HPALETTE CopyPalette(HPALETTE hPalSrc);
int CALLBACK iTT(LPLOGFONT, LPTEXTMETRIC, DWORD, LPARAM);
CMTMLTFMT *pLoadMltFmtFile(VOID);
HLOCAL Free(CMTMLTFMT *pMfmt);
BOOL bGetEPSBounds(LPVOID, RECTL *);
BOOL bIsAdobe(char *szStr);
BOOL bIsEPS(char *szStr);
BOOL bIsBndBox(char *szStr);
BOOL bIsEOF(char *szStr);
//BOOL bGetWord(LPVOID, char *, char **);
//BOOL bGoNextLine(LPVOID, char **);
BOOL bGetWord(LPVOID, char *, int*);
BOOL bGoNextLine(LPVOID, int*);

/***************************************************************************\
* WinMain
*
* History:
* 11-Feb-1992   Petrus Wong
\***************************************************************************/
int WINAPI WinMain(
#if 0
           HANDLE hInstance,
           HANDLE hPrevInstance,
#endif
           HINSTANCE hInstance,
           HINSTANCE hPrevInstance,
           LPSTR lpCmdLine,
           int nShowCmd)
{
    MSG    msg;
    HANDLE hAccel;

    ghModule = GetModuleHandle(NULL);
    if (!InitializeApp()) {
	MessageBox(ghwndMain, "MfEdit: InitializeApp failure!", "Error", MB_OK);
        return 0;
    }

    if (!(hAccel = LoadAccelerators (ghModule, MAKEINTRESOURCE(ACCEL_ID))))
	MessageBox(ghwndMain, "MfEdit: Load Accel failure!", "Error", MB_OK);


    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!TranslateAccelerator( ghwndMain, hAccel, &msg) ) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return 1;

    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
}


/***************************************************************************\
* InitializeApp
*
* History:
* 11-Feb-1992   Petrus Wong
*   Name changes.
* 09-09-91      Petrus Wong	Created.
\***************************************************************************/

BOOL InitializeApp(void)
{
    WNDCLASS wc;
    int index;
    HDC hDC;

    wc.style            = CS_DBLCLKS;
    wc.lpfnWndProc      = (WNDPROC)MainWndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra	= sizeof(DWORD);
    wc.hInstance        = ghModule;
    wc.hIcon            = LoadIcon(ghModule, MAKEINTRESOURCE(APP_ICON));
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground	= (HBRUSH)(COLOR_APPWORKSPACE + 1);
    wc.lpszMenuName     = "MainMenu";
    wc.lpszClassName	= "MetafDemoClass";

    if (!RegisterClass(&wc))
	return FALSE;

    wc.style            = CS_OWNDC | CS_SAVEBITS;
    wc.lpfnWndProc      = (WNDPROC)DrawSurfWndProc;
    wc.hIcon            = NULL;
    wc.hCursor          = NULL;
    wc.hbrBackground    = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "DrawSurfClass";

    if (!RegisterClass(&wc))
	return FALSE;

    wc.style		= CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc	= (WNDPROC)TextWndProc;
    wc.hIcon		= NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground	= (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszMenuName	= NULL;
    wc.lpszClassName	= "Text";

    if (!RegisterClass(&wc))
            return FALSE;



    hMenu	= LoadMenu(ghModule, "MainMenu");

    for (index = 0; index < OD_BTN_CNT; index++) {
        ghBmpDn[index] = (PVOID)LoadBitmap(ghModule, (LPCSTR)MAKEINTRESOURCE(BMID_BASED+index));
        ghBmpUp[index] = (PVOID)LoadBitmap(ghModule, (LPCSTR)MAKEINTRESOURCE(BMID_BASEU+index));
    }
    for (index = 0; index < OD_TOOL_CNT; index++) {
        ghToolBmpDn[index] = (PVOID)LoadBitmap(ghModule, (LPCSTR)MAKEINTRESOURCE(BMID_TOOLBASED+index));
        ghToolBmpUp[index] = (PVOID)LoadBitmap(ghModule, (LPCSTR)MAKEINTRESOURCE(BMID_TOOLBASEU+index));

    }

    ghwndMain = CreateWindowEx(0L, "MetafDemoClass", "Enhanced Metafile Editor",
	    WS_OVERLAPPED   | WS_CAPTION     | WS_BORDER       |
	    WS_THICKFRAME   | WS_MAXIMIZEBOX | WS_MINIMIZEBOX  |
	    WS_CLIPCHILDREN | WS_VISIBLE     | WS_SYSMENU,
            80, 70, 600, 300,
	    NULL, hMenu, ghModule, NULL);

    if (ghwndMain == NULL)
	return FALSE;

    SetWindowLong(ghwndMain, GWL_USERDATA, 0L);
    ghwndNext = SetClipboardViewer(ghwndMain);

    if (gbFit2Wnd)
        CheckMenuItem(hMenu, MM_FIT2WND, MF_CHECKED);
    else
        CheckMenuItem(hMenu, MM_FIT2WND, MF_UNCHECKED);

    if (gbImport3X)
        CheckMenuItem(hMenu, MM_IMPORT_3X, MF_CHECKED);
    else
        CheckMenuItem(hMenu, MM_IMPORT_3X, MF_UNCHECKED);

    if (gbExport3X)
        CheckMenuItem(hMenu, MM_EXPORT_3X, MF_CHECKED);
    else
        CheckMenuItem(hMenu, MM_EXPORT_3X, MF_UNCHECKED);

    SetFocus(ghwndMain);    /* set initial focus */

    gDib.ulFiles = gDib.ulFrames = 0;
    hDC = GetDC(NULL);
    ghHT = CreateHalftonePalette(hDC);
    ReleaseDC(NULL, hDC);

    return TRUE;
}


/***************************************************************************\
* MainWndProc
*
* History:
* 11-Feb-1992   Petrus Wong
*   Name changes.  Added comments.
* 09-09-91      Petrus Wong	Created.
\***************************************************************************/

long APIENTRY MainWndProc(
    HWND hwnd,
    UINT message,
    DWORD wParam,
    LONG lParam)
{
    static int         iMetafCnt=0;
    static char        szFilename[256] = "c:\\metaf";
    static BOOL        bReset=FALSE;
    static char        szLoadedMetaf[256] = " ";

    switch (message) {

      case WM_CREATE: {

	SetWindowLong(hwnd, 0, (LONG)NULL);
        ghDCMem = CreateCompatibleDC(NULL);

        ghwndCtrlPanel = CreateDialog(ghModule, (LPCSTR)MAKEINTRESOURCE(DID_CTRLPANEL),
                     hwnd, (DLGPROC) CtrlPanelDlgProc);

        ghwndDrawSurf = CreateWindow("DrawSurfClass", NULL,
                                    WS_BORDER | WS_CHILD | WS_VISIBLE,
                                    0, 0, 0, 0,
                                    hwnd,
                                    NULL,
                                    ghModule,
                                    NULL);

        ghTextWnd = CreateWindow("Text", NULL,
                                WS_BORDER | SS_LEFT | WS_CHILD | WS_VISIBLE,
                                0, 0, 0, 0,
                                hwnd,
                                NULL,               //(HMENU) 2,
                                ghModule,
                                NULL);

        ghbrRed = CreateSolidBrush(RGB(255, 0, 0));
        ghbrAppBkgd = CreateSolidBrush(GetSysColor(COLOR_BACKGROUND));
        ghpnWide = CreatePen(PS_SOLID, 5, RGB(0, 0, 0));
        return 0L;
      }
      case WM_DRAWCLIPBOARD:
        if ((IsClipboardFormatAvailable(CF_METAFILEPICT)) ||
            (IsClipboardFormatAvailable(CF_ENHMETAFILE)) )
            EnableMenuItem(hMenu, MM_PASTE, MF_ENABLED);
        else
            EnableMenuItem(hMenu, MM_PASTE,  MF_GRAYED);

        if (ghwndNext)
            SendMessage(ghwndNext, message, wParam, lParam);
        return 0L;

      case WM_SIZE: {
          RECT        rc;
          LONG        lcyCtrlPanel, lcyDrawSurf;

          GetWindowRect(ghwndCtrlPanel, &rc);
          lcyCtrlPanel = rc.bottom-rc.top;
          lcyDrawSurf = HIWORD(lParam) - lcyCtrlPanel - glcyStatus;

          //
          // CR!! Alternatively, this window can be created with cy
          //      equals to cy of the screen and saving this call
          //      altogether.
          //
          MoveWindow(ghwndCtrlPanel,
                     0, 0, LOWORD(lParam), lcyCtrlPanel, TRUE);

          //
          // This ordering guarantees the text window paints correctly
          //
          MoveWindow(ghTextWnd,
                     0, lcyCtrlPanel + lcyDrawSurf,
                     LOWORD(lParam),                    // cx of hwnd
                     glcyStatus, TRUE);

          MoveWindow(ghwndDrawSurf,
                     0, lcyCtrlPanel,
                     LOWORD(lParam),                    // cx of hwnd
                     lcyDrawSurf, TRUE);
          //break;
          return DefWindowProc(hwnd, message, wParam, lParam);
      }

      case WM_DESTROY: {
        DeleteDC(ghDCMem);
        DeleteEnhMetaFile(ghMetaf);
        DestroyWindow(ghwndCtrlPanel);
        DeleteObject(ghbrRed);
        DeleteObject(ghbrCur);
        DeleteObject(ghpnCur);
        DeleteObject(ghbrAppBkgd);
        DeleteObject(ghpnWide);
        if (ghHT)
            DeleteObject(ghHT);
        ChangeClipboardChain(ghwndMain, ghwndNext);
        bFreeDibFile(&gDib);
	PostQuitMessage(0);
	return 0L;
      }

      case WM_COMMAND: {
        static int     iPlus=0;


	switch (LOWORD(wParam)) {
            case DID_ZERO:
            case DID_ONE:
            case DID_TWO:
            case DID_THREE:
            case DID_FOUR:
            case DID_FIVE:
            case DID_SIX:
            case DID_SEVEN:
            case DID_EIGHT:
            case DID_NINE: {
                HDC           hDCDrawSurf;
                ENHMETAHEADER EnhMetaHdr;
                RECT          rcClientDS;
                int           iRecord;
                PLAYINFO      PlayInfo;

                if (ghMetaf == 0)
                    return 0L;

                GetEnhMetaFileHeader(ghMetaf, sizeof(EnhMetaHdr), &EnhMetaHdr);
                iRecord = LOWORD(wParam) - DID_ZERO + iPlus;
                SetDlgItemInt(ghwndCtrlPanel, DID_COUNTER, iRecord, FALSE);
                PlayInfo.iRecord = iRecord;
                PlayInfo.bPlayContinuous = FALSE;
                iPlus = 0;

                if ((EnhMetaHdr.nRecords > 1) && (iRecord > 0) &&
                    (iRecord <= (INT) EnhMetaHdr.nRecords)) {
                    hDCDrawSurf = GetDC(ghwndDrawSurf);
                    if (gbFit2Wnd) {
                        GetClientRect(ghwndDrawSurf, &rcClientDS);
                        EnumEnhMetaFile(hDCDrawSurf, ghMetaf, (ENHMFENUMPROC)bPlayRecord, (LPVOID) &PlayInfo, &rcClientDS);
                    } else {
                        EnumEnhMetaFile(hDCDrawSurf, ghMetaf, (ENHMFENUMPROC)bPlayRecord, (LPVOID) &PlayInfo, (LPRECT)&EnhMetaHdr.rclBounds);
                    }
                    //
                    // Enabling the user to record a metafile record selectively
                    //
                    if ((gbRecording) && (ghDCMetaf != NULL)) {
                        EnumEnhMetaFile(ghDCMetaf, ghMetaf, (ENHMFENUMPROC)bPlayRecord, (LPVOID) &PlayInfo, (LPRECT)&EnhMetaHdr.rclBounds);
                    }
                    ReleaseDC(ghwndDrawSurf, hDCDrawSurf);
                }
                return 0L;
            }
            case DID_TEN_PLUS: {
                if (ghMetaf == 0)
                    return 0L;

                iPlus += 10;
                SetDlgItemInt(ghwndCtrlPanel, DID_COUNTER, iPlus, FALSE);
                return 0L;
            }
            case MM_PAGESETUP:
            case MM_CUT:
                return 0L;

            case MM_COPY:   {

                if ((ghMetaf == 0) && (ghmf == 0)) {
                    SetWindowText(ghTextWnd, "No Metafile for copying");
                    return 0L;
                }

                OpenClipboard(ghwndMain);
                EmptyClipboard();

                if (gbExport3X)
                {
                    HGLOBAL          hmem;
                    LPMETAFILEPICT  lpmfp;
                    RECT            rcClientDS;
                    DWORD           x, y, mm;
                    HDC             hDCDrawSurf;
                    LPBYTE          pjData;
                    UINT            uiSize;

                    hDCDrawSurf = GetDC(ghwndDrawSurf);

                    if (ghmf == 0) {
                        SetWindowText(ghTextWnd, "Converting Enhanced Metafile to 3X format");

                        if (!(uiSize = GetWinMetaFileBits(ghMetaf, 0, NULL, MM_ANISOTROPIC, hDCDrawSurf))) {
                            MessageBox(ghwndMain, "Fail in 1st GetWinMetaFileBits!", "Error", MB_OK);
                            goto COPY_3X_EXIT;
                        }

                        if ((pjData = (LPBYTE) LocalAlloc(LMEM_FIXED, uiSize)) == NULL) {
                            MessageBox(ghwndMain, "Fail in Memory Allocation!", "Error", MB_OK);
                            goto COPY_3X_EXIT;
                        }

                        if (!(uiSize = GetWinMetaFileBits(ghMetaf, uiSize, pjData, MM_ANISOTROPIC, hDCDrawSurf))) {
                            MessageBox(ghwndMain, "Fail in 2nd GetWinMetaFileBits!", "Error", MB_OK);
                            LocalFree(pjData);
                            goto COPY_3X_EXIT;
                        }

                        ghmf = SetMetaFileBitsEx(uiSize, (LPBYTE) pjData);
                        LocalFree(pjData);
                    }

                    if ((hmem = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE,
                                        sizeof(METAFILEPICT))) == NULL) {

                        SetWindowText(ghTextWnd, "Failed in allocating memory");
                        goto COPY_3X_EXIT;

                    }

                    lpmfp = (LPMETAFILEPICT)GlobalLock(hmem);
                    lpmfp->mm = mm = MM_ANISOTROPIC;

                    GetClientRect(ghwndDrawSurf, &rcClientDS);
                    x = rcClientDS.right - rcClientDS.left;
                    x *= 2540;
                    x /= GetDeviceCaps(hDCDrawSurf, LOGPIXELSX);
                    lpmfp->xExt = x;                                // ie. in 0.01mm

                    y = rcClientDS.bottom - rcClientDS.top;
                    y *= 2540;
                    y /= GetDeviceCaps(hDCDrawSurf, LOGPIXELSY);
                    lpmfp->yExt = y;                                // ie. in 0.01mm

                    lpmfp->hMF = CopyMetaFile(ghmf, NULL);

                    GlobalUnlock(hmem);
                    SetWindowText(ghTextWnd, "Copying 3X Metafile to Clipboard");
                    SetClipboardData(CF_METAFILEPICT, hmem);

COPY_3X_EXIT:
                    ReleaseDC(ghwndDrawSurf, hDCDrawSurf);
                    goto COPY_EXIT;

                }

                //
                // gbExport3X == FALSE
                //
                if (ghMetaf == 0)           // requires conversion
                {
                    UINT            uiSize;
                    LPVOID          pvData;
                    HDC             hDCDrawSurf;

                    SetWindowText(ghTextWnd, "Converting 3X Metafile to Enhanced Metafile format");
                    if (!(uiSize = GetMetaFileBitsEx(ghmf, 0, NULL))) {
                        MessageBox(ghwndMain, "Fail in 1st GetMetaFileBitsEx!", "Error", MB_OK);
                        SetWindowText(ghTextWnd, "Conversion Failed");
                        goto COPY_EXIT;
                    }

                    if ((pvData = (LPVOID) LocalAlloc(LMEM_FIXED, uiSize)) == NULL) {
                        MessageBox(ghwndMain, "Fail in Memory Allocation!", "Error", MB_OK);
                        SetWindowText(ghTextWnd, "Conversion Failed");
                        goto COPY_EXIT;
                    }

                    if (!(uiSize = GetMetaFileBitsEx(ghmf, uiSize, pvData))) {
                        MessageBox(ghwndMain, "Fail in 2nd GetMetaFileBitsEx!", "Error", MB_OK);
                        goto COPY_ENH_EXIT;
                    }

                    hDCDrawSurf = GetDC(ghwndDrawSurf);

                    // !!! provide the correct picture extents in the METAFILEPICT structure
                    // where possible
                    ghMetaf = SetWinMetaFileBits(uiSize, (LPBYTE)pvData, hDCDrawSurf, NULL);

COPY_ENH_EXIT:
                    LocalFree(pvData);
                    ReleaseDC(ghwndDrawSurf ,hDCDrawSurf);

                    if (ghMetaf == 0) {
                        SetWindowText(ghTextWnd, "Conversion Failed");
                        goto COPY_EXIT;
                    }

                }

                //
                // No Conversion required
                //
                {

                    HENHMETAFILE hEmfTmp;

                    hEmfTmp = CopyEnhMetaFile(ghMetaf, NULL);

                    if (hEmfTmp) {
                        SetWindowText(ghTextWnd, "Copying Enhanced Metafile to Clipboard");
                        SetClipboardData(CF_ENHMETAFILE, hEmfTmp);
                        DeleteEnhMetaFile(hEmfTmp);
                    }

                }

COPY_EXIT:

                CloseClipboard();
                return 0L;
            }

            case MM_PASTE:  {
                OpenClipboard(ghwndMain);

                if (gbImport3X)
                {
                    HANDLE      hmem;
                    DWORD       dwXSugExt, dwYSugExt, dwMM;
                    HDC         hDCDrawSurf;
                    RECT        rc;
                    INT         iSavedDC;


                    hmem = GetClipboardData(CF_METAFILEPICT);

                    if (hmem)
                    {
                        LPMETAFILEPICT lpmfp;

                        SetWindowText(ghTextWnd, "Pasting 3X Metafile");
                        lpmfp = (LPMETAFILEPICT)GlobalLock(hmem);
                        ghmf  = lpmfp->hMF;
                        dwMM  = lpmfp->mm;
                        dwXSugExt = lpmfp->xExt;        // in 0.01 mm
                        dwYSugExt = lpmfp->yExt;
                        GlobalUnlock(hmem);

                        hDCDrawSurf = GetDC(ghwndDrawSurf);

                        iSavedDC = SaveDC(hDCDrawSurf);

                        GetClientRect(ghwndDrawSurf, &rc);

                        SetMapMode(hDCDrawSurf, dwMM);
                        if ((dwXSugExt > 0 )&& (dwYSugExt > 0))
                        {                               // suggested width & height of image
                            DWORD x;
                            DWORD y;

                            // no. of pixels in x and y
                            x = dwXSugExt;
                            x *= GetDeviceCaps(hDCDrawSurf,LOGPIXELSX);
                            x /= 2540;

                            y = dwYSugExt;
                            y *= GetDeviceCaps(hDCDrawSurf,LOGPIXELSY);
                            y /= 2540;

                            SetWindowExtEx(hDCDrawSurf, x, y, NULL);

                            if (gbFit2Wnd)
                                SetViewportExtEx(hDCDrawSurf, rc.right, rc.bottom, NULL);
                            else
                                SetViewportExtEx(hDCDrawSurf, x, y, NULL);

                        } else {
                            SetWindowText(ghTextWnd, "No information on 3X Metafile's extensions");
                            SetWindowExtEx(hDCDrawSurf, rc.right, rc.bottom, NULL);
                            SetViewportExtEx(hDCDrawSurf, rc.right, rc.bottom, NULL);
                        }

                        SetViewportOrgEx(hDCDrawSurf, 0, 0, NULL);
                        SetWindowOrgEx(hDCDrawSurf, 0, 0, NULL);

                        SetBoundsRect(hDCDrawSurf, NULL, DCB_ENABLE | DCB_SET);
                        PlayMetaFile(hDCDrawSurf, ghmf);
                        {
                        UINT    uiRC;
                        char    text[128];

                        wsprintf(text, "dwMM = %d\n", dwMM);
                        OutputDebugString(text);
                        wsprintf(text, "dwXSugExt = %d\n", dwXSugExt);
                        OutputDebugString(text);
                        wsprintf(text, "dwYSugExt = %d\n", dwYSugExt);
                        OutputDebugString(text);

                        uiRC = GetBoundsRect(hDCDrawSurf, &rc, DCB_RESET); // in logical coordinates
                        wsprintf(text, "GetBoundsRect = %d\n", uiRC);
                        OutputDebugString(text);
                        wsprintf(text, "left     = %d\n", rc.left);
                        OutputDebugString(text);
                        wsprintf(text, "right    = %d\n", rc.right);
                        OutputDebugString(text);
                        wsprintf(text, "top      = %d\n", rc.top);
                        OutputDebugString(text);
                        wsprintf(text, "bottom   = %d\n", rc.bottom);
                        OutputDebugString(text);
                        }

// !!!
// saving the wmf as an Aldus mf
//
{
OPENFILENAME    ofn;
char            szFile[256], szFileTitle[256];
static char     *szFilter;
UINT            uiSize;
HANDLE          hFile, hMapFile;
LPVOID          pMapFile;
DWORD           dwHigh, dwLow;

szFilter =
  "EnhMeta files Windows Metafiles (*.wmf)\0*.wmf\0\0";

strcpy(szFile, "*.wmf\0");
ofn.lStructSize = sizeof(OPENFILENAME);
ofn.hwndOwner = hwnd;
ofn.lpstrFilter = szFilter;
ofn.lpstrCustomFilter = (LPSTR) NULL;
ofn.nMaxCustFilter = 0L;
ofn.nFilterIndex = 1;
ofn.lpstrFile = szFile;
ofn.nMaxFile = sizeof(szFile);
ofn.lpstrFileTitle = szFileTitle;
ofn.nMaxFileTitle = sizeof(szFileTitle);
ofn.lpstrInitialDir = NULL;
ofn.lpstrTitle = "Save Metafile";
ofn.Flags = 0L;
ofn.nFileOffset = 0;
ofn.nFileExtension = 0;
ofn.lpstrDefExt = "WMF";

if (!GetOpenFileName(&ofn))
    return 0L;

uiSize = GetMetaFileBitsEx(ghmf, 0, NULL);
dwHigh = 0;
dwLow  = uiSize;

if ((hFile = CreateFile(szFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) == (HANDLE)-1) {
    MessageBox(ghwndMain, "Fail in file open!", "Error", MB_OK);
    return 0L;
}

//
// Create a map file of the opened file
//
if ((hMapFile = CreateFileMapping(hFile, NULL,
                         PAGE_READWRITE, dwHigh, dwLow, "MapF")) == NULL) {
    MessageBox(ghwndMain, "Fail in creating map file!", "Error", MB_OK);
    goto ErrorExit1;
}

//
// Map a view of the whole file
//
if ((pMapFile = MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, uiSize)) == NULL) {
    MessageBox(ghwndMain, "Fail in mapping view of the Map File object!", "Error", MB_OK);
    goto ErrorExit2;
}

if (uiSize) {
    APMFILEHEADER   AldHdr;
    PAPMFILEHEADER  pAldHdr;
    PBYTE           pjTmp;
    INT             iSize;
    char            text[128];

    AldHdr.key = ALDUS_ID;
    AldHdr.hmf = 0;                                 // Unused; must be zero
    AldHdr.bbox.Left   = 0;                         // in metafile units
    AldHdr.bbox.Top    = 0;
    //AldHdr.bbox.Right  = rc.right - rc.left;        // in logical coordinates
    //AldHdr.bbox.Bottom = rc.bottom - rc.top;

    switch (dwMM) {
        case MM_HIENGLISH:
            AldHdr.inch = 1000;
            AldHdr.bbox.Right  = (SHORT)dwXSugExt;
            AldHdr.bbox.Bottom = (SHORT)dwYSugExt;
            break;
        case MM_HIMETRIC:
            AldHdr.inch = 1440;
            AldHdr.bbox.Right  = (SHORT)(dwXSugExt / 2540 * 1440);
            AldHdr.bbox.Bottom = (SHORT)(dwYSugExt / 2540 * 1440);
            break;
        case MM_LOENGLISH:
            AldHdr.inch = 100;
            AldHdr.bbox.Right  = (SHORT)dwXSugExt;
            AldHdr.bbox.Bottom = (SHORT)dwYSugExt;
            break;
        case MM_LOMETRIC:
            AldHdr.inch = 254;
            AldHdr.bbox.Right  = (SHORT)dwXSugExt;
            AldHdr.bbox.Bottom = (SHORT)dwYSugExt;
            break;
        case MM_TEXT:
            AldHdr.inch = (WORD) (GetDeviceCaps(hDCDrawSurf, HORZRES) * 25.4 /
                          GetDeviceCaps(hDCDrawSurf, HORZSIZE));
            AldHdr.bbox.Right  = (SHORT)dwXSugExt;
            AldHdr.bbox.Bottom = (SHORT)dwYSugExt;
            break;
        case MM_TWIPS:
            AldHdr.inch = 1440;
            AldHdr.bbox.Right  = (SHORT)dwXSugExt;
            AldHdr.bbox.Bottom = (SHORT)dwYSugExt;
            break;
        default:
            AldHdr.inch = 1440;
            AldHdr.bbox.Right  = (SHORT)(dwXSugExt / 2540 * 1440);
            AldHdr.bbox.Bottom = (SHORT)(dwYSugExt / 2540 * 1440);
            break;
    }

    wsprintf(text, "MM           = %d\n", dwMM);
    OutputDebugString(text);
    wsprintf(text, "AldHdr.inch  = %d\n", AldHdr.inch);
    OutputDebugString(text);

    AldHdr.reserved = 0;
    AldHdr.checksum = 0;
    {
    WORD    *p;

    for (p = (WORD *)&AldHdr, AldHdr.checksum = 0;
            p < (WORD *)&(AldHdr.checksum); ++p)
        AldHdr.checksum ^= *p;
    }

    pAldHdr = &AldHdr;
    pjTmp = (PBYTE)pMapFile;

    iSize = 22;

    //!!! use memcpy...
    while (iSize--) {
        *(((PBYTE)pjTmp)++) = *(((PBYTE)pAldHdr)++);
    }

    pMapFile = (PBYTE)pMapFile + 22;
    GetMetaFileBitsEx(ghmf, uiSize, pMapFile);
}


UnmapViewOfFile(pMapFile);

ErrorExit2:
  CloseHandle(hMapFile);
ErrorExit1:
  CloseHandle(hFile);
}

                        RestoreDC(hDCDrawSurf, iSavedDC);
                        ReleaseDC(ghwndDrawSurf, hDCDrawSurf);

                    } else {
                        SetWindowText(ghTextWnd, "Cannot get 3X metafile from clipboard!");
                    }

                    goto PASTE_EXIT;

                }

                //
                // gbImport3X == FALSE
                //
                {
                    HENHMETAFILE hEmfTmp;
                    ENHMETAHEADER EnhMetaHdr;

                    hEmfTmp = GetClipboardData(CF_ENHMETAFILE);
                    if (hEmfTmp) {
                        SetWindowText(ghTextWnd, "Pasting Enhanced Metafile");
                        DeleteEnhMetaFile(ghMetaf);
                        ghMetaf = CopyEnhMetaFile(hEmfTmp, NULL);
                        DeleteEnhMetaFile(hEmfTmp);
                        GetEnhMetaFileHeader(ghMetaf, sizeof(EnhMetaHdr), &EnhMetaHdr);
                        SetDlgItemInt(ghwndCtrlPanel, DID_COUNTER, EnhMetaHdr.nRecords, FALSE);
                        bReset = TRUE;
                    } else {
                        SetWindowText(ghTextWnd, "Cannot get Enhanced metafile from clipboard!");
                    }
                }
PASTE_EXIT:

                CloseClipboard();
                EnableMenuItem(hMenu, MM_COPY,  MF_ENABLED);
                return 0L;
            }

            case MM_DEL:    {
                OpenClipboard(ghwndMain);
                EmptyClipboard();
                CloseClipboard();
                return 0L;
            }

            case MM_PEN: {
                HDC     hDC;
                DWORD   dwRGB;

                if (bChooseNewColor(hwnd, &dwRGB)) {
                    hDC = GetDC(ghwndDrawSurf);
                    if (ghpnCur != NULL)
                        DeleteObject(ghpnCur);
                    ghpnCur = CreatePen(PS_SOLID, 1, dwRGB);
                    SelectObject(hDC, ghpnCur);
                    if (ghDCMetaf != NULL)
                        SelectObject(ghDCMetaf, ghpnCur);
                    ReleaseDC(ghwndDrawSurf, hDC);
                }
                return 0L;
            }
            case MM_BRUSH: {
                HDC     hDC;
                static DWORD   dwRGB=RGB(255, 255, 255);

                if (bChooseNewColor(hwnd, &dwRGB)) {
                    hDC = GetDC(ghwndDrawSurf);
                    if (ghbrCur != NULL)
                        DeleteObject(ghbrCur);
                    ghbrCur = hBrCreateBrush(hDC, dwRGB);
                    SelectObject(hDC, ghbrCur);
                    if (ghDCMetaf != NULL)
                        SelectObject(ghDCMetaf, ghbrCur);
                    ReleaseDC(ghwndDrawSurf, hDC);
                }
                return 0L;
            }
            case MM_FONT: {
                HDC     hDC;
                char    text[128];

                if (bChooseNewFont(ghwndMain, &glf, &gCrText)) {
                    ghCurFont = CreateFontIndirect(&glf);

                    hDC = GetDC(ghwndDrawSurf);
                    EnumFonts(hDC, glf.lfFaceName, (FONTENUMPROC)iTT, (LPARAM)&gbTT);
                    wsprintf(text, "gbTT = %d\n", gbTT);
                    //OutputDebugString(text);
                    ReleaseDC(ghwndDrawSurf, hDC);

                    if (ghDCMetaf != NULL)
                        SelectObject(ghDCMetaf, ghCurFont);
                }
                return 0L;
            }

            case MM_TTOUTLN_STROKEFILL: {
                gbSFOutln = (gbSFOutln ? FALSE : TRUE);
                if (gbSFOutln) {
                    CheckMenuItem(hMenu, MM_TTOUTLN_STROKEFILL, MF_CHECKED);
                    CheckMenuItem(hMenu, MM_TTOUTLN_POLYDRAW, MF_UNCHECKED);
                    gbPDOutln = FALSE;
                }
                return 0L;
            }

            case MM_TTOUTLN_POLYDRAW: {
                gbPDOutln = (gbPDOutln ? FALSE : TRUE);
                if (gbPDOutln) {
                    CheckMenuItem(hMenu, MM_TTOUTLN_STROKEFILL, MF_UNCHECKED);
                    CheckMenuItem(hMenu, MM_TTOUTLN_POLYDRAW, MF_CHECKED);
                    gbSFOutln = FALSE;
                }
                return 0L;
            }

            case MM_C_WND_MF:
                return 0L;
            case MM_C_BEGIN_GP:
            case MM_C_END_GP:
                return 0L;
            case MM_C_MLTFMTS: {
                CMTMLTFMT *pMfmt;

                if ((gbRecording) && (ghDCMetaf != NULL)) {
                    if ((pMfmt = pLoadMltFmtFile()) != NULL) {
                        GdiComment(ghDCMetaf,
                                   sizeof(CMTMLTFMT)+pMfmt->aemrformat[0].cbData,
                                   (CONST BYTE *) pMfmt);
                        Free(pMfmt);
                    }
                    else
                        MessageBox(ghwndMain,
                                   "Fail to load Multiformat file!",
                                   "Error",
                                   MB_OK);
                }

                return 0L;
            }
            case MM_HITTEST: {
                static BOOL bHitTest=FALSE;
                HWND        hwndRecBtn;

                bHitTest = (bHitTest ? FALSE : TRUE);
                hwndRecBtn = GetDlgItem(ghwndCtrlPanel, DID_RECORD);
                if (bHitTest) {
                    CheckMenuItem(hMenu, MM_HITTEST, MF_CHECKED);
                    EnableMenuItem(hMenu, MM_RECORD, MF_GRAYED);
                    EnableWindow(hwndRecBtn, FALSE);
                    gbHitTest = TRUE;
                } else {
                    CheckMenuItem(hMenu, MM_HITTEST, MF_UNCHECKED);
                    EnableMenuItem(hMenu, MM_RECORD, MF_ENABLED);
                    EnableWindow(hwndRecBtn, TRUE);
                    gbHitTest = FALSE;
                    return 0L;
                }

                if (ghMetaf == 0) {
                    SetWindowText(ghTextWnd, "No Metafile loaded for hit-testing");
                    return 0L;
                }
                return 0L;
            }

	    case MM_LEABOUT:
		if (DialogBox(ghModule, (LPCSTR)"AboutBox", ghwndMain, (DLGPROC)About) == -1)
			MessageBox(ghwndMain, "DEMO: About Dialog Creation Error!", "Error", MB_OK);
                return 0L;

	    case MM_ABOUT:
		if (DialogBox(ghModule, "AboutBox", ghwndMain, (DLGPROC)About) == -1)
		   MessageBox(ghwndMain, "DEMO: About Dialog Creation Error!", "Error", MB_OK);
		return 0L;

            case MM_LOAD_MASKBMP:
                SetWindowText(ghTextWnd, "Load Mask Bitmap");
                bGetBMP(hwnd, TRUE);
                return 0L;

            case MM_LOAD_BMP:
                SetWindowText(ghTextWnd, "Load Bitmap");
                bGetBMP(hwnd, FALSE);
                return 0L;

            case MM_SAVE_BMP:
                SetWindowText(ghTextWnd, "Save Drawing Surface as Bitmap");
                return 0L;

            case MM_LOAD:
	    case DID_OPEN: {
                ENHMETAHEADER EnhMetaHdr;
                HENHMETAFILE  hEmfTmp;

                SetWindowText(ghTextWnd, "Load Metafile");
                //
                // If user hit cancel, we still have the original metafile
                //
                //DeleteEnhMetaFile(ghMetaf);
                //ghMetaf = hemfLoadMetafile(hwnd);
                hEmfTmp = hemfLoadMetafile(hwnd);
                if (hEmfTmp != 0) {
                    char     szDesc[256];

                    DeleteEnhMetaFile(ghMetaf);
                    ghMetaf = CopyEnhMetaFile(hEmfTmp, NULL);
                    GetEnhMetaFileHeader(ghMetaf, sizeof(EnhMetaHdr), &EnhMetaHdr);
                    SetDlgItemInt(ghwndCtrlPanel, DID_COUNTER, EnhMetaHdr.nRecords, FALSE);
                    DeleteEnhMetaFile(hEmfTmp);
                    EnableMenuItem(hMenu, MM_COPY,  MF_ENABLED);
                    if (GetEnhMetaFileDescription(ghMetaf, 256, szDesc) != 0) {
                        char    szText[256];
                        char    *szTmp, szSource[256];

                        szTmp = (char *)strtok(szDesc, "\\0");
                        strcpy(szSource, szTmp);
                        szTmp = (char *)strtok(NULL, "\\0\\0");
                        wsprintf(szText, "Source: %s  Title: %s", szSource, szTmp);
                        SetWindowText(ghTextWnd, szText);
                        strcpy(szLoadedMetaf, szTmp);
                    } else {
                        strcpy(szLoadedMetaf, "");
                    }
                //} else {
                //    SetDlgItemInt(ghwndCtrlPanel, DID_COUNTER, 0, FALSE);
                }
                bReset = TRUE;
                return 0L;
            }
            case MM_RECORD:
                if (gbHitTest) {
                    SetWindowText(ghTextWnd, "Please CANCEL Hit Testing Mode First!");
                    return 0L;
                }

                SetWindowText(ghTextWnd, "Recording...");
                if (!gbRecording) {
                    ghDCMetaf = hDCRecordMetafileAs(hwnd, szFilename);
                }

                if (ghDCMetaf == NULL) {
                   SetWindowText(ghTextWnd, "ERROR: Failed in creating the metafile DC!");
                   return 0L;
                }

                // Parse the szFilename for the title and GdiComment the metafile with it.
                {
                    char    szComment[256];
                    char    *szTmp, szTmp2[256];

                    szTmp = (char *)strtok(szFilename, "\\");
                    strcpy(szTmp2, szTmp);
                    while (szTmp != NULL) {
                        szTmp = (char *)strtok(NULL, "\\");
                        if (szTmp != NULL) {
                            strcpy(szTmp2, szTmp);
                        }
                    }
                    szTmp = (char *)strtok(szTmp2, ".");
                    wsprintf((LPSTR) szComment, "MfEdit:\\0%s\\0\\0", szTmp);
#if 0
                    if (!GdiComment(ghDCMetaf, 256, szComment)) {
                        MessageBox(ghwndMain, "Fail in adding comment!", "Error", MB_OK);
                    }
#endif
                }

                gbRecording = TRUE;

                if (ghpnCur != NULL)
                    SelectObject(ghDCMetaf, ghpnCur);

                if (ghbrCur != NULL)
                    SelectObject(ghDCMetaf, ghbrCur);

                if (ghCurFont != NULL)
                    SelectObject(ghDCMetaf, ghCurFont);
                return 0L;

	    case DID_RECORD: {
                int  iWidthMM, iHeightMM, iWidthPels, iHeightPels, iMMPerPelX, iMMPerPelY;
                char szComment[256];
                char szTitle[256];
                RECT rc;
                HDC  hDC;


                if (gbHitTest) {
                    SetWindowText(ghTextWnd, "Please CANCEL Hit Testing Mode First!");
                    return 0L;
                }

                SetWindowText(ghTextWnd, "Recording...");
                if (!gbRecording) {

                    hDC = GetDC(hwnd);
                    iWidthMM    = GetDeviceCaps(hDC, HORZSIZE);
                    iHeightMM   = GetDeviceCaps(hDC, VERTSIZE);
                    iWidthPels  = GetDeviceCaps(hDC, HORZRES);
                    iHeightPels = GetDeviceCaps(hDC, VERTRES);
                    ReleaseDC(hwnd, hDC);
                    iMMPerPelX  = (iWidthMM * 100)/iWidthPels;
                    iMMPerPelY  = (iHeightMM * 100)/iHeightPels;
                    GetClientRect(ghwndDrawSurf, &rc);
                    rc.left   = rc.left * iMMPerPelX;
                    rc.top    = rc.top * iMMPerPelY;
                    rc.right  = rc.right * iMMPerPelX;
                    rc.bottom = rc.bottom * iMMPerPelY;

                    {
                       char szFilenameWithExt[256];
                       char suffix[20];
                       char szDesc[256];
                       char *szTmp, szTmp2[256];

                       //
                       // assemble a new metafile name with the emf extension from
                       // the generic szFilename
                       //
                       wsprintf((LPSTR) suffix, "%d.emf", iMetafCnt);
                       iMetafCnt++;
                       strcpy(szFilenameWithExt, szFilename);
                       strcat(szFilenameWithExt, suffix);

                       //
                       // parse szFilename for the title for description
                       //
                       szTmp = (char *)strtok(szFilename, "\\");
                       strcpy(szTmp2, szTmp);
                       while (szTmp != NULL) {
                           szTmp = (char *)strtok(NULL, "\\");
                           if (szTmp != NULL) {
                               strcpy(szTmp2, szTmp);
                           }
                       }
                       szTmp = (char *)strtok(szTmp2, ".");
                       strcpy(szTitle, szTmp);
                       wsprintf(szDesc, "SDK Enhanced Metafile Editor\\0%s\\0\\0", szTitle);
                       ghDCMetaf = CreateEnhMetaFile((HDC)NULL, szFilenameWithExt, (LPRECT)&rc, (LPSTR)szDesc);
                    }

                    if ((SetGraphicsMode(ghDCMetaf, GM_ADVANCED)) == 0) {
                       MessageBox(ghwndMain, "Fail in setting Advanced Graphics Mode!", "Error", MB_OK);
                    }
                }

                if (ghDCMetaf == NULL) {
                   SetWindowText(ghTextWnd, "ERROR: Failed in creating the metafile DC!");
                   return 0L;
                }
                wsprintf((LPSTR) szComment, "MfEdit:\\0%s\\0\\0", szTitle);
#if 0
                if (!GdiComment(ghDCMetaf, 256, szComment)) {
                    MessageBox(ghwndMain, "Fail in adding comment!", "Error", MB_OK);
                }
#endif
                gbRecording = TRUE;

                if (ghpnCur != NULL)
                    SelectObject(ghDCMetaf, ghpnCur);

                if (ghbrCur != NULL)
                    SelectObject(ghDCMetaf, ghbrCur);

                if (ghCurFont != NULL)
                    SelectObject(ghDCMetaf, ghCurFont);

                return 0L;
            }
	    case DID_STOP:
                SetWindowText(ghTextWnd, "Stop");
                if (gbRecording) {
                    ghMetaf = CloseEnhMetaFile(ghDCMetaf);
                    gbRecording = FALSE;
                }
                return 0L;

	    case DID_PLAY: {
                HDC hDCDrawSurf;
                ENHMETAHEADER EnhMetaHdr;
                RECT          rcClientDS;
                int           iEntries;
                PLOGPALETTE     plogPal;
                PBYTE           pjTmp;
                HPALETTE        hPal;
                char            szTmp[256];

                wsprintf(szTmp, "Playing Metafile: %s", szLoadedMetaf);
                SetWindowText(ghTextWnd, szTmp);
                if (ghMetaf != NULL) {
                    hDCDrawSurf = GetDC(ghwndDrawSurf);
                    GetEnhMetaFileHeader(ghMetaf, sizeof(ENHMETAHEADER), &EnhMetaHdr);

                    iEntries = GetEnhMetaFilePaletteEntries(ghMetaf, 0, NULL);

                    if (iEntries) {
                        if ((plogPal = (PLOGPALETTE)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
                                sizeof(DWORD) + sizeof(PALETTEENTRY)*iEntries )) == NULL) {
                            MessageBox(ghwndMain, "Failed in Creating Palette!", "Error", MB_OK);
                        }

                        plogPal->palVersion = 0x300;
                        plogPal->palNumEntries = (WORD) iEntries;
                        pjTmp = (PBYTE) plogPal;
                        pjTmp += 8;

                        GetEnhMetaFilePaletteEntries(ghMetaf, iEntries, (PPALETTEENTRY)pjTmp);
                        hPal = CreatePalette(plogPal);
                        GlobalFree(plogPal);

                        SelectPalette(hDCDrawSurf, hPal, FALSE);
                        RealizePalette(hDCDrawSurf);
                    }


                    if (gbFit2Wnd) {
                        GetClientRect(ghwndDrawSurf, &rcClientDS);
                        if (!PlayEnhMetaFile( hDCDrawSurf, ghMetaf, (LPRECT) &rcClientDS)) {
                            char    text[128];

                            wsprintf(text, "Fail in PlayEnhMetaFile! Error %ld\n", GetLastError());
                            OutputDebugString(text);
                        }
                    } else {
                        RECT rc;

                        rc.top = rc.left = 0;
                        rc.right = EnhMetaHdr.rclBounds.right - EnhMetaHdr.rclBounds.left;
                        rc.bottom = EnhMetaHdr.rclBounds.bottom - EnhMetaHdr.rclBounds.top;
                        if (!PlayEnhMetaFile( hDCDrawSurf, ghMetaf, (LPRECT) &rc)) {
                            char    text[128];

                            wsprintf(text, "Fail in PlayEnhMetaFile! Error %ld\n", GetLastError());
                            OutputDebugString(text);
                        }

                    }
                    //
                    // Enabling the user to embed another metafile
                    //
                    if ((gbRecording) && (ghDCMetaf != NULL)) {
                        if (hPal != (HPALETTE)NULL) {
                            SelectPalette(ghDCMetaf, hPal, FALSE);
                            RealizePalette(ghDCMetaf);
                        }
                        {
                        RECT rc;

                        rc.top = rc.left = 0;
                        rc.right = EnhMetaHdr.rclBounds.right - EnhMetaHdr.rclBounds.left;
                        rc.bottom = EnhMetaHdr.rclBounds.bottom - EnhMetaHdr.rclBounds.top;
                        if (!PlayEnhMetaFile( ghDCMetaf, ghMetaf, (LPRECT) &rc)) {
                            char    text[128];

                            wsprintf(text, "Fail in PlayEnhMetaFile! Error %ld\n", GetLastError());
                            OutputDebugString(text);
                        }

                        }
                    }

                    ReleaseDC(ghwndDrawSurf, hDCDrawSurf);
                } else {
                    SetWindowText(ghTextWnd, "No Metafile for Playing");
                }

                return 0L;
            }
	    case DID_FF: {
                HDC           hDCDrawSurf;
                ENHMETAHEADER EnhMetaHdr;
                RECT          rcClientDS;
                static int    iRecord = 0;
                PLAYINFO      PlayInfo;
                int           iEntries;
                PLOGPALETTE     plogPal;
                PBYTE           pjTmp;
                HPALETTE        hPal;


                if (ghMetaf == 0)
                    return 0L;

                PlayInfo.iRecord = ++iRecord;
                PlayInfo.bPlayContinuous = TRUE;

                GetEnhMetaFileHeader(ghMetaf, sizeof(EnhMetaHdr), &EnhMetaHdr);
                SetDlgItemInt(ghwndCtrlPanel, DID_COUNTER, iRecord, FALSE);
                if ((EnhMetaHdr.nRecords > 1) && (iRecord <= (INT)EnhMetaHdr.nRecords)) {
                    hDCDrawSurf = GetDC(ghwndDrawSurf);

                    iEntries = GetEnhMetaFilePaletteEntries(ghMetaf, 0, NULL);

                    if (iEntries) {
                        if ((plogPal = (PLOGPALETTE)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
                                sizeof(DWORD) + sizeof(PALETTEENTRY)*iEntries )) == NULL) {
                            MessageBox(ghwndMain, "Failed in Creating Palette!", "Error", MB_OK);
                        }

                        plogPal->palVersion = 0x300;
                        plogPal->palNumEntries = (WORD) iEntries;
                        pjTmp = (PBYTE) plogPal;
                        pjTmp += 8;

                        GetEnhMetaFilePaletteEntries(ghMetaf, iEntries, (PPALETTEENTRY)pjTmp);
                        hPal = CreatePalette(plogPal);
                        GlobalFree(plogPal);

                        SelectPalette(hDCDrawSurf, hPal, FALSE);
                        RealizePalette(hDCDrawSurf);
                    }

                    if (gbFit2Wnd) {
                        GetClientRect(ghwndDrawSurf, &rcClientDS);
                        EnumEnhMetaFile(hDCDrawSurf, ghMetaf, (ENHMFENUMPROC)bPlayRecord, (LPVOID) &PlayInfo, (LPRECT) &rcClientDS);
                    } else {
                        EnumEnhMetaFile(hDCDrawSurf, ghMetaf, (ENHMFENUMPROC)bPlayRecord, (LPVOID) &PlayInfo, (LPRECT) &EnhMetaHdr.rclBounds);
                    }
                    //
                    // Enabling the user to record a metafile records selectively
                    //
                    if ((gbRecording) && (ghDCMetaf != NULL)) {
                        SelectPalette(ghDCMetaf, hPal, FALSE);
                        RealizePalette(ghDCMetaf);
                        EnumEnhMetaFile(ghDCMetaf, ghMetaf, (ENHMFENUMPROC)bPlayRecord, (LPVOID) &PlayInfo, (LPRECT)&EnhMetaHdr.rclBounds);
                    }

                    ReleaseDC(ghwndDrawSurf, hDCDrawSurf);
                }

                if ((iRecord == (INT) EnhMetaHdr.nRecords) || bReset) {
                    iRecord = 0;
                    if (bReset)
                        SetDlgItemInt(ghwndCtrlPanel, DID_COUNTER, 0, FALSE);
                    bReset = FALSE;
                }
                return 0L;
            }
	    case DID_CLEAR: {
                HDC     hDCDrawSurf;
                HGDIOBJ hObjOld;
                RECT    rcDrawSurf;

                SetWindowText(ghTextWnd, "Drawing Surface cleared");
                hDCDrawSurf = GetDC(ghwndDrawSurf);
                ghbrAppBkgd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
                hObjOld = SelectObject(hDCDrawSurf, ghbrAppBkgd);
                GetClientRect(ghwndDrawSurf, &rcDrawSurf);
                PatBlt(hDCDrawSurf, 0, 0, rcDrawSurf.right, rcDrawSurf.bottom, PATCOPY);
                ReleaseDC(ghwndDrawSurf, hDCDrawSurf);
                SelectObject(hDCDrawSurf, hObjOld);
                return 0L;
            }

            case DID_PEN:
                SetWindowText(ghTextWnd, "Pen");
                return 0L;
            case DID_TEXT:
                SetWindowText(ghTextWnd, "Text");
                return 0L;
            case DID_RECT:
                SetWindowText(ghTextWnd, "Rectangle");
                return 0L;
            case DID_FILLRECT:
                SetWindowText(ghTextWnd, "Filled Rectangle");
                return 0L;
            case DID_ELLIPSE:
                SetWindowText(ghTextWnd, "Ellipse");
                return 0L;
            case DID_FILLELLIPSE:
                SetWindowText(ghTextWnd, "Filled Ellipse");
                return 0L;
            case DID_LINE:
                SetWindowText(ghTextWnd, "Line");
                return 0L;
            case DID_BEZIER:
                SetWindowText(ghTextWnd,
                    "Bezier: Click with Left button for placing control points");
                return 0L;
            case DID_BMPOBJ:
                SetWindowText(ghTextWnd,
                    "Bitmap: Click three points for the destination of the bitmap");
                return 0L;
            case DID_METAF:
                SetWindowText(ghTextWnd,
                    "External Metafile: Click three points for the destination of the Metafile");
                return 0L;
            case MM_IMPORT_3X:
                gbImport3X = (gbImport3X ? FALSE : TRUE);

                if (gbImport3X)
                    CheckMenuItem(hMenu, MM_IMPORT_3X, MF_CHECKED);
                else
                    CheckMenuItem(hMenu, MM_IMPORT_3X, MF_UNCHECKED);
                return 0L;

            case MM_EXPORT_3X:
                gbExport3X = (gbExport3X ? FALSE : TRUE);

                if (gbExport3X)
                    CheckMenuItem(hMenu, MM_EXPORT_3X, MF_CHECKED);
                else
                    CheckMenuItem(hMenu, MM_EXPORT_3X, MF_UNCHECKED);
                return 0L;
            case MM_FIT2WND:
                gbFit2Wnd = (gbFit2Wnd ? FALSE : TRUE);

                if (gbFit2Wnd)
                    CheckMenuItem(hMenu, MM_FIT2WND, MF_CHECKED);
                else
                    CheckMenuItem(hMenu, MM_FIT2WND, MF_UNCHECKED);
                return 0L;

            case MM_PRINT: {
                DWORD   dwThrdID;
                HANDLE  hThrd;
                PPRTDATA pPrtData;

                if (ghMetaf == 0) {
                    SetWindowText(ghTextWnd, "NO Metafile to print");
                    return 0L;
                }

                //
                // bPrintMf is supposed to free up the memory when done.
                //
                if ((pPrtData = (PPRTDATA) GlobalAlloc(GPTR, sizeof(PRTDATA))) == NULL) {
                    SetWindowText(ghTextWnd, "Failed in allocating memory");
                    return 0L;
                }

                pPrtData->hMetaf = ghMetaf;
                pPrtData->bFit2Wnd = gbFit2Wnd;

                hThrd = CreateThread(NULL, 0,
                             (LPTHREAD_START_ROUTINE)bPrintMf,
                             pPrtData, STANDARD_RIGHTS_REQUIRED,
                             &dwThrdID);

                //
                // Free the memory if CreateThread fails...
                //
                if (hThrd == NULL) {
                    SetWindowText(ghTextWnd, "Failed in creating printing thread");
                    GlobalFree(pPrtData);
                }

                return 0L;

            }

	    default:
                return DefWindowProc(hwnd, message, wParam, lParam);
        }
      }     // WM_COMMAND
      default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
}

/******************************Public*Routine******************************\
*
* DrawSurfWndProc
*       Drawing surface window procedure
*
* Effects:  Trapping all mouse messages and call the DrawStuff appropriately
*           for drawing to the drawing surface DC and metafile DC as needed.
*
* Warnings:
*
* History:
*  30-Apr-1992 -by- Petrus Wong
* Wrote it.
\**************************************************************************/

long APIENTRY DrawSurfWndProc(
    HWND hwnd,
    UINT message,
    DWORD wParam,
    LONG lParam)
{
    static BOOL    bTrack = FALSE;
    static int     OrgX, OrgY;
    static int     PrevX, PrevY;
    static HDC     hDC;
    static HCURSOR hCurArrow, hCurHT;

    switch (message) {
      case WM_CREATE:
          {
              RECT       rect;

              hDC = GetDC(hwnd);
              if ((SetGraphicsMode(hDC, GM_ADVANCED)) == 0) {
                 MessageBox(ghwndMain, "Fail in setting Advanced Graphics Mode!", "Error", MB_OK);
              }
              ReleaseDC(hwnd, hDC);

              GetClientRect(GetParent(hwnd), &rect);

              SetWindowPos(hwnd, NULL,
                      0,
                      30,
                      rect.right-rect.left,
                      rect.bottom-rect.top-30,
                      SWP_NOZORDER | SWP_NOMOVE);

#if 0
//CreateCaret(hwnd, NULL, 1, 12);
//hbmp = LoadBitmap(ghModule, (LPCSTR)MAKEINTRESOURCE(BMID_TOOLBASED));

hPal = CreatePalette((LOGPALETTE *)&LogPal);
hDC = GetDC(hwnd);
hOldPal = SelectPalette(hDC, hPal, FALSE);
RealizePalette(hDC);

hbmp = CreateDIBitmap(hDC, (LPBITMAPINFOHEADER)&DIBHdr, CBM_CREATEDIB, &dib, (LPBITMAPINFO)&DIBHdr, DIB_PAL_COLORS);
CreateCaret(hwnd, hbmp, 1, 12);

ReleaseDC(hwnd, hDC);
#endif

              ghCurFont = GetStockObject(SYSTEM_FONT);
              GetObject(ghCurFont, sizeof(LOGFONT), &glf);
              hCurArrow = LoadCursor(NULL, IDC_ARROW);
              hCurHT = LoadCursor(NULL, IDC_CROSS);
              break;
          }

      case WM_LBUTTONDOWN: {
        int    x, y;

        x = (int) LOWORD(lParam);
        y = (int) HIWORD(lParam);

        if (gbHitTest) {
            hDC = GetDC(hwnd);
            bHitTest(hDC, x, y);
            ReleaseDC(hwnd, hDC);
            break;
        }

        bTrack = TRUE;
        OrgX = PrevX = x;
        OrgY = PrevY = y;

        hDC = GetDC(hwnd);
        SetCapture(hwnd);
        break;
      }

      case WM_MOUSEMOVE: {
        RECT rectClient;
        int NextX;
        int NextY;

        if (gbHitTest) {
            SetCursor(hCurHT);
        } else {
            SetCursor(hCurArrow);
        }

        // Update the selection region
        if (bTrack) {
            NextX = (SHORT) LOWORD(lParam);
            NextY = (SHORT) HIWORD(lParam);

            // Do not draw outside the window's client area

            GetClientRect (hwnd, &rectClient);
            if (NextX < rectClient.left) {
                NextX = rectClient.left;
            } else if (NextX >= rectClient.right) {
                NextX = rectClient.right - 1;
            }
            if (NextY < rectClient.top) {
                NextY = rectClient.top;
            } else if (NextY >= rectClient.bottom) {
                NextY = rectClient.bottom - 1;
            }
            if ((NextX != PrevX) || (NextY != PrevY)) {
               SetROP2(hDC, R2_NOT);           // Erases the previous box
               bDrawStuff(hDC, OrgX, OrgY, PrevX, PrevY, TRUE, TRUE, FALSE, NULL);

               //
               // Optimization.  Do not record in metafile DC if it is going
               // to be erased.  So only call bDrawStuff with the PEN tool.
               //
               if (gbRecording && (ghDCMetaf != NULL) && (gdwCurTool == DID_PEN)) {
                   bDrawStuff(ghDCMetaf, OrgX, OrgY, PrevX, PrevY, TRUE, TRUE, FALSE, NULL);
               }


            // Get the current mouse position
               PrevX = NextX;
               PrevY = NextY;

               //
               // SetROP2(hDC, R2_COPYPEN);
               //   This is commented out because we don't want to erase
               //   the background as it sweeps.
               //
               bDrawStuff(hDC, OrgX, OrgY, PrevX, PrevY, FALSE, TRUE, FALSE, NULL);

               if (gbRecording && (ghDCMetaf != NULL) && (gdwCurTool == DID_PEN)) {
                   bDrawStuff(ghDCMetaf, OrgX, OrgY, PrevX, PrevY, FALSE, TRUE, FALSE, NULL);
               }

            }
        }
        break;

      }

      case WM_LBUTTONUP: {
        int NextX;
        int NextY;

        if (!bTrack)
           break;

        // End the selection
           ReleaseCapture();
           bTrack = FALSE;

        // Erases the box
           //
           // SetROP2(hDC, R2_NOT);
           //   This is assumed to be R2_NOT, thus unnecessary
           //
           bDrawStuff(hDC, OrgX, OrgY, PrevX, PrevY, TRUE, FALSE, FALSE, NULL);

           if (gbRecording && (ghDCMetaf != NULL) && (gdwCurTool == DID_PEN)) {
               bDrawStuff(ghDCMetaf, OrgX, OrgY, PrevX, PrevY, TRUE, FALSE, FALSE, NULL);
           }

           NextX = (SHORT) LOWORD(lParam);
           NextY = (SHORT) HIWORD(lParam);

        // Draws the new box
           SetROP2(hDC, R2_COPYPEN);
           bDrawStuff(hDC, OrgX, OrgY, NextX, NextY, FALSE, FALSE, TRUE, NULL);

           ReleaseDC(hwnd, hDC);

           if (gbRecording && (ghDCMetaf != NULL)) {
                bDrawStuff(ghDCMetaf, OrgX, OrgY, NextX, NextY, FALSE, FALSE, FALSE, NULL);
           }

        break;
      } // case WM_LBUTTONUP

      case WM_CHAR: {

        if (gdwCurTool != DID_TEXT)
            break;

        hDC = GetDC(hwnd);
        bDrawStuff(hDC, 0, 0, 0, 0, TRUE, FALSE, FALSE, (LPSTR)&wParam);
        ReleaseDC(hwnd, hDC);

        if (gbRecording && (ghDCMetaf != NULL)) {
            bDrawStuff(ghDCMetaf, 0, 0, 0, 0, TRUE, FALSE, FALSE, (LPSTR)&wParam);
        }

        break;
      }

      case WM_DESTROY: {
        DestroyCaret();
        DeleteObject(ghCurFont);
	PostQuitMessage(0);
	return 0L;
      }

      default:
	return DefWindowProc(hwnd, message, wParam, lParam);
    }
}


/***************************************************************************\
* About
*
* About dialog proc.
*
* History:
* 09-09-91 Petrus Wong	Rewrote.
* 04-13-91 ????         Created.
\***************************************************************************/

BOOL CALLBACK About (
    HWND hDlg,
    UINT message,
    DWORD wParam,
    LONG lParam)
{
    switch (message) {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        if (wParam == IDOK)
            EndDialog(hDlg, wParam);
        break;
    }

    return FALSE;

    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(hDlg);
}

/***************************************************************************\
*
* TextWndProc
*
* Text Window procedure for displaying miscellaneous messages to user.
*
* History:
* 10-07-91  Petrus Wong
*   3D text output
*
\***************************************************************************/

LONG APIENTRY TextWndProc (HWND hwnd, UINT message, DWORD wParam, LONG lParam)
{
    static HFONT hFont = (HFONT) NULL;

    switch (message)
    {
    case WM_CREATE:
        {
	    LOGFONT    lf;
	    HDC        hDC;
	    HFONT      hOldFont;
            TEXTMETRIC tm;
	    //RECT       rect;

            SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(lf), (PVOID) &lf, (UINT)FALSE);

	    hDC = GetDC(hwnd);
	    // this is the height for 8 point size font in pixels
	    lf.lfHeight = 8 * GetDeviceCaps(hDC, LOGPIXELSY) / 72;

	    hFont = CreateFontIndirect(&lf);
	    hOldFont = SelectObject(hDC, hFont);
	    GetTextMetrics(hDC, &tm);

	    // base the height of the window on size of text
	    glcyStatus = tm.tmHeight+6*GetSystemMetrics(SM_CYBORDER)+2;
            ReleaseDC(hwnd, hDC);
            break;
        }

    case WM_DESTROY:
	    if (hFont)
		DeleteObject(hFont);
	    break;

    case WM_SETTEXT:
            DefWindowProc(hwnd, message, wParam, lParam);
            InvalidateRect(hwnd,NULL,FALSE);
            UpdateWindow(hwnd);
            return 0L;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            RECT   rc;
            char   ach[128];
            int    len, nxBorder, nyBorder;
            HFONT  hOldFont = NULL;

            BeginPaint(hwnd, &ps);

            GetClientRect(hwnd,&rc);

            nxBorder = GetSystemMetrics(SM_CXBORDER);
	    rc.left  += 9*nxBorder;
            rc.right -= 9*nxBorder;

            nyBorder = GetSystemMetrics(SM_CYBORDER);
	    rc.top    += 3*nyBorder;
	    rc.bottom -= 3*nyBorder;

	    // 3D Text
            len = GetWindowText(hwnd, ach, sizeof(ach));
	    SetBkColor(ps.hdc, GetSysColor(COLOR_BTNFACE));

	    SetBkMode(ps.hdc, TRANSPARENT);
	    SetTextColor(ps.hdc, RGB(64,96,96));
	    if (hFont)
		hOldFont = SelectObject(ps.hdc, hFont);
	    ExtTextOut(ps.hdc, rc.left+2*nxBorder+2, rc.top+2, ETO_OPAQUE | ETO_CLIPPED,
	                &rc, ach, len, NULL);

	    SetTextColor(ps.hdc, RGB(128,128,128));
  	    if (hFont)
		hOldFont = SelectObject(ps.hdc, hFont);
	    ExtTextOut(ps.hdc, rc.left+2*nxBorder+1, rc.top+1, ETO_CLIPPED,
			&rc, ach, len, NULL);

	    SetTextColor(ps.hdc, RGB(255,255,255));
	    if (hFont)
		hOldFont = SelectObject(ps.hdc, hFont);
	    ExtTextOut(ps.hdc, rc.left+2*nxBorder, rc.top, ETO_CLIPPED,
			&rc, ach, len, NULL);

	    SetBkMode(ps.hdc, OPAQUE);

	    if (hOldFont)
		SelectObject(ps.hdc, hOldFont);

            EndPaint(hwnd, &ps);
            return 0L;
        }
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

/******************************Public*Routine******************************\
*
* CtrlPanelDlgProc
*       The Control Panel dialog procedure
*
* Effects:  Responsible for drawing the owner draw buttons.  Notifying
*           parent of user's action.
*
* Warnings:
*
* History:
*  27-May-1992 -by- Petrus Wong
* Wrote it.
\**************************************************************************/

LONG APIENTRY CtrlPanelDlgProc(HWND hwnd, UINT msg, DWORD dwParam, LONG lParam)
{
    switch (msg) {
        case WM_INITDIALOG: {
            int index;

            for (index = 0; index < OD_BTN_CNT; index++) {
                grHwndCtrlBtn[index] = (PVOID)GetDlgItem(hwnd, (INT)(ID_OD_BTN_BASE+index));
            }
            for (index = 0; index < OD_TOOL_CNT; index++) {
                grHwndToolBtn[index] = (PVOID)GetDlgItem(hwnd, (INT)(ID_OD_TOOL_BASE+index));
            }
            return TRUE;
        }

        case WM_DRAWITEM: {
            PDRAWITEMSTRUCT pDIS = (PDRAWITEMSTRUCT) lParam;
            HBITMAP hBmpOld;
            BITMAP  bm;
            HANDLE  hCtl;
            HDC     hDCCtl;

            if (pDIS->CtlID == gdwCurCtrl) {
                GetObject((HBITMAP) ghBmpDn[pDIS->CtlID - ID_OD_BTN_BASE], sizeof(BITMAP), (LPSTR)&bm);
                hBmpOld = SelectObject(ghDCMem, (HBITMAP) ghBmpDn[pDIS->CtlID - ID_OD_BTN_BASE]);
            }

            if (pDIS->CtlID == gdwCurTool) {
                GetObject((HBITMAP)ghToolBmpDn[pDIS->CtlID - ID_OD_TOOL_BASE], sizeof(BITMAP), (LPSTR)&bm);
                hBmpOld = SelectObject(ghDCMem, (HBITMAP)ghToolBmpDn[pDIS->CtlID - ID_OD_TOOL_BASE]);
            }

            if ((pDIS->CtlID < ID_OD_TOOL_BASE) && (pDIS->CtlID != gdwCurCtrl)) {
                GetObject((HBITMAP)ghBmpUp[pDIS->CtlID - ID_OD_BTN_BASE], sizeof(BITMAP), (LPSTR)&bm);
                hBmpOld = SelectObject(ghDCMem, (HBITMAP)ghBmpUp[pDIS->CtlID - ID_OD_BTN_BASE]);
            }

            if ((pDIS->CtlID >= ID_OD_TOOL_BASE) && (pDIS->CtlID != gdwCurTool)) {
                GetObject((HBITMAP)ghToolBmpUp[pDIS->CtlID - ID_OD_TOOL_BASE], sizeof(BITMAP), (LPSTR)&bm);
                hBmpOld = SelectObject(ghDCMem, (HBITMAP)ghToolBmpUp[pDIS->CtlID - ID_OD_TOOL_BASE]);
            }

            //
            // pDIS->hDC is clipped to the update region but unfortunately
            // that doesn't work well with StretchBlt.  StretchBlt is used
            // because I don't have to make sure that the bitmap size is
            // exactly the same as the size of the button.
            //
            hCtl   = GetDlgItem(hwnd, pDIS->CtlID);
            hDCCtl = GetDC(hCtl);
            StretchBlt(hDCCtl,                                //pDIS->hDC,
                   pDIS->rcItem.left, pDIS->rcItem.top,
                   pDIS->rcItem.right - pDIS->rcItem.left,
                   pDIS->rcItem.bottom - pDIS->rcItem.top,
                   ghDCMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
            ReleaseDC(hCtl, hDCCtl);
            SelectObject(ghDCMem, hBmpOld);
            break;
        }

        case WM_COMMAND: {
            DWORD dwOldCtrl = gdwCurCtrl;
            DWORD dwOldTool = gdwCurTool;

            switch (dwParam) {
                case DID_ONE:
                case DID_TWO:
                case DID_THREE:
                case DID_FOUR:
                case DID_FIVE:
                case DID_SIX:
                case DID_SEVEN:
                case DID_EIGHT:
                case DID_NINE:
                case DID_ZERO:
                case DID_TEN_PLUS:
                    //SetDlgItemInt(ghwndCtrlPanel, DID_COUNTER, dwParam - DID_ZERO, FALSE);
                    SendMessage(ghwndMain, WM_COMMAND, dwParam, lParam);
                    break;
                case DID_OPEN:
                case DID_RECORD:
                case DID_STOP:
                case DID_PLAY:
                case DID_FF:
                    SendMessage(ghwndMain, WM_COMMAND, dwParam, lParam);
                    gdwCurCtrl = dwParam;
                    InvalidateRect((HWND)grHwndCtrlBtn[dwOldCtrl - ID_OD_BTN_BASE], NULL, FALSE);
                    InvalidateRect((HWND)grHwndCtrlBtn[gdwCurCtrl - ID_OD_BTN_BASE], NULL, FALSE);
                    break;
                case DID_CLEAR:
                    SendMessage(ghwndMain, WM_COMMAND, dwParam, lParam);
                    break;
                case DID_TEXT:
                case DID_PEN:
                case DID_RECT:
                case DID_FILLRECT:
                case DID_ELLIPSE:
                case DID_FILLELLIPSE:
                case DID_LINE:
                case DID_BEZIER:
                case DID_BMPOBJ:
                case DID_METAF:
                    SendMessage(ghwndMain, WM_COMMAND, dwParam, lParam);
                    gdwCurTool = dwParam;
                    InvalidateRect((HWND)grHwndToolBtn[dwOldTool - ID_OD_TOOL_BASE], NULL, FALSE);
                    InvalidateRect((HWND)grHwndToolBtn[gdwCurTool - ID_OD_TOOL_BASE], NULL, FALSE);
                    break;
            }
            break;
        }


        case WM_PAINT:
            {
                HDC hdc;
                RECT rc, rcDlg;
                PAINTSTRUCT ps;
                HPEN hpenWindowFrame, hpenDarkGray;
                int  icyDlg;
                int  icyBorder;

                icyBorder = GetSystemMetrics(SM_CYBORDER);

                GetWindowRect(hwnd, &rcDlg);
                icyDlg = rcDlg.right - rcDlg.left;

                /*
                 * Draw our border lines.
                 */
                GetClientRect(hwnd, &rc);
                hdc = BeginPaint(hwnd, &ps);

                SelectObject(hdc, GetStockObject(WHITE_PEN));
                MoveToEx(hdc, rc.left, rc.top, NULL);
                LineTo(hdc, rc.right, rc.top);

                hpenDarkGray = CreatePen(PS_SOLID, 1, DARKGRAY);
                SelectObject(hdc, hpenDarkGray);
                MoveToEx(hdc, rc.left, (rc.top + icyDlg) - icyBorder - 1, NULL);
                LineTo(hdc, rc.right, (rc.top + icyDlg) - icyBorder - 1);

                hpenWindowFrame = CreatePen(PS_SOLID, icyBorder,
                        GetSysColor(COLOR_WINDOWFRAME));
                SelectObject(hdc, hpenWindowFrame);
                MoveToEx(hdc, rc.left, (rc.top + icyDlg) - icyBorder, NULL);
                LineTo(hdc, rc.right, (rc.top + icyDlg) - icyBorder);

                EndPaint(hwnd, &ps);
                DeleteObject(hpenWindowFrame);
                DeleteObject(hpenDarkGray);
            }

            break;


        //case WM_CTLCOLOR:
        case WM_CTLCOLORDLG:
        //case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORSTATIC:
            switch (GET_WM_CTLCOLOR_TYPE(dwParam, lParam, msg)) {
                case CTLCOLOR_DLG:
                //case CTLCOLOR_LISTBOX:
                    return (BOOL)GetStockObject(LTGRAY_BRUSH);

                case CTLCOLOR_STATIC:
                    SetBkMode(GET_WM_CTLCOLOR_HDC(dwParam, lParam, msg),
                              TRANSPARENT);
                //    SetTextColor(GET_WM_CTLCOLOR_HDC(dwParam, lParam, msg),
                //              RGB(255,0,0));
                //    SetBkColor(GET_WM_CTLCOLOR_HDC(dwParam, lParam, msg),
                //            LIGHTGRAY);
                //              RGB(255, 255,0));
                    return (BOOL)GetStockObject(DKGRAY_BRUSH);
            }
            //return (BOOL)NULL;
            return (BOOL)GetStockObject(LTGRAY_BRUSH);

        default:
            return FALSE;
    }

    return FALSE;
}



/******************************Public*Routine******************************\
*
* bDrawStuff
*
* Effects:  The drawing routines are localized here.
*           bErase is TRUE if this fcn is called for erasing previous object.
*           (as in tracking objects.)  It is FALSE, otherwise.
*
*           bMove is TRUE if this fcn is called inside the WM_MOUSEMOVE (as
*           in tracking objects.)  It is FALSE, otherwise.
*
*           bCntPt is TRUE if this fcn is to increment either the iCnt or
*           iCntMF counter (used only in processing metafile or bezier.)
*           It is FALSE, otherwise.
*
*           lpstr contains the character to be drawn by TextOut when it is
*           not NULL.
*
* Warnings: Metafile and Bezier assume that the caller is calling this fcn
*           to draw in the screen DC first. Then draw it to the metafile DC.
*           Thus, when it is called to draw on the metafile DC, the points
*           would have been set already.
*
* History:
*  10-May-1992 -by- Petrus Wong
* Wrote it.
\**************************************************************************/

BOOL bDrawStuff(HDC hDC, INT OrgX, INT OrgY,
                         INT NextX, INT NextY,
                         BOOL bErase,
                         BOOL bMove,
                         BOOL bCntPt,
                         LPSTR lpstr) {
    BOOL bSuccess;
    HGDIOBJ hObjOld;
    static POINT rgPts[MAX_POINTS], rgPtsMF[MAX_POINTS_MF], rgPtsBMP[MAX_POINTS_BMP];
    static int   iCnt=0, iCntMF=0, iCntBMP=0;
    static BOOL  bCaretShown=FALSE;

    bSuccess = TRUE;
    if (bCaretShown) {
        HideCaret(ghwndDrawSurf);
        bCaretShown = FALSE;
    }

    switch (gdwCurTool) {
        case DID_PEN:
            if (bErase) {
                MoveToEx(hDC, NextX, NextY, NULL);
            } else {
                //
                // Override the ROP2 st. the pen won't erase its track
                //
                SetROP2(hDC, R2_COPYPEN);
                LineTo(hDC, NextX, NextY);
            }
            break;
        case DID_TEXT: {
            POINT   Pt;
#if 0
HDC hDCMem;
#endif

            if (lpstr == NULL) {
                ShowCaret(ghwndDrawSurf);
                bCaretShown = TRUE;
                SetCaretPos(NextX, NextY);
                MoveToEx(hDC, NextX, NextY, NULL);

#if 0
StretchDIBits(hDC, 20, 20+120, 64, 64, 0, 64, 64, -64, &dib,
					(LPBITMAPINFO)&DIBHdr, DIB_PAL_COLORS, SRCCOPY);

hDCMem = CreateCompatibleDC(hDC);
SelectPalette(hDCMem, hPal, FALSE);
RealizePalette(hDCMem);
SelectObject(hDCMem, hbmp);
BitBlt(hDC, 0,0,64,64,hDCMem, 0,0,SRCCOPY);
DeleteDC(hDCMem);
#endif




                SetFocus(ghwndDrawSurf);
                break;
            }

            SetTextAlign(hDC, TA_BASELINE | TA_LEFT | TA_UPDATECP);
            hObjOld = SelectObject(hDC, ghCurFont);
            SetTextColor(hDC, gCrText);

            if ((gbSFOutln || gbPDOutln) && gbTT) {
                // get rid of the char box
                SetBkMode(hDC, TRANSPARENT);
                BeginPath(hDC);
                TextOut(hDC, NextX,    NextY,    lpstr, 1);
                EndPath(hDC);

                if (gbSFOutln) {
                    StrokeAndFillPath(hDC);
                    goto DT_UPDATE;
                }

                //
                // Get path and polydraw
                //
                {
                int     iNumPt;
                PBYTE   pjTypes;
                PPOINT  pPts;

                if (iNumPt = GetPath(hDC, NULL, NULL, 0)) {
                    if ((pPts = (PPOINT)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
                                                sizeof(POINT)*iNumPt )) == NULL) {
                        MessageBox(ghwndMain, "Failed in Creating POINT!", "Error", MB_OK);
                        break;
                    }

                    if ((pjTypes = (PBYTE)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
                                                sizeof(BYTE)*iNumPt )) == NULL) {
                        MessageBox(ghwndMain, "Failed in Creating PBYTE!", "Error", MB_OK);
                        goto GP_EXIT1;
                    }

                    GetPath(hDC, pPts, pjTypes, iNumPt);
                }
                PolyDraw(hDC, pPts, pjTypes, iNumPt);
                LocalFree(pjTypes);

GP_EXIT1:
                LocalFree(pPts);

                }

            } else {
                TextOut(hDC, NextX,    NextY,    lpstr, 1);
            }
DT_UPDATE:
            //
            // Updating current position
            //
            {
            LONG    lHeight;
            LONG    lWidth;
            TEXTMETRIC     tm;

                if (GetTextMetrics(hDC, &tm)) {
                    lHeight = tm.tmHeight;
                    lWidth  = tm.tmMaxCharWidth;
                }

                GetCurrentPositionEx(hDC, (LPPOINT) &Pt);
                SetCaretPos(Pt.x+lWidth, Pt.y);

            }
            ShowCaret(ghwndDrawSurf);
            bCaretShown = TRUE;

            break;

#if 0

#define PT_LINECLOSE     (PT_LINETO | PT_CLOSEFIGURE)
#define PT_BEZIERCLOSE (PT_BEZIERTO | PT_CLOSEFIGURE)

            hpnRed = CreatePen(PS_SOLID, 0, RGB(255,0,0));
            SelectObject(hDC, hpnRed);

            while (iNumPt--) {
                static POINT pPnt[3];
                static int   iCnt=0;

                switch (*pjTypes++) {
                    case PT_MOVETO: {
                        MoveToEx(hDC, pPts->x, pPts->y, NULL);
                        pPts++;
                        break;
                    }
                    case PT_LINETO: {
                        LineTo(hDC, pPts->x, pPts->y);
                        pPts++;
                        break;

                    }
                    case PT_LINECLOSE: {
                        LineTo(hDC, pPts->x, pPts->y);
                        pPts++;
                        goto GP_EXIT2;
                    }
                    case PT_BEZIERTO: {
                        pPnt[iCnt].x = pPts->x;
                        pPnt[iCnt].y = pPts->y;
                        pPts++;

                        if (iCnt < 2) {
                            iCnt++;
                        } else {
                            PolyBezierTo(hDC, pPnt, 3);
                            iCnt = 0;
                        }
                        break;
                    }
                    case PT_BEZIERCLOSE: {
                        pPnt[iCnt].x = pPts->x;
                        pPnt[iCnt].y = pPts->y;
                        pPts++;

                        if (iCnt < 2) {
                            iCnt++;
                        } else {
                            PolyBezierTo(hDC, pPnt, 3);
                            iCnt = 0;
                        }
                        goto GP_EXIT2;
                    }

                    default:
                        break;
                }

            }

#endif
        }
        case DID_RECT:
            hObjOld = SelectObject(hDC, GetStockObject(NULL_BRUSH));
            Rectangle(hDC, OrgX, OrgY, NextX, NextY);
            SelectObject(hDC, hObjOld);
            break;
        case DID_FILLRECT:
            Rectangle(hDC, OrgX, OrgY, NextX, NextY);
            break;
        case DID_ELLIPSE:
            hObjOld = SelectObject(hDC, GetStockObject(NULL_BRUSH));
            Ellipse(hDC, OrgX, OrgY, NextX, NextY);
            SelectObject(hDC, hObjOld);
            break;
        case DID_FILLELLIPSE:
            Ellipse(hDC, OrgX, OrgY, NextX, NextY);
            break;
        case DID_LINE:
            MoveToEx(hDC, OrgX, OrgY, NULL);
            LineTo(hDC, NextX, NextY);
            break;
        case DID_BEZIER:
            if (bErase || bMove)
                return bSuccess;

            if (bCntPt) {
                rgPts[iCnt].x = NextX;
                rgPts[iCnt].y = NextY;
                iCnt++;

                if (iCnt == MAX_POINTS - 1)
                    iCnt = 0;
            }

            if ((iCnt % 3) == 1) {              // (iCnt + 1) % 3 == 1
                //
                // Override the ROP2 st. the pen won't erase its track
                //
                SetROP2(hDC, R2_COPYPEN);
                PolyBezier(hDC, (LPPOINT)&rgPts, (DWORD) iCnt);
            }
            return bSuccess;

        case DID_BMPOBJ: {
            static BOOL          bBltReady = FALSE;

            if (bErase || bMove)
                return bSuccess;

            if (ghBmp == NULL) {
                SetWindowText(ghTextWnd, "ERROR: No bitmap to embed!");
                return bSuccess;
            }

            if (bCntPt) {
                bBltReady = FALSE;
                rgPtsBMP[iCntBMP].x = NextX;
                rgPtsBMP[iCntBMP].y = NextY;
                iCntBMP++;

                if (iCntBMP < MAX_POINTS_BMP) {
                    return bSuccess;
                }
            } else {
                //
                // Caller don't want to increment counter, so must be doing
                // recording, so we just Blt again...
                //
                // But, if the Blt data is no good, bail out...
                //
                if (!bBltReady) {
                    return bSuccess;
                }
                bPlgBlt(hDC, rgPtsBMP);
                return bSuccess;
            }
            bBltReady = TRUE;

            bPlgBlt(hDC, rgPtsBMP);
            iCntBMP = 0;                         // reset
            return bSuccess;
        }

        case DID_METAF: {
            ENHMETAHEADER EnhMetaHdr;
            RECT          rcClientDS;
            static XFORM         xform;
            static BOOL          bXformReady = FALSE;
            int           iEntries;
            PLOGPALETTE     plogPal;
            PBYTE           pjTmp;
            HPALETTE        hPal;


            if (bErase || bMove)
                return bSuccess;

            if (ghMetaf == NULL) {
                SetWindowText(ghTextWnd, "ERROR: No metafile to embed!");
                return bSuccess;
            }

            if (bCntPt) {
                bXformReady = FALSE;
                rgPtsMF[iCntMF].x = NextX;
                rgPtsMF[iCntMF].y = NextY;
                iCntMF++;

                if (iCntMF < MAX_POINTS_MF) {
                    return bSuccess;
                }
            } else {
                //
                // Caller don't want to increment counter, so must be doing
                // recording, so we just set xform and play it again...
                //
                // But, if the xform data is no good, bail out...
                //
                if (!bXformReady) {
                    return bSuccess;
                }

                GetEnhMetaFileHeader(ghMetaf, sizeof(ENHMETAHEADER), &EnhMetaHdr);
                SetWorldTransform(hDC, &xform);
                GetClientRect(ghwndDrawSurf, &rcClientDS);

                iEntries = GetEnhMetaFilePaletteEntries(ghMetaf, 0, NULL);

                if (iEntries) {
                    if ((plogPal = (PLOGPALETTE)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
                            sizeof(DWORD) + sizeof(PALETTEENTRY)*iEntries )) == NULL) {
                        MessageBox(ghwndMain, "Failed in Creating Palette!", "Error", MB_OK);
                    }

                    plogPal->palVersion = 0x300;
                    plogPal->palNumEntries = (WORD) iEntries;
                    pjTmp = (PBYTE) plogPal;
                    pjTmp += 8;

                    GetEnhMetaFilePaletteEntries(ghMetaf, iEntries, (PPALETTEENTRY)pjTmp);
                    hPal = CreatePalette(plogPal);
                    GlobalFree(plogPal);

                    SelectPalette(hDC, hPal, FALSE);
                    RealizePalette(hDC);
                }

                //PlayEnhMetaFile(hDC, ghMetaf, (LPRECT) &rcClientDS);
                {
                RECT rc;

                rc.top = rc.left = 0;
                rc.right = EnhMetaHdr.rclBounds.right - EnhMetaHdr.rclBounds.left;
                rc.bottom = EnhMetaHdr.rclBounds.bottom - EnhMetaHdr.rclBounds.top;
                if (!PlayEnhMetaFile(hDC, ghMetaf, (LPRECT) &rc)) {
                    char    text[128];

                    wsprintf(text, "Fail in PlayEnhMetaFile! Error %ld\n", GetLastError());
                    OutputDebugString(text);
                }

                }
                ModifyWorldTransform(hDC, NULL, MWT_IDENTITY);
                return bSuccess;
            }

            GetEnhMetaFileHeader(ghMetaf, sizeof(ENHMETAHEADER), &EnhMetaHdr);
            //
            // Based on the three points, top-left, top-right and bottom-left
            // (in this order), of the destination, solve equations for the
            // elements of the transformation matrix.
            //
            xform.eDx = (float) rgPtsMF[0].x;
            xform.eDy = (float) rgPtsMF[0].y;
            xform.eM11 = (rgPtsMF[1].x - xform.eDx)/(EnhMetaHdr.rclBounds.right - EnhMetaHdr.rclBounds.left);
            xform.eM12 = (rgPtsMF[1].y - xform.eDy)/(EnhMetaHdr.rclBounds.right - EnhMetaHdr.rclBounds.left);
            xform.eM21 = (rgPtsMF[2].x - xform.eDx)/(EnhMetaHdr.rclBounds.bottom - EnhMetaHdr.rclBounds.top);
            xform.eM22 = (rgPtsMF[2].y - xform.eDy)/(EnhMetaHdr.rclBounds.bottom - EnhMetaHdr.rclBounds.top);

            bXformReady = TRUE;
            SetWorldTransform(hDC, &xform);
            GetClientRect(ghwndDrawSurf, &rcClientDS);

            iEntries = GetEnhMetaFilePaletteEntries(ghMetaf, 0, NULL);

            if (iEntries) {
                if ((plogPal = (PLOGPALETTE)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
                        sizeof(DWORD) + sizeof(PALETTEENTRY)*iEntries )) == NULL) {
                    MessageBox(ghwndMain, "Failed in Creating Palette!", "Error", MB_OK);
                }

                plogPal->palVersion = 0x300;
                plogPal->palNumEntries = (WORD) iEntries;
                pjTmp = (PBYTE) plogPal;
                pjTmp += 8;

                GetEnhMetaFilePaletteEntries(ghMetaf, iEntries, (PPALETTEENTRY)pjTmp);
                hPal = CreatePalette(plogPal);
                GlobalFree(plogPal);

                SelectPalette(hDC, hPal, FALSE);
                RealizePalette(hDC);
            }

            //PlayEnhMetaFile(hDC, ghMetaf, (LPRECT) &rcClientDS);
            {
            RECT rc;

            rc.top = rc.left = 0;
            rc.right = EnhMetaHdr.rclBounds.right - EnhMetaHdr.rclBounds.left;
            rc.bottom = EnhMetaHdr.rclBounds.bottom - EnhMetaHdr.rclBounds.top;
            if (!PlayEnhMetaFile(hDC, ghMetaf, (LPRECT) &rc)) {
                 char    text[128];

                 wsprintf(text, "Fail in PlayEnhMetaFile! Error %ld\n", GetLastError());
                 OutputDebugString(text);
            }

            }
            ModifyWorldTransform(hDC, NULL, MWT_IDENTITY);
            iCntMF = 0;                         // reset
            return bSuccess;
        }
        default:
            break;
    }
    //
    // Reset counter, user has selected other tools.
    //
    iCnt = 0;
    iCntMF = 0;
    iCntBMP = 0;
    return bSuccess;
}

/******************************Public*Routine******************************\
*
* hemfLoadMetafile
*
* Effects:   Brings up the Open file common dialog
*            Get the enhanced metafile spec'd by user
*            returns the handle to the enhanced metafile if successfull
*               otherwise, returns 0.
*
* Warnings:
*
* History:
*  08-May-1992 -by- Petrus Wong
* Wrote it.
*  28-Aug-1992 -by- Petrus Wong     supports aldus placable mf, wmf and emf
\**************************************************************************/

HENHMETAFILE hemfLoadMetafile(HWND hwnd) {
    OPENFILENAME    ofn;
    char            szFile[256], szFileTitle[256];
    static char     *szFilter;

    HMETAFILE       hmf;
    UINT            uiSize;
    LPVOID          pvData;
    HDC             hDCDrawSurf;
    HENHMETAFILE    hemf;

    HANDLE                  hFile, hMapFile;
    LPVOID                  pMapFile;
    LPENHMETAHEADER         pemh;

    BOOL        bSuccess;
    char            text[128];


    bSuccess = TRUE;

    szFilter =
      "EnhMeta files (*.emf)\0*.emf\0Windows Metafiles (*.wmf)\0*.wmf\0\0";

    strcpy(szFile, "*.emf\0");
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = szFilter;
    ofn.lpstrCustomFilter = (LPSTR) NULL;
    ofn.nMaxCustFilter = 0L;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFileTitle = szFileTitle;
    ofn.nMaxFileTitle = sizeof(szFileTitle);
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = "Load Metafile";
    ofn.Flags = 0L;
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = "EMF";

    if (!GetOpenFileName(&ofn))
        return 0L;

    if ((hFile = CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL)) == (HANDLE)-1) {
        wsprintf(text, "Fail in file open! Error %ld\n", GetLastError());
        MessageBox(ghwndMain, text, "Error", MB_OK);
        return 0L;
    }

    //
    // Create a map file of the opened file
    //
    if ((hMapFile = CreateFileMapping(hFile, NULL,
                             PAGE_READONLY, 0, 0, "MapF")) == NULL) {
        wsprintf(text, "Fail in creating map file! Error %ld\n", GetLastError());
        MessageBox(ghwndMain, text, "Error", MB_OK);
        bSuccess = FALSE;
        goto ErrorExit1;
    }

    //
    // Map a view of the whole file
    //
    if ((pMapFile = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0)) == NULL) {
        wsprintf(text, "Fail in mapping view of the Map File object! Error %ld\n", GetLastError());
        MessageBox(ghwndMain, text, "Error", MB_OK);
        bSuccess = FALSE;
        goto ErrorExit2;
    }

    //
    // First check that if it is an enhanced metafile
    //
    pemh = (LPENHMETAHEADER) pMapFile;
    if (pemh->dSignature == META32_SIGNATURE) {
        hemf = GetEnhMetaFile(szFile);
        goto HLM_EXIT;
    }

    //
    // If it has an ALDUS header skip it
    // Notice: APMSIZE is used because the HANDLE and RECT of the structure
    //         depends on the environment
    //
    if (*((LPDWORD)pemh) == ALDUS_ID) {
        //METAFILEPICT    mfp;

        MessageBox(ghwndMain, "This is an ALDUS metafile!", "Hey!", MB_OK);
        uiSize = *((LPDWORD) ((PBYTE)pMapFile + APMSIZE + 6));
        hDCDrawSurf = GetDC(ghwndDrawSurf);

        // Notice: mtSize is size of the file in word.
        // if LPMETAFILEPICT is NULL
        //    MM_ANISOTROPIC mode and default device size will be used.
        hemf = SetWinMetaFileBits(uiSize*2L, (PBYTE)pMapFile + APMSIZE, hDCDrawSurf, NULL);
#if 0
        switch ( ((PAPMFILEHEADER) pMapFile)->inch ) {
            // !!! End up in an upside down image
            //
            case 1440:
                mfp.mm = MM_TWIPS;
                OutputDebugString("MM_TWIPS\n");
                break;
            case 2540:
                OutputDebugString("MM_HIMETRIC\n");
                mfp.mm = MM_HIMETRIC;
                break;
            case 254:
                OutputDebugString("MM_LOMETRIC\n");
                mfp.mm = MM_LOMETRIC;
                break;
            case 1000:
                OutputDebugString("MM_HIENGLISH\n");
                mfp.mm = MM_HIENGLISH;
                break;
            case 100:
                OutputDebugString("MM_LOENGLISH\n");
                mfp.mm = MM_LOENGLISH;
                break;
            default:
                // !!! In addition, text is too small
                //
                OutputDebugString("MM_ANISOTROPIC\n");
                mfp.mm = MM_ANISOTROPIC;
                mfp.xExt = (((PAPMFILEHEADER) pMapFile)->bbox.Right - ((PAPMFILEHEADER) pMapFile)->bbox.Left)
                           * ((PAPMFILEHEADER) pMapFile)->inch * 2560;
                mfp.yExt = (((PAPMFILEHEADER) pMapFile)->bbox.Bottom - ((PAPMFILEHEADER) pMapFile)->bbox.Top)
                           * ((PAPMFILEHEADER) pMapFile)->inch * 2560;
                break;
        }
        mfp.hMF = 0;
        hemf = SetWinMetaFileBits(uiSize*2L, (PBYTE)pMapFile + APMSIZE, hDCDrawSurf, &mfp);
#endif

        if (!hemf) {
            char text[256];

            wsprintf(text, "SetWinMetaFileBits failed, %x", GetLastError());
            MessageBox(ghwndMain, text, "Error!", MB_OK);
        }

        ghmf = SetMetaFileBitsEx(uiSize*2L, (PBYTE)pMapFile + APMSIZE);
        if (!ghmf) {
            char text[256];

            wsprintf(text, "SetMetaFileBitsEx failed, %x", GetLastError());
            MessageBox(ghwndMain, text, "Error!", MB_OK);
        }

// !!! Displaying the Windows format metafile
//if (!PlayMetaFile(hDCDrawSurf, ghmf)) {
//    wsprintf(text, "PlayMetaFile failed, %x", GetLastError());
//    MessageBox(ghwndMain, text, "Error!", MB_OK);
//}
        ReleaseDC(ghwndDrawSurf, hDCDrawSurf);
        goto HLM_EXIT;
    }


    //
    // It is a Windows 3x format metafile (hopefully)
    //
    if (!(hmf = GetMetaFile((LPCSTR)szFile))) {
        char text[256];

        wsprintf(text, "GetMetaFile failed, %x", GetLastError());
        MessageBox(ghwndMain, text, "Error!", MB_OK);
        bSuccess = FALSE;
        goto ErrorExit3;
    }

    if (!(uiSize = GetMetaFileBitsEx(hmf, 0, NULL))) {
        MessageBox(ghwndMain, "Fail in 1st GetMetaFileBitsEx!", "Error", MB_OK);
        return NULL;
    }

    if ((pvData = (LPVOID) LocalAlloc(LMEM_FIXED, uiSize)) == NULL) {
        MessageBox(ghwndMain, "Fail in Memory Allocation!", "Error", MB_OK);
        bSuccess = FALSE;
        goto ErrorExit3;
    }

    if (!(uiSize = GetMetaFileBitsEx(hmf, uiSize, pvData))) {
        MessageBox(ghwndMain, "Fail in 2nd GetMetaFileBitsEx!", "Error", MB_OK);
        bSuccess = FALSE;
        goto ErrorExit3;
    }

    DeleteMetaFile(hmf);

    hDCDrawSurf = GetDC(ghwndDrawSurf);
    hemf = SetWinMetaFileBits(uiSize, (LPBYTE)pvData, hDCDrawSurf, NULL);
    ghmf = SetMetaFileBitsEx(uiSize, (LPBYTE) pvData);

    LocalFree(pvData);

    ReleaseDC(ghwndDrawSurf ,hDCDrawSurf);

HLM_EXIT:
ErrorExit3:
    UnmapViewOfFile(pMapFile);

ErrorExit2:
    CloseHandle(hMapFile);
ErrorExit1:
    CloseHandle(hFile);

    if (bSuccess)
        return hemf;
    else
        return 0L;
}

/******************************Public*Routine******************************\
*
* hDCRecordMetafileAs
*
* Effects:   Brings up the SaveAs common dialog
*            Creates the enhanced metafile with the filename spec'd by user
*            Modifies the second arg to reflect the new default filename
*            less extension
*            returns the created metafile DC if successful, otherwise, 0
*
* Warnings:
*
* History:
*  08-May-1992 -by- Petrus Wong
* Wrote it.
\**************************************************************************/

HDC hDCRecordMetafileAs(HWND hwnd, LPSTR szFilename) {
    OPENFILENAME ofn;
    char szFile[256], szFileTitle[256];
    static char *szFilter;
    char *szTmp, szTmp2[256];
    HDC  hDCMeta;

    int iWidthMM, iHeightMM, iWidthPels, iHeightPels, iMMPerPelX, iMMPerPelY;
    RECT rc;
    HDC hDC;


    szFilter = "EnhMeta files (*.emf)\0\0";
    strcpy(szFile, "*.emf\0");
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = szFilter;
    ofn.lpstrCustomFilter = (LPSTR) NULL;
    ofn.nMaxCustFilter = 0L;
    ofn.nFilterIndex = 0L;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFileTitle = szFileTitle;
    ofn.nMaxFileTitle = sizeof(szFileTitle);
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = "Save Metafile As";
    ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = (LPSTR)NULL;

    if (!GetSaveFileName(&ofn)) {
        return 0L;
    }


    hDC = GetDC(hwnd);
    iWidthMM = GetDeviceCaps(hDC, HORZSIZE);
    iHeightMM = GetDeviceCaps(hDC, VERTSIZE);
    iWidthPels = GetDeviceCaps(hDC, HORZRES);
    iHeightPels = GetDeviceCaps(hDC, VERTRES);
    ReleaseDC(hwnd, hDC);
    iMMPerPelX = (iWidthMM * 100)/iWidthPels;
    iMMPerPelY = (iHeightMM * 100)/iHeightPels;
    GetClientRect(ghwndDrawSurf, &rc);
    rc.left = rc.left * iMMPerPelX;
    rc.top = rc.top * iMMPerPelY;
    rc.right = rc.right * iMMPerPelX;
    rc.bottom = rc.bottom * iMMPerPelY;


    //hDCMeta = CreateEnhMetaFile((HDC)NULL, szFile, (LPRECT)NULL, (LPSTR)NULL);
    {
        CHAR    szDesc[256];

        wsprintf(szDesc, "SDK Enhanced Metafile Editor\\0%s\\0\\0", szFileTitle);
        hDCMeta = CreateEnhMetaFile((HDC)NULL, szFile, (LPRECT)&rc, (LPSTR)szDesc);
        if ((SetGraphicsMode(hDCMeta, GM_ADVANCED)) == 0) {
           MessageBox(ghwndMain, "Fail in setting Advanced Graphics Mode!", "Error", MB_OK);
        }
    }

    //
    // parses the new filename, removes the extension and copy it into
    // szFilename
    //
    strcpy(szFilename, "");
    szTmp = (char *)strtok(szFile, "\\");
    strcpy(szTmp2, szTmp);
    while (szTmp != NULL) {
        szTmp = (char *)strtok(NULL, "\\");
        if (szTmp != NULL) {
            strcat(szFilename, szTmp2);
            strcpy(szTmp2, szTmp);
            strcat(szFilename, "\\");
        }
    }
    szTmp = (char *)strtok(szTmp2, ".");
    strcat(szFilename, szTmp);

    return hDCMeta;
}


/******************************Public*Routine******************************\
*
* bPlayRecord
*
* Effects:  Play metafile
*           if PlayInfo.bPlayContinuous is TRUE
*               play metafile from 1st record up to the PlayInfo.iRecord th
*                   record
*           else only play the PlayInfo.iRecord th record and those preceding
*               records that are relevant like MoveTo, etc.
*           Terminates enumeration after playing up to the
*               PlayInfo.iRecord th record
*
* Warnings:
*
* History:
*  08-May-1992 -by- Petrus Wong
* Wrote it.
\**************************************************************************/

BOOL APIENTRY bPlayRecord(HDC hDC, LPHANDLETABLE lpHandleTable,
                                   LPENHMETARECORD lpEnhMetaRecord,
                                   UINT nHandles,
                                   LPVOID lpData) {
    BOOL bSuccess;
    static int  iCnt=0;
    int         i;
    char        ach[128];
    char        achTmp[128];
    LONG        lNumDword;

    bSuccess = TRUE;

    lNumDword = (lpEnhMetaRecord->nSize-8) / 4;

    iCnt++;
    if (((PLAYINFO *) lpData)->bPlayContinuous) {
        bSuccess = PlayEnhMetaFileRecord(hDC, lpHandleTable,
                                             lpEnhMetaRecord, nHandles);
        if (iCnt == ((PLAYINFO *) lpData)->iRecord) {
            wsprintf((LPSTR) ach, "%s", rgMetaName[lpEnhMetaRecord->iType]);
            for (i=0; i < lNumDword; i++) {
                wsprintf((LPSTR) achTmp, "%ld ", lpEnhMetaRecord->dParm[i]);
                if ((strlen(ach)+strlen(achTmp))/sizeof(char) >= 128)
                    break;
                strcat(ach, achTmp);
            }
        SetWindowText(ghTextWnd, ach);
        }
    } else {

        switch (lpEnhMetaRecord->iType) {
            case MR_SETWINDOWEXTEX:
            case MR_SETWINDOWORGEX:
            case MR_SETVIEWPORTEXTEX:
            case MR_SETVIEWPORTORGEX:
            case MR_SETBRUSHORGEX:
            case MR_SETMAPMODE:
            case MR_SETBKMODE:
            case MR_SETPOLYFILLMODE:
            case MR_SETROP2:
            case MR_SETSTRETCHBLTMODE:
            case MR_SETTEXTALIGN:
            case MR_SETTEXTCOLOR:
            case MR_SETBKCOLOR:
            case MR_OFFSETCLIPRGN:
            case MR_MOVETOEX:
            case MR_SETMETARGN:
            case MR_EXCLUDECLIPRECT:
            case MR_INTERSECTCLIPRECT:
            case MR_SCALEVIEWPORTEXTEX:
            case MR_SCALEWINDOWEXTEX:
            case MR_SAVEDC:
            case MR_RESTOREDC:
            case MR_SETWORLDTRANSFORM:
            case MR_MODIFYWORLDTRANSFORM:
            case MR_SELECTOBJECT:
            case MR_CREATEPEN:
            case MR_CREATEBRUSHINDIRECT:
            case MR_DELETEOBJECT:
            case MR_SELECTPALETTE:
            case MR_CREATEPALETTE:
            case MR_SETPALETTEENTRIES:
            case MR_RESIZEPALETTE:
            case MR_REALIZEPALETTE:
            case MR_SETARCDIRECTION:
            case MR_SETMITERLIMIT:
            case MR_BEGINPATH:
            case MR_ENDPATH:
            case MR_CLOSEFIGURE:
            case MR_SELECTCLIPPATH:
            case MR_ABORTPATH:
            case MR_EXTCREATEFONTINDIRECTW:
            case MR_CREATEMONOBRUSH:
            case MR_CREATEDIBPATTERNBRUSHPT:
            case MR_EXTCREATEPEN:
                goto PlayRec;
            default:
                break;
        } //switch

        if (iCnt == ((PLAYINFO *) lpData)->iRecord) {
PlayRec:
            bSuccess = PlayEnhMetaFileRecord(hDC, lpHandleTable,
                                             lpEnhMetaRecord, nHandles);
            wsprintf((LPSTR) ach, "%s", rgMetaName[lpEnhMetaRecord->iType]);
            for (i=0; i < lNumDword; i++) {
                wsprintf((LPSTR) achTmp, "%ld ", lpEnhMetaRecord->dParm[i]);
                if ((strlen(ach)+strlen(achTmp))/sizeof(char) >= 128)
                    break;
                strcat(ach, achTmp);
            }
            SetWindowText(ghTextWnd, ach);
        }
    }

    if (iCnt == ((PLAYINFO *) lpData)->iRecord) {
        iCnt = 0;
        return FALSE;
    }
    return bSuccess;
}

/******************************Public*Routine******************************\
*
* LoadBitmapFile
*
* Effects:  Loads the bitmap from file and return the bitmap
*
* Warnings: pszFileName contains the full path
*
* History:
*  18-Feb-1993 Petrus Wong           fix metaf bnp color problem
*  21-Oct-1992 Petrus Wong           fix data-misalignment
*  13-May-1992 Petrus Wong           return bitmap handle
*  09-Jan-1992 -by- Petrus Wong
* Wrote it.
\**************************************************************************/

HBITMAP hBmpLoadBitmapFile(HDC hDC, PSTR pszFileName)
{
    HANDLE              hFile, hMapFile;
    LPVOID              pMapFile, pMapFileTmp;
    LPBITMAPINFOHEADER  pbmh;
    LPBITMAPINFO        pbmi;
    PBYTE               pjTmp;
    ULONG               sizBMI;
    HBITMAP             hBitmap;
    INT                 iNumClr;
    BOOL                bCoreHdr;
    WORD                wBitCount;
    PFILEINFO           pFileInfo;

    hBitmap = NULL;

    if ((hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL)) == (HANDLE)-1) {
        SetWindowText(ghTextWnd, "Fail in file open");
        goto ErrExit1;
    }

    //
    // Create a map file of the opened file
    //
    if ((hMapFile = CreateFileMapping(hFile, NULL,
                             PAGE_READONLY, 0, 0, NULL)) == (HANDLE)-1) {
        SetWindowText(ghTextWnd, "Fail in creating map file");
        goto ErrExit2;

    }

    //
    // Map a view of the whole file
    //
    if ((pMapFile = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0)) == NULL) {
        SetWindowText(ghTextWnd, "Fail in mapping view of the Map File object");
        goto ErrExit3;
    }

    pMapFileTmp = pMapFile;

    //
    // First check that it is a bitmap file
    //
    if (*((PWORD)pMapFile) != 0x4d42) {              // 'BM'
        MessageBox(ghwndMain, "This is not a DIB bitmap file!", "Error", MB_OK);
        goto ErrExit3;
    }

    //
    // The file header doesn't end on DWORD boundary...
    //
    pbmh = (LPBITMAPINFOHEADER)((PBYTE)pMapFile + sizeof(BITMAPFILEHEADER));

    {
        BITMAPCOREHEADER bmch, *pbmch;
        BITMAPINFOHEADER bmih, *pbmih;
        PBYTE            pjTmp;
        ULONG            ulSiz;

        pbmch = &bmch;
        pbmih = &bmih;

        pjTmp = (PBYTE)pbmh;
        ulSiz = sizeof(BITMAPCOREHEADER);
        while (ulSiz--) {
            *(((PBYTE)pbmch)++) = *(((PBYTE)pjTmp)++);
        }

        pjTmp = (PBYTE)pbmh;
        ulSiz = sizeof(BITMAPINFOHEADER);
        while (ulSiz--) {
            *(((PBYTE)pbmih)++) = *(((PBYTE)pjTmp)++);
        }

        //
        // Use the size to determine if it is a BitmapCoreHeader or
        // BitmapInfoHeader
        //
        // Does PM supports 16 and 32 bpp? How?
        //
        if (bmch.bcSize == sizeof(BITMAPCOREHEADER))
        {
            wBitCount = bmch.bcBitCount;
            iNumClr = ((wBitCount == 24) ? 0 : (1 << wBitCount));
            sizBMI = sizeof(BITMAPCOREHEADER)+sizeof(RGBTRIPLE)*iNumClr;
            bCoreHdr = TRUE;
        }
        else            // BITMAPINFOHEADER
        {
            wBitCount = bmih.biBitCount;
            switch (wBitCount) {
                case 16:
                case 32:
                    sizBMI = sizeof(BITMAPINFOHEADER)+sizeof(DWORD)*3;
                    break;
                case 24:
                    sizBMI = sizeof(BITMAPINFOHEADER);
                    break;
                default:
                    iNumClr = (1 << wBitCount);
                    sizBMI = sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*iNumClr;
                    break;
            }
            bCoreHdr = FALSE;
        }
    }

    if ((pbmi = (LPBITMAPINFO) LocalAlloc(LMEM_FIXED,sizBMI)) == NULL) {
        MessageBox(ghwndMain, "Fail in Memory Allocation!", "Error", MB_OK);
        goto ErrExit3;
    }

    //
    // Make sure we pass in a DWORD aligned BitmapInfo to CreateDIBitmap
    // Otherwise, exception on the MIPS platform
    // CR!!!  Equivalent to memcpy
    //
    pjTmp = (PBYTE)pbmi;

    while(sizBMI--)
    {
        *(((PBYTE)pjTmp)++) = *(((PBYTE)pbmh)++);
    }

    pMapFile = (PBYTE)pMapFile + ((BITMAPFILEHEADER *)pMapFile)->bfOffBits;

// !!! Use CreateBitmap for monochrome bitmap?

    //
    // Select the palette into the DC first before CreateDIBitmap()
    //
    bSelectDIBPal(hDC, pbmi, bCoreHdr);

// !!! We always pass a screen DC to this routine.
// !!! Maybe we should pass a metafile DC to this routine too.
// !!! The bitmap handle created for the screen DC won't give correct
// !!! color for the metafile DC.  So now, we always use the original
// !!! DIB info.
    if ((hBitmap = CreateDIBitmap(hDC, (LPBITMAPINFOHEADER)pbmi,
                        CBM_INIT, pMapFile, pbmi, DIB_RGB_COLORS)) == NULL) {
        SetWindowText(ghTextWnd, "Fail in creating DIB bitmap from file!");
        goto ErrExit4;
    }

    // reset gbUseDIB flag, now that we have opened up a new DIB
    gbUseDIB = FALSE;

// !!! Always use the DIB info o.w. metafile DC don't get the right color.
#if 0
    if (GetDeviceCaps(hDC, BITSPIXEL) < wBitCount) {
#endif
        gbUseDIB = TRUE;
        bFreeDibFile(&gDib);
        pFileInfo = &(gDib.rgFileInfo[0]);
        pFileInfo->hFile        = hFile;
        pFileInfo->hMapFile     = hMapFile;
        pFileInfo->lpvMapView   = pMapFileTmp;

        gDib.rgpjFrame[0]       = pMapFile;
        gDib.rgpbmi[0]          = pbmi;
        gDib.rgbCoreHdr[0]      = bCoreHdr;
        gDib.ulFrames           =
        gDib.ulFiles            = 1;
        return (hBitmap);
#if 0
    }
#endif

ErrExit4:
    LocalFree(pbmi);
ErrExit3:
    CloseHandle(hMapFile);
ErrExit2:
    CloseHandle(hFile);
ErrExit1:

    return (hBitmap);

}


/******************************Public*Routine******************************\
*
* bFreeDibFile
*
* Effects:
*
* Warnings:
*
* History:
*  09-Feb-1993 -by- Petrus Wong
* Wrote it.
\**************************************************************************/

BOOL bFreeDibFile(PDIBDATA pDibData)
{
    ULONG               ulFiles;
    ULONG               ulFrames;
    ULONG               i;
    PFILEINFO           pFileInfo;

    ulFiles = pDibData->ulFiles;
    ulFrames = pDibData->ulFrames;

    for (i = 0; i < ulFrames; i++) {
        LocalFree(pDibData->rgpjFrame[i]);
        LocalFree(pDibData->rgpbmi[i]);
    }

    for (i = 0; i < ulFiles; i++) {
        pFileInfo = &(pDibData->rgFileInfo[i]);
        CloseHandle(pFileInfo->hFile);
        CloseHandle(pFileInfo->hMapFile);
        UnmapViewOfFile(pFileInfo->lpvMapView);
    }

    pDibData->ulFiles = 0;
    pDibData->ulFrames = 0;
    return TRUE;
}




/******************************Public*Routine******************************\
*
* bGetBMP
*
* Effects: call common dialog and pass the filename to hBmpLoadBitmapFile
*          return TRUE if successful, FALSE otherwise
*
* Warnings:
*
* History:
*  13-May-1992 -by- Petrus Wong
* Wrote it.
\**************************************************************************/

BOOL bGetBMP(HWND hwnd, BOOL bMask) {
    OPENFILENAME    ofn;
    char            szFile[256], szFileTitle[256];
    static char     *szFilter;
    BOOL            bSuccess;
    HDC             hDC;

    bSuccess = FALSE;

    szFilter =
      "DIB files (*.bmp)\0*.bmp\0RLE files (*.rle)\0*.rle\0\0";

    strcpy(szFile, "*.bmp\0");
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = szFilter;
    ofn.lpstrCustomFilter = (LPSTR) NULL;
    ofn.nMaxCustFilter = 0L;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFileTitle = szFileTitle;
    ofn.nMaxFileTitle = sizeof(szFileTitle);
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = (bMask ? "Load Mask" : "Load Bitmap");
    ofn.Flags = 0L;
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = "BMP";

    if (!GetOpenFileName(&ofn))
        return 0L;

    hDC = GetDC(ghwndDrawSurf);
    if (bMask) {
        ghBmpMask = hBmpLoadBitmapFile(hDC, szFile);
        if (ghBmpMask != NULL)
            bSuccess = TRUE;
    } else {
        ghBmp = hBmpLoadBitmapFile(hDC, szFile);
        if (ghBmp != NULL)
            bSuccess = TRUE;
    }
    ReleaseDC(ghwndDrawSurf, hDC);

    return bSuccess;
}

/******************************Public*Routine******************************\
*
* bHitTest
*
* Effects:  Enumerates metafile records
*           Calling bDoHitTest to process each record found.
*               The mouse position is passed to the bDoHitTest
*
* Warnings:
*
* History:
*  20-May-1992 -by- Petrus Wong
* Wrote it.
\**************************************************************************/

BOOL bHitTest(HDC hDC, INT x, INT y) {
    BOOL          bSuccess;
    ENHMETAHEADER EnhMetaHdr;
    RECT          rcClientDS;
    HTDATA        htData;
    static        HCURSOR hCurHT, hCurWait;

    bSuccess = TRUE;

    if (ghMetaf == 0)
        return 0L;

    hCurHT = LoadCursor(NULL, IDC_CROSS);
    hCurWait = LoadCursor(NULL, IDC_WAIT);

    GetEnhMetaFileHeader(ghMetaf, sizeof(EnhMetaHdr), &EnhMetaHdr);

    htData.point.x = x;
    htData.point.y = y;
    htData.iRecord = EnhMetaHdr.nRecords;

    SetCursor(hCurWait);
    if (gbFit2Wnd) {
        GetClientRect(ghwndDrawSurf, &rcClientDS);
        EnumEnhMetaFile(hDC, ghMetaf, (ENHMFENUMPROC)bDoHitTest, (LPVOID) &htData, (LPRECT)&rcClientDS);
    } else {
        RECT rc;

        rc.top = rc.left = 0;
        rc.right = EnhMetaHdr.rclBounds.right - EnhMetaHdr.rclBounds.left;
        rc.bottom = EnhMetaHdr.rclBounds.bottom - EnhMetaHdr.rclBounds.top;
        EnumEnhMetaFile(hDC, ghMetaf, (ENHMFENUMPROC)bDoHitTest, (LPVOID) &htData, (LPRECT)&rc);
    }
    SetCursor(hCurHT);

    return bSuccess;
}

/******************************Public*Routine******************************\
*
* bDoHitTest
*
* Effects:      Play all records related to transformation
*               Remember new mouse position if the record is a MoveTo
*               Convert rectangle, ellipse, lineto and bezier to path
*               Widen the path and convert it to region.
*               Test if the mouse position is inside the region.
*
* Warnings:     Only handle rectangle, ellipse, line and polybezier
*
* History:
*  20-May-1992 -by- Petrus Wong
* Wrote it.
\**************************************************************************/

BOOL APIENTRY bDoHitTest(HDC hDC, LPHANDLETABLE lpHandleTable,
                                  LPENHMETARECORD lpEnhMetaRecord,
                                  UINT nHandles,
                                  LPVOID lpData) {
    BOOL            bSuccess;
    char            ach[128];
    char            achTmp[128];
    POINT           PtOrg;
    LONG            lNumDword;
    XFORM           xfSave;
    SIZE            SizeWndEx, SizeViewEx;
    POINT           ptWndOrgin, ptViewOrgin;
    int             i, iMode;
    HRGN            hRgn;
    PPOINT          pPt, pPtTmp;
    static HGDIOBJ  hObjOld=NULL;
    static LONG     lCurX=0;
    static LONG     lCurY=0;
    static BOOL     bXform=FALSE;
    static int      iCnt=0;

    iCnt++;

    //
    // select a wide pen for widen path later on
    //
    hObjOld = SelectObject(hDC, ghpnWide);

    //
    // save the mouse hit position, this was passed in as a POINT structure
    //
    PtOrg.x = (((HTDATA *)lpData)->point).x;
    PtOrg.y = (((HTDATA *)lpData)->point).y;

    //
    // save the number of parameters for the GDI fcn concerned in DWORD.
    // This is the total size of metafile record in question less the
    // size of the GDI function
    //
    lNumDword = (lpEnhMetaRecord->nSize-8) / 4;

    switch (lpEnhMetaRecord->iType) {
    case MR_SETWINDOWEXTEX:
    case MR_SETWINDOWORGEX:
    case MR_SETVIEWPORTEXTEX:
    case MR_SETVIEWPORTORGEX:
    case MR_SETMAPMODE:
    case MR_SCALEVIEWPORTEXTEX:
    case MR_SCALEWINDOWEXTEX:
    case MR_SETMETARGN:
    case MR_SAVEDC:
    case MR_RESTOREDC:
    case MR_SETWORLDTRANSFORM:
    case MR_MODIFYWORLDTRANSFORM: {
        //
        // play all records related to transformation & font
        //
        PlayEnhMetaFileRecord(hDC, lpHandleTable,
                                   lpEnhMetaRecord, nHandles);
        bXform = TRUE;
        return TRUE;
    }
    //
    // convert the following GDI calls to path for hit testing
    //
    case MR_RECTANGLE: {
        BeginPath(hDC);
        Rectangle(hDC, lpEnhMetaRecord->dParm[0], lpEnhMetaRecord->dParm[1],
                       lpEnhMetaRecord->dParm[2], lpEnhMetaRecord->dParm[3]);
        EndPath(hDC);
        break;
    }
    case MR_ELLIPSE: {
        BeginPath(hDC);
        Ellipse(hDC, lpEnhMetaRecord->dParm[0], lpEnhMetaRecord->dParm[1],
                     lpEnhMetaRecord->dParm[2], lpEnhMetaRecord->dParm[3]);
        EndPath(hDC);
        break;
    }
    case MR_MOVETOEX: {
        //
        // Remember our current position
        //
        lCurX = lpEnhMetaRecord->dParm[0];
        lCurY = lpEnhMetaRecord->dParm[1];
        return TRUE;
    }
    case MR_LINETO: {
        BeginPath(hDC);
        MoveToEx(hDC, lCurX, lCurY, NULL);
        LineTo(hDC, lpEnhMetaRecord->dParm[0], lpEnhMetaRecord->dParm[1]);
        EndPath(hDC);
        break;
    }
    case MR_POLYBEZIER16: {
        int         i;
        LONG        lSize;
        LONG        lPtCnt;

        lPtCnt = lpEnhMetaRecord->dParm[4];
        lSize = lPtCnt * sizeof(POINTL);

        if ((pPt = (PPOINT) LocalAlloc(LMEM_FIXED, lSize)) == NULL) {
            SetWindowText(ghTextWnd, "ERROR: Failed in Memory Allocation: NO HIT");
            return TRUE;
        }

        pPtTmp = pPt;

        for (i=0; i < (INT) lPtCnt; i++, pPtTmp++) {
            pPtTmp->x = (LONG)(LOWORD(lpEnhMetaRecord->dParm[i+5]));
            pPtTmp->y = (LONG)(HIWORD(lpEnhMetaRecord->dParm[i+5]));
        }

        BeginPath(hDC);
        PolyBezier(hDC, (LPPOINT)pPt, (DWORD) lPtCnt);
        EndPath(hDC);
        LocalFree(pPt);
        break;
    }
    default:
        wsprintf((LPSTR) ach, "NO HIT: I don't Hit-Test %s", rgMetaName[lpEnhMetaRecord->iType]);
        SetWindowText(ghTextWnd, ach);
        return TRUE;
    }   //switch

    if (bXform) {
        //
        // Set World transform to identity temporarily so that pen width
        // is not affected by world to page transformation
        //
        GetWorldTransform(hDC, &xfSave);
        ModifyWorldTransform(hDC, NULL, MWT_IDENTITY);

        //
        // Set Page transform to identity temporarily so that pen width
        // is not affected by page to device transformation
        //
        iMode = GetMapMode(hDC);

        if ((iMode == MM_ISOTROPIC) || (iMode == MM_ANISOTROPIC)) {
            GetWindowOrgEx(hDC, &ptWndOrgin);
            GetWindowExtEx(hDC, &SizeWndEx);
            GetViewportExtEx(hDC, &SizeViewEx);
            GetViewportOrgEx(hDC, &ptViewOrgin);
        }

        SetMapMode(hDC, MM_TEXT);
    }

    WidenPath(hDC);

    hRgn = PathToRegion(hDC);

    if (hRgn == 0) {
        SetWindowText(ghTextWnd, "ERROR: Null Region: NO HIT");
        DeleteObject(hRgn);
        return TRUE;
    }
    //DPtoLP(hDC, &PtOrg, 1);
    //SetPixel(hDC, PtOrg.x, PtOrg.y, RGB(0, 255, 0));
    //
    // test if mouse hit position is in region
    //
    bSuccess = PtInRegion(hRgn, PtOrg.x, PtOrg.y);
    //Temporily comment this out
    FillRgn(hDC, hRgn, ghbrRed);
    DeleteObject(hRgn);
    //
    // Set transform back.
    //
    if (bXform) {
        SetWorldTransform(hDC, &xfSave);
        SetMapMode(hDC, iMode);

        if ((iMode == MM_ISOTROPIC) || (iMode == MM_ANISOTROPIC)) {
            SetWindowOrgEx(hDC, ptWndOrgin.x, ptWndOrgin.y, NULL);
            SetWindowExtEx(hDC, SizeWndEx.cx, SizeWndEx.cy, NULL);
            SetViewportExtEx(hDC, SizeViewEx.cx, SizeViewEx.cy, NULL);
            SetViewportOrgEx(hDC, ptViewOrgin.x, ptViewOrgin.y, NULL);
        }
    }

    if (bSuccess) {
        Beep(440, 500);
        //
        // Reporting the metafile record number.  Then reset counter.
        //
        SetDlgItemInt(ghwndCtrlPanel, DID_COUNTER, iCnt, FALSE);
        iCnt=0;
        wsprintf((LPSTR) ach, "HIT %s", rgMetaName[lpEnhMetaRecord->iType]);

        for (i=0; i < lNumDword; i++) {
            wsprintf((LPSTR) achTmp, "%ld ", lpEnhMetaRecord->dParm[i]);
            if ((strlen(ach)+strlen(achTmp))/sizeof(char) >= 128)
                break;
            strcat(ach, achTmp);
        }

        SetWindowText(ghTextWnd, ach);
        SelectObject(hDC, hObjOld);
        bXform = FALSE;
        return FALSE;
    }
    SetWindowText(ghTextWnd, "NO HIT");
    if (iCnt >= ((HTDATA *)lpData)->iRecord)
        iCnt = 0;
    return TRUE;

    UNREFERENCED_PARAMETER(lpHandleTable);
    UNREFERENCED_PARAMETER(nHandles);

}

/******************************Public*Routine******************************\
*
* bChooseNewFont
*
* Effects:
*
* Warnings:
*
* History:
*  20-May-1992 -by- Petrus Wong
* Wrote it.
\**************************************************************************/

BOOL bChooseNewFont(HWND hwnd, PLOGFONT plf, COLORREF *pClrRef) {
   HDC                  hDC;
   static CHOOSEFONT    chf;
   static BOOL          bInit=TRUE;


   if (bInit) {
        bInit = FALSE;

        hDC = GetDC( hwnd );
        chf.hDC = CreateCompatibleDC( hDC );
        ReleaseDC( hwnd, hDC );

        chf.lStructSize = sizeof(CHOOSEFONT);
        chf.hwndOwner = hwnd;
        chf.lpLogFont = plf;
        chf.Flags = CF_SCREENFONTS | CF_EFFECTS | CF_INITTOLOGFONTSTRUCT;
        chf.rgbColors = *pClrRef;
        chf.lCustData = 0;
        chf.hInstance = (HANDLE)NULL;
        chf.lpszStyle = (LPSTR)NULL;
        chf.nFontType = SCREEN_FONTTYPE;
        chf.nSizeMin = 0;
        chf.nSizeMax = 0;
        chf.lpfnHook = (LPCFHOOKPROC)NULL;
        chf.lpTemplateName = (LPSTR)NULL;
   }

   if (ChooseFont( &chf ) == FALSE ) {
        DeleteDC( hDC );
	return FALSE;
   }

   *pClrRef = chf.rgbColors;

   DeleteDC( hDC );
   return (TRUE);
}

/******************************Public*Routine******************************\
*
* bChooseNewColor
*
* Effects:  Returns TRUE if successful; lpdwRGB points the color selected.
*           Otherwise, FALSE.
*
* Warnings:
*
* History:
*  21-May-1992 -by- Petrus Wong
* Wrote it.
\**************************************************************************/

BOOL bChooseNewColor(HWND hwnd, LPDWORD lpdwRGB) {
    static DWORD argbCust[16] = {
        RGB(255, 255, 255), RGB(255, 255, 255),
        RGB(255, 255, 255), RGB(255, 255, 255),
        RGB(255, 255, 255), RGB(255, 255, 255),
        RGB(255, 255, 255), RGB(255, 255, 255),
        RGB(255, 255, 255), RGB(255, 255, 255),
        RGB(255, 255, 255), RGB(255, 255, 255),
        RGB(255, 255, 255), RGB(255, 255, 255),
        RGB(255, 255, 255), RGB(255, 255, 255)
    };
    CHOOSECOLOR cc;
    BOOL bResult;

    cc.lStructSize = sizeof(CHOOSECOLOR);
    cc.hwndOwner = hwnd;
    cc.hInstance = ghModule;
    cc.rgbResult = *lpdwRGB;
    cc.lpCustColors = argbCust;
    cc.Flags = CC_RGBINIT | CC_SHOWHELP;
    cc.lCustData = 0;
    cc.lpfnHook = NULL;
    cc.lpTemplateName = NULL;

    bResult = ChooseColor(&cc);

    if (bResult) {
        *lpdwRGB = cc.rgbResult;
        return TRUE;
    }

    return FALSE;
}


/******************************Public*Routine******************************\
*
* hBrCreateBrush
*
* Effects: Creates a brush with the specified RGB
*
* Warnings:
*
* History:
*  04-Mar-1992 -by- Petrus Wong
* Wrote it.
\**************************************************************************/

HBRUSH hBrCreateBrush(HDC hDC, DWORD dwRGB)
{
    HDC hdcMem;
    HBRUSH hbr;
    HBRUSH hbrOld;
    HBITMAP hbmPat;
    HBITMAP hbmOld;

    hbr = CreateSolidBrush(dwRGB);
    hdcMem = CreateCompatibleDC(hDC);

    //
    // Minimum size for a bitmap to be used in a fill pattern is 8x8
    //
    hbmPat = CreateCompatibleBitmap(hDC, 8, 8);

    hbmOld = SelectObject(hdcMem, hbmPat);
    hbrOld = SelectObject(hdcMem, hbr);
    PatBlt(hdcMem, 0, 0, 8, 8, PATCOPY);

    //
    // Deselect hbmPat and hbr
    //
    SelectObject(hdcMem, hbmOld);
    SelectObject(hdcMem, hbrOld);

    DeleteDC(hdcMem);
    DeleteObject(hbr);

    hbr = CreatePatternBrush(hbmPat);

    DeleteObject(hbmPat);

    return hbr;
}


/******************************Public*Routine******************************\
*
* bPrintMf  Brings up the print dialog for printer setup and then
*           starts printing the enhanced metafile.
*
*           pPD     Points to a PRTDATA structure that contains the
*                   the handle for the Enh. Metafile for printing.
*
* Effects:  Returns TRUE if sucessful.  Otherwise, it is FALSE.
*           GlobalFree pPD when exits.
*
* Warnings:
*
* History:
*  22-Oct-1992 -by- Petrus Wong
* Wrote it.
\**************************************************************************/

BOOL bPrintMf(PPRTDATA pPD) {
    DOCINFO         DocInfo;
    HDC             hDCPrinter;
    ENHMETAHEADER   EnhMetaHdr;
    HENHMETAFILE    hEnhMf;
    TCHAR           buf[128];
    PRINTDLG        pd;
    BOOL            bSuccess;
    int             iEntries;
    PLOGPALETTE     plogPal;
    PBYTE           pjTmp;
    HPALETTE        hPal;


    bSuccess = TRUE;

    if (pPD->hMetaf == 0) {
        SetWindowText(ghTextWnd, "NO Metafile to print");
        goto PMF_EXIT;
        bSuccess = FALSE;
    }

    hEnhMf = CopyEnhMetaFile(pPD->hMetaf, NULL);
    pd.lStructSize = sizeof(PRINTDLG);
    pd.hwndOwner   = ghwndMain;
    pd.Flags       = PD_RETURNDC;
    pd.hInstance   = ghModule;

    if (!PrintDlg(&pd)) {
        SetWindowText(ghTextWnd, "Cancel Printing");
        goto PMF_EXIT;
        bSuccess = FALSE;
    }


    if (pd.hDC == NULL) {
        SetWindowText(ghTextWnd, "Failed in creating printer DC");
        goto PMF_EXIT;
        bSuccess = FALSE;
    }

    hDCPrinter = pd.hDC;
    GetEnhMetaFileDescription(hEnhMf, 128, (LPTSTR)buf);

    DocInfo.cbSize      = sizeof(DOCINFO);
    DocInfo.lpszDocName = (LPTSTR) buf;
    DocInfo.lpszOutput  = NULL;
    StartDoc(hDCPrinter, &DocInfo);
    StartPage(hDCPrinter);

    SetWindowText(ghTextWnd, "Printing...");

    iEntries = GetEnhMetaFilePaletteEntries(hEnhMf, 0, NULL);

    if (iEntries) {
        if ((plogPal = (PLOGPALETTE)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
                sizeof(DWORD) + sizeof(PALETTEENTRY)*iEntries )) == NULL) {
            MessageBox(ghwndMain, "Failed in Creating Palette!", "Error", MB_OK);
        }

        plogPal->palVersion = 0x300;
        plogPal->palNumEntries = (WORD) iEntries;
        pjTmp = (PBYTE) plogPal;
        pjTmp += 8;

        GetEnhMetaFilePaletteEntries(hEnhMf, iEntries, (PPALETTEENTRY)pjTmp);
        hPal = CreatePalette(plogPal);
        GlobalFree(plogPal);

        SelectPalette(hDCPrinter, hPal, FALSE);
        RealizePalette(hDCPrinter);
    }

    if (pPD->bFit2Wnd) {
        int     iWidth, iHeight;
        RECT    rc;

        iWidth = GetDeviceCaps(hDCPrinter, HORZRES);
        iHeight = GetDeviceCaps(hDCPrinter, VERTRES);
        rc.left = rc.top = 0;
        rc.right = iWidth;
        rc.bottom = iHeight;
        bSuccess = PlayEnhMetaFile(hDCPrinter, hEnhMf, (LPRECT) &rc);
        if (!bSuccess) {
            char    text[128];

            wsprintf(text, "Fail in PlayEnhMetaFile! Error %ld\n", GetLastError());
            OutputDebugString(text);
        }


    } else {
        GetEnhMetaFileHeader(hEnhMf, sizeof(ENHMETAHEADER), &EnhMetaHdr);
        {
        RECT rc;

        rc.top = rc.left = 0;
        rc.right = EnhMetaHdr.rclBounds.right - EnhMetaHdr.rclBounds.left;
        rc.bottom = EnhMetaHdr.rclBounds.bottom - EnhMetaHdr.rclBounds.top;
        bSuccess = PlayEnhMetaFile(hDCPrinter, hEnhMf, (LPRECT) &rc);
        if (!bSuccess) {
            char    text[128];

            wsprintf(text, "Fail in PlayEnhMetaFile! Error %ld\n", GetLastError());
            OutputDebugString(text);
        }

        }
    }

    EndPage(hDCPrinter);
    EndDoc(hDCPrinter);
    SetWindowText(ghTextWnd, "Printing Thread Done...");

PMF_EXIT:

    ExitThread(0);
    GlobalFree(pPD);
    return bSuccess;

}

/******************************Public*Routine******************************\
*
* bSelectDIBPal
*
* Effects: Creates a logical palette from the DIB and select it into the DC
*          and realize the palette. Saving the hPal in the ghPal
*
* Warnings: Based on Windows NT DIB support.  If PM support 16,24,32 bpp
*           we need to modify this routine.
*           Global alert! ghPal is changed here...
*
* History:
*  22-Jan-1993      Petrus Wong         PM support
*  31-Dec-1992 -by- Petrus Wong
* Wrote it.
\**************************************************************************/

BOOL bSelectDIBPal(HDC hDC, LPBITMAPINFO pbmi, BOOL bCoreHdr)
{
  LOGPALETTE    *plogPal;
  UINT          uiSizPal;
  INT           i, iNumClr;
  WORD          wBitCount;

  if (bCoreHdr) {
    wBitCount = ((LPBITMAPCOREINFO)pbmi)->bmciHeader.bcBitCount;
  } else {
    wBitCount = pbmi->bmiHeader.biBitCount;
  }

  switch (wBitCount) {
    case 16:
    case 24:
    case 32:                            // Does PM supports these?
        return FALSE;
    default:
        iNumClr = (1 << wBitCount);
        break;
  }

  uiSizPal = sizeof(WORD)*2 + sizeof(PALETTEENTRY)*iNumClr;
  if ((plogPal = (LOGPALETTE *) LocalAlloc(LMEM_FIXED,uiSizPal)) == NULL) {
      MessageBox(ghwndMain, "Fail in Allocating palette!", "Error", MB_OK);
      ghPal = NULL;
      return FALSE;
  }

  plogPal->palVersion = 0x300;
  plogPal->palNumEntries = (WORD) iNumClr;

  if (bCoreHdr) {
    for (i=0; i<iNumClr; i++) {
        plogPal->palPalEntry[i].peRed   = ((LPBITMAPCOREINFO)pbmi)->bmciColors[i].rgbtRed;
        plogPal->palPalEntry[i].peGreen = ((LPBITMAPCOREINFO)pbmi)->bmciColors[i].rgbtGreen;
        plogPal->palPalEntry[i].peBlue  = ((LPBITMAPCOREINFO)pbmi)->bmciColors[i].rgbtBlue;
        plogPal->palPalEntry[i].peFlags = PC_RESERVED;
    }
  } else {
    for (i=0; i<iNumClr; i++) {
        plogPal->palPalEntry[i].peRed   = pbmi->bmiColors[i].rgbRed;
        plogPal->palPalEntry[i].peGreen = pbmi->bmiColors[i].rgbGreen;
        plogPal->palPalEntry[i].peBlue  = pbmi->bmiColors[i].rgbBlue;
        plogPal->palPalEntry[i].peFlags = PC_RESERVED;
    }
  }

  DeleteObject(ghPal);
  ghPal = CreatePalette((LPLOGPALETTE)plogPal);
  if ((ghPal) == NULL) {
      MessageBox(ghwndMain, "Fail in creating palette!", "Error", MB_OK);
      return FALSE;
  }

  if ((GetDeviceCaps(hDC, RASTERCAPS)) & RC_PALETTE) {
    SelectPalette(hDC, ghPal, FALSE);
    RealizePalette(hDC);
  }

  GlobalFree(plogPal);

  return TRUE;
}


/******************************Public*Routine******************************\
*
* bPlgBlt
*
* Effects:  If Source DIB bpp > Destination DC's
*           use Halftone for PlgBlt.
*
* Warnings: Global Alert!
*           gbUseDIB is always TRUE now.
*
* History:
*  12-Mar-1993      Petrus Wong     fixed clr problem on playback (non-HT)
*  18-Feb-1993      Petrus Wong     fixed clr problem on playback (HT)
*  10-Feb-1993 -by- Petrus Wong
* Wrote it.
\**************************************************************************/

BOOL bPlgBlt(HDC hDC, LPPOINT rgPtsBMP)
{
    HDC                  hDCRef;
    HDC                  hDCSrn;                // hDC can be metaf DC
    HGDIOBJ              hObjOld, hBmpMem;
    BITMAP               bm;
    INT                  iBpp;
    WORD                 wBitCnt;


    hDCSrn = GetDC(ghwndDrawSurf);
    hDCRef = CreateCompatibleDC(hDC);

    if (gbUseDIB) {
        int         cx, cy, dx, dy;
        PBITMAPINFO pbmi;

        pbmi = (gDib.rgpbmi[0]);
        dx = rgPtsBMP[0].x - rgPtsBMP[1].x;
        dy = rgPtsBMP[0].y - rgPtsBMP[1].y;
        cx = (INT) sqrt( dx * dx + dy * dy );

        dx = rgPtsBMP[0].x - rgPtsBMP[2].x;
        dy = rgPtsBMP[0].y - rgPtsBMP[2].y;
        cy = (INT) sqrt( dx * dx + dy * dy );

        iBpp = GetDeviceCaps(hDC, BITSPIXEL);

        if (gDib.rgbCoreHdr[0]) {
            wBitCnt = ((LPBITMAPCOREINFO)pbmi)->bmciHeader.bcBitCount;
        } else {
            wBitCnt = pbmi->bmiHeader.biBitCount;
        }

        if (iBpp < wBitCnt) {   // Do Halftone
            SetStretchBltMode(hDCRef, HALFTONE);
            if (ghHT) {
                SelectPalette(hDCRef, ghHT, FALSE);
                SelectPalette(hDC, ghHT, FALSE);
                SelectPalette(hDCSrn, ghHT, FALSE); // hDC can be metaf DC
                RealizePalette(hDCSrn);             // always realize the srn DC

                // Don't have to realize the palette in hDCRef
                // RealizePalette(hDCRef);

                // has to be compatible with screen DC, cannot be hDCRef
                // memory DC has no bitmap by default?
                // hDC may be a metafile DC, so use hDCSrn
                hBmpMem = CreateCompatibleBitmap(hDCSrn, cx, cy);
                SelectObject(hDCRef, hBmpMem);
            } else {
                MessageBox(ghwndMain, "Halftone palette is null!", "Error", MB_OK);
            }
        } else {
            SetStretchBltMode(hDCRef, COLORONCOLOR);
            if (ghPal) {
                if (ghDCMetaf == hDC)
                    CopyPalette(ghPal);
                SelectPalette(hDCRef, ghPal, FALSE);
                SelectPalette(hDC, ghPal, FALSE);
                SelectPalette(hDCSrn, ghPal, FALSE); // hDC can be metaf DC
                RealizePalette(hDCSrn);             // always realize the srn DC

                // Don't have to realize the palette in hDCRef
                // RealizePalette(hDCRef);

                // has to be compatible with screen DC, cannot be hDCRef
                // memory DC has no bitmap by default?
                // hDC may be a metafile DC, so use hDCSrn
                hBmpMem = CreateCompatibleBitmap(hDCSrn, cx, cy);
                SelectObject(hDCRef, hBmpMem);
            } else {
                MessageBox(ghwndMain, "Palette is null!", "Error", MB_OK);
            }
        }

        if (gDib.rgbCoreHdr[0]) {
            StretchDIBits(hDCRef, 0,0, cx, cy,
                          0,0, ((LPBITMAPCOREINFO)pbmi)->bmciHeader.bcWidth, ((LPBITMAPCOREINFO)pbmi)->bmciHeader.bcHeight,
                          gDib.rgpjFrame[0], pbmi, DIB_RGB_COLORS, SRCCOPY);
        } else {
            StretchDIBits(hDCRef, 0,0, cx, cy,
                          0,0, pbmi->bmiHeader.biWidth, pbmi->bmiHeader.biHeight,
                          gDib.rgpjFrame[0], pbmi, DIB_RGB_COLORS, SRCCOPY);
        }

        PlgBlt(hDC, rgPtsBMP, hDCRef, 0, 0, cx, cy,
               ghBmpMask, 0, 0);

        DeleteObject(hBmpMem);

    } else {
        hObjOld = SelectObject(hDCRef, ghBmp);

        GetObject(ghBmpMask, sizeof(BITMAP), (LPSTR)&bm);
        if (bm.bmBitsPixel != 1) {
            SetWindowText(ghTextWnd, "ERROR: Mask has to be a Monochrome bitmap!");
            ghBmpMask = NULL;
        }

        GetObject(ghBmp, sizeof(BITMAP), (LPSTR)&bm);

        if (ghPal) {
            SelectPalette(hDC, ghPal, FALSE);
            RealizePalette(hDC);
            SetStretchBltMode(hDC, COLORONCOLOR);
        }
        PlgBlt(hDC, rgPtsBMP, hDCRef, 0, 0, bm.bmWidth, bm.bmHeight,
               ghBmpMask, 0, 0);

        SelectObject(hDCRef, hObjOld);
    }

    DeleteDC(hDCRef);
    ReleaseDC(ghwndDrawSurf, hDCSrn);
    return TRUE;

}



/******************************Public*Routine******************************\
*
* HPALETTE CopyPalette
*
* Effects:
*
* Warnings:
*
* History:
*  18-Sep-1992 -by- Petrus Wong
* Wrote it.
\**************************************************************************/

HPALETTE CopyPalette(HPALETTE hPalSrc)
{
    PLOGPALETTE     plogPal;
    PBYTE           pjTmp;
    int             iNumEntries=0;
    HPALETTE        hPal;

    if ((iNumEntries = GetPaletteEntries(hPalSrc, 0, iNumEntries, NULL)) == 0) {
        MessageBox(ghwndMain, "No entry in palette to copy!", "Error", MB_OK);
        return (HPALETTE) NULL;
    }

    if ((plogPal = (PLOGPALETTE)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
            sizeof(DWORD) + sizeof(PALETTEENTRY)*iNumEntries )) == NULL) {
        MessageBox(ghwndMain, "Failed in CopyPalette!", "Error", MB_OK);
        return (HPALETTE) NULL;
    }

    plogPal->palVersion = 0x300;
    plogPal->palNumEntries = (WORD) iNumEntries;
    pjTmp = (PBYTE) plogPal;
    pjTmp += 8;
    GetPaletteEntries(hPalSrc, 0, iNumEntries, (PPALETTEENTRY)pjTmp);
    hPal = CreatePalette(plogPal);

    GlobalFree(plogPal);

    return hPal;
}




/******************************Public*Routine******************************\
*
* iTT
*
* Effects: set the global variable gbTT if the family is true type
*
* Warnings:
*
* History:
*  29-Apr-1993 -by- Petrus Wong
* Wrote it.
\**************************************************************************/

int CALLBACK iTT(
    LPLOGFONT    lpLF,
    LPTEXTMETRIC lpTM,
    DWORD        dwFontType,
    LPARAM       lpData)
{

    if (lpTM->tmPitchAndFamily & TMPF_TRUETYPE) {
        //OutputDebugString("TRUETYPE\n");
        *((BOOL *)lpData) = TRUE;
    } else {
        //OutputDebugString("NON-TRUETYPE\n");
        *((BOOL *)lpData) = FALSE;
    }

#if 0
    //
    // that's equivalent
    //
    if (dwFontType & TRUETYPE_FONTTYPE) {
        //OutputDebugString("TRUETYPE\n");
        *((BOOL *)lpData) = TRUE;
    } else {
        //OutputDebugString("NON-TRUETYPE\n");
        *((BOOL *)lpData) = FALSE;
    }
#endif
    return 0;

    UNREFERENCED_PARAMETER (lpLF);
    //UNREFERENCED_PARAMETER (lpTM);
    UNREFERENCED_PARAMETER (dwFontType);

}



/******************************Public*Routine******************************\
*
* CMTMLTFMT *pLoadMltFmtFile(VOID)
*
* Effects:  Load either EPS or enh mf
*
* Warnings: CR! change this to load multiple def of picture
*
* History:
*  16-Aug-1993 -by- Petrus Wong
* Wrote it.
\**************************************************************************/

CMTMLTFMT *pLoadMltFmtFile(VOID)
{
    OPENFILENAME    ofn;
    char            szDirName[256];
    char            szFile[256], szFileTitle[256];
    static char     *szFilter;
    HANDLE          hFile, hMapFile;
    LPVOID          pMapFile;
    DWORD           dwFileSizeLow, dwFileSizeHigh;
    CMTMLTFMT       *pMfmt;

    pMfmt = (CMTMLTFMT*)NULL;

    szFilter =
      "EPS files (*.eps)\0*.eps\0Enhanced Metafiles (*.emf)\0*.emf\0\0";

    GetSystemDirectory((LPSTR) szDirName, 256);
    strcpy(szFile, "*.eps\0");
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = GetFocus();
    ofn.lpstrFilter = szFilter;
    ofn.lpstrCustomFilter = (LPSTR) NULL;
    ofn.nMaxCustFilter = 0L;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFileTitle = szFileTitle;
    ofn.nMaxFileTitle = sizeof(szFileTitle);
    ofn.lpstrInitialDir = szDirName;
    ofn.lpstrTitle = (LPSTR) NULL;
    ofn.Flags = 0L;
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = "EPS";

    if (!GetOpenFileName(&ofn)) {
        goto EXIT;
    }

    if ((hFile = CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL)) == (HANDLE)-1) {
        goto EXIT;
    }

    dwFileSizeLow = GetFileSize(hFile, &dwFileSizeHigh);
    if ((dwFileSizeLow == 0xFFFFFFFF) && (GetLastError() != NO_ERROR)) {
        goto EXIT;
    }

    //
    // Create a map file of the opened file
    //
    if ((hMapFile = CreateFileMapping(hFile, NULL,
                             PAGE_READONLY, 0, 0, NULL)) == (HANDLE)-1) {
        goto EXIT2;
    }

    //
    // Map a view of the whole file
    //
    if ((pMapFile = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0)) == NULL) {
        goto EXIT3;
    }

//
// CR!! In future, change this to load different def of the picture...
//
  {
    ULONG       ulSize;
    PBYTE       pjTmp;
    RECTL       rectl;

    ulSize = dwFileSizeLow+sizeof(CMTMLTFMT);
    if ((pMfmt = (CMTMLTFMT *) LocalAlloc(LMEM_FIXED, ulSize)) == NULL) {
        MessageBox(GetFocus(), "Fail in Memory Allocation!", "Error", MB_OK);
        goto EXIT3;
    }

    pMfmt->ident = GDICOMMENT_IDENTIFIER;
    pMfmt->iComment = GDICOMMENT_MULTIFORMATS;
    pMfmt->nFormats = 1;
    pMfmt->aemrformat[0].cbData = dwFileSizeLow;
    pMfmt->aemrformat[0].offData = 11*sizeof(DWORD);

    // parse for %!PS-Adobe-3.0 EPSF keyword
    //           Enhanced Metafile signature
    // set EMRFORMAT.dSignature appropiately

    if (((ENHMETAHEADER *) pMapFile)->dSignature == ENHMETA_SIGNATURE) {

        pMfmt->aemrformat[0].dSignature = ENHMETA_SIGNATURE;
        pMfmt->aemrformat[0].nVersion = 0;                    // not for emf
        pMfmt->rclOutput.left   = ((ENHMETAHEADER *) pMapFile)->rclBounds.left;
        pMfmt->rclOutput.top    = ((ENHMETAHEADER *) pMapFile)->rclBounds.top;
        pMfmt->rclOutput.right  = ((ENHMETAHEADER *) pMapFile)->rclBounds.right;
        pMfmt->rclOutput.bottom = ((ENHMETAHEADER *) pMapFile)->rclBounds.bottom;
    }
    else    //assume it is Adobe EPS
    if (bGetEPSBounds(pMapFile, &rectl)) {

        char text[128];


        pMfmt->aemrformat[0].dSignature = 0x46535045;
        pMfmt->aemrformat[0].nVersion = 1;
        pMfmt->rclOutput.left   = rectl.left;
        pMfmt->rclOutput.top    = rectl.top;
        pMfmt->rclOutput.right  = rectl.right;
        pMfmt->rclOutput.bottom = rectl.bottom;

        wsprintf(text, "Bounds = %d %d %d %d",
                 rectl.left, rectl.top, rectl.right, rectl.bottom);
        MessageBox(GetFocus(), text, "Bounds", MB_OK);

    }
    else {
        // unknown file type
        Free(pMfmt);
        pMfmt = NULL;
        goto EXIT3;
    }

    pjTmp = (PBYTE)(((DWORD *)pMfmt->aemrformat)+4);
    while (dwFileSizeLow--) {
        *(((PBYTE)pjTmp)++) = *(((PBYTE)pMapFile)++);
    }

  }


EXIT3:
    CloseHandle(hMapFile);
EXIT2:
    CloseHandle(hFile);
EXIT:

    return pMfmt;
}


HLOCAL Free(CMTMLTFMT *pMfmt) {
    return LocalFree(pMfmt);
}

#define DBG 0

BOOL bIsAdobe(char *szStr)
{
    if (strcmp(szStr, "%!PS-Adobe-3.0") == 0)
        return TRUE;
    else
        return FALSE;
}

BOOL bIsEPS(char *szStr)
{
    if ((strcmp(szStr, "EPSF-3.0") == 0) ||
        (strcmp(szStr, "EPSF-2.0") == 0))
        return TRUE;
    else
        return FALSE;
}

BOOL bIsBndBox(char *szStr)
{
    if (strcmp(szStr, "%%BoundingBox:") == 0)
        return TRUE;
    else
        return FALSE;
}

BOOL bIsEOF(char *szStr)
{
    if (strcmp(szStr, "%%EOF") == 0)
        return TRUE;
    else
        return FALSE;
}


BOOL bGetEPSBounds(LPVOID lpData, RECTL *prctl)
{
    char szKeyWord[128], szValue[128];
    int  index;


    if (lpData == NULL) {
#if DBG
        MessageBox(GetFocus(), "Null Pointer!", "Error", MB_OK);
#endif
        return FALSE;
    }

    index = 0;

    if (!bGetWord(lpData, szKeyWord, &index))
        return FALSE;

    if (!bIsAdobe(szKeyWord)) {
        MessageBox(GetFocus(), "Not Adobe!", "Error", MB_OK);
        return FALSE;
    }

    if (!bGetWord(lpData, szValue, &index))
        return FALSE;

    if (!bIsEPS(szValue)) {
        MessageBox(GetFocus(), "Not EPS!", "Error", MB_OK);
        return FALSE;
    }

    if (!bGoNextLine(lpData, &index))
        return FALSE;

    while ((bGetWord(lpData, szKeyWord, &index)) &&
           (!bIsBndBox(szKeyWord))) {
#if DBG
        MessageBox(GetFocus(), "Skip to EOL", "Error", MB_OK);
#endif
        if (!bGoNextLine(lpData, &index)) {
            MessageBox(GetFocus(), "EOF unexpectedly!", "Error", MB_OK);
            return FALSE;
        }
    }

    if (bIsBndBox(szKeyWord)) {
        if (bGetWord(lpData, szValue, &index))
            prctl->left    = atol(szValue);
        else {
            MessageBox(GetFocus(), "Fail to get bnd: left!", "Error", MB_OK);
            return FALSE;
        }

        if (bGetWord(lpData, szValue, &index))
            prctl->top     = atol(szValue);
        else {
            MessageBox(GetFocus(), "Fail to get bnd: top!", "Error", MB_OK);
            return FALSE;
        }

        if (bGetWord(lpData, szValue, &index))
            prctl->right   = atol(szValue);
        else {
            MessageBox(GetFocus(), "Fail to get bnd: right!", "Error", MB_OK);
            return FALSE;
        }

        if (bGetWord(lpData, szValue, &index))
            prctl->bottom  = atol(szValue);
        else {
            MessageBox(GetFocus(), "Fail to get bnd: bottom", "Error", MB_OK);
            return FALSE;
        }
    }

    return TRUE;

}


BOOL bGetWord(LPVOID lpData, char *str, int* pi)
{
    char *pstr;

    pstr = str;

    while (((char *)lpData)[*pi] == ' ')
        (*pi)++;

    while ((((char *)lpData)[*pi] != ' ') &&
           (((char *)lpData)[*pi] != '\n') &&
           (((char *)lpData)[*pi] != '\r')) {
        *str++ = ((char *)lpData)[(*pi)++];
    }
    *str++ = '\0';

#if DBG
    {
    char text[128];

    wsprintf(text, "bGetWord gets %s", pstr);
    MessageBox(GetFocus(), text, "Info", MB_OK);
    }
#endif

    return TRUE;
}


BOOL bGoNextLine(LPVOID lpData, int* pi)
{
    char tmp[128];
    int  q;

    while ((((char *)lpData)[*pi] != '\n') &&
           (((char *)lpData)[*pi] != '\r'))
        (*pi)++;

    //
    // skip them
    //
    *pi += 2;

    q = *pi;

    if ((bGetWord(lpData, tmp, &q)) && (bIsEOF(tmp)) )
        return FALSE;

    return TRUE;
}
