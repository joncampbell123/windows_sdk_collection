/**[f******************************************************************
* $loader.h
*
* Copyright (C) 1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*
**f]*****************************************************************/
  
/*
 * $Header: 
 */

/*
 * $Log:
 */
  
/**************************************************************************
*  File:  loader.h
*/
/*
*  Copyright (C) 1987, All Rights Reserved, by
*  Compugraphic Corporation, Wilmington, Ma.
*
*  This software is furnished under a license and may be used and copied
*  only in accordance with the terms of such license and with the
*  inclusion of the above copyright notice. This software or any other
*  copies thereof may not be provided or otherwise made available to any
*  other person. No title to and ownership of the software is hereby
*  transferred.
*
*  The information in this software is subject to change without notice
*  and should not be construed as a commitment by Compugraphic
*  Corporation.
*
*
*  History:
*  ---------
*  07-Oct-89 Created.
*/
  
//  version information
static char words[] =
"Intellifont Loader Version %s Release %s\n\
Copyright (c) 1988 Compugraphic Corporation ALL RIGHTS RESERVED\n";
  
#define VERSION "2.1"
#define LASTMODIFIED "07-Oct-89"
  
  
#define  MIN_CHAR_SIZE                   12
#define  MAX_CHAR_SIZE                13312
#define  MAX_FONT_FILES                  32
#ifdef NODEF
#define  MAX_BUFFER                   30000  //  Must be greater or equal
#else
#define  MAX_BUFFER                   13312  //  Must be greater or equal
#endif                                             //  to MAX_CHAR_SIZE
#define  MAX_CHARS                     1000
#define  MAX_KEYS                        84  // max number of keys for
// file headers and hiqdata1
#define  MAX_HEADER        6 * MAX_KEYS + 8
  
#define  MAX_TEXKERN     16 * MAX_CHARS + 6
#define  MAX_DESKERN     16 * MAX_CHARS + 6
  
  
  
//  global defines
#define  HIQDATA1                       502
#define  GLOBALCHDATA                  5001
#define  CGCHARNUMS                    5002
#define  GLOBINTDATA                   5003
#define  DISPLAYHEADER                  301
#define  CHARWIDTH                      201
#define  ATTRIBUTEHEADER                102
#define  CCHARMETRICS                   233
#define  COMPLEMENTHEADER               104
#define  COMPOUNDCHAR                   231
#define  CCID                           234
#define  DESIGNKERN                     212
#define  FONTALIAS                      107
#define  FONTHEADER                     101
#define  TEXTKERN                       211
#define  TRACKKERN                      213
#define  TYPEFACEHEADER                 103
#define  DSK_LIB_DIR_SIZE               512
#define  RES_LIB_DIR_SIZE                32
#define  FIL_SEG_DIR_SIZE                32
#define  DISK_ALIGN                      16
#define  RAM_ALIGN                       16
#define  COPYRIGHTSIZE                   62
#define  RASTERPARAMSIZE                 10
#define  OK                               0
#define  fileheader_key                   1
#define  filedirectory_key                2
#define  faceglobal_key                  10
#define  globalintellifont_key          100
#define  trackkern_key                  101
#define  textkern_key                   102
#define  designkern_key                 103
#define  charwidth_key                  104
#define  attributeheader_key            105
#define  rasterparam_key                106
#define  typefaceheader_key             107
#define  compoundchar_key               108
#define  displayheader_key              109
#define  fontalias_key                  110
#define  copyright_key                  111
#define  cchartextkern_key              112
#define  cchardesignkern_key            113
#define  endofdir_key                    -1
#define  error_return                    -1
  
//  structure definitions
typedef struct {
    UWORD key;
    ULONG offset;
} keyoffset_type;
  
typedef struct {
    UWORD fileId;
    ULONG fileLength;
    UWORD NKEYS;
    keyoffset_type keyOffsets[1];
} fileheader_type;
  
typedef struct {
    UWORD blockID;
    ULONG blockLength;
    UWORD NBKEYS;
    keyoffset_type keyOffsets[1];
} hiqdata1_type;
  
typedef struct {
    UWORD NCHAR;
    WORD  fontSlant;
    ULONG offsets[1];
} globalchdata_type;
  
typedef UWORD cgcharnums_type;
  
typedef struct {
    UWORD cg_num;
    ULONG offset;
    UWORD size;
}  char_type;
  
typedef struct {
    ULONG faceglobal_offset;
    UWORD faceglobal_size;
    char_type chars[1];
} faceheader_type;
  
typedef struct {
    UWORD key;
    ULONG offset;
    UWORD size;
} seg_dir_type;
  
typedef struct {
    ULONG face;
    ULONG offset;
    UWORD size;
} file_dir_type;
  
typedef struct {
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
} displayheader_type;
  
typedef struct {
    UWORD      charWidth;
    UWORD      charFlags;
} charwidth_type;
  
typedef struct {
    WORD  kernSign;
    UWORD kernUnit;
    UWORD NSECT;
    struct{
        UBYTE data[4];
    } character[1];
} packedkern_type;
  
typedef  struct{
    UWORD data[8];
} unpkerndata_type;
  
typedef struct {
    WORD  kernSign;
    UWORD kernUnit;
    UWORD NSECT;
    unpkerndata_type character[1];
} unpackedkern_type;
  
typedef struct {
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
} attributeheader_type;
  
typedef struct {
    UWORD         NFACES;
    BYTE          typeFaceName[50];
    BYTE          familyName[20];
    BYTE          weight[20];
    LONG          typeFaceSet[12];
} typefaceheader_type;
  
typedef struct {
    UWORD         key;
    ULONG         size;
    seg_dir_type  seg_dir;
} faceglobal_type;
  
typedef struct {
    UWORD cg_num;
    WORD  xoffset;
    WORD  yoffset;
} libpart_type;
  
typedef struct {
    UWORD cg_num;
    WORD  horiz_esc;
    WORD  vert_esc;
    WORD  NPCC;
    libpart_type parts[1];
} libcompoundchar_type;
  
typedef struct {
    UWORD        ccCharCode;
    WORDVECTOR   offsets;
} part_type;
  
typedef struct {
    UWORD   ccCharCode;
    UWORD   NPCC;
    part_type parts[1];
} compoundchar_type;
  
typedef struct {
    UWORD   isoCode;
    LONG    typeFace;
    UWORD   cgCode;
} isocodetable_type;
  
  
typedef struct {
    BYTE      typeFaceId[6];
    BYTE      complementId[6];
    BYTE      characterPlane[50];
    UWORD     substituteCode;
    isocodetable_type table[1];
} complementheader_type;
  
typedef struct {
    UWORD cg_num;
    LONG  face;
} ccid_type;
  
typedef struct {
    WORD   escapementX;
    WORD   escapementY;
    BYTE   amplified;
    BYTE   correction;
    WORD   leftExtent;
    WORD   rightExtent;
    WORD   ascent;
    WORD   descent;
} ccharmetrics_type;
  
  
  
  
// global data
int errfile;
LONG status;     /* rcm: changed  WORD->LONG*/
UWORD type;
UWORD disk;
UWORD alignment;
ULONG typeface;
ULONG complement;
UWORD orThreshold;
PSTR fais_drive[4]={"/o=","/d=","/m=","/r="};
char fais_path[65];
char fileName[80];
BYTE libName[80];
int FONTDISPLAY;
int FONTATTRIBUTE;
int LIB;
UWORD nchar_in_lib; // number of valid characters to be put in lib
UWORD nchar_in_font; // number of chars in font including missing and pi etc.
LONG faceheaderoffset;
LONG globintdataoffset = -1l;
ULONG hiqdataoffset;
LONG globintdatasize;
UWORD faceheadersize;
WORD fontslant;
  
//  global buffers
BYTE buffer[MAX_BUFFER];
BYTE faceheader[8 * MAX_CHARS + 14];
BYTE far * designkern;    /* will be global alloc'd later */
WORD hdkrn;               /* handle to buffer */
BYTE far * textkern;      /* will be global alloc'd later */
WORD htkrn;               /* handle to buffer */
BYTE dfileheader[MAX_HEADER];
BYTE afileheader[MAX_HEADER];
BYTE hiqdata[MAX_HEADER];
/*
**  Definition of Font Index File
*/
struct {
    UWORD      volId;
    UWORD      nVols;
    UWORD      volNo;
    LONG       volLength;
    UWORD      nFiles;
    struct {
        BYTE     face[6];
        BYTE     comp[6];
        BYTE     style[2];
        BYTE     pointSize[4];
        BYTE     setSize[4];
        BYTE     res[4];
        BYTE     orient[4];
        BYTE     fileId[5];
        BYTE     fileType[2];
        BYTE     fill[5];
        UWORD    dupCode;
        UWORD    nVol;
        UWORD    iVol;
        BYTE     fileName[8];
    } font[MAX_FONT_FILES];
} index_file;
  
  
