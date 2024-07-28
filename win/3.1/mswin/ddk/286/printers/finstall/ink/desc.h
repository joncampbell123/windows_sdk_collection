/**[f******************************************************************
* desc.h -
*
* Copyright (C) 1988,1989 Aldus Corporation.  All rights reserved.
* Copyright (C) 1989-1990 Microsoft Corporation.
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
  
typedef struct {
    WORD magic;
    BYTE zero1;
    BYTE font_type;
    WORD zero2;
    WORD baseline;
    WORD cell_width;
    WORD cell_height;
    BYTE orientation;
    BYTE proportional;
    WORD symbol_set;
    WORD pitch;
    WORD height;
    WORD zero3;
    BYTE zero4;
    BYTE style;
    BYTE stroke_weight;
    BYTE typeface;
    BYTE zero5[22];
    char facename[16];
    BYTE heightRadix;
    BYTE zero6[2];
    char copyright[60];     /* may be longer than 60 char */
} LASERJETFONTDES;
  
typedef char SINT;
  
typedef struct {
    WORD font_desc_size;
    BYTE header_format;
    BYTE font_type;
    WORD reserved_word_4;
    WORD baseline;
    WORD cell_width;
    WORD cell_height;
    BYTE orientation;
    BYTE proportional;
    WORD symbol_set;
    WORD pitch;
    WORD height;
    WORD xHeight;
    BYTE width_type;
    BYTE style;
    SINT stroke_weight;
    BYTE typeface;
    BYTE zero5[4];
    SINT underline_pos;
    BYTE underline_height;
    WORD line_spacing;
    BYTE zero6[14];
    char facename[16];
    WORD horiz_res;
    WORD vertical_res;
    SINT top_underline_pos;
    BYTE top_underline_height;
    SINT bottom_underline_pos;
    BYTE bottom_underline_height;
    WORD block_size;
    WORD font_data_size;
    BYTE unidirection_flag;
    BYTE compressed_flag;
    BYTE hold_time_factor;
    BYTE no_half_pitch;
    BYTE no_double_pitch;
    BYTE no_half_height;
    BYTE no_bold;
    BYTE no_draft;
    BYTE bold_method;
    BYTE reserved_byte_85;
    WORD pass_2_baseline;
    WORD pass_3_baseline;
    WORD pass_4_baseline;
    char copyright[60];         /* may be longer than 60 char */
} DESKJETFONTDES;
  
typedef union {
    DESKJETFONTDES      dj;
    LASERJETFONTDES lj;
} FONTDES;
  
typedef struct {
    WORD magic;
    BYTE zero1;
    BYTE font_type;
    WORD zero2;
    WORD baseline;
    WORD cell_width;
    WORD cell_height;
    BYTE orientation;
    BYTE proportional;
    WORD symbol_set;
    WORD pitch;
    WORD height;
    WORD zero3;
    BYTE zero4;
    BYTE style;
    BYTE stroke_weight;
    BYTE typeface;
} GENERIC_FONTDES;
  
/*  The character descriptor structure is different for the DeskJet and
Laserjet:
  
DeskJet                     LaserJet
  
format (UB)                 format (UB)
continuation (B)            continuation (B)
descriptor size (UB)        descriptor size (UB)
character type (UB)         class (UB)
normal width (UB)           orientation (UB)
compressed width (UB)   left offset (SI)
left ps pad (UB)            top offset (SI)
right ps pad (UB)           character width (UI)
character height (UI)
delta x (SI)
*/
  
typedef struct {
    BYTE format;
    BYTE continuation;
    BYTE descriptor_size;
    BYTE char_type;
    BYTE norm_width;
    BYTE comp_width;
    BYTE left_ps_pad;
    BYTE right_ps_pad;
} DESKJETCHARDES;
  
typedef struct {
    BYTE format;
    BYTE continuation;
    BYTE descriptor_size;
    BYTE class;
    BYTE orientation;
    BYTE padding;
    WORD left_offset;
    WORD top_offset;
    WORD char_width;
    WORD char_height;
    WORD delta_x;
} LASERJETCHARDES;
  
typedef union {
    DESKJETCHARDES  dj;
    LASERJETCHARDES lj;
} CHARDES;
  
typedef struct {
    HANDLE hTrans;
    LPSTR lpTrans;
    HANDLE hMd;
    SYMBOLSET symbolSet;
    long count;
    int prevWidth;
    long totalWidth;
    int maxWidth;
    BYTE pitch;
    BYTE firstchar;
    BYTE lastchar;
} CHDATA;
  
  
typedef struct {
    WORD dfVersion;
    DWORD dfSize;
} SZPFMHEAD;
  
typedef FONTDES FAR *LPFONTDES;
typedef GENERIC_FONTDES FAR *LPGENERIC_FONTDES;
typedef CHARDES FAR *LPCHARDES;
typedef CHDATA FAR *LPCHDATA;
typedef SZPFMHEAD FAR *LPSZPFMHEAD;
typedef DESKJETFONTDES FAR * LPDJFONTDES;
typedef LASERJETFONTDES FAR * LPLJFONTDES;
  
  
