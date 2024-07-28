/**[f******************************************************************
* faislist.h -
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

#define FAIS_NO_FAIS     1
#define FAIS_READ_ERROR  2
#define ALLOC_ERROR      0
  
#define MAXLIST          20
  
/* typeface list structure */
  
  
typedef struct _LIST {   /* li */
    int  iSequence;
    long lFaceID;
    long lComp;
    char szFaceName[80];
    long lSpaceReq;
} LIST;
typedef LIST * PLIST;
typedef LIST far * FPLIST;
  
  
  
  
  
