//---------------------------------------------------------------------------
//
//  Module:   resource.h
//
//  Description:
//
//
//
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/

#define IDS_DRIVER_SYNTHNAME     0x100

#define IDS_ALERT_MSG            0x110
#define IDS_ALERT_NOVXD          0x111

#define DATA_FMPATCHES           1234

#ifndef RC_INVOKED
#define RT_BINARY                MAKEINTRESOURCE( 256 )
#else
#define RT_BINARY                256
#endif

//---------------------------------------------------------------------------
//  End of File: resource.h
//---------------------------------------------------------------------------
