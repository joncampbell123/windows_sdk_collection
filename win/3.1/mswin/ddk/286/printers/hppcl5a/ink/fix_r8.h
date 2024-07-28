/**[f******************************************************************
* fix_r8.h -
*
* Copyright (C) 1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
* Translation table to unscramble Roman-8.
*
**f]*****************************************************************/
  
/*
* $Header: fix_r8.h,v 3.890 92/02/06 16:13:42 dtk FREEZE $
*/
  
/*
* $Log:	fix_r8.h,v $
 * Revision 3.890  92/02/06  16:13:42  16:13:42  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.870  91/11/08  11:45:33  11:45:33  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:53:31  13:53:31  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:48:49  13:48:49  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:50:26  09:50:26  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  15:01:17  15:01:17  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:51:34  16:51:34  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:18:47  14:18:47  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:35:07  16:35:07  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:36:25  10:36:25  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:13:48  14:13:48  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.812  91/08/22  14:33:41  14:33:41  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.807  91/08/08  10:32:56  10:32:56  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.802  91/07/22  12:56:07  12:56:07  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.799  91/07/02  11:53:28  11:53:28  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.796  91/06/26  11:27:50  11:27:50  stevec (Steve Claiborne)
 * BETA
 * 
 * Revision 3.790  91/06/11  16:05:02  16:05:02  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.786  91/06/11  15:46:28  15:46:28  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.785  91/05/22  14:58:38  14:58:38  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.780  91/05/15  15:58:47  15:58:47  stevec (Steve Claiborne)
* Beta
*
* Revision 3.775  91/04/05  14:32:40  14:32:40  stevec (Steve Claiborne)
* Beta release to MS
*
* Revision 3.770  91/03/25  15:37:39  15:37:39  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.760  91/03/12  07:54:27  07:54:27  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/03  07:47:42  07:47:42  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.720  91/02/11  09:17:01  09:17:01  stevec (Steve Claiborne)
* Aldus version
*
* Revision 3.710  91/02/04  15:49:13  15:49:13  stevec (Steve Claiborne)
* Aldus freeze
*
* Revision 3.700  91/01/19  09:01:55  09:01:55  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:44:53  15:44:53  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.681  91/01/14  10:19:06  10:19:06  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.680  91/01/10  16:18:14  16:18:14  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.670  90/12/14  14:55:46  14:55:46  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:37:24  15:37:24  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.660  90/12/07  14:51:55  14:51:55  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.650  90/11/30  08:13:44  08:13:44  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 3.601  90/08/24  13:21:58  13:21:58  daniels (Susan Daniels)
* ../message.txt
*
* Revision 3.600  90/08/03  11:11:31  11:11:31  stevec (Steve Claiborne)
* This is the Aug. 3 release ver. 3.600
*
* Revision 3.550  90/07/27  11:32:49  11:32:49  root ()
* Experimental freeze 3.55
*
* Revision 3.540  90/07/25  12:37:24  12:37:24  stevec (Steve Claiborne)
* Experimental freeze 3.54
*
* Revision 3.520  90/07/21  11:39:53  11:39:53  root ()
* Created mappings based on scramble (translate) table in trans.h
*
*/
  
/* Note that the array begins with 0xa1, not 0xa0 or 0x00 */
  
unsigned char Fix_R8[] = {
    0xc0,   /* a1 */
    0xc2,   /* a2 */
    0xc8,   /* a3 */
    0xca,   /* a4 */
    0xcb,   /* a5 */
    0xce,   /* a6 */
    0xcf,   /* a7 */
    0xb4,   /* a8 */
    0xa8,   /* a9 */
    0x5e,   /* aa */
    0xa8,   /* ab */
    0xab,   /* ac */
    0xd9,   /* ad */
    0xdb,   /* ae */
    0xa3,   /* af */
  
    0xaf,   /* b0 */
    0x59,   /* b1 */
    0x79,   /* b2 */
    0xb0,   /* b3 */
    0xc7,   /* b4 */
    0xe7,   /* b5 */
    0xd1,   /* b6 */
    0xf1,   /* b7 */
    0xa1,   /* b8 */
    0xbf,   /* b9 */
    0xa4,   /* ba */
    0xa3,   /* bb */
    0xa5,   /* bc */
    0xa7,   /* bd */
    0xbd,   /* be */
    0xa2,   /* bf */
  
    0xe2,   /* c0 */
    0xea,   /* c1 */
    0xf4,   /* c2 */
    0xfb,   /* c3 */
    0xe1,   /* c4 */
    0xe9,   /* c5 */
    0xf3,   /* c6 */
    0xfa,   /* c7 */
    0xe0,   /* c8 */
    0xe8,   /* c9 */
    0xf2,   /* ca */
    0xf9,   /* cb */
    0xe4,   /* cc */
    0xeb,   /* cd */
    0xf6,   /* ce */
    0xfc,   /* cf */
  
    0xc5,   /* d0 */
    0xee,   /* d1 */
    0xd8,   /* d2 */
    0xc6,   /* d3 */
    0xe5,   /* d4 */
    0xed,   /* d5 */
    0xf8,   /* d6 */
    0xe6,   /* d7 */
    0xc4,   /* d8 */
    0xec,   /* d9 */
    0xd6,   /* da */
    0xdc,   /* db */
    0xc9,   /* dc */
    0xef,   /* dd */
    0xdf,   /* de */
    0xd4,   /* df */
  
    0xc1,   /* e0 */
    0xc3,   /* e1 */
    0xe3,   /* e2 */
    0xd0,   /* e3 */
    0xf0,   /* e4 */
    0xcd,   /* e5 */
    0xcc,   /* e6 */
    0xd3,   /* e7 */
    0xd2,   /* e8 */
    0xd5,   /* e9 */
    0xf5,   /* ea */
    0x53,   /* eb */
    0x73,   /* ec */
    0xda,   /* ed */
    0x59,   /* ee */
    0xff,   /* ef */
  
    0xde,   /* f0 */
    0xfe,   /* f1 */
    0xb7,   /* f2 */
    0xb5,   /* f3 */
    0xb6,   /* f4 */
    0xbe,   /* f5 */
    0xb1,   /* f6 */
    0xbc,   /* f7 */
    0xbd,   /* f8 */
    0xaa,   /* f9 */
    0xba,   /* fa */
    0xab,   /* fb */
    0x6f,   /* fc */
    0xbb,   /* fd */
    0xb1,   /* fe */
    0xa0,   /* ff */
};
