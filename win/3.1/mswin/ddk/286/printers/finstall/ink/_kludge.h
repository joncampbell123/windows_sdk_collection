/**[f******************************************************************
* $kludge.h
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
  
/*********************************************************************
  
this include file replaced resource.h for certain files.
These defines should be put somewhere else if time permits.
  
**********************************************************************/
  
#define MAX_FILENAME_SIZE 80
  
#define CLASS_LASERJET      0
#define CLASS_DESKJET       1
#define CLASS_DESKJET_PLUS  2
#define CLASS_DESKJET_500   3
  
#define LIB_ENTRY_FAIS   0x0001
#define LIB_ENTRY_TYPE   0x0002
#define LIB_ENTRY_PCLIO  0x0004
#define LIB_ENTRY_SCREEN 0x0008
#define LIB_ENTRY_SIZE   0x0010
  
#define LIB_ENTRY_PRINTER LIB_ENTRY_FAIS | LIB_ENTRY_TYPE | LIB_ENTRY_PCLIO
  
