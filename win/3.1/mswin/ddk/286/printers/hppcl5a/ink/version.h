/**[f******************************************************************
* version.h -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/
/*
*  30 jun 91  SD     Version 3.798 -- Beta for MS Windows 3.1
*  26 jun 91  SD     Version 3.796 -- Beta for MS Windows 3.1
*  15 may 91  SD     Version 3.78 -- Beta for MS Windows 3.1
*  28 Aug 90  SD     Version 3.601 -- Fix Bug #24 in environ.c.
*  90 Jul 90  SJC    Version 3.55 -- Freezing beta version of eventual 3.6.
*  90 jul 90  SD     Version 3.54 -- Freezing beta version of eventual 3.6.
*  21 jun 90  SD     Version 3.53 -- Added LJ IIID support.
*                                 -- Fixed Bug #16: incorrect paper sizes
*                                    in list.
*
*  08 may 90  SD     Change driver version number 3.52
*                           hppcl.def -- change library to hppcl5a.
*                           Make changes sent by MS for memory problem.
*
*                    Driver version 3.51 contains SAMNA fix and kerning
bugfix.
*
*  01 feb 90  SD     Change driver version number for LJIII only version
*                       to v.3.4.7
*
*  10 jan 90  SD     Change driver version number for Tetra to V.3.4
*/
  
/*
* $Header: version.h,v 3.890 92/02/06 16:13:01 dtk FREEZE $
*/
  
/*
* $Log:	version.h,v $
 * Revision 3.890  92/02/06  16:13:01  16:13:01  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.875  92/02/06  15:54:23  15:54:23  dtk (Doug Kaltenecker)
 * 
 * 
* Revision 3.874  92/01/10  13:07:34  13:07:34  dtk (Doug Kaltenecker)
 * *** empty log message ***
 * 
 * Revision 3.87
* Revision 3.785  91/05/22  14:57:51  14:57:51  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.781  91/05/22  14:17:23  14:17:23  oakeson (Ken Oakeson)
* Changed version number to 3.785 beta for minor MS rev
*
* Revision 3.780  91/05/15  15:57:59  15:57:59  stevec (Steve Claiborne)
* Beta
*
* Revision 3.776  91/05/15  14:43:09  14:43:09  daniels (Susan Daniels)
* Version 3.78:  Beta for MS Windows 3.1 Beta release.
*
* Revision 3.775  91/04/05  14:54:43  14:54:43  stevec (Steve Claiborne)
* *** empty log message ***
*
* Revision 3.771  91/04/05  14:11:41  14:11:41  daniels (Susan Daniels)
* Version 3.77 Beta:  3.77 with the Truetype landscape fix.
* Sent to MS for their Windows 3.1 Beta release.
* Did not show the GP fault when printing article.doc in Times New Roman.
*
* Revision 3.770  91/03/25  15:36:51  15:36:51  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.760  91/03/12  07:53:40  07:53:40  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/03  07:46:56  07:46:56  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.721  91/02/11  19:12:46  19:12:46  daniels (Susan Daniels)
* Adding ELI version 3.73 Beta
*
* Revision 3.720  91/02/11  09:47:01  09:47:01  stevec (Steve Claiborne)
* Encorporates fixes for the Aldus release of driver.
*
* Revision 3.711  91/02/11  00:41:26  00:41:26  oakeson (Ken Oakeson)
* v3.72 for SETCHARSET support
*
* Revision 3.710  91/02/04  17:06:34  17:06:34  stevec (Steve Claiborne)
* Updated the version # to 3.710
*
* Revision 3.700  91/01/19  09:01:09  09:01:09  stevec (Steve Claiborne)
* Release
*
* Revision 3.687  91/01/18  17:19:58  17:19:58  stevec (Steve Claiborne)
* Release version
*
* Revision 3.686  91/01/14  15:47:44  15:47:44  stevec (Steve Claiborne)
* Updated version # to 3.685.
*
* Revision 3.685  91/01/14  15:44:08  15:44:08  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.681  91/01/14  10:18:21  10:18:21  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.680  91/01/10  16:17:27  16:17:27  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.670  90/12/14  14:54:58  14:54:58  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.666  90/12/14  14:48:05  14:48:05  stevec (Steve Claiborne)
* 12-14-90 freeze - version 3.67
*
* Revision 3.665  90/12/10  15:36:38  15:36:38  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.661  90/12/10  15:24:31  15:24:31  stevec (Steve Claiborne)
* Version 3.665
*
* Revision 3.660  90/12/07  14:51:07  14:51:07  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.652  90/12/07  14:29:23  14:29:23  stevec (Steve Claiborne)
* 12-7-90 Freeze
*
* Revision 3.651  90/11/30  08:59:32  08:59:32  stevec (Steve Claiborne)
* Rolled version # to 3.65
*
* Revision 3.650  90/11/30  08:12:59  08:12:59  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 3.604  90/11/27  12:48:11  12:48:11  stevec (Steve Claiborne)
* Updated version # to reflect experimental status
*
* Revision 3.603  90/09/12  17:27:17  17:27:17  oakeson (Ken Oakeson)
* Moved 3.62 for character downloading on-demand
*
* Revision 3.602  90/08/28  14:18:46  14:18:46  daniels ()
* Roll version to 3.601 for Bug #24 fix for Aldus.  Only
* file to change from 3.600 is environ.c.
*
* Revision 3.600  90/08/03  11:10:40  11:10:40  stevec (Steve Claiborne)
* This is the Aug. 3 release ver. 3.600
*
* Revision 3.540  90/07/25  12:35:49  12:35:49  stevec (Steve Claiborne)
* Experimental freeze 3.54
*
* Revision 3.522  90/07/21  11:11:32  11:11:32  daniels ()
* Change version to 3.54: Beta version with LJ IIID, white text, print
* direction, performance enhancements, bug fixes.
*
* Revision 3.521  90/06/21  17:12:21  17:12:21  daniels ()
* Version 3.53:  Bug fix for # 16.  LJ IIID support.
*
*
*    Rev 1.3   09 May 1990 13:50:14   daniels
* Rolled in fixes MS sent that were put into 3.42.  Changed to V3.52.
*
*    Rev 1.2   08 May 1990 14:55:14   vordaz
* change to v3.51
*/

/*  VNUMint should reflect the current driver version number  */  
#define VNUMint (0x3881)
  

/*  The code that creates the font summary file in fontman.c, 
 *  putFileFontSummary(), truncates version strings that are over 17
 *  characters long, appends a '\0', and writes that to the file.  Be
 *  aware of this and be sure that the significant version information
 *  is in the first 17 characters.
 */
#if defined(DEBUG)
#define VNUM "DEBUG Version 31.3.89 for Windows 3.1"
#else
#define VNUM "Version 31.3.89 for Windows 3.1"
#endif
  
// date for PCLSTUB.C only.
#define VDATE "Feb 6 92"






