/**[f******************************************************************
* $fais.h
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
  
/****************************************************************************
*
*   File: fais.h  v01.001
*
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
*   02-Feb-92 rk(HP) Chagned $library.h to _library.h                  
*   16-Aug-88 jnc Changed UWORD key to WORD key in all struct seg_type
*   12-Jul-88 jnc Extended CHARDATA key (5100) definition to
*                 CHARDATA1 (5100)
*                 CHARDATA2 (5200)
*                 CHARDATA3 (5300)
*   17-Jun-88 jnc Added FONTALIAS code definition
*   27-Apr-88 jnc Removed displayHeaderType structure definition to library.h
*   12-Apr-88 bjg Added compoundChar_type.
*   26-Feb-88 bjg Removed duplicate definition of structures and included
*                 library.h
*   08-Feb-88 mac Added ENDOFSEGMNT code definition
*   03-Feb-88 jnc Added typeface header segment definitions
*   28-Jan-88 jnc Refer to FAIS Revision B:
*                 1. Modified UnSectKernType & SectKernType structures
*                 2. Added 3 elements (windowLeft, windowRight & spaceBand)
*                    to attributeHeaderType structure
*   21-Nov-87 ddd Added Unpacked Section Kerning Type
*    1-Nov-87     Initial Release
*
*/
  
#include "_library.h"
  
/*
**  Define FAIS Segment Keys
*/
#define FONTHEADER         101
#define ATTRIBUTEHEADER    102
#define TYPEFACEHEADER     103
#define COMPLEMENTHEADER   104
#define FONTALIAS          107
#define CHARWIDTH          201
#define TEXTKERN           211
#define DESIGNKERN         212
#define TRACKKERN          213
#define COMPOUNDCHAR       231
#define CCHARMETRICS       233
#define CCID               234
#define DISPLAYHEADER      301
#define HIQDATA1           502
#define FONTDISPLAYCODE   2003
#define FONTATTRIBUTECODE 2102
#define GLOBALCHDATA      5001
#define CGCHARNUMS        5002
#define GLOBINTDATA       5003
#define CHARDATA1         5100
#define CHARDATA2         5200
#define CHARDATA3         5300
#define ENDOFSEGMENT         1
  
/*
**  Definition of FAIS segment used by file headers, HiQdata blocks &
**  attribute file headers
**    FAIS Key = HIQDATA1
*/
struct seg_type{
    UWORD       Id;
    ULONG       Length;
    UWORD       nkeys;
    struct {
        WORD     key;
        ULONG     offset;
    } seg[50];
} FAR *fileHeader,FAR *hiQseg,FAR *attrFileHeader;
  
/*
**  Definition of Complement Header
**    FAIS Key = COMPLEMENTHEADER
*/
struct {
    BYTE      typeFaceId[6];
    BYTE      complementId[6];
    BYTE      characterPlane[50];
    UWORD     substituteCode;
    struct {
        UWORD   isoCode;
        LONG    typeFace;
        UWORD   cgCode;
    } character[1000];
} FAR *compHeader;
  
/*
**  Definition of Global Character Data Block
**    FAIS Key = GLOBALCHDATA
*/
struct globChDataType{
    WORD     nChar;
    WORD     fontSlant;
    LONG     charOffset[100];
} *globChData;
  
/*
**  Define Display Header
*/
struct displayHeaderType  *display_header;
  
/*
**  Definition of Cg Character Numbers
**    FAIS Key = CGCHARNUMS
*/
UWORD   *cgCharId;
UWORD    NPI;
WORD     PITableCode;
struct piMapTableType{
    UBYTE   face[6];
} *piMapTable;
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
  
struct font__head{
    UWORD     code;
    ULONG    length_head;
    ULONG    length_body;
    ULONG    admin_off;
    ULONG    id_off;
    ULONG    metric_off;
    ULONG    intell_off;
    ULONG    glob_char_off;
    ULONG    glob_intel_off;
    ULONG    spec_off;
} *font_head;
  
UWORD *num_pi_char;
  
struct pi__table{
    UWORD comp_code;
    ULONG pi_face;
} *pi_table;
  
/*
**  Define Section (Text & Designer) Kerning FAIS Segments - LDR.I5 & LDR.I6
*/
struct SectKernType{
    WORD  kernSign;
    UWORD kernUnit;
    UWORD NSECT;
    struct{
        UBYTE data[4];
    } character[1000];
} *textKern,*designKern;
  
/*
**  Define Unpacked Section (Text & Designer) Kerning FAIS Segments
**  - LDR.I5 & LDR.I6
*/
struct UnSectKernType{
    WORD  kernSign;
    UWORD kernUnit;
    UWORD NSECT;
    struct{
        UWORD data[8];
    } character[1000];
} *UnTextKern,*UnDesignKern;
  
/*
**  Define Track Kerning FAIS Segments - LDR.I7
*/
struct trakKernType *trakKern;
  
/*
** Define Raster Parameter Segment
*/
struct RasterParamType{
    UWORD      seg_key;
    ULONG      seg_size;
    UWORD      Parameter;
    WORD       globItalAng;
} *rasterParam;
  
/*
** Define Character Width Segment
*/
struct charWidthType{
    UWORD      charWidth;
    UWORD      charFlags;
} *width;
/*
**  Define Font Header Segemnt
*/
struct fontHeaderType {
    BYTE    copyright[62];
    UWORD   formatVersion;
    BYTE    fontFileName[42];
    BYTE    fileDescriptor[16];
    BYTE    fontDescription[100];
    UWORD   fontVersion;
    BYTE    orderVersion[8];
    BYTE    timeStamp[6];
    UWORD   NCHAR;
} *attrFontHeader;
  
/*
**  Define compound character segment
*/
typedef struct compoundChar {
    UWORD   ccCharCode;
    UWORD   NPCC;
    struct {
        UWORD        ccCharCode;
        WORDVECTOR   offsets;
    } pCCarray[1];
} compoundChar_type;
  
/*
** Define Attribute Header
*/
struct attributeHeaderType  *attributeHeader;
  
/*
** Define typeface Header segment
*/
struct typefaceHeaderType far *typefaceHeader;
  
