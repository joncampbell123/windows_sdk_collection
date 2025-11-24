/****************************************************************************
 *   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *   PURPOSE.
 *
 *   Copyright (c) 1994 - 1995	Microsoft Corporation.	All Rights Reserved.
 *
 *  File:       joycom.h
 *  Content:	common interface between MSJSTICK and VJOYD
 *
 ***************************************************************************/
#ifndef __JOYCOM_INCLUDED__
#define __JOYCOM_INCLUDED__

#define MAX_JOYSTICKS	16	/* maximum number of joysticks allowed */
#define NUM_JOYSTICKS	2	/* number of joysticks we can poll */

#ifndef JOYSTICKID3
#define JOYSTICKID3	JOYSTICKID2+1
#endif
#ifndef JOYSTICKID4
#define JOYSTICKID4	JOYSTICKID3+1
#endif

#define POV_MIN		0
#define POV_MAX		1

#define Z_ISJ2Y			(DWORD) -1 /* Z is polled for j2, y axis */
#define Z_ISJ2X			1	/* Z is polled for j2, x axis */

typedef struct {
    JOYREGHWCONFIG	hw;		/* hardware config info for joysticks */
    JOYPOS		cal_hw; 	/* hardware calibration values */
    DWORD		povcal[2][JOY_POV_NUMDIRS]; /* cal. constants for POV */
    DWORD		z_axis;		/* which axis for Z info */
    DWORD		has_z;		/* current joystick is 3D */
    DWORD		has_r;		/* current joystick has a rudder or 4th axis*/
    DWORD		has_pov;	/* current joystick has a POV hat */
    DWORD		pov_is_poll;	/* current POV hat is polled (else is buttons) */
    DWORD		has_u;		/* current joystick has 5th axis */
    DWORD		has_v;		/* current joystick has 6th axis */
    DWORD		present;
} IJOYDATA, FAR *LPIJOYDATA;

typedef struct {
    JOYREGUSERVALUES	usr;		/* ranges specified by user */
    JOYPOS		cal_usr;	/* usr calibration values */
    IJOYDATA		ijd[MAX_JOYSTICKS]; /* joystick specific data */
} JOYDATA, FAR *LPJOYDATA;

typedef struct {
    DWORD	dwMaxButtons;
    DWORD	dwMaxAxes;
    DWORD	dwNumAxes;
    char	szOEMVxD[MAX_JOYSTICKOEMVXDNAME];
} JOYHWCAPS, FAR *LPJOYHWCAPS;

#endif
