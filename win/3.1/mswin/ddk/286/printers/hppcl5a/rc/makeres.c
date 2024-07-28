/**[f******************************************************************
 * makeres.c -
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Copyright (C) 1989-1990 Microsoft Corporation.
 * Copyright (C) 1990, 1991 Hewlett-Packard Company.
 *  All rights reserved.
 * Company confidential.
 *
 **f]*****************************************************************/

/*
 * $Header: makeres.c,v 3.890 92/02/06 16:13:46 dtk FREEZE $
 */

/*
 * $Log:	makeres.c,v $
 * Revision 3.890  92/02/06  16:13:46  16:13:46  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.870  91/11/08  11:45:37  11:45:37  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:53:36  13:53:36  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:48:54  13:48:54  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:50:32  09:50:32  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  15:01:21  15:01:21  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:51:38  16:51:38  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:18:51  14:18:51  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:35:12  16:35:12  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:36:32  10:36:32  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:13:53  14:13:53  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.812  91/08/22  14:33:45  14:33:45  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.808  91/08/21  14:22:36  14:22:36  jsmart (Jerry Smart)
 * added support for roman8 remapping
 * 
 * Revision 3.807  91/08/08  10:33:01  10:33:01  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.802  91/07/22  12:56:12  12:56:12  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.799  91/07/02  11:53:33  11:53:33  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.796  91/06/26  11:27:55  11:27:55  stevec (Steve Claiborne)
 * BETA
 * 
 * Revision 3.790  91/06/11  16:05:06  16:05:06  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.785  91/05/22  14:58:41  14:58:41  stevec (Steve Claiborne)
 * Beta version to MS
 * 
 * Revision 3.780  91/05/15  15:58:50  15:58:50  stevec (Steve Claiborne)
 * Beta
 * 
 * Revision 3.775  91/04/05  14:32:43  14:32:43  stevec (Steve Claiborne)
 * Beta release to MS
 * 
 * Revision 3.770  91/03/25  15:37:42  15:37:42  stevec (Steve Claiborne)
 * maintance release
 * 
 * Revision 3.760  91/03/12  07:54:31  07:54:31  stevec (Steve Claiborne)
 * Maintance release
 * 
 * Revision 3.755  91/03/03  07:47:46  07:47:46  stevec (Steve Claiborne)
 * March 3 Freeze
 * 
 * Revision 3.720  91/02/11  09:17:05  09:17:05  stevec (Steve Claiborne)
 * Aldus version
 * 
 * Revision 3.710  91/02/04  15:49:17  15:49:17  stevec (Steve Claiborne)
 * Aldus freeze
 * 
 * Revision 3.700  91/01/19  09:01:59  09:01:59  stevec (Steve Claiborne)
 * Release
 * 
 * Revision 3.685  91/01/14  15:44:56  15:44:56  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.681  91/01/14  10:19:10  10:19:10  stevec (Steve Claiborne)
 * Updated the copy right stmt.
 * 
 * Revision 3.680  91/01/10  16:18:17  16:18:17  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.670  90/12/14  14:55:50  14:55:50  stevec (Steve Claiborne)
 * freeze for 12-14-90 ver. 3.670
 * 
 * Revision 3.665  90/12/10  15:37:28  15:37:28  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.660  90/12/07  14:52:00  14:52:00  stevec (Steve Claiborne)
 * Freeze 12-7-90
 * 
 * Revision 3.650  90/11/30  08:13:48  08:13:48  stevec (Steve Claiborne)
 * Freeze 3.650, 11-30-90
 * 
 * Revision 3.601  90/08/24  11:51:25  11:51:25  daniels (Susan Daniels)
 * ../message.txt
 * 
 * Revision 3.600  90/08/03  11:11:34  11:11:34  stevec (Steve Claiborne)
 * This is the Aug. 3 release ver. 3.600
 * 
 * Revision 3.550  90/07/27  11:32:53  11:32:53  root ()
 * Experimental freeze 3.55
 * 
 * Revision 3.541  90/07/27  08:18:08  08:18:08  oakeson ()
 * Nuked Generic8 translation table
 * 
 * Revision 3.540  90/07/25  12:37:30  12:37:30  stevec (Steve Claiborne)
 * Experimental freeze 3.54
 * 
 * Revision 3.521  90/07/19  12:35:54  12:35:54  oakeson ()
 * Removed Roman8 and Math8 table generators
 * 
 */

#include "printer.h"
#include "gdidefs.inc"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "hppcl.h"
#include "pfm.h"
#include "trans.h"
#define PAPER_DATA
#include "paperfmt.h"


main()
    {
    FILE *fp;
    char fname[32];
    BYTE *fdata;
    int fsize;
    int loop;

    for (loop = 0; loop < 4; ++loop)
	{
	switch (loop)
	    {
	    case 0:
		strcpy(fname, "USASCII.tbl");
		fdata = USASCII_Trans;
		fsize = sizeof(USASCII_Trans);
		break;
	    case 1:
		strcpy(fname, "GENERIC7.tbl");
		fdata = GENERIC7_Trans;
		fsize = sizeof(GENERIC7_Trans);
		break;
	    case 2:
		strcpy(fname, "ECMA94.tbl");
		fdata = ECMA94_Trans;
		fsize = sizeof(ECMA94_Trans);
		break;
		          /*    Added Roman8 to support the old bit mapped
		                cartridges for Europe     jcs   */
	    case 3:
		strcpy(fname, "Roman8.tbl");
		fdata = Roman8_Trans;
		fsize = sizeof(Roman8_Trans);
	    }

	if (fp = fopen(fname, "wb"))
	    {
	    fprintf(stderr, "MAKERES: building %s\n", fname);
	    if (fwrite(fdata, 1, fsize, fp) != fsize)
		{
		fprintf(stderr, "MAKERES: ***failed to write %s\n", fname);
		exit(1);
		}
	    fclose(fp);
	    }
	else
	    {
	    fprintf(stderr, "MAKERES: ***failed to open %s\n", fname);
	    exit(1);
	    }
	}

    if (fp = fopen("paperfmt.bin", "wb"))
	{
	PAPERHEAD PaperHead;

	fprintf(stderr, "MAKERES: building paperfmt.bin\n");

	PaperHead.numLists = sizeof(PaperList) / sizeof(PAPERLIST);
	PaperHead.numFormats = sizeof(PaperFormat) / sizeof(PAPERFORMAT);
	PaperHead.offsLists = sizeof(PaperHead);
	PaperHead.offsFormats = PaperHead.offsLists + sizeof(PaperList);

	if (fwrite(&PaperHead,1,sizeof(PaperHead),fp) != sizeof(PaperHead) ||
	    fwrite(PaperList,1,sizeof(PaperList),fp) != sizeof(PaperList) ||
	    fwrite(PaperFormat,1,sizeof(PaperFormat),fp) != sizeof(PaperFormat))
	    {
	    fprintf(stderr, "MAKERES: ***failed to write paperfmt.bin\n");
	    exit(1);
	    }
	fclose(fp);
	}
    else
	{
	fprintf(stderr, "MAKERES: ***failed to open %s\n", fname);
	exit(1);
	}

    return (0);
    }

