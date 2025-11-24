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

//---------------------------------------------------------------------------
//
// File: cstrings.c
//
//  This file contains read-only string constants
//
//---------------------------------------------------------------------------

#include "suiprv.h"

#pragma data_seg(DATASEG_READONLY)

char const FAR c_szNULL[] = "";
char const FAR c_szZero[] = "0";
char const FAR c_szDelim[] = " \t,";
char const FAR c_szBackslash[] = "\\";

char const FAR c_szWinHelpFile[] = "windows.hlp";

// Registry key names

char const FAR c_szClass[] = REGSTR_KEY_CLASS;
char const FAR c_szPortClass[] = "ports";
char const FAR c_szModemClass[] = "Modem";
char const FAR c_szDeviceDesc[] = "DeviceDesc";
char const FAR c_szPortName[] = "PortName";
char const FAR c_szFriendlyName[] = REGSTR_VAL_FRIENDLYNAME;
char const FAR c_szDCB[] = "DCB";

#pragma data_seg()

