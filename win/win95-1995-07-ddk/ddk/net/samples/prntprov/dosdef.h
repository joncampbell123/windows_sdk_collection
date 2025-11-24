/***************************************************************************\
*
* Module Name: DOSDEF.H
*
* Common Type Definitions file
*
* Copyright (c) 1990 - 1995, Microsoft Corporation.  All rights reserved.
*
\***************************************************************************/

#if !defined( DOSDEF_INCLUDED )

#define DOSDEF_INCLUDED


/* structure for Device Driver data */

typedef struct _DRIVDATA {
        long    cb;
        long    lVersion;
        char    szDeviceName[32];
        char    abGeneralData[1];
} DRIVDATA;
typedef DRIVDATA far   *PDRIVDATA;


#endif /* DOSDEF_INCLUDED */

