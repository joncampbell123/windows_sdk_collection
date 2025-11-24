/*****************************************************************/ 
/**               Microsoft Windows 95                          **/
/**           Copyright (C) Microsoft Corp., 1991-1995          **/
/*****************************************************************/ 

#include "mspp.h"

static char szFmt0[] = "File %.40s, Line %u";
static char szFmt1[] = "%.60s: File %.40s, Line %u";
static char szMBCaption[] = "ASSERTION FAILED";
static char szFAE[] = "ASSERTION FAILURE IN APP";

void UIAssertHelper(
    const char* pszFileName,
    UINT    nLine )
{
    char szBuff[sizeof(szFmt0)+60+40];

    wsprintf(szBuff, szFmt0, pszFileName, nLine);
    MessageBox(NULL, szBuff, szMBCaption,
           (MB_TASKMODAL | MB_ICONSTOP | MB_OK) );

    FatalAppExit(0, szFAE);
}

void UIAssertSzHelper(
    const char* pszMessage,
    const char* pszFileName,
    UINT    nLine )
{
    char szBuff[sizeof(szFmt1)+60+40];

    wsprintf(szBuff, szFmt1, pszMessage, pszFileName, nLine);
    MessageBox(NULL, szBuff, szMBCaption,
           (MB_TASKMODAL | MB_ICONSTOP | MB_OK) );

    FatalAppExit(0, szFAE);
}

