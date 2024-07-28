/**[f******************************************************************
* $tmu.h
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
  
/*  tmu.h */
/*  (C) Copyright 1990 Agfa Compugraphic Corporation. All rights reserved.
*/
  
/*---------------------------------------------*/
/*    E r r o r    R e t u r n    C o d e s    */
  
/* fais.c */
  
#define ERRfais_malloc       100
#define ERRnot_fais          101
#define ERRfais_read         102
  
/* face.c */
  
#define ERRface_malloc       200
#define ERRno_list_file      201
#define ERRnew_file          202
#define ERRface_write        203
#define ERRface_fstat        204
#define ERRface_read         205
#define ERRface_no_lib       206
#define ERRbad_update_type   207
#define ERRface_bad_info     208
#define ERRface_entry_exists 209
  
/* ixdata.c */
  
#define ERRix_malloc         300
#define ERRix_no_lib_file    301
#define ERRix_not_disk_lib   302
#define ERRix_read           303
#define ERRix_seek           304
#define ERRix_tf_not_in_lib  307
#define ERRix_no_font_alias  308
#define ERRix_too_many_faces 309
  
/* lib.c */
  
/*---------------------------------------------*/
  
#define MAXNAME 80
typedef BYTE TFNAME[51];
typedef TFNAME FAR * LPTFNAME;
  
typedef struct
{
    LONG id;                /* typeface id */
    LONG complement;            /* character complement */
    LONG space_req;         /* space required for the library */
    BYTE typefaceName[51];              /* null terminated string */
    BYTE nameOrDir[MAXNAME];
    BYTE pad;     /* To make the if.dsc agree with the dos built version */
} TYPEINFO, FAR * LPTYPEINFO;
  
//typedef TYPEINFO FAR * LPTYPEINFO;
  
/* tmu function definitions */
  
WORD FAR PASCAL FACEadd(LPTYPEINFO, LPSTR);
WORD FAR PASCAL FACEdelete(LPTYPEINFO, LPSTR, BOOL);
WORD FAR PASCAL FACElist(LPUWORD, LPTYPEINFO, LPSTR);
WORD FAR PASCAL FACEinstall(LPTYPEINFO, LPSTR, LPSTR, WORD, WORD, LPSTR, LPSTR);
WORD FAR PASCAL FAISlist(LPSTR, LPUWORD, LPTYPEINFO);
WORD FAR PASCAL FAISload(LPTYPEINFO, LPSTR, LPSTR);
WORD FAR PASCAL LIBlist(LPSTR, LPUWORD, LPTYPEINFO, WORD);
  
#define UPDATE_IF_FNT  0
#define UPDATE_HQ3_FNT 1
