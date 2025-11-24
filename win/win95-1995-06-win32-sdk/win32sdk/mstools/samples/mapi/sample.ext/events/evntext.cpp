///////////////////////////////////////////////////////////////////////////////
//
//  File Name
//      EVNTEXT.CPP 
//
//  Description
//  This is an implementation of a message event extension example.  The
//  extension calculates the checksum of a message that is submitted by
//  The Exchange client and then signs the message with the checksum result
//  to be used upon reading.  The sample includes a Tools Options property
//  page to enable or disable signing.
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
//      IExchExtMessageEvents interface methods:
//          IExchExtMessageEvents::QueryInterface()
//          IExchExtMessageEvents::AddRef()
//          IExchExtMessageEvents::Release()
//
//          IExchExtMessageEvents::OnCheckNames()
//          IExchExtMessageEvents::OnCheckNamesComplete()
//          IExchExtMessageEvents::OnRead()
//          IExchExtMessageEvents::OnReadComplete()
//          IExchExtMessageEvents::OnSubmit()
//          IExchExtMessageEvents::OnSubmitComplete()
//          IExchExtMessageEvents::OnWrite()
//          IExchExtMessageEvents::OnWriteComplete()
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
#define USES_IID_IMessage
#define USES_PS_MAPI

#include "EVNTEXT.H"

#include <INITGUID.H>
#include <MAPIGUID.H>


///////////////////////////////////////////////////////////////////////////////
// named id for custom named property, MsgChecksum
MAPINAMEID NamedID[1];
    
///////////////////////////////////////////////////////////////////////////////
//    global data that supports extension functionality
BOOL bSignatureOn = TRUE;


///////////////////////////////////////////////////////////////////////////////
//    global data for DLL
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

  m_pExchExtPropertySheets = new MyExchExtPropertySheets;
  m_pExchExtMessageEvents = new MyExchExtMessageEvents;

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
    else if (IID_IExchExtMessageEvents == riid)
    {
        *ppvObj = (LPUNKNOWN) m_pExchExtMessageEvents;
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
     case EECONTEXT_SENDNOTEMESSAGE:
     case EECONTEXT_SENDPOSTMESSAGE:
     case EECONTEXT_SENDRESENDMESSAGE:
     case EECONTEXT_READNOTEMESSAGE:
     case EECONTEXT_READPOSTMESSAGE:
     case EECONTEXT_READREPORTMESSAGE:
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
//    Exchange calls this to know how many PROPSHEETPAGE buffers it needs
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
     case EEPS_MESSAGE:
        ulNumExtSheets = 0;
        break;
         
     case EEPS_TOOLSOPTIONS:
        ulNumExtSheets = 1;   // adding one propery page
        break;

     default:
        ulNumExtSheets = 0;
        break;
    }

    return ulNumExtSheets;  
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
 LPMDB pMDB = NULL;
 LPMESSAGE pItem = NULL;

    *pcpsp = 0;


    // fill out members for the property page
    ppsp[0].dwSize = sizeof (PROPSHEETPAGE);
    ppsp[0].dwFlags = PSP_DEFAULT | PSP_HASHELP;
    ppsp[0].hInstance = ghInstDLL;
    ppsp[0].pszTemplate = MAKEINTRESOURCE(IDD_SIGNATURE);
    ppsp[0].hIcon = NULL;     // not used in this sample
    ppsp[0].pszTitle = NULL;  // not used in this sample
    ppsp[0].pfnDlgProc = (DLGPROC)SignatureOptionsDlgProc;
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
   // not used in this sample
}


///////////////////////////////////////////////////////////////////////////////
//  IExchExtMessageEvents virtual member functions implementation
//

///////////////////////////////////////////////////////////////////////////////
//    MyExchExtMessageEvents::QueryInterface()
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

STDMETHODIMP MyExchExtMessageEvents::QueryInterface(REFIID riid, LPVOID FAR * ppvObj)          
{   

    *ppvObj = NULL;
    if ((riid == IID_IExchExtMessageEvents) || (riid == IID_IUnknown))
    {
        *ppvObj = (LPVOID)this;
        // Increase usage count of this object
        AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;

}



///////////////////////////////////////////////////////////////////////////////
//    MyExchExtMessageEvents::OnRead()
//
//    Parameters
//    lpeecb -- pointer to IExchExtCallback interface
//
//    Purpose
//    To extend or inhibit Exchange when displaying the send or read note form.
//    
//    Return Value
//    S_OK signals Exchange to not continue calling extensions
//    S_FALSE signals Exchange to continue calling extensions
//    Other MAPI Code errors will abort the send or read note form.
//
//

HRESULT MyExchExtMessageEvents::OnRead(LPEXCHEXTCALLBACK lpeecb)
{
  HRESULT hr;
  LPMESSAGE pMessage;
  LPMDB pMDB;
  ULONG ulCheckSum;
  LPSPropTagArray pNamedPropTags;
  LPSPropValue pPropValues;
  ULONG ulcValues;
  HWND hWnd;
  HCURSOR hOldCursor;

  m_hrOnReadComplete = S_FALSE; // tell OnReadComplete to show its dialog

  pMessage = NULL;
  pMDB = NULL;
  pPropValues = NULL;
  pNamedPropTags = NULL;

  if (!bSignatureOn)      // user has turned off signatures
  {
    goto error_return;    // return and do nothing
  }


  hr = lpeecb->GetObject(&pMDB, (LPMAPIPROP *)&pMessage);
  if (FAILED(hr))
  {
    goto error_return;
  }


  lpeecb->GetWindow(&hWnd);
  hOldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));

  // check to see if MsgChecksum exists
  // use a named property for the message body checksum
  NamedID[0].lpguid = (LPGUID)&IID_IMessage;
  NamedID[0].ulKind = MNID_STRING;
  NamedID[0].Kind.lpwstrName = L"MsgChecksum";

  hr = pMessage->GetIDsFromNames(1, (LPMAPINAMEID *)&NamedID, 0, &pNamedPropTags);
  if (FAILED(hr))     // likely the message is not signed
  {
    goto error_return;
  }

  hr = pMessage->GetProps(pNamedPropTags, 0, &ulcValues, &pPropValues);
  if (FAILED(hr))    // if it fails for some other reason, bark.
  {
    goto error_return;
  }

  if (hr == MAPI_W_ERRORS_RETURNED)    // likely the message was not signed
  {
    goto error_return;
  }


  // we have a signed message, continue with checking validity through checksum
  hr = CalcULONGCheckSum(pMessage, &ulCheckSum);
  if (FAILED(hr))
  {
    ErrorMessageBox(hWnd, hr, "OnRead", 
        "An error occured while calculating\n"
        "the message body checksum.");
    goto error_return;
  }

  if (pPropValues[0].Value.ul == ulCheckSum)
  {
      MessageBox(hWnd, "Signed Message Verified.\n"
                       "Message has authentic contents.", "Event Extension", MB_OK);
  }
  else
  {
   int nRet = MessageBox(hWnd, "Signed Message was altered.\n"
                       "Message may not have authentic contents.\n"
                       "Do you wish to view the message anyway?", "Event Extension", MB_YESNO);
   
   if (nRet == IDNO)
   // this will tell OnReadComplete to not display the read note UI at all.
      m_hrOnReadComplete = MAPI_E_CALL_FAILED;
  }

  error_return:

  hr = S_FALSE;
  
  if (pMDB != NULL)
    pMDB->Release();

  if (pMessage != NULL)
    pMessage->Release();

  if (pNamedPropTags != NULL)
    MAPIFreeBuffer(pNamedPropTags);

  if (pPropValues != NULL)
    MAPIFreeBuffer(pPropValues);

  SetCursor(hOldCursor);

  return hr;
}

///////////////////////////////////////////////////////////////////////////////
//    MyExchExtMessageEvents::OnReadComplete()
//
//    Parameters
//    lpeecb -- pointer to IExchExtCallback interface
//
//    Purpose
//    To do processing after message has been read.
//    
//    Return Value
//    S_OK signals Exchange to not continue calling extensions
//    S_FALSE signals Exchange to continue calling extensions
//    Some MAPI Code error indicates a problem and will not display the send
//    or read note form.
//
//    Comments.
//    If an error code, such as MAPI_E_CALL_FAILED, is returned, Exchange will
//    call OnReadComplete again with the ulFlags parameter set to 
//    EEME_COMPLETE_FAILED.  Returning the error code again will cause Exchange
//    to not display the UI.
//

HRESULT MyExchExtMessageEvents::OnReadComplete(LPEXCHEXTCALLBACK lpeecb, ULONG ulFlags)
{

  return m_hrOnReadComplete;
}

///////////////////////////////////////////////////////////////////////////////
//    MyExchExtMessageEvents::OnWrite()
//
//    Parameters
//    lpeecb -- pointer to IExchExtCallback interface
//
//    Purpose
//    This method is called when a message is about to be written.  The message
//    only has default properties at this point.  It does not contain 
//    properties which the user has added by way of recipients, subject,
//    message text, or attachments.
//    This method is called when the user Sends or Saves a message
//    
//    Return Value
//    S_OK signals Exchange to not continue calling extensions
//    S_FALSE signals Exchange to continue calling extensions
//
//

HRESULT MyExchExtMessageEvents::OnWrite(LPEXCHEXTCALLBACK lpeecb)
{
  HRESULT hr;

  hr = S_FALSE; 

  return hr;
}

///////////////////////////////////////////////////////////////////////////////
//    MyExchExtMessageEvents::OnWriteComplete()
//
//    Parameters
//    lpeecb -- pointer to IExchExtCallback interface
//
//    Purpose
//    This method is called when the data (recipients, attachments, body, 
//    subject, etc.) has been written to the message.  This sample will
//    calculate the checksum on the PR_BODY property.
//    
//    Return Value
//    S_OK signals Exchange to not continue calling extensions
//    S_FALSE signals Exchange to continue calling extensions
//
//

HRESULT MyExchExtMessageEvents::OnWriteComplete(LPEXCHEXTCALLBACK lpeecb, ULONG ulFlags)
{
  HRESULT hr;
  LPMESSAGE pMessage;
  LPMDB pMDB;
  ULONG ulCheckSum;
  SPropValue pChksumProp[1];
  LPSPropTagArray pNamedPropTags;
  LPSPropProblemArray pPropProblems;
  HWND hWnd;
  HCURSOR hOldCursor;

  if (!bSignatureOn)      // user has turned off signatures
  {
    return S_FALSE;    // return and do nothing
  }

  if (!m_bInSubmitState)    // means user is just "saving the message"
  {
    return S_FALSE;
  }

  pMessage = NULL;
  pMDB = NULL;
  pPropProblems = NULL;
  pNamedPropTags = NULL;


  hOldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));


  hr = lpeecb->GetObject(&pMDB, (LPMAPIPROP *)&pMessage);
  if (FAILED(hr))
  {
    goto error_return;
  }

  hr = CalcULONGCheckSum(pMessage, &ulCheckSum);
  if (FAILED(hr))
  {
    lpeecb->GetWindow(&hWnd);

    ErrorMessageBox(hWnd, hr, "OnWriteComplete", 
        "An error occured while calculating\n"
        "the message body checksum.");
    goto error_return;
  }

  // use a named property for the message body checksum
  NamedID[0].lpguid = (LPGUID)&IID_IMessage;
  NamedID[0].ulKind = MNID_STRING;
  NamedID[0].Kind.lpwstrName = L"MsgChecksum";

  hr = pMessage->GetIDsFromNames(1, (LPMAPINAMEID *)&NamedID, MAPI_CREATE, &pNamedPropTags);
  if (FAILED(hr))
  {
   goto error_return;
  }

  pChksumProp[0].ulPropTag = PROP_TAG(PT_LONG, HIWORD(pNamedPropTags->aulPropTag[0]));
  pChksumProp[0].dwAlignPad = 0L;
  pChksumProp[0].Value.ul = ulCheckSum;

  hr = pMessage->SetProps(1, pChksumProp, &pPropProblems);
  if (FAILED(hr))
  {
   goto error_return;
  }

  // everything succeeded.  return S_FALSE to tell Exchange to continue calling
  // extensions.
  hr = S_FALSE; 

  error_return:

  
  if (pMDB != NULL)
    pMDB->Release();

  if (pMessage != NULL)
    pMessage->Release();

  if (pNamedPropTags != NULL)
    MAPIFreeBuffer(pNamedPropTags);

  if (pPropProblems != NULL)
    MAPIFreeBuffer(pPropProblems);

  SetCursor(hOldCursor);

  return hr;
}

///////////////////////////////////////////////////////////////////////////////
//    MyExchExtMessageEvents::OnCheckNames()
//
//    Parameters
//    lpeecb -- pointer to IExchExtCallback interface
//
//    Purpose
//    Called when user selects the Check Names button and just before message
//    is submitted to MAPI.
//    
//    Return Value
//    S_OK signals Exchange to not continue calling extensions
//    S_FALSE signals Exchange to continue calling extensions
//

HRESULT MyExchExtMessageEvents::OnCheckNames(LPEXCHEXTCALLBACK lpeecb)
{
  // Not used by the sample.
  return S_FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//    MyExchExtMessageEvents::OnCheckNamesComplete()
//
//    Parameters
//    lpeecb -- pointer to IExchExtCallback interface
//
//    Purpose
//    Called after exchange has completed resolving names in the message
//    recipients table.
//    
//    Return Value
//    S_OK signals Exchange to not continue calling extensions
//    S_FALSE signals Exchange to continue calling extensions
//

HRESULT MyExchExtMessageEvents::OnCheckNamesComplete(LPEXCHEXTCALLBACK lpeecb, ULONG ulFlags)
{
  // Not used by the sample
  return S_FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//    MyExchExtMessageEvents::OnSubmit()
//
//    Parameters
//    lpeecb -- pointer to IExchExtCallback interface
//
//    Purpose
//    Called before message data has been written and is
//    is submitted to MAPI.
//    
//    Return Value
//    S_OK signals Exchange to not continue calling extensions
//    S_FALSE signals Exchange to continue calling extensions
//
//    Set a member function to show that submit has been called
//    to indicate to OnWriteComplete that the user has hit the
//    Send button and is not just saving the message.
//

HRESULT MyExchExtMessageEvents::OnSubmit(LPEXCHEXTCALLBACK lpeecb)
{
  HRESULT hr;

  hr = S_FALSE;   

  m_bInSubmitState = TRUE;  // submit is called


  return hr;
}

///////////////////////////////////////////////////////////////////////////////
//    MyExchExtMessageEvents::OnSubmitComplete()
//
//    Parameters
//    lpeecb -- pointer to IExchExtCallback interface
//
//    Purpose
//    Called after message has been submitted to MAPI.
//    
//    Return Value - none
//
//    A flag is cleared to indicate to other methods that Exchange is
//    finished submitting the message.
//

VOID MyExchExtMessageEvents::OnSubmitComplete(LPEXCHEXTCALLBACK lpeecb, ULONG ulFlags)
{

  m_bInSubmitState = FALSE;  // out of submit state

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
//    CalcULONGCheckSum(pMessage, &ulCheckSum)()
//
//    Parameters
//    pMessage -- pointer to message object
//    *pulCheckSum -- pointer to buffer to contain calculated checksum
//
//    Purpose
//    Calculates a checksum on the PR_BODY property of a message
//
//    Return Value
//    HRESULT value:
//       S_OK -- succeeded
//       anything means some internal call failed
//
//    Comments
//    Checksum is calculated by adding the ascii values for 4 bytes of data
//    concatenated together at a time.  If the message does not have an even
//    multiple of 4 characters in the PR_BODY, the remain 1 2 or 3 characters
//    are concatenated to make a ULONG with leading zeros.
//

HRESULT CalcULONGCheckSum(LPMESSAGE pMessage, ULONG *pulCheckSum)
{
 HRESULT hr;
 LPSTREAM pStreamBody;
 ULONG ulValue;
 ULONG ulRead;
 LARGE_INTEGER large_int;
 ULARGE_INTEGER ularge_int;

 if ( (pMessage == NULL) ||
      (pulCheckSum == NULL) )
 {
      hr = MAPI_E_INVALID_PARAMETER;
      goto error_return;
 }

 pStreamBody = NULL;

 hr = pMessage->OpenProperty(PR_BODY, &IID_IStream, STGM_DIRECT | STGM_READ, 0, 
                    (LPUNKNOWN *) &pStreamBody);
 if (FAILED(hr))
 {
    goto error_return;
 }

 large_int.LowPart = 0;
 large_int.HighPart = 0;
 pStreamBody->Seek(large_int, STREAM_SEEK_SET, &ularge_int);  
 
 (*pulCheckSum) = 0;
 ulValue = 0;
 while ( (S_OK == (hr = pStreamBody->Read((LPVOID)&ulValue, 4, &ulRead)))  &&
         (ulRead > 0) )
 {
    (*pulCheckSum) += ulValue;
    ulValue = 0;
 }      


 error_return:

 if (pStreamBody != NULL)
    pStreamBody->Release();

 return hr;

}
