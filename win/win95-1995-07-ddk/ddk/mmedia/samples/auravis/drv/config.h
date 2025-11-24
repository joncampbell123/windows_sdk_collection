//////////////////////////////////////////////////////////////////////////
//	CONFIG.H							//
//									//
//	Configuration dialog box definitions.				//
//									//
//	For the AuraVision video capture driver AVCAPT.DRV.		//
//									//
//////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1994 - 1995 Microsoft Corporation.	All Rights Reserved.
//
//---------------------------------------------------------------------------

// Video Configuration dialog

#define DLG_VIDEOCONFIG             100

#define ID_UNDER1MEG                110  
#define ID_OVER1MEG                 111
#define IDH_ORCHID_VIDEO_CARD       5000

#define ID_PH9051411                115
#define ID_PH7191422                116
#define ID_PH7110411                117
#define ID_PH7110422                118
#define IDH_ORCHID_VIDEO_DECODE     5001

// Video Format dialog

#define DLG_VIDEOFORMAT             200
#define ID_LBIMAGEFORMAT            201
#define IDH_ORCHID_IMAGE_FORMAT     5002

#define ID_PBSIZEFULL               202
#define ID_PBSIZEHALF               203
#define ID_PBSIZEQUARTER            204
#define ID_PBSIZEEIGHTH             205
#define ID_LBSIZE                   206
#define IDH_ORCHID_SIZE             5003

// Video Source dialog

#define DLG_VIDEOSOURCE             300
#define IDH_ORCHID_COLOR_CONTROLS   5004

#define ID_SBHUE                    310
#define ID_SBSATURATION             311
#define ID_SBBRIGHTNESS             312
#define ID_SBCONTRAST               313
#define ID_SBXPOSITION              314
#define ID_SBYPOSITION              315
#define IDH_ORCHID_POSITION         5005

#define ID_EBHUE                    320
#define ID_EBSATURATION             321
#define ID_EBBRIGHTNESS             322
#define ID_EBCONTRAST               323
#define ID_EBXPOSITION              324
#define ID_EBYPOSITION              325

#define ID_PBSOURCE0                330
#define ID_PBSOURCE1                331
#define ID_PBSOURCE2                332
#define IDH_ORCHID_VIDEO_CONNECT    5006


#define ID_PBNTSC                   340
#define ID_PBPAL                    341
#define IDH_ORCHID_VIDEO_STANDARD   5007

#define ID_PBCOMPOSITE              345
#define ID_PBSVIDEO                 346

#define ID_PBDEFAULT                350
#define IDH_ORCHID_DEFAULT          5008

#define ID_PBOK                     351

// Video Display dialog

#define DLG_VIDEODISPLAY            400
#define ID_RBDISPLAYBUFFER          401
#define ID_RBDISPLAYLIVE            402
