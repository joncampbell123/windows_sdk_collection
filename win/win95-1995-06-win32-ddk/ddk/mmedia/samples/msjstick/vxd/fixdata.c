/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1994 - 1995  Microsoft Corporation.  All Rights Reserved.
 * 
 *  File:       fixdata.c
 *  Content:	data that is used with interrupts disabled
 */
#define WANTVXDWRAPS
#include <basedef.h>
#include <vmm.h>
#include <vxdwraps.h>
#include <configmg.h>
#include <debug.h>
#include "vjoyd.h"
#include "getpos.h"

#undef CURSEG               
#define CURSEG()                   LCODE

#pragma VxD_LOCKED_CODE_SEG
#pragma VxD_LOCKED_DATA_SEG

DWORD	dwXpos;
DWORD	dwYpos;
DWORD	dwZpos;
DWORD	dwRpos;
DWORD	dwButtons;
DWORD	dwTimeOut = 0x2000;
DWORD	bPass1;
DWORD	dwJoyPort = DEFAULT_JOY_PORT;
char	cButtonShift[4] = { JOY1_BUTTON_SHIFT, JOY2_BUTTON_SHIFT };
char	cXshift[4] = { JOY1_X_SHIFT, JOY2_X_SHIFT };
char	cYshift[4] = { JOY1_Y_SHIFT, JOY2_Y_SHIFT };
char	cZshift[4];
char	cRshift[4];
char	c3Bits[4] = { JOY1_X_SHIFT | JOY1_Y_SHIFT, JOY1_X_SHIFT | JOY1_Y_SHIFT };
char	cXYBits[4] = { JOY1_X_SHIFT | JOY1_Y_SHIFT, JOY2_X_SHIFT | JOY2_Y_SHIFT };
char	cAllBits[4] = { JOY1_X_SHIFT | JOY1_Y_SHIFT, JOY2_X_SHIFT | JOY2_Y_SHIFT };
char	cXbit[4] = { JOY1_X_MASK, JOY2_X_MASK };
char	cYbit[4] = { JOY1_Y_MASK, JOY2_Y_MASK };
char	cRbit[4];
char	cZbit[4];
