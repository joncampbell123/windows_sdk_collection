/*--------------------------------------------------------------+
| AVIClip.h - definitions for AVIClip app			|
|								|
+--------------------------------------------------------------*/
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/

/* Menu Items */

#define IDM_OPEN 		1
#define	IDM_SNIP 		2
#define IDM_EXIT		3

#define MENU_AVICLIP	20 
#define ID_APP     	1000
#define ID_NULL		2000
#define IDD_VERSION	200
#define	PALVERSION     	0x0300

/* DIB Macros */
#define DibColors(lpbi)      ((LPRGBQUAD)((LPBYTE)(lpbi) + (int)(lpbi)->biSize))
