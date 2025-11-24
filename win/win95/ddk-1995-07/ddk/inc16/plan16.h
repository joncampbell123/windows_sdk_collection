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

/*****************************************************************************\
* PLAN16.H - PORTABILITY MAPPING HEADER FILE FOR LANMAN API
*
* This file provides macros to map portable lanman code to its 16 bit form.
*
\*****************************************************************************/

/*-----------------------------------LANMAN----------------------------------*/

/* LANMAN MACROS: */

#define COPYTOARRAY(pDest, pSource)     strcpy(pDest, pSource)

/* LANMAN API: */
