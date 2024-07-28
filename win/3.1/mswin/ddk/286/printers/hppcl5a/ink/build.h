/*
 * $Header: build.h,v
 */

/**[f******************************************************************
 * build.h - 
 *
 * Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
 * All rights reserved.  Company confidential.
 *
 **f]*****************************************************************/
  
/*
 * $Log:	build.h,v $
 * Revision 3.890  92/02/06  16:13:43  16:13:43  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.872  91/12/03  09:29:35  09:29:35  dtk (Doug Kaltenecker)
 * *** empty log message ***
 * 
 * Revision 3.871  91/12/02  16:42:33  16:42:33  dtk (Doug Kaltenecker)
 * Took out the TT and USE_GRC build directives.
 * 
 * Revision 3.871  91/11/22  13:21:09  13:21:09  dtk (Doug Kaltenecker)
 * Win 3.1 Post Beta 3 version.
 * 
 * Revision 3.870  91/11/11  13:29:11  13:29:11  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:53:32  13:53:32  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:48:51  13:48:51  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:50:28  09:50:28  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  15:01:19  15:01:19  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:51:35  16:51:35  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:18:48  14:18:48  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:35:09  16:35:09  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:36:28  10:36:28  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:13:50  14:13:50  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.814  91/09/04  15:01:55  15:01:55  dtk (Doug Kaltenecker)
 * Added the header and log statements.
 * 
 */

/*      Created 6/20/91
 *
 *      Defines variables to turn on compiler options to build a driver
 *      for Windows 3.1.
 *      
 *      If the variable WIN31 is defined, a Windows 3.1 driver will be
 *      built, otherwise, a Windows 3.0 driver will be built.
 *
 *      This file needs to be included in:  hppcl.rc, devmode.c, environ.c,
 *      options.c, and many others...
 *      
 *      dtk -   added the TT define and included this file in physical.c
 *              truetype.c, fontutil.c, realize.c and truetype.h. This 
 *              fixes the RIPs in the debug version of Windows 3.0. 8/91
 *      
 *      dtk -   changed the TT and USE_GRC defines to WIN31 defines to 
 *              clean up the source code. 12/91
 */



////////////////////////////

#define WIN31

////////////////////////////

