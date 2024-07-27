#define abs(a)  ((a) > 0? (a): (-(a)))

typedef struct
{
    char    orientation;        /* 1 - landscape, 0 - portrait */
    char    pitch;              /* 1 - variable,  0 - fixed */
    char    facename;           /* index to facenames[] */
    char    family;             /* see adaptation guide E.2 */
    char    height;             /* in device pixels */
    char    width;              /* in device pixels */
    char    italic;             /* 1 - italic, 0 - normal font */
    char    weight;             /* in increments of 100 */
}   FONTTAB;

#define ORIENTATION_WEIGHT  14
#define PITCH_WEIGHT        11
#define FACENAME_WEIGHT     13
#define FAMILY_WEIGHT       12
#define LARGE_HEIGHT_WEIGHT  7
#define HEIGHT_WEIGHT        6
#define WIDTH_WEIGHT         4
#define ITALIC_WEIGHT        2
#define WEIGHT_WEIGHT        0

/* default: 10 pitch fixed font, pica */

#if defined(TI850) || defined(OKI92) || defined(IBMCOLOR) || defined(TOSHP351) || defined(TOSHP1351)
#define DEFAULT_FACENAME     0
#else
#define DEFAULT_FACENAME     1
#endif
#define DEFAULT_FAMILY       3
#define DEFAULT_WEIGHT       4
#if 0
#define DEFAULT_FIXED_WIDTH  12 /* smaller default average width for */
#define DEFAULT_VAR_WIDTH    10 /* variable-pitch font  */
#define DEFAULT_HEIGHT       9
#define DEFAULT_ORIENTATION   0
#endif

#if defined(IBMCOLOR)
#define TMHeight          10
#define TMAscent          8
#define TMDescent         2
#define TMInternalLeading 0
#define TMExternalLeading 4
#else
#define TMHeight          9
#define TMAscent          7
#define TMDescent         2
#define TMInternalLeading 0
#define TMExternalLeading 3
#endif
#define TMFirstChar       32
#define TMLastChar       126
#define TMBreakChar       32
#define TMDefaultChar     46

#if 0
TEXTMETRIC TMModel =
{
        TMHeight,
        TMAscent,
        TMDescent,
        TMInternalLeading,
        TMExternalLeading,
        0,      /* tmAveCharWidth - to be filled in at run time */
        0,      /* tmMaxCharWidth */
        0,      /* tmWeight */
        0,      /* tmItalic */
        0,      /* tmUnderline */
        0,      /* tmStrikeOut */
        TMFirstChar,
        TMLastChar,
        TMDefaultChar,
        TMBreakChar,
        0,      /* tmPitchAndFamily */
        0,      /* tmCharSet */
        0,      /* tmOverHang */
        VDPI,   /* tmDigitizedAspectX */
        HDPI,   /* tmDigitizedAspectY */
};

#endif

#define DIFF_CELL_CHAR      0   /* number of pixels between your cell height
                                   and character height */


#define HP_LOGFONT     1
#define HP_TEXTMETRIC  2
#define HP_FONTINFO    3
#define HP_DONTCARE    4
#define HP_TEXTXFORM   5


/* the italic proportional fonts are listed as separate fonts becuase they
   have different width tables */

#if defined(EPSON) || defined(SG10)
#define NFONTS          6
/* 2 ps fonts */
#define PSFONTS         4
#define NFACES          8

#elif defined(LQ1500)
#define NFONTS          6
#define PSFONTS         2
#define NFACES          8

#elif defined(EPSONMX) || defined(IBMGRX) || defined(TOSHP1351)
#define NFONTS          4
#define PSFONTS         0
#define NFACES          4

#elif defined(TI850) || defined(IBMCOLOR)
#define NFONTS          2
#define PSFONTS         0
#define NFACES          2

#elif defined(OKI92)
#define NFONTS          5
#define PSFONTS         0
#define NFACES          5

#elif defined(TOSHP351)
#define NFONTS          4
#define PSFONTS         2
#define NFACES          6

#elif defined(NECP2)
#define NFONTS          6
#define PSFONTS         0
#define NFACES          6

#elif defined(CITOH)
#define NFONTS          1
#define PSFONTS         0
#define NFACES          1		/* ??? */
#endif

FONTTAB FontTable[] = {
#if defined(TI850)
/* ti 850 series has compressed fonts which we cannot support */
{0x0, 0x0, 0x0, 0x3, 0x9, 0xc, 0x0, 0x4},   /* pica */
{0x0, 0x0, 0x1, 0x3, 0x9, 0x18, 0x0, 0x4},  /* pica expanded */

#elif defined(OKI92)
{0x0, 0x0, 0x0, 0x3, 0x9, 0x6, 0x0, 0x4},   /* pica */
{0x0, 0x0, 0x1, 0x3, 0x9, 0x7, 0x0, 0x4},   /* pica expanded compressed */
{0x0, 0x0, 0x2, 0x3, 0x9, 0xc, 0x0, 0x4},   /* pica expanded */
{0x0, 0x0, 0x3, 0x3, 0x9, 0x5, 0x0, 0x4},   /* elite */
{0x0, 0x0, 0x4, 0x3, 0x9, 0xa, 0x0, 0x4},   /* elite expanded */

#elif defined(CITOH)
{0x0, 0x0, 0x0, 0x3, 0x9, 0x8, 0x0, 0x4},   /* Elite (only one font used) */

#elif defined(IBMCOLOR)
{0x0, 0x0, 0x0, 0x3, 0xa, 0xe, 0x0, 0x4},   /* OCR-B */
{0x0, 0x0, 0x1, 0x3, 0xa, 0x1c, 0x0, 0x4},  /* OCR-B double-width */

#elif defined(TOSHP351) || defined(TOSHP1351)
/* throw out the compressed font - 10.5 pixels
 * {0x0, 0x0, 0x0, 0x3, 0x9, 0xa, 0x0, 0x4},   courier compressed
 * {0x0, 0x0, 0x1, 0x3, 0x9, 0x17, 0x0, 0x4},  courier compressed expanded
 */
{0x0, 0x0, 0x0, 0x3, 0x9, 0x12, 0x0, 0x4},  /* courier */
{0x0, 0x0, 0x1, 0x3, 0x9, 0x24, 0x0, 0x4},  /* courier expanded */
{0x0, 0x0, 0x2, 0x3, 0x9, 0x0f, 0x0, 0x4},  /* elite */
{0x0, 0x0, 0x3, 0x3, 0x9, 0x1e, 0x0, 0x4},  /* elite expanded */

#else	/* THIS MUST BE A BUG (very strange default) - MitchL */
{0x0, 0x0, 0x0, 0x3, 0x9, 0x7, 0x0, 0x4},   /* pica compressed */
{0x0, 0x0, 0x1, 0x3, 0x9, 0xc, 0x0, 0x4},   /* pica */
{0x0, 0x0, 0x2, 0x3, 0x9, 0xe, 0x0, 0x4},   /* pica compressed expanded */
{0x0, 0x0, 0x3, 0x3, 0x9, 0x18, 0x0, 0x4},  /* pica expanded */
#endif

#if defined(EPSON) || defined(LQ1500) || defined(SG10) || defined(NECP2)
{0x0, 0x0, 0x4, 0x3, 0x9, 0x0a, 0x0, 0x4},  /* elite */
{0x0, 0x0, 0x5, 0x3, 0x9, 0x14, 0x0, 0x4},  /* elite expanded */
#endif

#if defined(EPSON) || defined(LQ1500)
{0x0, 0x1, 0x6, 0x2, 0x9, 0x0a, 0x0, 0x4},  /* proportional */
{0x0, 0x1, 0x7, 0x2, 0x9, 0x14, 0x0, 0x4},  /* proportional expanded */
#elif defined(SG10)
{0x0, 0x1, 0x6, 0x2, 0x9, 0x0c, 0x0, 0x4},  /* proportional */
{0x0, 0x1, 0x7, 0x2, 0x9, 0x18, 0x0, 0x4},  /* proportional expanded */
#elif defined(TOSHP351)
{0x0, 0x1, 0x4, 0x1, 0x9, 0x12, 0x0, 0x4},  /* proportional */
{0x0, 0x1, 0x5, 0x1, 0x9, 0x24, 0x0, 0x4},  /* proportional expanded */
#endif

/* only enumerate the base fonts */
#if defined(EPSON)
{0x0, 0x1, 0x6, 0x2, 0x9, 0x0a, 0x1, 0x4},  /* proportional italic */
{0x0, 0x1, 0x7, 0x2, 0x9, 0x14, 0x1, 0x4},  /* proportional expanded italic */
#elif defined(LQ1500) || defined(SG10)
{0x0, 0x1, 0x6, 0x2, 0x9, 0x0c, 0x1, 0x4},  /* proportional italic */
{0x0, 0x1, 0x7, 0x2, 0x9, 0x18, 0x1, 0x4},  /* proportional expanded italic */
#endif
};


char *facenames[] = {
#if defined(TI850)
        "Pica",
        "Pica Expanded",

#elif defined(OKI92)
	"Pica",
	"Pica Expanded Compressed",
	"Pica Expanded",
	"Elite",
	"Elite Expanded", 

#elif defined (CITOH)
        "Elite",

#elif defined(IBMCOLOR)
	"OCR-B",
	"OCR-B double-width",

#elif defined(TOSHP351) || defined(TOSHP1351)
        /* "Courier Compressed",
         * "Courier Expanded Compressed",
	 */
        "Courier",
        "Courier Expanded",
        "Elite",
        "Elite Expanded",

#else
        "Pica Compressed",
        "Pica",
        "Pica Expanded Compressed",
        "Pica Expanded",
#endif

#if defined(EPSON) || defined(LQ1500) || defined(SG10) || defined(NECP2)
        "Elite",
        "Elite Expanded",
#endif

#if defined(EPSON) || defined(LQ1500) || defined(SG10) || defined(TOSHP351)
        "Proportional",
        "Proportional Expanded",
#endif
        };

void NEAR PASCAL CodeToInfo(FONTTAB *, short, LPSTR);
void NEAR PASCAL InfoToCode(short, LPLOGFONT, FONTTAB far *);

#define MYFONT      257
