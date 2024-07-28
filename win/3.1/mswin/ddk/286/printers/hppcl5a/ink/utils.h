/**[f******************************************************************
* utils.h -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/
  
/*
* $Header: utils.h,v 3.890 92/02/06 16:13:26 dtk FREEZE $
*/
  
/*
* $Log:	utils.h,v $
 * Revision 3.890  92/02/06  16:13:26  16:13:26  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.870  91/11/08  11:45:14  11:45:14  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:53:13  13:53:13  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:48:32  13:48:32  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:50:07  09:50:07  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  15:01:00  15:01:00  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:51:16  16:51:16  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:18:29  14:18:29  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:34:49  16:34:49  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:36:00  10:36:00  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:13:30  14:13:30  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.812  91/08/22  14:33:22  14:33:22  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.807  91/08/08  10:32:38  10:32:38  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.802  91/07/22  12:55:47  12:55:47  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.799  91/07/02  11:53:09  11:53:09  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.796  91/06/26  11:27:30  11:27:30  stevec (Steve Claiborne)
 * BETA
 * 
 * Revision 3.790  91/06/11  16:04:43  16:04:43  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.786  91/06/11  15:46:02  15:46:02  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.785  91/05/22  14:58:18  14:58:18  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.780  91/05/15  15:58:28  15:58:28  stevec (Steve Claiborne)
* Beta
*
* Revision 3.775  91/04/05  14:32:20  14:32:20  stevec (Steve Claiborne)
* Beta release to MS
*
* Revision 3.770  91/03/25  15:37:19  15:37:19  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.760  91/03/12  07:54:08  07:54:08  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/03  07:47:24  07:47:24  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.720  91/02/11  09:16:43  09:16:43  stevec (Steve Claiborne)
* Aldus version
*
* Revision 3.710  91/02/04  15:48:56  15:48:56  stevec (Steve Claiborne)
* Aldus freeze
*
* Revision 3.700  91/01/19  09:01:37  09:01:37  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:44:36  15:44:36  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.681  91/01/14  10:18:48  10:18:48  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.680  91/01/10  16:17:55  16:17:55  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.671  90/12/19  13:13:34  13:13:34  oakeson (Ken Oakeson)
* Added masterunits (short) parameter to MakeSizeEscape
*
* Revision 3.670  90/12/14  14:55:28  14:55:28  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:37:06  15:37:06  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.660  90/12/07  14:51:37  14:51:37  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.650  90/11/30  08:13:26  08:13:26  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 3.601  90/08/24  13:24:36  13:24:36  daniels (Susan Daniels)
* ../message.txt
*
* Revision 3.600  90/08/03  11:11:11  11:11:11  stevec (Steve Claiborne)
* This is the Aug. 3 release ver. 3.600
*
* Revision 3.550  90/07/27  11:32:27  11:32:27  root ()
* Experimental freeze 3.55
*
* Revision 3.540  90/07/25  12:36:54  12:36:54  stevec (Steve Claiborne)
* Experimental freeze 3.54
*
* Revision 3.520  90/06/13  16:54:15  16:54:15  root ()
* 5_2_release
*
*
*    Rev 1.1   20 Feb 1990 15:48:00   vordaz
* Support for downloadables.
*/
  
extern long FAR PASCAL labdivc(long, long, long);
extern long FAR PASCAL lmul(long, long);
extern long FAR PASCAL ldiv(long, long);
extern long FAR PASCAL FontMEM(int, long, long);
extern int FAR PASCAL itoa(int, LPSTR);
extern int FAR PASCAL atoi(LPSTR);
extern int FAR PASCAL _lopenp(LPSTR, WORD);
extern void FAR PASCAL MakeAppName(LPSTR, LPSTR, LPSTR, int);
  
/*** Tetra begin ***/
extern int FAR PASCAL lstrind(LPSTR, int);
extern int FAR PASCAL hplstrcpyn(LPSTR, LPSTR, int);
extern int FAR PASCAL MakeEscSize(long, LPSTR);
extern long FAR PASCAL CalcPtSize(long, long);
extern short FAR PASCAL ScaleWidth(long, long, long, long);
/*** Tetra II begin ***/
extern int FAR PASCAL MakeSizeEscape(lpESC, BYTE, short, short, short, short);
extern short FAR PASCAL ScaleVertical(short, short, short);
/*** Tetra II end ***/
/*** Tetra end ***/
  
#ifndef NO_OUTUTIL
extern int FAR PASCAL myWriteSpool(LPDEVICE);
extern int FAR PASCAL myWrite(LPDEVICE, LPSTR, short);
extern int FAR PASCAL MakeEscape(lpESC, char, char, char, short);
extern int FAR PASCAL xmoveto(LPDEVICE, WORD);
extern int FAR PASCAL ymoveto(LPDEVICE, WORD);
#endif
