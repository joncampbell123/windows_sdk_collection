/****************************************************************************
 *   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *   PURPOSE.
 *
 *   Copyright (c) 1993 - 1995	Microsoft Corporation.	All Rights Reserved.
 *
 *  File:       msjstick.h
 *  Content:	joystick include file
 *
 ***************************************************************************/

extern WORD PASCAL jsPollj2( int zOnly );
extern void PASCAL jsSetVJoyDData( LPVOID jd );
extern LRESULT PASCAL jsGetPos( UINT id, LPJOYINFO lpInfo );
extern LRESULT PASCAL jsGetPosEx( UINT id, LPJOYINFOEX lpInfo );
extern LRESULT PASCAL jsGetHWCaps( UINT id, LPJOYHWCAPS );

extern int PASCAL jsInitVJoyD( void );

#define IDS_DESCRIPTION 	0x0001
#define MIN_PERIOD		10	/* minimum polling period */
#define MAX_PERIOD		1000	/* maximum polling period */
#define DEFAULT_DELTA		100	/* default scale value for values */
#define DEFAULT_RANGE_MIN	0	/* default min value returned for axis*/
#define DEFAULT_RANGE_MAX	65535	/* default max value returned for axis*/
#define DEFAULT_TIMEOUT		5000	/* default timeout value when polling */
#define DEFAULT_DEADZONE	5	/* default dead zone around center = 5% */
#define DEFAULT_HW		655	/* default max hw count */
