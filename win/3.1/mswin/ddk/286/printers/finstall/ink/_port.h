/**[f******************************************************************
* $port.h
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
  
/***************************************************************************
*  port.h
*
*  Copyright (C) 1987, 1988, 1989, 1990. All Rights Reserved, by
*  Agfa Compugraphic Corporation, Wilmington, Ma.
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
*   07-Aug-88 mac Changed VOIDTYPE definition to YES for MSC
*   07-Jul-88 mac Changed VOID definition from typedef to #define because
*                 VOID does not apply to variables, only to functions.
*                 Defined MAX_LONG, MIN_LONG, and MAX_ULONG.
*   27-Jun-88 mac Fixed BOOLEAN typedef comment ( TRUE=1, FALSE=0 ).
*   07-Jun-88 mac Added configuration definitions and conditionals.
*             mac Added ABS and LABS macro definitions.
*   11-May-88 mac Change PATHNAMELEN to 63 for MS-DOS
*   16-Mar-88 mac Added typedef FIXED : 32 bit fixed point number
*   23-Feb-88 mac Added MIN_WORD
*   15-Feb-88 mac Clean up and reorganization of typedefs
*    1-Nov-87     Initial Release
*
*/
/************************************************************************/
/*                      Configuration Define                            */
/************************************************************************/
/*                Identifier Definition for the                         */
/*                Compiler - OS Configuration.                          */
/*                Define only one of the following:                     */
/* #define  MSWINDOWS                  Microsoft Windows                */
/* #define  MSC                        Microsoft C - MSDOS              */
/* #define  SUN                        SUN C - UNIX                     */
/* #define  VAX                        VAX C - VMS                      */
/*                                                                      */
/************************************************************************/
  
  
/************************************************************************/
/*                      Configuration Conditionals                      */
/************************************************************************/
  
#define  YES            1           /* Configuration feature enable     */
#define  NO             0           /* Configuration feature disable    */
#define  LOHI           1           /* Addr:LoByte, Addr+1:HiByte       */
#define  HILO           0           /* Addr:HiByte, Addr+1:LoByte       */
  
#ifdef   MSWINDOWS                  /* Microsoft C - Windows 3.0        */
#ifndef  MSDOS
#define  MSDOS                      /* MSDOS                            */
#endif
#define  INTLENGTH      16          /* int length of 16 bits            */
#define  BYTEORDER      LOHI        /* byte ordering is Lo-Hi bytes     */
#define  SIGNEDCHAR     YES         /* char is treated as signed byte   */
#define  VOIDTYPE       YES         /* compiler does have void type     */
#define  LINTARGS       YES         /* function decl with arguments     */
#define  ENTRYTYPE      YES         /* _export callback function def    */
#endif
  
#ifdef   MSC                        /* Microsoft C - MSDOS              */
#ifndef  MSDOS
#define  MSDOS                      /* MSDOS                            */
#endif
#define  INTLENGTH      16          /* int length of 16 bits            */
#define  BYTEORDER      LOHI        /* byte ordering is Lo-Hi bytes     */
#define  SIGNEDCHAR     YES         /* char is treated as signed byte   */
#define  VOIDTYPE       YES         /* compiler does have void type     */
#define  LINTARGS       YES         /* function decl with arguments     */
#define  ENTRYTYPE      NO          /* _export callback function def    */
#endif
  
#ifdef   SUN                        /* SUN C - UNIX                     */
#ifndef  UNIX
#define  UNIX                       /* UNIX                             */
#endif
#define  INTLENGTH      32          /* int length of 32 bits            */
#define  BYTEORDER      HILO        /* byte ordering is Lo-Hi bytes     */
#define  SIGNEDCHAR     YES         /* char is treated as signed byte   */
#define  VOIDTYPE       YES         /* compiler does have void type     */
#define  LINTARGS       NO          /* function decl without arguments  */
#define  ENTRYTYPE      NO          /* _export callback function def    */
#endif
  
#ifdef   VAX                        /* VAX C - VMS                      */
#ifndef  VMS
#define  VMS                        /* VMS                              */
#endif
#define  INTLENGTH      32          /* int length of 32 bits            */
#define  BYTEORDER      LOHI        /* byte ordering is Lo-Hi bytes     */
#define  SIGNEDCHAR     YES         /* char is treated as signed byte   */
#define  VOIDTYPE       YES         /* compiler does have void type     */
#define  LINTARGS       YES         /* function decl with arguments     */
#define  ENTRYTYPE      NO          /* _export callback function def    */
#endif
  
#if      LINTARGS == NO
#undef   LINTARGS
#endif
  
#ifdef  MSC
#ifdef  LINTARGS
#define LINT_ARGS                   /* MSC argument checking identifier */
#endif
#endif
  
/************************************************************************/
/*                      Typedef Definitions                             */
/************************************************************************/
  
  
#if ENTRYTYPE == YES
#define ENTRY _export far pascal    /* windows callback entry function  */
#else
#define ENTRY
#endif
  
  
#if SIGNEDCHAR == YES
#ifndef TMU //Done for TMU utilities
typedef  char           BYTE;       /* signed byte ( -127, 128 )        */
#endif
typedef  unsigned char  UBYTE;      /* unsigned byte ( 0, 255 )         */
#else
#ifndef TMU //Done for TMU utilities
typedef  signed char    BYTE;       /* signed byte ( -127, 128 )        */
#endif
typedef  unsigned char  UBYTE;      /* unsigned byte ( 0, 255 )         */
#endif
  
  
#if INTLENGTH == 16
typedef  int            WORD;       /* 16-bit signed word               */
typedef  long           LONG;       /* 32-bit signed longword           */
typedef  unsigned int   UWORD;      /* 16-bit unsigned word             */
typedef  unsigned long  ULONG;      /* 32-bit unsigned longword         */
#endif
#if INTLENGTH == 32
typedef  short          WORD;       /* 16-bit signed word               */
typedef  unsigned short UWORD;      /* 16-bit unsigned word             */
typedef  int            LONG;       /* 32-bit signed longword           */
typedef  unsigned int   ULONG;      /* 32-bit unsigned longword         */
#endif
  
#ifdef TMU
#ifndef MSWINDOWS
typedef  unsigned int   UWORD;      /* 16-bit unsigned word             */
typedef  unsigned long  ULONG;      /* 32-bit unsigned longword         */
#endif
typedef UWORD far * LPUWORD;
typedef ULONG far * LPULONG;
#endif
  
  
#ifndef TMU
#if VOIDTYPE == YES
#define  VOID           void        /* void data type                   */
#else
#define  VOID           int         /* void data type changed to int    */
#endif
#endif
  
typedef  int            BOOLEAN;    /* boolean ( TRUE = 1, FALSE = 0 )  */
typedef  int            COUNTER;    /* machine's word signed counter    */
typedef  unsigned int   UCOUNTER;   /* machine's word unsigned counter  */
typedef  double         DOUBLE;     /* floating point double precision  */
typedef  LONG           FIXED;      /* 32 bit fixed point number        */
/* with 16 bit fractional part      */
  
/* Data Types Definitions */
  
typedef struct word_point_struct {
    WORD x;                      /* point x coordinate                   */
    WORD y;                      /* point y coordinate                   */
} WORDPOINT;
  
typedef struct word_vector_struct {
    WORD x;                      /* vector x component                   */
    WORD y;                      /* vector y component                   */
} WORDVECTOR;
  
typedef struct word_window_struct {
    WORD Top;                    /* top edge coordinate                  */
    WORD Left;                   /* left edge coordinate                 */
    WORD Bottom;                 /* bottom edge coordinate               */
    WORD Right;                  /* right edge coordinate                */
} WORDWINDOW;
  
typedef struct
{
    FIXED x;
    FIXED y;
} POINTFX;
  
/************************************************************************/
/*                      Storage Class Definitions                       */
/************************************************************************/
  
#define  REG            register    /* register variable                */
#define  LOCAL          auto        /* local or automatic variable      */
#define  EXTERN         extern      /* external variable                */
#define  MLOCAL         static      /* local to module                  */
#define  GLOBAL         /**/        /* global variable                  */
  
/************************************************************************/
/*                      Path and File name lengths                      */
/************************************************************************/
  
#ifdef  MSDOS
#define FILENAMELEN  13
#define PATHNAMELEN  63
typedef BYTE FILECHAR;
typedef FILECHAR FILENAME [FILENAMELEN];
typedef FILECHAR PATHNAME [PATHNAMELEN];
typedef FILECHAR DIRNAME [PATHNAMELEN];
#endif
  
#ifdef  SUN
#define FILENAMELEN  13
#define PATHNAMELEN  63
typedef BYTE FILECHAR;
typedef FILECHAR FILENAME [FILENAMELEN];
typedef FILECHAR PATHNAME [PATHNAMELEN];
typedef FILECHAR DIRNAME [PATHNAMELEN];
#endif
  
#ifdef  VAX
#define FILENAMELEN  13
#define PATHNAMELEN  63
typedef BYTE FILECHAR;
typedef FILECHAR FILENAME [FILENAMELEN];
typedef FILECHAR PATHNAME [PATHNAMELEN];
typedef FILECHAR DIRNAME [PATHNAMELEN];
#endif
  
/************************************************************************/
/*                      Macro Definitions                               */
/************************************************************************/
  
#define ABS(a)    ( ((a) < (WORD)0) ? -(a) : (a) )
#define LABS(a)   ( ((a) < (LONG)0) ? -(a) : (a) )
#define MIN(a,b)  ( ((a) < (b)) ? (a) : (b) )
#define MAX(a,b)  ( ((a) < (b)) ? (b) : (a) )
  
/************************************************************************/
/*                      Miscellaneous Definitions                       */
/************************************************************************/
  
#define  FAILURE        (-1)        /* Function failure return val      */
#define  SUCCESS        (0)         /* Function success return val      */
  
#ifndef TMU
#define  TRUE           (1)         /* Function TRUE  value             */
#define  FALSE          (0)         /* Function FALSE value             */
#endif
  
#define  MAX_WORD       ((WORD)(0x7FFF))
#define  MIN_WORD       ((WORD)(0x8000))
#define  MAX_UWORD      ((UWORD)(0xFFFF))
  
#define  MAX_LONG       ((LONG)(0x7FFFFFFF))
#define  MIN_LONG       ((LONG)(0x80000000))
#define  MAX_ULONG      ((ULONG)(0xFFFFFFFF))
  
  
