/**[f******************************************************************
* segments.h -
*
* Copyright (C) 1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*
**f]*****************************************************************/
  
/* Copyright (C) Agfa Compugraphic, 1989. All rights reserved. */
  
/*  Typedefs for the various segments in a typeface library file */
/*     20-Mar-90   jfd   Added "glob_ital_tan" to FACE structure */
/*                       Added parameter definition "NO_TANGENT" */
  
  
#define NO_TANGENT   0x20000  /* Flag indicating that a screen font */
/* has been substituted and an italic */
/* tangent has been calculated, so due*/
/* not recompute the tangent using the*/
/* sin and cos                        */
  
  
#define FACE_GLOBAL_KEY   0    /* Entire face global segment */
#define GIF_KEY         100    /* Global Intellifont segment */
#define TRACK_KERN_KEY  101    /* track kerning              */
#define TEXT_KERN_KEY   102    /* text kerning               */
#define DESIGN_KERN_KEY 103    /* designer kerning           */
#define WIDTH_KEY       104    /* character width segment    */
#define ATTRIBUTE_KEY   105    /* Attribute header           */
#define RASTER_KEY      106    /* Raster parameter           */
#define TF_HEADER_KEY   107    /* Typeface Header segment    */
#define COMPOUND_KEY    108    /* Compound character         */
#define DISPLAY_KEY     109    /* Display header             */
#define FONT_ALIAS_KEY  110    /* Font Alias segment         */
#define COPYRIGHT_KEY   111    /* Copyright notice           */
  
/*---------------------------------------------------------*/
/*   Character Directory (misnamed Face Header Segment)    */
/*---------------------------------------------------------*/
typedef struct
{
    UWORD  charnum;
    ULONG  charoff;
    UWORD  charcount;
  
} CH_DIR_ENTRY;
  
  
  
/*--------------------------------*/
/*  Character Width Segment (104) */
/*--------------------------------*/
typedef struct
{
    UWORD width;
    UWORD flags;
  
} CHARWIDTH;
  
  
/*---------------------------*/
/*     dimension_type        */
/*---------------------------*/
  
typedef struct
{
    UBYTE   num_dim;        /*  number of dimensions              */
    UWORD   *value;         /*  arr of dimension values           */
    UBYTE   *attrib;        /*  arr of dim flags (RTZ = bit 0)    */
  
}  dimension_type;
  
/*---------------------------*/
/*   yclass_def_type         */
/*---------------------------*/
  
typedef struct
{
    UBYTE   num_yclass_def; /*  num of loc Y class definitions    */
    UBYTE   *yline_high_i;  /*  arr of loc Y line indices         */
    UBYTE   *yline_low_i;   /*  arr of loc Y line indices         */
  
}  yclass_def_type;
  
  
  
/*-----------------------------------*/
/*  Global Intellifont segment (100) */
/*    20-Mar-90  jfd                 */
/*      Added "glob_ital_tan" to end */
/*      of structure                 */
/*-----------------------------------*/
/* 46 BYTEs */
typedef struct
{
    UWORD   orThreshold;    /* Pixel size threshold above which ON */
    /* transitions may be OR-ed to bitmap. */
  
    /*      I5.0            Face Identifier                               */
  
    UWORD   if_flag;        /*  flag to indicate if intellifont   */
    /*  data is present                   */
  
    /*      I5.1            Global Y Class Definition Data                */
  
    UWORD   num_ylines;     /*  number of Y lines                 */
    UWORD   *ylines;        /*  single arr of Y class Y lines     */
    yclass_def_type  glob_yclass;
    /*  actual Y class definitions        */
  
    /*      I5.2            Global Dimension Data                         */
  
    dimension_type   glob_x_dim;
    dimension_type   glob_y_dim;
  
    /*      I5.3            Global Italic Angle Data                      */
  
    WORD    glob_ital_ang;
  
    /*      I5.4.1          Global Standard Dimension Data                */
  
    UWORD   stan_dim_lim;   /*  pix size above which standrd used */
  
    /*      I5.4.2          [Generic Screen] Face Substituion Data        */
  
    UWORD   subst_wdth_fac; /*  width fac. to adjust subst face   */
  
    UWORD   subst_cutin;    /*  width fac. to adjust subst face   */
  
    FIXED   glob_ital_tan;  /*  tangent of angle in radians       */
  
}  FACE;
  
  
  
  
/*--------------------------------*/
/* Display Header Segment (109)   */
/*--------------------------------*/
  
typedef struct
{
    UWORD     NCHAR;
    BYTE      binaryFileName[12];
    UWORD     fontLimits[4];
    UWORD     reverseVideoLimits[4];
    UWORD     leftReference;
    UWORD     baselinePosition;
    UWORD     minimumPointSize;
    UWORD     maximumPointSize;
    UWORD     minimumSetSize;
    UWORD     maximumSetSize;
    UBYTE     controlCode[4];
    UWORD     masterPointSize;
    UWORD     scanDirection;
    WORD      italicAngle;
    WORD      xHeight;
    UWORD     scanResolutionY;
    UWORD     scanResolutionX;
    UWORD     outputEnable;
} DISPLAY;
  
  
  
/*--------------------------------*/
/*  Face Attribute Segment (105)  */
/*--------------------------------*/
  
typedef struct
{
    UBYTE         languageType;
    UBYTE         fontUsage;
    UBYTE         isFixedPitch;
    UBYTE         escapeUnit;
    UWORD         scaleFactor;
    UWORD         fixedSpaceRelWidths[3];
    UWORD         leftReference;
    UWORD         baselinePosition;
    WORD          windowTop;
    WORD          windowBottom;
    struct {
                WORD   zeroPoint;
                WORD   variablePoint;
            } autoVarComp[3];

    WORD          ascender;
    WORD          descender;
    WORD          capHeight;
    WORD          xHeight;
    WORD          lcAccenHeight;
    WORD          ucAccentHeight;
    UWORD         charPica;
    WORD          leftAlign;
    WORD          rightAlign;
    WORD          uscoreDepth;
    UWORD         uscoreThickness;
    WORD          windowLeft;
    WORD          windowRight;
    UWORD         spaceBand;
}
FACE_ATT;
  
