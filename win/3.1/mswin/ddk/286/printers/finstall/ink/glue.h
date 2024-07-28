/**[f******************************************************************
* glue.h -
*
* Copyright (C) 1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*
**f]*****************************************************************/
  
/****************************************************************************
**  This file contains structure information that will be                  **
**  writen to the PFM file.                                                **
****************************************************************************/
  
/*
 * $Header: 
 */

/*
 * $Log:
 */

typedef struct
  
{
    BYTE     ssPCLcode[4];
    /* int      ssClass;        Don't use class for now */
} SYMINFO;
  
typedef SYMINFO FAR *LPSYMINFO;
  
  
  
typedef struct
  
{
    int      giType;
    DWORD    giMemusage;     /* font's printer memory usage */
    BYTE     giName[50];
    BYTE     giSymSet[4];
    BYTE     giWeight[11];
    BYTE     giSlant[9];
    BYTE     giFFile[MAX_FILEPATH];  /* PCLEO path and file name */
    BYTE     giTfm[MAX_FILEPATH];    /* TFM path and file name */
    // int      giSFind;            /* index into finstall structure */
} GLUEINFO;
  
typedef GLUEINFO FAR *LPGLUEINFO;
  
  
  
typedef struct
  
{
    /* BYTE        gcFName[50]; */
    BYTE        gcClass;
    BYTE        gcOrient;
    HANDLE   gchSym;
    /* SYMINFO far *gcSymSet;   Make this ptr a local var... */
    int         gcSSIndex;
    int      gcNumSizes;
    BYTE        gcPtSizes[50];
    BYTE        gcTfm[MAX_FILEPATH];
} GCINFO;
  
typedef GCINFO FAR *LPGCINFO;
  
  
  
//typedef struct
//
//   {
//   BYTE far fiFName[50];
//   BYTE far fiFFile[MAX_FILEPATH];
//   BYTE far fiSymSet[4];
//   int      fiSSClass;
//   BYTE far fiTfm[MAX_FILEPATH];
//
//   } GFINFO;
//
//typedef GFINFO FAR *LPGFINFO;
//
//
//
//typedef struct
//
//   {
//   BYTE far diName[85];
//   int      diIndex;
//
//   } DISPLAY;
//
//typedef DISPLAY FAR *LPDISPLAY;
  
  
  
#define EOS     '\0'
