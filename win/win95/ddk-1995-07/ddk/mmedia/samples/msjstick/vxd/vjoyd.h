/****************************************************************************
 *   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *   PURPOSE.
 *
 *   Copyright (c) 1994 - 1995	Microsoft Corporation.	All Rights Reserved.
 *
 *  File:       vjoyd.h
 *  Content:	include file for mini-joystick drivers to communicate with
 *		vjoyd
 *
 ***************************************************************************/
#ifndef __VJOYD_INCLUDED__
#define __VJOYD_INCLUDED__
#include "mm.h"
enum {
    JOY_OEMPOLL_POLL1=0,
    JOY_OEMPOLL_POLL2,
    JOY_OEMPOLL_POLL3,
    JOY_OEMPOLL_POLL4,
    JOY_OEMPOLL_POLL5,
    JOY_OEMPOLL_POLL6,
    JOY_OEMPOLL_GETBUTTONS
};

#define JOY_OEMPOLLRC_OK		1
#define JOY_OEMPOLLRC_FAIL		0
#define JOY_OEMPOLLRC_YOUPOLL		-1

#define JOY_AXIS_X	0
#define JOY_AXIS_Y	1
#define JOY_AXIS_Z	2
#define JOY_AXIS_R	3
#define JOY_AXIS_U	4
#define JOY_AXIS_V	5

#define POV_UNDEFINED	(DWORD) -1

#ifndef MAX_JOYSTICKS
    #define MAX_JOYSTICKS	16
#endif


typedef struct {
    DWORD	id;
    DWORD	do_other;
    JOYPOS	jp;
    DWORD	dwPOV;
    DWORD	dwButtons;
    DWORD	dwButtonNumber;
} JOYOEMPOLLDATA, *LPJOYOEMPOLLDATA;


typedef struct {
    DWORD	dwMaxButtons;
    DWORD	dwMaxAxes;
    DWORD	dwNumAxes;
} JOYOEMHWCAPS, *LPJOYOEMHWCAPS;

typedef int (__stdcall	*JOYOEMPOLLRTN)( int type, LPJOYOEMPOLLDATA pojd );

typedef int (__stdcall	*JOYOEMHWCAPSRTN)( int joyid, LPJOYOEMHWCAPS pohwcaps );

typedef int (__stdcall	*JOYOEMJOYIDRTN)( int joyid, BOOL inuse );
#endif
