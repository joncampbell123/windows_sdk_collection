/*********************************************************************/
/*
 -  iid.h
 -  Copyright (C) 1995 Microsoft Corporation
 -
 *  Purpose:
 *      Defines id's, function prototypes, classes for Interface Dialog class
 *
 */
/*********************************************************************/

#ifndef __iid_h_
#define __iid_h_


#include "propvu.h"



typedef struct _testiid
{
    LPIID       lpInterface;
    LPSTR       lpsz;

} TESTIID, *LPTESTIID;


static TESTIID AllIIDs[]=
{
    ((LPIID)(&PS_MAPI)),                    "PS_MAPI",
    ((LPIID)(&PS_PUBLIC_STRINGS)),          "PS_PUBLIC_STRINGS",
    ((LPIID)(&IID_IPersistMessage)),        "IID_IPersistMessage",
    ((LPIID)(&IID_IMAPIViewAdviseSink)),    "IID_IMAPIViewAdviseSink",
    ((LPIID)(&IID_IStreamDocfile)),         "IID_IStreamDocfile",
    ((LPIID)(&IID_IMAPIFormProp)),          "IID_IMAPIFormProp",
    ((LPIID)(&IID_IMAPIFormContainer)),     "IID_IMAPIFormContainer",
    ((LPIID)(&IID_IMAPIFormAdviseSink)),    "IID_IMAPIFormAdviseSink",
    ((LPIID)(&IID_IStreamTnef)),            "IID_IStreamTnef",
    ((LPIID)(&IID_IMAPIMessageSite)),       "IID_IMAPIMessageSite",
    ((LPIID)(&IID_IUnknown)),               "IID_IUnknown",
    ((LPIID)(&IID_IMAPISession)),           "IID_IMAPISession",
    ((LPIID)(&IID_IMAPITable)),             "IID_IMAPITable",
    ((LPIID)(&IID_IMAPIAdviseSink)),        "IID_IMAPIAdviseSink",
    ((LPIID)(&IID_IMAPIControl)),           "IID_IMAPIControl",
    ((LPIID)(&IID_IProfAdmin)),             "IID_IProfAdmin",
    ((LPIID)(&IID_IMsgServiceAdmin)),       "IID_IMsgServiceAdmin",
    ((LPIID)(&IID_IProviderAdmin)),         "IID_IProviderAdmin",
    ((LPIID)(&IID_IMAPIProgress)),          "IID_IMAPIProgress",    
    ((LPIID)(&IID_IMAPIProp)),              "IID_IMAPIProp",
    ((LPIID)(&IID_IProfSect)),              "IID_IProfSect",
    ((LPIID)(&IID_IMAPIStatus)),            "IID_IMAPIStatus",
    ((LPIID)(&IID_IMsgStore)),              "IID_IMsgStore",
    ((LPIID)(&IID_IMessage)),               "IID_IMessage",
    ((LPIID)(&IID_IAttachment)),            "IID_IAttachment",
    ((LPIID)(&IID_IAddrBook)),              "IID_IAddrBook",
    ((LPIID)(&IID_IMailUser)),              "IID_IMailUser",
    ((LPIID)(&IID_IMAPIContainer)),         "IID_IMAPIContainer",
    ((LPIID)(&IID_IMAPIFolder)),            "IID_IMAPIFolder",
    ((LPIID)(&IID_IABContainer)),           "IID_IABContainer",
    ((LPIID)(&IID_IDistList)),              "IID_IDistList",
    ((LPIID)(&IID_IMAPISup)),               "IID_IMAPISup",
    ((LPIID)(&IID_IMSProvider)),            "IID_IMSProvider",
    ((LPIID)(&IID_IABProvider)),            "IID_IABProvider",
    ((LPIID)(&IID_IXPProvider)),            "IID_IXPProvider",
    ((LPIID)(&IID_IMSLogon)),               "IID_IMSLogon",
    ((LPIID)(&IID_IABLogon)),               "IID_IABLogon",
    ((LPIID)(&IID_IXPLogon)),               "IID_IXPLogon",
    ((LPIID)(&IID_IMAPITableData)),         "IID_IMAPITableData",
    ((LPIID)(&IID_IMAPISpoolerInit)),       "IID_IMAPISpoolerInit",
    ((LPIID)(&IID_IMAPISpoolerSession)),    "IID_IMAPISpoolerSession",
    ((LPIID)(&IID_ITNEF)),                  "IID_ITNEF",
    ((LPIID)(&IID_IMAPIPropData)),          "IID_IMAPIPropData",
    ((LPIID)(&IID_ISpoolerHook)),           "IID_ISpoolerHook",
    ((LPIID)(&IID_IMAPISpoolerService)),    "IID_IMAPISpoolerService",
    ((LPIID)(&IID_IMAPIViewContext)),       "IID_IMAPIViewContext",
    ((LPIID)(&IID_IMAPIFormMgr)),           "IID_IMAPIFormMgr",
    ((LPIID)(&IID_IMAPIForm)),              "IID_IMAPIForm",
};


// this is the number of elements in TESTIID AllIIDs
#define   NUM_INTERFACES    48

/*********************************************************************/
/*
 -  CInterfaceDlg Class
 -
 *  Purpose:
 *      Defines the controls for the Interface Property Dialog
 *
 */
/*********************************************************************/

class CInterfaceDlg : public CModalDialog
{
public:

    IID                 m_rgiidSelect[NUM_INTERFACES];
    ULONG               m_ciidSelect;
    char                *m_rgszSelected[NUM_INTERFACES];

    CInterfaceDlg(CWnd  *pParentWnd)
        : CModalDialog(IIDBLD, pParentWnd)
    {
        m_ciidSelect    = 0;        
    }

    ~CInterfaceDlg();   

    BOOL OnInitDialog();
            
    void OnAddInterface();
    void OnRemoveInterface();
    void DisplayInterfaces();

    DECLARE_MESSAGE_MAP();

};
 

                       

#endif



