/**[f******************************************************************
* $library.h
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
  
/*
*  library.h
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
*   13-Aug-88 mac Fixed data types for members of struct attributeHeader
*                 and members of struct displayHeader
*   12-Aug-88 mac Added typedef struct lib_list
*   08-Aug-88 bjg Added cchar_kerning keys.
*   07-Aug-88 mac Added tag identifier to struct library
*   03-Jul-88 bjg Added font_copyright_key definition
*   20-Jun-88 bjg Corrected typefaceHeaderType, added descriptorSetType.
*                 Changed typeFaceSet[] to LONG for Library compatibility
*   17-Jun-88 jnc Added font_alias_key definition
*   27-Apr-88 jnc Moved definition of displayHeaderType structure from
*                 fais.h to here and added 1 more element (outputEnable) in
*                 structure (refer to FAIS revision B)
*   27-Apr-88 jnc Defined display_header_key as 109
*   29-Feb-88 bjg changed SectKernType to LibSectKernType to avoid confusion
*                 with fais.h
*   26-Feb-88 bjg added comments about intelfnt.h
*   26-Feb-88 bjg changed char_width_key to global_charWidth_key
*                 to be consistant with loader.c
*   08-Feb-88 jnc Added typeface header segment type definition and key #(107)
*   06-Feb-88 tbh changed .face to LONG
*   28-Jan-88 jnc modified structures setKernType and attributeHeaderType
(based on FASI Revision B)
*    1-Nov-87    Initial Release
*
*/
/*<><><><><><><><>< Define Font Library ><><><><><><><><><><><><><>*/
  
#define file_directory_key          2
#define face_global_key            10
#define global_intellifont_key    100
#define track_kerning_key         101
#define text_kerning_key          102
#define design_kerning_key        103
#define global_charWidth_key      104
#define attribute_header_key      105
#define raster_param_key          106
#define typeface_header_key       107
#define compound_char_key         108
#define display_header_key        109
#define font_alias_key            110
#define font_copyright_key        111
#define cchar_text_kerning_key    112
#define cchar_design_kerning_key  113
  
/*
*   Segment Directory - Resident
*/
typedef struct {
    UBYTE      type;
    UBYTE      fill;
    ULONG      size;
    UWORD      checksum;
    struct {
        UWORD     key;
        ULONG     offset;
        UWORD     size;
    } dir[1000];
} res_seg_dir_type;
/*
*   Segment Directory - Disk
*/
typedef struct {
    UWORD     key;
    ULONG     offset;
    UWORD     size;
} dir_type;
typedef struct {
    UBYTE      type;
    UBYTE      fill;
    dir_type dir[1000];
} dsk_seg_dir_type;
/*
*  Segment Directory - Union
*/
typedef union {
    dsk_seg_dir_type  FAR *dsk;
    res_seg_dir_type  FAR *res;
} seg_dir_type;
/*
*  Library File Directory
*/
typedef struct {
    LONG        face;
    ULONG       offset;
    UWORD       size;
} file_dir_type;
  
/*
**  Define lib_list structure
*/
/************************************************************************
*   This definition of lib_list_type is duplicated in intelfnt.h,       *
*   any changes made here should also be made in intelfnt.h             *
************************************************************************/
typedef struct lib_list {
    UWORD nface;
    struct {
        ULONG facenum;
        UWORD nchar;
        UWORD max_size;
        UWORD min_size;
    } face[1];
} lib_list_type;
  
/*<><><><><><><>< LIBRARY TABLE DEFINITION ><><><><><><><><><><>*/
/*
*  Defines an entry of the Library Table
*/
typedef struct library {
    UWORD           type;
    file_dir_type   FAR *dir;
    UBYTE           count;
    int            file;
    UBYTE          FAR *file_name;
    UBYTE          FAR *mem_address;
} library_type;
  
  
/*
**  Define Section (Text & Designer) Kerning FAIS Segments - LDR.I5 & LDR.I6
*/
struct LibSectKernType{
    UWORD segType;
    ULONG segSize;
    WORD  kernSign;
    UWORD kernUnit;
    UWORD NSECT;
    struct{
        UBYTE data[4];
    } character[1000];
};
  
/*
**  Define Track Kerning FAIS Segments - LDR.I7
*/
/************************************************************************
*   This definition of trakKernType is duplicated in intelfnt.h,        *
*   any changes made here should also be made in intelfnt.h             *
************************************************************************/
struct trakKernType {
    UWORD  NTRACK;
    struct trakArrayType {
        WORD    kernDegree;
        UWORD   minKernPtSize;
        UWORD   maxKernPtSize;
        WORD    minKernAmt;
        WORD    maxKernAmt;
    } track[4];
};
  
/*
** Define Attribute Header
*/
/************************************************************************
*   This definition of attributeHeaderType is duplicated in intelfnt.h, *
*   any changes made here should also be made in intelfnt.h             *
************************************************************************/
struct attributeHeaderType {
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
};
  
/*
** Define Character Width Segment
*/
struct Width_type{
    UWORD      charWidth;
    UWORD      charFlags;
};
  
/*
** Define Typeface Header Segment
*/
/************************************************************************
*   This definition of typefaceHeaderType  is duplicated in intelfnt.h, *
*   any changes made here should also be made in intelfnt.h             *
************************************************************************/
struct typefaceHeaderType {
    UWORD         NFACES;
    BYTE          typeFaceName[50];
    BYTE          familyName[20];
    BYTE          weight[20];
    LONG          typeFaceSet[12];
};
  
struct descriptorSetType {
    UBYTE      stemStyle;
    UBYTE      stemMod;
    UBYTE      stemWeight;
    UBYTE      slantStyle;
    UBYTE      horizStyle;
    UBYTE      vertXHeight;
    UBYTE      videoStyle;
    UBYTE      copyUsage;
};
  
/*
**  Define Display Header
*/
struct displayHeaderType {
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
};
  
