/**[f******************************************************************
* deskjet.h -
*
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
  
/* DeskJet Family defines and type definitions. */
  
typedef struct
{
    BYTE left;
    BYTE width;
    BYTE right;
    WORD total;
} WIDTH_TABLE;
  
#define CLASS_LASERJET        0
#define CLASS_DESKJET         1
#define CLASS_DESKJET_PLUS    2
#define CLASS_LSRJETIII       4
#define CLASS_PAINTJET        5

#define LASERJET_FONT         0
#define DESKJET_FONT          5
#define DESKJET_PLUS_FONT     9
