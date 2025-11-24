/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

/* CALERR.h: Calibrate DLL error handling header

	General header for pencal.dll
*/

#ifndef _INCLUDE_CALERRH
#define _INCLUDE_CALERRH

/******************* Includes ***********************************************/

/******************* Defines ************************************************/
#define CALERR_OPAQUE			(0)
#define CALERR_OLDPENDRIVER		(1)
#define CALERR_CANTOPENPENDRIVER	(3)
#define CALERR_NOPEN			(4)
#define CALERR_UNUSUALINPUT		(5)

/******************* Macros *************************************************/

/******************* Typedefs ***********************************************/

/******************* Externals **********************************************/

/******************* Public Prototypes **************************************/

/******************* Export Prototypes **************************************/

VOID CalError(WORD wErr);

#endif // _INCLUDE_CALERRH
