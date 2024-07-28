/**[f******************************************************************
* $resourc.h
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
  
/********************************************************************\
* Global Structures
\********************************************************************/
  
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
  
  
/********************************************************************\
* Global Functions
\********************************************************************/
  
LPSTR FAR PASCAL lstrncpy(LPSTR, LPSTR, int);
LPSTR FAR PASCAL lstrncat(LPSTR, LPSTR, int);
  
  
#define SFINSTALL   1
#define SFADDFONT   3
#define SMARTSFADD  4
#define SFCOPYFONT  5
#define SFEDIT      6
#define SFUNKNOWN   7
#define SFTARGDIR   8
#define SFDLQUERY   9
#define SFDLHELP    10
#define SFPERMALERT 11
#define SFPERMHELP  12
#define SFSCRNALERT 13
#define SFDUALERT   14
#define SFMAXFALERT 15
#define NOSFDIRFILE 16
#define NOSFDLFILE  17
#define DUPSFID     18
#define SFDLDIRFNM  19
#define SFABOUT     20
