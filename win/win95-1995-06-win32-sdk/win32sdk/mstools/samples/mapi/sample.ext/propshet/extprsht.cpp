///////////////////////////////////////////////////////////////////////////////
//
//  File Name
//      EXTPRSHT.CPP 
//
//  Description
//      Implementation of MyExchExt and MyExchExtPropertySheets along
//      with helper functions towards the end.
//
//      IExchExt interface methods:
//          MyExchExt::QueryInterface()
//          MyExchExt::AddRef()
//          MyExchExt::Release()
//
//          MyExchExt::Install()
//
//      IExchExtPropertySheets interface methods:
//          IExchExtPropertySheets::QueryInterface()
//          IExchExtPropertySheets::AddRef()
//          IExchExtPropertySheets::Release()
//
//          IExchExtPropertySheets::GetMaxPageCount()
//          IExchExtPropertySheets::GetPages()
//          IExchExtPropertySheets::FreePages()
//
//  Author
//      Gary Peluso
//
//  Revision: 1.00
//
// Written for Microsoft Windows Developer Support
// Copyright (c) 1992-1995 Microsoft Corporation. All rights reserved.
//
#define INITGUID
#define USES_IID_IExchExt
#define USES_IID_IExchExtAdvancedCriteria
#define USES_IID_IExchExtAttachedFileEvents
#define USES_IID_IExchExtCommands
#define USES_IID_IExchExtMessageEvents
#define USES_IID_IExchExtPropertySheets
#define USES_IID_IExchExtSessionEvents
#define USES_IID_IExchExtUserEvents
#define USES_IID_IMAPIFolder
#define USES_PS_PUBLIC_STRINGS

#include "EXTPRSHT.H"

#include <INITGUID.H>
#include <MAPIGUID.H>



///////////////////////////////////////////////////////////////////////////////
//  tag enumeration definitions and arrays
//
// 
 enum { MESSAGE_CLASS, OBJECT_TYPE, OBJPROPTAGS };

static const SizedSPropTagArray(OBJPROPTAGS,  ObjPropTags) =
      { OBJPROPTAGS, 
        { 
         PR_MESSAGE_CLASS,
         PR_OBJECT_TYPE,        
        } 
      };

 enum { LASTEDIT, MORETAGS };

static const SizedSPropTagArray(MORETAGS,  MoreObjTags) =
      { MORETAGS, 
        { 
         PR_LAST_MODIFICATION_TIME,
        } 
      };


///////////////////////////////////////////////////////////////////////////////
//    Office Document Named Properties
//

enum { KEYWORDS, COMMENTS, AUTHOR, COMPANY, 
       APPNAME, CATEGORY, TITLE, DOCSUBJECT, NAMEIDS};

LPWSTR awszDocProperties[] = { L"Keywords", L"Comments", L"Author", L"Company", 
                               L"AppName",  L"Category", L"Title", L"Subject"};


ULONG ulOfficePropertiesBitmap = 0;     // bitmap of present office document properties
#define OPBM_KEYWORDS     0x00000001
#define OPBM_COMMENTS     0x00000002
#define OPBM_AUTHOR       0x00000004
#define OPBM_COMPANY      0x00000008
#define OPBM_APPNAME      0x00000010
#define OPBM_CATEGORY     0x00000020
#define OPBM_TITLE        0x00000040
#define OPBM_DOCSUBJECT   0x00000080

// array of property tags for named properties
SizedSPropTagArray(NAMEIDS, pDocPropTags);

// arrays for summary sheet properties  
LPSPropValue pSummaryProps = NULL;
LPSPropValue pMoreDocProps = NULL;

static HINSTANCE ghInstDLL = NULL;  // instance handle of DLL

///////////////////////////////////////////////////////////////////////////////
//    FUNCTION: DLLMain()
//
//    Purpose
//    Do initilization processesing
//
//    Return Value
//    TRUE - DLL successfully loads and LoadLibrary will succeed.
//    FALSE - will cause an Exchange error message saying it cannot locate
//            the extension DLL.
//
//    Comments
//    We only need to get a copy of the DLL's HINSTANCE.
//
BOOL WINAPI DllMain(
    HINSTANCE  hinstDLL,
    DWORD  fdwReason,   
    LPVOID  lpvReserved) 
{
 if (DLL_PROCESS_ATTACH == fdwReason)
 {
    ghInstDLL = hinstDLL;
 }
 return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
//    FUNCTION: ExchEntryPoint
//
//    Parameters - none
//
//    Purpose
//    The entry point called by Exchange.
//
//    Return Value
//    Pointer to Exchange Extension (IExchExt) interface
//
//    Comments
//    Exchange Client calls this for each context entry.  
//

LPEXCHEXT CALLBACK ExchEntryPoint(void)
{
        
     return new MyExchExt;


}


///////////////////////////////////////////////////////////////////////////////
//    MyExchExt::MyExchExt()
//
//    Parameters - none
//
//    Purpose
//    Comstructor.  Called during instantiation of MyExchExt object.
//
//

MyExchExt::MyExchExt()
{
  m_cRef = 1; 
  m_context = 0;
  
  m_pExchExtPropertySheets = new MyExchExtPropertySheets;

};



///////////////////////////////////////////////////////////////////////////////
//  IExchExt virtual member functions implementation
//

///////////////////////////////////////////////////////////////////////////////
//    MyExchExtPropertySheets::Release()
//
//    Parameters - none
//
//    Purpose
//    Frees memory when interface is not referenced any more
//
//    Return value
//    reference count of interface
//

STDMETHODIMP_(ULONG) MyExchExt::Release() 
{ 
 ULONG ulCount = --m_cRef;
                         
 if (!ulCount) 
 { 
  pDocPropTags.cValues = 0;
  delete this;
 }

return ulCount;

}

///////////////////////////////////////////////////////////////////////////////
//    MyExchExt::QueryInterface()
//
//    Parameters
//    riid   -- Interface ID.
//    ppvObj -- address of interface object pointer.
//
//    Purpose
//    Returns a pointer to an interface object that is requested by ID.
//
//    Comments
//    The interfaces are requested everytime a new context is entered.  The 
//    IID_IExchExt* interfaces are ignored if not specified in the Exchange
//    extensions registry.
//
//    If an interface pointer is returned for more than one context, that
//    interface is used by the client for each of those contexts.  Check the
//    current context to verify if it is appropriate to pass back an interface
//    pointer.
//    
//    Return Value - none
//

STDMETHODIMP MyExchExt::QueryInterface(REFIID riid, LPVOID FAR * ppvObj)          
{
    HRESULT hResult = S_OK;

    *ppvObj = NULL;

    if (( IID_IUnknown == riid) || ( IID_IExchExt == riid) )
    {
        *ppvObj = (LPUNKNOWN)this;
    }
    else if (IID_IExchExtPropertySheets == riid)
    {
        *ppvObj = (LPUNKNOWN) m_pExchExtPropertySheets;
    }
    else
        hResult = E_NOINTERFACE;

    if (NULL != *ppvObj)
        ((LPUNKNOWN)*ppvObj)->AddRef();

    return hResult;
}



///////////////////////////////////////////////////////////////////////////////
//    MyExchExt::Install()
//
//    Parameters
//    peecb     -- pointer to Exchange Extension callback function
//    eecontext -- context code at time of being called.
//
//    Purpose
//    Called once for each new context that is entered.
//
//    Return Value
//    S_OK - the installation succeeded for the context
//    S_FALSE - deny the installation fo the extension for the context
//
STDMETHODIMP MyExchExt::Install(LPEXCHEXTCALLBACK peecb, ULONG eecontext, ULONG ulFlags)
{
    ULONG ulBuildVersion;
    HRESULT hr;

    m_context = eecontext;
       
    // make sure this is the right version 
    peecb->GetVersion(&ulBuildVersion, EECBGV_GETBUILDVERSION);
    if (EECBGV_BUILDVERSION_MAJOR != (ulBuildVersion & 
                                      EECBGV_BUILDVERSION_MAJOR_MASK))
        return S_FALSE;


    switch (eecontext)
    {
     case EECONTEXT_PROPERTYSHEETS:
        hr = S_OK;
        break;

     default:
        hr = S_FALSE;
        break;
    }


    return hr;

}

///////////////////////////////////////////////////////////////////////////////
//  IExchExtPropertySheets virtual member functions implementation
//

///////////////////////////////////////////////////////////////////////////////
//    MyExchExtPropertySheets::QueryInterface()
//
//    Parameters
//    riid   -- Interface ID.
//    ppvObj -- address of interface object pointer.
//
//    Purpose
//    Return interface object upon request
//    
//    Return Value - none
//
//    Comments
//    Currently the Exchange client does not call QueryInterface from any object
//    except for IExchExt.  This is implemented in case features are added to
//    Exchange to require QueryInterface from any object.  Also, as a "rule of
//    OLE COM" this is the proper implementation of QueryInterface.
//

STDMETHODIMP MyExchExtPropertySheets::QueryInterface(REFIID riid, LPVOID FAR * ppvObj)          
{   

    *ppvObj = NULL;
    if ((riid == IID_IExchExtPropertySheets) || (riid == IID_IUnknown))
    {
        *ppvObj = (LPVOID)this;
        // Increase usage count of this object
        AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;

}


///////////////////////////////////////////////////////////////////////////////
//    MyExchExtPropertySheets::GetMaxPageCount()
//
//    Parameters
//    ulFlags -- a bitmask indicating what type of property sheet is being 
//               displayed
//
//    Purpose
//    Returns the number of property pages which are to be added.
//
//    Return Value - maximum number of custom pages for the property sheet
//
//    Exchange Calls this to know how many PROPSHEETPAGE buffers it needs
//    to allocate.
//

ULONG MyExchExtPropertySheets::GetMaxPageCount(ULONG ulFlags)          
{
 ULONG ulNumExtSheets;

    switch (ulFlags)
    {
     // ignore these objects.
     case EEPS_FOLDER:
     case EEPS_STORE:
     case EEPS_TOOLSOPTIONS:
        ulNumExtSheets = 0;
        break;
         
     case EEPS_MESSAGE:
        ulNumExtSheets = 1;
        break;

     default:
        ulNumExtSheets = 0;
        break;
    }

    return ulNumExtSheets;   // adding one propery page
}


///////////////////////////////////////////////////////////////////////////////
//    MyExchExtPropertySheets::GetPages()
//
//    Parameters
//    peecb   -- pointer to Exchange callback interface
//    ulFlags -- a bitmask indicating what type of property sheet is being 
//               displayed
//    ppsp    -- output parm pointing to pointer to list of property sheets
//    pcpsp   -- output parm pointing to buffer contaiing number of property
//               sheets actually used.
//
//    Purpose
//    Fills the PROPSHEETPAGE members for the custom property page.
//
//    Return Value
//    S_FALSE - do not add a new page
//    S_OK - use the ppsp information for new pages.
//
//    Comments
//    Exchange calls this method to gather information for any custom
//    property pages to be added to the sheet.  Here we are only adding
//    one.  ppsp may be an array of PROPSHEETPAGE structures to allow you
//    to all multiple property pages.
//

STDMETHODIMP MyExchExtPropertySheets::GetPages(LPEXCHEXTCALLBACK peecb,
                      ULONG ulFlags, LPPROPSHEETPAGE ppsp, ULONG FAR * pcpsp)
{
 HRESULT hr;
 LPMDB pMDB = NULL;
 LPMESSAGE pItem = NULL;

    *pcpsp = 0;

    hr = peecb->GetObject(&pMDB, (LPMAPIPROP *)&pItem);
    if (pItem == NULL)
    {
        ErrorMessageBox(NULL, hr, "GetPages", "GetObject failed");
        return hr;
    }

    if (!ValidDocObject(pItem))
    {
      // do nothing
      return S_OK;
    }

    if (!GetSummarySheetInfo(pItem))
    {
        ErrorMessageBox(NULL, 0, "GetPages", "Could not get summary sheet information");
        return E_FAIL;
    }


    // fill out members for the property page
    ppsp[0].dwSize = sizeof (PROPSHEETPAGE);
    ppsp[0].dwFlags = PSP_DEFAULT;
    ppsp[0].hInstance = ghInstDLL;
    ppsp[0].pszTemplate = MAKEINTRESOURCE(IDD_SUMMARY);
    ppsp[0].hIcon = NULL;     // not used in this sample
    ppsp[0].pszTitle = NULL;  // not used in this sample
    ppsp[0].pfnDlgProc = (DLGPROC)SummaryPageDlgProc;
    ppsp[0].lParam = 0;     
    ppsp[0].pfnCallback = NULL;
    ppsp[0].pcRefParent = NULL; // not used in this sample

    *pcpsp = 1;

    return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
//    MyExchExtPropertySheets::FreePages()
//
//    Parameters
//    ppsp -- pointer to a pointer to the first of a list of property pages
//    cpsp -- number of custom property pages in the list
//    ulFlags -- type of property page
//    
//    Purpose
//    Free any memory associated to the property sheet.
//
//    Return Value - none
//
//    Comments
//    No parameters are used in this example but the function is used as
//    a signal that the property sheet is going away and so memory may
//    be freed.
//
                         
VOID MyExchExtPropertySheets::FreePages(LPPROPSHEETPAGE ppsp, ULONG ulFlags, 
                                        ULONG cpsp)          
{

    if (pSummaryProps != NULL)
    {
        MAPIFreeBuffer(pSummaryProps);
        pSummaryProps = NULL;
    }

}


///////////////////////////////////////////////////////////////////////////////
//  Helper Functions
//

///////////////////////////////////////////////////////////////////////////////
//    ErrorMessageBox()
//
//    Parameters
//    hWnd       -- parent window
//    hr         -- HRESULT value
//    szFunction -- function name in which the error occurred
//    szMessage  -- error message
//
//    Purpose
//    Displays an error message using MessageBox
//
//    Return Value - none
//
//    Comments
//    Pass 0 for hr to not display an error number.  Pass NULL for
//    szFunction to not display a function name.  Use these options
//    to display error messages for public consumption.  Use hr and
//    function name for internal errors and for debugging.  szMessage
//    is mandatory.
//
void ErrorMessageBox(HWND hWnd, HRESULT hr, LPSTR szFunction, LPSTR szMessage)
{
 static char szError[256];

 if (szMessage == NULL)
 {
    MessageBox(hWnd, 
         "An unknown error occured in\nSample property sheet extension",
         "Sample Property Sheet Extension", MB_ICONEXCLAMATION | MB_OK);
    return;
 }

 if ((hr == 0) && (szFunction == NULL))
 {
    MessageBox(hWnd, szMessage, "Sample Extension Error", MB_ICONEXCLAMATION | MB_OK);
    return;
 }


 if (szFunction != NULL) 
 {
  wsprintf(szError, "Error %08X in %s\n%s", hr, szFunction, szMessage);
  MessageBox(hWnd, szError, "Sample Extension Error", MB_ICONEXCLAMATION | MB_OK);
 }

}



///////////////////////////////////////////////////////////////////////////////
//    GetSummarySheetInfo()
//
//    Parameters
//    pMessage -- pointer to an Office Document object, message object of
//                class IPM.Document
//
//    Purpose
//    gathers the summary sheet information from the properties of a message
//
//    Return Value
//    TRUE if everything succeeded
//    FALSE something failed, inform the caller
//
//    Comments
//    The pointers to the buffers are defined in the PRSHTDLG.CPP file and are
//    externally referenced here.
//
//    pDocPropTags is allocated memory for property tags by GetNamedIDs
//    Only need to call this function once when tags are first needed.
//    The buffer for the tags are freed in the Release method of MyExchExt when
//    Exchange is closing down.
//

BOOL GetSummarySheetInfo(LPMESSAGE pMessage)
{
 HRESULT hr;
 ULONG ulcValues;
 BOOL bSuccess;
 UINT p;

 bSuccess = TRUE;

 if (pMessage == NULL)
 {
    ErrorMessageBox(NULL, 0, "GetSummarySheetInfo", "Null pMessage");
    return FALSE;
 }

 GetNamedIDs(pMessage);
 if (pDocPropTags.cValues == 0)
 {
    ErrorMessageBox(NULL, 0, "GetSummarySheetInfo", "GetNamedIDs failed");
    bSuccess = FALSE;
    goto error_return;
 }       


 hr = pMessage->GetProps((LPSPropTagArray)&pDocPropTags, 0, &ulcValues, &pSummaryProps);
 if ((FAILED(hr)) || (ulcValues == 0))
 {
    ErrorMessageBox(NULL, hr, "GetSummarySheetInfo", "GetProps failed");
    bSuccess = FALSE;
    goto error_return;
 }


// These must be validated in the same order as the named id property tag array.
// See GetNamedIDs below for more information.
p=0;
if ( (OPBM_KEYWORDS & ulOfficePropertiesBitmap) && 
      (pSummaryProps[p].Value.err != MAPI_E_NOT_FOUND) )
{
    mvszKeywords = pSummaryProps[p++].Value.MVszA;
}
else
{
    char szDefault[] = "No Keywords";
    szDocKeywords = (LPSTR)malloc(lstrlen(szDefault) + 1);
    lstrcpy (szDocKeywords, szDefault);
    mvszKeywords.cValues = 0;
}


if ( (OPBM_COMMENTS & ulOfficePropertiesBitmap) && 
      (pSummaryProps[p].Value.err != MAPI_E_NOT_FOUND) )
    szDocComments = pSummaryProps[p++].Value.lpszA;
else
    szDocComments = "No Comments";


if ( (OPBM_AUTHOR & ulOfficePropertiesBitmap) && 
      (pSummaryProps[p].Value.err != MAPI_E_NOT_FOUND) )
    szDocAuthor = pSummaryProps[p++].Value.lpszA;
else
    szDocAuthor = "No Author";

if ( (OPBM_COMPANY & ulOfficePropertiesBitmap) && 
      (pSummaryProps[p].Value.err != MAPI_E_NOT_FOUND) )
    szDocCompany = pSummaryProps[p++].Value.lpszA;
else
    szDocCompany = "No Company";

if ( (OPBM_APPNAME & ulOfficePropertiesBitmap) && 
      (pSummaryProps[p].Value.err != MAPI_E_NOT_FOUND) )
    szDocApplication = pSummaryProps[p++].Value.lpszA;
else
    szDocApplication = "No Application";

if ( (OPBM_CATEGORY & ulOfficePropertiesBitmap) && 
      (pSummaryProps[p].Value.err != MAPI_E_NOT_FOUND) )
    szDocCategory = pSummaryProps[p++].Value.lpszA;
else
    szDocCategory = "No Category";

if ( (OPBM_TITLE & ulOfficePropertiesBitmap) && 
      (pSummaryProps[p].Value.err != MAPI_E_NOT_FOUND) )
    szDocTitle = pSummaryProps[p++].Value.lpszA;
else
    szDocTitle = "No Title";

if ( (OPBM_DOCSUBJECT & ulOfficePropertiesBitmap) && 
      (pSummaryProps[p].Value.err != MAPI_E_NOT_FOUND) )
    szDocSubject = pSummaryProps[p++].Value.lpszA;
else
    szDocSubject = "No Subject";

hr = pMessage->GetProps((LPSPropTagArray)&MoreObjTags, 0, &ulcValues, &pMoreDocProps);
if ((FAILED(hr)) || (ulcValues == 0))
{
    ErrorMessageBox(NULL, hr, "GetSummarySheetInfo", "GetProps failed");
    bSuccess = FALSE;
    goto error_return;
}

if (pMoreDocProps[LASTEDIT].Value.err == MAPI_E_NOT_FOUND)
    szDocLastSaved = "No last saved time";
else
    szDocLastSaved = GetFileTimeString(&(pMoreDocProps[LASTEDIT].Value.ft));

error_return:

return bSuccess;

}

///////////////////////////////////////////////////////////////////////////////
//    GetNamedIDs()
//
//    Parameters
//    pMessage -- message object of which to call GetIDsFromNames()
//
//    Purpose
//    This function retrieves the property IDs of names from Office Document
//    summary sheet properties.
//
//    Return Value - none
//
//    Comments
//    The name strings used were found imperically because there were no
//    documentation publishing the Office document named properties at the
//    time this sample was implemented.  It is possible to get a list
//    of all public named properties by calling GetNamesFromIDs().
//
//    pDocPropTags is a global variable listed at the top of this file.
//    The property tag array is not freed until Exchange is closing down
//    since we want to reuse these tags over and over per session.
//
void GetNamedIDs(LPMESSAGE pMessage)
{
 HRESULT hr;
 LPMAPINAMEID apMAPINameId[NAMEIDS];
 ULONG i, p;
 LPSPropTagArray pTags = NULL;

 if (pMessage == NULL)
 {
  ErrorMessageBox(NULL, 0, "GetNamedIDs", "Null pMessage");
 }

 memset(apMAPINameId, 0, NAMEIDS * sizeof(LPVOID));

 // create an array of property names
 for (i=0; i<NAMEIDS; i++)
 {
   hr = MAPIAllocateBuffer(sizeof(MAPINAMEID), (LPVOID *)&apMAPINameId[i]);
   if (FAILED(hr))
   {
     ErrorMessageBox(NULL, hr, "GetNamedIDs", "Memory Allocation Error");
     goto error_return;
   }

   apMAPINameId[i]->lpguid = (LPGUID)&PS_PUBLIC_STRINGS;
   apMAPINameId[i]->ulKind = MNID_STRING;
   apMAPINameId[i]->Kind.lpwstrName = awszDocProperties[i];

 }


 // get the named ids and store in global pointer pDocPropTags;
 hr = pMessage->GetIDsFromNames(NAMEIDS, apMAPINameId, 0, &pTags);
 if (FAILED(hr))
 {
   pTags = NULL;
   ErrorMessageBox(NULL, hr, "GetNamedIDs", "GetIDsFromNames failed");
   goto error_return;
 }


 // filter out the named id errors.  Keep track of which were found
 // and which weren't using a bitmap.  Order is important as 
 // GetSummarySheetInfo depends on it.
 p=0;
 ulOfficePropertiesBitmap = 0;

 if (PT_ERROR != PROP_TYPE(pTags->aulPropTag[KEYWORDS]))
 {
   pDocPropTags.aulPropTag[p++] = pTags->aulPropTag[KEYWORDS];
   ulOfficePropertiesBitmap |= OPBM_KEYWORDS;
 }

 if (PT_ERROR != PROP_TYPE(pTags->aulPropTag[COMMENTS]))
 {
   pDocPropTags.aulPropTag[p++] = pTags->aulPropTag[COMMENTS];
   ulOfficePropertiesBitmap |= OPBM_COMMENTS;
 }

 if (PT_ERROR != PROP_TYPE(pTags->aulPropTag[AUTHOR]))
 {
   pDocPropTags.aulPropTag[p++] = pTags->aulPropTag[AUTHOR];
   ulOfficePropertiesBitmap |= OPBM_AUTHOR;
 }

 if (PT_ERROR != PROP_TYPE(pTags->aulPropTag[COMPANY]))
 {
   pDocPropTags.aulPropTag[p++] = pTags->aulPropTag[COMPANY];
   ulOfficePropertiesBitmap |= OPBM_COMPANY;
 }

 if (PT_ERROR != PROP_TYPE(pTags->aulPropTag[APPNAME]))
 {
   pDocPropTags.aulPropTag[p++] = pTags->aulPropTag[APPNAME];
   ulOfficePropertiesBitmap |= OPBM_APPNAME;
 }

 if (PT_ERROR != PROP_TYPE(pTags->aulPropTag[CATEGORY]))
 {
   pDocPropTags.aulPropTag[p++] = pTags->aulPropTag[CATEGORY];
   ulOfficePropertiesBitmap |= OPBM_CATEGORY;
 }

 if (PT_ERROR != PROP_TYPE(pTags->aulPropTag[TITLE]))
 {
   pDocPropTags.aulPropTag[p++] = pTags->aulPropTag[TITLE];
   ulOfficePropertiesBitmap |= OPBM_TITLE;
 }

 if (PT_ERROR != PROP_TYPE(pTags->aulPropTag[DOCSUBJECT]))
 {
   pDocPropTags.aulPropTag[p++] = pTags->aulPropTag[DOCSUBJECT];
   ulOfficePropertiesBitmap |= OPBM_DOCSUBJECT;
 }

 pDocPropTags.cValues = p;


error_return:

 for (i=0; i<NAMEIDS; i++)
    if (apMAPINameId[i] != NULL)
          MAPIFreeBuffer(apMAPINameId[i]);
 
 if (pTags != NULL)
    MAPIFreeBuffer(pTags);


}


///////////////////////////////////////////////////////////////////////////////
//    ValidDocObject()
//
//    Parameters
//    pObject -- pointer to property object validate
//
//    Purpose
//    Inspects the properties of the object to see if the selected object
//    is a message type object and has the proper class.
//
//    Return Value
//    TRUE if the object is MAPI_MESSAGE and the class is IPM.Document
//    FALSE if either the object is not MAPI_MESSAGE or class is not
//    IPM.Document
//
//    Comments
//    This function is intended to be called from GetPages with an
//    object pointer from GetObject.
//
BOOL ValidDocObject(LPMAPIPROP pObject)
{
 HRESULT hr;
 ULONG ulcValues;
 LPSPropValue pObjectProps = NULL;
 BOOL bRet;

 bRet = FALSE;   // assume invalid object

 if (pObject == NULL)
 {
  ErrorMessageBox(NULL, 0, "ValidDocObject", "Null pObject");
  return FALSE;
 }

 hr = pObject->GetProps((LPSPropTagArray)&ObjPropTags, 0, &ulcValues, 
                        &pObjectProps);
 if ((FAILED(hr)) || (ulcValues == 0))
 {
    ErrorMessageBox(NULL, hr, "ValidDocObject", "pObject->GetProps failed");
    bRet = FALSE;
    goto error_return;
 }

 if ( (pObjectProps[OBJECT_TYPE].Value.l == MAPI_MESSAGE)  &&
      (strstr(pObjectProps[MESSAGE_CLASS].Value.lpszA, "IPM.Document")) )
    bRet = TRUE;
 else
    bRet = FALSE;


 // clean up
error_return:
 if (pObjectProps != NULL)
    MAPIFreeBuffer(pObjectProps);

 return bRet;


}

///////////////////////////////////////////////////////////////////////////////
//    GetFileTimeString()
//
//    Parameters
//    pft -- filetime structure pointer
//
//    Purpose
//    convert the given filetime date into a string.
//
//    Return Value
//    pointer to a string buffer containing formatted filed time.
//
//    Comments
//    The filetime is formated into dd/mm/yy hh:mm.  The string buffer
//    is static (not on the stack) because it is referred to outside this
//    function.
//

LPSTR GetFileTimeString(FILETIME * pft)
{
 static char szTimeStringBuff[25];
 FILETIME ftLocal;
 SYSTEMTIME systime;
 WORD wAdjustedHour;
 LPSTR pszAMPM[2] = { "AM", "PM" };
 int nAMPM;

 FileTimeToLocalFileTime(pft, &ftLocal);
 FileTimeToSystemTime(&ftLocal, &systime);
 
 szTimeStringBuff[0] = '\0';
 
// convert from Military time to Civilian time format 
 wAdjustedHour = systime.wHour;

 nAMPM = 1;  // assume PM time
 if ( wAdjustedHour < 12 )
    nAMPM = 0;   // AM time

 if (wAdjustedHour > 12)
 {
    wAdjustedHour-=12;
 }
 
 wsprintf(szTimeStringBuff, "%02d/%02d/%02d %02d:%02d%s", 
            systime.wMonth,
            systime.wDay,
            systime.wYear,
            wAdjustedHour,
            systime.wMinute,
            pszAMPM[nAMPM]);

 return szTimeStringBuff;

}



