/****************************************************************************
 *   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *   PURPOSE.
 *
 *   Copyright (c) 1994 - 1995	Microsoft Corporation.	All Rights Reserved.
 *
 *  File:       vjoydapi.h
 *  Content:	VJOYDAPI service equates and structures
 *
 ***************************************************************************/

#define	MULTIMEDIA_OEM_ID	0x0440		       // MS Reserved OEM # 34
#define	VJOYD_Device_ID		MULTIMEDIA_OEM_ID + 9  // VJOYD API Device

#define VJOYD_Ver_Major           1               
#define VJOYD_Ver_Minor           0

/* VJOYDAPI_Get_Version
//;
//   ENTRY:
//	AX = 0
//
//   RETURNS:
//	SUCCESS: AX == TRUE
//	ERROR:   AX == FALSE
*/
#define	VJOYDAPI_GetVersion		0
#define	VJOYDAPI_IOCTL_GetVersion	0

/* VJOYDAPI_GetPosEx
//;
//   ENTRY:
//	AX = 1
//	DX = joystick id (0,1)
//	ES:BX = pointer to JOYINFOEX struct
//
//   RETURNS:
//	SUCCESS: EAX == MMSYSERR_NOERROR
//	ERROR:   EAX == JOYERR_PARMS
//			JOYERR_UNPLUGGED
*/
#define	VJOYDAPI_GetPosEx	1
#define	VJOYDAPI_IOCTL_GetPosEx	1

/* VJOYDAPI_GetPos
//;
//   ENTRY:
//	AX = 1
//	DX = joystick id (0,1)
//	ES:BX = pointer to JOYINFO struct
//
//   RETURNS:
//	SUCCESS: EAX == MMSYSERR_NOERROR
//	ERROR:   EAX == JOYERR_PARMS
//			JOYERR_UNPLUGGED
*/
#define	VJOYDAPI_GetPos		2
#define	VJOYDAPI_IOCTL_GetPos	2

/* VJOYDAPI_SetData
//;
//   ENTRY:
//	AX = 3
//	ES:BX = pointer to joystick data (private)
//;
//   RETURNS:
//	SUCCESS: Always
*/
#define	VJOYDAPI_SetData	3				/* ;Internal */
#define	VJOYDAPI_IOCTL_SetData	3				/* ;Internal */

/* VJOYDAPI_GetHWCaps
//;
//   ENTRY:
//	AX = 4
//	ES:BX = pointer to hw caps data (private)
//;
//   RETURNS:
//	SUCCESS: Always
*/
#define	VJOYDAPI_GetHWCaps	4				/* ;Internal */
