/**[f******************************************************************
* makefsnm.c -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.
* Company confidential.
*
**f]*****************************************************************/
  
// history
// 27 apr 89    peterbe     Tabs are 8 spaces, other format cleanup.
  
#define DBGmakefn(msg) DBMSG(msg)
  
/*  MakeFontSumFNm
*
*  Merge the path and file name from the string table at the end of
*  the fontSummary struct.
*/
static BOOL MakeFontSumFNm (LPFONTSUMMARYHDR, LPFONTSUMMARY, LPSTR, int, BOOL);
static BOOL MakeFontSumFNm (lpFontSummary, lpSummary, lpFileName, sizefn, forPFM)
LPFONTSUMMARYHDR lpFontSummary;
LPFONTSUMMARY lpSummary;
LPSTR lpFileName;
int sizefn;
BOOL forPFM;
{
    LPSTR lpName;
    short indPath, indName;
#ifdef DEBUG_FUNCT
    DB(("Entering MakeFontSumFNm\n"));
#endif
  
    if (forPFM)
    {
        DBGmakefn(("MakeFontSumFNm(): forPFM=TRUE\n"));
        indPath = lpSummary->indPFMPath;
        indName = lpSummary->indPFMName;
    }
    else
    {
        DBGmakefn(("MakeFontSumFNm(): forPFM=FALSE\n"));
        indPath = lpSummary->indDLPath;
        indName = lpSummary->indDLName;
    }
  
    DBGmakefn(("                  indPath=%d, indName=%d\n",
    indPath, indName));
  
    /*  Locate string table at end of fontSummary struct.
    */
    lpName = (LPSTR) &lpFontSummary->f[lpFontSummary->len];
  
    #ifdef DEBUG
    if (indPath > 0) {
        LPSTR abckkk = (LPSTR) &lpName[indPath];
        DBGmakefn(("                  path=%ls\n", abckkk));
    } else {
        DBGmakefn(("                  path *undefined*\n"));
    }
    if (indName > 0) {
        LPSTR abckkk = (LPSTR) &lpName[indName];
        DBGmakefn(("                  name=%ls\n", abckkk));
    } else {
        DBGmakefn(("                  name *undefined*\n"));
    }
    #endif
  
    /*  Locate file name inside of stringtable.
    */
    lpFileName[0] = '\0';
  
    if ((indPath > 0) && (lstrlen(&lpName[indPath]) < sizefn))
    {
        lstrcpy(lpFileName, &lpName[indPath]);
    }
  
    if ((indName > 0) &&
        (lstrlen(&lpName[indName]) + lstrlen(lpFileName) < sizefn))
    {
        lstrcat(lpFileName, &lpName[indName]);
    }
    else
        lpFileName[0] = '\0';
  
#ifdef DEBUG_FUNCT
    DB(("Exiting MakeFontSumFNm\n"));
#endif
    return (lstrlen(lpFileName));
}
  
  
  
