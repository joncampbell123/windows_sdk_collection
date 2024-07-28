/**[f******************************************************************
* memoman.h -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/
/*********************************************************************/
/*
*   revisions:
*
*   02-08-90    VO     Added pixel height to DownLoadSoft arg list
*
*/
  
/*
* $Header: memoman.h,v 3.890 92/02/06 16:13:19 dtk FREEZE $
*/
  
/*
* $Log:	memoman.h,v $
 * Revision 3.890  92/02/06  16:13:19  16:13:19  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.870  91/11/08  11:45:07  11:45:07  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:53:05  13:53:05  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:48:24  13:48:24  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:49:59  09:49:59  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  15:00:52  15:00:52  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:51:08  16:51:08  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:18:22  14:18:22  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:34:41  16:34:41  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:35:49  10:35:49  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:13:22  14:13:22  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.812  91/08/22  14:33:15  14:33:15  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.807  91/08/08  10:32:31  10:32:31  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.803  91/08/02  15:49:50  15:49:50  dtk (Doug Kaltenecker)
 * Added FRAGMEM at 250K to account for fragmented mem in the printer.
 * 
 * Revision 3.802  91/07/22  12:55:39  12:55:39  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.799  91/07/02  11:53:00  11:53:00  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.796  91/06/26  11:27:21  11:27:21  stevec (Steve Claiborne)
 * BETA
 * 
 * Revision 3.790  91/06/11  16:04:35  16:04:35  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.786  91/06/11  15:45:51  15:45:51  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.785  91/05/22  14:58:11  14:58:11  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.780  91/05/15  15:58:19  15:58:19  stevec (Steve Claiborne)
* Beta
*
* Revision 3.775  91/04/05  14:32:12  14:32:12  stevec (Steve Claiborne)
* Beta release to MS
*
* Revision 3.770  91/03/25  15:37:11  15:37:11  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.760  91/03/12  07:54:01  07:54:01  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/03  07:47:16  07:47:16  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.720  91/02/11  09:16:35  09:16:35  stevec (Steve Claiborne)
* Aldus version
*
* Revision 3.710  91/02/04  15:48:48  15:48:48  stevec (Steve Claiborne)
* Aldus freeze
*
* Revision 3.700  91/01/19  09:01:29  09:01:29  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:44:27  15:44:27  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.681  91/01/14  10:18:41  10:18:41  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.680  91/01/10  16:17:47  16:17:47  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.670  90/12/14  14:55:19  14:55:19  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:36:58  15:36:58  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.660  90/12/07  14:51:27  14:51:27  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.650  90/11/30  08:13:19  08:13:19  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 3.603  90/09/06  15:00:26  15:00:26  oakeson (Ken Oakeson)
* Added constants and typedefs for dynamic storage of char download data
*
* Revision 3.602  90/08/29  17:44:14  17:44:14  oakeson (Ken Oakeson)
* Set CHUNK_SIZE and added params to functions
*
* Revision 3.601  90/08/24  13:23:00  13:23:00  daniels ()
* ../message.txt
*
* Revision 3.600  90/08/03  11:11:01  11:11:01  stevec (Steve Claiborne)
* This is the Aug. 3 release ver. 3.600
*
* Revision 3.550  90/07/27  11:32:18  11:32:18  root ()
* Experimental freeze 3.55
*
* Revision 3.540  90/07/25  12:36:39  12:36:39  stevec (Steve Claiborne)
* Experimental freeze 3.54
*
* Revision 3.520  90/06/13  16:54:02  16:54:02  root ()
* 5_2_release
*
*
*    Rev 1.1   20 Feb 1990 15:49:14   vordaz
* Support for downloadables.
*/
  
#define BACKUP_LC   500 /* large last code, in case file is hosed */
  
#define CHUNK_SIZE  1024    /* size of chunks sent to printer */
  
#define MINMEM      50000   /* minimum KB to image a page, don't let
                             * soft fonts use more than this */

#define FRAGMEM     256000  /* amount of memory to subtract from freemem
                             * cause of mem fragmentation */

#define MAXSOFTNUM  32      /* max number of fonts that may be dwnlded */
#define MAXSOFTPERPG 16     /* max nbr of fonts that may be used per page */
  
#define CharMEM(i)  ((17*i)>>2)     /*i= number of characters*/
  
/*number of bytes per raster line*/
#define RasterMEM(i) (i+10)     /*i= number of bytes on raster line*/
  
#ifdef SEG_PHYSICAL
/*** Tetra II begin ***/
/*** Added pixel height to arg list ***/
BOOL FAR PASCAL DownLoadSoft(LPDEVICE, short, short, LPSTR, short);
/*** Tetra II end ***/
void FAR PASCAL UpdateSoftInfo(LPDEVICE, LPFONTSUMMARYHDR, short);
#endif
  
typedef WORD far *LPWORD;
typedef short far *LPSHORT;
typedef long far *LPLONG;
