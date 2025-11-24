/****************************************************************************
 *   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *   PURPOSE.
 *
 *   Copyright (c) 1994 - 1995	Microsoft Corporation.	All Rights Reserved.
 *
 *  File:       getpos.h
 *  Content:	position computing header file
 *
 ***************************************************************************/
#ifndef __GETPOS_INCLUDED__
#define __GETPOS_INCLUDED__

#include <configmg.h>
#include "mm.h"
#include "vjoyd.h"
#include "joycom.h"

/*
 * describes other vxd info
 */
typedef struct {
    char		*pszVxDModName;
    char		*pszVxDName;
    JOYOEMPOLLRTN	pExternalPoll;
    JOYOEMHWCAPSRTN	pExternalHWCaps;
    CMCONFIGHANDLER	pExternalCfg;
    JOYOEMJOYIDRTN	pExternalJoyId;
} VXDINFO, *LPVXDINFO;

typedef struct {
    BOOL	bUsingMiniVxD;
    int (__stdcall *doPoll1)(DWORD id, DWORD axis);
    int (__stdcall *doPoll2)(DWORD id);
    int (__stdcall *doPoll3)(DWORD id, DWORD do_r);
    int (__stdcall *doPoll4)(DWORD id);
    int (__stdcall *doPoll5)(DWORD id, DWORD do_v );
    int (__stdcall *doPoll6)(DWORD id);
    void (__stdcall *doGetButtons)(DWORD id);
} JOYPROCESSING, *LPJOYPROCESSING;

extern void __stdcall RegisterDeviceDriver( JOYOEMPOLLRTN pollrtn,
				     CMCONFIGHANDLER cfgrtn,
				     JOYOEMHWCAPSRTN capsrtn,
				     JOYOEMJOYIDRTN joyidrtn );

typedef struct _VXDUSAGE {
    LPVXDINFO		pvi;
    int			iUsage;
} VXDUSAGE, *LPVXDUSAGE;

/*
 * joystick port bits
 */
#define JOY1_X_MASK	0x0001
#define JOY1_Y_MASK	0x0002
#define JOY2_X_MASK	0x0004
#define JOY2_Y_MASK	0x0008

/*
 * joystick ports
 */
#define MIN_JOY_PORT		0x200
#define MAX_JOY_PORT		0x20F
#define DEFAULT_JOY_PORT	0x201

/*
 * shift amounts to get to bits
 */
#define JOY1_X_SHIFT		0
#define JOY1_Y_SHIFT		1
#define JOY2_X_SHIFT		2
#define JOY2_Y_SHIFT		3
#define JOY1_BUTTON_SHIFT	4
#define JOY2_BUTTON_SHIFT	6

/*
 * counts for dual port joystick ports
 */
#define MIN_JOYPORT_AXES	2
#define MAX_JOYPORT_AXES	4
#define ONE_DUALPORT_JOYCNT	2
#define TWO_DUALPORT_JOYCNT	4
#define MIN_JOYPORT_BUTTONS	2
#define MAX_JOYPORT_BUTTONS	4

/*
 * these are found in fixdata.c - page locked data for use by polling routines
 */
extern DWORD	dwXpos;
extern DWORD	dwYpos;
extern DWORD	dwZpos;
extern DWORD	dwRpos;
extern DWORD	dwButtons;
extern DWORD	dwTimeOut;
extern DWORD	bPass1;
extern DWORD	dwJoyPort;
extern char	cXshift[4];
extern char	cYshift[4];
extern char	cZshift[4];
extern char	cRshift[4];
extern char	cButtonShift[4];
extern char	cXYBits[4];
extern char	c3Bits[4];
extern char	cAllBits[4];
extern char	cXbit[4];
extern char	cYbit[4];
extern char	cRbit[4];
extern char	cZbit[4];

/*
 * found in getpos.c - global data, pageable
 */
extern DWORD	dwJoyPortList[MAX_JOYSTICKS];
extern DWORD	dwJoyPortCount;
extern DWORD	dwMaxAxes[MAX_JOYSTICKS];

/*
 * found in othervxd.c - global data, pageable
 */
extern LPVXDINFO pVxDInfo[MAX_JOYSTICKS+1];
extern DWORD	dwOEMJoyId;
extern int	iNumVxDs;
extern VXDUSAGE	VxDUsage[MAX_JOYSTICKS+1];

/*
 * found in the various poll*.asm files
 */
extern void __stdcall jsGetButtons( DWORD id );
extern int __stdcall jsPoll1( DWORD id, DWORD axis );
extern int __stdcall jsPoll2( DWORD id );
extern int __stdcall jsPoll3( DWORD id, DWORD do_r );
extern int __stdcall jsPoll4( DWORD id );

/*
 * found in othervxd.c
 */
extern void LoadOtherVxDs( void );

/*
 * foundin getpos.c
 */
void InitJoyProcessing( void );

/*
 * REAL_POS converts a raw position to a "real" position in the users range
 */
#define REAL_POS( res, pijd, a ) \
    { \
	int pos = (int) dw##a##pos-(int)pijd->hw.hwv.jrvHardware.jpMin.dw##a; \
	if( pos < 0 ) { \
	    res = jd.usr.jrvRanges.jpMin.dw##a; \
	} else { \
	    DWORD	r; \
	    r = (((DWORD)pos * jd.cal_usr.dw##a##)/pijd->cal_hw.dw##a##) + jd.usr.jrvRanges.jpMin.dw##a; \
	    if( r > jd.usr.jrvRanges.jpMax.dw##a ) { \
		res = jd.usr.jrvRanges.jpMax.dw##a; \
	    } else { \
		res = r; \
	    } \
	} \
    }

#endif
