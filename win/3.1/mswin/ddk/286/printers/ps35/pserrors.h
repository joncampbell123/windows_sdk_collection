/**[f******************************************************************
 * pserrors.h - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Company confidential.
 *
 **f]*****************************************************************/

#define PS_GENERAL	0
#define PS_CORRUPT	1
#define PS_RESOLUTION0	2
#define PS_COPIES0	3
//  #define PS_JOBTIMEOUT0  4
#define PS_DLGBUSY	4
#define PS_DOWNLOAD	5
#define PS_PORT		6
//  #define PS_AT	    7
//  #define PS_NEEDTRUETYPE 8
#define PS_SCALING0     9

#define IDS_ERROR_CAPTION_GENERAL	1000
#define IDS_ERROR_CAPTION_DATA		1001

#define IDS_ERROR_MESSAGE_GENERAL	1010
#define IDS_ERROR_MESSAGE_CORRUPT	1011
#define IDS_ERROR_MESSAGE_RESOLUTION	1012
#define IDS_ERROR_MESSAGE_COPIES	1013
// #define IDS_ERROR_MESSAGE_JOBTIMEOUT    1014
#define IDS_ERROR_MESSAGE_DLGBUSY	1014
#define IDS_ERROR_MESSAGE_DOWNLOAD	1015
#define IDS_ERROR_MESSAGE_PORT		1016
// #define IDS_ERROR_MESSAGE_AT 	   1017
// #define IDS_ERROR_MESSAGE_NEEDTRUETYPE  1018
#define IDS_ERROR_MESSAGE_SCALINGRANGE  1019

void FAR PASCAL PSError(short);
void FAR PASCAL PSDownloadError(short);
