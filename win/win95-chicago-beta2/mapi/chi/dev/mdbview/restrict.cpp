/*******************************************************************/
/*
 -  restrict.cpp
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Contains member functions for CRestrictionDlg
 */
/*******************************************************************/
                                                   
#undef  CINTERFACE      // C++ calling convention for mapi calls


#ifdef WIN32
#ifdef CHICAGO
#define _INC_OLE
#endif
#define INC_OLE2
#define INC_RPC
#endif

#include <afxwin.h>     
#include <string.h>

#ifdef WIN16
#include <compobj.h>
#endif

#ifdef WIN32
#include <objbase.h>
#include <objerror.h>
#ifdef CHICAGO
#include <ole2.h>
#endif
#endif


#ifdef WIN16
#include <mapiwin.h>    
#endif
#include <mapix.h>      
#include <strtbl.h>     
#include <misctool.h>
#include <pvalloc.h>
#include "resource.h"   
#include "mdbview.h"  
#include "restrict.h"

/* Global Data */

ULONG   ulNumResTypes   = 0;        // determine number of restriction types

/*******************************************************************/
/**************************** RESTRICTION **************************/

/********************************************************************/
/*
 -  CRestrictionDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *
 */
/********************************************************************/

BOOL CRestrictionDlg::OnInitDialog()
{
    int             rgTabStops[12];
    DWORD           dwReturn        = 0;      
    char            szSearchState[80];
        
    SendDlgItemMessage(IDC_RESTRICTION,LB_RESETCONTENT,0,0);

    rgTabStops[0] = 20;
    rgTabStops[1] = 30;
    rgTabStops[2] = 40;
    rgTabStops[3] = 50;
    rgTabStops[4] = 60;
    rgTabStops[5] = 70;
    rgTabStops[6] = 80;
    rgTabStops[7] = 90;
    rgTabStops[8] = 100;
    rgTabStops[9] = 110;
    rgTabStops[10]= 120;

    dwReturn = SendDlgItemMessage(IDC_RESTRICTION,LB_SETTABSTOPS,
                    (WPARAM) 11,(LPARAM)rgTabStops );

    wsprintf(szSearchState,"Search State: %lu,%s%s%s%s",m_ulSearchState,
      ((m_ulSearchState & SEARCH_RUNNING)      ? "SEARCH_RUNNING | "        : ""),
      ((m_ulSearchState & SEARCH_REBUILD)      ? "SEARCH_REBUILD | "  : ""),
      ((m_ulSearchState & SEARCH_RECURSIVE)    ? "SEARCH_RECURSIVE | "   : ""),
      ((m_ulSearchState & SEARCH_FOREGROUND)   ? "SEARCH_FOREGROUND  "   : "0"));

    SetDlgItemText(IDT_SEARCHSTATE,szSearchState);

    DisplayRestriction(m_prest);

    return TRUE;    

}

/*******************************************************************/
/*
 -  CBldPropDlg::
 -  DisplayRestriction
 *
 *  Purpose:
 *
 *  Parameters:
 *
 *  Returns:
 *
 */
/*******************************************************************/

void CRestrictionDlg::DisplayRestriction(LPSRestriction lpRes)
{
    DWORD           dwReturn        = 0;      
    char            szBuff1[256];
    char            szBuff2[256];
    char            szBuff3[256];
    char            szBuffer[1024];
    ULONG           i = 0;
    static  ULONG   cTabs = 0;
    static  char    szTabs[11][22] = {"",
                     "\t",
                     "\t\t",
                     "\t\t\t",
                     "\t\t\t\t",
                     "\t\t\t\t\t",
                     "\t\t\t\t\t\t",
                     "\t\t\t\t\t\t\t",
                     "\t\t\t\t\t\t\t\t",
                     "\t\t\t\t\t\t\t\t\t",
                     "\t\t\t\t\t\t\t\t\t\t"};

    if(!lpRes)
    {
        dwReturn = SendDlgItemMessage(IDC_RESTRICTION,LB_ADDSTRING,0,(LPARAM)"lpRestriction == NULL"); 
        return;
    }        

    switch(lpRes->rt)
    {
    case RES_CONTENT:       
        wsprintf(szBuffer,"%sSContentRestriction:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_RESTRICTION,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        wsprintf(szBuffer,"%sFuzzy Level: %s", szTabs[cTabs+1],
            GetString("FuzzyLevel", lpRes->res.resContent.ulFuzzyLevel, szBuff1));
        dwReturn = SendDlgItemMessage(IDC_RESTRICTION,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        wsprintf(szBuffer,"%s%s contains %s", szTabs[cTabs+1],
            GetString("PropTags", lpRes->res.resContent.ulPropTag, szBuff1),
            SzGetPropValue(szBuff2, lpRes->res.resContent.lpProp));
        dwReturn = SendDlgItemMessage(IDC_RESTRICTION,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        break;

    case RES_PROPERTY:
        wsprintf(szBuffer,"%sSPropertyRestriction:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_RESTRICTION,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        wsprintf(szBuffer,"%s%s %s %s", szTabs[cTabs+1],
            GetString("PropTags", lpRes->res.resProperty.ulPropTag, szBuff1),
            GetString("RelOp", lpRes->res.resProperty.relop, szBuff2),
            SzGetPropValue(szBuff3, lpRes->res.resProperty.lpProp));
        dwReturn = SendDlgItemMessage(IDC_RESTRICTION,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        break;

    case RES_COMPAREPROPS:
        wsprintf(szBuffer,"%sSComparePropsRestriction:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_RESTRICTION,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        wsprintf(szBuffer,"%s%s %s %s", szTabs[cTabs+1],
            GetString("PropTags", lpRes->res.resCompareProps.ulPropTag1, szBuff1),
            GetString("RelOp", lpRes->res.resCompareProps.relop, szBuff2),
            GetString("PropTags", lpRes->res.resCompareProps.ulPropTag2, szBuff3));        
        dwReturn = SendDlgItemMessage(IDC_RESTRICTION,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        break;

    case RES_BITMASK:
        wsprintf(szBuffer,"%sSBitMaskRestriction:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_RESTRICTION,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        wsprintf(szBuffer,"%s(%s & 0x%08lX) %s", szTabs[cTabs+1],
            GetString("PropTags", lpRes->res.resBitMask.ulPropTag, szBuff1),
            lpRes->res.resBitMask.ulMask,
            GetString("Bmr", lpRes->res.resBitMask.relBMR, szBuff2));        
        dwReturn = SendDlgItemMessage(IDC_RESTRICTION,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        break;

    case RES_SIZE:
        wsprintf(szBuffer,"%sSSizeRestriction:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_RESTRICTION,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        wsprintf(szBuffer,"%ssizeof(%s) %s %lu", szTabs[cTabs+1],
            GetString("PropTags", lpRes->res.resSize.ulPropTag, szBuff1),
            GetString("RelOp", lpRes->res.resSize.relop, szBuff2),
            lpRes->res.resSize.cb);
        dwReturn = SendDlgItemMessage(IDC_RESTRICTION,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        break;

    case RES_EXIST:
        wsprintf(szBuffer,"%sSExistRestriction:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_RESTRICTION,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        wsprintf(szBuffer,"%s%s Exists", szTabs[cTabs+1],
            GetString("PropTags", lpRes->res.resExist.ulPropTag, szBuff1));       
        dwReturn = SendDlgItemMessage(IDC_RESTRICTION,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        break;

    case RES_SUBRESTRICTION:
        wsprintf(szBuffer,"%sSSubRestriction:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_RESTRICTION,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        wsprintf(szBuffer,"%s ulSubObject: %s", szTabs[cTabs+1],
            (lpRes->res.resSub.ulSubObject == PR_MESSAGE_ATTACHMENTS) 
                        ? "PR_MESSAGE_ATTACHMENTS" :
                          "PR_MESSAGE_RECIPIENTS"     );
        dwReturn = SendDlgItemMessage(IDC_RESTRICTION,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        cTabs++;
        DisplayRestriction(lpRes->res.resSub.lpRes);
        cTabs--;
        break;

    case RES_NOT:
        wsprintf(szBuffer,"%sNot:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_RESTRICTION,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        cTabs++;
        DisplayRestriction(lpRes->res.resNot.lpRes);
        cTabs--;
        break;

    case RES_AND:
        wsprintf(szBuffer,"%sAnd:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_RESTRICTION,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        cTabs++;
        for(i = 0; i < lpRes->res.resAnd.cRes; i++)
        {
            DisplayRestriction(&lpRes->res.resAnd.lpRes[i]);
        }
        cTabs--;
        break;

    case RES_OR:
        wsprintf(szBuffer,"%sOr:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_RESTRICTION,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        cTabs++;
        for(i = 0; i < lpRes->res.resOr.cRes; i++)
        {
            DisplayRestriction(&lpRes->res.resOr.lpRes[i]);
        }
        cTabs--;
        break;

    case RES_COMMENT:
        wsprintf(szBuffer,"%sSCommentRestriction:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_RESTRICTION,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        wsprintf(szBuffer,"%s cValues: %lu, lpPropValue: 0x%08X", szTabs[cTabs+1],
            lpRes->res.resComment.cValues,
            lpRes->res.resComment.lpProp);
        dwReturn = SendDlgItemMessage(IDC_RESTRICTION,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        cTabs++;
        DisplayRestriction(lpRes->res.resComment.lpRes);
        cTabs--;
        break;

    default:
        wsprintf(szBuffer,"%sUNKNOWN RES TYPE lpRes->rt == %lu",szTabs[cTabs] , lpRes->rt);
        dwReturn = SendDlgItemMessage(IDC_RESTRICTION,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        break;
    }
    return;
}

/*******************************************************************/
/*
 -  CRestrictionDlg::
 -  ~CRestrictionDlg
 -
 *  Purpose:
 *      Destructor for class CRestrictionDlg. Releases and Frees memory
 *      allocated in this class
 *
 */
/*******************************************************************/

CRestrictionDlg::~CRestrictionDlg()
{

} 



////////////////////////////////////////////////////



/*-----------------------------*/
/* CResDlg Message Handlers    */
/*-----------------------------*/

BEGIN_MESSAGE_MAP(CResDlg, CModalDialog)
    ON_COMMAND(IDC_AND,      OnAnd)
    ON_COMMAND(IDC_OR,       OnOr)
    ON_COMMAND(IDC_SUBRES1,  OnSubRes1)
    ON_COMMAND(IDC_SUBRES2,  OnSubRes2)
    ON_COMMAND(IDC_SUBRES3,  OnSubRes3)
    ON_CBN_SELCHANGE(IDC_RESTYPE1, OnResType1)
    ON_CBN_SELCHANGE(IDC_RESTYPE2, OnResType2)
    ON_CBN_SELCHANGE(IDC_RESTYPE3, OnResType3)
END_MESSAGE_MAP()

/*
 -  CResDlg::
 -  CResDlg
 *
 *  Purpose:
 *      Constructor for the CResDlg class.
 */

CResDlg::CResDlg(LPSPropTagArray lpspta, LPSRestriction lpr, CWnd* lpParent)
        : CModalDialog(IDD_RESDLG, lpParent)
{
    m_lpCurColumns = lpspta;
    m_lpRes        = lpr;

    m_lpSubRes = (LPSRestriction)PvAlloc(3 * sizeof(SRestriction));
}


/*
 -  CResDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *      Constructor for main dialog class.
 *
 */

BOOL CResDlg::OnInitDialog()
{
    ULONG   idx;
    char    szBuff[512];
    char    szBuffer[256];
    ULONG   ulNum;

    CheckDlgButton(IDC_AND, TRUE);

    ulNumResTypes = GetRowCount("RestrictionType");

    for(idx = 0; idx < ulNumResTypes; idx++)
    {
        GetRowString("RestrictionType",idx,szBuffer);
        SendDlgItemMessage(IDC_RESTYPE1, CB_ADDSTRING, 0,
                (LPARAM)szBuffer);
        SendDlgItemMessage(IDC_RESTYPE2, CB_ADDSTRING, 0,
                (LPARAM)szBuffer);
        SendDlgItemMessage(IDC_RESTYPE3, CB_ADDSTRING, 0,
                (LPARAM)szBuffer);
    }

    SendDlgItemMessage(IDC_RESTYPE1, CB_ADDSTRING, 0,
                (LPARAM) (char *)"None");
    SendDlgItemMessage(IDC_RESTYPE2, CB_ADDSTRING, 0,
                (LPARAM) (char *)"None");
    SendDlgItemMessage(IDC_RESTYPE3, CB_ADDSTRING, 0,
                (LPARAM) (char *)"None");

    SendDlgItemMessage(IDC_RESTYPE1, CB_SETCURSEL, (WPARAM) ulNumResTypes, (LPARAM) 0L);
    SendDlgItemMessage(IDC_RESTYPE2, CB_SETCURSEL, (WPARAM) ulNumResTypes, (LPARAM) 0L);
    SendDlgItemMessage(IDC_RESTYPE3, CB_SETCURSEL, (WPARAM) ulNumResTypes, (LPARAM) 0L);

    for(idx = 0; idx < m_lpCurColumns->cValues; idx++)
    {
        GetString("PropTags", m_lpCurColumns->aulPropTag[idx], szBuff);

        SendDlgItemMessage(IDC_PROPTAG1, CB_ADDSTRING, 0, (LPARAM)szBuff);
        SendDlgItemMessage(IDC_PROPTAG2, CB_ADDSTRING, 0, (LPARAM)szBuff);
        SendDlgItemMessage(IDC_PROPTAG3, CB_ADDSTRING, 0, (LPARAM)szBuff);
    }

    ulNum = GetRowCount("RelOp");

    for(idx = 0; idx < ulNum; idx++)
    {
        GetRowString("RelOp",idx,szBuffer);
  
        SendDlgItemMessage(IDC_RELATIONSHIP1, CB_ADDSTRING, 0,
                (LPARAM)szBuffer);
        SendDlgItemMessage(IDC_RELATIONSHIP2, CB_ADDSTRING, 0,
                (LPARAM)szBuffer);
        SendDlgItemMessage(IDC_RELATIONSHIP3, CB_ADDSTRING, 0,
                (LPARAM)szBuffer);
    }

    /* Hide everything until the Restriction Type is chosen
       Restriction 1 */
    GetDlgItem(IDT_PROPTAG1)->ShowWindow(FALSE);
    GetDlgItem(IDC_PROPTAG1)->ShowWindow(FALSE);
    GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);
    GetDlgItem(IDT_RELATIONSHIP1)->ShowWindow(FALSE);
    GetDlgItem(IDC_RELATIONSHIP1)->ShowWindow(FALSE);
    GetDlgItem(IDT_VALUE1)->ShowWindow(FALSE);
    GetDlgItem(IDC_VALUE1)->ShowWindow(FALSE);
    GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);

    /* Restriction 2 */
    GetDlgItem(IDT_PROPTAG2)->ShowWindow(FALSE);
    GetDlgItem(IDC_PROPTAG2)->ShowWindow(FALSE);
    GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);
    GetDlgItem(IDT_RELATIONSHIP2)->ShowWindow(FALSE);
    GetDlgItem(IDC_RELATIONSHIP2)->ShowWindow(FALSE);
    GetDlgItem(IDT_VALUE2)->ShowWindow(FALSE);
    GetDlgItem(IDC_VALUE2)->ShowWindow(FALSE);
    GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);

    /* Restriction 3 */
    GetDlgItem(IDT_PROPTAG3)->ShowWindow(FALSE);
    GetDlgItem(IDC_PROPTAG3)->ShowWindow(FALSE);
    GetDlgItem(IDC_PROPTAG32)->ShowWindow(FALSE);
    GetDlgItem(IDT_RELATIONSHIP3)->ShowWindow(FALSE);
    GetDlgItem(IDC_RELATIONSHIP3)->ShowWindow(FALSE);
    GetDlgItem(IDT_VALUE3)->ShowWindow(FALSE);
    GetDlgItem(IDC_VALUE3)->ShowWindow(FALSE);

    m_fComb = 0;
    m_nResType1 = (int) ulNumResTypes;
    m_nResType2 = (int) ulNumResTypes;
    m_nResType3 = (int) ulNumResTypes;

    GetDlgItem(IDC_AND)->SetFocus();

    return TRUE;
}


/*
 -  CResDlg
 -  OnAnd
 -
 *  Purpose:
 *      Sets the AND restriction flag.
 *
 */

void CResDlg::OnAnd()
{
    m_fComb = 0;
}


/*
 -  CResDlg
 -  OnOr
 -
 *  Purpose:
 *      Sets the OR restriction flag.
 *
 */

void CResDlg::OnOr()
{
    m_fComb = 1;
}


/*
 -  CResDlg
 -  OnResType1
 -
 *  Purpose:
 *      Changes the view of the restriction based on the type for
 *      Restriction 1.
 *
 */

void CResDlg::OnResType1()
{
    INT     idx;
    LONG    nNewResType;
    char    szBuff[128];
    char    szBuffer[256];
    ULONG   ulNum   = 0;

    nNewResType = SendDlgItemMessage(IDC_RESTYPE1, CB_GETCURSEL, 0, 0L);

    if(nNewResType == m_nResType1)
        return;

    switch(nNewResType)
    {
    case RES_AND:
    case RES_OR:
    case RES_NOT:
        GetDlgItem(IDT_PROPTAG1)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG1)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP1)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP1)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE1)->ShowWindow(FALSE);
        GetDlgItem(IDC_VALUE1)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES1)->EnableWindow(TRUE);
        break;

    case RES_CONTENT:
        GetDlgItem(IDT_PROPTAG1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP1)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP1)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE1)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE1)->ShowWindow(TRUE);
        GetDlgItem(IDC_SUBRES1)->EnableWindow(FALSE);
        
        GetDlgItem(IDT_VALUE1)->SetWindowText("Value:");
        GetDlgItem(IDT_PROPTAG1)->SetWindowText("PropTag:");
        GetDlgItem(IDT_RELATIONSHIP1)->SetWindowText("Fuzzy Level:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP1, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("FuzzyLevel");

        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("FuzzyLevel",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP1, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }

        break;

    case RES_PROPERTY:
        GetDlgItem(IDT_PROPTAG1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP1)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP1)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE1)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE1)->ShowWindow(TRUE);
        GetDlgItem(IDC_SUBRES1)->EnableWindow(FALSE);
        
        GetDlgItem(IDT_VALUE1)->SetWindowText("Value:");
        GetDlgItem(IDT_PROPTAG1)->SetWindowText("PropTag:");
        GetDlgItem(IDT_RELATIONSHIP1)->SetWindowText("Relationship:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP1, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("RelOp");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("RelOp",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP1, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }
        break;

    case RES_COMPAREPROPS:
        GetDlgItem(IDT_PROPTAG1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG12)->ShowWindow(TRUE);
        GetDlgItem(IDT_RELATIONSHIP1)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP1)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE1)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE1)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES1)->EnableWindow(FALSE);
        
        GetDlgItem(IDT_PROPTAG1)->SetWindowText("PropTag1:");
        GetDlgItem(IDT_VALUE1)->SetWindowText("PropTag2:");
        GetDlgItem(IDT_RELATIONSHIP1)->SetWindowText("Relationship:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP1, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("RelOp");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("RelOp",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP1, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }

        // update second PROPS
        SendDlgItemMessage(IDC_PROPTAG12, CB_RESETCONTENT, 0, (LPARAM)szBuff);
        for(idx = 0; idx < m_lpCurColumns->cValues; idx++)
        {
            GetString("PropTags", m_lpCurColumns->aulPropTag[idx], szBuff);
            SendDlgItemMessage(IDC_PROPTAG12, CB_ADDSTRING, 0, (LPARAM)szBuff);
        }
        break;

    case RES_BITMASK:
        GetDlgItem(IDT_PROPTAG1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP1)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP1)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE1)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE1)->ShowWindow(TRUE);
        GetDlgItem(IDC_SUBRES1)->EnableWindow(FALSE);

        GetDlgItem(IDT_VALUE1)->SetWindowText("Mask: ex. 0x00000678");
        GetDlgItem(IDT_PROPTAG1)->SetWindowText("PropTag:");
        GetDlgItem(IDT_RELATIONSHIP1)->SetWindowText("Relationship:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP1, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("Bmr");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("Bmr",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP1, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }
        break;

    case RES_SUBRESTRICTION:
        GetDlgItem(IDT_PROPTAG1)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG1)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG12)->ShowWindow(TRUE);
        GetDlgItem(IDT_RELATIONSHIP1)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP1)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE1)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE1)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES1)->EnableWindow(TRUE);

        GetDlgItem(IDT_VALUE1)->SetWindowText("ulSubObj:");

        // DIFFERENT SPECIAL PROPERTIES
        SendDlgItemMessage(IDC_PROPTAG12, CB_RESETCONTENT, 0, 0L);
        SendDlgItemMessage(IDC_PROPTAG12, CB_ADDSTRING,    0, (LPARAM)(char *)"PR_MESSAGE_ATTACHMENTS");
        SendDlgItemMessage(IDC_PROPTAG12, CB_ADDSTRING,    0, (LPARAM)(char *)"PR_MESSAGE_RECIPIENTS");

        break;

    case RES_SIZE:
        GetDlgItem(IDT_PROPTAG1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP1)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP1)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE1)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES1)->EnableWindow(FALSE);

        GetDlgItem(IDT_PROPTAG1)->SetWindowText("PropTag:");
        GetDlgItem(IDT_RELATIONSHIP1)->SetWindowText("Relationship:");
        GetDlgItem(IDT_VALUE1)->SetWindowText("cb:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP1, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("RelOp");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("RelOp",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP1, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }

        break;

    case RES_EXIST:
        GetDlgItem(IDT_PROPTAG1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP1)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP1)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE1)->ShowWindow(FALSE);
        GetDlgItem(IDC_VALUE1)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES1)->EnableWindow(FALSE);

        GetDlgItem(IDT_PROPTAG1)->SetWindowText("PropTag:");
        break;

    case RES_COMMENT:
        GetDlgItem(IDT_PROPTAG1)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG1)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP1)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP1)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE1)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE1)->ShowWindow(TRUE);
        GetDlgItem(IDC_SUBRES1)->EnableWindow(TRUE);

        GetDlgItem(IDT_VALUE1)->SetWindowText("cValues:");

        break;

    
    default:
        GetDlgItem(IDT_PROPTAG1)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG1)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP1)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP1)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE1)->ShowWindow(FALSE);
        GetDlgItem(IDC_VALUE1)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES1)->EnableWindow(FALSE);
    }

    m_nResType1 = (int)nNewResType;
}


/*
 -  CResDlg
 -  OnResType2
 -
 *  Purpose:
 *      Changes the view of the restriction based on the type for
 *      Restriction 2.
 *
 */

void CResDlg::OnResType2()
{
    INT     idx;
    LONG    nNewResType;
    char    szBuff[128];
    char    szBuffer[256];
    ULONG   ulNum   = 0;

    nNewResType = SendDlgItemMessage(IDC_RESTYPE2, CB_GETCURSEL, 0, 0L);

    if(nNewResType == m_nResType2)
        return;

    switch(nNewResType)
    {
    case RES_AND:
    case RES_OR:
    case RES_NOT:
        GetDlgItem(IDT_PROPTAG2)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG2)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP2)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP2)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE2)->ShowWindow(FALSE);
        GetDlgItem(IDC_VALUE2)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES2)->EnableWindow(TRUE);
        break;

    case RES_CONTENT:
        GetDlgItem(IDT_PROPTAG2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP2)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP2)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE2)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE2)->ShowWindow(TRUE);

        GetDlgItem(IDC_SUBRES2)->EnableWindow(FALSE);
        GetDlgItem(IDT_VALUE2)->SetWindowText("Value:");
        GetDlgItem(IDT_PROPTAG2)->SetWindowText("PropTag:");
        GetDlgItem(IDT_RELATIONSHIP2)->SetWindowText("Fuzzy Level:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP2, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("FuzzyLevel");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("FuzzyLevel",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP2, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }
        break;

    case RES_PROPERTY:
        GetDlgItem(IDT_PROPTAG2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP2)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP2)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE2)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE2)->ShowWindow(TRUE);

        GetDlgItem(IDC_SUBRES2)->EnableWindow(FALSE);
        GetDlgItem(IDT_VALUE2)->SetWindowText("Value:");
        GetDlgItem(IDT_PROPTAG2)->SetWindowText("PropTag:");
        GetDlgItem(IDT_RELATIONSHIP2)->SetWindowText("Relationship:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP2, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("RelOp");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("RelOp",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP2, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }

        break;

    case RES_COMPAREPROPS:
        GetDlgItem(IDT_PROPTAG2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG22)->ShowWindow(TRUE);
        GetDlgItem(IDT_RELATIONSHIP2)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP2)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE2)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE2)->ShowWindow(FALSE);

        GetDlgItem(IDC_SUBRES2)->EnableWindow(FALSE);
        GetDlgItem(IDT_PROPTAG2)->SetWindowText("PropTag1:");
        GetDlgItem(IDT_VALUE2)->SetWindowText("PropTag2:");
        GetDlgItem(IDT_RELATIONSHIP2)->SetWindowText("Relationship:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP2, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("RelOp");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("RelOp",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP2, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }

        // update 2nd PROPS
        SendDlgItemMessage(IDC_PROPTAG22, CB_RESETCONTENT, 0, (LPARAM)szBuff);
        for(idx = 0; idx < m_lpCurColumns->cValues; idx++)
        {
            GetString("PropTags", m_lpCurColumns->aulPropTag[idx], szBuff);
            SendDlgItemMessage(IDC_PROPTAG22, CB_ADDSTRING, 0, (LPARAM)szBuff);
        }
        break;

    case RES_BITMASK:
        GetDlgItem(IDT_PROPTAG2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP2)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP2)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE2)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE2)->ShowWindow(TRUE);

        GetDlgItem(IDC_SUBRES2)->EnableWindow(FALSE);
        GetDlgItem(IDT_VALUE2)->SetWindowText("Mask: ex. 0x00000678");
        GetDlgItem(IDT_PROPTAG2)->SetWindowText("PropTag:");
        GetDlgItem(IDT_RELATIONSHIP2)->SetWindowText("Relationship:");


        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP2, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("Bmr");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("Bmr",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP2, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }

        break;

    case RES_SUBRESTRICTION:
        GetDlgItem(IDT_PROPTAG2)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG2)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG22)->ShowWindow(TRUE);
        GetDlgItem(IDT_RELATIONSHIP2)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP2)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE2)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE2)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES2)->EnableWindow(TRUE);

        GetDlgItem(IDT_VALUE2)->SetWindowText("ulSubObj:");

        // DIFFERENT SPECIAL PROPERTIES
        SendDlgItemMessage(IDC_PROPTAG22, CB_RESETCONTENT, 0, 0L);
        SendDlgItemMessage(IDC_PROPTAG22, CB_ADDSTRING,    0, (LPARAM)(char *)"PR_MESSAGE_ATTACHMENTS");
        SendDlgItemMessage(IDC_PROPTAG22, CB_ADDSTRING,    0, (LPARAM)(char *)"PR_MESSAGE_RECIPIENTS");
                
        break;

    case RES_SIZE:
        GetDlgItem(IDT_PROPTAG2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP2)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP2)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE2)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES2)->EnableWindow(FALSE);

        GetDlgItem(IDT_PROPTAG2)->SetWindowText("PropTag:");
        GetDlgItem(IDT_RELATIONSHIP2)->SetWindowText("Relationship:");
        GetDlgItem(IDT_VALUE2)->SetWindowText("cb:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP2, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("RelOp");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("RelOp",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP2, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }

        break;

    case RES_EXIST:
        GetDlgItem(IDT_PROPTAG2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP2)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP2)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE2)->ShowWindow(FALSE);
        GetDlgItem(IDC_VALUE2)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES2)->EnableWindow(FALSE);

        GetDlgItem(IDT_PROPTAG2)->SetWindowText("PropTag:");
        break;


    case RES_COMMENT:
        GetDlgItem(IDT_PROPTAG2)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG2)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP2)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP2)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE2)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE2)->ShowWindow(TRUE);
        GetDlgItem(IDC_SUBRES2)->EnableWindow(TRUE);

        GetDlgItem(IDT_VALUE2)->SetWindowText("cValues:");

        break;



    default:
        GetDlgItem(IDT_PROPTAG2)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG2)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP2)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP2)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE2)->ShowWindow(FALSE);
        GetDlgItem(IDC_VALUE2)->ShowWindow(FALSE);
        GetDlgItem(IDC_VALUE2)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES1)->EnableWindow(FALSE);
  
    }

    m_nResType2 = (int)nNewResType;
}


/*
 -  CResDlg
 -  OnResType3
 -
 *  Purpose:
 *      Changes the view of the restriction based on the type for
 *      Restriction 3.
 *
 */

void CResDlg::OnResType3()
{
    INT     idx;
    LONG    nNewResType;
    char    szBuff[128];
    char    szBuffer[256];
    ULONG   ulNum   = 0;

    nNewResType = SendDlgItemMessage(IDC_RESTYPE3, CB_GETCURSEL, 0, 0L);

    if(nNewResType == m_nResType3)
        return;

    switch(nNewResType)
    {
    case RES_AND:
    case RES_OR:
    case RES_NOT:
        GetDlgItem(IDT_PROPTAG3)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG3)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG32)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP3)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP3)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE3)->ShowWindow(FALSE);
        GetDlgItem(IDC_VALUE3)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES3)->EnableWindow(TRUE);
        break;

    case RES_CONTENT:
        GetDlgItem(IDT_PROPTAG3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG32)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP3)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP3)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE3)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE3)->ShowWindow(TRUE);

        GetDlgItem(IDC_SUBRES3)->EnableWindow(FALSE);
        GetDlgItem(IDT_VALUE3)->SetWindowText("Value:");
        GetDlgItem(IDT_PROPTAG3)->SetWindowText("PropTag:");
        GetDlgItem(IDT_RELATIONSHIP3)->SetWindowText("Fuzzy Level:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP3, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("FuzzyLevel");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("FuzzyLevel",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP3, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }

        break;

    case RES_PROPERTY:
        GetDlgItem(IDT_PROPTAG3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG32)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP3)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP3)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE3)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE3)->ShowWindow(TRUE);

        GetDlgItem(IDC_SUBRES3)->EnableWindow(FALSE);
        GetDlgItem(IDT_VALUE3)->SetWindowText("Value:");
        GetDlgItem(IDT_PROPTAG3)->SetWindowText("PropTag:");
        GetDlgItem(IDT_RELATIONSHIP3)->SetWindowText("Relationship:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP3, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("RelOp");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("RelOp",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP3, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }

        break;

    case RES_COMPAREPROPS:
        GetDlgItem(IDT_PROPTAG3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG32)->ShowWindow(TRUE);
        GetDlgItem(IDT_RELATIONSHIP3)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP3)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE3)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE3)->ShowWindow(FALSE);

        GetDlgItem(IDC_SUBRES3)->EnableWindow(FALSE);
        GetDlgItem(IDT_PROPTAG3)->SetWindowText("PropTag1:");
        GetDlgItem(IDT_VALUE3)->SetWindowText("PropTag2:");
        GetDlgItem(IDT_RELATIONSHIP3)->SetWindowText("Relationship:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP3, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("RelOp");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("RelOp",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP3, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }

        // update 2nd PROPS
        SendDlgItemMessage(IDC_PROPTAG32, CB_RESETCONTENT, 0, (LPARAM)szBuff);
        for(idx = 0; idx < m_lpCurColumns->cValues; idx++)
        {
            GetString("PropTags", m_lpCurColumns->aulPropTag[idx], szBuff);
            SendDlgItemMessage(IDC_PROPTAG32, CB_ADDSTRING, 0, (LPARAM)szBuff);
        }
        break;

    case RES_BITMASK:
        GetDlgItem(IDT_PROPTAG3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG32)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP3)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP3)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE3)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE3)->ShowWindow(TRUE);

        GetDlgItem(IDC_SUBRES3)->EnableWindow(FALSE);
        GetDlgItem(IDT_VALUE3)->SetWindowText("Mask: ex. 0x00000678");
        GetDlgItem(IDT_PROPTAG3)->SetWindowText("PropTag:");
        GetDlgItem(IDT_RELATIONSHIP3)->SetWindowText("Relationship:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP3, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("Bmr");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("Bmr",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP3, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }

        break;

    case RES_SUBRESTRICTION:
        GetDlgItem(IDT_PROPTAG3)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG3)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG32)->ShowWindow(TRUE);
        GetDlgItem(IDT_RELATIONSHIP3)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP3)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE3)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE3)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES3)->EnableWindow(TRUE);
        
        GetDlgItem(IDT_VALUE3)->SetWindowText("ulSubObj:");

        // DIFFERENT SPECIAL PROPERTIES
        SendDlgItemMessage(IDC_PROPTAG32, CB_RESETCONTENT, 0, 0L);
        SendDlgItemMessage(IDC_PROPTAG32, CB_ADDSTRING,    0, (LPARAM)(char *)"PR_MESSAGE_ATTACHMENTS");
        SendDlgItemMessage(IDC_PROPTAG32, CB_ADDSTRING,    0, (LPARAM)(char *)"PR_MESSAGE_RECIPIENTS");
        
        break;

    case RES_SIZE:
        GetDlgItem(IDT_PROPTAG3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG32)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP3)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP3)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE3)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG32)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES3)->EnableWindow(FALSE);

        GetDlgItem(IDT_PROPTAG3)->SetWindowText("PropTag:");
        GetDlgItem(IDT_RELATIONSHIP3)->SetWindowText("Relationship:");
        GetDlgItem(IDT_VALUE3)->SetWindowText("cb:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP3, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("RelOp");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("RelOp",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP3, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }
        break;

    case RES_EXIST:
        GetDlgItem(IDT_PROPTAG3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG32)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP3)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP3)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE3)->ShowWindow(FALSE);
        GetDlgItem(IDC_VALUE3)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG32)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES3)->EnableWindow(FALSE);

        GetDlgItem(IDT_PROPTAG3)->SetWindowText("PropTag:");
        
        break;

    case RES_COMMENT:
        GetDlgItem(IDT_PROPTAG3)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG3)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG32)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP3)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP3)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE3)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE3)->ShowWindow(TRUE);
        GetDlgItem(IDC_SUBRES3)->EnableWindow(TRUE);

        GetDlgItem(IDT_VALUE2)->SetWindowText("cValues:");
        break;


    default:
        GetDlgItem(IDT_PROPTAG3)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG3)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG32)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP3)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP3)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE3)->ShowWindow(FALSE);
        GetDlgItem(IDC_VALUE3)->ShowWindow(FALSE);
        GetDlgItem(IDC_VALUE3)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES3)->EnableWindow(FALSE);
    }

    m_nResType3 = (int)nNewResType;
}


/*
 -  CResDlg
 -  OnSubRes1
 -
 *  Purpose:
 *      Creates a sub-Restriction for Restriction 1.
 *
 */

void CResDlg::OnSubRes1()
{
    LPSRestriction  lpRes;
    ULONG           rt;

    rt = SendDlgItemMessage(IDC_RESTYPE1, CB_GETCURSEL, 0, 0L);

    if(rt == RES_NOT)
    {
        lpRes = (LPSRestriction)PvAllocMore(sizeof(SRestriction), m_lpSubRes);

        m_lpSubRes[0].rt = RES_NOT;
        m_lpSubRes[0].res.resNot.lpRes = lpRes;
    }
    else if(rt == RES_SUBRESTRICTION)
    {
        lpRes = (LPSRestriction)PvAllocMore(sizeof(SRestriction), m_lpSubRes);

        m_lpSubRes[0].rt = RES_SUBRESTRICTION;
        m_lpSubRes[0].res.resSub.lpRes = lpRes;
    }
    else if(rt == RES_COMMENT)
    {
        lpRes = (LPSRestriction)PvAllocMore(sizeof(SRestriction), m_lpSubRes);

        m_lpSubRes[0].rt = RES_COMMENT;
        m_lpSubRes[0].res.resComment.lpRes = lpRes;
    }
    else
    {
        lpRes = &m_lpSubRes[0];
    }

    CResDlg dlgSubRes1(m_lpCurColumns, lpRes);
    dlgSubRes1.SetWindowText("Sub Restriction");

    if(dlgSubRes1.DoModal() != IDOK) 
    {
        if(rt == RES_NOT)
        {
            m_lpSubRes[0].res.resNot.lpRes = NULL;
            SendDlgItemMessage(IDC_RESTYPE1, CB_SETCURSEL, (WPARAM) ulNumResTypes, 0L);
            GetDlgItem(IDC_SUBRES1)->EnableWindow(FALSE);
        }
        else if(rt ==  RES_SUBRESTRICTION )
        { 
            m_lpSubRes[0].res.resSub.lpRes = NULL;
            SendDlgItemMessage(IDC_RESTYPE1, CB_SETCURSEL, (WPARAM) ulNumResTypes, 0L);
            GetDlgItem(IDC_SUBRES1)->EnableWindow(FALSE);
            
            GetDlgItem(IDT_VALUE1)->ShowWindow(FALSE);
            GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);
        }
        else if(rt ==  RES_COMMENT )
        {
            m_lpSubRes[0].res.resComment.lpRes = NULL;
            SendDlgItemMessage(IDC_RESTYPE1, CB_SETCURSEL, (WPARAM) ulNumResTypes, 0L);
            GetDlgItem(IDC_SUBRES1)->EnableWindow(FALSE);
         
            GetDlgItem(IDT_VALUE1)->ShowWindow(FALSE);
            GetDlgItem(IDC_VALUE1)->ShowWindow(FALSE);
        }        
    }
}


/*
 -  CResDlg
 -  OnSubRes2
 -
 *  Purpose:
 *      Creates a sub-Restriction for Restriction 2.
 *
 */

void CResDlg::OnSubRes2()
{
    LPSRestriction  lpRes;
    ULONG           rt;

    rt = SendDlgItemMessage(IDC_RESTYPE2, CB_GETCURSEL, 0, 0L);

    if(rt == RES_NOT)
    {
        lpRes = (LPSRestriction)PvAllocMore(sizeof(SRestriction), m_lpSubRes);

        m_lpSubRes[1].rt = RES_NOT;
        m_lpSubRes[1].res.resNot.lpRes = lpRes;
    }
    else if(rt == RES_SUBRESTRICTION)
    {
        lpRes = (LPSRestriction)PvAllocMore(sizeof(SRestriction), m_lpSubRes);

        m_lpSubRes[1].rt = RES_SUBRESTRICTION;
        m_lpSubRes[1].res.resSub.lpRes = lpRes;
    }
    else if(rt == RES_COMMENT)
    {
        lpRes = (LPSRestriction)PvAllocMore(sizeof(SRestriction), m_lpSubRes);

        m_lpSubRes[1].rt = RES_COMMENT;
        m_lpSubRes[1].res.resComment.lpRes = lpRes;
    }
    else
    {
        lpRes = &m_lpSubRes[1];
    }

    CResDlg dlgSubRes2(m_lpCurColumns, lpRes);
    dlgSubRes2.SetWindowText("Sub Restriction");

    if(dlgSubRes2.DoModal() != IDOK) 
    {
        if(rt == RES_NOT)
        {
            m_lpSubRes[1].res.resNot.lpRes = NULL;
            SendDlgItemMessage(IDC_RESTYPE2, CB_SETCURSEL, (WPARAM) ulNumResTypes, 0L);
            GetDlgItem(IDC_SUBRES2)->EnableWindow(FALSE);

        }
        else if(rt ==  RES_SUBRESTRICTION )
        { 
            m_lpSubRes[1].res.resSub.lpRes = NULL;
            SendDlgItemMessage(IDC_RESTYPE2, CB_SETCURSEL, (WPARAM) ulNumResTypes, 0L);
            GetDlgItem(IDC_SUBRES2)->EnableWindow(FALSE);
            GetDlgItem(IDT_VALUE2)->ShowWindow(FALSE);
            GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);
        }
        else if(rt ==  RES_COMMENT )
        {
            m_lpSubRes[1].res.resComment.lpRes = NULL;
            SendDlgItemMessage(IDC_RESTYPE2, CB_SETCURSEL, (WPARAM) ulNumResTypes, 0L);
            GetDlgItem(IDC_SUBRES2)->EnableWindow(FALSE);

            GetDlgItem(IDT_VALUE2)->ShowWindow(FALSE);
            GetDlgItem(IDC_VALUE2)->ShowWindow(FALSE);
        }        
    }
}


/*
 -  CResDlg
 -  OnSubRes3
 -
 *  Purpose:
 *      Creates a sub-Restriction for Restriction 3.
 *
 */

void CResDlg::OnSubRes3()
{
    LPSRestriction  lpRes;
    ULONG           rt;

    rt = SendDlgItemMessage(IDC_RESTYPE3, CB_GETCURSEL, 0, 0L);

    if(rt == RES_NOT)
    {
        lpRes = (LPSRestriction)PvAllocMore(sizeof(SRestriction), m_lpSubRes);

        m_lpSubRes[2].rt = RES_NOT;
        m_lpSubRes[2].res.resNot.lpRes = lpRes;
    }
    else if(rt == RES_SUBRESTRICTION)
    {
        lpRes = (LPSRestriction)PvAllocMore(sizeof(SRestriction), m_lpSubRes);

        m_lpSubRes[2].rt = RES_SUBRESTRICTION;
        m_lpSubRes[2].res.resSub.lpRes = lpRes;
    }
    else if(rt == RES_COMMENT)
    {
        lpRes = (LPSRestriction)PvAllocMore(sizeof(SRestriction), m_lpSubRes);

        m_lpSubRes[2].rt = RES_COMMENT;
        m_lpSubRes[2].res.resComment.lpRes = lpRes;
    }
    else
    {
        lpRes = &m_lpSubRes[2];
    }

    CResDlg dlgSubRes3(m_lpCurColumns, lpRes);
    dlgSubRes3.SetWindowText("Sub Restriction");


    if(dlgSubRes3.DoModal() != IDOK) 
    {
        if(rt == RES_NOT)
        {
            m_lpSubRes[2].res.resNot.lpRes = NULL;
            SendDlgItemMessage(IDC_RESTYPE3, CB_SETCURSEL, (WPARAM) ulNumResTypes, 0L);
            GetDlgItem(IDC_SUBRES3)->EnableWindow(FALSE);

        }
        else if(rt ==  RES_SUBRESTRICTION )
        { 
            m_lpSubRes[2].res.resSub.lpRes = NULL;
            SendDlgItemMessage(IDC_RESTYPE3, CB_SETCURSEL, (WPARAM) ulNumResTypes, 0L);
            GetDlgItem(IDC_SUBRES3)->EnableWindow(FALSE);

            GetDlgItem(IDT_VALUE3)->ShowWindow(FALSE);
            GetDlgItem(IDC_PROPTAG32)->ShowWindow(FALSE);

        }                         
        else if(rt ==  RES_COMMENT )
        {
            m_lpSubRes[2].res.resComment.lpRes = NULL;
            SendDlgItemMessage(IDC_RESTYPE3, CB_SETCURSEL, (WPARAM) ulNumResTypes, 0L);
            GetDlgItem(IDC_SUBRES3)->EnableWindow(FALSE);

            GetDlgItem(IDT_VALUE3)->ShowWindow(FALSE);
            GetDlgItem(IDC_VALUE3)->ShowWindow(FALSE);
        }        
    }
}


/*
 -  CResDlg
 -  OnOK
 -
 *  Purpose:
 *      Collects the restiction and sub-restriction information and does
 *      the appropriate assignments.  This function performs the assignments
 *      for all three restrictions.
 *
 */

void CResDlg::OnOK()
{
    ULONG           cRes = 1;
    char            szBuff[1024];
    char            *szEnd      = NULL;
    LPSPropValue    lpProp;
    ULONG           idx;
    BOOL            fTrans;
    LONG            lSelection   = 0;
    ULONG           i = 0;

    m_lpRes->rt = m_fComb;
    m_lpRes->res.resAnd.cRes = 0;
    m_lpRes->res.resAnd.lpRes = m_lpSubRes;

    /* Collect values from Restriction 1 group */
    switch(m_lpSubRes[0].rt = SendDlgItemMessage(IDC_RESTYPE1,
            CB_GETCURSEL, 0, 0L))
    {
    case RES_AND:
    case RES_OR:
    case RES_NOT:
        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_CONTENT:
        idx = SendDlgItemMessage(IDC_PROPTAG1, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[0].res.resContent.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP1, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[0].res.resContent.ulFuzzyLevel = GetRowID("FuzzyLevel",lSelection);
        
        SendDlgItemMessage(IDC_VALUE1, WM_GETTEXT, sizeof(szBuff),
                (LPARAM)szBuff);
        lpProp = (LPSPropValue)PvAllocMore(sizeof(SPropValue),
                (LPVOID)m_lpSubRes);

        MakePropValue(lpProp, PROP_TAG(PT_TSTRING, 0x0000), szBuff, m_lpSubRes);

        m_lpSubRes[0].res.resContent.lpProp = lpProp;
        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_PROPERTY:
        idx = SendDlgItemMessage(IDC_PROPTAG1, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[0].res.resProperty.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP1, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[0].res.resProperty.relop  = GetRowID("RelOp",lSelection);
        
        SendDlgItemMessage(IDC_VALUE1, WM_GETTEXT, sizeof(szBuff),
                (LPARAM)szBuff);

        lpProp = (LPSPropValue)PvAllocMore(sizeof(SPropValue),
                (LPVOID)m_lpSubRes);

        MakePropValue(lpProp,m_lpCurColumns->aulPropTag[idx] , szBuff, m_lpSubRes);
        
        m_lpSubRes[0].res.resProperty.lpProp = lpProp;
        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_COMPAREPROPS:
        idx = SendDlgItemMessage(IDC_PROPTAG1, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[0].res.resCompareProps.ulPropTag1 = m_lpCurColumns->aulPropTag[idx];

        idx = SendDlgItemMessage(IDC_PROPTAG12, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[0].res.resCompareProps.ulPropTag2 = m_lpCurColumns->aulPropTag[idx];

        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP1, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[0].res.resCompareProps.relop  = GetRowID("RelOp",lSelection);

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_BITMASK:
        idx = SendDlgItemMessage(IDC_PROPTAG1, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[0].res.resBitMask.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP1, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[0].res.resBitMask.relBMR  = GetRowID("Bmr",lSelection);

        m_lpSubRes[0].res.resBitMask.relBMR = SendDlgItemMessage(IDC_RELATIONSHIP1,
                CB_GETCURSEL, 0, 0L);

        SendDlgItemMessage(IDC_VALUE1, WM_GETTEXT, sizeof(szBuff),
                (LPARAM)szBuff);

//        #ifdef WIN16
//            wsscanf(szBuff, "0x%lX", &m_lpSubRes[0].res.resBitMask.ulMask);
//        #else
//            sscanf(szBuff, "0x%lX", &m_lpSubRes[0].res.resBitMask.ulMask);
//        #endif


        m_lpSubRes[0].res.resBitMask.ulMask = strtoul(szBuff,&szEnd,16);
        
        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_SIZE:
    
        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP1, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[0].res.resSize.relop  = GetRowID("RelOp",lSelection);
    
        idx = SendDlgItemMessage(IDC_PROPTAG1, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[0].res.resSize.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        m_lpSubRes[0].res.resSize.cb = (ULONG)GetDlgItemInt(IDC_VALUE1, &fTrans , FALSE );

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_EXIST:
        idx = SendDlgItemMessage(IDC_PROPTAG1, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[0].res.resExist.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_SUBRESTRICTION:
        idx = SendDlgItemMessage(IDC_PROPTAG12, CB_GETCURSEL, 0, 0L);
        if( idx == 1 ) 
            m_lpSubRes[0].res.resSub.ulSubObject = PR_MESSAGE_RECIPIENTS;
        else
            m_lpSubRes[0].res.resSub.ulSubObject = PR_MESSAGE_ATTACHMENTS;

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_COMMENT:
        m_lpSubRes[0].res.resComment.cValues = (ULONG)GetDlgItemInt(IDC_VALUE1, &fTrans , FALSE );

        lpProp = (LPSPropValue)PvAllocMore( m_lpSubRes[0].res.resComment.cValues * sizeof(SPropValue),
                (LPVOID)m_lpSubRes);

        for( i = 0 ; i < m_lpSubRes[0].res.resComment.cValues ; i++)
            MakePropValue( &(lpProp[i]),PR_SUBJECT, "TEST STRING", m_lpSubRes);

        m_lpSubRes[0].res.resComment.lpProp = lpProp;
        m_lpRes[0].res.resAnd.cRes++;
        break;

    default:
        break;
    }

    /* Collect values from Restriction 2 group */
    switch(m_lpSubRes[1].rt = SendDlgItemMessage(IDC_RESTYPE2,
            CB_GETCURSEL, 0, 0L))
    {
    case RES_AND:
    case RES_OR:
    case RES_NOT:
        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_CONTENT:
        idx = SendDlgItemMessage(IDC_PROPTAG2, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[1].res.resContent.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP2, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[1].res.resContent.ulFuzzyLevel = GetRowID("FuzzyLevel",lSelection);

        SendDlgItemMessage(IDC_VALUE2, WM_GETTEXT, sizeof(szBuff),
                (LPARAM)szBuff);
        lpProp = (LPSPropValue)PvAllocMore(sizeof(SPropValue),
                (LPVOID)m_lpSubRes);

        MakePropValue(lpProp, PROP_TAG(PT_TSTRING, 0x0000), szBuff, m_lpSubRes);

        m_lpSubRes[1].res.resContent.lpProp = lpProp;
        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_PROPERTY:
        idx = SendDlgItemMessage(IDC_PROPTAG2, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[1].res.resProperty.ulPropTag = m_lpCurColumns->aulPropTag[idx];


        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP2, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[1].res.resProperty.relop  = GetRowID("RelOp",lSelection);

        SendDlgItemMessage(IDC_VALUE2, WM_GETTEXT, sizeof(szBuff),
                (LPARAM)szBuff);
        lpProp = (LPSPropValue)PvAllocMore(sizeof(SPropValue),
                (LPVOID)m_lpSubRes);

        MakePropValue(lpProp,m_lpCurColumns->aulPropTag[idx] , szBuff, m_lpSubRes);

        m_lpSubRes[1].res.resProperty.lpProp = lpProp;
        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_COMPAREPROPS:
        idx = SendDlgItemMessage(IDC_PROPTAG2, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[1].res.resCompareProps.ulPropTag1 = m_lpCurColumns->aulPropTag[idx];

        idx = SendDlgItemMessage(IDC_PROPTAG22, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[1].res.resCompareProps.ulPropTag2 = m_lpCurColumns->aulPropTag[idx];

        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP2, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[1].res.resCompareProps.relop  = GetRowID("RelOp",lSelection);

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_BITMASK:
        idx = SendDlgItemMessage(IDC_PROPTAG2, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[1].res.resBitMask.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP2, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[1].res.resBitMask.relBMR  = GetRowID("Bmr",lSelection);

        SendDlgItemMessage(IDC_VALUE2, WM_GETTEXT, sizeof(szBuff),
                (LPARAM)szBuff);

//        #ifdef WIN16
//            wsscanf(szBuff, "0x%lX", &m_lpSubRes[1].res.resBitMask.ulMask);
//        #else
//            sscanf(szBuff, "0x%lX", &m_lpSubRes[1].res.resBitMask.ulMask);
//        #endif

        m_lpSubRes[1].res.resBitMask.ulMask = strtoul(szBuff,&szEnd,16);

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_SIZE:

        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP2, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[1].res.resSize.relop  = GetRowID("RelOp",lSelection);

        idx = SendDlgItemMessage(IDC_PROPTAG2, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[1].res.resSize.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        m_lpSubRes[1].res.resSize.cb = (ULONG)GetDlgItemInt(IDC_VALUE2, &fTrans , FALSE );

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_EXIST:
        idx = SendDlgItemMessage(IDC_PROPTAG2, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[1].res.resExist.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_SUBRESTRICTION:
        idx = SendDlgItemMessage(IDC_PROPTAG22, CB_GETCURSEL, 0, 0L);
        if( idx == 1 ) 
            m_lpSubRes[1].res.resSub.ulSubObject = PR_MESSAGE_RECIPIENTS;
        else
            m_lpSubRes[1].res.resSub.ulSubObject = PR_MESSAGE_ATTACHMENTS;

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_COMMENT:
        m_lpSubRes[1].res.resComment.cValues = (ULONG)GetDlgItemInt(IDC_VALUE2, &fTrans , FALSE );

        lpProp = (LPSPropValue)PvAllocMore( m_lpSubRes[1].res.resComment.cValues * sizeof(SPropValue),
                (LPVOID)m_lpSubRes);

        for( i = 0 ; i < m_lpSubRes[1].res.resComment.cValues ; i++)
            MakePropValue( &(lpProp[i]),PR_SUBJECT, "TEST STRING", m_lpSubRes);

        m_lpSubRes[1].res.resComment.lpProp = lpProp;
        m_lpRes[0].res.resAnd.cRes++;
        break;

    default:
        break;
    }

    /* Collect values from Restriction 3 group */
    switch(m_lpSubRes[2].rt = SendDlgItemMessage(IDC_RESTYPE3,
            CB_GETCURSEL, 0, 0L))
    {
    case RES_AND:
    case RES_OR:
    case RES_NOT:
        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_CONTENT:
        idx = SendDlgItemMessage(IDC_PROPTAG3, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[2].res.resContent.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP3, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[2].res.resContent.ulFuzzyLevel = GetRowID("FuzzyLevel",lSelection);

        SendDlgItemMessage(IDC_VALUE3, WM_GETTEXT, sizeof(szBuff),
                (LPARAM)szBuff);
        lpProp = (LPSPropValue)PvAllocMore(sizeof(SPropValue),
                (LPVOID)m_lpSubRes);

        MakePropValue(lpProp, PROP_TAG(PT_TSTRING, 0x0000), szBuff, m_lpSubRes);

        m_lpSubRes[2].res.resContent.lpProp = lpProp;
        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_PROPERTY:
        idx = SendDlgItemMessage(IDC_PROPTAG3, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[2].res.resProperty.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP3, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[2].res.resProperty.relop  = GetRowID("RelOp",lSelection);

        SendDlgItemMessage(IDC_VALUE3, WM_GETTEXT, sizeof(szBuff),
                (LPARAM)szBuff);
        lpProp = (LPSPropValue)PvAllocMore(sizeof(SPropValue),
                (LPVOID)m_lpSubRes);

        MakePropValue(lpProp,m_lpCurColumns->aulPropTag[idx] , szBuff, m_lpSubRes);

        m_lpSubRes[2].res.resProperty.lpProp = lpProp;
        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_COMPAREPROPS:
        idx = SendDlgItemMessage(IDC_PROPTAG3, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[2].res.resCompareProps.ulPropTag1 = m_lpCurColumns->aulPropTag[idx];

        idx = SendDlgItemMessage(IDC_PROPTAG32, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[2].res.resCompareProps.ulPropTag2 = m_lpCurColumns->aulPropTag[idx];

        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP3, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[2].res.resCompareProps.relop  = GetRowID("RelOp",lSelection);

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_BITMASK:
        idx = SendDlgItemMessage(IDC_PROPTAG3, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[2].res.resBitMask.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP3, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[2].res.resBitMask.relBMR  = GetRowID("Bmr",lSelection);

        SendDlgItemMessage(IDC_VALUE3, WM_GETTEXT, sizeof(szBuff),
                (LPARAM)szBuff);

//        #ifdef WIN16
//            wsscanf(szBuff, "0x%lX", &m_lpSubRes[2].res.resBitMask.ulMask);
//        #else
//            sscanf(szBuff, "0x%lX", &m_lpSubRes[2].res.resBitMask.ulMask);
//       #endif

        m_lpSubRes[2].res.resBitMask.ulMask = strtoul(szBuff,&szEnd,16);

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_SIZE:
        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP3, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[2].res.resSize.relop  = GetRowID("RelOp",lSelection);

        idx = SendDlgItemMessage(IDC_PROPTAG3, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[2].res.resSize.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        m_lpSubRes[2].res.resSize.cb = (ULONG)GetDlgItemInt(IDC_VALUE3, &fTrans , FALSE );

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_EXIST:
        idx = SendDlgItemMessage(IDC_PROPTAG3, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[2].res.resExist.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_SUBRESTRICTION:
        idx = SendDlgItemMessage(IDC_PROPTAG32, CB_GETCURSEL, 0, 0L);
        if( idx == 1 ) 
            m_lpSubRes[2].res.resSub.ulSubObject = PR_MESSAGE_RECIPIENTS;
        else
            m_lpSubRes[2].res.resSub.ulSubObject = PR_MESSAGE_ATTACHMENTS;
       
        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_COMMENT:
        m_lpSubRes[2].res.resComment.cValues = (ULONG)GetDlgItemInt(IDC_VALUE3, &fTrans , FALSE );

        lpProp = (LPSPropValue)PvAllocMore( m_lpSubRes[2].res.resComment.cValues * sizeof(SPropValue),
                (LPVOID)m_lpSubRes);

        for( i = 0 ; i < m_lpSubRes[2].res.resComment.cValues ; i++)
            MakePropValue( &(lpProp[i]),PR_SUBJECT, "TEST STRING", m_lpSubRes);

        m_lpSubRes[2].res.resComment.lpProp = lpProp;
        m_lpRes[0].res.resAnd.cRes++;
        break;

    default:
        break;
    }

    // display restriction
    CAcceptRestrictionDlg Res(this);
    
    Res.m_prest = m_lpRes;
    if(Res.DoModal() == IDOK)
        EndDialog(IDOK);        

}


void CResDlg::OnCancel()
{
    PvFree(m_lpSubRes);
    EndDialog(IDCANCEL);
}



/*---------------------------------*/
/* CRestriction Message Handlers   */
/*---------------------------------*/

BEGIN_MESSAGE_MAP(CAcceptRestrictionDlg, CModalDialog)
    ON_COMMAND(IDMODIFY,    OnModify)
END_MESSAGE_MAP()

void CAcceptRestrictionDlg::OnModify()
{
    EndDialog(IDCANCEL);
}

/********************************************************************/
/*
 -  CAcceptRestrictionDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *
 */
/********************************************************************/

BOOL CAcceptRestrictionDlg::OnInitDialog()
{
    int             rgTabStops[12];
    DWORD           dwReturn        = 0;      
        
    SendDlgItemMessage(IDC_ACCEPTRES,LB_RESETCONTENT,0,0);

    rgTabStops[0] = 20;
    rgTabStops[1] = 30;
    rgTabStops[2] = 40;
    rgTabStops[3] = 50;
    rgTabStops[4] = 60;
    rgTabStops[5] = 70;
    rgTabStops[6] = 80;
    rgTabStops[7] = 90;
    rgTabStops[8] = 100;
    rgTabStops[9] = 110;
    rgTabStops[10]= 120;

    dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_SETTABSTOPS,
                    (WPARAM) 11,(LPARAM)rgTabStops );

    DisplayRestriction(m_prest);

    return TRUE;    
}

/*******************************************************************/
/*
 -  CAcceptRestrictionDlg::
 -  DisplayRestriction
 *
 *  Purpose:
 *
 *  Parameters:
 *
 *  Returns:
 *
 */
/*******************************************************************/

void CAcceptRestrictionDlg::DisplayRestriction(LPSRestriction lpRes)
{
    DWORD           dwReturn        = 0;      
    char            szBuff1[256];
    char            szBuff2[256];
    char            szBuff3[256];
    char            szBuffer[1024];
    ULONG           i = 0;
    static  ULONG   cTabs = 0;
    static  char    szTabs[11][22] = {"",
                     "\t",
                     "\t\t",
                     "\t\t\t",
                     "\t\t\t\t",
                     "\t\t\t\t\t",
                     "\t\t\t\t\t\t",
                     "\t\t\t\t\t\t\t",
                     "\t\t\t\t\t\t\t\t",
                     "\t\t\t\t\t\t\t\t\t",
                     "\t\t\t\t\t\t\t\t\t\t"};

    if(!lpRes)
    {
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)(char *)"lpRestriction == NULL"); 
        return;
    }        

    switch(lpRes->rt)
    {
    case RES_CONTENT:       
        wsprintf(szBuffer,"%sSContentRestriction:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        wsprintf(szBuffer,"%sFuzzy Level: %s", szTabs[cTabs+1],
            GetString("FuzzyLevel", lpRes->res.resContent.ulFuzzyLevel, szBuff1));
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        wsprintf(szBuffer,"%s%s contains %s", szTabs[cTabs+1],
            GetString("PropTags", lpRes->res.resContent.ulPropTag, szBuff1),
            SzGetPropValue(szBuff2, lpRes->res.resContent.lpProp));
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        break;

    case RES_PROPERTY:
        wsprintf(szBuffer,"%sSPropertyRestriction:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        wsprintf(szBuffer,"%s%s %s %s", szTabs[cTabs+1],
            GetString("PropTags", lpRes->res.resProperty.ulPropTag, szBuff1),
            GetString("RelOp", lpRes->res.resProperty.relop, szBuff2),
            SzGetPropValue(szBuff3, lpRes->res.resProperty.lpProp));
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        break;

    case RES_COMPAREPROPS:
        wsprintf(szBuffer,"%sSComparePropsRestriction:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        wsprintf(szBuffer,"%s%s %s %s", szTabs[cTabs+1],
            GetString("PropTags", lpRes->res.resCompareProps.ulPropTag1, szBuff1),
            GetString("RelOp", lpRes->res.resCompareProps.relop, szBuff2),
            GetString("PropTags", lpRes->res.resCompareProps.ulPropTag2, szBuff3));        
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        break;

    case RES_BITMASK:
        wsprintf(szBuffer,"%sSBitMaskRestriction:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        wsprintf(szBuffer,"%s(%s & 0x%08lX) %s", szTabs[cTabs+1],
            GetString("PropTags", lpRes->res.resBitMask.ulPropTag, szBuff1),
            lpRes->res.resBitMask.ulMask,
            GetString("Bmr", lpRes->res.resBitMask.relBMR, szBuff2));        
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        break;

    case RES_SIZE:
        wsprintf(szBuffer,"%sSSizeRestriction:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        wsprintf(szBuffer,"%ssizeof(%s) %s %lu", szTabs[cTabs+1],
            GetString("PropTags", lpRes->res.resSize.ulPropTag, szBuff1),
            GetString("RelOp", lpRes->res.resSize.relop, szBuff2),
            lpRes->res.resSize.cb);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        break;

    case RES_EXIST:
        wsprintf(szBuffer,"%sSExistRestriction:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        wsprintf(szBuffer,"%s%s Exists", szTabs[cTabs+1],
            GetString("PropTags", lpRes->res.resExist.ulPropTag, szBuff1));       
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        break;

    case RES_SUBRESTRICTION:
        wsprintf(szBuffer,"%sSSubRestriction:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        wsprintf(szBuffer,"%s ulSubObject: %s", szTabs[cTabs+1],
            (lpRes->res.resSub.ulSubObject == PR_MESSAGE_ATTACHMENTS) 
                        ? "PR_MESSAGE_ATTACHMENTS" :
                          "PR_MESSAGE_RECIPIENTS"     );
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        cTabs++;
        DisplayRestriction(lpRes->res.resSub.lpRes);
        cTabs--;
        break;

    case RES_NOT:
        wsprintf(szBuffer,"%sNot:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        cTabs++;
        DisplayRestriction(lpRes->res.resNot.lpRes);
        cTabs--;
        break;

    case RES_AND:
        wsprintf(szBuffer,"%sAnd:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        cTabs++;
        for(i = 0; i < lpRes->res.resAnd.cRes; i++)
        {
            DisplayRestriction(&lpRes->res.resAnd.lpRes[i]);
        }
        cTabs--;
        break;

    case RES_OR:
        wsprintf(szBuffer,"%sOr:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        cTabs++;
        for(i = 0; i < lpRes->res.resOr.cRes; i++)
        {
            DisplayRestriction(&lpRes->res.resOr.lpRes[i]);
        }
        cTabs--;
        break;

    case RES_COMMENT:
        wsprintf(szBuffer,"%sSCommentRestriction:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        wsprintf(szBuffer,"%s cValues: %lu, lpPropValue: 0x%08X", szTabs[cTabs+1],
            lpRes->res.resComment.cValues,
            lpRes->res.resComment.lpProp);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        cTabs++;
        DisplayRestriction(lpRes->res.resComment.lpRes);
        cTabs--;
        break;

    default:
        wsprintf(szBuffer,"%sUNKNOWN RES TYPE lpRes->rt == %lu",szTabs[cTabs] , lpRes->rt);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        break;
    }
    return;
}

/*******************************************************************/
/*
 -  CAcceptRestrictionDlg::
 -  ~CAcceptRestrictionDlg
 -
 *  Purpose:
 *      Destructor for class CAcceptRestrictionDlg. Releases and Frees memory
 *      allocated in this class
 *
 */
/*******************************************************************/

CAcceptRestrictionDlg::~CAcceptRestrictionDlg()
{

} 



////////////////////////////////////////////////////
