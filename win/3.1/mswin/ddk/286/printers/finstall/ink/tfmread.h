/**[f******************************************************************
* tfmread.h -
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
  
/************************************************************************/
/*                      Configuration Define                            */
/************************************************************************/
/*                Identifier Definition for the                         */
/*                Compiler - OS Configuration.                          */
/*                Define only one of the following:                     */
/* #define  MSC                        Microsoft C - MSDOS              */
/* #define  SUN                        SUN C - UNIX                     */
/* #define  VAX                        VAX C - VMS                      */
/*                                                                      */
/************************************************************************/
  
#define  MSC                        /* Microsoft C - MSDOS              */
  
  
/************************************************************************/
/*                      Typedef Definitions                             */
/************************************************************************/
  
//typedef  unsigned char  BYTE;       /* unsigned byte ( 0, 255 )         */
//typedef  int            WORD;       /* 16-bit signed word               */
//typedef  unsigned int   UWORD;      /* 16-bit unsigned word             */
//typedef  long           LONG;       /* 32-bit signed longword           */
//typedef  unsigned long  ULONG;      /* 32-bit unsigned longword         */
//typedef  int            BOOLEAN;    /* boolean ( TRUE = 1, FALSE = 0 )  */
//typedef  int            COUNTER;    /* machine's word signed counter    */
//typedef  unsigned int   UCOUNTER;   /* machine's word unsigned counter  */
//typedef  double         DOUBLE;     /* floating point double precision  */
//typedef  double         RATIONAL;   /* 64-bit (8-bytes) floating point  */
//typedef  LONG           FIXED;      /* 32 bit fixed point number        */
/* with 16 bit fraccional part      */
  
/************************************************************************/
/*                      Miscellaneous Definitions                       */
/************************************************************************/
//#define  TRUE           (1)         /* Function TRUE  value             */
//#define  FALSE          (0)         /* Function FALSE value             */
  
#define  MAX_FILEPATH   78
  
#define  MIN_NO_TAG      400
#define  MAX_NO_TAG      442
  
#define  DASH            "-"
#define  ZERO            "0"
#define  POINTSIZE       "00010072\0"
#define  NOMINAL_POINT   "02500001\0"
  
/* constants used in the getit routines */
#define TWO_8            256
#define TWO_16           65536
#define TWO_24           16777216
  
#define MAXBUFSIZE 32768L
  
