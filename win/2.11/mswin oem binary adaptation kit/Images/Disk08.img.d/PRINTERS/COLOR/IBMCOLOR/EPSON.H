#define DEV_MEMORY  0

/******************** device dependent escape sequences **********************/

#ifdef CITOH
#define HYPOTENUSE     120          /* sq root of x^2 + y^2 */
#define YMAJOR          96          /* the direction with finer resolution has less */
#define XMAJOR          72          /* distance to travel */

#else	/* Not CITOH */
#define HYPOTENUSE     140          /* sq root of x^2 + y^2 */
#define YMAJOR         120          /* the direction with finer resolution has less */
#define XMAJOR          72          /* distance to travel */
#endif

#define MAXSTYLEERR     HYPOTENUSE*2


/* sends form feed */
#define EP_FF      "\014", 1

/* line feed */
#define EP_LF      "\012", 1
#define LF_CHAR     '\012'

/* carriage return */
#define EP_CR       "\015", 1
#define EP_NL       "\012\015", 2

/* escape code for x microspace justification */
#define EP_BLANK    " ", 1
#define SPACE       ' '

#define LETTER              20
#define DINA4               21
#define FANFOLD             22
#define STANDARDPF          LETTER  /* standard paper format */

#define RIBBON_OFFSET       9       /* printer head is 1/8" from the edge of the ribbon */

#if DEVMODE_WIDEPAPER		
#define MAXPAPERFORMAT      6
#define NSMALLPAPERFORMATS  3
#else
#define MAXPAPERFORMAT      3       /* support 3 paper formats for now */
#endif

#ifdef CITOH
#define HPAGESIZE     816          /* 8.5" x 96dpi */
#define VPAGESIZE     792          /* 11"  x 72dpi */
#define HDPI          96           /* 96 dots per inch across the page */
#define VDPI          72           /* 72 DPI down the page */
#define HSIZE          8           /* 8 inches across the page*/
#define VSIZE         10           /* 10 inches down the page */
#define HOFFSET        24
#define VOFFSET        36	   /* move down by 1/2 inch */

#define DINA4_VOFFSET        60   /* center the printable area down */
#define DINA4_HOFFSET        12   /* center the printable area horizontally */
#define DINA4_HPAGESIZE     794     /* 210 mm */
#define DINA4_VPAGESIZE     841     /* 297 mm */

/* Fandfold paper */
#define FANF_VOFFSET        70    /* center the printable area vertically   */
#define FANF_HOFFSET        12    /* center the printable area horizontally */
#define FANF_HPAGESIZE     792      /* 8 1/42" */
#define FANF_VPAGESIZE     864      /* 12" */

#define NPINS     8     /* citoh fires eight pins at a time */
#define MAXBAND  15     /* number of bands */

/* PG_ACROSS must be a multiple of 16
   PG_ACROSS is the resolution of one scan line */
#define PG_ACROSS   768        /* 40 * 8 */

/* PG_DOWN must be a multiple of (8 * MAX_BAND), number of scan lines per page */
#define PG_DOWN     720
#else				/* EPSON STYLE */
/* standard page sizes and offsets for letter sized paper (R120X72) */
#define HPAGESIZE    1020           /* 8 1/2" */
#define WIDE_HPAGESIZE    1740      /* 14 1/2" */
#define VPAGESIZE     792           /* 11" */
#define HOFFSET        30
#define VOFFSET        36           /* move down by 1/2" */

/* DIN A4 paper */
#define DINA4_VOFFSET        60     /* center the printable area down */
#define DINA4_HOFFSET        15     /* center the printable area horizontally */
#define DINA4_HPAGESIZE     992     /* 210 mm */
#define DINA4_VPAGESIZE     841     /* 297 mm */

/* Fandfold paper */
#define FANF_VOFFSET        70      /* center the printable area vertically   */
#define FANF_HOFFSET        15      /* center the printable area horizontally */
#define FANF_HPAGESIZE     990      /* 8 1/42" */
#define FANF_VPAGESIZE     864      /* 12" */

#define HDPI         120            /* 120 dots per inch across the page */
#define VDPI          72            /* 72 DPI down the page */
#define HSIZE          8            /* 8 inches across the page*/
#define VSIZE         10            /* 10 inches down the page */

#define NPINS   8   /* epson fires eight pins at a time */
#define MAXBAND  15  /* number of bands */

/* PG_ACROSS must be a multiple of 16
   PG_ACROSS is the resolution of one scan line */
#define PG_ACROSS   960             /* 120 * 8 */
#define WIDE_PG_ACROSS   1584       /* 120 * 13.2 rounded to 16 pels */

/*
 * PG_DOWN must be a multiple of (t * 16), where t is the product of factor 2 of
 * MAXBAND.  This is to ensure word alignment of bitmaps.
 * It must also be divisible by (MAXBAND * NPINS).
 * Size of bitmap required for a band = (PG_ACROSS/8 * PG_DOWN/MAXBAND) BYTES
 * PG_DOWN is the number of scan lines per page 
 */
#define PG_DOWN     720
#endif /* Not a CITOH printer */

#if defined(EPSON) || defined(SG10)
#define EP_ENABLE   "\033@", 2
#define EP_NORMAL   "\033@", 2
#endif

#if defined(CITOH)
/* set line spacing to 1/9 inch i.e 8 dots */
#define EP_ENABLE   "\033T16\033>\033E", 8   

/* resets the printer back default spacing */
#define EP_NORMAL  "\033A", 2

#define EP_BIDIRECTIONAL "\033\074", 2
/* resets the printer back to character mode */

#define PICA_WIDTH     8		/* actually only have elite */
#define ELITE_WIDTH    8
#define COMP_MODE_WIDTH 8
#else

#define PICA_WIDTH   12
#define ELITE_WIDTH  10
#define COMP_MODE_WIDTH 7
#endif


/* if it is a variable pitch font, we will try to be smart and use
   a fixed pitch font with a width that is n pixels less just to be safe */

#ifdef CITOH
#define MAXDELY     48      /* max number of dots a line feed covers */
#define CHARWIDTH    8      /* only 8 dots per hardware font character */
#define VAR_PITCH_KLUDGE 2
#else
#define MAXDELY     85      /* max number of dots a line feed covers */
#define CHARWIDTH   12      /* 12 dots per hardware font character */
#define VAR_PITCH_KLUDGE 3
#endif

#if defined(EPSON) || defined(EPSONMX)
#define XMS     'L'
#define FASTXMS 'Y'

#elif defined(SG10)
#define XMS     'L'
#define FASTXMS 'y'

#elif defined(IBMGRX)
#define XMS     'L'
#define FASTXMS 'Y'
/* select in IBM char set 2 */
#define EP_CHAR2    "\0336", 2

#elif defined(TI850)
#define XMS     'L'
#define FASTXMS 'L'
#endif

#if defined(SG10)
#define TRANS_WIDTH_SIZE	28	/* size of Trans_width[] */
#elif defined(EPSON)
#define TRANS_WIDTH_SIZE	30	/* size of Trans_width[] */
#endif

/******************* end of device dependent information ********************/

#define BAND_SIZE(page_width) ((PG_DOWN / MAXBAND) * (page_width  / 8))   
						/* in bytes */

#define BUF_SIZE(page_width)   (page_width)   /* in bytes */
#define SPOOL_SIZE(page_width) (BAND_SIZE(page_width) / 2)

#define BAND_HEIGHT  (PG_DOWN / MAXBAND) /* number of y-pixels in one band */
#define BAND_LINE   (BAND_HEIGHT / NPINS)/* number of printing scan lines that can fit in a band */

#define Y_INIT_ENT   (10)   /* 10 entries, i.e 10 textout calls per band */
#define X_INIT_ENT   (4)
#define INIT_BUF    (512)   /* buffer space for textout calls */
#define MARG_BUF    (512)   /* increase by buffer space */
