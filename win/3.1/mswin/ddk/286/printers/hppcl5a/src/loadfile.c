/**[f******************************************************************
* loadfile.c -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/
  
// history
// 12 feb 90   VO              Added provision for seeking to a scalable
//                             width table.
// 07 aug 89    peterbe     after comment "Copy struct from .pfm file."
//              change code to "sizeofStruct = 0;" (around line
//              141).
// 27 apr 89    peterbe     Tabs at 8 spaces, other format cleanup.
  
/*
* $Header: loadfile.c,v 3.890 92/02/06 16:11:59 dtk FREEZE $
*/
  
/*
* $Log:	loadfile.c,v $
 * Revision 3.890  92/02/06  16:11:59  16:11:59  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.870  91/11/08  11:43:44  11:43:44  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:51:43  13:51:43  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:47:03  13:47:03  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:48:29  09:48:29  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  14:59:31  14:59:31  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:49:43  16:49:43  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:17:02  14:17:02  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:33:17  16:33:17  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:33:47  10:33:47  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:11:57  14:11:57  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.812  91/08/22  14:31:53  14:31:53  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.807  91/08/08  10:31:10  10:31:10  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.802  91/07/22  12:54:06  12:54:06  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.799  91/07/02  11:51:33  11:51:33  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.796  91/06/26  11:25:52  11:25:52  stevec (Steve Claiborne)
 * BETA
 * 
 * Revision 3.790  91/06/11  16:03:08  16:03:08  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.786  91/06/11  15:43:59  15:43:59  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.785  91/05/22  14:56:47  14:56:47  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.780  91/05/15  15:56:56  15:56:56  stevec (Steve Claiborne)
* Beta
*
* Revision 3.775  91/04/05  14:30:49  14:30:49  stevec (Steve Claiborne)
* Beta release to MS
*
* Revision 3.770  91/03/25  15:35:49  15:35:49  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.760  91/03/12  07:52:32  07:52:32  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/03  07:45:54  07:45:54  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.720  91/02/11  09:15:12  09:15:12  stevec (Steve Claiborne)
* Aldus version
*
* Revision 3.711  91/02/08  16:25:52  16:25:52  stevec (Steve Claiborne)
* Added debuging
*
* Revision 3.710  91/02/04  15:47:30  15:47:30  stevec (Steve Claiborne)
* Aldus freeze
*
* Revision 3.700  91/01/19  09:00:12  09:00:12  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:43:06  15:43:06  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.681  91/01/14  10:17:23  10:17:23  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.680  91/01/10  16:16:29  16:16:29  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.670  90/12/14  14:53:55  14:53:55  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:35:38  15:35:38  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.660  90/12/07  14:50:08  14:50:08  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.650  90/11/30  08:12:02  08:12:02  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 3.601  90/08/24  11:37:40  11:37:40  daniels (Susan Daniels)
* message.txt
*
* Revision 3.600  90/08/03  11:09:34  11:09:34  stevec (Steve Claiborne)
* This is the Aug. 3 release ver. 3.600
*
* Revision 3.550  90/07/27  11:30:42  11:30:42  root ()
* Experimental freeze 3.55
*
* Revision 3.540  90/07/25  12:34:13  12:34:13  stevec (Steve Claiborne)
* Experimental freeze 3.54
*
* Revision 3.520  90/06/13  16:52:15  16:52:15  root ()
* 5_2_release
*
*
*    Rev 1.1   20 Feb 1990 15:32:34   vordaz
* Support for downloadables.
*/
  
#define DBGloadfile(msg) DBMSG(msg)
  
/*  loadStructFromFile
*/
static int loadStructFromFile(LPFONTSUMMARYHDR, LPFONTSUMMARY, LPSTR, WORD);
static int loadStructFromFile(lpFontSummary, lpSummary, lpDest, kind)
LPFONTSUMMARYHDR lpFontSummary;
LPFONTSUMMARY lpSummary;
LPSTR lpDest;
WORD kind;
{
    int sizeofStruct = 0;
    long seek = 0L;
    char buf[128];
    int hFile;
#ifdef DEBUG_FUNCT
    DB(("Entering loadStructFromFile\n"));
#endif
  
    DBGloadfile(("loadStructFromFile(%lp,%lp,%lp,%d)\n",
    lpFontSummary, lpSummary, lpDest, (WORD)kind));
  
    /*  Open .pfm file.
    */
    if (MakeFontSumFNm(lpFontSummary,lpSummary,buf,sizeof(buf),TRUE) &&
        ((hFile = _lopenp(buf,OF_READ)) > 0))
    {
        DBGloadfile(("Reading PFM source %ls\n",(LPSTR)buf));
        /*  Initially pick up location and size of width table.
        */
        seek = sizeof(PFMHEADER) - 2;
        sizeofStruct = (lpSummary->dfLastChar -lpSummary->dfFirstChar +2) *2;
  
        /*** Tetra II begin ***/
        /*** If getting a scalable width table, then set seek to dfExtentTable ***/
        if ((lpSummary->scaleInfo.scalable) && (kind == FNTLD_WIDTHS))
        {
            if (lpSummary->dfPitchAndFamily & 0x1)
                seek += sizeofStruct;
            _llseek(hFile, seek + lpSummary->lPCMOffset, 0);
            if (_lread(hFile, buf, sizeof(PFMEXTENSION)) == sizeof(PFMEXTENSION))
            {
                seek = (long) (((LPPFMEXTENSION)buf)->dfExtentTable);
            }
            else
            {
                DBGloadfile(("loadStructFromFile(): read pfmExtension *failed*\n"));
                seek = 0L;
                sizeofStruct = 0;
            }
        }
        /*** Tetra II end ***/
  
        if (kind != FNTLD_WIDTHS)
        {
            /*  Move file pointer to after width table in file
            *  (for fixed pitch fonts the table does not exist).
            */
            if (lpSummary->dfPitchAndFamily & 0x1)
                seek += sizeofStruct;
            sizeofStruct = 0;                   /* reset to zero */
            DBGloadfile(("_llseek(hFile,%ld (%ld+%ld),0)\n",
            seek+lpSummary->lPCMOffset,seek,lpSummary->lPCMOffset));
            _llseek(hFile, seek+lpSummary->lPCMOffset, 0);
  
            /*  Read pfmExtension.
            */
            if (_lread(hFile, buf, sizeof(PFMEXTENSION)) ==
                sizeof(PFMEXTENSION))
            {
                /*  Locate extended text metrics structure.
                */
                long tseek = ((LPPFMEXTENSION)buf)->dfExtMetricsOffset;
  
                /*  Pick up the location of the struct.
                */
                switch (kind)
                {
                    case FNTLD_EXTMETRICS:
                        seek = tseek;
                        sizeofStruct = sizeof(EXTTEXTMETRIC);
                        break;
                    case FNTLD_PAIRKERN:
                        seek = ((LPPFMEXTENSION)buf)->dfPairKernTable;
                        break;
                    case FNTLD_TRACKKERN:
                        seek = ((LPPFMEXTENSION)buf)->dfTrackKernTable;
                        break;
                    default:
                        seek = 0L;
                        sizeofStruct = 0;
                        break;
                }
  
                /*  Pick up the size of the struct.
                */
                if ((kind != FNTLD_EXTMETRICS) && seek && tseek)
                {
                    DBGloadfile(("_llseek(hFile,%ld (%ld+%ld),0)\n",
                    tseek+lpSummary->lPCMOffset,tseek,lpSummary->lPCMOffset));
                    _llseek(hFile, tseek+lpSummary->lPCMOffset, 0);
                    if (_lread(hFile, buf, sizeof(EXTTEXTMETRIC)) ==
                        sizeof(EXTTEXTMETRIC))
                    {
                        switch(kind)
                        {
                            case FNTLD_PAIRKERN:
                                sizeofStruct = sizeof(KERNPAIR) *
                                ((LPEXTTEXTMETRIC)buf)->emKernPairs;
                                break;
                            case FNTLD_TRACKKERN:
                                sizeofStruct = sizeof(KERNTRACK) *
                                ((LPEXTTEXTMETRIC)buf)->emKernTracks;
                                break;
                            default:
                                seek = 0L;
                                sizeofStruct = 0;
                                break;
                        }
                    }
                    else
                    {
                        DBGloadfile(
                        ("loadStructFromFile(): read extTextMetric *failed*\n"));
                        seek = 0L;
                        sizeofStruct = 0;
                    }
                }
            }
            else
            {
                DBGloadfile(
                ("loadStructFromFile(): read pfmExtenstion *failed*\n"));
                seek = 0L;
                sizeofStruct = 0;
            }
        }
  
        if ((seek > 0L) && (sizeofStruct > 0))
        {
            DBGloadfile(("_llseek(hFile,%ld (%ld+%ld),0)\n",
            seek+lpSummary->lPCMOffset,seek,lpSummary->lPCMOffset));
            _llseek(hFile, seek+lpSummary->lPCMOffset, 0);
  
            /*  Copy struct from .pfm file.
            */
            if (_lread(hFile, lpDest, sizeofStruct) != sizeofStruct)
            {
                sizeofStruct = 0;
            #ifdef DEBUG
                DBGloadfile(
                ("loadStructFromFile(): did *not* successfully read struct\n"));
            #endif
            }
        }
  
        /*  Close file.
        */
        _lclose (hFile);
    }
    #ifdef DEBUG
    else
    {
        DBGloadfile(("loadStructFromFile(): could *not* open .pfm file\n"));
    }
    #endif
  
#ifdef DEBUG_FUNCT
    DB(("Exiting loadStructFromFile\n"));
#endif
    return (kind == FNTLD_PAIRKERN ?
    (sizeofStruct / sizeof(KERNPAIR)) : sizeofStruct);
}
