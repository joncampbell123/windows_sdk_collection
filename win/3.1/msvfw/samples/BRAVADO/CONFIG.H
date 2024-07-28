/****************************************************************************
 *
 *   config.h
 * 
 *   Dialog box definitions
 *
 *   Microsoft Video for Windows Sample Capture Driver
 *   Chips & Technologies 9001 based frame grabbers.
 *
 ***************************************************************************/
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

/* Video Configuration dialog */
#define DLG_VIDEOCONFIG             100
#define ID_LBPORTBASEADDRESS        101
#define ID_LBINTERRUPTNUMBER        102
#define ID_LBMEMORYBASEADDRESS      103
#define ID_LBWAITSTATE              104
#define ID_LBBOARDTYPE              105
#define ID_FILEDESCRIPTION          106
#define ID_DRIVERVERSION            107

/* Video Format dialog */
#define DLG_VIDEOFORMAT             200
#define ID_LBIMAGEFORMAT            201
#define ID_PBSIZEFULL               202
#define ID_PBSIZEHALF               203
#define ID_PBSIZEQUARTER            204
#define ID_PBSIZEEIGHTH             205
#define ID_LBSIZE                   206

/* Video Source dialog */
#define DLG_VIDEOSOURCE             300
#define ID_SBHUE                    310
#define ID_SBSAT                    311
#define ID_SBBRIGHTNESS             312
#define ID_SBCONTRAST               313
#define ID_SBRED                    314
#define ID_SBGREEN                  315
#define ID_SBBLUE                   316

#define ID_EBHUE                    320
#define ID_EBSAT                    321
#define ID_EBBRIGHTNESS             322
#define ID_EBCONTRAST               323
#define ID_EBRED                    324
#define ID_EBGREEN                  325
#define ID_EBBLUE                   326

#define ID_PBSOURCE0                330
#define ID_PBSOURCE1                331
#define ID_PBSOURCE2                332

#define ID_PBNTSC                   340
#define ID_PBPAL                    341

#define ID_PBCOMPOSITE              345
#define ID_PBSVIDEO                 346

#define ID_PBDEFAULT                350

/* Video Display dialog */
#define DLG_VIDEODISPLAY            400
#define ID_RBDISPLAYBUFFER          401
#define ID_RBDISPLAYLIVE            402
