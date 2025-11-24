//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Copyright 1993-1995 Microsoft Corporation, all rights reserved.
//
/* ***************************************************************************/
/* Dialer ---- Windows TAPI sample application created as an illustration of
   the usage of Windows TAPI

    Dialer does the following 3 things :

    (1) initiates/drops calls (2) handles simple TAPI request for other
    application initiating/dropping calls on their behalf (3) monitors
    incoming/outgoing calls and keeps a call log based on the user's request.

    inipref.c : contains dialer's ini file access code. */

/* ***************************************************************************/
/* include files */

#include <windows.h>
#include "dialer.h"

/* ***************************************************************************/
/* global declarations */

static char szIniFileS[13] = "dialer.ini";

/* ***************************************************************************/
/* function declarations */

int CchGetDialerProfileString(HINSTANCE hInst,int ikszSecName,int ikszFieldName,
           int ikszDefault,LPSTR lpszBuffer,int cchBuffer);
int WGetDialerProfileInt(HINSTANCE hInst,int ikszSecName,int ikszFieldName,
           int wDefault);

void SetDialerProfileString(HINSTANCE hInst,int ikszSecName,int ikszFieldName,
           LPSTR lpszVal);
void SetDialerProfileInt(HINSTANCE hInst,int ikszSecName,int ikszFieldName,
           int wVal);

void FAR GetSetINIScreenPos(HINSTANCE hInst,POINT *pptTopLeft,BOOL fGet);
void FAR GetSetCallLogWinRect(HINSTANCE hInst,HWND hwndCL,BOOL fGet);
void FAR GetSetINIQuickDial(HINSTANCE hInst,UINT ips,char *szName,char *szNum,
           BOOL fGet);
void FAR GetAllINIQuickDials(HINSTANCE hInst,HWND hwndDialer);

void FAR GetLastDialedNumbers(HINSTANCE hInst,HWND hwndDialer);
void FAR SaveLastDialedNumbers(HINSTANCE hInst,HWND hwndDialer);

/* ***************************************************************************/
/* %%Function:CchGetDialerProfileString  */
/* get the specified string from dialer.ini and return its length. */

int CchGetDialerProfileString(HINSTANCE hInst,int ikszSecName,int ikszFieldName,
           int ikszDefault,LPSTR lpszBuffer,int cchBuffer)

{
    char rgchSection[cchSzMax], rgchField[cchSzMax], rgchDefault[cchSzMax];

    LoadString(hInst,ikszSecName,rgchSection,sizeof(rgchSection));
    LoadString(hInst,ikszFieldName,rgchField,sizeof(rgchField));
    if (ikszDefault != ikszNull)
        LoadString(hInst,ikszDefault,rgchDefault,sizeof(rgchDefault));
    else
        rgchDefault[0] = 0;

    return GetPrivateProfileString(rgchSection,rgchField,rgchDefault,lpszBuffer,
            cchBuffer,szIniFileS);

} /* CchGetDialerProfileString */

/* ***************************************************************************/
/* %%Function:WGetDialerProfileInt  */
/* get the specified integer from dialer.ini and return it. */

int WGetDialerProfileInt(HINSTANCE hInst,int ikszSecName,int ikszFieldName,
           int wDefault)

{
    char rgchSection[cchSzMax], rgchField[cchSzMax];

    LoadString(hInst,ikszSecName,rgchSection,sizeof(rgchSection));
    LoadString(hInst,ikszFieldName,rgchField,sizeof(rgchField));

    return GetPrivateProfileInt(rgchSection,rgchField,wDefault,szIniFileS);

} /* WGetDialerProfileInt */

/* ***************************************************************************/
/* %%Function:SetDialerProfileString   */
/* sets lpszVal to the specified field in dialer.ini */

void SetDialerProfileString(HINSTANCE hInst,int ikszSecName,int ikszFieldName,
           LPSTR lpszVal)

{
    char rgchSection[cchSzMax], rgchField[cchSzMax];

    LoadString(hInst,ikszSecName,rgchSection,sizeof(rgchSection));
    LoadString(hInst,ikszFieldName,rgchField,sizeof(rgchField));
    WritePrivateProfileString(rgchSection,rgchField,lpszVal,szIniFileS);

} /* SetDialerProfileString */

/* ***************************************************************************/
/* %%Function:SetDialerProfileInt   */
/* sets wVal to the specified field in dialer.ini */

void SetDialerProfileInt(HINSTANCE hInst,int ikszSecName,int ikszFieldName,
           int wVal)

{
    char szBuffer[32];

    _itoa(wVal,szBuffer,10);
    SetDialerProfileString(hInst,ikszSecName,ikszFieldName,szBuffer);

} /* SetDialerProfileInt */

/* ***************************************************************************/
/* %%Function:GetSetINIPoint   */
/* gets/sets the point specified by *pptfrom/to the ini file. gets iff fGet is
   TRUE. *ppt contains the default if fGet. */

void FAR GetSetINIPoint(HINSTANCE hInst,int ikszSecName,int ikszFieldName,
           POINT *ppt,BOOL fGet)
{
    char szPt[32];
    int ichComma;

    if (!fGet)
        {
        wsprintf(szPt,"%d, %d",ppt->x,ppt->y);
        SetDialerProfileString(hInst,ikszSecName,ikszFieldName,szPt);
        return;
        } /* if */

    CchGetDialerProfileString(hInst,ikszSecName,ikszFieldName,ikszNull,szPt,
            sizeof(szPt));
    if (!szPt[0])
        return;
    ichComma = strcspn(szPt,",");

    szPt[ichComma] = 0;
    ppt->x = atoi(szPt);

    if (*(szPt+ichComma+1))
        ppt->y = atoi(szPt+ichComma+1);

} /* GetSetINIPoint */

/* ***************************************************************************/
/* %%Function:GetSetINIScreenPos   */
/* gets/sets the top-left point of the main Window from the INI file. gets iff
   fGet is TRUE. */

void FAR GetSetINIScreenPos(HINSTANCE hInst,POINT *pptTopLeft,BOOL fGet)
{
    if (fGet)
        {
        pptTopLeft->x = 100;
        pptTopLeft->y = 100;
        } /* if */
    GetSetINIPoint(hInst,ikszSecPreference,ikszFieldDialerWndLT,pptTopLeft,
            fGet);

} /* GetSetINIScreenPos */

/* ***************************************************************************/
/* %%Function:GetSetCallLogWinRect   */
/* gets/sets the rect of the call logging window from the INI file. gets iff
   fGet is TRUE. */

void FAR GetSetCallLogWinRect(HINSTANCE hInst,HWND hwndCL,BOOL fGet)
{
    RECT rcWin;
    POINT pt;

    if (!hwndCL)
        return;

    GetWindowRect(hwndCL,&rcWin);
    pt.x = rcWin.right - rcWin.left;
    pt.y = rcWin.bottom - rcWin.top;

    GetSetINIPoint(hInst,ikszSecCallLogging,ikszFieldCLWinPos,(POINT *)&rcWin,
            fGet);
    GetSetINIPoint(hInst,ikszSecCallLogging,ikszFieldCLWinDimension,&pt,fGet);

    if (fGet)
        {
        ScreenToClient(GetParent(hwndCL),(POINT *)&rcWin);
        MoveWindow(hwndCL,rcWin.left,rcWin.top,pt.x,pt.y,TRUE);
        } /* if */

} /* GetSetCallLogWinRect */

/* ***************************************************************************/
/* %%Function:GetSetINIQuickDial   */
/* gets/sets the (name,number) sets associated with the "Quick Dial" buttons
   from the INI file. gets iff fGet is TRUE. we are assuming the size of
   szName and szNum to be cchSzMax-1. */

void FAR GetSetINIQuickDial(HINSTANCE hInst,UINT ips,char *szName,char *szNum,
           BOOL fGet)
{
    if (!fGet)
        {// make the data (name & number) into a delimited entry
        if (szName && *szName)
            SetDialerProfileString(hInst,ikszSecSpeedDialSettings,
                    ikszFieldSDName1+ips,szName);
        else if (szNum)
            SetDialerProfileString(hInst,ikszSecSpeedDialSettings,
                    ikszFieldSDName1+ips,szNum);

        if (szNum)
            SetDialerProfileString(hInst,ikszSecSpeedDialSettings,
                    ikszFieldSDNumber1+ips,szNum);

        /* flush cached info. */
        WritePrivateProfileString(NULL,NULL,NULL,szIniFileS);
        } /* if */
    else
        {
        if (szName)
            CchGetDialerProfileString(hInst,ikszSecSpeedDialSettings,
                    ikszFieldSDName1+ips,ikszNull,szName,cchSzMax-1);
        if (szNum)
            CchGetDialerProfileString(hInst,ikszSecSpeedDialSettings,
                    ikszFieldSDNumber1+ips,ikszNull,szNum,cchSzMax-1);
        } /* else */

} /* GetSetINIQuickDial */

/* ***************************************************************************/
/* %%Function:GetAllINIQuickDials   */
/* reads in all the quick dial (name,number) sets associated with the "Quick
   Dial" buttons from the INI file and associates the names to the
   corresponding buttons.

*/

VOID FAR GetAllINIQuickDials(HINSTANCE hInst,HWND hwndDialer)
{
    WORD ips;
    char szName[cchSzMax];
    char szTxt[cchSzMax];

    for (ips = 0; ips <= didDialerBtnSpeedDialLast-didDialerBtnSpeedDial1;
             ++ips)
        {
        GetSetINIQuickDial(hInst,ips,szName,szTxt,TRUE);
        DoubleUpAmpersandsInSz(szName,cchSzMax);
        SetDlgItemText(hwndDialer,didDialerBtnSpeedDial1+ips,(LPCSTR) szName);
        } /* for */

} /* GetAllINIQuickDials */

/* ***************************************************************************/
/* %%Function:GetLastDialedNumbers   */
/* reads in the last 10 dialed numbers from the ini file and inserts them into
   the combo box didDialerComboNumDial. */

VOID FAR GetLastDialedNumbers(HINSTANCE hInst,HWND hwndDialer)
{
    WORD iksz;
    char szNumber[cchSzMax];

    for (iksz = ikszFieldLastDialedNumberFirst;
             iksz <= ikszFieldLastDialedNumberLast; ++iksz)
        {
        CchGetDialerProfileString(hInst,ikszSecLastDialNumber,iksz,ikszNull,
                szNumber,sizeof(szNumber));
        if (szNumber[0])
            SendDlgItemMessage(hwndDialer,didDialerComboNumDial,CB_ADDSTRING,0,
                    (LPARAM)(LPCSTR)szNumber);
        } /* for */

} /* GetLastDialedNumbers */


/* ***************************************************************************/
/* %%Function:SaveLastDialedNumbers   */
/* save the last 10 dialed numbers into the ini file. */

VOID FAR SaveLastDialedNumbers(HINSTANCE hInst,HWND hwndDialer)

{
    WORD iksz, cItem;
    char szNumber[cchSzMax];

    cItem = (WORD)SendDlgItemMessage(hwndDialer,didDialerComboNumDial,
            CB_GETCOUNT,0,0);
    for (iksz = ikszFieldLastDialedNumberFirst;
             iksz <= ikszFieldLastDialedNumberLast; ++iksz)
        {
        if (iksz - ikszFieldLastDialedNumberFirst < cItem)
            SendDlgItemMessage(hwndDialer,didDialerComboNumDial,CB_GETLBTEXT,
                    iksz-ikszFieldLastDialedNumberFirst,
                    (LPARAM)(LPCSTR)szNumber);
        else
            szNumber[0] = 0;
        SetDialerProfileString(hInst,ikszSecLastDialNumber,iksz,szNumber);
        } /* for */

} /* SaveLastDialedNumbers */
