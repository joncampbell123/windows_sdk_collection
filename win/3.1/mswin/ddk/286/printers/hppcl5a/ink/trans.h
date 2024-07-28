/**[f******************************************************************
* trans.h -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/
  
/*
* $Header: trans.h,v 3.890 92/02/06 16:13:14 dtk FREEZE $
*/
  
/*
* $Log:	trans.h,v $
 * Revision 3.890  92/02/06  16:13:14  16:13:14  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.870  91/11/08  11:45:02  11:45:02  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:53:00  13:53:00  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:48:19  13:48:19  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:49:53  09:49:53  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  15:00:47  15:00:47  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:51:03  16:51:03  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:18:17  14:18:17  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/20  15:08:18  15:08:18  dtk (Doug Kaltenecker)
 * Added 2 correct chars to Roman8 table
 * 
 * Revision 3.830  91/09/18  16:34:36  16:34:36  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.823  91/09/18  16:22:21  16:22:21  dtk (Doug Kaltenecker)
 * Changed to incorrect chars in the Roman-8 map.
 * 
 * Revision 3.822  91/09/16  10:35:42  10:35:42  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:13:17  14:13:17  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.812  91/08/22  14:33:10  14:33:10  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.808  91/08/21  15:49:43  15:49:43  jsmart (Jerry Smart)
 * add roman8 translation table back
 * 
 * Revision 3.807  91/08/08  10:32:25  10:32:25  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.802  91/07/22  12:55:34  12:55:34  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.799  91/07/02  11:52:55  11:52:55  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.796  91/06/26  11:27:16  11:27:16  stevec (Steve Claiborne)
 * BETA
 * 
 * Revision 3.790  91/06/11  16:04:29  16:04:29  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.786  91/06/11  15:45:43  15:45:43  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.785  91/05/22  14:58:05  14:58:05  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.780  91/05/15  15:58:14  15:58:14  stevec (Steve Claiborne)
* Beta
*
* Revision 3.775  91/04/05  14:32:07  14:32:07  stevec (Steve Claiborne)
* Beta release to MS
*
* Revision 3.770  91/03/25  15:37:06  15:37:06  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.760  91/03/12  07:53:55  07:53:55  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/03  07:47:11  07:47:11  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.720  91/02/11  09:16:29  09:16:29  stevec (Steve Claiborne)
* Aldus version
*
* Revision 3.710  91/02/04  15:48:43  15:48:43  stevec (Steve Claiborne)
* Aldus freeze
*
* Revision 3.700  91/01/19  09:01:23  09:01:23  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:44:22  15:44:22  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.681  91/01/14  10:18:35  10:18:35  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.680  91/01/10  16:17:41  16:17:41  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.670  90/12/14  14:55:14  14:55:14  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:36:52  15:36:52  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.660  90/12/07  14:51:22  14:51:22  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.650  90/11/30  08:13:13  08:13:13  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 3.601  90/08/24  13:24:31  13:24:31  daniels (Susan Daniels)
* ../message.txt
*
* Revision 3.600  90/08/03  11:10:57  11:10:57  stevec (Steve Claiborne)
* This is the Aug. 3 release ver. 3.600
*
* Revision 3.550  90/07/27  11:32:14  11:32:14  root ()
* Experimental freeze 3.55
*
* Revision 3.541  90/07/27  08:19:54  08:19:54  oakeson ()
* Nuked Generic8 translation table
*
* Revision 3.540  90/07/25  12:36:32  12:36:32  stevec (Steve Claiborne)
* Experimental freeze 3.54
*
* Revision 3.521  90/07/19  12:37:43  12:37:43  oakeson ()
* Removed Math8 and Roman8 translation tables
*
*/
  
// History
// 26 oct 89   peterbe Change Roman8 trans for a9,ac,ae,b2,b3,b6,b7,b8,b9,be,
//         all punctuation or special symbols.
// 18 oct 89   peterbe Multiply is 'x' and divide is '-' plus ':' in
//         USASCII, Roman8.
// 01 sep 89   peterbe Added copyright.
// 25 aug 89   craigc  Added MATH8 symbol set translation.
  
/* translation table using standard characters */
unsigned char USASCII_Trans[] = {
    HP_DF_CH, NULL, /*  80  */
    HP_DF_CH, NULL, /*  81  */
    HP_DF_CH, NULL, /*  82  */
    HP_DF_CH, NULL, /*  83  */
    HP_DF_CH, NULL, /*  84  */
    HP_DF_CH, NULL, /*  85  */
    HP_DF_CH, NULL, /*  86  */
    HP_DF_CH, NULL, /*  87  */
    HP_DF_CH, NULL, /*  88  */
    HP_DF_CH, NULL, /*  89  */
    HP_DF_CH, NULL, /*  8a  */
    HP_DF_CH, NULL, /*  8b  */
    HP_DF_CH, NULL, /*  8c  */
    HP_DF_CH, NULL, /*  8d  */
    HP_DF_CH, NULL, /*  8e  */
    HP_DF_CH, NULL, /*  8f  */
    HP_DF_CH, NULL, /*  90  */
    0x60, NULL,     /*  91  */
    0x27, NULL,     /*  92  */
    HP_DF_CH, NULL, /*  93  */
    HP_DF_CH, NULL, /*  94  */
    HP_DF_CH, NULL, /*  95  */
    HP_DF_CH, NULL, /*  96  */
    HP_DF_CH, NULL, /*  97  */
    HP_DF_CH, NULL, /*  98  */
    HP_DF_CH, NULL, /*  99  */
    HP_DF_CH, NULL, /*  9a  */
    HP_DF_CH, NULL, /*  9b  */
    HP_DF_CH, NULL, /*  9c  */
    HP_DF_CH, NULL, /*  9d  */
    HP_DF_CH, NULL, /*  9e  */
    HP_DF_CH, NULL, /*  9f  */
    0xa0, NULL,     /*  a0  */
    HP_DF_CH, NULL, /*  a1  */
    'c' , '|' ,     /*  a2  */
    HP_DF_CH, NULL, /*  a3  */
    HP_DF_CH, NULL, /*  a4  */
    '=' , 'Y' ,     /*  a5  */
    '|' , NULL,     /*  a6  */
    HP_DF_CH, NULL, /*  a7  */
    '"' , NULL,     /*  a8  */
    HP_DF_CH, NULL, /*  a9  */
    '_' , 'a' ,     /*  aa  */
    HP_DF_CH, NULL, /*  ab  */
    HP_DF_CH, NULL, /*  ac  */
    '-' , NULL,     /*  ad  */
    HP_DF_CH, NULL, /*  ae  */
    HP_DF_CH, NULL, /*  af  */
    HP_DF_CH, NULL, /*  b0  */
    '_' , '+' ,     /*  b1  */
    HP_DF_CH, NULL, /*  b2  */
    HP_DF_CH, NULL, /*  b3  */
    HP_DF_CH, NULL, /*  b4  */
    'u' , NULL,     /*  b5  */
    HP_DF_CH, NULL, /*  b6  */
    '*' , NULL,     /*  b7  */
    HP_DF_CH, NULL, /*  b8  */
    HP_DF_CH, NULL, /*  b9  */
    '_' , 'o' ,     /*  ba  */
    HP_DF_CH, NULL, /*  bb  */
    HP_DF_CH, NULL, /*  bc  */
    HP_DF_CH, NULL, /*  bd  */
    HP_DF_CH, NULL, /*  be  */
    HP_DF_CH, NULL, /*  bf  */
    'A' , NULL,     /*  c0  */
    'A' , NULL,     /*  c1  */
    'A' , NULL,     /*  c2  */
    'A' , NULL,     /*  c3  */
    'A' , NULL,     /*  c4  */
    'A' , NULL,     /*  c5  */
    'A' , NULL,     /*  c6  */
    'C' , ',' ,     /*  c7  */
    'E' , NULL,     /*  c8  */
    'E' , NULL,     /*  c9  */
    'E' , NULL,     /*  ca  */
    'E' , NULL,     /*  cb  */
    'I' , NULL,     /*  cc  */
    'I' , NULL,     /*  cd  */
    'I' , NULL,     /*  ce  */
    'I' , NULL,     /*  cf  */
    'D' , '-' ,     /*  d0  */
    'N' , NULL,     /*  d1  */
    'O' , NULL,     /*  d2  */
    'O' , NULL,     /*  d3  */
    'O' , NULL,     /*  d4  */
    'O' , NULL,     /*  d5  */
    'O' , NULL,     /*  d6  */
    'x', NULL,      /*  d7  multiply */
    'O' , '/' ,     /*  d8  */
    'U' , NULL,     /*  d9  */
    'U' , NULL,     /*  da  */
    'U' , NULL,     /*  db  */
    'U' , NULL,     /*  dc  */
    'Y' , NULL,     /*  dd  */
    'p' , 'b' ,     /*  de  */
    HP_DF_CH, NULL, /*  df  */
    'a' , '`' ,     /*  e0  */
    'a' , '\'',     /*  e1  */
    'a' , '^' ,     /*  e2  */
    'a' , NULL,     /*  e3 - cannot overstrike ~ */
    'a' , '"' ,     /*  e4  */
    'a' , NULL,     /*  e5  */
    'a' , NULL,     /*  e6  */
    'c' , ',' ,     /*  e7  */
    'e' , '`' ,     /*  e8  */
    'e' , '\'',     /*  e9  */
    'e' , '^' ,     /*  ea  */
    'e' , '"' ,     /*  eb  */
    '`' , 'i' ,     /*  ec  */
    '\'', 'i' ,     /*  ed  */
    '^' , 'i' ,     /*  ee  */
    '"' , 'i' ,     /*  ef  */
    'd' , '-' ,     /*  f0  */
    'n' , NULL,     /*  f1  */
    'o' , '`' ,     /*  f2  */
    'o' , '\'',     /*  f3  */
    'o' , '^' ,     /*  f4  */
    'o' , NULL,     /*  f5  */
    'o' , '"' ,     /*  f6  */
    '-', ':',       /*  f7  divide */
    'o' , '/' ,     /*  f8  */
    'u' , '`' ,     /*  f9  */
    'u' , '\'',     /*  fa  */
    'u' , '^' ,     /*  fb  */
    'u' , '"' ,     /*  fc  */
    'y' , '\'',     /*  fd  */
    'p' , 'b' ,     /*  fe  */
'y' , '"'   };      /*  ff  */

     /*    Added Roman8 translation table back to support Roman8
           Bit mapped cartridges for Europe       jcs    */

/* translation table from Extended Roman character set */

unsigned char Roman8_Trans[] = {
    HP_DF_CH, NULL, /*  80  */
    HP_DF_CH, NULL, /*  81  */
    HP_DF_CH, NULL, /*  82  */
    HP_DF_CH, NULL, /*  83  */
    HP_DF_CH, NULL, /*  84  */
    HP_DF_CH, NULL, /*  85  */
    HP_DF_CH, NULL, /*  86  */
    HP_DF_CH, NULL, /*  87  */
    HP_DF_CH, NULL, /*  88  */
    HP_DF_CH, NULL, /*  89  */
    HP_DF_CH, NULL, /*  8a  */
    HP_DF_CH, NULL, /*  8b  */
    HP_DF_CH, NULL, /*  8c  */
    HP_DF_CH, NULL, /*  8d  */
    HP_DF_CH, NULL, /*  8e  */
    HP_DF_CH, NULL, /*  8f  */
    HP_DF_CH, NULL, /*  90  */
    0x60, NULL,     /*  91  open single quote */
    0x27, NULL,     /*  92  close single quote */
    HP_DF_CH, NULL, /*  93  */
    HP_DF_CH, NULL, /*  94  */
    HP_DF_CH, NULL, /*  95  */
    HP_DF_CH, NULL, /*  96  */
    HP_DF_CH, NULL, /*  97  */
    HP_DF_CH, NULL, /*  98  */
    HP_DF_CH, NULL, /*  99  */
    HP_DF_CH, NULL, /*  9a  */
    HP_DF_CH, NULL, /*  9b  */
    HP_DF_CH, NULL, /*  9c  */
    HP_DF_CH, NULL, /*  9d  */
    HP_DF_CH, NULL, /*  9e  */
    HP_DF_CH, NULL, /*  9f  */
    0xa0, NULL,     /*  a0  */
    0xb8, NULL,     /*  a1  */
    0xbf, NULL,     /*  a2  */
    0xbb, NULL,     /*  a3  */
    0xba, NULL,     /*  a4  */
    0xbc, NULL,     /*  a5  */
    '|' , NULL,     /*  a6  */
    0xbd, NULL,     /*  a7  */
    0xab, NULL,     /*  a8  */
    'C' , NULL,     /*  a9  Copyright */
    0xf9, NULL,     /*  aa  */
    0xfb, NULL,     /*  ab  */
    '-', NULL,      /*  ac  logical not */
    '-' , NULL,     /*  ad  special dash */
    'R', NULL,      /*  ae  registered trademark */
    0xb0, NULL,     /*  af  */
    0xb3, NULL,     /*  b0  */
    0xfe, NULL,     /*  b1  */
    '2', NULL,      /*  b2  '2' superscript */
    '3', NULL,      /*  b3  '3' superscript */
    0xa8, NULL,     /*  b4  */
    0xF3, NULL,     /*  b5  changed to the real char 243 from u */
    0xF4, NULL,     /*  b6  paragraph -> section sign - NOT - changed back again to 244 */
    242, NULL,      /*  b7  raised dot (was FC block) */
    ',', NULL,      /*  b8  cedilla (deadkey) -> comma */
    '1', NULL,      /*  b9  1-super */
    0xfa, NULL,     /*  ba  */
    0xfd, NULL,     /*  bb  */
    0xf7, NULL,     /*  bc  */
    0xf8, NULL,     /*  bd  */
    245, NULL,      /*  be  3/4 */
    0xb9, NULL,     /*  bf  */
    0xa1, NULL,     /*  c0  */
    0xe0, NULL,     /*  c1  */
    0xa2, NULL,     /*  c2  */
    0xe1, NULL,     /*  c3  */
    0xd8, NULL,     /*  c4  */
    0xd0, NULL,     /*  c5  */
    0xd3, NULL,     /*  c6  */
    0xb4, NULL,     /*  c7  */
    0xa3, NULL,     /*  c8  */
    0xdc, NULL,     /*  c9  */
    0xa4, NULL,     /*  ca  */
    0xa5, NULL,     /*  cb  */
    0xe6, NULL,     /*  cc  */
    0xe5, NULL,     /*  cd  */
    0xa6, NULL,     /*  ce  */
    0xa7, NULL,     /*  cf  */
    0xe3, NULL,     /*  d0  */
    0xb6, NULL,     /*  d1  */
    0xe8, NULL,     /*  d2  */
    0xe7, NULL,     /*  d3  */
    0xdf, NULL,     /*  d4  */
    0xe9, NULL,     /*  d5  */
    0xda, NULL,     /*  d6  */
    'x', NULL,      /*  d7  multiply sign = 'x' */
    0xd2, NULL,     /*  d8  */
    0xad, NULL,     /*  d9  */
    0xed, NULL,     /*  da  */
    0xae, NULL,     /*  db  */
    0xdb, NULL,     /*  dc  */
    'Y' , 0xa8,     /*  dd  */
    0xf0, NULL,     /*  de  */
    0xde, NULL,     /*  df  */
    0xc8, NULL,     /*  e0  */
    0xc4, NULL,     /*  e1  */
    0xc0, NULL,     /*  e2  */
    0xe2, NULL,     /*  e3  */
    0xcc, NULL,     /*  e4  */
    0xd4, NULL,     /*  e5  */
    0xd7, NULL,     /*  e6  */
    0xb5, NULL,     /*  e7  */
    0xc9, NULL,     /*  e8  */
    0xc5, NULL,     /*  e9  */
    0xc1, NULL,     /*  ea  */
    0xcd, NULL,     /*  eb  */
    0xd9, NULL,     /*  ec  */
    0xd5, NULL,     /*  ed  */
    0xd1, NULL,     /*  ee  */
    0xdd, NULL,     /*  ef  */
    0xe4, NULL,     /*  f0  */
    0xb7, NULL,     /*  f1  */
    0xca, NULL,     /*  f2  */
    0xc6, NULL,     /*  f3  */
    0xc2, NULL,     /*  f4  */
    0xea, NULL,     /*  f5  */
    0xce, NULL,     /*  f6  */
    '-', ':',       /*  f7  divide sign */
    0xd6, NULL,     /*  f8  */
    0xcb, NULL,     /*  f9  */
    0xc7, NULL,     /*  fa  */
    0xc3, NULL,     /*  fb  */
    0xcf, NULL,     /*  fc  */
    'y',  0xa8,     /*  fd  */
    0xf1, NULL,     /*  fe  */
0xef, NULL };       /*  ff  */

  
/* generic 7-bit translation table */
unsigned char GENERIC7_Trans[] = {
    HP_DF_CH, NULL, /*  80  */
    HP_DF_CH, NULL, /*  81  */
    HP_DF_CH, NULL, /*  82  */
    HP_DF_CH, NULL, /*  83  */
    HP_DF_CH, NULL, /*  84  */
    HP_DF_CH, NULL, /*  85  */
    HP_DF_CH, NULL, /*  86  */
    HP_DF_CH, NULL, /*  87  */
    HP_DF_CH, NULL, /*  88  */
    HP_DF_CH, NULL, /*  89  */
    HP_DF_CH, NULL, /*  8a  */
    HP_DF_CH, NULL, /*  8b  */
    HP_DF_CH, NULL, /*  8c  */
    HP_DF_CH, NULL, /*  8d  */
    HP_DF_CH, NULL, /*  8e  */
    HP_DF_CH, NULL, /*  8f  */
    HP_DF_CH, NULL, /*  90  */
    HP_DF_CH, NULL, /*  91  */
    HP_DF_CH, NULL, /*  92  */
    HP_DF_CH, NULL, /*  93  */
    HP_DF_CH, NULL, /*  94  */
    HP_DF_CH, NULL, /*  95  */
    HP_DF_CH, NULL, /*  96  */
    HP_DF_CH, NULL, /*  97  */
    HP_DF_CH, NULL, /*  98  */
    HP_DF_CH, NULL, /*  99  */
    HP_DF_CH, NULL, /*  9a  */
    HP_DF_CH, NULL, /*  9b  */
    HP_DF_CH, NULL, /*  9c  */
    HP_DF_CH, NULL, /*  9d  */
    HP_DF_CH, NULL, /*  9e  */
    HP_DF_CH, NULL, /*  9f  */
    HP_DF_CH, NULL, /*  a0  */
    HP_DF_CH, NULL, /*  a1  */
    HP_DF_CH, NULL, /*  a2  */
    HP_DF_CH, NULL, /*  a3  */
    HP_DF_CH, NULL, /*  a4  */
    HP_DF_CH, NULL, /*  a5  */
    HP_DF_CH, NULL, /*  a6  */
    HP_DF_CH, NULL, /*  a7  */
    HP_DF_CH, NULL, /*  a8  */
    HP_DF_CH, NULL, /*  a9  */
    HP_DF_CH, NULL, /*  aa  */
    HP_DF_CH, NULL, /*  ab  */
    HP_DF_CH, NULL, /*  ac  */
    HP_DF_CH, NULL, /*  ad  */
    HP_DF_CH, NULL, /*  ae  */
    HP_DF_CH, NULL, /*  af  */
    HP_DF_CH, NULL, /*  b0  */
    HP_DF_CH, NULL, /*  b1  */
    HP_DF_CH, NULL, /*  b2  */
    HP_DF_CH, NULL, /*  b3  */
    HP_DF_CH, NULL, /*  b4  */
    HP_DF_CH, NULL, /*  b5  */
    HP_DF_CH, NULL, /*  b6  */
    HP_DF_CH, NULL, /*  b7  */
    HP_DF_CH, NULL, /*  b8  */
    HP_DF_CH, NULL, /*  b9  */
    HP_DF_CH, NULL, /*  ba  */
    HP_DF_CH, NULL, /*  bb  */
    HP_DF_CH, NULL, /*  bc  */
    HP_DF_CH, NULL, /*  bd  */
    HP_DF_CH, NULL, /*  be  */
    HP_DF_CH, NULL, /*  bf  */
    HP_DF_CH, NULL, /*  c0  */
    HP_DF_CH, NULL, /*  c1  */
    HP_DF_CH, NULL, /*  c2  */
    HP_DF_CH, NULL, /*  c3  */
    HP_DF_CH, NULL, /*  c4  */
    HP_DF_CH, NULL, /*  c5  */
    HP_DF_CH, NULL, /*  c6  */
    HP_DF_CH, NULL, /*  c7  */
    HP_DF_CH, NULL, /*  c8  */
    HP_DF_CH, NULL, /*  c9  */
    HP_DF_CH, NULL, /*  ca  */
    HP_DF_CH, NULL, /*  cb  */
    HP_DF_CH, NULL, /*  cc  */
    HP_DF_CH, NULL, /*  cd  */
    HP_DF_CH, NULL, /*  ce  */
    HP_DF_CH, NULL, /*  cf  */
    HP_DF_CH, NULL, /*  d0  */
    HP_DF_CH, NULL, /*  d1  */
    HP_DF_CH, NULL, /*  d2  */
    HP_DF_CH, NULL, /*  d3  */
    HP_DF_CH, NULL, /*  d4  */
    HP_DF_CH, NULL, /*  d5  */
    HP_DF_CH, NULL, /*  d6  */
    HP_DF_CH, NULL, /*  d7  */
    HP_DF_CH, NULL, /*  d8  */
    HP_DF_CH, NULL, /*  d9  */
    HP_DF_CH, NULL, /*  da  */
    HP_DF_CH, NULL, /*  db  */
    HP_DF_CH, NULL, /*  dc  */
    HP_DF_CH, NULL, /*  dd  */
    HP_DF_CH, NULL, /*  de  */
    HP_DF_CH, NULL, /*  df  */
    HP_DF_CH, NULL, /*  e0  */
    HP_DF_CH, NULL, /*  e1  */
    HP_DF_CH, NULL, /*  e2  */
    HP_DF_CH, NULL, /*  e3  */
    HP_DF_CH, NULL, /*  e4  */
    HP_DF_CH, NULL, /*  e5  */
    HP_DF_CH, NULL, /*  e6  */
    HP_DF_CH, NULL, /*  e7  */
    HP_DF_CH, NULL, /*  e8  */
    HP_DF_CH, NULL, /*  e9  */
    HP_DF_CH, NULL, /*  ea  */
    HP_DF_CH, NULL, /*  eb  */
    HP_DF_CH, NULL, /*  ec  */
    HP_DF_CH, NULL, /*  ed  */
    HP_DF_CH, NULL, /*  ee  */
    HP_DF_CH, NULL, /*  ef  */
    HP_DF_CH, NULL, /*  f0  */
    HP_DF_CH, NULL, /*  f1  */
    HP_DF_CH, NULL, /*  f2  */
    HP_DF_CH, NULL, /*  f3  */
    HP_DF_CH, NULL, /*  f4  */
    HP_DF_CH, NULL, /*  f5  */
    HP_DF_CH, NULL, /*  f6  */
    HP_DF_CH, NULL, /*  f7  */
    HP_DF_CH, NULL, /*  f8  */
    HP_DF_CH, NULL, /*  f9  */
    HP_DF_CH, NULL, /*  fa  */
    HP_DF_CH, NULL, /*  fb  */
    HP_DF_CH, NULL, /*  fc  */
    HP_DF_CH, NULL, /*  fd  */
    HP_DF_CH, NULL, /*  fe  */
HP_DF_CH, NULL};    /*  ff  */
  
/* ECMA-94 translation table */
unsigned char ECMA94_Trans[] = {
    HP_DF_CH, NULL, /*  80  */
    HP_DF_CH, NULL, /*  81  */
    HP_DF_CH, NULL, /*  82  */
    HP_DF_CH, NULL, /*  83  */
    HP_DF_CH, NULL, /*  84  */
    HP_DF_CH, NULL, /*  85  */
    HP_DF_CH, NULL, /*  86  */
    HP_DF_CH, NULL, /*  87  */
    HP_DF_CH, NULL, /*  88  */
    HP_DF_CH, NULL, /*  89  */
    HP_DF_CH, NULL, /*  8a  */
    HP_DF_CH, NULL, /*  8b  */
    HP_DF_CH, NULL, /*  8c  */
    HP_DF_CH, NULL, /*  8d  */
    HP_DF_CH, NULL, /*  8e  */
    HP_DF_CH, NULL, /*  8f  */
    HP_DF_CH, NULL, /*  90  */
    0x60, NULL,     /*  91  */
    0x27, NULL,     /*  92  */
    HP_DF_CH, NULL, /*  93  */
    HP_DF_CH, NULL, /*  94  */
    HP_DF_CH, NULL, /*  95  */
    HP_DF_CH, NULL, /*  96  */
    HP_DF_CH, NULL, /*  97  */
    HP_DF_CH, NULL, /*  98  */
    HP_DF_CH, NULL, /*  99  */
    HP_DF_CH, NULL, /*  9a  */
    HP_DF_CH, NULL, /*  9b  */
    HP_DF_CH, NULL, /*  9c  */
    HP_DF_CH, NULL, /*  9d  */
    HP_DF_CH, NULL, /*  9e  */
    HP_DF_CH, NULL, /*  9f  */
    0xa0, NULL,     /*  a0  */
    0xa1, NULL,     /*  a1  */
    0xa2, NULL,     /*  a2  */
    0xa3, NULL,     /*  a3  */
    0xa4, NULL,     /*  a4  */
    0xa5, NULL,     /*  a5  */
    0xa6, NULL,     /*  a6  */
    0xa7, NULL,     /*  a7  */
    0xa8, NULL,     /*  a8  */
    0xa9, NULL,     /*  a9  */
    0xaa, NULL,     /*  aa  */
    0xab, NULL,     /*  ab  */
    0xac, NULL,     /*  ac  */
    0xad, NULL,     /*  ad  */
    0xae, NULL,     /*  ae  */
    0xaf, NULL,     /*  af  */
    0xb0, NULL,     /*  b0  */
    0xb1, NULL,     /*  b1  */
    0xb2, NULL,     /*  b2  */
    0xb3, NULL,     /*  b3  */
    0xb4, NULL,     /*  b4  */
    0xb5, NULL,     /*  b5  */
    0xb6, NULL,     /*  b6  */
    0xb7, NULL,     /*  b7  */
    0xb8, NULL,     /*  b8  */
    0xb9, NULL,     /*  b9  */
    0xba, NULL,     /*  ba  */
    0xbb, NULL,     /*  bb  */
    0xbc, NULL,     /*  bc  */
    0xbd, NULL,     /*  bd  */
    0xbe, NULL,     /*  be  */
    0xbf, NULL,     /*  bf  */
    0xc0, NULL,     /*  c0  */
    0xc1, NULL,     /*  c1  */
    0xc2, NULL,     /*  c2  */
    0xc3, NULL,     /*  c3  */
    0xc4, NULL,     /*  c4  */
    0xc5, NULL,     /*  c5  */
    0xc6, NULL,     /*  c6  */
    0xc7, NULL,     /*  c7  */
    0xc8, NULL,     /*  c8  */
    0xc9, NULL,     /*  c9  */
    0xca, NULL,     /*  ca  */
    0xcb, NULL,     /*  cb  */
    0xcc, NULL,     /*  cc  */
    0xcd, NULL,     /*  cd  */
    0xce, NULL,     /*  ce  */
    0xcf, NULL,     /*  cf  */
    0xd0, NULL,     /*  d0  */
    0xd1, NULL,     /*  d1  */
    0xd2, NULL,     /*  d2  */
    0xd3, NULL,     /*  d3  */
    0xd4, NULL,     /*  d4  */
    0xd5, NULL,     /*  d5  */
    0xd6, NULL,     /*  d6  */
    0xd7, NULL,     /*  d7  */
    0xd8, NULL,     /*  d8  */
    0xd9, NULL,     /*  d9  */
    0xda, NULL,     /*  da  */
    0xdb, NULL,     /*  db  */
    0xdc, NULL,     /*  dc  */
    0xdd, NULL,     /*  dd  */
    0xde, NULL,     /*  de  */
    0xdf, NULL,     /*  df  */
    0xe0, NULL,     /*  e0  */
    0xe1, NULL,     /*  e1  */
    0xe2, NULL,     /*  e2  */
    0xe3, NULL,     /*  e3  */
    0xe4, NULL,     /*  e4  */
    0xe5, NULL,     /*  e5  */
    0xe6, NULL,     /*  e6  */
    0xe7, NULL,     /*  e7  */
    0xe8, NULL,     /*  e8  */
    0xe9, NULL,     /*  e9  */
    0xea, NULL,     /*  ea  */
    0xeb, NULL,     /*  eb  */
    0xec, NULL,     /*  ec  */
    0xed, NULL,     /*  ed  */
    0xee, NULL,     /*  ee  */
    0xef, NULL,     /*  ef  */
    0xf0, NULL,     /*  f0  */
    0xf1, NULL,     /*  f1  */
    0xf2, NULL,     /*  f2  */
    0xf3, NULL,     /*  f3  */
    0xf4, NULL,     /*  f4  */
    0xf5, NULL,     /*  f5  */
    0xf6, NULL,     /*  f6  */
    0xf7, NULL,     /*  f7  */
    0xf8, NULL,     /*  f8  */
    0xf9, NULL,     /*  f9  */
    0xfa, NULL,     /*  fa  */
    0xfb, NULL,     /*  fb  */
    0xfc, NULL,     /*  fc  */
    0xfd, NULL,     /*  fd  */
    0xfe, NULL,     /*  fe  */
0xff, NULL};        /*  ff  */
