/**[f******************************************************************
* tfmdefs.h -
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
  
/*****************************************************************************
**  This file contains tag structures containing information that will be  **
**  writen to the TFM file.                                                **
****************************************************************************/
  
  
/**************************************************************************/
/*                      tag segment keys                                  */
/**************************************************************************/
  
#define  tagSUBFILE          400
#define  tagCOPYRIGHT        401
#define  tagCOMMENT          402
#define  tagSYMBOLMAP        403
#define  tagSYMBOLSETDIR     404
#define  tagUNIQUEASSOCID    405
#define  tagPOINT            406
#define  tagNOMINALPOINT     407
#define  tagDESIGNUNITS      408
#define  tagTYPESTRUCT       410
#define  tagSTROKEWT         411
#define  tagSPACING          412
#define  tagSLANT            413
#define  tagAPPEARWIDTH      414
#define  tagSERIFSTYLE       415
#define  tagTYPESTYLE        416
#define  tagTYPEFACE         417
#define  tagTFSOURCE         418
#define  tagAVERAGEWD        419
#define  tagMAXWIDTH         420
#define  tagINTERWORDSP      421
#define  tagRECLINESP        422
#define  tagCAPHEIGHT        423
#define  tagXHEIGHT          424
#define  tagASCENT           425
#define  tagDESCENT          426
#define  tagLOWERASCENT      427
#define  tagLOWERDESCENT     428
#define  tagUNDERDEPTH       429
#define  tagUNDERTHICK       430
#define  tagUPPERACCENT      431
#define  tagLOWERACCENT      432
#define  tagHORIZONTALESC    433
#define  tagVERTICALESC      434
#define  tagLEFTEXTENT       435
#define  tagRIGHTEXTENT      436
#define  tagCHARASCENT       437
#define  tagCHARDESCENT      438
#define  tagKERNPAIRS        439
#define  tagSECTORKERN       440
#define  tagTRACKKERN        441
#define  tagTFSELECTSTR      442
  
/**************************************************************************/
/*                    Data type definitions                               */
/**************************************************************************/
  
#define  tagBYTE           1
#define  tagASCII          2
#define  tagSHORT          3
#define  tagLONG           4
#define  tagRATIONAL       5
#define  tagSIGNEDBYTE    16
#define  tagSIGNEDSHORT   17
#define  tagSIGNEDLONG    18
  
  
struct tagType
{
    WORD            tag_num;
    WORD            data_type;
    DWORD           data_size;
    BYTE            data_offset[4];
    BYTE            *data;
} ;
  
/* Size of "PCL5 / HP LaserJet III" string */
#define  DRVNAMESIZE    23
  
/* Symbol set values */
#define D1  364
#define D2  396
#define D3  428
#define DS  332
#define DT  234
#define DV  300
#define E1  14
#define LG  53
#define M8  269
#define MS  173
#define PB  202
#define PC  341
#define PD  373
#define PI  501
#define PM  405
#define R8  277
#define TS  330
#define US  21
#define VI  426
#define VM  205
#define VU  458
#define WN  309
  
#define FRSTCHAR     0        /* first char for WN set   */
