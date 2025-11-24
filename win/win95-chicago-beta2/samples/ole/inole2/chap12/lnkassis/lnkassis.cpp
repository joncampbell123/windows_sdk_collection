/*
 * LNKASSIS.CPP
 * Links Assistant Chapter 12
 *
 * Implementation of the CLinks object with the IOleUILinkContainer
 * interface to assist handling the Links dialog for linking
 * containers.
 *
 * Copyright (c)1993-1994 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#include "lnkassis.h"


/*
 * CLinks::CLinks
 * CLinks::~CLinks
 *
 * Parameters (Constructor):
 *  pfnDestroy      PFNDESTROYED to call when object is destroyed.
 */

CLinks::CLinks(PFNDESTROYED pfnDestroy)
    {
    m_cRef=0;
    m_pfnDestroy=pfnDestroy;
    return;
    }


CLinks::~CLinks(void)
    {
    return;
    }




/*
 * CLinks::QueryInterface
 * CLinks::AddRef
 * CLinks::Release
 *
 * Purpose:
 *  IUnknown members for CLinks object.
 */

STDMETHODIMP CLinks::QueryInterface(REFIID riid, PPVOID ppv)
    {
    *ppv=NULL;

    if (IID_IUnknown==riid || IID_IOleUILinkContainer==riid)
        {
        *ppv=this;
        AddRef();
        return NOERROR;
        }

    return ResultFromScode(E_NOINTERFACE);
    }


STDMETHODIMP_(ULONG) CLinks::AddRef(void)
    {
    return ++m_cRef;
    }


STDMETHODIMP_(ULONG) CLinks::Release(void)
    {
    ULONG       cRefT;

    cRefT=--m_cRef;

    if (0L==m_cRef)
        {
        if (NULL!=m_pfnDestroy)
            (*m_pfnDestroy)();

        delete this;
        }

    return cRefT;
    }





/*
 * CLinks::GetNextLink
 *
 * Purpose:
 *  Function to fill out the IOleUILinkContainer interface.
 *  Does nothing.
 *
 * Parameters:
 *  dwLink          DWORD ignored.
 *
 * Return Value:
 *  DWORD           Alwaus 0L
 *
 */

STDMETHODIMP_(DWORD) CLinks::GetNextLink(DWORD dwLink)
    {
    return 0L;
    }





/*
 * CLinks::SetLinkUpdateOptions
 *
 * Purpose:
 *  Calls IOleLink::SetUpdateOptions for the object identified by
 *  dwLink.
 *
 * Parameters:
 *  dwLink          DWORD, an IOleLink pointer to the object
 *                  affected.
 *  dwOptions       DWORD containing the new options.
 *
 * Return Value:
 *  HRESULT         Return value of IOleLink::SetUpdateOptions.
 */

STDMETHODIMP CLinks::SetLinkUpdateOptions(DWORD dwLink
    , DWORD dwOptions)
    {
    LPOLELINK       pIOleLink=(LPOLELINK)dwLink;

    if (NULL==pIOleLink)
        return ResultFromScode(E_FAIL);

    return pIOleLink->SetUpdateOptions(dwOptions);
    }





/*
 * CLinks::GetLinkUpdateOptions
 *
 * Purpose:
 *  Call IOleLink::GetUpdateOptions for the object identified by
 *  dwLink.
 *
 * Parameters:
 *  dwLink          DWORD, an IOleLink pointer to the object
 *                  affected.
 *  pdwOptions      LPDWORD in which to store the options.
 *
 * Return Value:
 *  HRESULT         Return value of IOleLink::GetUpdateOptions
 */

STDMETHODIMP CLinks::GetLinkUpdateOptions(DWORD dwLink
    , LPDWORD pdwOptions)
    {
    LPOLELINK       pIOleLink=(LPOLELINK)dwLink;

    if (NULL==pIOleLink)
        return ResultFromScode(E_FAIL);

    return pIOleLink->GetUpdateOptions(pdwOptions);
    }





/*
 * CLinks::SetLinkSource
 *
 * Purpose:
 *  Changes the moniker to which an object is linked.
 *
 * Parameters:
 *  dwLink          DWORD, an IOleLink pointer to the object
 *                  affected.
 *  pszName         LPTSTR to the displayable name of the source.
 *  cchName         ULONG length of the file portaion of pszName
 *  pchEaten        ULONG * in which to return the number of
 *                  characters used in parsing pszDisplayName.
 *  fValidate       BOOL indicating if we're to validate that the
 *                  source exists first.
 *
 * Return Value:
 *  HRESULT         If successful, NOERROR indicates that the link
 *                  is available, S_FALSE to indicate it's not.
 *                  This information is later required in
 *                  GetLinkSource.  E_FAIL on failure.
 */

STDMETHODIMP CLinks::SetLinkSource(DWORD dwLink, LPOLESTR pszName
    , ULONG cchName, ULONG *pchEaten, BOOL fValidate)
    {
    LPOLELINK       pIOleLink=(LPOLELINK)dwLink;
    HRESULT         hr;
    CLSID           clsID=CLSID_NULL;
    LPMONIKER       pmk=NULL;
    BOOL            fAvail=FALSE;

    if (fValidate)
        {
        //Check things out and get a moniker and CLSID.
        if (!FValidateLinkSource(pszName, pchEaten, &pmk, &clsID))
            return ResultFromScode(E_FAIL);

        //If we got a CLSID, then we found the source.
        if (CLSID_NULL!=clsID)
            fAvail=TRUE;
        }
    else
        {
        if (!FCreateNewSourceMoniker(pszName, cchName, &pmk))
            return ResultFromScode(E_FAIL);
        }

    if (NULL==pIOleLink)
        {
        pmk->Release();
        return ResultFromScode(E_FAIL);
        }

    if (NULL!=pmk)
        {
        hr=pIOleLink->SetSourceMoniker(pmk, clsID);
        pmk->Release();
        }
    else
    {
        hr=pIOleLink->SetSourceDisplayName(pszName);
    }

    if (FAILED(hr))
        return hr;

    return fAvail ? NOERROR : ResultFromScode(S_FALSE);
    }






/*
 * CLinks::GetLinkSource
 *
 * Purpose:
 *  Retrieves various strings and values for this link source.
 *
 * Parameters:
 *  dwLink          DWORD, an IOleLink pointer to the object
 *                  affected.
 *  ppszName        LPTSTR * in which to return the new source
 *                  name
 *  pcchName        ULONG * in which to return the length of
 *                  pszName
 *  ppszFullLink    LPTSTR * in which to return the full name of
 *                  the class of linked object.
 *  ppszShortLink   LPTSTR * in which to return the short name of
 *                  the class of linked object.
 *  pfSourceAvail   BOOL * ignored.
 *  pfSelected      BOOL * ignored.
 *
 * Return Value:
 *  HRESULT         NOERROR on success, error code otherwise.
 */

STDMETHODIMP CLinks::GetLinkSource(DWORD dwLink
    , LPOLESTR *ppszName, ULONG *pcchName
    , LPOLESTR *ppszFullLink, LPOLESTR *ppszShortLink
    , BOOL *pfSourceAvail, BOOL *pfSelected)
    {
    LPOLELINK       pIOleLink=(LPOLELINK)dwLink;
    HRESULT         hr;
    LPOLEOBJECT     pIOleObject=NULL;
    LPMONIKER       pmk=NULL;
    LPMONIKER       pmkFirst=NULL;
    LPBC            pbc=NULL;

    if (NULL==pIOleLink)
        return ResultFromScode(E_FAIL);

    *ppszName=NULL;
    *pcchName=0;
    *ppszFullLink=NULL;
    *ppszShortLink=NULL;

    hr=pIOleLink->GetSourceMoniker(&pmk);

    if (SUCCEEDED(hr))
        {
        hr=pIOleLink->QueryInterface(IID_IOleObject
            , (PPVOID)&pIOleObject);

        if (SUCCEEDED(hr))
            {
            pIOleObject->GetUserType(USERCLASSTYPE_FULL
                , ppszFullLink);
            pIOleObject->GetUserType(USERCLASSTYPE_SHORT
                , ppszShortLink);
            pIOleObject->Release();
            }

        *pcchName=OleStdGetLenFilePrefixOfMoniker(pmk);
        pmk->Release();
        }
    hr = pIOleLink->GetSourceDisplayName(ppszName);
    return hr;
    }





/*
 * CLinks::OpenLinkSource
 *
 * Purpose:
 *  Does nothing.  The container using this object is the only
 *  one that knows how to activate an object properly.
 *
 * Parameters:
 *  dwLink          DWORD ignored.
 *
 * Return Value:
 *  HRESULT         NOERROR
 */

STDMETHODIMP CLinks::OpenLinkSource(DWORD dwLink)
    {
    return NOERROR;
    }






/*
 * CLinks::UpdateLink
 *
 * Purpose:
 *  Updates the link for this object.
 *
 * Parameters:
 *  dwLink          DWORD, an IOleLink pointer to the object
 *                  affected.
 *  fErrorMessage   BOOL indicating if we can show errors.
 *  fErrorAction    BOOL making no sense whatsoever.
 *
 * Return Value:
 *  HRESULT         NOERROR if successful, error code otherwise. If
 *                  there is an error, the caller should set the
 *                  link availability flag to FALSE.  Otherwise set
 *                  to TRUE.
 */

STDMETHODIMP CLinks::UpdateLink(DWORD dwLink
    , BOOL fErrorMessage, BOOL fErrorAction)
    {
    LPOLELINK       pIOleLink=(LPOLELINK)dwLink;
    HRESULT         hr;
    LPOLEOBJECT     pIOleObject;

    if (NULL==pIOleLink)
        return ResultFromScode(E_FAIL);

    hr=pIOleLink->QueryInterface(IID_IOleObject
        , (PPVOID)&pIOleObject);

    if (FAILED(hr))
        return hr;

    hr=pIOleObject->IsUpToDate();

    if (NOERROR!=hr)
        {
        hr=pIOleObject->Update();

        if (FAILED(hr))
            return hr;
        }

    return NOERROR;
    }






/*
 * CLinks::CancelLink
 *
 * Purpose:
 *  Sets the source moniker in the link to NULL but does nothing
 *  else.  How the container decides to convert this to static
 *  is its choice.
 *
 * Parameters:
 *  dwLink          DWORD, an IOleLink pointer to the object
 *                  affected.
 *
 * Return Value:
 *  HRESULT         Standard.
 */

STDMETHODIMP CLinks::CancelLink(DWORD dwLink)
    {
    LPOLELINK       pIOleLink=(LPOLELINK)dwLink;

    if (NULL!=pIOleLink)
        return pIOleLink->SetSourceMoniker(NULL, CLSID_NULL);

    return NOERROR;
    }





//PROTECTED FUNCTIONS INTERNAL TO CLinks

/*
 * CLinks::FValidateLinkSource
 * (Protected)
 *
 * Purpose:
 *  Given a name of a link source retrieve a moniker for it and
 *  a CLSID if we can bind.
 *
 * Parameters:
 *  pszName         LPTSTR of the source
 *  pchEaten        ULONG * into which to return how many
 *                  characters we parse.
 *  ppmk            LPMONIKER * into which to store the moniker
 *  pclsID          LPCLSID into which to store the clsID.
 *
 * Return Value:
 *  BOOL            TRUE if *ppmk has a valid moniker,
 *                  FALSE otherwise.
 */

BOOL CLinks::FValidateLinkSource(LPOLESTR pszName
    , ULONG *pchEaten, LPMONIKER *ppmk, LPCLSID pclsID)
    {
    HRESULT     hr;
    LPBC        pbc=NULL;
    LPOLEOBJECT pIOleObject;

    *ppmk=NULL;
    *pclsID=CLSID_NULL;

    if (FAILED(CreateBindCtx(0, &pbc)))
        return FALSE;

    hr=MkParseDisplayName(pbc, pszName, pchEaten, ppmk);

    if (SUCCEEDED(hr))
        {
        /*
         * Now that we have a moniker for this new source, so try
         * binding to that source and get its CLSID.
         */
        hr=(*ppmk)->BindToObject(pbc, NULL, IID_IOleObject
            , (PPVOID)&pIOleObject);

        if (SUCCEEDED(hr))
            {
            pIOleObject->GetUserClassID(pclsID);
            pIOleObject->Release();
            }

        return TRUE;
        }

    pbc->Release();
    return FALSE;
    }




/*
 * CLinks::FCreateNewSourceMoniker
 * (Protected)
 *
 * Purpose:
 *  Given a name of a link source create a moniker for it.
 *
 * Parameters:
 *  pszName         LPTSTR of the source
 *  cchName         ULONG length of the filename in pszName.
 *  ppmk            LPMONIKER * into which to store the moniker
 *
 * Return Value:
 *  BOOL            TRUE if *ppmk has a valid moniker,
 *                  FALSE otherwise.
 */

BOOL CLinks::FCreateNewSourceMoniker(LPOLESTR pszName
    , ULONG cchName, LPMONIKER *ppmk)
    {
    OLECHAR     szName[OLEUI_CCHPATHMAX];
    LPMONIKER   pmkFile=NULL;
    LPMONIKER   pmkItem=NULL;

    *ppmk=NULL;
    wcsncpy(szName, pszName, (int)cchName+1);

    CreateFileMoniker(szName, &pmkFile);

    if (NULL==pmkFile)
        return FALSE;

    if (wcslen(pszName) > (int)cchName)
        {
        wcscpy(szName, pszName+cchName+1);
        CreateItemMoniker(OLESTDDELIM, szName, &pmkItem);

        if (NULL!=pmkItem)
            {
            CreateGenericComposite(pmkFile, pmkItem, ppmk);
            pmkItem->Release();
            }

        pmkFile->Release();

        if (NULL==*ppmk)
            return FALSE;
        }
    else
        *ppmk=pmkFile;

    return TRUE;
    }
